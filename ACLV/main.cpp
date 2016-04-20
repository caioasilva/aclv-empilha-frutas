#include <iostream>
#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include "Item.h"
#include "Pilha.h"
#include <vector>
#include <algorithm>  
#include <ctime>        // std::time
#include <string>

using std::vector;
using std::string;

#define DISPLAY_W 960
#define DISPLAY_H 540
#define FPS 60
#define NUM_TIPOS 7
#define PONTOS_PILHA_CERTA 100
#define Y_ITEM_INICIAL 30
#define Y_ITEM_FINAL 390
#define Y_DELTA_ITEM 54
#define X_ITEM_0 300
#define X_DELTA_ITEM 170
#define X_PRIMEIRO_COPO 260
#define X_DELTA_COPO 170
#define Y_COPO 220
#define Y_BOTAO_INICIAL 475
#define Y_BOTAO_FINAL 505
#define X_PRIMEIRO_BOTAO 292
#define X_TAMANHO_BOTAO 70
#define Y_COLISAO 170
#define Y_VEL_QUEDA_COPO 1.1
#define X_PRIMEIRO_ANIM 223
#define Y_PRIMEIRO_ANIM 190

bool defineTiros(int& tiro, vector<int>& tipo_tiro, Pilha<Item> pilha[], Pilha<int> gabarito[])
{
	if (tiro < 0)
	{
		tipo_tiro.erase(tipo_tiro.begin(), tipo_tiro.end());
		printf("Criando fila de lançamento: \n");
		int i;
		for (i = 0; i < 4; i++)
		{
			int quant = pilha[i].get_quant_itens();
			int ritem = gabarito[i].get_item(quant);
			printf("Pilha: %d Andar da pilha: %d Item: %d\n", i, quant, ritem);
			tipo_tiro.push_back(ritem);
		}
		std::random_shuffle(tipo_tiro.begin(), tipo_tiro.end());
		tiro = 3;
		return true;
	}
	else {
		return false;
	}
}

int main()
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP  *bg = NULL;
	ALLEGRO_BITMAP  *copo = NULL;
	ALLEGRO_BITMAP  *sel = NULL;
	ALLEGRO_BITMAP  *sel_down = NULL;
	ALLEGRO_BITMAP  *bebida[7] = { NULL };
	ALLEGRO_BITMAP  *tb_bebida[7] = { NULL };
	ALLEGRO_BITMAP  *anim[4][22] = { NULL };
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_EVENT_QUEUE *display_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_MOUSE_STATE state;
	ALLEGRO_SAMPLE *jump;


	std::srand(unsigned(std::time(0)));
	bool done = false;
	bool item_novo = true;
	int tiro = -1;
	vector<int> tipo_tiro;
	tipo_tiro.reserve(4);
	Pilha<Item> pilha[4];
	Pilha<Item> p_novo;
	Pilha<int> gabarito[4];
	Item novo;
	Item anterior;
	int p_selecionada = 0;
	int l_pilha = -1;
	bool gab_stat[4] = { false };
	bool KeyDown = false;
	int Pontos = 0;
	int i, j;
	int pontuou[3] = { -1 };
	int vel_queda = 1;

	//geração da posicao dos botoes
	int X_BOTAO[4][2];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 2;j++)
		{
			X_BOTAO[i][j] = X_PRIMEIRO_BOTAO + i*X_DELTA_COPO + j*X_TAMANHO_BOTAO;
		}

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

	al_init_ttf_addon();// initialize the ttf (True Type Font) addon


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
	ALLEGRO_FONT *font = al_load_ttf_font("KOMIKAX_.ttf", 28, 0);
	if (!font) {
		fprintf(stderr, "Could not load 'KOMIKAX_.ttf'.\n");
		return -1;
	}
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}


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
	tb_bebida[0] = al_load_bitmap("frutas/tb-limao.png");
	tb_bebida[1] = al_load_bitmap("frutas/tb-tangerina.png");
	tb_bebida[2] = al_load_bitmap("frutas/tb-cereja.png");
	tb_bebida[3] = al_load_bitmap("frutas/tb-maca.png");
	tb_bebida[4] = al_load_bitmap("frutas/tb-uva.png");
	tb_bebida[5] = al_load_bitmap("frutas/tb-melancia.png");
	tb_bebida[6] = al_load_bitmap("frutas/tb-morango.png");
	sel = al_load_bitmap("sel.jpg");
	sel_down = al_load_bitmap("sel_down.jpg");
	if (!copo||!bg||!bebida[0] || !bebida[1] || !bebida[2] || !bebida[3] || !bebida[4] || !bebida[5] || !bebida[6]) {
		fprintf(stderr, "failed to load image!\n");
		al_destroy_display(display);
		return 0;
	}

	for (j = 1; j <= 4; j++)
	{
		for (i = 0; i < 22; i++)
		{
			string nome = "anims/" + std::to_string(j*25) +"/" + std::to_string(i) + ".png";
			anim[j-1][i] = al_load_bitmap(nome.c_str());
		}
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

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));

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

		//Keyboard
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN && KeyDown == false)///look for keyboard events
		{
			KeyDown = true;
			if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT && p_selecionada <3)
			{
				++p_selecionada;

			}else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT && p_selecionada>0)
			{
				--p_selecionada;
			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{
				vel_queda=2;
			}
		}else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			KeyDown = false;
			if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
				vel_queda = 1;
		}


		//mouse
		al_get_mouse_state(&state);
		if (state.buttons & 1) {
			if (state.y > Y_BOTAO_INICIAL && state.y < Y_BOTAO_FINAL)
			{
				if (state.x > X_BOTAO[0][0] && state.x < X_BOTAO[0][1])
				{
					p_selecionada = 0;
				}
				else if (state.x > X_BOTAO[1][0] && state.x < X_BOTAO[1][1])
				{
					p_selecionada = 1;
				}
				else if (state.x > X_BOTAO[2][0] && state.x < X_BOTAO[2][1])
				{
					p_selecionada = 2;
				}
				else if (state.x > X_BOTAO[3][0] && state.x < X_BOTAO[3][1])
				{
					p_selecionada = 3;
				}
			}
		}
	

		//criação do novo item
		if (item_novo)
		{
			defineTiros(tiro, tipo_tiro, pilha, gabarito);
			item_novo = false;
			int tipo = tipo_tiro.at(tiro--);
			novo = Item(tipo, bebida[tipo], X_ITEM_0 + p_selecionada * X_DELTA_ITEM, Y_ITEM_INICIAL);
			printf("Novo item gerado: tipo %d\n", novo.get_tipo());
			al_play_sample(jump, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}


		//Draw do item e COLISAO
		int gy = novo.get_y();
		if (gy < Y_COLISAO)
		{
			al_draw_bitmap(novo.get_bitmap(), novo.get_x(), gy, 0);
			novo.set_pos(X_ITEM_0 + p_selecionada * X_DELTA_ITEM, gy + vel_queda);
			
		}else if (gy >= Y_COLISAO)
		{
			l_pilha = p_selecionada;
			anterior = novo;
			item_novo = true;
			printf("Pilha escolhida: %d\n", l_pilha);
			if (!(pilha[l_pilha].cheia()))
			{
				int yfinal = Y_ITEM_FINAL - pilha[l_pilha].get_quant_itens() * Y_DELTA_ITEM;
				novo.set_pos(X_ITEM_0 + p_selecionada * X_DELTA_ITEM, yfinal);
				pilha[l_pilha].coloca(novo);
				printf("Item adicionado a Pilha %d\n", l_pilha);
				//anterior.set_pos(X_ITEM_0 + p_selecionada * X_DELTA_ITEM, gy );
			}
			else
			{
				printf("Pilha %d cheia\n", l_pilha);
				l_pilha = -1;
			}
		}
		
		//animação apos colisao
		if (l_pilha != -1)
		{
			int ay = anterior.get_y();
			int yfinal = Y_ITEM_FINAL - pilha[l_pilha].get_quant_itens() * Y_DELTA_ITEM;

				if (ay >= yfinal)
				{
					anterior.set_pos(X_ITEM_0 + l_pilha * X_DELTA_ITEM, yfinal);
					l_pilha = -1;
				}
				else
				{
					al_draw_bitmap(anterior.get_bitmap(), anterior.get_x(), ay, 0);
					anterior.set_pos(X_ITEM_0 + l_pilha * X_DELTA_ITEM, ay*Y_VEL_QUEDA_COPO);
				}

		}
		
		//Loop pilhas
		for (i = 0; i<4; i++)
		{
			//Draw
			int t = 0;
			int quant = pilha[i].get_quant_itens();
			if (l_pilha == i)
				quant--;
			while (t<quant)
			{
				Item temp = pilha[i].get_item(t);
				al_draw_bitmap(temp.get_bitmap(), temp.get_x(), temp.get_y(), 0);
				t++;
			}

			//Draw dos gabaritos
			for (j = 3; j >= 0; j--)
			{
				int tipo = gabarito[i].get_item(j);
				al_draw_bitmap(tb_bebida[tipo], 240 + 170 * i, 430 - 30 * j, 0);
			}

			//Detecção se ta cheia + pontuação
			if (pilha[i].cheia())
			{
				int iguais = 0;
				for (j = 0; j < 4; j++)
				{
					if (pilha[i].retira().get_tipo() == gabarito[i].retira())
					{
						iguais++;
					}
				}
				switch (iguais)
				{
				case 4:
					Pontos += PONTOS_PILHA_CERTA;
					printf("Pilha igual. %d pontos add\n", PONTOS_PILHA_CERTA);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 3;
					break;
				case 3:
					Pontos += PONTOS_PILHA_CERTA * 3 / 4;
					printf("3 certos. %d pontos add\n", PONTOS_PILHA_CERTA * 3 / 4);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 2;
					break;
				case 2:
					Pontos += PONTOS_PILHA_CERTA / 2;
					printf("2 certos. %d pontos add\n", PONTOS_PILHA_CERTA / 2);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 1;
					break;
				case 1:
					Pontos += PONTOS_PILHA_CERTA * 1 / 4;
					printf("1 certos. %d pontos add\n", PONTOS_PILHA_CERTA * 1 / 4);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 0;
					break;
				default:
					break;
				}
				gab_stat[i] = false;
			}
		}
		
		//draw dos copos
		al_draw_bitmap(copo, X_PRIMEIRO_COPO + 3*X_DELTA_COPO, Y_COPO, 0);
		al_draw_bitmap(copo, X_PRIMEIRO_COPO + 2*X_DELTA_COPO, Y_COPO, 0);
		al_draw_bitmap(copo, X_PRIMEIRO_COPO + X_DELTA_COPO, Y_COPO, 0);
		al_draw_bitmap(copo, X_PRIMEIRO_COPO, Y_COPO, 0);
		
		//Draw dos botoes
		al_draw_bitmap(sel, X_BOTAO[0][0], Y_BOTAO_INICIAL, 0);
		al_draw_bitmap(sel, X_BOTAO[1][0], Y_BOTAO_INICIAL, 0);
		al_draw_bitmap(sel, X_BOTAO[2][0], Y_BOTAO_INICIAL, 0);
		al_draw_bitmap(sel, X_BOTAO[3][0], Y_BOTAO_INICIAL, 0);
		al_draw_bitmap(sel_down, X_BOTAO[p_selecionada][0], Y_BOTAO_INICIAL, 0);


		//Animação pontos
		if (pontuou[0] >= 0)
		{
			al_draw_bitmap(anim[pontuou[2]][(int)(pontuou[0]/2)], X_PRIMEIRO_ANIM + pontuou[1] * X_DELTA_COPO, Y_PRIMEIRO_ANIM, 0);
			pontuou[0]++;
			if (pontuou[0] == 43)
				pontuou[0] = -1;
		}

		//HUD
		char num[5];
		_itoa_s(Pontos, num, 10);
		al_draw_text(font, al_map_rgb(255,255,255), 100, 400, ALLEGRO_ALIGN_CENTER, "Placar: ");
		al_draw_text(font, al_map_rgb(255, 255, 255), 100, 430, ALLEGRO_ALIGN_CENTER, num);

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
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