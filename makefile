main: main.cpp
	g++ -I src/Include -I src/header -o main main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf

clean:
	rm -f main
