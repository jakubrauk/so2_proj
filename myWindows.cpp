#include "myWindows.h"

MyWindows::MyWindows(int* can_enter_a, int* can_enter_b, int* can_enter_c) {
    main_window = newwin(30, 90, 0, 0);

    can_a = can_enter_a;
    can_b = can_enter_b;
    can_c = can_enter_c;

    window_a = subwin(main_window, 30, 30, 0, 0);
    window_b = subwin(main_window, 30, 30, 0, 30);
    window_c = subwin(main_window, 30, 30, 0, 60);

    print_all_borders();
}

void MyWindows::print_all_borders() {
    mvwprintw(window_a, 1, 2, "A");
    mvwprintw(window_b, 1, 2, "B");
    box(window_b, 0, 0);
    mvwprintw(window_c, 1, 2, "C");
    box(main_window, 0, 0);
    if (run) {
        run_str = "running";
    } else {
        run_str = "not running";
    }
	mvwprintw(main_window, 0, 1, "Projekt SO2 - Jakub Rauk 256768: %s", run_str.c_str());
    mvwprintw(window_a, 29, 0, "blocked: %d", *can_a);
    mvwprintw(window_b, 29, 0, "blocked: %d", *can_b);
    mvwprintw(window_c, 29, 0, "blocked: %d", *can_c);

    refresh_all_windows();
}

void MyWindows::refresh_all_windows() {
    wrefresh(window_a);
    wrefresh(window_b);
    wrefresh(window_c);
    wrefresh(main_window);
    // refresh();
}

void MyWindows::draw_ball(Ball *ball) {
    if (ball->exists) {
        int x = static_cast<int>(ball->bx);
        int y = static_cast<int>(ball->by);
        if (ball->is_blocking) {
            mvwprintw(main_window, x, y, "%c%d", 'B', ball->get_zone_number());
        }
        else {
            mvwprintw(main_window, x, y, "%c%d", ball->ball_ch, ball->get_zone_number());
        }
    }
}

void MyWindows::draw_balls(std::vector<Ball> *balls) {
    for (int i = 0; i < balls->size(); i++) {
        draw_ball(&balls->at(i));
    }
}

void MyWindows::start_ball_threads() {
    std::thread ball_thread = ball.spawn();
    ball_thread.join();
}

void MyWindows::main_thread() {
    while (run) {
        wclear(main_window);
        print_all_borders();
        draw_ball(&ball);
        refresh_all_windows();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

void MyWindows::set_run(bool true_or_false){
    run = true_or_false;
}


