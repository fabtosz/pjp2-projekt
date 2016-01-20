#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "resources/objects.h"
#include "resources/collisions.h"
#include "resources/movement.h"
#include "resources/main_menu.h"

using namespace std;

//stałe
const int WIDTH = 900;
const int HEIGHT = 600;
const int NUM_BULLETS = 3;
const int NUM_MONSTERS = 5;

bool keys[] = { false, false, false, false, false };
enum KEYS{ UP, DOWN, LEFT, RIGHT, SPACE };

void ShowMainMenu(bool isStarted, ALLEGRO_BITMAP *mainPage, bool done, ALLEGRO_EVENT_QUEUE *event_queue);

bool isCollide(float pos1_x, float pos1_y, float size1_x, float size1_y, float pos2_x, float pos2_y, float size2_x, float size2_y);
bool isCollidePlayerTile(float player_x, float player_y, float tile_x, float tile_y);
bool canItMove(float player_x, float player_y,  /* współrzędne gracza */float x, float y /* o ile ruszyć */);
bool isOnSolidGround(int player_x, int player_y);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, Player &player);
void UpdateBullet(Bullet bullet[], int size, Player &player);

void MovePlayerRight(Player &player);
void MovePlayerLeft(Player &player);

void InitMonster(Monster monster[], int numMonsters, float x, float y);

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
	ALLEGRO_BITMAP *player_r = NULL;
	ALLEGRO_BITMAP *player_l = NULL;
	ALLEGRO_BITMAP *monster_r = NULL;
	ALLEGRO_BITMAP *monster_l = NULL;
	ALLEGRO_BITMAP *mainPage = NULL;
	ALLEGRO_BITMAP *endGame = NULL;
	ALLEGRO_BITMAP *victory = NULL;
	ALLEGRO_BITMAP *gem1 = NULL;
	ALLEGRO_BITMAP *gem2 = NULL;
	ALLEGRO_BITMAP *gem3 = NULL;
	ALLEGRO_BITMAP *gem4 = NULL;
	ALLEGRO_BITMAP *fire = NULL;

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
	bgTiles = al_load_bitmap("images/tiles.gif");
	bgClouds = al_load_bitmap("images/background.png");
	gem1 = al_load_bitmap("images/gem1.png");
	gem2 = al_load_bitmap("images/gem2.png");
	gem3 = al_load_bitmap("images/gem3.png");
	gem4 = al_load_bitmap("images/gem4.png");

	// gracz
	Player player;
	player_r = al_load_bitmap("images/player_r.gif");
	player_l = al_load_bitmap("images/player_l.gif");

	// grafiki menu
	mainPage = al_load_bitmap("images/mainpage.jpg");
	endGame = al_load_bitmap("images/death.jpg");
	victory = al_load_bitmap("images/victory.jpg");

	// Współrzędne początkowe gracza
	player.x = 50;
	player.y = 50;

	// pociski
	Bullet bullets[NUM_BULLETS];
	InitBullet(bullets, NUM_BULLETS);

	// potwory i przeszkody
	Monster monster[NUM_MONSTERS];
	monster_r = al_load_bitmap("images/monster_r.gif");
	monster_l = al_load_bitmap("images/monster_l.gif");
	fire = al_load_bitmap("images/fire.gif");

	// Współrzędne potworow
	InitMonster(monster, 0, 100, 0);
	InitMonster(monster, 1, 1000, 100);
	InitMonster(monster, 2, 800, 100);
	InitMonster(monster, 3, 1300, 100);
	InitMonster(monster, 4, 1600, 50);

	// kolejka zdarzen i timer
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	
	int wysokoscSkoku = 0;
	int points = 0;
	bool isStarted = false;
	bool mayJumpAgain = true;

	ShowMainMenu(isStarted, mainPage, done, event_queue);

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
			for (int i = 0; i < NUM_MONSTERS; i++)
			{
				if (isCollide(player.x, player.y, playerSizeX, playerSizeY, monster[i].x, monster[i].y, monster[i].sizeX, monster[i].sizeY) && player.alive)
					player.lives--;
			}

			//kolizja z przepascia
			if (player.y > HEIGHT + 50)
			{
				player.alive = false;
				player.lives -= 50;
				player.y = 0;
			}
			player.alive = true;

			//kolizja pociskow z potworami
			for (int i = 0; i < NUM_BULLETS; i++)
			{
				for (int j = 0; j < NUM_MONSTERS; j++)
				{
					if (isCollide(bullets[i].x, bullets[i].y, bullets[i].size, bullets[i].size, monster[j].x + xOff, monster[j].y + yOff, monster[j].sizeX, monster[j].sizeY))
					{
						bullets[i].alive = false;
						monster[j].lives--;
						
						if (monster[j].lives == 0)
						{
							monster[j].x = -100;
							monster[j].y = -100;
							monster[j].alive = false;
						}


						bullets[i].alive = false;
						bullets[i].x = -100;
						bullets[i].y = -100;

					}
				}
			}

			//grawitacja potworow
			for (int i = 0; i < NUM_MONSTERS; i++)
			{
				//grawitacja
				if (!isOnSolidGround(monster[i].x, monster[i].y))
				{
					if (canItMove(monster[i].x, monster[i].y, 0, 5))
					{
						monster[i].y += 5;
					}
				}
			}

			//kolizja potworow z przepascia
			for (int i = 0; i < NUM_MONSTERS; i++)
			{
				if (monster[i].y > HEIGHT)
					monster[i].alive = false;
			}

			//ruch potworow z odbiciem
			MonsterMove(monster, NUM_MONSTERS);

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

			//warunek zwyciestwa
			if ((player.x + player.x + xOff) > 3000)
			{
				al_draw_bitmap(victory, 0, 0, 0);
				al_flip_display();
				al_rest(2.0);
				done = true;
			}

			render = true;
		}

		/* R E N D E R O W A N I E */
		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;

			//rysuj tlo
			al_draw_bitmap(bgClouds, 0 + xOff / 8, 0, 0);

			//rysuj mapę
			for (int i = 0; i < sizeArrayMap; i++)
			{
				if (map[i] == 0 || map[i] == 1 || map[i] == 2 || map[i] == 3 || map[i] == 4)
				al_draw_bitmap_region(bgTiles, tileSize * map[i], 0, tileSize, tileSize,
					xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);

				//skarby
				if (map[i] == 5)
				{
					al_draw_bitmap_region(gem1, curFrame * 50, 0, 50, 50,/*gdzie rysowac*/ xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
				}
				if (map[i] == 6)
				{
					al_draw_bitmap_region(gem2, curFrame * 50, 0, 50, 50,/*gdzie rysowac*/ xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
				}
				if (map[i] == 7)
				{
					al_draw_bitmap_region(gem3, curFrame * 50, 0, 50, 50,/*gdzie rysowac*/ xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
				}
				if (map[i] == 8)
				{
					al_draw_bitmap_region(gem4, curFrame * 50, 0, 50, 50,/*gdzie rysowac*/ xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
				}
				
			}
			
			//rysuj gracza
			if (player.alive)
			{
				if (player.dir == 1)
					al_draw_bitmap_region(player_r, curFrame * 70, 0, 70, 94,/*gdzie rysowac*/ player.x + xOff, player.y + yOff, 0);
				if (player.dir == -1)
					al_draw_bitmap_region(player_l, curFrame * 70, 0, 70, 94,/*gdzie rysowac*/ player.x + xOff, player.y + yOff, 0);
			}
				
			//rysuj pociski
			DrawBullet(bullets, NUM_BULLETS);
			UpdateBullet(bullets, NUM_BULLETS, player);

			//rysuj potwory
			for (int i = 0; i < NUM_MONSTERS; i++)
			{
				if (monster[i].alive)
				{
					if (monster[i].side == 1)
						al_draw_bitmap_region(monster_l, curFrame * 53, 0, 53, 85,/*gdzie rysowac*/ monster[i].x + xOff, monster[i].y + yOff, 0);
					if (monster[i].side == -1)																			 
						al_draw_bitmap_region(monster_r, curFrame * 53, 0, 53, 85,/*gdzie rysowac*/ monster[i].x + xOff, monster[i].y + yOff, 0);
				}
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

			if (player.lives <= 50)
			{
				al_draw_bitmap(endGame, 0, 0, 0);
				al_flip_display();
				al_rest(3.0);
				done = true;
			}

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}

	al_destroy_bitmap(bgClouds);
	al_destroy_bitmap(bgTiles);
	al_destroy_bitmap(mainPage);
	al_destroy_bitmap(endGame);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
	

	return 0;
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
			bullet[i].dir = player.dir;
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
			bullet[i].x += bullet[i].speed * bullet[i].dir;
			if (bullet[i].x > WIDTH || bullet[i].x < 0)
			{
				bullet[i].alive = false;
				bullet[i].x = -100;
				bullet[i].y = -100;
			}

		}
	}
}
void InitMonster(Monster monster[], int indeks, float x, float y)
{
	monster[indeks].x = x;
	monster[indeks].y = y;
}