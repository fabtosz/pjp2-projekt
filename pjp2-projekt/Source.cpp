#include <allegro5\allegro.h>

enum KEYS{ UP, DOWN, LEFT, RIGHT };

int main(void)
{
	int wysokosc = 600;
	int szerokosc = 400;

	bool done = false;

	ALLEGRO_DISPLAY *display = NULL;

	if (!al_init())
	{
		return -1;
	}

	display = al_create_display(wysokosc, szerokosc);

	if (!display)
	{
		return -1;
	}

	al_destroy_display(display);

	return 0;
}