#include <bcm2835.h>
#include <stdio.h>

#define HOT RPI_GPIO_P1_11

int main(int argc, char *argv[])
{
    if (!bcm2835_init())
        return 1;
    bcm2835_gpio_fsel(HOT, BCM2835_GPIO_FSEL_OUTP); // sets HOT to output

    bcm2835_gpio_clr(HOT);
    printf("Pins cleared\n");

    return 0;
}
