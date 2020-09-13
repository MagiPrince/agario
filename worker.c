#include "agario.h"
#include "worker.h"

//returns the radius of a cell
int radius_calc(int weight){
    return 4 + 6*sqrt(weight);
}

//returns the speed of a cell
double speed_calc(int weight){
    return ALPHA + BETA/(1+exp((double)GAMMA*(weight-DELTA)));
}

//returns if a cell will hit the right side
bool hits_right(int width, position_t pos){
    return pos.pos_x >= width;
}

//returns if a cell will hit the left side
bool hits_left(position_t pos){
    return pos.pos_x < 0;
}

//returns if a cell will hit the up side
bool hits_up(position_t pos){
    return pos.pos_y < 0;
}

//returns if a cell will hit the down side
bool hits_down(int height, position_t pos){
    return pos.pos_y >= height;
}

//returns if a cell will hit a border
bool hits_border(int width, int height, position_t pos){
    return hits_right(width, pos) || hits_left(pos) || hits_up(pos) || hits_down(height, pos);
}

//changes the direction of a cell
void change_direction(int width, position_t pos, cell_t array_cell[], int i){
    if (hits_right(width, pos)){
        switch(array_cell[i].direction){
            case NORD_EST:
                array_cell[i].direction = NORD_OUEST;
                break;
            case EST:
                array_cell[i].direction = OUEST;
                break;
            case SUD_EST:
                array_cell[i].direction = SUD_OUEST;
                break;
            default:
                printf("WTF hits right wrong direction\n");
        }
    }
    else if (hits_left(pos)){
        switch(array_cell[i].direction){
            case NORD_OUEST:
                array_cell[i].direction = NORD_EST;
                break;
            case OUEST:
                array_cell[i].direction = EST;
                break;
            case SUD_OUEST:
                array_cell[i].direction = SUD_EST;
                break;
            default:
                printf("WTF hits left wrong direction\n");
        }
    }
    else if (hits_up(pos)){
        switch(array_cell[i].direction){
            case NORD_EST:
                array_cell[i].direction = SUD_EST;
                break;
            case NORD:
                array_cell[i].direction = SUD;
                break;
            case NORD_OUEST:
                array_cell[i].direction = SUD_OUEST;
                break;
            default:
                printf("WTF hits up wrong direction\n");
        }
    }
    else{
        switch(array_cell[i].direction){
            case SUD_EST:
                array_cell[i].direction = NORD_EST;
                break;
            case SUD:
                array_cell[i].direction = NORD;
                break;
            case SUD_OUEST:
                array_cell[i].direction = NORD_OUEST;
                break;
            default:
                printf("WTF hits down wrong direction\n");
        }
    }
}

//manages movement of cells
void calcul_next_position(cell_t array_cell[], int from_cell, int to_cell, int width, int height){
    position_t dir[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    
    for(int i = from_cell; i <= to_cell; i++){
        if(array_cell[i].alive){
            int speed = speed_calc(array_cell[i].weight);

            position_t pos;
            pos.pos_x = array_cell[i].pos.pos_x + dir[array_cell[i].direction].pos_x * speed;
            pos.pos_y = array_cell[i].pos.pos_y + dir[array_cell[i].direction].pos_y * speed;

            if(hits_border(width, height, pos)){
                change_direction(width, pos, array_cell, i);
            }
            else{
                array_cell[i].pos.pos_x = pos.pos_x;
                array_cell[i].pos.pos_y = pos.pos_y;
            }
        }
    }
}

//detects and adds collisions to the stack
void detection_collision(cell_t array_cell[], int nb_cells, int from_cell,int to_cell, particle_t array_food[], int size_array_food, stack_t **stack, pthread_mutex_t *lock){
    for(int i = from_cell; i <= to_cell; i++){
        if (array_cell[i].alive){
            //Check collision with the others cells
            int result_rayon = radius_calc(array_cell[i].weight);
            for(int j = i; j < nb_cells; j++){
                if(array_cell[j].alive && i != j){
                    position_t delta;
                    delta.pos_x = array_cell[i].pos.pos_x - array_cell[j].pos.pos_x;
                    delta.pos_y = array_cell[i].pos.pos_y - array_cell[j].pos.pos_y;

                    if (array_cell[i].weight < array_cell[j].weight){
                        result_rayon = radius_calc(array_cell[j].weight);
                    }

                    if(pow(delta.pos_x, 2) + pow(delta.pos_y, 2) <= pow(result_rayon,2)){
                        data_stack_t data;
                        data.is_food = false;
                        if (array_cell[i].weight >= array_cell[j].weight){
                            data.eater = i;
                            data.eaten = j;
                        }
                        else {
                            data.eater = j;
                            data.eaten = i;
                        }
                        push(stack, data, lock);
                    }
                }
            }
            //Check collision with the food
            result_rayon = radius_calc(array_cell[i].weight);
            for(int j = 0; j < size_array_food; j++){
                if(array_food[j].alive){
                    position_t delta;
                    delta.pos_x = array_cell[i].pos.pos_x - array_food[j].pos.pos_x;
                    delta.pos_y = array_cell[i].pos.pos_y - array_food[j].pos.pos_y;

                    if(pow(delta.pos_x, 2) + pow(delta.pos_y, 2) <= pow(result_rayon,2)){
                        data_stack_t data;
                        data.is_food = true;
                        data.eater = i;
                        data.eaten = j;
                        push(stack, data, lock);
                        break;
                    }
                }
            }
        }
    }
}

//revives cells with a probability
void proba_res(cell_t array_cell[], int from_cell, int to_cell, unsigned int *seed, int width, int height, float res){
    for (int i = from_cell; i <= to_cell; i++){
        if(!array_cell[i].alive){
            if(rand_r(seed)%1000 < (int)(res*1000)){
                array_cell[i] = init_cell(width, height, seed);
            }
        }
    }
}

//revives food with a probability
void proba_place_food(particle_t array_food[], unsigned int *seed, int width, int height, int max_particles, float nf){
    for(int i = 0; i < max_particles; i++){
        if(!array_food[i].alive){
            if(rand_r(seed)%1000 < (int)(nf*1000)){
                array_food[i] = init_particle(max_particles, array_food, width, height, seed);
            }
        }
    }
}

//makes cells change of direction with a probability
void proba_change_dir(cell_t array_cell[], int from_cell, int to_cell, unsigned int *seed, float dir){
    for (int i = from_cell; i <= to_cell; i++) {
        if(array_cell[i].alive){
            if(rand_r(seed)%1000 < (int)(dir*1000)){
                array_cell[i].direction = rand_r(seed)%NB_CARDINAL;
            }
        }
    }
}

//start of worker thread
void *worker_routine(void *arg){
    worker_t worker = *((worker_t*) arg);
    int nb_cells_to_manage = 0;

    // Repart "equitably" the cells between the differents threads
    if(!worker.last_worker){
        nb_cells_to_manage = (int)round(worker.map.max_cells / worker.map.max_threads);
    }
    else{
        nb_cells_to_manage = worker.map.max_cells - (int)round(worker.map.max_cells / worker.map.max_threads)*worker.id;
    }

    int from_cell = (!worker.last_worker) ? worker.id*nb_cells_to_manage : worker.map.max_cells-nb_cells_to_manage;
    int to_cell = from_cell+nb_cells_to_manage-1;

    // Repeat the worker routine until the ESC bouton is pressed
    while(*worker.map.playing){

        proba_res(worker.collisions->cells, from_cell, to_cell, &worker.seed, worker.map.width, worker.map.height, worker.map.res);
        
        if (worker.last_worker){
            proba_place_food(worker.collisions->food, &worker.seed, worker.map.width, worker.map.height, worker.map.max_particles, worker.map.nf);
        }

        proba_change_dir(worker.collisions->cells, from_cell, to_cell, &worker.seed, worker.map.dir);

        calcul_next_position(worker.collisions->cells, from_cell, to_cell, worker.map.width, worker.map.height);
        
        barrier_wait(worker.barrier_worker);

        detection_collision(worker.collisions->cells, worker.map.max_cells, from_cell, to_cell, worker.collisions->food, worker.map.max_particles, worker.collisions->stack, worker.collisions->lock);

        barrier_wait(worker.collisions->barrier_collision);

        barrier_wait(worker.collisions->barrier_collision);
    }

    return NULL;
}