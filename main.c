#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <SDL.h>

int g_err = 0;
#define CHECK(code, str)                             \
if (code) {                                          \
    fprintf(stderr,"%s: %s\n", str, SDL_GetError()); \
    g_err = 1;                                       \
    goto exit;                                       \
}

char ** tmp_ = NULL;
#define SWAP(x, y) tmp_ = x; x = y; y = tmp_;

#define PARSE_BYTE (strtoul(optarg, NULL, 10) & 0xFF)

enum alignment { LEFT, RIGHT, CENTER };

uint8_t g_rule = 110;
size_t g_width = 401, g_height = 400;
int g_align = CENTER;

void init(const char * input, char * buf)
{
    char * d = NULL;
    switch (g_align) {
    case LEFT:
        d = buf + 1;
        break;
    case RIGHT:
        d = buf + g_width + 1 - strlen(input);
        break;
    default:
        d = buf + g_width / 2 + 1 - strlen(input) / 2;
        break;
    }
    for (const char * c = input; *c != 0; ++c, ++d) {
        if (*c == '0' || *c == '1') *d = *c - '0'; 
        else {
            puts("Input must be a binary number\n");
            exit(69);
        }
    }
}
    
void fill(const char * src, char * dst, int8_t rule)
{
    for(size_t i = 1; i <= g_width; ++i) {
        dst[i] = (rule >> (src[i - 1] << 2 | src[i] << 1 | src[i + 1])) & 1;
    }
}

void show_line(SDL_Renderer * r, int y, const char * line)
{
    // SDL_RenderClear(r);
    for (size_t x = 1; x <= g_width; ++x) {
        if (line[x]) SDL_RenderDrawPoint(r, x, y);
    }
    SDL_RenderPresent(r);
}

int parse_args(int argc, char** argv)
{
    int c, err = 0;
    while((c = getopt(argc, argv, ":r:w:h:LR")) != -1) {
        switch(c) {
        case 'r':
            g_rule = PARSE_BYTE;
            break;
        case 'w':
            g_width = PARSE_BYTE;
            break;
        case 'h':
            g_height = PARSE_BYTE;
            break;
        case 'L':
            g_align = LEFT;
            break;
        case 'R':
            g_align = RIGHT;
            break;
        case ':':
            fprintf(stderr, "Option -%c requires an operand\n", optopt);
            err++;
            break;
        case '?':
            fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
            err++;
        }
    }

    if (err || (optind >= argc)) {
        fprintf(stderr, "Usage: vodca [-r rule] [-w width] [-h height] [-l] input\n");
        exit(69);
    }
    
    return optind;
}

int main(int argc, char ** argv) 
{
    char * buf1 = calloc(sizeof(char), g_width + 2);
    char * buf2 = calloc(sizeof(char), g_width + 2);
    char ** current = &buf1;
    char ** next = &buf2;
    init(argv[parse_args(argc, argv)], *current);

    SDL_Window *w = NULL;
    SDL_Renderer *r = NULL;
    SDL_Event *e = NULL;

    CHECK(SDL_Init(SDL_INIT_VIDEO), "Error initializing SDL");
    CHECK(!(w = SDL_CreateWindow("sdl_window", 100, 100, g_width, g_height,
                SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS)), "Error opening window");
    CHECK(!(r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED)), "Error creating renderer");

    SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(r, NULL);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0xFF);

    show_line(r, 1, *current);
    for (int y = 2; y <= g_height; ++y) {
        fill(*current, *next, g_rule);
        show_line(r, y, *next);
        SWAP(current, next);
        SDL_Delay(10);
    }

    while (SDL_WaitEvent(e))
        if (e->type == SDL_KEYDOWN) break;

exit:
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return g_err;
}
