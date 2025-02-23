#include <bcm2835.h>
#include <stdio.h>

#define LED RPI_GPIO_P1_11

int main(int argc, char *argv[])
{
    // int LED = 11;
    if (!bcm2835_init())
        return 1;
    bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP); // sets LED to output
    unsigned int delay = 1000;
    uint8_t count = 0;
    while (count < 10)
    {
        bcm2835_gpio_set(LED);
        bcm2835_delay(delay);
        bcm2835_gpio_clr(LED);
        bcm2835_delay(delay);
        printf("loop number %d\n", count);
        count++;
    }

    // std::cout << "Template Project Example mother fucker!!!!!!!!!!!!!!!!!!!!!! " << std::endl;

    return 0;
}
