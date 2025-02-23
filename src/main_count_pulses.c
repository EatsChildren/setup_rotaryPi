#include <bcm2835.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>

#define HOT RPI_GPIO_P1_11
#define COUNTER RPI_GPIO_P1_16  // counts the pulses -> pin 16
#define FLAG    RPI_GPIO_P1_15  // flag (to tell if someone is about to dial) -> pin 15

uint8_t CountPulses()
{
    printf("in the function\n");
    unsigned int delay = 10;
    uint8_t count      = 0;
    uint8_t last2      = bcm2835_gpio_lev( COUNTER );
    uint8_t last       = bcm2835_gpio_lev( COUNTER );
    uint8_t z0         = bcm2835_gpio_lev( COUNTER );
    uint8_t exitCond   = 0;
    uint8_t exitCond2  = 0;
    struct timeval start, stop;
    double secs;

    while( 1 ) {

        last2 = last;
        last  = z0;
        z0    = bcm2835_gpio_lev( COUNTER );
        if( z0 == LOW && last == HIGH ) {
            count++;
            exitCond = 1;
        }

        if( last == HIGH && z0 == HIGH && exitCond ) {
            gettimeofday( &start, NULL );
            exitCond  = 0;
            exitCond2 = 1;
        }

        gettimeofday( &stop, NULL );
        if( !exitCond && exitCond2 ) {
            secs = (double)( stop.tv_usec - start.tv_usec ) * 1e-6 + (double)( stop.tv_sec - start.tv_sec );
            // printf("Secs %f\n", secs);
            if( secs > 0.3 ) {
                break;
            }
        }
        bcm2835_delay( delay );  // should delay for 1000 ms
    }
    if( count > 9 ) {
        count = 0;
    }
    return count;
}

int main(int argc, char *argv[])
{
    if (!bcm2835_init())
        return 1;
    bcm2835_gpio_fsel(HOT, BCM2835_GPIO_FSEL_OUTP); // sets HOT to output
    bcm2835_gpio_fsel( COUNTER, BCM2835_GPIO_FSEL_INPT );    // sets COUNTER as input 
    bcm2835_gpio_set_pud( COUNTER, BCM2835_GPIO_PUD_DOWN );  // Sets the Pull-up mode for the pin.
    
    bcm2835_gpio_fsel( FLAG, BCM2835_GPIO_FSEL_INPT );       // sets HOOK to input
    bcm2835_gpio_set_pud( FLAG, BCM2835_GPIO_PUD_DOWN );     // Sets the Pull-up mode for the pin.
    
    uint8_t count = 0;
    unsigned int delay = 100;
    
    bcm2835_gpio_set(HOT);  //turn the header on!!
    while (count < 1000)
    {
        uint8_t state = bcm2835_gpio_lev( FLAG );
        //~ printf("state of flag is %d\n", state);
        if (!state)
        {
            uint8_t num = CountPulses();
            printf("number of pulses: %d\n", num);
        }
        bcm2835_delay(delay);
        count++;
    }
    bcm2835_gpio_clr(HOT);
    return 0;
}
