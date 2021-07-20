CFLAGS=-Wall -Wmultichar
LDFLAGS=-lm -lSDL2

all: synth

synth:
	gcc src/main.c src/synth.c $(LDFLAGS) -o synth

synthgui:
	gcc -c src/synth.c $(LDFLAGS) -o synth.o
	g++ -I/usr/include/SDL2 -Igui/imgui gui/main.cpp gui/imgui/*.cpp gui/imgui/backends/imgui_impl_opengl2.cpp gui/imgui/backends/imgui_impl_sdl.cpp synth.o $(LDFLAGS) -lGL -o synthgui
	rm -f synth.o

clean:
	rm -f synth synthgui
