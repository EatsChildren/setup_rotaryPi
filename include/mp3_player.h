#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include <mutex>
#include <stdlib.h> // Required for setenv()
#include <random>
#include "miniaudio.h"
#include <vector>


class MP3_Player
{

public:
    typedef std::shared_ptr<MP3_Player> Ptr;
    static Ptr create(const std::string &path) { return std::make_shared<MP3_Player>(path); }
    MP3_Player(const std::string &path);
    ~MP3_Player();
    void playSong(std::string& phone_number);
    void stopCurrentSong();
    void playDialTone();
    void playErrorMessage();
    void initializePlayer();
    void safeShutdown();

private:
    std::recursive_mutex _mutex;
    std::atomic<bool> _running;
    std::string _path; // file path to folder where songs are stored
    ma_engine _engine;
    ma_context _context;
    //ma_sound _sound;
    std::thread _song_loop;
    int _ms_delay;

    void playLoop(std::string phone_number);
    void playRandom();
    std::string randomPhoneNumber();
    
};

#endif
