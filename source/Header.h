// ==================================================
// file: Header.h
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#ifndef HEADER_H
#define HEADER_H

#ifdef DEBUG
#include <iostream>
#define _log(x) std::cout<<x<<std::endl
#endif // DEBUG

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include <vector>
#include <string>

const float DISPLAY_WIDTH = 800;
const float DISPLAY_HEIGHT = 600;

enum class GAME_STATE
{
	PLAYING,
	GAME_OVER,
	OPTIONS,
	END
};

struct Snake
{
	float box_r = 0;

	unsigned eaten_food = 0;
	unsigned done_poops = 0;

	float rgba[4] = {};
	float rgba2[4] = {};
};

struct Food
{
	float box_r = 0;
	float pos_cx = 0;
	float pos_cy = 0;

	unsigned short fruit = 0;
};

struct Poop
{
	float box_r = 25;
	float pos_cx = 0;
	float pos_cy = 0;

	unsigned short index = 0;
};

struct PopUp
{
	float pos_x = 0;
	float pos_y = 0;
	float scale = 1.0;
	float theta = 0;

	std::string text = "null";

	unsigned timer_ticks = 60;

	float rgba[4] = {0, 0, 0, 1.0};
};

struct LastGameResult
{
	unsigned eaten_food = 0;
	unsigned done_poops = 0;
	unsigned max_length = 0;

	std::string why_lost = "?";

	float time_sec = 0;

	bool buttons[3] = {0, 1, 0};
};

struct ButtonRec
{
	float pos_x = 0;
	float pos_y = 0;
	float width = 0;
	float height = 0;

	bool on = 0;
	bool hovered = 0;

	std::string text = "click me";
};

typedef void (*func_popup)(PopUp*, std::vector<float>);

struct Game
{
	ALLEGRO_DISPLAY* display = 0;
	ALLEGRO_EVENT_QUEUE* event_queue = 0;
	ALLEGRO_TIMER* timer = 0;
	ALLEGRO_FONT* font = 0;
	ALLEGRO_BITMAP* bitmap_prev_state = 0;
	ALLEGRO_BITMAP* bitmap_dest_state = 0;

	Snake snake = {0};
	LastGameResult last_game_result = {0};

	bool transition = false;
};

bool InitGame(Game* game);
void CleanGame(Game* game);

void TransformDisplay(ALLEGRO_DISPLAY* display);
void TransformMouse(float* mx, float* my);

void DrawText(ALLEGRO_FONT* font, float scale, float theta, float pos_x, float pos_y,
			  bool centre, ALLEGRO_COLOR color, std::string text);
void DrawText(ALLEGRO_FONT* font, float width, float height, float theta, float pos_x, float pos_y,
			  bool centre, ALLEGRO_COLOR color, std::string text);

void DrawPopUps(std::vector<PopUp>& vec, ALLEGRO_FONT* font);
void DrawPopUps(PopUp* pop_up, unsigned size, ALLEGRO_FONT* font);

bool UpdatePopUp(PopUp& pop_up,
				 std::vector<std::vector<float>> vec_var,
				 std::vector<func_popup> vec_ptr);

// vec.at(0) = direction x
// vec.at(1) = direction y
// vec.at(2) = speed x
// vec.at(3) = speed y
void Move(PopUp* pop_up, std::vector<float> vec);

// vec.at(0) = how much increase theta
// vec.at(1) = when stop rotating (how many timer ticks left)
// vec.at(2) = what to set after stooping rotating
void Rotate(PopUp* pop_up, std::vector<float> vec);

// vec.at(0) = -1/1 (decrease/increase)
// vec.at(1) = how much
void Scale(PopUp* pop_up, std::vector<float> vec);

// vec.at(0) = red
// vec.at(1) = green
// vec.at(2) = blue
// vec.at(3) = alpha
void ChangeColor(PopUp* pop_up, std::vector<float> vec);

void DrawButton(ButtonRec& button, ALLEGRO_FONT* font,
				ALLEGRO_COLOR color_on, ALLEGRO_COLOR color_off);
bool CheckIfHoveredButton(ButtonRec& button, float mx, float my);

// return false if end everything
bool MakeTransition(ALLEGRO_BITMAP* prev_state, ALLEGRO_BITMAP* dest_state, ALLEGRO_DISPLAY* display);

#define FILL_DEST_STATE al_set_target_bitmap(game->bitmap_dest_state); DrawCurrentState(); al_set_target_backbuffer(game->display);
#define FILL_PREV_STATE al_set_target_bitmap(game->bitmap_prev_state); DrawCurrentState(); al_set_target_backbuffer(game->display);

#endif // !HEADER_H