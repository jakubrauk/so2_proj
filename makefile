all:
	g++ ./main.cpp ./myWindows.cpp ./ball.cpp -o ./main -lncurses -lpthread

clean:
	rm ./main
