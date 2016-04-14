#include <allegro5/allegro.h>

#pragma once
class Item
{
public:
	Item(int t=-1, ALLEGRO_BITMAP* bit=NULL, int pX=0, int pY=0);
	~Item();
	void set_pos(int px, int py);
	void set_y_mais();
	int get_x() const;
	int get_y() const;
	ALLEGRO_BITMAP* get_bitmap() const;
	int get_tipo() const;

private:
	int tipo;
	int x;
	int y;
	ALLEGRO_BITMAP* bitmap;
};

