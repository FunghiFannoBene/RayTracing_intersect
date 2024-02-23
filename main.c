#include "all.h"

float getT(float t1, float t2)
{
    if (t1 < 0 && t2 < 0)
        return -1.0;
    if (t1 < 0)
		return t2;
    if (t2 < 0)
		return t1;
    return fmin(t1, t2);
}

float hit_sphere(v3 center, float radius, v3 ray_origin, v3 ray_dir)
{
    v3 oc;
    float a;
    float b; 
    float c;
	float discr;

	oc = (v3){ray_origin.x - center.x, ray_origin.y - center.y, ray_origin.z - center.z};
	a = dot(ray_dir, ray_dir);
	b = 2.0 * dot(oc, ray_dir);
	c = dot(oc, oc) - radius * radius;
    discr = b * b - 4 * a * c;
    if (discr < 0)
        return -1.0;
    else
        return(getT((-b - sqrt(discr)) / (2*a), (-b + sqrt(discr)) / (2*a)));
}

bool intersectPlane(Plane* plane, v3 rayOrigin, v3 rayDirection, float* t)
{
    float denom;
	v3 co;

	denom = dot(plane->normal, rayDirection);
    if (fabs(denom) > EPSILON)
	{
        co = subtract(plane->point, rayOrigin);
        *t = dot(co, plane->normal) / denom;
        if (*t >= 0)
            return (rayDirection.y < 0);
    }
    return false;
}

int calculate_base(cyl_calc c, cylinder cyl, int *flag, float *t_hit, v3 ray_origin, v3 ray_dir)
{
	float d;
	v3 p;
	int i;
	int h;
	float t_base;

	i = 0;
    while (i < 2)
	{
		h = cyl.height;
		if(i == 0)
			h = 0;
        t_base = (h - c.dot_oc_axis) / c.dot_dir_axis;
        if (t_base > 0 && t_base < *t_hit)
		{
            p = point_at_parameter(ray_origin, ray_dir, t_base);
            d = distance(add(cyl.center, scale(c.rotate, h)), p);
            if (d <= cyl.radius)
			{
                *t_hit = t_base;
                c.hit = 1;
                *flag = 2;
            }
        }
		++i;
    }
	return c.hit;
}

float hit_cyl(cylinder cyl, v3 ray_origin, v3 ray_dir, int *flag)
{
	cyl_calc c;
	float h;
	
    c.rotate = normalize(cyl.axis);
    c.dot_dir_axis = dot(ray_dir, c.rotate);
    c.dot_oc_axis = dot(subtract(ray_origin, cyl.center), c.rotate);
    c.oc_per = subtract(subtract(ray_origin, cyl.center), scale(c.rotate, c.dot_oc_axis));
    c.rdir_per = subtract(ray_dir, scale(c.rotate, c.dot_dir_axis));
    c.a = dot(c.rdir_per, c.rdir_per);
    c.b = 2.0 * dot(c.oc_per, c.rdir_per);
    c.c = dot(c.oc_per, c.oc_per) - cyl.radius * cyl.radius;
    c.discriminant = c.b * c.b - 4 * c.a * c.c;
    if (c.discriminant < 0)
		return 0;
    c.sqrtDiscriminant = sqrt(c.discriminant);
    c.t0 = (-c.b - c.sqrtDiscriminant) / (2 * c.a);
    c.t1 = (-c.b + c.sqrtDiscriminant) / (2 * c.a);
    c.z0 = c.dot_oc_axis + c.t0 * c.dot_dir_axis;
    c.z1 = c.dot_oc_axis + c.t1 * c.dot_dir_axis;
    c.hit = 0;
    c.t_hit = INFINITY;
    if (c.t0 > 0 && c.z0 >= 0 && c.z0 <= cyl.height && c.t0 < c.t_hit)
	{
        c.t_hit = c.t0;
        c.hit = 1;
        *flag = 1;
    }
    if (c.t1 > 0 && c.z1 >= 0 && c.z1 <= cyl.height && c.t1 < c.t_hit)
	{
        c.t_hit = c.t1;
        c.hit = 1;
        *flag = 0;
    }
	c.hit = calculate_base(c, cyl, flag, &c.t_hit, ray_origin, ray_dir);
	if(c.hit)
		return(c.t_hit);
    return 0;
}

int calculate_lighting(v3 hitPoint, v3 normal, v3 object_color, Light light) {
    v3 light_dir = normalize(subtract(light.origin, hitPoint));
    float diff = fmax(dot(normal, light_dir), 0.0);
    v3 diff_color = {
        object_color.x * light.intensity * diff,
        object_color.y * light.intensity * diff,
        object_color.z * light.intensity * diff
    };
    int r = (int)(diff_color.x * 255);
    int g = (int)(diff_color.y * 255);
    int b = (int)(diff_color.z * 255);
    return (r << 16) | (g << 8) | b;
}

int get_color(v3 ray, v3 origin, cylinder cylinder, Plane plane) {
    Light light = {
        .origin = {-10, 1000, 10},
        .intensity = 1.0
    };

    v3 point_of_intersection;
    v3 normal;
    int flag = 0;
    float min_t = INFINITY;
    int color = 0x5BFFF; // Colore di default

    float t_cyl = hit_cyl(cylinder, origin, ray, &flag);
    if (t_cyl > 0.0 && t_cyl < min_t)
	{
        point_of_intersection = point_at_parameter(origin, ray, t_cyl);
        if (flag == 1)
            normal = normalize(subtract(point_of_intersection, add(cylinder.center, scale(cylinder.axis, dot(subtract(point_of_intersection, cylinder.center), cylinder.axis)))));
        else if (flag == 2)
            normal = cylinder.axis;
        v3 cyl_color = {cylinder.color.x / 255.0f, cylinder.color.y / 255.0f, cylinder.color.z / 255.0f};
        color = calculate_lighting(point_of_intersection, normal, cyl_color, light);
        min_t = t_cyl;
    }

    float t_sphere = hit_sphere((v3){100, 20, 53.0}, 15.0, origin, ray);
    if (t_sphere > 0.0 && t_sphere < min_t)
	{
        min_t = t_sphere;
        v3 hitPoint = add(origin, scale(ray, t_sphere));
        v3 norm = normalize(subtract(hitPoint, (v3){100, 20, 53.0}));
        v3 base_color = {0.663f, 0.176f, 0.173f}; // Colore della sfera
        color = calculate_lighting(hitPoint, norm, base_color, light);
    }

    float t_plane;
    if (intersectPlane(&plane, origin, ray, &t_plane) && t_plane < min_t) {
        v3 hitPoint = {
            round((origin.x + ray.x * t_plane) * 10) / 10,
            origin.y + ray.y * t_plane,
            round((origin.z + ray.z * t_plane) * 10) / 10
        };
        int checkerWidth = 120;
        int checkerX = (int)(floor(fabs(hitPoint.x) / checkerWidth)) & 1;
        int checkerZ = (int)(floor(fabs(hitPoint.z) / checkerWidth)) & 1;
        v3 plane_color = {plane.color.x / 255.0f, plane.color.y / 255.0f, plane.color.z / 255.0f};
        if ((checkerX ^ checkerZ) & 1)
            plane_color = (v3){1.0f, 1.0f, 1.0f};
        else
            plane_color = (v3){0.322f, 0.161f, 0.322f};
        normal = plane.normal;
        color = calculate_lighting(hitPoint, normal, plane_color, light);
    }

    return color;
}

float clamp(double x, double min, double max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

void render(camera cam, cylinder cylinder, win win, Plane plane) 
{
    float aspect_ratio = (float)(WINWIDTH / SCALE) / (float)(WINHEIGHT / SCALE);
    float viewport_height = 2.0 * tan(cam.fov * M_PI / 360.0);
    float viewport_width = viewport_height * aspect_ratio;
    int renderWidth = WINWIDTH / SCALE;
    int renderHeight = WINHEIGHT / SCALE;
    
    for (int y = 0; y < renderHeight; ++y)
	{
        for (int x = 0; x < renderWidth; ++x)
		{
            float viewport_x = (x - renderWidth / 2.0) * (viewport_width / renderWidth);
            float viewport_y = -(y - renderHeight / 2.0) * (viewport_height / renderHeight);
            
            v3 ray_direction = {viewport_x, viewport_y, 1.0};
            float cosAngleX = cos(cam.dir.x);
            float sinAngleX = sin(cam.dir.x);
            v3 rotatedX = 
			{
                ray_direction.x,
                ray_direction.y * cosAngleX - ray_direction.z * sinAngleX,
                ray_direction.y * sinAngleX + ray_direction.z * cosAngleX
            };
            float cosAngleY = cos(cam.dir.y);
            float sinAngleY = sin(cam.dir.y);
            v3 rotatedY = {
                rotatedX.x * cosAngleY - rotatedX.z * sinAngleY,
                rotatedX.y,
                rotatedX.x * sinAngleY + rotatedX.z * cosAngleY
            };
            v3 normalized_direction = normalize(rotatedY);

            int color = get_color(normalized_direction, cam.origin, cylinder, plane);

            for (int dy = 0; dy < SCALE; ++dy) {
                for (int dx = 0; dx < SCALE; ++dx) {
                    int finalX = x * SCALE + dx;
                    int finalY = y * SCALE + dy;
                    if (finalX < WINWIDTH && finalY < WINHEIGHT)
                        win.img_data[finalX + finalY * WINWIDTH] = color;
                }
            }
        }
    }
    mlx_put_image_to_window(win.init, win.window, win.img_ptr, 0, 0);
}

int main(int argc, char **argv, char **env)
{
	general general = {0};
	camera cam = 
	{
		.origin = {-10, 10, 0},
		.dir = {0.2, -0.5, 1.0},
		.fov = 90
	};
	Plane plane = 
	{
    .point = {0.0f, 0.0f, -10.0f},
    .normal = {0.0f, 1.0f, 0.0f},
    .color = {25 , 140 , 40}
	};
	cylinder cylinder = {
	.center = {400.0, 100.0, 60.6}, 
	.axis = {0.5, 1.0, 1.0},
	.radius = 40.1,
	.height = 402.1,
	.color = {19, 70, 15}
	};
	win win = {0};
	general.camera = &cam;
	general.cylinder = &cylinder;
	general.win = &win;
	general.plane = &plane;

	init_all(&win);
	mlx_key_hook(win.window,renderize, &general);
	mlx_hook(win.window, KeyPress, KeyPressMask, renderize, &general);
	mlx_hook(win.window, KeyPress, KeyPressMask, renderize2, &general);
	mlx_hook(win.window, MotionNotify, PointerMotionMask, mouserender, &general);
	mlx_loop_hook(win.init, render_general, &general);
	mlx_loop(win.init);
}