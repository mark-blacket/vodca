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

#define PARSE_UINT (strtoul(optarg, NULL, 10))

typedef enum { LEFT, RIGHT, CENTER } alignment;

uint8_t g_rule = 110;
size_t g_width = 401, g_height = 400;
alignment g_align = CENTER;

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

void render_line(SDL_Renderer * r, int y, const char * line)
{
    for (size_t x = 1; x <= g_width; ++x) {
        if (line[x]) SDL_RenderDrawPoint(r, x, y);
    }
}

int parse_args(int argc, char** argv)
{
    int c, err = 0;
    while((c = getopt(argc, argv, ":r:w:h:LR")) != -1) {
        switch(c) {
        case 'r':
            g_rule = PARSE_UINT & 0xFF;
            break;
        case 'w':
            g_width = PARSE_UINT;
            break;
        case 'h':
            g_height = PARSE_UINT;
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
    int optind = parse_args(argc, argv); // this sets global vars and should run first
    char * buf1 = calloc(sizeof(char), g_width + 2);
    char * buf2 = calloc(sizeof(char), g_width + 2);
    if (!buf1 || !buf2) {
        fprintf(stderr, "Error allocating memory for buffers\n");
        exit(69);
    }

    char ** current = &buf1;
    char ** next = &buf2;
    init(argv[optind], *current);

    SDL_Window *w = NULL;
    SDL_Renderer *r = NULL;
    SDL_Event e;

    CHECK(SDL_Init(SDL_INIT_VIDEO), "Error initializing SDL");
    CHECK(!(w = SDL_CreateWindow("sdl_window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                g_width, g_height, SDL_WINDOW_OPENGL)), "Error opening window");
    CHECK(!(r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED)), "Error creating renderer");

    SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(r, NULL);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0xFF);

    render_line(r, 1, *current);
    for (int y = 2; y <= g_height; ++y) {
        fill(*current, *next, g_rule);
        render_line(r, y, *next);
        SWAP(current, next);
    }
    SDL_RenderPresent(r);

    while (SDL_WaitEvent(&e))
        if (e.type == SDL_KEYDOWN) break;

exit:
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return g_err;
}
