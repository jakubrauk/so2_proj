#include "ball.h"


Ball::Ball(int start_x, int start_y, int max_bounces_, int initial_speed, bool *run_) {
    bx = start_x;
    by = start_y;
	ball_ch = random_ball_char();
    max_bounces = max_bounces_;
    speed = initial_speed;
	bool run = run_;

    std::random_device rd;
	std::mt19937 mt{rd()};
	std::uniform_real_distribution<double> ud(0.0, 1.0);

    double theta = ud(mt) * M_PI;
	vx = cos(theta);
	vy = sin(theta);
}

void Ball::move_ball() {
	int previous_x = bx;
	int previous_y = by;

    bx += vx;
	by += vy;

	if (previous_y <= 29 && by > 29) {
		// A -> B
		speed -= 0.2 * speed;
	}
	if (previous_y >= 29 && by < 29) {
		// B -> A
		speed += 0.2 * speed;
	}
	if (previous_y <= 59 && by > 59) {
		// B -> C
		speed += 0.1 * speed;
	}
	if (previous_y >= 59 && by < 59) {
		// C -> B
		speed -= 0.2 * speed;
	}
	if (speed <= 0 ) {
		speed = 2;
	}

	if (bx < 0) {
		bx = 0;
		vx = -vx;
		bounces++;
	}
	if (by < 0) {
		by = 0;
		vy = -vy;
		bounces++;
	}
	if (bx > 30) {
		bx = 30;
		vx = -vx;
		bounces++;
	}
	if (by > 90) {
		by = 90;
		vy = -vy;
		bounces++;
	}
    if (bounces == max_bounces) {
		exists = false;
		run = false;
	};
}

void Ball::thread_func() {
	while (run) {
		while(exists) {
			move_ball();
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}
	}
	
}

char Ball::random_ball_char() {
	char choices[] = "axscuore+=";
	std::random_device rd;
	std::mt19937 mt{rd()};
	std::uniform_int_distribution<int> ui(0, (sizeof(choices) / sizeof(choices[0])));
	int random_index = ui(mt);
	return choices[random_index];
}
