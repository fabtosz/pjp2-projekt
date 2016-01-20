void MovePlayerRight(Player &player)
{
	if (canItMove(player.x, player.y, 5, 0))
	{
		player.x += 4;
		player.dir = 1;
	}
}
void MovePlayerLeft(Player &player)
{
	if (canItMove(player.x, player.y, -5, 0))
	{
		player.x -= 4;
		player.dir = -1;
	}
}
void MovePlayerGravity(Player player)
{
	if (canItMove(player.x, player.y, 0, 5))
	{
		player.y += 5;
	}
}
void MonsterMove(Monster monster[], int NUM_MONSTERS)
{
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
		//ruch w boki
		if (canItMove(monster[i].x, monster[i].y, 0, 0))
		{
			monster[i].x -= 2 * monster[i].side;
		}
		if (!canItMove(monster[i].x, monster[i].y, -5, 0))
		{
			monster[i].side = -1;
		}
		if (!canItMove(monster[i].x, monster[i].y, 5, 0))
		{
			monster[i].side = 1;
		}
	}
}