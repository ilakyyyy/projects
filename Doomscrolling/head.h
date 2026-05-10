#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __SDL__
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define STUDENT_NIP 000000

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1120
#define STRING_LENGTH 256

#define VONMISES_MU 0
#define VONMISES_KAPPA 2

#define STRESS_LEVELS 5

#define BATTERY_LEVELS 5
#define BATTERY_DRAIN_CIRCLE 1.0f

#define DETECTION_RADIUS 150
#define ASKING_RADIUS 20

#define DETECTION_HALF_ANGLE (M_PI / 4.0f)
#define DETECTION_WIDTH 40

#define MAX_CAMPING 5
#define WALLHACK_ALLOWED 1 //0 for false, 1 for true
#define PHASE_ALLOWED 0 //0 for false, 1 for true (will override wallhack setting and act as if it was true)

/* Geometry */
typedef struct {
    float x, y;
} Point;

typedef struct {
    Point start, end;
} Segment;

/* Person */

enum status {SEARCHING, CHASING, ASKING};
enum detection_shape {CIRCLE, CONE};

typedef struct {
    int r, g, b, a; // 0-255
} Color;

typedef struct {
    int questions_asked;
    float distance;
} Log;

typedef struct {
    float current_stress;
    float maximum_stress;
    float stress_searching_rate;
    float stress_chasing_rate;
    float stress_recovery_rate;
    int levels;
} Panic;

typedef struct {
    float current_charge;
    float maximum_charge;
    int levels;
} Battery;

typedef struct {
    enum status status;
    Panic panic;
    Battery battery;

    Log log;
    Color color;

    float step_size;
    float angle;
    float delta_angle;
    float max_step;

    float velocity;
    int radius;

    enum detection_shape detection_shape;
    float detection_half_angle;
    float detection_width;

    Point position;
    Point destination;
    Point direction;

    int target_idx;

    float camping_timer;
} Person;

/* Game */

typedef struct {
    Segment *walls;
    int n_walls;
    Point center;
    Person *professors;
    int n_professors;
} Level;

#ifdef __SDL__
typedef struct {
    SDL_Window *win;
    SDL_Renderer *renderer;
    SDL_Texture *shadow_texture;
    TTF_Font *font;
    Uint64 initialTime;
    bool running;
} Game;
#endif


//**********PARISI RAPUANO*************
#define NormRANu (2.3283063671E-10F)

extern unsigned int irr[256];
extern unsigned int ir1;
extern unsigned char ind_ran,ig1,ig2,ig3;

extern float Random(void);
extern void ini_ran(int SEMILLA);
extern float uniformRandomIn(double min, double max);
extern float vonMises(float pastAngle, float mu, float kappa);
//************************************

/* Game functions */
#ifdef __SDL__
extern bool startGame(Game *game);
extern void checkQuitEvent(Game *game, Level level, Person person, Uint64 lastTime);
extern void renderGame(Game game, Level level, Person person, Uint64 frameStartTime, int fps);
extern float updateTime(Uint64 *lastTime);
extern void exitGame(Game game, Level level);
#endif

/* Person functions */
extern void setUpStudent(Person *student);
extern void setUpProfessors(Level *level);
#ifdef __SDL__
extern void runDynamics(Level *level, Person *student, Uint64 *lastTime, int* hack);
#endif

/* Input/Output functions */
extern void loadData(Level *level);
extern void storeResults(Level level, Person student, int duration);

/* Movements functions */
extern Point newDestination(Person *person);
extern void updateDestination(Person *person, Point destination);
extern bool movePerson(Person *person, Level level, float dt);

/* Geometry functions */
extern Point substractVectors(Point p1, Point p2);
extern float dotProduct(Point p1, Point p2);
extern Point scalarProduct(Point p, float scalar);
extern float norm(Point p);
extern Point unitVector(Point a, Point b);
extern float distancePoints(Point a, Point b);

/* Light functions (detection area) */
extern bool professorInRange(Person student, Point professor_position);
extern void updateDetectionArea(Person *student);

/* Battery functions */
extern void updateBattery(Person *student, float dt);

/* Collision Functions */
extern bool allowedMovement(Point p_start, Point p_end, int radius, Level level);
