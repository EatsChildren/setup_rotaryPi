#ifndef ROTARY_PHONE_H
#define ROTARY_PHONE_H

#include <iostream>
#include <memory>
#include <sys/time.h>
#include <sys/wait.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <cstdint>
#include <string>
#include <mutex>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "bcm2835.h"

#define LED RPI_GPIO_P1_12
#define OUT RPI_GPIO_P1_11     // 3.3 volts out (to hot header)
#define HOOK RPI_GPIO_P1_07    // hook -> pin 7
#define FLAG RPI_GPIO_P1_15    // flag (to tell if someone is about to dial) -> pin 15
#define COUNTER RPI_GPIO_P1_16 // counts the pulses -> pin 16
// pin 6 or 9 -> ground (to ground header )

class Rotary_Phone
{

public:
    typedef std::shared_ptr<Rotary_Phone> Ptr;
    static Ptr create() { return std::make_shared<Rotary_Phone>(); }
    Rotary_Phone();
    ~Rotary_Phone();
    void startRX();
    void closeRX();
    uint8_t getHook();
    uint8_t getDialingFlag();
    uint8_t countPulses();
    bool getDigits(std::vector<uint8_t>& nums);
    std::string getPhoneNumber(std::vector<uint8_t> counts);
    void initializeGPIO();

private:
    std::atomic<bool> _running;
    uint8_t _hook;
    uint8_t _flag;
    std::thread _rx_thread;
    std::recursive_mutex _mutex;
    int64_t _ms_delay;
    unsigned int _bcm_counter_delay;

    void rx_loop();
};

#endif