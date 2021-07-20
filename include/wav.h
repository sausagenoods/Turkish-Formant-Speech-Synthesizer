#ifndef WAV_H
#define WAV_H

#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 32
#define N_CHANNELS 1

typedef struct {
    char riff[4];
    int32_t file_size;
    char wave[4];
    char fmt[4];
    int32_t format_size;
    int16_t audio_format;
    int16_t channels;
    int32_t sample_rate;
    int32_t byte_rate;
    int16_t block_align;
    int16_t bits_per_sample;
    char data[4];
    int32_t data_size;
} wav_header_t;

void make_wav_header(wav_header_t *wav_header)
{
    wav_header->riff[0] = 'R';
    wav_header->riff[1] = 'I';
    wav_header->riff[2] = 'F';
    wav_header->riff[3] = 'F';

    wav_header->wave[0] = 'W';
    wav_header->wave[1] = 'A';
    wav_header->wave[2] = 'V';
    wav_header->wave[3] = 'E';

    wav_header->fmt[0] = 'f';
    wav_header->fmt[1] = 'm';
    wav_header->fmt[2] = 't';
    wav_header->fmt[3] = ' ';

    wav_header->format_size = 16;
    wav_header->audio_format = 3; /* float */
    wav_header->channels = N_CHANNELS;
    wav_header->sample_rate = SAMPLE_RATE;
    wav_header->byte_rate = SAMPLE_RATE * N_CHANNELS * BITS_PER_SAMPLE / 8;
    wav_header->block_align = N_CHANNELS * BITS_PER_SAMPLE / 8;
    wav_header->bits_per_sample = BITS_PER_SAMPLE;

    wav_header->data[0] = 'd';
    wav_header->data[1] = 'a';
    wav_header->data[2] = 't';
    wav_header->data[3] = 'a';
}

#endif
