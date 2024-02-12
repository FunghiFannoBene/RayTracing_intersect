#include "all.h"

int close_x()
{
	exit(1);
	return (0);
}

float dot(v3 u, v3 v)
{
	return(u.x*v.x + u.y * v.y + u.z * v.z);
}

void init_all(cylinder *cylinder, win *win)
{
	if (!cylinder)
		return;

	cylinder->center.x = 0.0;
	cylinder->center.y = 10.0;
	cylinder->center.z = 60.6;

	cylinder->axis.x = 0.0;
	cylinder->axis.y = 0.0;
	cylinder->axis.z = 1.0;

	cylinder->radius = 7.1;
	cylinder->height = 33.42;

	cylinder->color.x = 139;
	cylinder->color.y = 70;
	cylinder->color.z = 15; 

	win->init = mlx_init();
	win->window = mlx_new_window(win->init, 1920, 1080, "title rx");
	win->img_ptr = mlx_new_image(win->init, 1920, 1080);
	win->img_data = (int *)mlx_get_data_addr(win->img_ptr, &win->bits_per_pixel, &win->size_line, &win->endian);
	mlx_hook(win->window, DestroyNotify, ButtonPressMask, close_x, NULL);
}

double pitagora(v3 dir)
{
	return (sqrt(dot(dir, dir)));
}

v3 point_at_parameter(v3 ray_origin, v3 ray_direction, float t)
{
    return (v3){ ray_origin.x + t * ray_direction.x,
                 ray_origin.y + t * ray_direction.y,
                 ray_origin.z + t * ray_direction.z };
}

float distance(v3 p1, v3 p2)
{
	p2.x -= p1.x;
	p2.y -= p1.y;
	p2.z -= p1.z;
	return pitagora(p2);
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


int hit_sphere(v3 center, float radius, v3 ray_origin, v3 ray_dir)
{
    v3 origine_meno_center = {ray_origin.x - center.x, ray_origin.y - center.y, ray_origin.z - center.z};
    float a = dot(ray_dir, ray_dir);
    float b = 2.0 * dot(origine_meno_center, ray_dir);
    float c = dot(origine_meno_center, origine_meno_center) - radius * radius;
    float discriminante = b * b - 4 * a * c;
    return (discriminante > 0);
}

bool intersectPlane(Plane* plane, v3 rayOrigin, v3 rayDirection, float* t)
{
    float denominatore = dot(plane->normal, rayDirection);
    if (fabs(denominatore) > EPSILON)
	{
        v3 originToPlane = subtract(plane->point, rayOrigin);
        *t = dot(originToPlane, plane->normal) / denominatore;
        if (*t >= 0)
            return (rayDirection.y < 0);
    }
    return false;
}

int hit_cyl(cylinder cyl, v3 ray_origin, v3 ray_dir, int *flag)
{
	v3 origin_meno_center = {ray_origin.x - cyl.center.x, ray_origin.y - cyl.center.y, ray_origin.z - cyl.center.z};

	float a = ray_dir.x * ray_dir.x + ray_dir.z * ray_dir.z;
	float b = 2.0 * (origin_meno_center.x * ray_dir.x + origin_meno_center.z * ray_dir.z);
	float c = origin_meno_center.x * origin_meno_center.x + origin_meno_center.z * origin_meno_center.z - cyl.radius * cyl.radius;

	float discriminante = b * b - 4 * a * c;
	if (discriminante < 0)
		return 0;

	float t0 = (-b - sqrt(discriminante)) / (2 * a);
	float t1 = (-b + sqrt(discriminante)) / (2 * a);

	if (t0 > 0)
	{
		float y0 = ray_origin.y + t0 * ray_dir.y;
		if (y0 >= cyl.center.y && y0 <= cyl.center.y + cyl.height)
		{
			*flag = 1;
			return t0;
		}
	}

	if (t1 > 0)
	{
		float y1 = ray_origin.y + t1 * ray_dir.y;
		if (y1 >= cyl.center.y && y1 <= cyl.center.y + cyl.height)
		{
			*flag = 0;
			return t1;
		}
	}
	return 0;
}

int get_color(v3 ray, v3 origin, cylinder cylinder, Plane plane)
{
	v3 point_of_intersection;
	v3 normal;
	int flag;
	float t = hit_cyl(cylinder, origin, ray, &flag);
	float t2 = intersectPlane(&plane, origin, ray, &t2);
    if (t > 0.0)
	{
		if(flag == 0)
			return(0xFFFFFF);
        point_of_intersection = 
		(v3){
            origin.x + t * ray.x,
            origin.y + t * ray.y,
            origin.z + t * ray.z
        };

        normal = 
		(v3){
            (point_of_intersection.x - cylinder.center.x) / cylinder.radius, 
			0, 
			(point_of_intersection.z - cylinder.center.z) / cylinder.radius
        };
        normal = normalize(normal);

        int r = (int)((normal.x + 1) * 0.5 * cylinder.color.x);
        int g = (int)((normal.y + 1) * 0.5 * cylinder.color.y);
        int b = (int)((normal.z + 1) * 0.5 * cylinder.color.z);

        return (r << 16) | (g << 8) | b;
    }
	else if (t2 > 0.0)
	{
        return ((int)(plane.color.x) << 16) | ((int)(plane.color.y) << 8) | (int)plane.color.z;
    }
	else
		return (0x5BFFF);
}

void render(camera cam, cylinder cylinder, win win, Plane plane)
{
	float viewport_height = 2.0;
	float viewport_width = 2.0 * 1920 / 1080;
	float forigin_meno_centeral_length = 1.0;
	int x = 0, y = 0;
	while (y < 1080)
	{
		x = 0;
		while (x < 1920)
		{
			float viewport_x = (x - 1920 / 2.0) * (viewport_width / 1920);
			float viewport_y = -(y - 1080 / 2.0) * (viewport_height / 1080);

			v3 ray_direction = {viewport_x, viewport_y, forigin_meno_centeral_length};
			ray_direction = normalize(ray_direction);
			win.img_data[x + y * 1920] = get_color(ray_direction, cam.origin, cylinder, plane);

			x++;
		}
		y++;
	}
	mlx_put_image_to_window(win.init, win.window, win.img_ptr, 0, 0);
	printf("done\n");
}

int renderize(int key, general *general)
{
	if(key == UP)
	{
		general->camera->origin.z += 5.0;
		render(*general->camera, *general->cylinder, *general->win, *general->plane);
	}
	else if(key == LEFT)
	{
		general->camera->origin.x -= 5.0;
		render(*general->camera, *general->cylinder, *general->win, *general->plane);
	}
	else if(key == RIGHT)
	{
		general->camera->origin.x += 5.0;
		render(*general->camera, *general->cylinder, *general->win, *general->plane);
	}
	else if(key == DOWN)
	{
	general->camera->origin.z -= 5.0;
	render(*general->camera, *general->cylinder, *general->win, *general->plane);
	}
}

int main(int argc, char **argv, char **env)
{
	general general = {0};
	camera cam = 
	{
		.origin = {-10, 0, 0},
		.dir = {0.0, 0.0, 1.0},
		.fov = 70
	};
	Plane plane = 
	{
    .point = {0.0f, 0.0f, -10.0f},
    .normal = {0.0f, 1.0f, 0.0f},
    .color = {25 , 140 , 40}
	};
	
	cylinder cylinder;
	win win = {0};
	general.camera = &cam;
	general.cylinder = &cylinder;
	general.win = &win;
	general.plane = &plane;

	init_all(&cylinder, &win);
	mlx_key_hook(win.window,renderize, &general);
	mlx_hook(win.window, KeyPress, KeyPressMask, renderize, &general);
	render(cam, cylinder, win, plane);

	mlx_loop(win.init);
}