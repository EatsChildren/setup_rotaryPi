#include "rotary_phone.h"

Rotary_Phone::Rotary_Phone()
{

    _hook = 1;
    _flag = 0;
    _running = false;
    _ms_delay = 10;
    _bcm_counter_delay = 10;
    this->initializeGPIO();
}

Rotary_Phone::~Rotary_Phone()
{
    this->closeRX();
}

void Rotary_Phone::initializeGPIO()
{
    try
    {
        if (!bcm2835_init())
        {
            throw "bcm2835 not initialized";
        }
    }
    catch (const char *msg)
    {
        std::cout << "Exception Caught: " << msg;
    }
    // initializes the bcm2835 library
    bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);       // sets LET to output
    bcm2835_gpio_fsel(OUT, BCM2835_GPIO_FSEL_OUTP);       // sets OUT to output
    bcm2835_gpio_fsel(HOOK, BCM2835_GPIO_FSEL_INPT);      // sets HOOK to input
    bcm2835_gpio_fsel(FLAG, BCM2835_GPIO_FSEL_INPT);      // sets HOOK to input
    bcm2835_gpio_set_pud(HOOK, BCM2835_GPIO_PUD_UP);      // Sets the Pull-up mode for the pin.
    bcm2835_gpio_set_pud(FLAG, BCM2835_GPIO_PUD_DOWN);    // Sets the Pull-up mode for the pin.
    bcm2835_gpio_fsel(COUNTER, BCM2835_GPIO_FSEL_INPT);   // sets HOOK to input
    bcm2835_gpio_set_pud(COUNTER, BCM2835_GPIO_PUD_DOWN); // Sets the Pull-up mode for the pin.
    bcm2835_gpio_set(OUT);                                // turn on the power
}

void Rotary_Phone::closeRX()
{
    _running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if (_rx_thread.joinable())
    {
        _rx_thread.join();
    }
}

void Rotary_Phone::startRX()
{
    _running = true;
    _rx_thread = std::thread(&Rotary_Phone::rx_loop, this);
}

uint8_t Rotary_Phone::getHook()
{
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    return _hook;
}

uint8_t Rotary_Phone::getDialingFlag()
{
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    return _flag;
}

bool Rotary_Phone::getDigits(std::vector<uint8_t> &nums)
{
    bool ret = false;
    int num_count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (num_count < 3)
    {
        auto end = std::chrono::high_resolution_clock::now();
        if (!_flag)
        {
            nums.push_back(this->countPulses());
            num_count++;
            start = std::chrono::high_resolution_clock::now();
        }
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (duration.count() > 10000)
        {
            break;
        }
    }

    if (num_count == 3)
    {
        ret = true;
    }
    return ret;
}

std::string Rotary_Phone::getPhoneNumber(std::vector<uint8_t> counts)
{
    try
    {
        if (counts.size() != 3)
        {
            throw -1;
        }
    }
    catch (int e)
    {
        std::cout << "Exception Caught: Size of phone number must be 3 " << e;
    }

    std::string s = std::to_string(static_cast<int>(counts[0])) + std::to_string(static_cast<int>(counts[1])) + std::to_string(static_cast<int>(counts[2]));
    return s;
}

uint8_t Rotary_Phone::countPulses()
{
    uint8_t count = 0;
    uint8_t last = bcm2835_gpio_lev(COUNTER);
    uint8_t z0 = bcm2835_gpio_lev(COUNTER);
    uint8_t exitCond = 0;
    uint8_t exitCond2 = 0;
    struct timeval start, stop;
    double secs;

    while (1)
    {

        last = z0;
        z0 = bcm2835_gpio_lev(COUNTER);
        if (z0 == LOW && last == HIGH)
        {
            count++;
            exitCond = 1;
        }

        if (last == HIGH && z0 == HIGH && exitCond) // signal is staying high
        {
            gettimeofday(&start, NULL);
            exitCond = 0;
            exitCond2 = 1;
        }

        gettimeofday(&stop, NULL);
        if (!exitCond && exitCond2)
        {
            secs = (double)(stop.tv_usec - start.tv_usec) * 1e-6 + (double)(stop.tv_sec - start.tv_sec);
            if (secs > 0.3)
            {
                // if the signal is high for more than 300ms then it is done counting and can exit the function
                break;
            }
        }
        bcm2835_delay(_bcm_counter_delay); // should delay for 10 ms
    }
    if (count > 9)
    {
        count = 0;
    }
    return count;
}

// ######################################################
// ######################## RX LOOP #####################
// ######################################################

void Rotary_Phone::rx_loop()
{

    while (_running)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(_mutex);
            _hook = bcm2835_gpio_lev(HOOK);
            _flag = bcm2835_gpio_lev(FLAG);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
    }
}
