Asteroids: Asteroids.c
	gcc Asteroids.c -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf helper.c -lm -o Asteroids

clean:
	rm -f Asteroids
