#include "include/audio.h"

#include "include/arena.h"

#ifdef __APPLE__
    #define MA_NO_RUNTIME_LINKING
#endif
#define MINIAUDIO_IMPLEMENTATION
#include "include/miniaudio.h"


ma_engine* engine;
Arena arena;

bool audio_Setup() {
    init_arena(&arena, &pool);
    engine = (ma_engine*) push_size(&arena, sizeof(*engine));

    ma_result result = ma_engine_init(NULL, engine);

    if (result != MA_SUCCESS) {
        return false;  // Failed to initialize the engine.
    }

    return true;
}

AudioHandle audio_Load(const char* soundName) 
{
    AudioHandle handle;
    handle.sound = push_size(&arena, sizeof(ma_sound));

    ma_result result = ma_sound_init_from_file(engine, soundName, 0, NULL, NULL, (ma_sound*) handle.sound);
    if (result != MA_SUCCESS) {
        printf("Failed to load sound: %s\n", soundName);
        handle.sound = NULL;
        return handle;
    }

    return handle;
}

void audio_Play(AudioHandle* instance)
{
    ma_sound_start((ma_sound*) instance->sound);

}

void audio_Stop(AudioHandle* instance)
{
    ma_sound_stop((ma_sound*) instance->sound);
}

void audio_UnloadAll()
{
    // TODO: Stop engine here
    dispose(&arena);
}
