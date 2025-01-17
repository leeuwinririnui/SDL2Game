main: main.cpp
	g++ -I src/Include -I src/header -L src/lib -o main main.cpp -lSDL2 -lSDL2_image

clean:
	rm -f main
