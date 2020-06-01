// ==================================================
// file: Header.cpp
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#include "Header.h"

#include <allegro5/allegro_primitives.h>

#include <fstream>

static bool LoadStats(Game* game)
{
	std::ifstream file;
	file.open("data//stats.txt");
	if(file.is_open())
	{
		file >> game->snake.eaten_food;
		file >> game->snake.done_poops;
		return true;
	}
	return false;
}
static bool SaveStats(Game* game)
{
	std::ofstream file;
	file.open("data//stats.txt");
	if(file.is_open())
	{
		file << game->snake.eaten_food << " " << game->snake.done_poops;
		return true;
	}
	return false;
}

bool InitGame(Game* game)
{
	game->display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	game->event_queue = al_create_event_queue();
	game->timer = al_create_timer(1.0 / 60.0);
	game->font = al_load_font("resources//fonts//kenny_mini.ttf", 32, 0);
	game->bitmap_prev_state = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	game->bitmap_dest_state = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);

	if((!game->display) || (!game->event_queue) || (!game->font) || (!game->timer) || (!game->bitmap_prev_state) || (!game->bitmap_dest_state))
		return false;

	al_register_event_source(game->event_queue, al_get_display_event_source(game->display));
	al_register_event_source(game->event_queue, al_get_timer_event_source(game->timer));
	al_register_event_source(game->event_queue, al_get_mouse_event_source());
	al_register_event_source(game->event_queue, al_get_keyboard_event_source());

	game->snake.box_r = 32;
	game->snake.rgba[0] = 1.0;
	game->snake.rgba[1] = 0;
	game->snake.rgba[2] = 0;
	game->snake.rgba[3] = 1.0;
	game->snake.rgba2[0] = 0;
	game->snake.rgba2[1] = 1.0;
	game->snake.rgba2[2] = 0;
	game->snake.rgba2[3] = 1.0;

	if(!LoadStats(game))
	{
		game->snake.eaten_food = 0;
		game->snake.done_poops = 0;
	}

	return true;
}
void CleanGame(Game* game)
{
	if(!SaveStats(game))
	{
#ifdef DEBUG
		_log("stats not saved!");
#endif // DEBUG
	}

	al_destroy_font(game->font);
	al_destroy_timer(game->timer);
	al_destroy_event_queue(game->event_queue);
	al_destroy_display(game->display);
	al_destroy_bitmap(game->bitmap_prev_state);
	al_destroy_bitmap(game->bitmap_dest_state);
}

void TransformDisplay(ALLEGRO_DISPLAY* display)
{
	ALLEGRO_TRANSFORM t;

	float scaleX = al_get_display_width(display) / DISPLAY_WIDTH;
	float scaleY = al_get_display_height(display) / DISPLAY_HEIGHT;
	float scale = scaleX < scaleY ? scaleX : scaleY;

	al_identity_transform(&t);
	al_scale_transform(&t, scale, scale);
	if(scale == scaleX)
	{
		float y = (al_get_display_height(display) / 2) - (DISPLAY_HEIGHT * scale * 0.5f);
		al_translate_transform(&t, 0, y);
	}
	else if(scale == scaleY)
	{
		float x = (al_get_display_width(display) / 2) - (DISPLAY_WIDTH * scale * 0.5f);
		al_translate_transform(&t, x, 0);
	}
	al_use_transform(&t);
}
void TransformMouse(float* mx, float* my)
{
	ALLEGRO_TRANSFORM t;
	al_copy_transform(&t, al_get_current_transform());
	al_invert_transform(&t);
	al_transform_coordinates(&t, mx, my);
}

void DrawText(ALLEGRO_FONT* font, float scale, float theta, float pos_x, float pos_y,
			  bool centre, ALLEGRO_COLOR color, std::string text)
{
	ALLEGRO_TRANSFORM prev_t;
	ALLEGRO_TRANSFORM t;

	al_copy_transform(&prev_t, al_get_current_transform());

	al_identity_transform(&t);
	al_translate_transform(&t, -pos_x, -pos_y);
	al_scale_transform(&t, scale, scale);
	al_rotate_transform(&t, theta);
	al_translate_transform(&t, pos_x, pos_y);

	al_compose_transform(&t, &prev_t);
	al_use_transform(&t);

	float font_h = al_get_font_line_height(font);
	al_draw_textf(font, color, pos_x, pos_y - (font_h / 2), centre, "%s", text.c_str());

	al_use_transform(&prev_t);
}
void DrawText(ALLEGRO_FONT* font, float width, float height, float theta, float pos_x, float pos_y,
			  bool centre, ALLEGRO_COLOR color, std::string text)
{
	float text_width = al_get_text_width(font, text.c_str());
	float text_height = al_get_font_line_height(font);

	float scalex = width / text_width;
	float scaley = height / text_height;

	float scale = scalex < scaley ? scalex : scaley;

	ALLEGRO_TRANSFORM prev_t;
	ALLEGRO_TRANSFORM t;

	al_copy_transform(&prev_t, al_get_current_transform());

	al_identity_transform(&t);
	al_translate_transform(&t, -pos_x, -pos_y);
	al_scale_transform(&t, scale, scale);
	al_rotate_transform(&t, theta);
	al_translate_transform(&t, pos_x, pos_y);

	al_compose_transform(&t, &prev_t);
	al_use_transform(&t);

	float font_h = al_get_font_line_height(font);
	al_draw_textf(font, color, pos_x, pos_y - (font_h / 2), centre, "%s", text.c_str());

	al_use_transform(&prev_t);
}

void DrawPopUps(std::vector<PopUp>& vec, ALLEGRO_FONT* font)
{
	for(PopUp& pop_up : vec)
	{
		ALLEGRO_COLOR color_yummy =
			al_map_rgba_f(pop_up.rgba[0],
						  pop_up.rgba[1],
						  pop_up.rgba[2],
						  pop_up.rgba[3]);
		DrawText(font,
				 pop_up.scale,
				 pop_up.theta,
				 pop_up.pos_x,
				 pop_up.pos_y,
				 1,
				 color_yummy,
				 pop_up.text);
	}
}
void DrawPopUps(PopUp* pop_up, unsigned size, ALLEGRO_FONT* font)
{
	for(unsigned i = 0; i < size; ++i)
	{
		ALLEGRO_COLOR color_yummy =
			al_map_rgba_f(pop_up[i].rgba[0],
						  pop_up[i].rgba[1],
						  pop_up[i].rgba[2],
						  pop_up[i].rgba[3]);
		DrawText(font,
				 pop_up[i].scale,
				 pop_up[i].theta,
				 pop_up[i].pos_x,
				 pop_up[i].pos_y,
				 1,
				 color_yummy,
				 pop_up[i].text);
	}
}

bool UpdatePopUp(PopUp& pop_up,
				 std::vector<std::vector<float>> vec_var,
				 std::vector<func_popup> vec_ptr)
{
	if((--pop_up.timer_ticks <= 0))
	{
		// now you can delete it
		return false;
	}
	else
	{
		for(unsigned i = 0; i < vec_ptr.size(); ++i)
		{
			(vec_ptr[i])(&pop_up, vec_var[i]);
		}
	}
	return true;
}

// vec.at(0) = direction x
// vec.at(1) = direction y
// vec.at(2) = speed x
// vec.at(3) = speed y
void Move(PopUp* pop_up, std::vector<float> vec)
{
	if(vec.size() == 4)
	{
		pop_up->pos_x += (vec.at(0) * vec.at(2));
		pop_up->pos_y += (vec.at(1) * vec.at(3));
	}
}

// vec.at(0) = how much increase theta
// vec.at(1) = when stop rotating (how many timer ticks left)
// vec.at(2) = what to set after stooping rotating
void Rotate(PopUp* pop_up, std::vector<float> vec)
{
	if(vec.size() == 3)
	{
		pop_up->theta += vec.at(0);

		if(pop_up->timer_ticks < vec.at(1))
			pop_up->theta = vec.at(2);
	}
}

// vec.at(0) = -1/1 (decrease/increase)
// vec.at(1) = how much
void Scale(PopUp* pop_up, std::vector<float> vec)
{
	if(vec.size() == 2)
	{
		pop_up->scale += (vec.at(0) * vec.at(1));
		if(pop_up->scale < 0)
			pop_up->timer_ticks = 0;
	}
}

// vec.at(0) = red
// vec.at(1) = green
// vec.at(2) = blue
// vec.at(3) = alpha
void ChangeColor(PopUp* pop_up, std::vector<float> vec)
{
	if(vec.size() == 4)
	{
		pop_up->rgba[0] += vec[0];
		pop_up->rgba[1] += vec[1];
		pop_up->rgba[2] += vec[2];
		pop_up->rgba[3] += vec[3];
	}
}

void DrawButton(ButtonRec& button, ALLEGRO_FONT* font,
				ALLEGRO_COLOR color_on, ALLEGRO_COLOR color_off)
{
	if(button.on)
	{
		al_draw_filled_rectangle(button.pos_x, button.pos_y,
								 button.pos_x + button.width,
								 button.pos_y + button.height,
								 color_on);
		DrawText(font, button.width, button.height, 0,
				 button.pos_x + (button.width / 2), button.pos_y + (button.height / 2),
				 1, color_off, button.text);
	}
	else
	{
		al_draw_filled_rectangle(button.pos_x, button.pos_y,
								 button.pos_x + button.width,
								 button.pos_y + button.height,
								 color_off);
		DrawText(font, button.width, button.height, 0,
				 button.pos_x + (button.width / 2), button.pos_y + (button.height / 2),
				 1, color_on, button.text);
	}

	if(button.hovered)
	{
		al_draw_filled_rectangle(button.pos_x, button.pos_y,
								 button.pos_x + button.width,
								 button.pos_y + button.height,
								 al_map_rgba(132, 132, 132, 132));
	}
}
bool CheckIfHoveredButton(ButtonRec& button, float mx, float my)
{
	if(mx > button.pos_x && mx < button.pos_x + button.width)
	{
		if(my > button.pos_y && my < button.pos_y + button.height)
		{
			button.hovered = true;
			return true;
		}
	}
	button.hovered = false;
	return false;
}

// return false if end everything
bool MakeTransition(ALLEGRO_BITMAP* prev_state, ALLEGRO_BITMAP* dest_state, ALLEGRO_DISPLAY* display)
{
	bool result = true;

	ALLEGRO_TIMER* timer = al_create_timer(1 / 60.f);
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	if((!timer) || (!event_queue))
		result = false;

	if(result)
	{
		bool end = false;
		bool draw = false;

		// bitmaps' positions
		float px1 = 0.f;
		float px2 = -DISPLAY_WIDTH;
		float road = 42.f;

		al_register_event_source(event_queue, al_get_display_event_source(display));
		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_start_timer(timer);
		while(!end)
		{
			ALLEGRO_EVENT event;
			al_wait_for_event(event_queue, &event);

			switch(event.type)
			{
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					end = true;
					result = false;
					break;
				case ALLEGRO_EVENT_TIMER:
					draw = true;
					px1 += road;
					px2 += road;
					/*px1 += 20;
					px2 += 20;*/
					road *= 0.95f;
					if(px1 >= DISPLAY_WIDTH)
						end = true;
					break;

				default:
					break;
			}

			if(draw && al_is_event_queue_empty(event_queue))
			{
				al_clear_to_color(al_map_rgb(0, 0, 0));

				al_draw_bitmap_region(prev_state, 0.f, 0.f, DISPLAY_WIDTH - px1, DISPLAY_HEIGHT, px1, 0.f, 0);
				al_draw_bitmap_region(dest_state, DISPLAY_WIDTH - px1, 0.f, px1, DISPLAY_HEIGHT, 0.f, 0.f, 0);

				al_flip_display();
			}
		}
		al_stop_timer(timer);
	}

	al_destroy_timer(timer);
	return result;
}