#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <SDL2/SDL.h>

#include "../include/synth.h"
#include "../include/wav.h"

#include "../include/frames_f.h"
#include "../include/frames_m.h"

#define ORDER 48
#define BUFFER_ORDER 256
#define DX 0.15f

wav_header_t wav_header;

SDL_AudioDeviceID audio_device;

float temp[BUFFER_ORDER] = {0};
unsigned offset = 0, x = 0;

void lpc_decode(frame_t p, int pitch, float tempo, float gain,
                  float asp, int save)
{
    unsigned period, samples_per_frame;
    double y, f, sum;
    float sample;

    samples_per_frame = DX * SAMPLE_RATE / tempo;
    for (int i = 0; i < samples_per_frame; ++i) {
        x++;

        /* Triangle wave source */
        period = SAMPLE_RATE / pitch;
        y = 0.005f * abs((x %= period) - period);

        /* Hacky but needed for Windows */
        f = 0.05f * ((double)rand()/(double)RAND_MAX) + exp2(y) * asp;

        /* Apply coefficients and overlap */
        sum = f;
        for (int j = 0; j < ORDER; ++j)
            sum -= p.a[j] * temp[(offset + BUFFER_ORDER-j) % BUFFER_ORDER];

        offset++;
        temp[offset %= BUFFER_ORDER] = sum;

        sample = sum * sqrt(p.gain + gain);
        SDL_QueueAudio(audio_device, &sample, sizeof(sample));

        if (save)
            fwrite(&sample, sizeof(sample), 1, out);
    }
}

int synth(wchar_t *text, int voice_bank, int pitch_val, float tempo_val,
            float gain_val, float asp_val, int save)
{
    voice_bank_t* vbank;
    if (voice_bank == 1) {
        vbank = &female_bank;
    } else {
        vbank = &male_bank;
    }

    int pitch;
    if (pitch_val < 0)
        pitch = vbank->pitch;
    else
        pitch = pitch_val;

    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_F32SYS;
    spec.channels = 1;
    spec.samples = 4096;
    spec.callback = NULL;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);

    unsigned samples_per_frame;
    if (save) {
        make_wav_header(&wav_header);
        samples_per_frame = DX * SAMPLE_RATE;
        wav_header.data_size = wcslen(text) * ORDER * samples_per_frame *
                                 N_CHANNELS * BITS_PER_SAMPLE / 8;
        wav_header.file_size = 36 + wav_header.data_size;

        fwrite(&wav_header, sizeof(wav_header_t), 1, out);
    }

    wchar_t sounds[30] = L"abcçdefgğhiıjklmnoöprsştuüvyz_";
    for (int i = 0; text[i] != L'\0'; i++) {
        for (int j = 0; j < 30; j++) {
            if (text[i] == sounds[j]) {
                lpc_decode(vbank->frames[j], pitch, tempo_val,
                             gain_val, asp_val, save);
                break;
            }
        }
    }

    SDL_PauseAudioDevice(audio_device, 0);
    SDL_Delay((wcslen(text)*DX*1000/tempo_val));

    memset(temp, sizeof(temp), 0);
    SDL_CloseAudioDevice(audio_device);
    return 0;
}
