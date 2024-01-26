#ifndef AUDIO_H
#define AUDIO_H


struct AudioHandle 
{
    void* sound;
};

bool audio_Setup();
AudioHandle audio_Load(const char* soundName);
void audio_Play(AudioHandle* instance);
void audio_Stop(AudioHandle* instance);
void audio_UnloadAll();


#endif //AUDIO_H
