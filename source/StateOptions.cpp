// ==================================================
// file: StateOptions.cpp
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#include "GameStates.h"

#include <allegro5/allegro_primitives.h>

struct ColorPicker
{
	float px1 = 0.f;
	float py1 = 0.f;
	float px2 = 0.f;
	float py2 = 0.f;
	float box_w = 0.f;
	float box_h = 0.f;
	float rgba[4] = {0.f};

	bool is_touched = false;

	ButtonRec buttons_plus[4] = {0};  // indexes: 0-3
	ButtonRec buttons_minus[4] = {0}; // indexes: 4-7
	ButtonRec* button_active = 0;

	unsigned char active_button_index = 0;
};

static void Init(ColorPicker* color_picker, float px, float py, float width, float height, ALLEGRO_COLOR color)
{
	ColorPicker* ptr = color_picker;
	ptr->px1 = px;
	ptr->py1 = py;
	ptr->px2 = px + width;
	ptr->py2 = py + height;
	ptr->box_w = width / 4.f;
	ptr->box_h = height / 5.f;
	ptr->rgba[0] = color.r;
	ptr->rgba[1] = color.g;
	ptr->rgba[2] = color.b;
	ptr->rgba[3] = color.a;
	ptr->is_touched = false;

	float pos_x = px;
	float pos_y = py + (ptr->box_h);
	for(ButtonRec& button : ptr->buttons_plus)
	{
		button.pos_x = pos_x;
		button.pos_y = pos_y;
		button.width = ptr->box_w;
		button.height = ptr->box_h;
		button.text = "+";
		button.on = true;
		pos_x += ptr->box_w;
	}
	pos_x = px;
	pos_y += ptr->box_h;
	for(ButtonRec& button : ptr->buttons_minus)
	{
		button.pos_x = pos_x;
		button.pos_y = pos_y;
		button.width = ptr->box_w;
		button.height = ptr->box_h;
		button.text = "-";
		button.on = true;
		pos_x += ptr->box_w;
	}
}
static void Draw(ColorPicker* color_picker, ALLEGRO_FONT* font)
{
	for(ButtonRec& button : color_picker->buttons_plus)
		DrawButton(button, font, al_map_rgb(232, 232, 232), al_map_rgb(32, 32, 32));
	for(ButtonRec& button : color_picker->buttons_minus)
		DrawButton(button, font, al_map_rgb(232, 232, 232), al_map_rgb(32, 32, 32));

	// draw individual colors
	float px = color_picker->px1;
	float py2 = color_picker->py1 + color_picker->box_h;
	al_draw_filled_rectangle(px, color_picker->py1, px + color_picker->box_w, py2,
							 al_map_rgb_f(color_picker->rgba[0], 0, 0));
	px += color_picker->box_w;
	al_draw_filled_rectangle(px, color_picker->py1, px + color_picker->box_w, py2,
							 al_map_rgb_f(0.f, color_picker->rgba[1], 0));
	px += color_picker->box_w;
	al_draw_filled_rectangle(px, color_picker->py1, px + color_picker->box_w, py2,
							 al_map_rgb_f(0.f, 0.f, color_picker->rgba[2]));
	px += color_picker->box_w;
	al_draw_filled_rectangle(px, color_picker->py1, px + color_picker->box_w, py2,
							 al_map_rgba_f(0.f, 0.f, 0.f, color_picker->rgba[3]));

	// draw final color
	al_draw_filled_rectangle(color_picker->px1, color_picker->py1 + (color_picker->box_h * 3.f),
							 color_picker->px2, color_picker->py2,
							 al_map_rgba_f(color_picker->rgba[0], color_picker->rgba[1], color_picker->rgba[2], color_picker->rgba[3]));
}
static bool IfTouched(ColorPicker* color_picker, float mx, float my)
{
	for(unsigned char i = 0; i < 4; ++i)
	{
		if(CheckIfHoveredButton(color_picker->buttons_plus[i], mx, my))
		{
			color_picker->active_button_index = i + 1;
			color_picker->button_active = &color_picker->buttons_plus[i];
		}
		if(CheckIfHoveredButton(color_picker->buttons_minus[i], mx, my))
		{
			color_picker->active_button_index = 5 + i;
			color_picker->button_active = &color_picker->buttons_minus[i];
		}
	}
	return false;
}
static bool ButtonDown(ColorPicker* color_picker)
{
	if(color_picker->button_active && color_picker->button_active->hovered)
	{
		switch(color_picker->active_button_index)
		{
			case 1:
				color_picker->rgba[0] += 0.1f;
				break;
			case 2:
				color_picker->rgba[1] += 0.1f;
				break;
			case 3:
				color_picker->rgba[2] += 0.1f;
				break;
			case 4:
				color_picker->rgba[3] += 0.1f;
				break;
			case 5:
				color_picker->rgba[0] -= 0.1f;
				break;
			case 6:
				color_picker->rgba[1] -= 0.1f;
				break;
			case 7:
				color_picker->rgba[2] -= 0.1f;
				break;
			case 8:
				color_picker->rgba[3] -= 0.1f;
				break;

			default: return false;
				break;
		}
		return true;
	}
	return false;
}

GAME_STATE
StateOptions(Game* game)
{
	GAME_STATE next_game_state = GAME_STATE::END;

	bool end = false;
	bool draw = false;

	ColorPicker color_picker[2];
	Init(&color_picker[0], 50, 50, 340, 400, al_map_rgba_f(1.f, 0.f, 0.f, 1.f));
	Init(&color_picker[1], 420, 50, 340, 400, al_map_rgba_f(0.f, 1.f, 0.f, 1.f));

	// update pickers' colors
	for(unsigned char i = 0; i < 4; ++i)
		color_picker[0].rgba[i] = game->snake.rgba[i];
	for(unsigned char i = 0; i < 4; ++i)
		color_picker[1].rgba[i] = game->snake.rgba2[i];

	auto DrawCurrentState = [&]()
	{
		draw = false;

		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_filled_rectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, al_map_rgb(232, 232, 232));

		Draw(&color_picker[0], game->font);
		Draw(&color_picker[1], game->font);

		al_flip_display();
	};

	FILL_DEST_STATE;

	if(!MakeTransition(game->bitmap_prev_state, game->bitmap_dest_state, game->display))end = true;

	al_start_timer(game->timer);
	while(!end)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(game->event_queue, &event);

		switch(event.type)
		{
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
			{
				next_game_state = GAME_STATE::END;
				end = true;
			}
			break;
			case ALLEGRO_EVENT_DISPLAY_RESIZE:
			{
				al_acknowledge_resize(game->display);
				TransformDisplay(game->display);
			}
			break;
			case ALLEGRO_EVENT_TIMER:
			{
				draw = true;
			}
			break;
			case ALLEGRO_EVENT_MOUSE_AXES:
			{
				float mx = event.mouse.x;
				float my = event.mouse.y;

				TransformMouse(&mx, &my);

				IfTouched(&color_picker[0], mx, my);
				IfTouched(&color_picker[1], mx, my);
			}
			break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			{
				if((!ButtonDown(&color_picker[0])) && (!ButtonDown(&color_picker[1])))
				{
					next_game_state = GAME_STATE::PLAYING;
					end = true;
				}
			}
			break;

			default:
				break;
		}

		if(draw && al_is_event_queue_empty(game->event_queue))
		{
			DrawCurrentState();
		}
	}
	al_stop_timer(game->timer);

	FILL_PREV_STATE;

	// update snake's colors
	for(unsigned char i = 0; i < 4; ++i)
		game->snake.rgba[i] = color_picker[0].rgba[i];
	for(unsigned char i = 0; i < 4; ++i)
		game->snake.rgba2[i] = color_picker[1].rgba[i];

	return next_game_state;
}