main:
	g++ -Isrc/Include -Isrc/header -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

clean:
	del -f main.exe