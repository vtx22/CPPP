all: compile link

compile:
	g++ -Isfml/include -c main.cpp cppp.cpp -Ofast

link: 
	g++ main.o cppp.o -o main -Lsfml/lib -lsfml-graphics -lsfml-window -lsfml-system 
