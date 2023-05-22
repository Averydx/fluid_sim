#include "fluid.h"
#include <cstdio>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



fluid::fluid(double density, int num_x, int num_y, double h)
{
	srand((int)time(NULL));
	this->density = density; 
	this->num_x = num_x; 
	this->num_y = num_y; 
	this->num_cells = num_x * num_y; 
	this->cell_height = h; 
	this->center_x = num_x / 2; 
	this->center_y = num_y / 2; 

	u = new std::vector<double>(num_cells); 
	v = new std::vector<double>(num_cells); 
	new_u = new std::vector<double>(num_cells);
	new_v = new std::vector<double>(num_cells);
	m = new std::vector<double>(num_cells);
	s = new std::vector<double>(num_cells); 
	new_m = new std::vector<double>(num_cells);

	

	for (int i = 0; i < num_cells;i++)
	{
		m->at(i) = 0; 
		s->at(i) = 1; 
		double d_1 = (double)(rand() % 10);
		this->u->at(i) += d_1;
		double d_2 = (double)(rand() % 10);	
		this->v->at(i) += d_2;


	}

	

}

void fluid::integrate(double dt,double gravity)
{

	for (int i = 0; i < num_cells; i++)
	{
		if (this->s->at(i) != 0)
		{
			this->v->at(i) += gravity * dt;
		}

	}
}

void fluid::project(double iters, double dt)
{
	int n = num_y; 

	for (int i = 0; i < iters; i++)
	{
		for (int x = 0; x < this->num_x; x++)
		{
			for (int y = 0; y < this->num_y; y++)
			{
				
				if (s->at(x * n + y) == 0)
					continue;

				double s = this->s->at((x + 1) * n + y) +this->s->at((x-1) * n + y) + this->s->at(x * n + (y + 1)) + this->s->at(x * n + (y-1));

				if (s == 0)
					continue;


				double div = this->u->at((x + 1) * n + y) - this->u->at(x * n + y) + this->v->at(x * n + (y + 1)) - this->v->at(x * n + y);
				double p = -1 * div / s; 
				p *= 1.9; 

				this->u->at(x * n + y) -= this->s->at((x - 1) * n + y) * p;
				this->u->at((x+1) * n + y) += this->s->at((x + 1) * n + y) * p;
				this->v->at(x * n + y) -= this->s->at(x * n + (y-1)) * p;
				this->v->at(x * n + (y+1)) += this->s->at(x * n + (y + 1)) * p;
			}
		}
	}
}

int fluid::IX(int x, int y)
{
	return x * num_x + y; 
}

void fluid::render_grid(SDL_Renderer* renderer,int SCREEN_HEIGHT, int SCREEN_WIDTH)
{
	
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0xff, SDL_ALPHA_OPAQUE);
	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			if (this->s->at(IX(x, y)) == 0)
			{
				SDL_Rect* rect = (SDL_Rect*)malloc(sizeof(*rect)); 
				rect->x = x * SCREEN_WIDTH / num_x;
				rect->y = y * SCREEN_HEIGHT / num_y;
				rect->w = SCREEN_WIDTH / num_x;
				rect->h = SCREEN_HEIGHT / num_y;
				SDL_RenderFillRect(renderer,rect);
				free(rect); 
				
			}
		}
	}

}

void fluid::extrapolate()
{ 
	int n = num_y;
	for (int i = 0; i < num_x; i++) {
		u->at(i * n) = u->at(i * n + 1);
		u->at(i * n + this->num_y - 1) = this->u->at(i * n + this->num_y - 2);
		}
	for (int j = 0; j < num_y; j++) {
		v->at(0 * n + j) = v->at(1 * n + j);
		this->v->at((this->num_x - 1) * n + j) = this->v->at((this->num_x - 2) * n + j);
	}
}

void fluid::render(SDL_Renderer* renderer, int SCREEN_HEIGHT, int SCREEN_WIDTH)
{
	render_smoke(renderer, SCREEN_HEIGHT, SCREEN_WIDTH);
	render_grid(renderer, SCREEN_HEIGHT, SCREEN_WIDTH);
}

void fluid::advect_smoke(double dt)
{
	for (int i = 0; i < m->size(); i++)
		this->new_m->at(i) = this->m->at(i); 

	int n = this->num_y;
	double h = this->cell_height;
	double h2 = 0.5 * h;

	for (int i = 1; i < this->num_x; i++) {
		for (int j = 1; j < this->num_y; j++) {

			if (this->s->at(i * n + j) != 0.0) {
				double u_val = (this->u->at(i * n + j) + this->u->at((i + 1) * n + j)) * 0.5;
				double v_val = (this->v->at(i * n + j) + this->v->at(i * n + j + 1)) * 0.5;
				double x = i * h + h2 - dt * u_val;
				double y = j * h + h2 - dt * v_val;

				this->new_m->at(i * n + j) = this->sample_field(x, y, 2);
			}
		}
	}

	for (int i = 0; i < m->size(); i++)
		this->m->at(i) = this->new_m->at(i);

}

void fluid::advect_vel(double dt)
{
	for (int i = 0; i < this->v->size(); i++)
	{
		new_u->at(i) = u->at(i); 
		new_v->at(i) = v->at(i);
	}

	int n = num_y; 
	double h2 = 0.5 * cell_height;

	for (int i = 1; i < this->num_x; i++) {
		for (int j = 1; j < this->num_y; j++) 
		{
			//U components
			if (this->s->at(IX(i, j)) != 0)
			{
				double x = i * cell_height;
				double y = j * cell_height + h2;

				double u_val = this->u->at(i * n + j);
				double v_val = this->avg_v(i, j);

				x = x - dt * u_val;
				y = y - dt * v_val;

				u_val = this->sample_field(x, y, 0);
				this->new_u->at(i * n + j) = u_val;


				//V components
				x = i * cell_height + h2;
				y = j * cell_height;

				v_val = this->v->at(i * n + j);
				u_val = this->avg_u(i, j);

				x = x - dt * u_val;
				y = y - dt * v_val;

				v_val = this->sample_field(x, y, 1);
				this->new_v->at(i * n + j) = v_val;
			}
		}
	}
}

double fluid::avg_u(int i, int j)
{
	int n = this->num_y;
	double u = (this->u->at(i * n + (j - 1)) + this->u->at(i * n + j) +
		this->u->at((i + 1) * n + (j - 1)) + this->u->at((i + 1) * n + j)) * 0.25;
	return u;
}

double fluid::avg_v(int i, int j)
{
	int n = this->num_y;
	double v = this->v->at((i - 1) * n + j) + this->v->at(i * n + j) +
		this->v->at((i - 1) * n + j + 1) + this->v->at(i * n + j + 1) * 0.25;
	return v;
}

double fluid::sample_field(double x, double y, int field)
{
	int n = this->num_y;
	double h1 = 1.0 / cell_height;
	double h2 = 0.5 * cell_height;


	x = std::max(std::min(x, num_x * cell_height), cell_height);
	y = std::max(std::min(y, num_y * cell_height), cell_height);

	double dx = 0.0;
	double dy = 0.0;

	double x0 = 0; 
	double tx = 0; 
	double x1 = 0; 
	double y0 = 0; 
	double ty = 0; 
	double y1 = 0; 
	double sx = 0;
	double sy = 0;
	double val = 0; 


	switch (field) {
	case 0:  
		dy = h2; 
		x0 = std::min(floor((x - dx) * h1), (double)this->num_x - 1);
		tx = ((x - dx) - x0 * cell_height) * h1;
		x1 = std::min(x0 + 1, (double)this->num_x - 1);

		y0 = std::min(floor((y - dy) * h1), (double)this->num_y - 1);
		ty = ((y - dy) - y0 * cell_height) * h1;
		y1 = std::min(y0 + 1, (double)num_y - 1);

		sx = 1.0 - tx;
		sy = 1.0 - ty;

		val = sx * sy * u->at(x0 * n + y0) +
			tx * sy * u->at(x1 * n + y0) +
			tx * ty * u->at(x1 * n + y1) +
			sx * ty * u->at(x0 * n + y1);

		return val;
	case 1: 
		dx = h2; 
		x0 = std::min(floor((x - dx) * h1), (double)this->num_x - 1);
		tx = ((x - dx) - x0 * cell_height) * h1;
		x1 = std::min(x0 + 1, (double)this->num_x - 1);

	    y0 = std::min(floor((y - dy) * h1), (double)this->num_y - 1);
		ty = ((y - dy) - y0 * cell_height) * h1;
		y1 = std::min(y0 + 1, (double)num_y - 1);

		sx = 1.0 - tx;
		sy = 1.0 - ty;

		val = sx * sy * v->at(x0 * n + y0) +
			tx * sy * v->at(x1 * n + y0) +
			tx * ty * v->at(x1 * n + y1) +
			sx * ty * v->at(x0 * n + y1);

		return val;
	case 2: 
		dx = h2; 
		dy = h2; 
		x0 = std::min(floor((x - dx) * h1), (double)this->num_x - 1);
		tx = ((x - dx) - x0 * cell_height) * h1;
		x1 = std::min(x0 + 1, (double)this->num_x - 1);

		y0 = std::min(floor((y - dy) * h1), (double)this->num_y - 1);
		ty = ((y - dy) - y0 * cell_height) * h1;
		y1 = std::min(y0 + 1, (double)num_y - 1);

		sx = 1.0 - tx;
		sy = 1.0 - ty;

		val = sx * sy * m->at(x0 * n + y0) +
			tx * sy * m->at(x1 * n + y0) +
			tx * ty * m->at(x1 * n + y1) +
			sx * ty * m->at(x0 * n + y1);
		return val;
	}

}

void fluid::add_smoke()
{

		m->at(IX(num_x / 2 + (rand() % 30),1)) += 1;
		m->at(IX(num_x / 2 - (rand() % 30), 1)) += 1;
}

void fluid::create_circle(int center_x, int center_y)
{

	this->center_x = center_x; 
	this->center_y = center_y; 

	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			if (distance(center_x, center_y, x, y) < 10)
			{
				
				u->at(x * num_y + y) = 0;
				v->at(x * num_y + y) = 0;
			}
		}
	}
}

double fluid::distance(double x1, double y1, double x2, double y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void fluid::set_boundaries()
{

	for (int i = 0; i < num_cells; i++)
	{
		s->at(i) = 1; 
	}

	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			if (distance(center_x, center_y, x, y) < 10)
			{
				s->at(x * num_y + y) = 0;
			}

		}
	}
	for (int i = 0; i < num_x; i++)
	{
		s->at(IX(i, 0)) = 0;

		s->at(IX(i, num_y - 1)) = 0;

		s->at(IX(0, i)) = 0;

		s->at(IX(num_x - 1, i)) = 0;

		
	}

}

void fluid::smoke_collision()
{
	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			if (distance(x, y, center_x, center_y) < 10)
			{
				m->at(IX(x, y)) = 0; 
			}
		}
	}
}


void fluid::render_smoke(SDL_Renderer* renderer, int SCREEN_HEIGHT, int SCREEN_WIDTH)
{
	
	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
				SDL_Rect* rect = (SDL_Rect*)malloc(sizeof(*rect));
				rect->x = x * SCREEN_WIDTH / num_x;
				rect->y = y * SCREEN_HEIGHT / num_y;
				rect->w = SCREEN_WIDTH / num_x;
				rect->h = SCREEN_HEIGHT / num_y;
				SDL_SetRenderDrawColor(renderer,m->at(IX(x,y)) * 255 , 0x0, 0x0, SDL_ALPHA_OPAQUE);
				SDL_RenderFillRect(renderer, rect);
				free(rect);


		}
	}
}

