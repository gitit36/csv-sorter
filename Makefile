output: main.o bubblesort.o insertsort.o bubble.o
	g++ myhie.o -o myhie
	g++ bubblesort.o bubble.o -o bubble
	g++ insertsort.o -o insert

bubblesort.o: bubblesort.cpp
	g++ -c bubblesort.cpp

insertsort.o: insertsort.cpp
	g++ -c insertsort.cpp

main.o: myhie.cpp
	g++ -c myhie.cpp

bubble.o: bubble.cpp bubble.h
	g++ -c bubble.cpp

clean:
	rm *.o inser
	rm *.o bubble
	rm *.o myhie