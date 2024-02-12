#ifndef ALL_H
#define ALL_H
# define UP 65362
# define DOWN 65364
# define LEFT 65361
# define RIGHT 65363
#include <X11/X.h>
#include <X11/keysym.h>
#include <math.h>
#include <fcntl.h>
#include <png.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <FL/math.h>
#include "minilibx-linux/mlx.h"
#define EPSILON 1e-6
typedef struct vec3
{
	double x;
	double y;
	double z;
} v3;

typedef struct camera
{
	v3 origin;
	v3 dir;
	int fov;
} camera;

typedef struct s_cylinder
{
	v3 center;
	v3 axis;
	float radius;
	float height;
	v3 color;
} cylinder;

typedef struct Plane{
    v3 point;
    v3 normal;
    v3 color;
} Plane;

typedef struct window
{
	void *init;
	void *img_ptr;
	void *window;
	int *img_data;
	int endian;
	int size_line;
	int bits_per_pixel;
	int     img_width;
    int     img_height;
} win;

typedef struct general
{
	win *win;
	camera *camera;
	cylinder *cylinder;
	Plane *plane;
} general;

#endif