//
// Created by Avery Drennan on 4/27/23.
//

#ifndef CLOTH_SIM_ENVIRONMENT_H
#define CLOTH_SIM_ENVIRONMENT_H
#include <SDL.h>
#include "SDL2_framerate.h"
#include <string>
#include "fluid.h"


typedef class environment {

    SDL_Window* window;
    SDL_Renderer* renderer;
    FPSmanager* fps_manager;
    fluid* fluid_ptr; 
    std::string background;
    int SCREEN_WIDTH; 
    int SCREEN_HEIGHT; 



public: 
    //holds event_data for closing and mouse actions
    struct event_data* event_data;

    //holds the time of the last update for calculating delta_time
    uint32_t last_update_time;

    //base constructor, sets up the null pointers
    environment();

    //initializes the window and renderer to specifications
    void env_init(char* title, int window_width, int window_height, fluid* fluid_ptr);

    //The main while loop to handle program iteration
    void run();

    //destroys the app and frees its pointers
    void close();

    //sets the mouse_pos variable in event data
    void mouse_pos();

private:
    //handles all events each frame
    void event_handler(SDL_Event& event);

    //renders all objects to the screen
    void render();

    //updates the physics
    void update();

    //calculate delta_time
    double calc_dt();

    //renders the background
    void render_background();

    ~environment();

}environment;




#endif //CLOTH_SIM_ENVIRONMENT_H