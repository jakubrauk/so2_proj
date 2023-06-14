#include <ncurses.h>
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
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

int can_enter_a = 0;
int can_enter_b = 0;
int can_enter_c = 0;

std::mutex var_access_mutex;
std::mutex a_mutex;
std::mutex b_mutex;
std::mutex c_mutex;

std::condition_variable a_zone_cv;
std::condition_variable b_zone_cv;
std::condition_variable c_zone_cv;


void ballThreadFunc(Ball *ball) {
	while (ball->exists) {
		if (!keep_going.load()) {
			break;
		}
		int current_zone = ball->get_zone_number();
		int next_zone = ball->get_next_zone_number();

		// UNLOCKING - only when changing zones
		if (ball->is_blocking && current_zone != next_zone) {
			var_access_mutex.lock();
			if (current_zone == 1 && next_zone == 2) {
				// A -> B
				// A --
				// can_enter_a.fetch_sub(1, std::memory_order_relaxed);
				can_enter_a--;
				a_zone_cv.notify_all();
			}
			else if (current_zone == 2 && next_zone == 1) {
				// B -> A
				// B --
				// can_enter_b.fetch_sub(1, std::memory_order_relaxed);
				can_enter_b--;
				b_zone_cv.notify_all();
			}
			else if (current_zone == 2 && next_zone == 3) {
				// B -> C
				// B --
				// can_enter_b.fetch_sub(1, std::memory_order_relaxed);
				can_enter_b--;
				b_zone_cv.notify_all();
			}
			else if (current_zone == 3 && next_zone == 2) {
				// C -> B
				// C --
				// can_enter_c.fetch_sub(1, std::memory_order_relaxed);
				can_enter_c--;
				c_zone_cv.notify_all();
			}
			var_access_mutex.unlock();
		}

		// only when changing zones
		if (current_zone != next_zone) {
			if (current_zone == 1 && next_zone == 2) {
				// A -> B
				std::unique_lock<std::mutex> lock_b(b_mutex);
				b_zone_cv.wait(lock_b, []() { return can_enter_b == 0; });
			}
			else if (current_zone == 2 && next_zone == 1) {
				// B -> A
				std::unique_lock<std::mutex> lock_a(a_mutex);
				a_zone_cv.wait(lock_a, []() { return can_enter_a == 0; });
			}
			else if (current_zone == 2 && next_zone == 3) {
				// B -> C
				std::unique_lock<std::mutex> lock_c(c_mutex);
				c_zone_cv.wait(lock_c, []() { return can_enter_c == 0; });
			}
			else if (current_zone == 3 && next_zone == 2) {
				// C -> B
				std::unique_lock<std::mutex> lock_b(b_mutex);
				b_zone_cv.wait(lock_b, []() { return can_enter_b == 0; });
			}
		}

		ball->move_ball();

		// LOCKING - only when changing zones
		var_access_mutex.lock();
		if (ball->exists && ball->is_blocking && keep_going.load()) {
			if (current_zone != next_zone) {
				if (current_zone == 1 && next_zone == 2) {
					// A -> B
					// B ++
					// can_enter_b.fetch_add(1, std::memory_order_relaxed);
					can_enter_b++;
				}
				else if (current_zone == 2 && next_zone == 1) {
					// B -> A
					// A ++
					// can_enter_a.fetch_add(1, std::memory_order_relaxed);
					can_enter_a++;
				}
				else if (current_zone == 2 && next_zone == 3) {
					// B -> C
					// C ++
					// can_enter_c.fetch_add(1, std::memory_order_relaxed);
					can_enter_c++;
				}
				else if (current_zone == 3 && next_zone == 2) {
					// C -> B
					// B ++
					// can_enter_b.fetch_add(1, std::memory_order_relaxed);
					can_enter_b++;
				}
			}
		} else if (ball->is_blocking) {
			if (current_zone == 1) {
				// can_enter_a.fetch_sub(1, std::memory_order_relaxed);
				can_enter_a--;
				a_zone_cv.notify_all();
			} else if (current_zone == 2) {
				// can_enter_b.fetch_sub(1, std::memory_order_relaxed);
				can_enter_b--;
				b_zone_cv.notify_all();
			} else if (current_zone == 3) {
				// can_enter_c.fetch_sub(1, std::memory_order_relaxed);
				can_enter_c--;
				c_zone_cv.notify_all();
			}
		}
		var_access_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(ball->speed));
	}
}


void scanKey()
{
	while(keep_going.load()) {
		int ch = getch();
		if (ch == ' ') {
			keep_going.store(false);
			can_enter_a = 0;
			can_enter_b = 0;
			can_enter_c = 0;
			a_zone_cv.notify_all();
			b_zone_cv.notify_all();
			c_zone_cv.notify_all();
			// var_access_mutex.unlock();
			break;
		}
	}
}


void generate_balls() {
	while(keep_going.load()) {
		// new ball declaration
		Ball *new_ball = new Ball(15, 30, 6, 120, &running);

		// pushing ball to balls vector
		balls_vector_ptr->push_back(new_ball);
		
		int new_ball_zone = new_ball->get_zone_number();

		var_access_mutex.lock();
		if (new_ball->is_blocking) {
			if (new_ball_zone == 1) {
				// can_enter_a.fetch_add(1, std::memory_order_relaxed);
				can_enter_a++;
			} else if (new_ball_zone == 2) {
				// can_enter_b.fetch_sub(1, std::memory_order_relaxed);
				can_enter_b++;
			} else if (new_ball_zone == 3) {
				// can_enter_c.fetch_sub(1, std::memory_order_relaxed);
				can_enter_c++;
			}
		}
		var_access_mutex.unlock();

		// initialize and run ball thread
		threads_vector.push_back(std::thread(ballThreadFunc, new_ball));
		std::random_device rd;
		std::mt19937 mt{rd()};
		std::uniform_int_distribution<int> ui(5, 7);
		std::this_thread::sleep_for(std::chrono::seconds(ui(mt)));
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

	MyWindows windows = MyWindows(&can_enter_a, &can_enter_b, &can_enter_c);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
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
