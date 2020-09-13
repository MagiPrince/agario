#include "agario.h"
#include "worker.h"
#include "collision.h"
#include "keyboard.h"

//returns a new cell
cell_t init_cell(const int width, const int height, unsigned int *seed){
    cell_t cell;
    cell.color = rand_r(seed)%NB_COLORS;
    cell.direction = rand_r(seed)%NB_CARDINAL;
    cell.weight = MIN_WEIGHT_INIT + rand_r(seed)%(MAX_WEIGHT_INIT-MIN_WEIGHT_INIT);
    cell.pos.pos_x = rand_r(seed)%width;
    cell.pos.pos_y = rand_r(seed)%height;
    cell.alive = true;
    return cell;
}

//returns the entire cell array of agario
cell_t *init_cells(const int max_cells, const int width, const int height, unsigned int *seed){
    cell_t *cells = malloc(sizeof(cell_t) * max_cells);

    for (int i=0;i<max_cells;i++){
        cells[i] = init_cell(width,height, seed);
    }

    return cells;
}

//returns if two positions are equals
bool equalpos(position_t t1, position_t t2){
    return t1.pos_x == t2.pos_x && t1.pos_y == t2.pos_x;
}

//returns if the position is already taken by a particle that is alive
bool existant_pos(position_t pos, const int max_particles, particle_t *food){
    for (int i=0;i<max_particles;i++){
        if (food[i].alive){
            if (equalpos(food[i].pos, pos)){
                return true;
            }
        }
    }
    return false;
}

//returns a new particle of food
particle_t init_particle(const int max_particles, particle_t *food, const int width, const int height, unsigned int *seed){
    particle_t particle;

    particle.color = rand_r(seed)%NB_COLORS;
    
    do {
        particle.pos.pos_x = rand_r(seed)%width;
        particle.pos.pos_y = rand_r(seed)%height;
    } while(existant_pos(particle.pos, max_particles, food));

    particle.alive = true;
    return particle;
}

//returns the entire food particles array of agario
particle_t *init_food(const int max_particles, const int width, const int height, unsigned int *seed){
    particle_t *food = malloc(sizeof(particle_t) * max_particles);
    
    for (int i=0;i<max_particles;i++){
        food[i].alive = false;
    }

    for (int i=0;i<max_particles;i++){
        food[i] = init_particle(max_particles, food, width, height, seed);
    }

    return food;
}

//start of program
int main(int argc, char *argv[])
{
    if (argc != 11){
        printf("agario <width> <height> <seed> <food> <dir> <res> <nf> <freq> <workers> <cells>\n");
        return EXIT_SUCCESS;
    }

    bool playing = true;

    map_t map;
    collisioner_t *collisioner = malloc(sizeof(collisioner_t));

    map.width = atoi(argv[1]);
    map.height = atoi(argv[2]);
    unsigned int seed = atoi(argv[3]);
    const float food_rate = atof(argv[4]);
    map.dir = atof(argv[5]);
    map.res = atof(argv[6]);
    map.nf = atof(argv[7]);
    collisioner->freq = atoi(argv[8]);
    map.max_threads = atoi(argv[9]);
    map.max_cells = atoi(argv[10]);
    map.playing = &playing;

    map.max_particles = (int) map.width*map.height*food_rate;

    collisions_t *collisions = malloc(sizeof(collisions_t));

    collisions->cells = init_cells(map.max_cells, map.width, map.height, &seed);
    collisions->food = init_food(map.max_particles, map.width, map.height, &seed);
    collisions->stack = (stack_t**) malloc(sizeof(stack_t*));
    *(collisions->stack) = init_stack();

    collisions->lock = malloc(sizeof(pthread_mutex_t));
    init_mutex(collisions->lock);

    pthread_barrier_t barrier_collision;
    barrier_init(&barrier_collision, NULL, map.max_threads+1);
    collisions->barrier_collision = &barrier_collision;

    worker_t worker[map.max_threads];
    pthread_t thread_worker[map.max_threads];

    pthread_barrier_t barrier_worker;
    barrier_init(&barrier_worker, NULL, map.max_threads);

    for (int i = 0; i < map.max_threads; i++)
    {
        worker[i].id = i;
        worker[i].collisions = collisions;
        worker[i].map = map;
        worker[i].seed = seed+i+1;
        worker[i].barrier_worker = &barrier_worker;
        
        if(i == map.max_threads-1){
            worker[i].last_worker = true;
        }
        else{
            worker[i].last_worker = false;
        }

        create_thread(&thread_worker[i], NULL, worker_routine, &worker[i]);
    }

    collisioner->map = map;
    collisioner->collisions = collisions;

    struct gfx_context_t *ctxt = gfx_create("Agar.io", map.width, map.height);
    if (!ctxt) {
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_SUCCESS;
	}
    collisioner->ctxt = ctxt;

    pthread_t thread_collisioner;
    create_thread(&thread_collisioner, NULL, collisioner_routine, collisioner);

    pthread_t thread_keyboard;
    create_thread(&thread_keyboard, NULL, keyboard_routine, &playing);

    join_thread(thread_collisioner, NULL);

    for (int i = 0; i < map.max_threads; i++)
    {
        join_thread(thread_worker[i], NULL);
    }

    join_thread(thread_keyboard, NULL);

    pthread_barrier_destroy(&barrier_worker);
    pthread_barrier_destroy(&barrier_collision);
    pthread_mutex_destroy(collisions->lock);

    free(collisions->lock);
    free(collisions->food);
    free(collisions->cells);
    free((*collisions->stack)->next);
    free(*collisions->stack);
    free(collisions->stack);

    free(collisions);
    free(collisioner);
    return EXIT_SUCCESS;
}
