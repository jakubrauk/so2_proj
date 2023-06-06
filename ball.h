#include <random>
#include <thread>


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
        bool is_blocking;

        Ball(int start_x, int start_y, int max_bounces_, int initial_speed, bool *run_);
        
        int get_next_y();
        int get_zone_number();
        int get_next_zone_number();
        char random_ball_char();
        void move_ball();
        void thread_func();
        std::thread spawn() {
            return std::thread ( [this] {this->thread_func(); } );
        }
};
