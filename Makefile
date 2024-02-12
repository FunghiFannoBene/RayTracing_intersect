all:
	gcc -g *.c -L./minilibx-linux -lmlx -lXext -lX11 -lm -o rt
run:
	./rt