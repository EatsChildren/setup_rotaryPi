#include <sys/time.h>
#include <sys/wait.h>

#include <bcm2835.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LED RPI_GPIO_P1_12
#define OUT RPI_GPIO_P1_11     // 3.3 volts out (to hot header)
#define HOOK RPI_GPIO_P1_07    // hook -> pin 7
#define FLAG RPI_GPIO_P1_15    // flag (to tell if someone is about to dial) -> pin 15
#define COUNTER RPI_GPIO_P1_16 // counts the pulses -> pin 16
// pin 6 or 9 -> ground (to ground header )

// ###########################################
// ############### Functions #################
// ###########################################

uint8_t CountPulses()
{
    unsigned int delay = 10;
    uint8_t count = 0;
    uint8_t last2 = bcm2835_gpio_lev(COUNTER);
    uint8_t last = bcm2835_gpio_lev(COUNTER);
    uint8_t z0 = bcm2835_gpio_lev(COUNTER);
    uint8_t exitCond = 0;
    uint8_t exitCond2 = 0;
    struct timeval start, stop;
    double secs;

    while (1)
    {

        last2 = last;
        last = z0;
        z0 = bcm2835_gpio_lev(COUNTER);
        if (z0 == LOW && last == HIGH)
        {
            count++;
            exitCond = 1;
        }

        if (last == HIGH && z0 == HIGH && exitCond)
        {
            gettimeofday(&start, NULL);
            exitCond = 0;
            exitCond2 = 1;
        }

        gettimeofday(&stop, NULL);
        if (!exitCond && exitCond2)
        {
            secs = (double)(stop.tv_usec - start.tv_usec) * 1e-6 + (double)(stop.tv_sec - start.tv_sec);
            // printf("Secs %f\n", secs);
            if (secs > 0.3)
            {
                break;
            }
        }
        bcm2835_delay(delay); // should delay for 1000 ms
    }
    if (count > 9)
    {
        count = 0;
    }
    return count;
}

void playDialTone()
{
    char *argumentVector0[] = {"mpg123", "/home/eatschildren/Desktop/songs/toneANDerrMessage.mp3", NULL};
    int status0 = execvp(argumentVector0[0], argumentVector0);
}


int main(int argc, char **argv)
{
    if (!bcm2835_init())
        return 1;                                         // initializes the bcm2835 library
    bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);       // sets LET to output
    bcm2835_gpio_fsel(OUT, BCM2835_GPIO_FSEL_OUTP);       // sets OUT to output
    bcm2835_gpio_fsel(HOOK, BCM2835_GPIO_FSEL_INPT);      // sets HOOK to input
    bcm2835_gpio_fsel(FLAG, BCM2835_GPIO_FSEL_INPT);      // sets HOOK to input
    bcm2835_gpio_set_pud(HOOK, BCM2835_GPIO_PUD_UP);      // Sets the Pull-up mode for the pin.
    bcm2835_gpio_set_pud(FLAG, BCM2835_GPIO_PUD_DOWN);    // Sets the Pull-up mode for the pin.
    bcm2835_gpio_fsel(COUNTER, BCM2835_GPIO_FSEL_INPT);   // sets HOOK to input
    bcm2835_gpio_set_pud(COUNTER, BCM2835_GPIO_PUD_DOWN); // Sets the Pull-up mode for the pin.
    bcm2835_gpio_set(OUT);  //turn on the power 
    char num[4];
    uint8_t count = 0;
    pid_t child;
    pid_t child2;

    int wstatus;
    int conditionFlag = 0;
    uint8_t flag;
    uint8_t hookFlag;
    uint8_t last_hook = 1;
    uint8_t state;

    while (1)
    {
        // main loop to check if the phone is off the hook
        
        state = bcm2835_gpio_lev(HOOK);
        if (!last_hook && state)
        {
            // phone has been taken off the hook!!!!!!!!!!
            child = fork();
            if (child < 0)
            {
                perror("Fork fail");
                exit(1);
            }
            else if (child == 0)
            {
                playDialTone();
                bcm2835_delay(1000); // should delay for 1000 ms
                exit(1);             // KEEP THIS!!!!!!!!!!
            }
            else
            {
                while (1)
                {
                    flag = bcm2835_gpio_lev(FLAG);
                    pid_t childDead = waitpid(child, &wstatus, WNOHANG);
                    hookFlag = bcm2835_gpio_lev(HOOK);

                    if (!hookFlag)
                    {
                        // if the phone is hung up kill the process and exit the program
                        kill(child, SIGKILL);
                        break; // break out of inner while loop, remain in main while(1) loop
                    }

                    if (!flag)
                    {
                        conditionFlag = 1; // dialing began
                        kill(child, SIGKILL);
                        break;
                    }
                }
            }

            // kill( child, SIGKILL );  // kills the child process

            if (conditionFlag == 1)
            {
                child2 = fork();
                if (child2 < 0)
                {
                    perror("fork fail");
                    exit(1);
                }
                else if (child2 == 0)
                {
                    while (1)
                    {
                        flag = bcm2835_gpio_lev(FLAG);
                        if (!flag)
                        {
                            uint8_t pulses = CountPulses();
                            char pulseChar = pulses + '0';
                            num[count] = pulseChar;
                            count++;
                            if (count == 3)
                            {
                                break;
                            }
                        }
                    }
                    printf("num: %s \n", num);
                    
                    if (num[0] == '0' && num[1] == '0' && num[2] == '0')
                    {
                         char *argumentVector[] = {"mpg123", "-@", "/home/eatschildren/Desktop/playlist.txt", "-z", NULL};
                         int status = execvp(argumentVector[0], argumentVector);
                    }
                    else
                    {
                        char fullAddress[] = "/home/eatschildren/Desktop/songs/";
                        char ending[] = ".mp3";
                        num[3] = '\0';
                        char *argumentVector[] = {"mpg123", "-vZC", strcat(strcat(fullAddress, num), ending), NULL};
                        int status = execvp(argumentVector[0], argumentVector);
                    }
                }
                else
                {
                    while (1)
                    {
                        hookFlag = bcm2835_gpio_lev(HOOK);
                        if (!hookFlag)
                        {
                            state = 0;
                            conditionFlag = 0;
                            kill(child2, SIGKILL);
                            break;
                        }
                    }
                }
            }
        }
        last_hook = state;

    }
    return 0;
}
