#include <iostream>
#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Item.h"
#include "Pilha.h"


#define DISPLAY_W 960
#define DISPLAY_H 540
#define FPS 60
#define NUM_TIPOS 7

/*Função geradora do novo item*/
Item gera_item(ALLEGRO_BITMAP* bitmap[NUM_TIPOS], int x, int y)
{
	int random = rand() % (NUM_TIPOS);
	return Item(random, bitmap[random], x, y);
}

int main()
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP  *bg = NULL;
	ALLEGRO_BITMAP  *copo = NULL;
	ALLEGRO_BITMAP  *sel = NULL;
	ALLEGRO_BITMAP  *sel_down = NULL;
	ALLEGRO_BITMAP  *bebida[7] = { NULL };
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_EVENT_QUEUE *display_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_MOUSE_STATE state;
	ALLEGRO_SAMPLE *jump;
	
	//posicao do novo item
	int x_item = 300;
	int y_item = 50;

	bool done = false;
	bool g_novo = true;
	Pilha<Item> pilha[4];
	Pilha<Item> p_novo;
	Pilha<int> gabarito[4];
	Item novo;
	Item anterior;
	int p_selecionada = 0;
	int l_pilha = -1;
	bool gab_stat[4] = { false };

	/*Inicializações*/
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_init_image_addon()) {
		fprintf(stderr, "failed to init image addon!\n");
		return -1;
	}

	if (!al_install_mouse()) {
		fprintf(stderr, "Error installing mouse.\n");
		return -1;
	}
	

	display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	al_init_font_addon();

	al_init_primitives_addon();

	if (!al_install_audio()) {
		fprintf(stderr, "failed to initialize audio!\n");
		return -1;
	}

	if (!al_init_acodec_addon()) {
		fprintf(stderr, "failed to initialize audio codecs!\n");
		return -1;
	}

	if (!al_reserve_samples(1)) {
		fprintf(stderr, "failed to reserve samples!\n");
		return -1;
	}
	ALLEGRO_FONT* font = al_create_builtin_font();

	/*Load de bitmaps*/
	bg = al_load_bitmap("bg.jpg");
	copo = al_load_bitmap("copo.png");
	bebida[0] = al_load_bitmap("frutas/limao.png");
	bebida[1] = al_load_bitmap("frutas/tangerina.png");
	bebida[2] = al_load_bitmap("frutas/cereja.png");
	bebida[3] = al_load_bitmap("frutas/maca.png");
	bebida[4] = al_load_bitmap("frutas/uva.png");
	bebida[5] = al_load_bitmap("frutas/melancia.png");
	bebida[6] = al_load_bitmap("frutas/morango.png");
	sel = al_load_bitmap("sel.jpg");
	sel_down = al_load_bitmap("sel_down.jpg");
	if (!copo||!bg||!bebida[0] || !bebida[1] || !bebida[2] || !bebida[3] || !bebida[4] || !bebida[5] || !bebida[6]) {
		fprintf(stderr, "failed to load image!\n");
		al_destroy_display(display);
		return 0;
	}

	jump = al_load_sample("jump.wav");
	if (!jump) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}

	/*Display*/
	al_clear_to_color(al_map_rgb(0, 0, 0));
	timer = al_create_timer(1.0 / FPS);
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_start_timer(timer);



	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		//Background
		al_draw_bitmap(bg, 0, 0, 0);

		int i,j;
		for (i = 0; i < 4; i++)
		{
			if (gab_stat[i] == false)
			{
				gabarito[i].coloca(rand() % (NUM_TIPOS));
				gabarito[i].coloca(rand() % (NUM_TIPOS));
				gabarito[i].coloca(rand() % (NUM_TIPOS));
				gabarito[i].coloca(rand() % (NUM_TIPOS));
				gab_stat[i] = true;
			}
		}

		//mouse
		al_get_mouse_state(&state);
		if (state.buttons & 1) {
			if (state.y > 480 && state.y < 510)
			{
				if (state.x > 290 && state.x < 360)
				{
					p_selecionada = 0;
					x_item = 300;
				}
				else if (state.x > 460 && state.x < 530)
				{
					p_selecionada = 1;
					x_item = 470;
				}
				else if (state.x > 630 && state.x < 700)
				{
					p_selecionada = 2;
					x_item = 640;
				}
				else if (state.x > 800 && state.x < 870)
				{
					p_selecionada = 3;
					x_item = 810;
				}
			}
		}
		
		//criação do novo item
		if (g_novo)
		{
			g_novo = false;
			novo = gera_item(bebida, x_item, y_item);
			printf("Novo item gerado: tipo %d\n", novo.get_tipo());
			al_play_sample(jump, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		int gy = novo.get_y();
		if (gy <= 170)
		{
			al_draw_bitmap(novo.get_bitmap(), novo.get_x(), gy, 0);
			novo.set_pos(x_item, gy + 1);
			if (gy == 170)
			{
				l_pilha = p_selecionada;
				anterior = novo;
				g_novo = true;
				printf("Pilha escolhida: %d\n", l_pilha);
			}
		}
		
		//inserção do item na pilha apos colisao
		if (l_pilha != -1)
		{
			int ay = anterior.get_y();
			int yfinal = 390 - pilha[l_pilha].get_quant_itens() * 54;
			if ((pilha[l_pilha].cheia() == false) && ay != yfinal)
			{
				if (ay >= yfinal)
				{
					anterior.set_pos(anterior.get_x(), yfinal);
					pilha[l_pilha].coloca(anterior);
					printf("Item adicionado a Pilha %d\n", l_pilha);
					l_pilha = -1;
				}
				else
				{
					al_draw_bitmap(anterior.get_bitmap(), anterior.get_x(), ay, 0);
					anterior.set_pos(x_item, ay*1.1);
					
				}

			}
			else if (pilha[l_pilha].cheia() == true)
			{
				printf("Pilha %d cheia\n", l_pilha);
				l_pilha = -1;
			}
		}
		
		//Draw das pilhas
		for (i = 0; i<4; i++)
		{
			int t = 0;
			while (t<pilha[i].get_quant_itens())
			{
				Item temp = pilha[i].get_item(t);
				al_draw_bitmap(temp.get_bitmap(), temp.get_x(), temp.get_y(), 0);
				t++;
			}
		}

		//Draw das pilhas gabarito
		for (i = 0; i<4; i++)
		{
			for (j = 3; j >= 0;j--)
			{
				int tipo = gabarito[i].get_item(j);
				al_draw_bitmap(bebida[tipo], 20+52*i, 470-52*j, 0);
			}
		}
		
		//draw dos copos
		al_draw_bitmap(copo, 770, 220, 0);
		al_draw_bitmap(copo, 600, 220, 0);
		al_draw_bitmap(copo, 430, 220, 0);
		al_draw_bitmap(copo, 260, 220, 0);
		
		//Draw dos botoes
		al_draw_bitmap(sel, 290, 480, 0);
		al_draw_bitmap(sel, 460, 480, 0);
		al_draw_bitmap(sel, 630, 480, 0);
		al_draw_bitmap(sel, 800, 480, 0);
		switch (p_selecionada)
		{
		case 0:
			al_draw_bitmap(sel_down, 290, 480, 0);
			break;
		case 1:
			al_draw_bitmap(sel_down, 460, 480, 0);
			break;
		case 2:
			al_draw_bitmap(sel_down, 630, 480, 0);
			break;
		case 3:
			al_draw_bitmap(sel_down, 800, 480, 0);
			break;
		}

		//novo frame
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}

	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_sample(jump);

	return 0;
}