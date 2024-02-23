#ifndef ALL_H
#define ALL_H
# define UP 65362
# define DOWN 65364
# define LEFT 65361
# define RIGHT 65363
# define SPACE 32
# define SCALE 4
# define SAMPLES_PER_PIXEL 4
// # define ANIMATION 1
#define WINWIDTH 2880
#define WINHEIGHT 1800
#include <X11/X.h>
#include <X11/keysym.h>
#include <math.h>
#include <fcntl.h>
#include <png.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <FL/math.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
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

typedef struct s_calc
{
	v3 oc;
	v3 rotate;
	v3 oc_per;
	v3 rdir_per;
	float a;
	float b;
	float c;
	float discriminant;
	float dot_dir_axis;
	float dot_oc_axis;


	float sqrtDiscriminant;
    float t0;
    float t1;

    float z0;
    float z1;
    float hit;
    float t_hit;
} cyl_calc;

typedef struct {
    v3 origin;
    v3 direction;
    float intensity;
} Light;

void init_all(win *win);
int close_x();
float dot(v3 u, v3 v);
double pitagora(v3 dir);
float distance(v3 a, v3 b);
v3 normalize(v3 dir);
v3 subtract(v3 a, v3 b);
v3 add(v3 a, v3 b);
v3 multiply(v3 a, v3 b);
v3 scale(v3 a, float b);
v3 point_at_parameter(v3 ray_origin, v3 ray_direction, float t);
int renderize(int key, general *general);
int render_general(general *g);
int mouserender(int x, int y, general *g);
void render(camera cam, cylinder cylinder, win win, Plane plane);
int renderize2(int key, general *general);
int calculate_base(cyl_calc c, cylinder cyl, int *flag, float *t_hit, v3 ray_origin, v3 ray_dir);
#endif