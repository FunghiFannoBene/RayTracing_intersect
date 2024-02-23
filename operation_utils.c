#include "all.h"

void init_all(win *win)
{
	win->init = mlx_init();
	win->window = mlx_new_window(win->init, WINWIDTH, WINHEIGHT, "title rx");
	win->img_ptr = mlx_new_image(win->init, WINWIDTH, WINHEIGHT);
	win->img_data = (int *)mlx_get_data_addr(win->img_ptr, &win->bits_per_pixel, &win->size_line, &win->endian);
	mlx_hook(win->window, DestroyNotify, ButtonPressMask, close_x, NULL);
}

int close_x()
{
	exit(1);
	return (0);
}

float dot(v3 u, v3 v)
{
	return(u.x*v.x + u.y * v.y + u.z * v.z);
}

double pitagora(v3 dir)
{
	return (sqrt(dot(dir, dir)));
}


v3 normalize(v3 dir)
{
	double norm = pitagora(dir);
	dir.x /= norm;
	dir.y /= norm;
	dir.z /= norm;
	return dir;
}

v3 subtract(v3 a, v3 b)
{
	a.x-=b.x;
	a.y-=b.y;
	a.z-=b.z;
	return a;
}

v3 add(v3 a, v3 b)
{
	a.x+=b.x;
	a.y+=b.y;
	a.z+=b.z;
	return a;
}

v3 multiply(v3 a, v3 b)
{
	a.x*=b.x;
	a.y*=b.y;
	a.z*=b.z;
	return a;
}

v3 scale(v3 a, float b)
{
	a.x*=b;
	a.y*=b;
	a.z*=b;
	return a;
}

float distance(v3 a, v3 b)
{
    return sqrt(dot(subtract(a,b), subtract(a,b)));
}


v3 point_at_parameter(v3 ray_origin, v3 ray_direction, float t)
{
    return (v3)
	{
		ray_origin.x + t * ray_direction.x,
        ray_origin.y + t * ray_direction.y,
        ray_origin.z + t * ray_direction.z
	};
}
