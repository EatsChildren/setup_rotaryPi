#include "rotary_phone.h"
#include "mp3_player.h"
#include <iostream>
#include <chrono>
#include <vector>


int main(){
    Rotary_Phone::Ptr phone = Rotary_Phone::create();
    MP3_Player::Ptr player = MP3_Player::create("/home/rotaryphone3/Desktop/songs/");
    phone->startRX();
    uint8_t last_hook = phone->getHook();
    
    while(true){
        uint8_t hook = phone->getHook();
        uint8_t flag = phone->getDialingFlag();

        if(!hook){ //phone is off the hook
            // std::cout<<"phone is off the hook"<<std::endl;
            if(last_hook){ //phone has just been taken off the hook
                player->playDialTone();
            }

            if(flag){  //dialing has begun
                player->stopCurrentSong();
                 //std::cout<<"not flag!!!"<<std::endl;
                std::vector<uint8_t> digits;
                bool pulse_ok = phone->getDigits(digits);
                if(pulse_ok){
                    std::string phone_number = phone->getPhoneNumber(digits);
                    std::cout<<"phone number: "<<phone_number<<std::endl;
                    player->playSong(phone_number);
                }else{
                    player->playErrorMessage();
                }
            }
        }else{
            player->stopCurrentSong();
        }
        last_hook = hook;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
