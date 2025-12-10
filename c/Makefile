CC:=gcc
SRCS:=main.c renderer.c vec.c
SRCS:=$(addprefix src/,$(SRCS))
IMGMGK:=convert
ifdef OS
IMGMGK:=magick
endif

all:make render convert
make:
	$(CC) $(SRCS) -lm -std=gnu99 -o main.out
render:make
	rm -rf img
	mkdir img
	./main.out 256 128 64
convert:
	$(IMGMGK) -delay 50 -loop -1 img/*.ppm out.gif
