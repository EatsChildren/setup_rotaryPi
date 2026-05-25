#include <mp3_player.h>

MP3_Player::MP3_Player(const std::string &path) : _path(path)
{
    this->initializePlayer();
    _running = false;
    _ms_delay = 5;
}

MP3_Player::~MP3_Player()
{
    this->safeShutdown();
}

void MP3_Player::initializePlayer()
{
    try
    {
        if (ma_engine_init(NULL, &_engine) != MA_SUCCESS)
        {
            throw "could not initialize miniaudio engine";
        }
    }
    catch (const char *msg)
    {
        std::cout << "Exception Caught: " << msg;
    }
}

void MP3_Player::safeShutdown()
{
    _running = false;
    if (_song_loop.joinable())
    {
        _song_loop.join();
    }
}

void MP3_Player::playLoop(const std::string &phone_number)
{
    std::string full_path = _path + phone_number;
    ma_sound_init_from_file(&_engine, full_path.c_str(), 0, NULL, NULL, &_sound);
    ma_sound_start(&_sound);

    while (_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
    }
    ma_sound_stop(&_sound);
}

void MP3_Player::playSong(const std::string &phone_number)
{
    if (_running)
    {
        this->stopCurrentSong();
        std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
        if (_song_loop.joinable())
        {
            _song_loop.join();
        }
    }
    std::unique_lock<std::recursive_mutex>(_mutex);
    _running = true;
    _song_loop = std::thread(&MP3_Player::playLoop, this, phone_number);
}

void MP3_Player::stopCurrentSong()
{
    std::unique_lock<std::recursive_mutex>(_mutex);
    _running = false;
}

void MP3_Player::playDialTone()
{
    std::string number = "toneANDerrMessage.mp3";
    this->playSong(number);
}

void MP3_Player::playErrorMessage()
{
    std::string number = "AT&T We're Sorry Message.mp3";
    this->playSong(number);
}