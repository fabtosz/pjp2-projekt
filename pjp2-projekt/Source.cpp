#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

const int WIDTH = 600;
const int HEIGHT = 500;

bool keys[] = { false, false, false, false };
enum KEYS{ UP, DOWN, LEFT, RIGHT };

int main(void)
{
	//variables
	bool done = false;
	bool render = false;

	int xOff = 0;
	int yOff = 0;

	int mapColumns = 20;
	int mapSize = 200;
	int tileSize = 50;

	int map[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		          0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0,
		          2, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 2, 2, 2,
		          1, 2, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1,
		          1, 1, 2, 2, 1, 1, 1, 1, 0, 0, 0, 2, 1, 1, 0, 0, 0, 1, 1, 1,
		          1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1 };

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
	ALLEGRO_BITMAP *splash;

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

	// sprite
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

	splash = al_load_bitmap("splash.jpg");
	al_draw_bitmap(splash, 0, 0, 0);
	al_flip_display();
	al_rest(3.0);
	al_destroy_bitmap(splash);

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

			xOff -= keys[RIGHT] * 5;
			xOff += keys[LEFT] * 5;


			render = true;
		}

		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;

			al_draw_bitmap(bgClouds, 0, 0, 0);

			for (int i = 0; i < mapSize; i++)
			{
				al_draw_bitmap_region(bgTiles, tileSize * map[i], 0, tileSize, tileSize,
					xOff + tileSize * (i % mapColumns), yOff + tileSize * (i / mapColumns), 0);
			}

			al_draw_bitmap(sprite[curFrame], 200 + xOff, 210, 0);

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