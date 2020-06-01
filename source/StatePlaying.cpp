// ==================================================
// file: StatePlaying.cpp
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#include "GameStates.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>

static void ResetFood(Food& food)
{
	food.box_r = 20;
	food.pos_cx = rand() % unsigned(500.f - (6.0 * food.box_r)) + (3.0 * food.box_r) + 150.f;
	food.pos_cy = rand() % unsigned(500.f - (6.0 * food.box_r)) + (3.0 * food.box_r) + 100.f;
	food.fruit = (rand() % 9);
}
static void DrawTheTwoLines(float px, float py, ALLEGRO_COLOR color, float size)
{
	al_draw_line(px, 100.f, px, 600.f, color, size);
	al_draw_line(150.f, py, 650.f, py, color, size);
}

static bool IsCircleCircleCollision(float disX, float disY, float r)
{
	if(sqrtf((disX * disX) + (disY * disY)) <= r)
		return true;
	return false;
}

GAME_STATE StatePlaying(Game* game)
{
	GAME_STATE next_game_state = GAME_STATE::END;

	ALLEGRO_BITMAP* bitmap_food = al_load_bitmap("resources//bitmaps//food.png");
	ALLEGRO_BITMAP* bitmap_poops = al_load_bitmap("resources//bitmaps//poops.png");

	ALLEGRO_SAMPLE* sample_eating = al_load_sample("resources//samples//eating.ogg");
	ALLEGRO_SAMPLE* sample_no = al_load_sample("resources//samples//no.ogg");
	ALLEGRO_SAMPLE* sample_poop = al_load_sample("resources//samples//poop.ogg");
	ALLEGRO_SAMPLE* sample_start = al_load_sample("resources//samples//start.ogg");

	if(bitmap_food && bitmap_poops && sample_eating && sample_no && sample_poop && sample_start)
	{
		al_play_sample(sample_start, 1.f, 0.f, 1.f, ALLEGRO_PLAYMODE_ONCE, 0);

		bool end = false;
		bool draw = false;
		bool pause = false;

		// transition bitmap position x
		float dx = 0.f;

		unsigned long timer_count = 0;

		int dirX = 0;
		int dirY = 0;
		int prev_dirX = 0;
		int prev_dirY = 0;

		unsigned done_poops = 0;
		unsigned eaten_food = 0;
		unsigned max_length = 0;

		Snake* snake = &(game->snake);

		std::vector<std::vector<float>> positions = {{150.f + 250, 100.f + 250}};
		std::vector<Poop> poops = {};
		std::vector<PopUp> pop_up_yummy = {};
		std::vector<PopUp> pop_up_nope = {};
		std::vector<PopUp> pop_up_sorry = {};

		Food food;
		ResetFood(food);

		ButtonRec buttons_left[3];
		for(unsigned i = 0; i < 3; ++i)
		{
			buttons_left[i].pos_x = 10;
			buttons_left[i].pos_y = 110 + (i * 60);
			buttons_left[i].width = 130;
			buttons_left[i].height = 50;
		}
		buttons_left[0].text = "dark mode";
		buttons_left[0].on = game->last_game_result.buttons[0];
		buttons_left[1].text = "pop ups";
		buttons_left[1].on = game->last_game_result.buttons[1];
		buttons_left[2].text = "extra lines";
		buttons_left[2].on = game->last_game_result.buttons[2];

		ButtonRec buttons_right[1];
		for(unsigned i = 0; i < 1; ++i)
		{
			buttons_right[i].pos_x = DISPLAY_WIDTH - 140;
			buttons_right[i].pos_y = 110 + (i * 60);
			buttons_right[i].width = 130;
			buttons_right[i].height = 50;
		}
		buttons_right[0].text = "options";
		buttons_right[0].on = true;

		ALLEGRO_COLOR color_game_background = buttons_left[0].on ? al_map_rgb(0, 0, 0) : al_map_rgb(255, 255, 255);
		ALLEGRO_COLOR color_results_background = buttons_left[0].on ? al_map_rgb(50, 50, 50) : al_map_rgb(132, 132, 132);
		ALLEGRO_COLOR color_bars_background = buttons_left[0].on ? al_map_rgb(100, 100, 100) : al_map_rgb(200, 200, 200);

		game->last_game_result.why_lost = "";

		auto DrawCurrentState = [&]()
		{
			draw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_filled_rectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, color_bars_background);
			al_draw_filled_rectangle(150.f, 100.f, 650.f, 600.f, color_game_background);

			// draw extra
			if(buttons_left[2].on)
			{
				for(Poop& poop : poops)
					DrawTheTwoLines(poop.pos_cx, poop.pos_cy, al_map_rgba(132, 0, 0, 132), 10);
				DrawTheTwoLines(food.pos_cx, food.pos_cy, al_map_rgba(0, 132, 0, 132), 10);
				DrawTheTwoLines(positions[0][0], positions[0][1], al_map_rgba(132, 132, 0, 132), 20);
			}

			// draw results
			{
				float w = DISPLAY_WIDTH / 4;
				float h = (DISPLAY_HEIGHT - 500.f) / 2;
				al_draw_filled_rectangle(0, 0, DISPLAY_WIDTH, h + h, color_results_background);
				DrawText(game->font, w * 0.5, h, 0, 0.5 * w, 1.5 * h, 1, al_map_rgb(255, 255, 255), "eaten food");
				DrawText(game->font, w * 0.8, h, 0, 0.5 * w, h / 2, 1, al_map_rgb(255, 255, 255), std::to_string(eaten_food));
				DrawText(game->font, w * 0.5, h, 0, 1.5 * w, 1.5 * h, 1, al_map_rgb(255, 255, 255), "all eaten food");
				DrawText(game->font, w * 0.8, h, 0, 1.5 * w, h / 2, 1, al_map_rgb(255, 255, 255), std::to_string(snake->eaten_food + eaten_food));
				DrawText(game->font, w * 0.5, h, 0, 2.5 * w, 1.5 * h, 1, al_map_rgb(255, 255, 255), "done poops");
				DrawText(game->font, w * 0.8, h, 0, 2.5 * w, h / 2, 1, al_map_rgb(255, 255, 255), std::to_string(done_poops));
				DrawText(game->font, w * 0.5, h, 0, 3.5 * w, 1.5 * h, 1, al_map_rgb(255, 255, 255), "all done poops");
				DrawText(game->font, w * 0.8, h, 0, 3.5 * w, h / 2, 1, al_map_rgb(255, 255, 255), std::to_string(snake->done_poops + done_poops));
			}

			// draw left bar
			for(ButtonRec& button : buttons_left)
				DrawButton(button, game->font, al_map_rgb(232, 232, 0), al_map_rgb(232, 0, 0));
			DrawText(game->font, 125, 50, 0, 75, 560, 1, al_map_rgb(0, 0, 0), "TAB: pause");
			// darw right bar
			for(ButtonRec& button : buttons_right)
				DrawButton(button, game->font, al_map_rgb(232, 232, 0), al_map_rgb(232, 0, 0));
			// draw poops
			for(Poop& poop : poops)
				al_draw_bitmap_region(bitmap_poops, poop.index * 50, 0, 50, 80, poop.pos_cx - 25, poop.pos_cy - 23, 0);

			// draw food
			al_draw_bitmap_region(bitmap_food, int(food.fruit) * 50, 0, 50, 50, food.pos_cx - 25, food.pos_cy - 25, 0);

			// draw the snake
			{
				float size = float(positions.size());
				float xr = snake->rgba[0] > snake->rgba2[0] ? (snake->rgba[0] - snake->rgba2[0]) / -size : (snake->rgba2[0] - snake->rgba[0]) / size;
				float xg = snake->rgba[1] > snake->rgba2[1] ? (snake->rgba[1] - snake->rgba2[1]) / -size : (snake->rgba2[1] - snake->rgba[1]) / size;
				float xb = snake->rgba[2] > snake->rgba2[2] ? (snake->rgba[2] - snake->rgba2[2]) / -size : (snake->rgba2[2] - snake->rgba[2]) / size;
				for(unsigned i = 0; i < positions.size(); ++i)
				{
					al_draw_filled_circle(positions[i][0], positions[i][1], 30,
										  al_map_rgba_f(snake->rgba[0] + (xr * i),
										  snake->rgba[1] + (xg * i),
										  snake->rgba[2] + (xb * i),
										  snake->rgba[3]));
				}
			}

			// draw pop ups
			if(buttons_left[1].on)
			{
				DrawPopUps(pop_up_yummy, game->font);
				DrawPopUps(pop_up_nope, game->font);
				DrawPopUps(pop_up_sorry, game->font);
			}

			if(pause)
				al_draw_filled_rectangle(150.f, 100.f, 650.f, 600.f, al_map_rgba(132, 132, 132, 132));

			al_flip_display();
		};

		FILL_DEST_STATE;

		if(!MakeTransition(game->bitmap_prev_state, game->bitmap_dest_state, game->display))end = true;

		float time_start = al_get_time();
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
					timer_count++;
					draw = true;

					if(!pause)
					{
						// update pop ups
						{
							for(PopUp& pop_up : pop_up_yummy)
							{
								std::vector<std::vector<float>> vec_var =
								{{0.5, 30, 0}, {1, 0.025}, {-0.001, 0.1, 0, 0}};
								std::vector<func_popup> vec_ptr =
								{Rotate, Scale, ChangeColor};
								if(!UpdatePopUp(pop_up, vec_var, vec_ptr))
									pop_up_yummy.erase(pop_up_yummy.begin());
							}
							for(PopUp& pop_up : pop_up_nope)
							{
								std::vector<std::vector<float>> vec_var = {{-1, 0.025}};
								std::vector<func_popup> vec_ptr = {Scale};
								if(!UpdatePopUp(pop_up, vec_var, vec_ptr))
									pop_up_nope.erase(pop_up_nope.begin());
							}
							for(PopUp& pop_up : pop_up_sorry)
							{
								std::vector<std::vector<float>> vec_var = {{0, -1, 0, 2}, {0, 0, 0, -0.03}};
								std::vector<func_popup> vec_ptr = {Move, ChangeColor};
								if(!UpdatePopUp(pop_up, vec_var, vec_ptr))
									pop_up_sorry.erase(pop_up_sorry.begin());
							}
						}

						if(!(timer_count % 2))
						{
							// check if snake is eating
							{
								if(IsCircleCircleCollision(food.pos_cx - positions[0][0],
								   food.pos_cy - positions[0][1],
								   food.box_r + snake->box_r))
								{
									eaten_food++;

									float pos_x = positions[0][0] + dirX * 10;
									float pos_y = positions[0][1] + dirX * 10;
									positions.push_back({pos_x, pos_y});

									al_play_sample(sample_eating, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);

									PopUp pop_up;
									pop_up.pos_x = positions[0][0];
									pop_up.pos_y = positions[0][1];
									pop_up.scale = 0.1;
									pop_up.text = "yummy";
									pop_up.rgba[0] = 1.0;
									pop_up_yummy.push_back(pop_up);

									ResetFood(food);

									if(positions.size() > max_length)
										max_length = positions.size();
								}
							}

							// update snake's positions
							{
								for(unsigned i = positions.size() - 1; i > 0; --i)
								{
									positions[i][0] = positions[i - 1][0];
									positions[i][1] = positions[i - 1][1];
								}
								positions[0][0] += dirX * 10;
								positions[0][1] += dirY * 10;
							}

							// check for snake's collision
							{
								// collision with own body segments
								// n - how many do not consider, they collide all the time
								unsigned n = unsigned((snake->box_r * 2.0f) / 10.0f);
								for(unsigned i = n; i < positions.size(); ++i)
								{
									if(IsCircleCircleCollision(positions[i][0] - positions[0][0],
									   positions[i][1] - positions[0][1],
									   snake->box_r))
									{
										game->last_game_result.why_lost = "you cannot collide with own body";
										next_game_state = GAME_STATE::GAME_OVER;
										end = true;
										break;
									}
								}

								// collision with own poops
								for(unsigned i = 0; i < poops.size(); ++i)
								{
									if(IsCircleCircleCollision(poops[i].pos_cx - positions[0][0],
									   poops[i].pos_cy - positions[0][1],
									   poops[i].box_r * 2))
									{
										game->last_game_result.why_lost = "you cannot collide with own poop";
										next_game_state = GAME_STATE::GAME_OVER;
										end = true;
										break;
									}
								}

								// collision with game display borders
								if(positions[0][0] < 150.f || positions[0][0] > 650.f ||
								   positions[0][1] < 100.f || positions[0][1] > 600.f)
								{
									game->last_game_result.why_lost = "you cannot get out of borders";
									next_game_state = GAME_STATE::GAME_OVER;
									end = true;
									break;
								}
							}
						}
					}
				}
				break;
				case ALLEGRO_EVENT_KEY_DOWN:
				{
					// if the game should end
					int result = 0;

					switch(event.keyboard.keycode)
					{
						case ALLEGRO_KEY_LEFT:
						{
							if(!pause)
							{
								if(prev_dirX == 1)
									result = 1;
								dirX = -1;
								dirY = 0;
								prev_dirX = -1;
								prev_dirY = 0;
							}
						}
						break;
						case ALLEGRO_KEY_RIGHT:
						{
							if(!pause)
							{
								if(prev_dirX == -1)
									result = 1;
								dirX = 1;
								dirY = 0;
								prev_dirX = 1;
								prev_dirY = 0;
							}
						}
						break;
						case ALLEGRO_KEY_UP:
						{
							if(!pause)
							{
								if(prev_dirY == 1)
									result = 1;
								dirY = -1;
								dirX = 0;
								prev_dirX = 0;
								prev_dirY = -1;
							}
						}
						break;
						case ALLEGRO_KEY_DOWN:
						{
							if(!pause)
							{
								if(prev_dirY == -1)
									result = 1;
								dirY = 1;
								dirX = 0;
								prev_dirX = 0;
								prev_dirY = 1;
							}
						}
						break;
						case ALLEGRO_KEY_SPACE:
						{
							// do number two
							unsigned size = positions.size();
							if(size > 10)
							{
								done_poops++;

								Poop poop;
								poop.box_r = food.box_r;
								poop.pos_cx = positions[size - 1][0];
								poop.pos_cy = positions[size - 1][1];
								poop.index = (rand() % 9);
								poops.push_back(poop);

								PopUp pop_up;
								pop_up.pos_x = poop.pos_cx;
								pop_up.pos_y = poop.pos_cy;
								pop_up.timer_ticks = 30;
								pop_up.text = "sorry";
								pop_up.rgba[0] = 0.25;
								pop_up.rgba[1] = 0.25;
								pop_up.rgba[2] = 0.75;
								pop_up_sorry.push_back(pop_up);

								float head_px = positions[0][0];
								float head_py = positions[0][1];
								positions.erase(positions.end() - 11, positions.end() - 1);
								positions[0][0] = head_px;
								positions[0][1] = head_py;

								al_play_sample(sample_poop, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
							}
							else
							{
								al_play_sample(sample_no, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);

								PopUp pop_up;
								pop_up.pos_x = positions[0][0];
								pop_up.pos_y = positions[0][1];
								pop_up.text = "nope";
								pop_up.rgba[0] = 1.0;
								pop_up.rgba[3] = 0.5;
								pop_up.timer_ticks = 30;
								pop_up.scale = 2.0;
								pop_up_nope.push_back(pop_up);
							}
						}
						break;
						case ALLEGRO_KEY_TAB:
							pause = !pause;
							break;
						default:
							break;
					}

					if(result && (positions.size() > 1))
					{
						game->last_game_result.why_lost = "you cannot get back";
						next_game_state = GAME_STATE::GAME_OVER;
						end = true;
						break;
					}
				}
				break;
				case ALLEGRO_EVENT_MOUSE_AXES:
				{
					float mx = event.mouse.x;
					float my = event.mouse.y;

					TransformMouse(&mx, &my);

					for(ButtonRec& button : buttons_left)
						CheckIfHoveredButton(button, mx, my);
					for(ButtonRec& button : buttons_right)
						CheckIfHoveredButton(button, mx, my);
				}
				break;
				case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				{
					if(buttons_left[0].hovered)
					{
						buttons_left[0].on = !buttons_left[0].on;
						color_game_background = buttons_left[0].on ? al_map_rgb(0, 0, 0) : al_map_rgb(255, 255, 255);
						color_results_background = buttons_left[0].on ? al_map_rgb(50, 50, 50) : al_map_rgb(132, 132, 132);
						color_bars_background = buttons_left[0].on ? al_map_rgb(100, 100, 100) : al_map_rgb(200, 200, 200);
					}
					else if(buttons_left[1].hovered)
					{
						buttons_left[1].on = !buttons_left[1].on;
					}
					else if(buttons_left[2].hovered)
					{
						buttons_left[2].on = !buttons_left[2].on;
					}
					else if(buttons_right[0].hovered)
					{
						next_game_state = GAME_STATE::OPTIONS;
						end = true;
						break;
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
		float time_sec = al_get_time() - time_start;

		FILL_PREV_STATE;

		// update values
		snake->eaten_food += eaten_food;
		snake->done_poops += done_poops;
		game->last_game_result.eaten_food = eaten_food;
		game->last_game_result.done_poops = done_poops;
		game->last_game_result.max_length = max_length;
		game->last_game_result.time_sec = time_sec;
		if(game->last_game_result.why_lost == "")
			game->last_game_result.why_lost = "... you should be still alive ...";
		game->last_game_result.buttons[0] = buttons_left[0].on;
		game->last_game_result.buttons[1] = buttons_left[1].on;
		game->last_game_result.buttons[2] = buttons_left[2].on;
	}

	al_destroy_bitmap(bitmap_food);
	al_destroy_bitmap(bitmap_poops);

	al_destroy_sample(sample_eating);
	al_destroy_sample(sample_no);
	al_destroy_sample(sample_poop);
	al_destroy_sample(sample_start);

	return next_game_state;
}