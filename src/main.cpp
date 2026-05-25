#include "rotary_phone.h"
#include "mp3_player.h"
#include <iostream>
#include <chrono>
#include <vector>


int main(){
    Rotary_Phone::Ptr phone = Rotary_Phone::create();
    MP3_Player::Ptr player = MP3_Player::create("/home/rotaryphone3/Desktop/songs/");
    uint8_t last_hook = phone->getHook();
    
    while(true){
        uint8_t hook = phone->getHook();
        uint8_t flag = phone->getDialingFlag();

        if(hook){ //phone is off the hook
            if(!last_hook){ //phone has just been taken off the hook
                player->playDialTone();
            }

            if(!flag){  //dialing has begun
                std::vector<uint8_t> digits;
                bool pulse_ok = phone->getDigits(digits);
                if(pulse_ok){
                    std::string phone_number = phone->getPhoneNumber(digits);
                    player->playSong(phone_number);
                }else{
                    player->playErrorMessage();
                }
            }
        }else{
            player->stopCurrentSong();
        }
        last_hook = hook;
    }
    return 0;
}