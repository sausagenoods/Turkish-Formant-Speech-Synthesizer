#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>

#include <SDL2/SDL.h>

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "../include/optparse.h"

#include "../include/synth.h"

void process_text(char *src, wchar_t **dest);
void err_exit(char *message);

FILE *out;

void print_usage()
{
    printf("Usage: synth -s \"text\" -o out.wav\n"
           "    -s <text>\n"
           "    -o <path>: WAV output to file\n"
           "    -f: use the female voice\n"
           "    -m: use the male voice (experimental, very loud)\n"
           "    -t<tempo value>\n"
           "    -p<pitch value>\n"
           "    -g<gain value>\n"
           "    -a<aspiration value>\n"
           "    -h: print help\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    setlocale(LC_ALL, "en_US.utf8");

    char save_loc[125] = {0};
    wchar_t *text_buf;

    int save = 0;
    int pitch = -1;
    float asp = 1;
    float gain = 0;
    float tempo = 1;
    int vb = 1;

    struct optparse_long longopts[] = {
        {"text", 's', OPTPARSE_REQUIRED},
        {"output", 'o', OPTPARSE_REQUIRED},
        {"female", 'f', OPTPARSE_OPTIONAL},
        {"male", 'm', OPTPARSE_OPTIONAL},
        {"pitch", 'p', OPTPARSE_OPTIONAL},
        {"tempo", 't', OPTPARSE_OPTIONAL},
        {"gain", 'g', OPTPARSE_OPTIONAL},
        {"aspiration", 'a', OPTPARSE_OPTIONAL},
        {"help", 'h', OPTPARSE_OPTIONAL},
        {0}
    };

    int option;
    struct optparse options;
    optparse_init(&options, argv);

    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
        case 's':
            process_text(options.optarg, &text_buf);
            break;

        case 'o':
            save = 1;
            strcpy(save_loc, options.optarg);
            break;

        case 'f':
            vb = 1;
            break;

        case 'm':
            vb = 2;
            break;

        case 'p':
            pitch = options.optarg ? atoi(options.optarg) : 0;
            break;

        case 't':
            tempo = options.optarg ? atof(options.optarg) : 1.5;
            break;

        case 'g':
            gain = options.optarg ? atof(options.optarg) : 1;
            break;

        case 'a':
            asp = options.optarg ? atof(options.optarg) : 0;
            break;

        case 'h':
            print_usage();
            return 1;
            break;

        case '?':
            err_exit("Unknown option. Exiting...");
            break;
        }
    }

    if (wcslen(text_buf) <= 0)
        err_exit("Nothing to synthesize. Exiting...");

    if (save)
        out = fopen(save_loc, "wb");

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return 1;
    }

    synth(text_buf, vb, pitch, tempo, gain, asp, save);

    SDL_Quit();
    return 0;
}

#define append(x)   wd = wcscpy(wd, x);\
                    wd += wcslen(x)

void process_text(char *src, wchar_t **dest)
{
    size_t mbslen = mbstowcs(NULL, src, 0);
    *dest = calloc(mbslen + 128, sizeof(wchar_t));

    wchar_t *wcs = calloc(mbslen + 1, sizeof(wchar_t));
    mbstowcs(wcs, src, mbslen + 1);

    wchar_t *wp = wcs, *wd = *dest;
    for (; *wp != 0; wp++) {
        if (iswalpha(*wp)) {
            if (iswupper(*wp)) *wd++ = towlower(*wp);
            else *wd++ = *wp;
        } else if (iswdigit(*wp)) {
            switch(*wp) {
            case L'0':
                append(L"sıfır");
                break;
            case L'1':
                append(L"bir");
                break;
            case L'2':
                append(L"iki");
                break;
            case L'3':
                append(L"üç");
                break;
            case L'4':
                append(L"dört");
                break;
            case L'5':
                append(L"beş");
                break;
            case L'6':
                append(L"altı");
                break;
            case L'7':
                append(L"yedi");
                break;
            case L'8':
                append(L"sekiz");
                break;
            case L'9':
                append(L"dokuz");
                break;
            default:
                break;
            }
        } else {
            switch(*wp) {
            case L'+':
                append(L"artı");
                break;
            case L'-':
                append(L"eksi");
                break;
            case L'*':
                append(L"çarpı");
                break;
            case L'/':
                append(L"bölü");
                break;
            case L'>':
                append(L"büyüktür");
                break;
            case L'<':
                append(L"küçüktür");
                break;
            case L'%':
                append(L"yüzde");
                break;
            case L'=':
                append(L"eşittir");
                break;
            default:
                *wd++ = L'_';
                break;
            }
        }
    }
    *wd = L'\0';
    free(wcs);
}

void err_exit(char *message)
{
    puts(message);
    exit(EXIT_FAILURE);
}
