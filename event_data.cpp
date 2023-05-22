//
// Created by Avery Drennan on 4/27/23.
//

#include "event_data.h"

event_data::event_data() {

    close = false;
    mouse_pos = std::make_pair(0.f, 0.f);
    mouse_prev_pos = std::make_pair(0.f, 0.f);
    right_click = false;
    left_click = false;

}