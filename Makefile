CC=gcc
CFLAGS=-Wall -I/usr/X11R6/include
LDFLAGS=-pthread -L/usr/X11R6/lib -lm -lGL -lglut
OBJS=main.o scene.o shader.o

glsl_lighting:	$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o glsl_lighting

clean:
	rm -f glsl_lighting
	rm -f $(OBJS)

main.o: main.c
scene.o: scene.c
shader.o: shader.c
