#include <alsa/asoundlib.h>

class Audio
{
public:
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    int dir;
    unsigned int sample_rate;
    snd_pcm_uframes_t frames; // frames per period

    Audio()
    {
        sample_rate = 44100;
        frames = 32;
    }

    ~Audio()
    {
        snd_pcm_hw_params_free(params);
        snd_pcm_close(pcm_handle);
    }

    int record();
};
