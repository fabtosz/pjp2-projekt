#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>

// globalne
const int width = 800;
const int height = 400;

int main(void)
{
	
	bool done = false;

	int x = 0;
	int y = 300;

	const int maxFrame = 6;
	int curFrame = 0;
	int frameCount = 0;
	int frameDelay = 7;

	//allegro variable
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer;
	ALLEGRO_BITMAP *image[maxFrame];
	ALLEGRO_BITMAP *splash;

	//program init
	if (!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(width, height);			//create our display object

	if (!display)										//test display object
		return -1;

	//addon init
	al_install_keyboard();
	al_init_image_addon();

	image[0] = al_load_bitmap("00.gif");
	image[1] = al_load_bitmap("01.gif");
	image[2] = al_load_bitmap("02.gif");
	image[3] = al_load_bitmap("03.gif");
	image[4] = al_load_bitmap("04.gif");
	image[5] = al_load_bitmap("05.gif");

	splash = al_load_bitmap("splash.jpg");
	al_draw_bitmap(splash, 0, 0, 0);
	al_flip_display();
	al_rest(3.0);
	al_destroy_bitmap(splash);

	for (int i = 0; i < maxFrame; i++)
		al_convert_mask_to_alpha(image[i], al_map_rgb(0, 0, 0));

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

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
				x -= 10;

				if (x <= 0 - al_get_bitmap_width(image[curFrame]))
					x = width;
				break;
			case ALLEGRO_KEY_RIGHT:
				x += 10;

				if (x <= 0 - al_get_bitmap_width(image[curFrame]))
					x = width;
				break;
			case ALLEGRO_KEY_UP:

				break;
			case ALLEGRO_KEY_DOWN:

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

			
		}

		al_draw_bitmap(image[curFrame], x, y, 0);

		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}

	for (int i = 0; i < maxFrame; i++)
		al_destroy_bitmap(image[i]);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);						//destroy our display object

	return 0;
}