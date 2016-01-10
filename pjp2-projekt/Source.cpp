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
const int NUM_BULLETS = 3;

bool keys[] = { false, false, false, false, false };
enum KEYS{ UP, DOWN, LEFT, RIGHT, SPACE };

//ogólne sprawdzenie kolizji
bool isCollide(float pos1_x, float pos1_y, float size1_x, float size1_y, float pos2_x, float pos2_y, float size2_x, float size2_y)
{
	if ((pos1_x > pos2_x + size2_x - 1) || // is b1 on the right side of b2?
		(pos1_y > pos2_y + size2_y - 1) || // is b1 under b2?
		(pos2_x > pos1_x + size1_x - 1) || // is b2 on the right side of b1?
		(pos2_y > pos1_y + size1_y - 1))   // is b2 under b1?
	{
		// brak kolizji
		return 0;
	}
	// jest kolizja
	return 1;
}

// Sprawdzenie kolizji gracza z bloczkiem
// użycie isCollide z konkretnymi rozmiarami prostokątów
bool isCollidePlayerTile(float player_x, float player_y, float tile_x, float tile_y)
{
	return isCollide(player_x, player_y, playerSizeX, playerSizeY, tile_x, tile_y, tileSize, tileSize);
}

bool canItMove(float player_x, float player_y,  /* współrzędne gracza */float x, float y /* o ile ruszyć */)
{
	bool collision = false;

	//sprawdz kolizje dla każdego kloca
	for (int i = 0; i < sizeArrayMap; i++)
	{
		//jeśli klocek jest powietrzem to go pomiń
		if (map[i] != 0 && map[i] != 3)
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
bool isOnSolidGround(int player_x, int player_y)
{
	if (!canItMove(player_x, player_y, 0, 5))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, Player &player);
void UpdateBullet(Bullet bullet[], int size, Player &player);
void MovePlayerRight(Player &player);
void MovePlayerLeft(Player &player);

int main(void)
{
	//variables
	bool done = false;
	bool render = false;



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
	ALLEGRO_BITMAP *spritePlayer[maxFrame];
	ALLEGRO_BITMAP *spriteMonster[maxFrame];
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

	// Współrzędne początkowe gracza
	player.x = 100;
	player.y = 100;

	// pociski
	Bullet bullets[NUM_BULLETS];

	InitBullet(bullets, NUM_BULLETS);

	// potwory
	Monster monster[3];

	// Współrzędne potwora 1
	monster[0].x = 300;
	monster[0].y = 100;
	// Współrzędne potwora 2
	monster[1].x = 1000;
	monster[1].y = 200;
	// Współrzędne potwora 3
	monster[2].x = 800;
	monster[2].y = 100;

	spritePlayer[0] = al_load_bitmap("00.gif");
	spritePlayer[1] = al_load_bitmap("01.gif");
	spritePlayer[2] = al_load_bitmap("02.gif");
	spritePlayer[3] = al_load_bitmap("03.gif");
	spritePlayer[4] = al_load_bitmap("04.gif");
	spritePlayer[5] = al_load_bitmap("05.gif");

	spriteMonster[0] = al_load_bitmap("monster00.gif");
	spriteMonster[1] = al_load_bitmap("monster01.gif");
	spriteMonster[2] = al_load_bitmap("monster02.gif");
	spriteMonster[3] = al_load_bitmap("monster03.gif");
	spriteMonster[4] = al_load_bitmap("monster04.gif");
	spriteMonster[5] = al_load_bitmap("monster05.gif");

	for (int i = 0; i < maxFrame; i++)
		al_convert_mask_to_alpha(spritePlayer[i], al_map_rgb(106, 76, 48));

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

	bool mayJumpAgain = true;
	int wysokoscSkoku = 0;
	double kierunek = 1;

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
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
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
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				if (player.mana > 100)
				{
					FireBullet(bullets, NUM_BULLETS, player);
					if (player.mana > 100)
						player.mana -= 50;
				}
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			//sprity
			if (++frameCount >= frameDelay)
			{
				if (++curFrame >= maxFrame)
					curFrame = 0;

				frameCount = 0;
			}

			// tu jest zmieniana pozycja grcza
			// jeśli ruch wywoła zderzenie z drugim obiektem to nie jest wykonywany 
			if (keys[RIGHT])
				MovePlayerRight(player);
			if (keys[LEFT])
				MovePlayerLeft(player);
			/* * * *  S K A K A N I E * * * */
			if (mayJumpAgain)
			{
				if (keys[UP])
				{
					if (canItMove(player.x, player.y, 0, -5))
					{
						player.y -= 10;
						wysokoscSkoku++;
					}
					if (wysokoscSkoku == 30)
					{
						mayJumpAgain = false;
					}
				}
				else mayJumpAgain = false;
			}
			if (!mayJumpAgain)
			{
				if (isOnSolidGround(player.x, player.y))
				{
					wysokoscSkoku = 0;
					mayJumpAgain = true;
				}
			}
			if (!canItMove(player.x, player.y, 0, -5))
			{
				wysokoscSkoku = 0;
				mayJumpAgain = false;
			}
			if (isOnSolidGround(player.x, player.y))
			{
				wysokoscSkoku = 0;
				mayJumpAgain = true;
			}
			/* * * * * * * * * * * * * * * */

			//spadanie
			if (canItMove(player.x, player.y, 0, 5))
			{
				player.y += 5;
			}

			//kolizja gracza z potworami
			for (int i = 0; i < 3; i++)
			{
				if (isCollide(player.x, player.y, playerSizeX, playerSizeY, monster[i].x, monster[i].y, monster[i].sizeX, monster[i].sizeY))
					player.lives--;
			}

			//kolizja pociskow z potworami
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (isCollide(bullets[i].x, bullets[i].y, bullets[i].size, bullets[i].size, monster[j].x + xOff, monster[j].y + yOff, monster[j].sizeX, monster[j].sizeY))
					{
						monster[j].alive = bullets[i].alive = false;
						monster[j].x = -100;
						monster[j].y = -100;

						bullets[i].alive = false;
						bullets[i].x = -100;
						bullets[i].y = -100;

						player.points += 100;
						cout << player.points << endl;
					}
				}
			}


			//przeliczenie współrzędnych mapy
			//gracz z prawej
			if (player.x < WIDTH / 2) {
				xOff = 0;
			}
			//gracz w środku
			if (WIDTH / 2 < player.x  && player.x < mapWidth - WIDTH / 2) {
				xOff = -(player.x - WIDTH / 2);
			}
			//gracz z lewej
			if (mapWidth - WIDTH / 2 < player.x) {
				xOff = -(mapWidth - WIDTH);
			}

			render = true;
		}

		// if od rysowania
		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;

			//rysuj tlo
			al_draw_bitmap(bgClouds, 0 + xOff / 8, 0, 0);

			//rysuj bloczki
			for (int i = 0; i < sizeArrayMap; i++)
			{
				al_draw_bitmap_region(bgTiles, tileSize * map[i], 0, tileSize, tileSize,
					xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
			}

			//rysuj gracza
			al_draw_bitmap(spritePlayer[curFrame], player.x + xOff, player.y + yOff, 0);

			//rysuj pociski
			DrawBullet(bullets, NUM_BULLETS);
			UpdateBullet(bullets, NUM_BULLETS, player);

			//rysuj potwory
			for (int i = 0; i < 3; i++)
			{
				if (monster[i].alive)
					al_draw_bitmap(spriteMonster[curFrame], monster[i].x + xOff, monster[i].y + yOff, 0);
			}

			//ruch potwora
			if (canItMove(monster[0].x, monster[0].y, 0, 5))
			{
				monster[0].y += 4;
			}
			if (canItMove(monster[1].x, monster[1].y, 0, 5))
			{
				monster[1].y += 4;
			}
			if (canItMove(monster[2].x, monster[2].y, 0, 5))
			{
				monster[2].y += 4;
			}
			if (canItMove(monster[0].x, monster[0].y, 0, 0))
			{
				monster[0].x -= 0.8*kierunek;
			}
			if (!canItMove(monster[0].x, monster[0].y, -5, 0))
			{
				kierunek = -1;
			}
			if (!canItMove(monster[0].x, monster[0].y, 5, 0))
			{
				kierunek = 1;
			}


			//pasek zycia
			al_draw_filled_rectangle(50, 50, 200, 60, al_map_rgb(165, 0, 0));
			al_draw_filled_rectangle(50, 50, player.lives, 60, al_map_rgb(255, 0, 0));

			//pasek many
			al_draw_filled_rectangle(400, 50, 550, 60, al_map_rgb(30, 80, 70));
			al_draw_filled_rectangle(400, 50, 350 + player.mana, 60, al_map_rgb(30, 150, 250));

			// ładowanie many
			if (player.mana >= 0 && player.mana <= 200)
				player.mana += 0.4;

			if (player.lives == 50)
			{
				cout << "G A M E    O V E R !";
				al_rest(2.0);
				done = true;
			}

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}

	for (int i = 0; i < maxFrame; i++)
		al_destroy_bitmap(spritePlayer[i]);

	al_destroy_bitmap(bgClouds);
	al_destroy_bitmap(bgTiles);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);						//destroy our display object

	return 0;
}
void MovePlayerRight(Player &player)
{
	if (canItMove(player.x, player.y, 5, 0))
	{
		player.x += 5;
		player.side = 1;
	}
}
void MovePlayerLeft(Player &player)
{
	if (canItMove(player.x, player.y, -5, 0))
	{
		player.x -= 5;
		player.side = -1;
	}
}
void InitBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		bullet[i].speed = 10;
		bullet[i].alive = false;
	}
}
void DrawBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].alive)
		{
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 8, al_map_rgb(0, 255, 255));
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 3, al_map_rgb(255, 255, 255));
		}
	}
}
void FireBullet(Bullet bullet[], int size, Player &player)
{
	for (int i = 0; i < size; i++)
	{
		if (!bullet[i].alive)
		{
			bullet[i].x = player.x + 17 + xOff;
			bullet[i].y = player.y + 17;
			bullet[i].alive = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullet[], int size, Player &player)
{
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].alive)
		{
			bullet[i].x += bullet[i].speed;
			if (bullet[i].x > WIDTH || bullet[i].x < 0)
				bullet[i].alive = false;
		}
	}
}