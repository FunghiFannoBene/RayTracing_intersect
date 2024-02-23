all:
	gcc -g *.c -L./minilibx-linux -lmlx -lXext -lX11 -lm -o rt
f:
	gcc -g *.c -L./minilibx-linux -lmlx -L/usr/X11R6/lib -lX11 -lXext -lm -o rt
run:
	./rt
