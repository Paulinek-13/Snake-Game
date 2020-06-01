// ==================================================
// file: StateGameOver.cpp
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#include "GameStates.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>

GAME_STATE StateGameOver(Game* game)
{
	GAME_STATE next_game_state = GAME_STATE::END;

	ALLEGRO_SAMPLE* sample_game_over = al_load_sample("resources//samples//game_over.ogg");

	if(sample_game_over)
	{
		al_play_sample(sample_game_over, 1.f, 0.f, 1.f, ALLEGRO_PLAYMODE_ONCE, 0);

		bool end = false;
		bool draw = false;

		LastGameResult* game_result = &(game->last_game_result);

		PopUp pop_up_game_over;
		PopUp pop_up_results[5] = {};

		// init pop ups with last game results
		{
			float px = DISPLAY_WIDTH / 2;
			float py = DISPLAY_HEIGHT / 7;
			for(unsigned i = 0; i < 5; ++i)
			{
				pop_up_results[i].pos_x = 0;
				pop_up_results[i].pos_y = (float(0.5 + i)) * py;
				pop_up_results[i].scale = 0.1;
			}
			pop_up_results[0].text = std::to_string(game_result->eaten_food) + " = eaten food";
			pop_up_results[1].text = std::to_string(game_result->done_poops) + " = done poops";
			pop_up_results[2].text = std::to_string(game_result->max_length) + " = max length";
			pop_up_results[3].text = "time: " + std::to_string(unsigned(game_result->time_sec)) + "s";
			pop_up_results[4].text = "remember: " + game_result->why_lost;
		}

		auto DrawCurrentState = [&]()
		{
			draw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_filled_rectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, al_map_rgb(255, 255, 0));

			DrawPopUps(pop_up_results, 5, game->font);

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

					// update pop ups
					if(pop_up_results[0].timer_ticks > 0)
					{
						for(PopUp& pop_up : pop_up_results)
						{
							std::vector<std::vector<float>> vec_var =
							{{1, 0.01}, {1, 0, 6.66, 0}};
							std::vector<func_popup> vec_ptr =
							{Scale, Move};
							UpdatePopUp(pop_up, vec_var, vec_ptr);
						}
					}
				}
				break;
				case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				{
					next_game_state = GAME_STATE::PLAYING;
					end = true;
				}
				break;
				case ALLEGRO_EVENT_KEY_DOWN:
				{
					next_game_state = GAME_STATE::PLAYING;
					end = true;
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
	}

	al_destroy_sample(sample_game_over);

	return next_game_state;
}