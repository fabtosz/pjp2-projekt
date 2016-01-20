#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

void ShowMainMenu(bool isStarted, ALLEGRO_BITMAP *mainPage, bool done, ALLEGRO_EVENT_QUEUE *event_queue)
{
	while (!isStarted)
	{
		al_draw_bitmap(mainPage, 0, 0, 0);
		al_flip_display();

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				isStarted = true;
				done = true;
				break;
			case ALLEGRO_KEY_ENTER:
				isStarted = true;
				break;
			}
		}
	}
}