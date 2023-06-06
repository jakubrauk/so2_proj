#include <ncurses.h>
#include <random>
#include <thread>
#include <signal.h>
#include <iostream>
#include <atomic>
#include "myWindows.h"

int px = 45;
const int py = 15;
bool animation = false;
bool now_playing = true;
double bx = 0.0, by = 0.0;
double vx = 0.0, vy = 0.0;
char available_chars[] = "axscuore+=";
bool running = true;
static std::atomic<bool> keep_going;
int balls_amount = 20;
std::vector<std::thread> threads_vector;
std::vector<Ball*> balls_vector;
std::vector<Ball*> *balls_vector_ptr = &balls_vector;


void ballThreadFunc(Ball *ball) {
	while (ball->exists) {
		if (!keep_going.load()) {
			break;
		}
		ball->move_ball();
		std::this_thread::sleep_for(std::chrono::milliseconds(ball->speed));
	}
}


void scanKey()
{
	while(keep_going.load()) {
		int ch = getch();
		if (ch == ' ') {
			keep_going.store(false);
			break;
		}
	}
}


void generate_balls() {
	while(keep_going.load()) {
		std::random_device rd;
		std::mt19937 mt{rd()};
		std::uniform_int_distribution<int> ui(0, 6);
		std::this_thread::sleep_for(std::chrono::seconds(ui(mt)));

		// new ball declaration
		Ball *new_ball = new Ball(15, 30, 6, 80, &running);

		// pushing ball to balls vector
		balls_vector_ptr->push_back(new_ball);
		
		// initialize and run ball thread
		threads_vector.push_back(std::thread(ballThreadFunc, new_ball));
	}
}


int main(int argc, char ** argv)
{
	initscr();
	noecho();
	curs_set(0);
	signal(SIGWINCH, NULL);

	// keep_going initialy set to true
	keep_going.store(true);

	MyWindows windows = MyWindows();
	windows.print_all_borders();

	// scan key thread initialization (waiting for space char to end program)
	std::thread scan_key_thread (scanKey);

	// generate balls thread initialization
	std::thread generate_balls_thread (generate_balls);

	wclear(windows.main_window);
	do {
		werase(windows.main_window);
        windows.print_all_borders();
		for (int i = 0; i < balls_vector_ptr->size(); i++) {
			windows.draw_ball(balls_vector_ptr->at(i));
		}
        windows.refresh_all_windows();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
	} while (keep_going.load());
	
	generate_balls_thread.join();

	scan_key_thread.join();

	for (int i = 0; i < threads_vector.size(); i++) {
		threads_vector[i].join();
	}

	threads_vector.clear();
	balls_vector.clear();

	endwin();

	return 0;
}
