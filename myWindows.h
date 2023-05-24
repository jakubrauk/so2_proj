#include <ncurses.h>
#include "ball.h"

class MyWindows {
    public:

        WINDOW *main_window = NULL;
        WINDOW *window_a = NULL;
        WINDOW *window_b = NULL;
        WINDOW *window_c = NULL;
        bool run = true;
        Ball ball = Ball(15, 30, 6, 15, &run);
        std::string run_str;

        MyWindows(); // initializes all windows

        void print_all_borders();
        void refresh_all_windows();
        void draw_ball(Ball *ball);
        void draw_balls(std::vector<Ball> *balls);
        void start_ball_threads();
        void main_thread();
        void set_run(bool true_or_false);
        std::thread spawn() {
            return std::thread ( [this] {this->main_thread(); } );
        };
};
