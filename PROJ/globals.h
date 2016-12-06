#define NDIM                    3
#define EPS                     1.0e+1
#define DOMAIN_SIZE             1.0e+23
#define MASS_MAX                1.0e+32 /* kg */
#define G                       6.6726e-11

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

typedef struct Particle {
    double pos[NDIM];
    double vel[NDIM];
    double mass;
    
} Particle;

typedef struct Pass_Particle {
    double pos[NDIM];
    double mass;
} Pass_Particle;

typedef enum { X, Y, Z } axis;
typedef enum { INIT_LINEAR, INIT_SPIRAL } initt;
typedef enum { false, true } bool;

#ifdef __MAIN
int                     rank;
int                     np;
int                     my_name_len;
char                    my_name[255];
#else
extern int              rank;
extern int              np;
extern int              my_name_len;
extern char             *my_name;
#endif

//
// Nbody globals
//
#ifdef __MAIN
int                     num_part;
int                     my_num_part;
int                     num_iter;
int                     dt;
int                     img_dim;
int                     img_len;
int                     num_threads;
bool                    writing;
bool                    super_comp;
initt                   init_type;

Particle                *Particles_a;
Particle                *Particles_b;
Pass_Particle           *Particles_pass_a;
Pass_Particle           *Particles_pass_b;
Particle                *Particles_out;
char                    *out_buffer;

#else  
extern int              num_part;
extern int              my_num_part;
extern int              num_iter;
extern int              dt;
extern int              img_dim;
extern int              img_len;
extern int              num_threads;
extern bool             writing;
extern bool             super_comp;
extern initt            init_type;

extern Particle         *Particles_a;
extern Particle         *Particles_b;
extern Pass_Particle    *Particles_pass_a;
extern Pass_Particle    *Particles_pass_b;
extern Particle         *Particles_out;
extern char             *out_buffer;

#endif
