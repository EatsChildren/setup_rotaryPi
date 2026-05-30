#include <mp3_player.h>

MP3_Player::MP3_Player(const std::string &path) : _path(path)
{

    setenv("ALSA_PCM_CARD", "2", 1);
    setenv("ALSA_PCM_DEVICE", "0", 1);
    this->initializePlayer();
    _running.store(false);
    _ms_delay = 10;
}

MP3_Player::~MP3_Player()
{
    this->safeShutdown();
     ma_engine_uninit(&_engine);
    ma_context_uninit(&_context);
}

void MP3_Player::initializePlayer()
{
    
    // 1. Force ALSA Backend (bypassing higher-level OS mixers)
    ma_backend backends[] = { ma_backend_alsa };
    ma_context_config contextConfig = ma_context_config_init();
    
   
    ma_result result = ma_context_init(backends, 1, &contextConfig, &_context);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize ALSA context.\n");
        exit(EXIT_FAILURE);;
    }

    // 2. Configure the Engine with a larger buffer to stop Pi hardware pops
    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.pContext = &_context;
    engineConfig.periodSizeInMilliseconds = 50; // 100ms buffer 

    try
    {
        if (ma_engine_init(&engineConfig, &_engine) != MA_SUCCESS)
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
    _running.store(false);
    if (_song_loop.joinable())
    {
        _song_loop.join();
    }
}

void MP3_Player::playLoop(std::string phone_number)
{
    std::atomic<bool> _loop_exit = true;
    while(_loop_exit.load()){
        std::string full_path = _path + phone_number;
        ma_sound sound;
        ma_result result = ma_sound_init_from_file(&_engine, full_path.c_str(), 0, NULL, NULL, &sound);
        if (result != MA_SUCCESS) {
            return;
        }
        ma_sound_start(&sound);
        
        while (_running.load())
        {
            
            if (ma_sound_at_end(&sound)) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
        }
            if(!_running.load()){
            _loop_exit.store(false);
        }else{
            phone_number = "AT&T We're Sorry Message.mp3";
        }
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }

}

void MP3_Player::playRandom()
{
   
    std::atomic<bool> _loop_exit = true;
    while(_loop_exit.load()){
         ma_sound sound;
        std::string full_path = _path + randomPhoneNumber();
        
        ma_result result = ma_sound_init_from_file(&_engine, full_path.c_str(), 0, NULL, NULL, &sound);
        if (result != MA_SUCCESS) {
            return;
        }
        ma_sound_start(&sound);
        
        while (_running.load())
        {
            if (ma_sound_at_end(&sound)) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
        }
        if(!_running.load()){
            _loop_exit.store(false);
        }
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }
    
}    

std::string MP3_Player::randomPhoneNumber()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution<int> distr(0, 9);
    std::vector<int> counts;
    // 4. Generate random integers
    for (int n = 0; n < 3; ++n) {
        counts.push_back(distr(gen));
    }
     std::string rn = std::to_string((counts[0])) + std::to_string((counts[1])) + std::to_string((counts[2])) + ".mp3";
    return rn;
}

void MP3_Player::playSong(std::string &phone_number)
{
   
    this->stopCurrentSong();
    // std::this_thread::sleep_for(std::chrono::milliseconds(_ms_delay));
    if (_song_loop.joinable())
    {
        _song_loop.join();
    }

    _running.store(true);
    if(phone_number != "000.mp3"){
        _song_loop = std::thread(&MP3_Player::playLoop, this, phone_number);
    }else{
         _song_loop = std::thread(&MP3_Player::playRandom, this);
    }
    
}

void MP3_Player::stopCurrentSong()
{
    std::unique_lock<std::recursive_mutex> lock(_mutex);
   _running.store(false);
   
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
