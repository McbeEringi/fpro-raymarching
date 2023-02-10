CC:=gcc
SRCS:=main.c renderer.c vec.c
SRCS:=$(addprefix src/,$(SRCS))
IMGMGK:=convert
ifdef OS
IMGMGK:=magick convert
endif

all:make render convert
make:
	$(CC) $(SRCS) -o main.out
render:make
	./main.out 256 128 64
convert:
	$(IMGMGK) -delay 50 -loop -1 img/*.ppm out.gif