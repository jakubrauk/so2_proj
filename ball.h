#include <random>
#include <future>


class Ball {
    public:
        double bx = 0.0, by = 0.0;
        double  vx = 0.0, vy = 0.0;
        int bounces = 0;
        int speed = 15;
        bool run;
        int max_bounces = 6;
        bool exists = true;
        char ball_ch;

        Ball(int start_x, int start_y, int max_bounces_, int initial_speed, bool *run_);

        char random_ball_char();
        void move_ball();
        void thread_func();
        std::thread spawn() {
            return std::thread ( [this] {this->thread_func(); } );
        }
};
