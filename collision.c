#include "agario.h"
#include <time.h>
#include <unistd.h>

//makes a cell eat a particle
void eat_food(data_stack_t collision, cell_t *cells, particle_t *food, int height){
    particle_t *p = &food[collision.eaten];
    if (p->alive){
        p->alive = false;
        if (cells[collision.eater].weight < height)
            cells[collision.eater].weight += 1;
    }
}

//makes a cell eat another
void eat_cell(data_stack_t collision, cell_t *cells, int height){
    cell_t *eater = &cells[collision.eater];
    cell_t *eaten = &cells[collision.eaten];
    if (eater->alive && eaten->alive){
        if (eater->weight + eaten->weight <= height)
            eater->weight += eaten->weight;
        else
            eater->weight = height;
        
        eaten->alive = false;
    }
}

//pop a collision from the stack and manages it
void pop_collision(collisions_t collisions, int height){
    data_stack_t collision = pop(collisions.stack, collisions.lock);
    if (collision.is_food)
        eat_food(collision, collisions.cells, collisions.food, height);
    else
        eat_cell(collision, collisions.cells, height);
}

//transpose from color_t enum to gfx.h defined color
int transpose_color(color_t color){
    switch (color)
    {
    case RED:
        return COLOR_RED;
        break;
    
    case GREEN:
        return COLOR_GREEN;
        break;
    
    case BLUE:
        return COLOR_BLUE;
        break;
    
    case WHITE:
        return COLOR_WHITE;
        break;
    
    default:
        return COLOR_YELLOW;
        break;
    }
}

//draw the cells and particles
void render(struct gfx_context_t *context, map_t map, collisions_t collisions) {
	gfx_clear(context, COLOR_BLACK);

    for (int i=0; i<map.max_particles; i++){
        if (collisions.food[i].alive){
            gfx_putpixel(context, collisions.food[i].pos.pos_x, collisions.food[i].pos.pos_y, transpose_color(collisions.food[i].color));
        }
    }

    for (int i=0; i<map.max_cells; i++){
        cell_t cell = collisions.cells[i];
        if (cell.alive){
            int color = transpose_color(cell.color);
            gfx_drawcircle(context, cell.pos.pos_x, cell.pos.pos_y, 4 + 6*sqrt(cell.weight), color);
        }
    }
}

//start of collisioner thread
void* collisioner_routine(void* arg) {
    collisioner_t collisioner = *((collisioner_t*) arg);
    map_t map = collisioner.map;
    collisions_t collisions = *collisioner.collisions;
    struct gfx_context_t *ctxt = collisioner.ctxt;

    struct timespec start, finish;

    while (*map.playing) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        barrier_wait(collisions.barrier_collision);

        while(! is_empty(*collisions.stack)){
            pop_collision(collisions, map.height);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &finish);
        double elapsed = (finish.tv_sec - start.tv_sec) * 1e6;
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1e3;
        int time_to_sleep = 1000000/collisioner.freq - elapsed;
        if(time_to_sleep > 0){
            usleep(time_to_sleep);
        }

		render(ctxt, map, collisions);
		gfx_present(ctxt);

        barrier_wait(collisions.barrier_collision);
	}

	gfx_destroy(ctxt);

    return NULL;
}