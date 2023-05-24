#include <ncurses.h>
#include <random>
#include <thread>
#include <signal.h>
#include <iostream>
#include "myWindows.h"

int px = 45;
const int py = 15;
bool animation = false;
bool now_playing = true;
double bx = 0.0, by = 0.0;
double vx = 0.0, vy = 0.0;
char available_chars[] = "axscuore+=";
bool running = true;
int balls_amount = 20;
std::vector<std::thread> threads_vector;
std::vector<Ball> balls_vector;


void ballThreadFunc(Ball *ball) {
	while (ball->run) {
		while (ball->exists) {
			ball->move_ball();
			std::this_thread::sleep_for(std::chrono::milliseconds(ball->speed));
		}
	}
}

// void mainThreadFunc(MyWindows *wins) {
// 	while (wins->run) {
// 		wclear(wins->main_window);
//         wins->print_all_borders();
//         wins->draw_ball(&wins->ball);
//         wins->refresh_all_windows();
//         std::this_thread::sleep_for(std::chrono::milliseconds(15));
// 	}
// }

void scanKey()
{
	while(running) {
		int ch = getch();
		if (ch == ' ') {
			running = false;
			break;
		}
	}
}

void generate_balls() {
	
	// std::mt19937 mt{rd()};
	// std::uniform_int_distribution<int> ui(1, 10);

	for (int i = 0; i < balls_amount; i++) {
		if (running) {
			std::random_device rd;
			std::mt19937 mt{rd()};
			std::uniform_int_distribution<int> ui(1, 6);
			std::this_thread::sleep_for(std::chrono::seconds(ui(mt)));
			threads_vector.push_back(std::thread(ballThreadFunc, &balls_vector[i]));
			threads_vector[i].detach();
		} else {
			break;
		}
	}
}

int main(int argc, char ** argv)
{
	initscr();												// initializes the screen
	noecho();												// no echoing
	curs_set(0);											// set cursor not visible
	signal(SIGWINCH, NULL);

	MyWindows windows = MyWindows();
	windows.print_all_borders();

	for (int i = 0; i < balls_amount; i++) {
		balls_vector.push_back(Ball(15, 30, 6, 80, &running));
	}

	std::thread run_thread (scanKey);
	std::thread detach_threads (generate_balls);

	bool all_finished = false;

	do {
		wclear(windows.main_window);
        windows.print_all_borders();
		all_finished = true;
		for (int i = 0; i < balls_vector.size(); i++) {
			windows.draw_ball(&balls_vector[i]);
			if (all_finished && balls_vector[i].exists) {
				all_finished = false;
			}
		}
		if (all_finished) {
			running = false;
			break;
		};
        windows.refresh_all_windows();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
	} while (running);
	
	detach_threads.detach();
	run_thread.detach();

	threads_vector.clear();
	balls_vector.clear();

	endwin();

	return 0;
}
