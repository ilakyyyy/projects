#ifndef __SDL__
#define __SDL__
#endif

#include "head.h"

#ifdef __SDL__
int main(int argc, char* argv[]) {

    Game game;
    Level level;
    Uint64 lastTime, frameStartTime;
    Person student;
    int fps, cnt;
    int hack[2];

    ini_ran(STUDENT_NIP);
    fps = cnt = 0;

    startGame(&game);
    loadData(&level);
    setUpStudent(&student);
    setUpProfessors(&level);

    game.initialTime = lastTime = SDL_GetTicks();
    while (game.running) {
        frameStartTime = SDL_GetTicks();

        Uint64 passedTime = frameStartTime - lastTime;
        cnt++;
        if (passedTime > 0 && cnt%28 == 0) {
            fps = 1000 / passedTime;
        }

        runDynamics(&level, &student, &lastTime, hack);
        renderGame(game, level, student, frameStartTime, fps);

        checkQuitEvent(&game, level, student, lastTime);
    }

    printf("Game ended. Final stress level: %d%%\n", (int) student.panic.current_stress);

    storeResults(level, student, (int)(lastTime - game.initialTime) / 1000);

    exitGame(game, level);

    return 0;
}
#endif