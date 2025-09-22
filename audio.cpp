#include <iostream>

#include "audio.hpp"

int Audio::record()
{
    // Open PCM device for recording (capture)
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
        std::cerr << "ERROR: Cannot open default PCM device " << std::endl;
        return 1;
    }

    // Allocate hardware parameters object
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);

    // Set parameters
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE); // 16-bit
    snd_pcm_hw_params_set_channels(pcm_handle, params, 1); // mono
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);

    // Write parameters to the driver
    if (snd_pcm_hw_params(pcm_handle, params) < 0) {
        std::cerr << "ERROR: Cannot set hardware parameters." << std::endl;
        return 1;
    }

    // Allocate buffer to hold one period
    int buffer_size = frames * 2; // 2 bytes per sample (16-bit mono)
    char *buffer = new char[buffer_size];

    std::cout << "Recording... (press Ctrl+C to quit)" << std::endl;

    while (true)
    {
        int rc = snd_pcm_readi(pcm_handle, buffer, frames);
        if (rc == -EPIPE) {
            // Overrun occurred
            std::cerr << "Overrun occurred!" << std::endl;
            snd_pcm_prepare(pcm_handle);
        } else if (rc < 0) {
            std::cerr << "Error reading from PCM device: " << snd_strerror(rc) << std::endl;
        } else if (rc != (int)frames) {
            std::cerr << "Short read, read " << rc << " frames" << std::endl;
        } else {
            // For now, just print the first sample value
            short *samples = reinterpret_cast<short*>(buffer);
            std::cout << "Sample[0] = " << samples[0] << std::endl;
        }
    }

    delete[] buffer;
}

