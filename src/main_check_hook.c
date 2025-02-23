#include <bcm2835.h>
#include <stdio.h>

#define HOT RPI_GPIO_P1_11
#define HOOK    RPI_GPIO_P1_07  // hook -> pin 7


int main(int argc, char *argv[])
{
    // int LED = 11;
    if (!bcm2835_init())
        return 1;
    bcm2835_gpio_fsel(HOT, BCM2835_GPIO_FSEL_OUTP); // sets LED to output
    bcm2835_gpio_fsel( HOOK, BCM2835_GPIO_FSEL_INPT );       // sets HOOK to input
    bcm2835_gpio_set_pud( HOOK, BCM2835_GPIO_PUD_DOWN );     // Sets the Pull-up mode for the pin.


    bcm2835_gpio_set(HOT);  //turn the header on!!
    while (1)
    {
        uint8_t state = bcm2835_gpio_lev( HOOK );
        //~ printf("state of flag is %d\n", state);
        if (!state)
        {
            printf("your phone is off the hook \n");
        }
        printf("...\n");
  
    }

    // std::cout << "Template Project Example mother fucker!!!!!!!!!!!!!!!!!!!!!! " << std::endl;

    return 0;
}
