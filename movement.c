#include "all.h"

bool isJumping = false;
float jumpSpeed = 0.0f;
float gravity = -0.1f;
float jumpHeight = 10.0f;
float initialJumpSpeed = 0.9f;

int renderize(int key, general *general)
{
    float moveStep = 3;
    float rotateStep = 0.01;
    float angleY = general->camera->dir.y;
    float angleX = general->camera->dir.x;
    const float maxPitch = M_PI / 2 - 0.1; 
    const float minPitch = -M_PI / 2 + 0.1;

	if (key == UP || key == 'W'  || key == 'w')
	{
		general->camera->origin.x -= sin(angleY) * moveStep;
		general->camera->origin.z += cos(angleY) * moveStep;
	}
	if (key == DOWN || key == 'S'  || key == 's') {
		general->camera->origin.x += sin(angleY) * moveStep;
		general->camera->origin.z -= cos(angleY) * moveStep;
	}
	if (key == SPACE ) {
		isJumping = true;
		jumpSpeed += initialJumpSpeed;
	}
	if(key == ',')
		jumpSpeed -= initialJumpSpeed;

    general->camera->dir.y = fmod(general->camera->dir.y, 2 * M_PI);

    if (general->camera->dir.x < minPitch)
        general->camera->dir.x = minPitch;
    else if (general->camera->dir.x > maxPitch)
        general->camera->dir.x = maxPitch;

    return 0;
}

int renderize2(int key, general *general)
{
	float moveStep = 3;
    float rotateStep = 0.01;
    float angleY = general->camera->dir.y;
    float angleX = general->camera->dir.x;
    const float maxPitch = M_PI / 2 - 0.1; 
    const float minPitch = -M_PI / 2 + 0.1;
	if (key == LEFT || key == 'a'  || key == 'A') {
		general->camera->origin.x -= cos(angleY) * moveStep;
		general->camera->origin.z -= sin(angleY) * moveStep;
	}
	if (key == RIGHT  || key == 'd'  || key == 'D') {
		general->camera->origin.x += cos(angleY) * moveStep;
		general->camera->origin.z += sin(angleY) * moveStep;
	}

	general->camera->dir.y = fmod(general->camera->dir.y, 2 * M_PI);

    if (general->camera->dir.x < minPitch)
        general->camera->dir.x = minPitch;
    else if (general->camera->dir.x > maxPitch)
        general->camera->dir.x = maxPitch;
}

int render_general(general *g)
{
	#ifdef ANIMATION
    static float direction = 1.0;
    static float speed = 2.5;
    static float upper_limit = 100.0;
    static float lower_limit = 5.0;
    static float angle = 0.0;
    static float rotation_speed = 0.05;

    g->cylinder->center.y += direction * speed;

    if (g->cylinder->center.y >= upper_limit || g->cylinder->center.y <= lower_limit)
        direction *= -1;

    angle += rotation_speed;
    if (angle > 2 * M_PI) {
        angle -= 2 * M_PI;
    }

    g->cylinder->axis.x = cos(angle);
    g->cylinder->axis.z = sin(angle);

	if (isJumping) {
		g->camera->origin.y += jumpSpeed;
		jumpSpeed += gravity;

		if (g->camera->origin.y <= jumpHeight)
		{
			g->camera->origin.y = jumpHeight;
			isJumping = false;
			jumpSpeed = 0.0f;
		}
	}
	#endif
    render(*g->camera, *g->cylinder, *g->win, *g->plane);
}

int mouserender(int x, int y, general *g)
{
    static int prev_x = 0;
    static int prev_y = 0;
    float rotateSpeed = 0.005;
    const float maxPitch = M_PI / 2 - 0.1;
    const float minPitch = -M_PI / 2 + 0.1;

    if (prev_x == 0 && prev_y == 0)
	{
        prev_x = x;
        prev_y = y;
    }
    int delta_x = x - prev_x;
    int delta_y = y - prev_y;
    g->camera->dir.y -= delta_x * rotateSpeed;
    g->camera->dir.x += delta_y * rotateSpeed;
    g->camera->dir.x = fmax(minPitch, fmin(maxPitch, g->camera->dir.x));
    prev_x = x;
    prev_y = y;
    return 0;
}
