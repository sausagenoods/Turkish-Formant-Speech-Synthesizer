#ifndef SYNTH_H
#define SYNTH_H

extern FILE *out;

typedef struct {
    float a[48];
    float gain;
} frame_t;

typedef struct {
    int pitch;
    frame_t frames[30];
} voice_bank_t;

int synth(wchar_t *text, int vb, int pitch, float tempo,
            float gain, float aspiration, int save);

#endif
