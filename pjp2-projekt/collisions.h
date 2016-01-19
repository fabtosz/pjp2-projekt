#include <iostream>

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
// u¿ycie isCollide z konkretnymi rozmiarami prostok¹tów
bool isCollidePlayerTile(float player_x, float player_y, float tile_x, float tile_y)
{
	return isCollide(player_x, player_y, playerSizeX, playerSizeY, tile_x, tile_y, tileSize, tileSize);
}
bool isCollidePlayerGem(float player_x, float player_y, float tile_x, float tile_y)
{
	return isCollide(player_x, player_y, playerSizeX, playerSizeY, tile_x, tile_y, gemSize, gemSize);
}
bool canItMove(float player_x, float player_y,  /* wspó³rzêdne gracza */float x, float y /* o ile ruszyæ */)
{
	bool collision = false;

	//sprawdz kolizje dla ka¿dego kloca
	for (int i = 0; i < sizeArrayMap; i++)
	{
		//jeœli klocek jest powietrzem to go pomiñ
		if (map[i] != 0 && map[i] != 3 && map[i] != 4 && map[i] != 5 && map[i] != 6 && map[i] != 7)
		{
			if (isCollidePlayerTile(player_x + x, player_y + y, tileSize * (i % mapColumns), tileSize * (i / mapColumns)))
			{
				collision = true;
				break;
			}
		}
		//jeœli klocek jest itemem(diamentem, potionem)
		if (map[i] == 4 || map[i] == 5 || map[i] == 6 || map[i] == 7)
		{
			if (isCollidePlayerGem(player_x + x, player_y + y, tileSize * (i % mapColumns), tileSize * (i / mapColumns)))
			{
				map[i] = 0;
				std::cout << "B A N G!" << std::endl;
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
