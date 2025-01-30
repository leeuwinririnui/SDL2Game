main: main.cpp
	g++ -I src/Include -I src/header -o main main.cpp -l SDL2 -l SDL2_image -l SDL2_ttf

clean:
	rm -f main
