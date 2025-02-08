main: main.cpp src/*.cpp
	g++ -I headers -I src -o main main.cpp src/*.cpp -l SDL2 -l SDL2_image -l SDL2_ttf

clean:
	rm -f main
