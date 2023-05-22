//
// Created by Avery Drennan on 4/27/23.
//

#include "environment.h"
#include "event_data.h"

#define TICK_INTERVAL    50

static Uint32 next_time;

Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if (next_time <= now)
        return 0;
    else
        return next_time - now;
}

void environment::env_init(char* title, int window_width, int window_height,fluid* fluid_ptr)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }


    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        window_width, window_height, SDL_WINDOW_SHOWN);

    if (!window)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    this->fps_manager = static_cast<FPSmanager*>(malloc(sizeof(*fps_manager)));
    SDL_setFramerate(this->fps_manager, 100);

    this->fluid_ptr = fluid_ptr; 

    this->last_update_time = SDL_GetTicks();

    int* w = (int*)malloc(sizeof(*w));
    int* h = (int*)malloc(sizeof(*h));
    SDL_GetWindowSize(window, w, h);

    this->SCREEN_HEIGHT = *h;
    this->SCREEN_WIDTH = *w;

    if (w != nullptr && h != nullptr)
    {
        delete w;
        delete h;
    }

}

environment::environment()
{
    window = nullptr;
    renderer = nullptr;
    this->event_data = new struct event_data();

}

void environment::run()
{
    SDL_Event event;
    next_time = SDL_GetTicks() + TICK_INTERVAL;
    while (!this->event_data->close)
    {
        
        this->event_handler(event);
        this->update();
        this->render();
        
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;

        
        
    }
}

void environment::event_handler(SDL_Event& event)
{
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            this->event_data->close = true;
        }

        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (SDL_BUTTON_LEFT == event.button.button) {
                event_data->left_click = true;
            }

            if (SDL_BUTTON_RIGHT == event.button.button)
                event_data->right_click = true;

        }

        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                event_data->left_click = false;

            else if (event.button.button == SDL_BUTTON_RIGHT)
                event_data->right_click = false;

        }
        else if (event_data->left_click && event.type == SDL_MOUSEMOTION)
        {
            printf("(%f,%f)\n", event_data->mouse_pos.first, event_data->mouse_pos.second);
            
            int grid_pos_x = (event_data->mouse_pos.first / SCREEN_WIDTH) * fluid_ptr->num_x;
            int grid_pos_y = (event_data->mouse_pos.second / SCREEN_HEIGHT) * fluid_ptr->num_y;


            fluid_ptr->m->at(fluid_ptr->IX(grid_pos_x, grid_pos_y)) += 1; 
        }

        else if (event_data->right_click && event.type == SDL_MOUSEMOTION)
        {
            printf("(%f,%f)\n", event_data->mouse_pos.first, event_data->mouse_pos.second);

            int grid_pos_x = (event_data->mouse_pos.first / SCREEN_WIDTH) * fluid_ptr->num_x;
            int grid_pos_y = (event_data->mouse_pos.second / SCREEN_HEIGHT) * fluid_ptr->num_y;


            if (fluid_ptr->distance(grid_pos_x, grid_pos_y, fluid_ptr->center_x, fluid_ptr->center_y) < 10)
            {
                fluid_ptr->center_x = grid_pos_x; 
                fluid_ptr->center_y = grid_pos_y;
            }
        }



      
       
    }
}

void environment::render()
{
    SDL_SetRenderDrawColor(this->renderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE);
    SDL_RenderSetViewport(this->renderer, nullptr);
    SDL_RenderClear(this->renderer);

    

    fluid_ptr->render(renderer, SCREEN_HEIGHT, SCREEN_WIDTH); 
    SDL_RenderPresent(renderer);

}

void environment::update() {
    double dt = calc_dt();
    this->mouse_pos();
    fluid_ptr->create_circle(fluid_ptr->center_x, fluid_ptr->center_y);
    fluid_ptr->set_boundaries(); 
    fluid_ptr->add_smoke();
    fluid_ptr->smoke_collision();
    fluid_ptr->integrate(dt, 1);
    fluid_ptr->project(5, dt); 
    fluid_ptr->extrapolate(); 
    fluid_ptr->advect_vel(dt);
    fluid_ptr->advect_smoke(dt);

}

void environment::close() {
    delete(this);
}

environment::~environment() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    delete(this->event_data);
    free(this->fps_manager); 
}

double environment::calc_dt() {
    uint32_t current_time = SDL_GetTicks();
    double delta_time = (current_time - last_update_time) / 1000.f;
    this->last_update_time = current_time;
    return delta_time;
}

void environment::mouse_pos()
{
    this->event_data->mouse_prev_pos.first = this->event_data->mouse_pos.first;
    this->event_data->mouse_prev_pos.second = this->event_data->mouse_pos.second;

    int x, y;
    SDL_GetMouseState((&x), (&y));
    this->event_data->mouse_pos.first = x;
    this->event_data->mouse_pos.second = y;



}
