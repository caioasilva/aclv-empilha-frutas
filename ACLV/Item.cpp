#include "Item.h"



Item::Item(int t, ALLEGRO_BITMAP* bit, int pX, int pY)
{
	tipo = t;
	bitmap = bit;
	x = pX;
	y = pY;
}


Item::~Item()
{
}

void Item::set_pos(int px, int py)
{
	x = px;
	y = py;
}

void Item::set_y_mais()
{
	y++;
}

int Item::get_x() const
{
	return x;
}

int Item::get_y() const
{
	return y;
}

ALLEGRO_BITMAP* Item::get_bitmap() const
{
	return bitmap;
}

int Item::get_tipo() const
{
	return tipo;
}
