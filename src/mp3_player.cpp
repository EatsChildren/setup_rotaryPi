#include <mp3_player.h>

MP3_Player::MP3_Player(const std::string &path) : _path(path)
{
    this->initializePlayer();
    _running = false;
    _ms_delay = 50;
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
    std::cout<<"in the function:"<<std::endl;
    ma_sound sound;
    std::string full_path = _path + phone_number;
    // std::cout<<"full path: "<<full_path<<std::endl;
    
    if (ma_sound_init_from_file(
            &_engine,
            full_path.c_str(),
            0,
            NULL,
            NULL,
            &sound) != MA_SUCCESS)
    {
        std::cout << "Failed to load sound\n";
        return;
    }
    ma_sound_start(&sound);
    // std::cout<<"running: "<<_running<<std::endl;

    while (_running)
    {
        // std::cout<<"in the loop: "<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
    }
    ma_sound_stop(&sound);
    ma_sound_uninit(&sound);
}

void MP3_Player::playSong(const std::string &phone_number)
{
   
    this->stopCurrentSong();
    // std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
    if (_song_loop.joinable())
    {
        _song_loop.join();
    }
    

    _running = true;
    _song_loop = std::thread(&MP3_Player::playLoop, this, phone_number);
}

void MP3_Player::stopCurrentSong()
{
    std::unique_lock<std::recursive_mutex> lock(_mutex);
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