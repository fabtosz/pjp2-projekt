#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "objects.h"
#include <math.h>

#include <iostream>
using namespace std;

const int WIDTH = 600;
const int HEIGHT = 500;

bool keys[] = { false, false, false, false };
enum KEYS{ UP, DOWN, LEFT, RIGHT };

//ogólne sprawdzenie kolizji
//sprawdza czy dwa prostok¹ty na siebie nachodz¹
bool isCollide(float pos1_x, float pos1_y, float size1_x, float size1_y,
	float pos2_x, float pos2_y, float size2_x, float size2_y)
{
	float sr1_x = pos1_x + size1_x / 2;
	float sr1_y = pos1_y + size1_y / 2;

	float sr2_x = pos2_x + size2_x / 2;
	float sr2_y = pos2_y + size2_y / 2;

	if (
		abs(sr1_x - sr2_x) < abs((size1_x + size2_x) / 2) &&
		abs(sr1_y - sr2_y) < abs((size1_y + size2_y) / 2)
		)
		return true;

	return false;

}

// Sprawdzenie kolizji gracza z bloczkiem
// u¿ycie isCollide z konkretnymi rozmiarami prostok¹tów
bool isCollidePlayerTile(float player_x, float player_y, float tile_x, float tile_y)
{
	return isCollide(player_x, player_y, playerSizeX, playerSizeY, tile_x, tile_y, tileSize, tileSize);
}

bool canItMove(float player_x, float player_y,  // wspó³rzêdne gracza
	float x, float y)				        // o ile ruszyæ
{
	bool collision = false;

	//sprawdz kolizje dla ka¿dego kloca
	for (int i = 0; i < sizeArrayMap; i++)
	{
		//jeœli klocek jest powietrzem to go pomiñ
		if (map[i] != 0)
		{
			if (isCollidePlayerTile(player_x + x, player_y + y, tileSize * (i % mapColumns), tileSize * (i / mapColumns)))
			{
				collision = true;
				break;
			}
		}
	}
	return !collision;
}

int main(void)
{
	//variables
	bool done = false;
	bool render = false;

	int xOff = 0;
	int yOff = 0;

	// zmienne sprite'a
	const int maxFrame = 6;
	int curFrame = 0;
	int frameCount = 0;
	int frameDelay = 5;

	//allegro variable
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer;
	ALLEGRO_BITMAP *bgTiles = NULL;
	ALLEGRO_BITMAP *bgClouds = NULL;
	ALLEGRO_BITMAP *sprite[maxFrame];
	//	ALLEGRO_BITMAP *splash;

	//program init
	if (!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);			//create our display object

	if (!display)										//test display object
		return -1;

	//addon init
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

	// plansza
	bgTiles = al_load_bitmap("tiles.gif");
	bgClouds = al_load_bitmap("clouds.png");

	// gracz
	Player player;

	// Wspó³rzêdne pocz¹tkowe gracza
	player.x = 200;
	player.y = 200;

	sprite[0] = al_load_bitmap("00.gif");
	sprite[1] = al_load_bitmap("01.gif");
	sprite[2] = al_load_bitmap("02.gif");
	sprite[3] = al_load_bitmap("03.gif");
	sprite[4] = al_load_bitmap("04.gif");
	sprite[5] = al_load_bitmap("05.gif");

	for (int i = 0; i < maxFrame; i++)
		al_convert_mask_to_alpha(sprite[i], al_map_rgb(106, 76, 48));

	// kolejka zdarzen
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	/*
	splash = al_load_bitmap("splash.jpg");
	al_draw_bitmap(splash, 0, 0, 0);
	al_flip_display();
	al_rest(3.0);
	al_destroy_bitmap(splash);
	*/
	al_start_timer(timer);
	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			if (++frameCount >= frameDelay)
			{
				if (++curFrame >= maxFrame)
					curFrame = 0;

				frameCount = 0;
			}

			// tu jest zmieniana pozycja grcza
			// jeœli ruch wywo³a zderzenie z drugim obiektem to nie jest wykonywany 
			if (keys[RIGHT])
			{
				if (canItMove(player.x, player.y, 5, 0))
				{
					player.x += 5;
				}
			}

			if (keys[LEFT])
			{
				if (canItMove(player.x, player.y, -5, 0))
				{
					player.x -= 5;
				}
			}

			if (keys[DOWN])
			{
				if (canItMove(player.x, player.y, 0, 5))
				{
					player.y += 5;
				}
			}

			if (keys[UP])
			{
				if (canItMove(player.x, player.y, 0, -5))
				{
					player.y -= 5;
				}
			}

			//przeliczenie wspó³rzêdnych mapy
			//gracz z prawej
			if (player.x < WIDTH / 2) {
				xOff = 0;
			}
			//gracz w œrodku
			if (WIDTH / 2 < player.x  && player.x < mapWidth - WIDTH / 2) {
				xOff = -(player.x - WIDTH / 2);
			}
			//gracz z lewej
			if (mapWidth - WIDTH / 2 < player.x) {
				xOff = -(mapWidth - WIDTH);
			}

			render = true;
		}

		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;

			//rysuj tlo
			al_draw_bitmap(bgClouds, 0, 0, 0);

			//rysuj bloczki
			for (int i = 0; i < sizeArrayMap; i++)
			{
				al_draw_bitmap_region(bgTiles, tileSize * map[i], 0, tileSize, tileSize,
					xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
			}

			//rysuj gracza
			al_draw_bitmap(sprite[curFrame], player.x + xOff, player.y + yOff, 0);

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}

	for (int i = 0; i < maxFrame; i++)
		al_destroy_bitmap(sprite[i]);
	al_destroy_bitmap(bgClouds);
	al_destroy_bitmap(bgTiles);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);						//destroy our display object

	return 0;
}