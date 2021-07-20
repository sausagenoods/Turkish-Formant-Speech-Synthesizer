/*
 * gcc lpc2wav.c -lm -o lpc2wav
 *
 * Convert an LPC file to WAV.
 * Usage: ./lpc2wav lpc/alfabe_f_44100.LPC out.wav
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/wav.h"

#define ORDER 48
#define BUFFER_ORDER 256
#define VOICE_PITCH 230
#define SAMPLE_RATE 44100

int main(int argc, char *argv[])
{
    if (argc != 3) {
        puts("Usage: ./lpc2wav lpcfile wavfile");
        exit(EXIT_FAILURE);
    }

    double *a, temp[BUFFER_ORDER] = {0};
    double coefficient = 0, gain = 0, frame_length = 0.2;
    unsigned offset = 0, x = 0, index = 0, order = ORDER, samples_per_frame = 0;
    int frame_count = 0;
    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");

    if (in == NULL || out == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    wav_header_t wav_header;
    make_wav_header(&wav_header);

    unsigned period = SAMPLE_RATE / VOICE_PITCH;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, in)) != -1) {
        if (sscanf(line, "nx = %d", &frame_count) == 1) {}
        else if (sscanf(line, "dx = %lf", &frame_length) == 1) {}
        else if (sscanf(line, "maxnCoefficients = %d", &order) == 1) {
            a = malloc(order * sizeof(double));

            samples_per_frame = frame_length * SAMPLE_RATE;
            wav_header.data_size = frame_count * samples_per_frame * N_CHANNELS * BITS_PER_SAMPLE / 8;
            wav_header.file_size = 36 + wav_header.data_size;

            fwrite(&wav_header, sizeof(wav_header_t), 1, out);
        } else if (sscanf(line, "%*s [%d] = %lf\n", &index, &coefficient) == 2) {
            a[index-1] = coefficient;
        } else if (sscanf(line, "        gain = %lf", &gain) == 1) {
            for (unsigned i = 0; i < samples_per_frame; ++i) {
                x++;

                /* Triangle wave */
                double y = 0.005f * abs((x %= period) - period);

                /* Aspiration */
                double f = 0.05f * ((double)rand()/(double)RAND_MAX) + exp2(y);

                /* Apply LPC filter */
                double sum = f;
                for (unsigned j = 0; j < ORDER; ++j)
                    sum -= a[j] * temp[(offset + BUFFER_ORDER-j) % BUFFER_ORDER];

                offset++;
                temp[offset %= BUFFER_ORDER] = sum;

                float sample = sum * sqrt(gain);
                fwrite(&sample, sizeof(sample), 1, out);
            }
        }
    }

    free(a);
    free(line);
    fclose(in);
    fclose(out);
    return 0;
}
