#pragma once
#include <SDL.h>
#include <utility>
#include <vector>
class fluid
{
private: 
	double density;

	double cell_height; 

	std:: vector<double>* u; 
	std::vector<double>* v;
	std::vector<double>* new_u;
	std::vector<double>* new_v;
	


	std::vector<double>* s; 
	std::vector<double>* new_m;
	void render_smoke(SDL_Renderer* renderer, int SCREEN_HEIGHT, int SCREEN_WIDTH);
	void render_grid(SDL_Renderer* renderer, int SCREEN_HEIGHT, int SCREEN_WIDTH);

public: 
	std::vector<double>* m;
	int num_x;
	int num_y;
	int num_cells = num_x * num_y;
	int center_x;
	int center_y;


	fluid(double density, int num_x, int num_y, double h); 
	void integrate(double dt,double gravity);
	void project(double iters, double dt); 
    int IX(int x, int y); 
	
	void extrapolate(); 
	
	void render(SDL_Renderer* renderer, int SCREEN_HEIGHT, int SCREEN_WIDTH);

	void advect_smoke(double dt);

	void advect_vel(double dt); 

	double avg_u(int i, int j); 
	double avg_v(int i, int j);

	double sample_field(double x, double y, int field); 
	void add_smoke(); 
	void create_circle(int x, int y); 
	double distance(double x1, double y1,double x2, double y2); 
	void set_boundaries();
	void smoke_collision(); 
};