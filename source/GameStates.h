// ==================================================
// file: GameStates.h
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "Header.h"

GAME_STATE StatePlaying(Game* game);
GAME_STATE StateGameOver(Game* game);
GAME_STATE StateOptions(Game*game);

#endif // !GAME_STATES_H