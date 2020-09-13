#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "lib/gfx.h"

#define NB_COLORS 5
#define NB_CARDINAL 8
#define MIN_WEIGHT_INIT 5
#define MAX_WEIGHT_INIT 8


typedef enum{
    NORD_OUEST,
    NORD,
    NORD_EST,
    EST,
    SUD_EST,
    SUD,
    SUD_OUEST,
    OUEST
} cardinal_points_t;

typedef enum{
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    WHITE = 3,
    YELLOW = 4
} color_t;

typedef struct{
    int pos_x;
    int pos_y;
} position_t;

typedef struct{
    bool alive;
    color_t color;
    cardinal_points_t direction;
    position_t pos;
    int weight;
} cell_t;

typedef struct{
    bool alive;
    color_t color;
    position_t pos;
} particle_t;

typedef struct{
    int width;
    int height;
    int max_particles;
    int max_threads;
    int max_cells;
    float dir;
    float res;
    float nf;
    bool *playing;
} map_t;

typedef struct{
    cell_t *cells;
    particle_t *food;
    stack_t **stack;
    pthread_barrier_t *barrier_collision;
    pthread_mutex_t *lock;
} collisions_t;

typedef struct{
    int id;
    bool last_worker;
    map_t map;
    collisions_t *collisions;
    unsigned int seed;
    pthread_barrier_t *barrier_worker;
} worker_t;

typedef struct{
    map_t map;
    collisions_t *collisions;
    unsigned int freq;
    struct gfx_context_t *ctxt;
} collisioner_t;

particle_t init_particle(const int max_particles, particle_t *food, const int width, const int height, unsigned int *seed);
cell_t init_cell(const int width, const int height, unsigned int *seed);