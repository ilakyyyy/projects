#ifndef __SDL__
#define __SDL__
#endif

#include "head.h"

/**
 * Start the game by initializing SDL and creating a window and renderer.
 * Returns true if successful, false otherwise.
 */
bool startGame(Game *game) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    game->win = SDL_CreateWindow("SDL Test", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_RESIZABLE);
    if (!game->win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    game->renderer = SDL_CreateRenderer(game->win, NULL);
    if (!game->renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        return false;
    }

    SDL_SetRenderLogicalPresentation(game->renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);


    if (!TTF_Init()) {
        printf("TTF_Init Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        return false;
    }

    game->font = TTF_OpenFont("fonts/cookie.ttf", 144);
    if (!game->font) {
        printf("TTF_OpenFont Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        return false;
    }

    game->shadow_texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!game->shadow_texture) {
        printf("SDL_CreateTexture (shadow) Error: %s\n", SDL_GetError());
        TTF_CloseFont(game->font);
        TTF_Quit();
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        return false;
    }

    game->running = true;
    return true;
}

/**
 * Checks for quit events and stops the game if one is detected.
 */
void checkQuitEvent(Game *game, Level level, Person student, Uint64 lastTime) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            game->running = false;
            printf("Cerramos el juego por evento de cierre.\n");
        }
    }

    if (student.panic.current_stress >= student.panic.maximum_stress * 0.9999f) game->running = false;
    if (level.n_professors == 0) game->running = false;
}

/**
 * Renders the walls of the level.
 */
void renderWalls(Game game, Level level) {
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255); // Black
    for(int i = 0; i < level.n_walls; i++) {
        SDL_RenderLine(game.renderer, level.walls[i].start.x, level.walls[i].start.y, level.walls[i].end.x, level.walls[i].end.y);
    }
}

/**
 * Renders the person at a given position with a specified radius.
 */
void renderPerson(Game game, Person person) {
    Point p = person.position;

    SDL_SetRenderDrawColor(game.renderer, person.color.r, person.color.g, person.color.b, person.color.a);
    for(float y = -person.radius; y <= person.radius; y+=0.1) {
        float dx = sqrt(person.radius * person.radius - y * y);
        SDL_RenderLine(game.renderer, p.x - dx, p.y + y, p.x + dx, p.y + y);
    }
}

/**
 * Draws a panic meter at a specified position with a given level.
 */
void renderPanic(SDL_Renderer *renderer, int level) {
    Point panic_position = { .x = 480, .y = 992 };
    int panic_width = 180;
    int panic_height = 70;
    int terminal_width = 10;
    int terminal_height = 25;
    int bar_padding = 4;

    // Ensure level is between 0 and STRESS_LEVELS
    if (level < 0) level = 0;
    if (level > STRESS_LEVELS) level = STRESS_LEVELS;

    // Panic Outline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_FRect panic_rect = {panic_position.x, panic_position.y, panic_width, panic_height};
    SDL_RenderRect(renderer, &panic_rect);

    // Battery Terminal
    SDL_FRect terminal_rect = {panic_position.x + panic_width, panic_position.y + panic_height / 3, terminal_width, terminal_height};
    SDL_RenderFillRect(renderer, &terminal_rect);

    // Battery Levels
    float bar_width = (panic_width - (STRESS_LEVELS + 1) * bar_padding) / STRESS_LEVELS;
    float bar_height = panic_height - 2 * bar_padding;

    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);  // Blue
    for (int i = 0; i < level; i++) {
        SDL_FRect bar_rect = {panic_position.x + bar_padding + i * (bar_padding + bar_width), panic_position.y + bar_padding, bar_width, bar_height};
        SDL_RenderFillRect(renderer, &bar_rect);
    }
}

/**
 * Draws the phone battery meter at a specified position, draining left-to-right.
 */
void renderBattery(SDL_Renderer *renderer, int level) {
    Point bat_position = { .x = 700, .y = 992 };
    int bat_width = 180;
    int bat_height = 70;
    int terminal_width = 10;
    int terminal_height = 25;
    int bar_padding = 4;

    if (level < 0) level = 0;
    if (level > BATTERY_LEVELS) level = BATTERY_LEVELS;

    // Outline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_FRect bat_rect = {bat_position.x, bat_position.y, bat_width, bat_height};
    SDL_RenderRect(renderer, &bat_rect);

    // Terminal
    SDL_FRect terminal_rect = {bat_position.x + bat_width, bat_position.y + bat_height / 3, terminal_width, terminal_height};
    SDL_RenderFillRect(renderer, &terminal_rect);

    // Bars (blue)
    float bar_width = (bat_width - (BATTERY_LEVELS + 1) * bar_padding) / BATTERY_LEVELS;
    float bar_height = bat_height - 2 * bar_padding;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
    for (int i = 0; i < level; i++) {
        SDL_FRect bar_rect = {bat_position.x + bar_padding + i * (bar_padding + bar_width), bat_position.y + bar_padding, bar_width, bar_height};
        SDL_RenderFillRect(renderer, &bar_rect);
    }
}

/**
 * Renders the text on the screen.
 */
void renderTexts(Game game, Level level, Person student, int fps) {
    char text[STRING_LENGTH];
    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    SDL_Color black = {255, 255, 255, 255};  // White color
    SDL_FRect status_rect = {610, 880, 400, 50};
    SDL_FRect panic_rect  = {480, 1072, 180, 35};
    SDL_FRect battery_rect = {700, 1072, 180, 35};
    SDL_FRect proff_rect  = {950, 1000, 250, 50};
    SDL_FRect fps_rect = {750, 800, 100, 50};

    sprintf(text, "Student status: %s",
        student.status == SEARCHING ? "SEARCHING" :
         (student.status == CHASING ? "CHASING" : "ASKING")
    );

    textSurface = TTF_RenderText_Solid(game.font, text, 0, black);
    textTexture = SDL_CreateTextureFromSurface(game.renderer, textSurface);

    SDL_RenderTexture(game.renderer, textTexture, NULL, &status_rect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    sprintf(text, "Stress: %d%%", (int) student.panic.current_stress);

    textSurface = TTF_RenderText_Solid(game.font, text, 0, black);
    textTexture = SDL_CreateTextureFromSurface(game.renderer, textSurface);

    SDL_RenderTexture(game.renderer, textTexture, NULL, &panic_rect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    sprintf(text, "Battery: %d%%", (int)(student.battery.current_charge));

    textSurface = TTF_RenderText_Solid(game.font, text, 0, black);
    textTexture = SDL_CreateTextureFromSurface(game.renderer, textSurface);

    SDL_RenderTexture(game.renderer, textTexture, NULL, &battery_rect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    sprintf(text, "Professors left: %d", level.n_professors);

    textSurface = TTF_RenderText_Solid(game.font, text, 0, black);
    textTexture = SDL_CreateTextureFromSurface(game.renderer, textSurface);

    SDL_RenderTexture(game.renderer, textTexture, NULL, &proff_rect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);

    sprintf(text, "FPS: %d", fps);
    textSurface = TTF_RenderText_Solid(game.font, text, 0, black);
    textTexture = SDL_CreateTextureFromSurface(game.renderer, textSurface);
    SDL_RenderTexture(game.renderer, textTexture, NULL, &fps_rect);
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

/**
 * Renders the person's destination point.
 */
void renderDestination(Game game, Person person) {
    SDL_SetRenderDrawColor(game.renderer, 0, 255, 0, 255); // Green
    for(float y = -4; y <= 4; y+=0.1) {
        float dx = sqrt(16 - y * y);
        SDL_RenderLine(game.renderer, person.destination.x - dx, person.destination.y + y, person.destination.x + dx, person.destination.y + y);
    }
}

/**
 * Renders a black overlay over everything outside the student's detection area.
 * Uses a shadow texture: fill it black, cut out the lit area with transparent
 * geometry (SDL_BLENDMODE_NONE), then composite it over the scene.
 */
static void renderBlackout(Game game, Person student) {
    SDL_SetRenderTarget(game.renderer, game.shadow_texture);

    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    SDL_SetRenderDrawBlendMode(game.renderer, SDL_BLENDMODE_NONE);
    SDL_FColor tr = {0.0f, 0.0f, 0.0f, 0.0f};
    Point p = student.position;

    if (student.detection_shape == CIRCLE) {
        SDL_Vertex verts[362];
        verts[0] = (SDL_Vertex){{p.x, p.y}, tr, {0,0}};
        for (int i = 0; i <= 360; i++) {
            float r = i * (float)M_PI / 180.0f;
            verts[i+1] = (SDL_Vertex){
                {p.x + DETECTION_RADIUS * cosf(r), p.y + DETECTION_RADIUS * sinf(r)},
                tr, {0,0}};
        }
        int idx[360*3];
        for (int i = 0; i < 360; i++) { idx[i*3]=0; idx[i*3+1]=i+1; idx[i*3+2]=i+2; }
        SDL_RenderGeometry(game.renderer, NULL, verts, 362, idx, 360*3);

    } else if (student.detection_shape == CONE) {
        float a0 = student.angle - student.detection_half_angle;
        float a1 = student.angle + student.detection_half_angle;
        int steps = (int)((a1 - a0) * 180.0f / (float)M_PI);
        if (steps < 2) steps = 2;
        SDL_Vertex *verts = malloc((steps + 2) * sizeof(SDL_Vertex));
        int *idx = malloc(steps * 3 * sizeof(int));
        verts[0] = (SDL_Vertex){{p.x, p.y}, tr, {0,0}};
        for (int i = 0; i <= steps; i++) {
            float r = a0 + i * (a1 - a0) / steps;
            verts[i+1] = (SDL_Vertex){
                {p.x + DETECTION_RADIUS * cosf(r), p.y + DETECTION_RADIUS * sinf(r)},
                tr, {0,0}};
        }
        for (int i = 0; i < steps; i++) { idx[i*3]=0; idx[i*3+1]=i+1; idx[i*3+2]=i+2; }
        SDL_RenderGeometry(game.renderer, NULL, verts, steps+2, idx, steps*3);
        free(verts); free(idx);

    }

    SDL_SetRenderDrawBlendMode(game.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(game.renderer, NULL);

    SDL_SetTextureBlendMode(game.shadow_texture, SDL_BLENDMODE_BLEND);
    SDL_RenderTexture(game.renderer, game.shadow_texture, NULL, NULL);
}

/**
 * Renders the detection area around the student for debugging.
 */
void renderDetectionRadius(Game game, Person student) {
    SDL_SetRenderDrawColor(game.renderer, 0, 200, 0, 255); // Green
    Point p = student.position;

    if (student.detection_shape == CIRCLE) {
        for (int angle = 0; angle < 360; angle++) {
            float rad  = angle       * M_PI / 180.0f;
            float rad2 = (angle + 1) * M_PI / 180.0f;
            SDL_RenderLine(game.renderer,
                p.x + DETECTION_RADIUS * cos(rad),  p.y + DETECTION_RADIUS * sin(rad),
                p.x + DETECTION_RADIUS * cos(rad2), p.y + DETECTION_RADIUS * sin(rad2));
        }
    } else if (student.detection_shape == CONE) {
        float a0 = student.angle - student.detection_half_angle;
        float a1 = student.angle + student.detection_half_angle;
        SDL_RenderLine(game.renderer, p.x, p.y,
            p.x + DETECTION_RADIUS * cosf(a0), p.y + DETECTION_RADIUS * sinf(a0));
        SDL_RenderLine(game.renderer, p.x, p.y,
            p.x + DETECTION_RADIUS * cosf(a1), p.y + DETECTION_RADIUS * sinf(a1));
        int steps = (int)(student.detection_half_angle * 2 * 180.0f / M_PI);
        if (steps < 2) steps = 2;
        for (int i = 0; i < steps; i++) {
            float r1 = a0 + i       * (a1 - a0) / steps;
            float r2 = a0 + (i + 1) * (a1 - a0) / steps;
            SDL_RenderLine(game.renderer,
                p.x + DETECTION_RADIUS * cosf(r1), p.y + DETECTION_RADIUS * sinf(r1),
                p.x + DETECTION_RADIUS * cosf(r2), p.y + DETECTION_RADIUS * sinf(r2));
        }
    }
}

/**
 * Renders the asking radius around the student as a circle outline.
 */
void renderAskingRadius(Game game, Person student) {
    Point p = student.position;
    SDL_SetRenderDrawColor(game.renderer, 255, 165, 0, 255); // Orange
    for (int angle = 0; angle < 360; angle++) {
        float rad = angle * M_PI / 180.0f;
        float rad2 = (angle + 1) * M_PI / 180.0f;
        SDL_RenderLine(game.renderer,
            p.x + ASKING_RADIUS * cos(rad), p.y + ASKING_RADIUS * sin(rad),
            p.x + ASKING_RADIUS * cos(rad2), p.y + ASKING_RADIUS * sin(rad2));
    }
}

/**
 * Renders the game by clearing the screen and drawing everything.
 */
void renderGame(Game game, Level level, Person student, Uint64 frameStartTime, int fps) {
    Uint64 frameTime;

    // Clear screen to white
    SDL_SetRenderDrawColor(game.renderer, 255, 255, 255, 255);
    SDL_RenderClear(game.renderer);

    // Render world elements
    renderWalls(game, level);
    for (int i = 0; i < level.n_professors; i++) {
        renderPerson(game, level.professors[i]);
    }
    renderPerson(game, student);

    // Black overlay outside the detection area (lantern effect)
    renderBlackout(game, student);

    // UI elements rendered above the blackout
    renderPanic(game.renderer, student.panic.levels);
    renderBattery(game.renderer, student.battery.levels);
    renderTexts(game, level, student, fps);

    // Auxiliary renders for debugging
    renderDetectionRadius(game, student);
    renderAskingRadius(game, student);
    renderDestination(game, student);

    // Present the renderer
    SDL_RenderPresent(game.renderer);

    // Delay to control frame rate
    frameTime = SDL_GetTicks() - frameStartTime;
    if (frameTime < 7) {
        SDL_Delay(7 - frameTime); // Delay to maintain approximately 144 FPS
    }
}

/**
 * Updates the time since the last frame and returns the delta time in seconds.
 */
float updateTime(Uint64 *lastTime) {
    // printf("edu maricon -- called correct float updateTime function\n");
    Uint64 currentTime = SDL_GetTicks();
    // printf("edu maricon -- currentTime = %d, lastTime = %d\n", currentTime, *lastTime);
    float delta_t = (float) (currentTime - *lastTime)/1000.0;
    // printf("edu maricon -- delta_time = %f\n", delta_t);
    *lastTime = currentTime;
    return(delta_t);
}

/**
 * Exits the game by destroying the window and quitting SDL.
 */
void exitGame(Game game, Level level) {
    free(level.walls);
    free(level.professors);

    TTF_CloseFont(game.font);
    TTF_Quit();

    SDL_DestroyTexture(game.shadow_texture);
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.win);
    SDL_Quit();
}
