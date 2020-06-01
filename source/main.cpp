// ==================================================
// file: main.cpp
// project: Snake Game
// author: Paulina Kalicka
// ==================================================

#include "Header.h"
#include "GameStates.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

int main()
{
	srand(time(NULL));

	if(!al_init())
		return -1;

	if(!al_init_primitives_addon())
		return -1;
	if(!al_init_font_addon())
		return -1;
	if(!al_init_ttf_addon())
		return -1;
	if(!al_init_image_addon())
		return -1;
	if(!al_init_acodec_addon())
		return -1;
	if(!al_install_keyboard())
		return -1;
	if(!al_install_mouse())
		return -1;
	if(!al_install_audio())
		return -1;

	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	al_reserve_samples(3);

	ALLEGRO_BITMAP* bitmap_icon = al_load_bitmap("resources//icon.png");

	Game game;

	if(InitGame(&game))
	{
		al_set_window_title(game.display, "Snake Game");
		al_set_display_icon(game.display, bitmap_icon);
		al_set_app_name("Snake Game");
		al_set_org_name("PAULINEK");

		// pointer to a function that loops a game state and returns the next game state
		GAME_STATE(*game_state_ptr)
			(Game * game) = StatePlaying;

		bool end = false;

		while(!end)
		{
			switch(game_state_ptr(&game))
			{
				case GAME_STATE::END:
					end = true;
					break;
				case GAME_STATE::PLAYING:
					game_state_ptr = StatePlaying;
					break;
				case GAME_STATE::GAME_OVER:
					game_state_ptr = StateGameOver;
					break;
				case GAME_STATE::OPTIONS:
					game_state_ptr = StateOptions;
					break;
				default:
					end = false;
					break;
			}
		}
	}

	al_destroy_bitmap(bitmap_icon);

	CleanGame(&game);

	return 0;
}