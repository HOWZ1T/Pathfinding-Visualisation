#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <SDL.h>

#define println(fmt, ...) do {fflush(stdout); fprintf(stdout, fmt, __VA_ARGS__); fputc('\n', stdout);} while(0)

struct Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};

typedef struct Color Color;

#define WHITE (Color){ 255, 255, 255, 255 }
#define BLACK (Color){ 0, 0, 0, 255 }
#define RED (Color){ 255, 0, 0, 255 }
#define GREEN (Color){ 0, 255, 0, 255 }
#define BLUE (Color){ 0, 0, 255, 255 }