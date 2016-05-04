#include <iostream>
#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_acodec.h>
#include "Item.h"
#include "Pilha.h"
#include <vector>
#include <algorithm>  
#include <ctime>
#include <string>

using std::vector;
using std::string;

#define DISPLAY_W 800
#define DISPLAY_H 600
#define FPS 60
#define NUM_TIPOS 7
#define VIDAS_INICIAL 4
#define PONTOS_PILHA_CERTA 100
#define ACELERACAO_INICIAL 0.5
#define Y_ITEM_INICIAL 50
#define Y_ITEM_FINAL 480
#define Y_DELTA_ITEM 54
#define X_ITEM_0 140
#define X_DELTA_ITEM 170
#define X_PRIMEIRO_COPO 100
#define X_DELTA_COPO 170
#define Y_COPO 310
#define Y_BOTAO_INICIAL 565
#define Y_BOTAO_FINAL 595
#define Y_COLISAO 260
#define Y_VEL_QUEDA_COPO 1.1
#define X_PRIMEIRO_ANIM 63
#define Y_PRIMEIRO_ANIM 220
#define X_PRIMEIRA_VIDA 420
#define Y_PRIMEIRA_VIDA 9
#define X_DELTA_VIDA 36
#define Y_GABARITO_PRIMEIRO 430
#define Y_PLACAR -2
#define X_PLACAR 55
#define X_GABARITO 80

//variaveis globais
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP  *bg = NULL;
ALLEGRO_BITMAP  *loading = NULL;
ALLEGRO_BITMAP  *gameover = NULL;
ALLEGRO_BITMAP  *menu = NULL;
ALLEGRO_BITMAP  *ajuda = NULL;
ALLEGRO_BITMAP  *copo = NULL;
ALLEGRO_BITMAP  *icon = NULL;
ALLEGRO_BITMAP  *vida = NULL;
ALLEGRO_BITMAP  *bebida[7] = { NULL };
ALLEGRO_BITMAP  *tb_bebida[7] = { NULL };
ALLEGRO_BITMAP  *anim[4][22] = { NULL };
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT_QUEUE *display_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_MOUSE_STATE state;
ALLEGRO_SAMPLE *jump;
ALLEGRO_SAMPLE *ring;
ALLEGRO_SAMPLE *musicgame;
ALLEGRO_FONT *font;
ALLEGRO_FONT *pontos_gameover;
int i, j;

//Função que realiza um efeito de fade out nas telas
void fadeout(int velocidade)
{
	ALLEGRO_BITMAP *buffer = NULL;
	ALLEGRO_EVENT ev;
	buffer = al_create_bitmap(DISPLAY_W, DISPLAY_H);
	al_set_target_bitmap(buffer);
	al_draw_bitmap(al_get_backbuffer(display), 0, 0, 0);
	al_set_target_bitmap(al_get_backbuffer(display));

	if (velocidade <= 0)
	{
		velocidade = 1;
	}
	else if (velocidade > 15)
	{
		velocidade = 15;
	}

	int alfa;
	for (alfa = 0; alfa <= 255; alfa += velocidade)
	{
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
		al_draw_tinted_bitmap(buffer, al_map_rgba(255 - alfa, 255 - alfa, 255 - alfa, alfa), 0, 0, 0);
		al_wait_for_event(event_queue, &ev);
		al_flip_display();
	}

	al_destroy_bitmap(buffer);
}

//Função que realiza um efeito de fade in nas telas
void fadein(ALLEGRO_BITMAP *imagem, int velocidade)
{
	ALLEGRO_EVENT ev;
	
	if (velocidade < 0)
	{
		velocidade = 1;
	}
	else if (velocidade > 15)
	{
		velocidade = 15;
	}

	int alfa;
	for (alfa = 0; alfa <= 255; alfa += velocidade)
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_tinted_bitmap(imagem, al_map_rgba(alfa, alfa, alfa, alfa), 0, 0, 0);
		al_wait_for_event(event_queue, &ev);
		al_flip_display();
	}
}

//Função geradora do tipo da proxima fruta a ser lançada
int defineTiro(Pilha<Item> pilha[], Pilha<int> gabarito[])
{
	vector<int> tipo_tiro;
	tipo_tiro.reserve(4);
	//printf("Criando lançamentos: \n");
	int i;
	for (i = 0; i < 4; i++)
	{
		int quant = pilha[i].get_quant_itens();
		int ritem = gabarito[i].get_item(quant);
		//printf("Pilha: %d Andar da pilha: %d Item: %d\n", i, quant, ritem);
		tipo_tiro.push_back(ritem);
	}
	std::random_shuffle(tipo_tiro.begin(), tipo_tiro.end());
	return tipo_tiro.at(0);

}

//Tela de menu
int iniciaMenu()
{
	int botao = -1;

	while (botao==-1) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		al_draw_bitmap(menu, 0, 0, 0);

		al_get_mouse_state(&state);
		if (state.buttons & 1)
			if(state.y > 499 && state.y < 550)
				if (state.x > 127 && state.x < 294)
				{
					botao = 1;
					fadeout(10);
				}else if (state.x > 315 && state.x < 483)
				{
					botao = 2;
					fadeout(10);
				}
				else if(state.x > 505 && state.x < 672)
				{
					botao = 0;
				}


		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			return 0;
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}
	
	return botao;

}

//Tela de ajuda
bool iniciaAjuda()
{
	bool botao = false;

	while (!botao) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		al_draw_bitmap(ajuda, 0, 0, 0);

		al_get_mouse_state(&state);
		if (state.buttons & 1)
			if ((state.y > 483 && state.y < 550)&& (state.x > 28 && state.x < 201))
				{
					botao = true;
					fadeout(10);
				}
				if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
					return false;
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
	}
	return botao;
}

//Jogo
bool iniciaJogo()
{
	Pilha<Item> pilha[4];
	int Pontos = 0;
	bool item_novo = true;
	vector<int> tipo_tiro;
	tipo_tiro.reserve(4);
	Pilha<int> gabarito[4];
	int p_selecionada = 0;
	int l_pilha = -1;
	bool KeyDown = false;
	bool gab_stat[4] = { false };
	int pontuou[3] = { -1 };
	int vidas = VIDAS_INICIAL;
	float aceleracao = ACELERACAO_INICIAL;
	int vel_queda = 1;
	bool acel_vel = false;
	Item novo;
	Item anterior;
	bool acabou = false;
	int frames_vel = 0;
	bool fade = true;
	bool fout = false;

	ALLEGRO_SAMPLE_INSTANCE* songInstance = al_create_sample_instance(musicgame);
	al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);

	al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());
	al_play_sample_instance(songInstance);
	while (!acabou)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		//Background
		al_draw_bitmap(bg, 0, 0, 0);

		//Keyboard
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN && KeyDown == false)///look for keyboard events
		{
			KeyDown = true;
			if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT && p_selecionada < 3)
			{
				++p_selecionada;

			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT && p_selecionada>0)
			{
				--p_selecionada;
			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{
				acel_vel = true;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			KeyDown = false;
			if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
				acel_vel = false;
		}

		//gabaritos
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

		//criação do novo item
		if (item_novo&&vidas>0)
		{
			int tipo = defineTiro(pilha, gabarito);
			item_novo = false;
			novo = Item(tipo, bebida[tipo], X_ITEM_0 + p_selecionada * X_DELTA_ITEM, Y_ITEM_INICIAL);
			//printf("Novo item gerado: tipo %d\n", novo.get_tipo());
			al_play_sample(jump, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			aceleracao += 0.1;
		}

		int gy = novo.get_y();
		if (gy < Y_COLISAO)
		{
			al_draw_bitmap(novo.get_bitmap(), novo.get_x(), gy, 0);
			int mais = vel_queda;
			int numero = 6-aceleracao;
			frames_vel++;

			if (numero == 0)
			{
				vel_queda++;
				aceleracao = 0;
				mais = vel_queda;
				frames_vel = 0;
			}

			else if (frames_vel >= numero)
			{
				frames_vel=0;
				mais++;
			}

			if (acel_vel)
				mais *= 2;
			novo.set_pos(X_ITEM_0 + p_selecionada * X_DELTA_ITEM, gy + mais);
		}
		else if (gy >= Y_COLISAO)
		{
			l_pilha = p_selecionada;
			anterior = novo;
			item_novo = true;
			if (!(pilha[l_pilha].cheia()))
			{
				int yfinal = Y_ITEM_FINAL - pilha[l_pilha].get_quant_itens() * Y_DELTA_ITEM;
				novo.set_pos(X_ITEM_0 + p_selecionada * X_DELTA_ITEM, yfinal);
				pilha[l_pilha].coloca(novo);
			}
			else
			{
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
		for (i = 0; i < 4; i++)
		{
			//Draw
			int t;
			int quant = pilha[i].get_quant_itens();
			Pilha<Item> aux(4);
			for (t = 0; t < quant;t++)
			{
				Item temp = pilha[i].retira();
				aux.coloca(temp);
				if (!(l_pilha == i&&t==0))
				{
					al_draw_bitmap(temp.get_bitmap(), temp.get_x(), temp.get_y(), 0);
				}
			}
			for (t = 0; t < quant; t++)
			{
				Item temp = aux.retira();
				pilha[i].coloca(temp);
			}

			
			Pilha<int> auxGab(4);
			//Draw dos gabaritos
			for (j = 0; j < 4; j++)
			{
				int tipo = gabarito[i].retira();
				al_draw_bitmap(tb_bebida[tipo], X_GABARITO + 170 * i, Y_GABARITO_PRIMEIRO + 30 * j, 0);
				auxGab.coloca(tipo);
			}
			for (j = 0; j < 4; j++)
			{
				int tipo = auxGab.retira();
				gabarito[i].coloca(tipo);
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
					//printf("Pilha igual. %d pontos add\n", PONTOS_PILHA_CERTA);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 3;
					if (vidas < 10)
						vidas++;
					aceleracao += 0.1;
					break;
				case 3:
					Pontos += PONTOS_PILHA_CERTA * 3 / 4;
					//printf("3 certos. %d pontos add\n", PONTOS_PILHA_CERTA * 3 / 4);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 2;
					vidas -= 1;
					aceleracao += 0.05;
					break;
				case 2:
					Pontos += PONTOS_PILHA_CERTA / 2;
					//printf("2 certos. %d pontos add\n", PONTOS_PILHA_CERTA / 2);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 1;
					vidas -= 2;
					aceleracao += 0.05;
					break;
				case 1:
					Pontos += PONTOS_PILHA_CERTA * 1 / 4;
					//printf("1 certos. %d pontos add\n", PONTOS_PILHA_CERTA * 1 / 4);
					pontuou[0] = 0;
					pontuou[1] = i;
					pontuou[2] = 0;
					vidas -= 3;
					aceleracao += 0.05;
					break;
				default:
					vidas -= 4;
					if (vidas < 1)
					{
						fout = true;
					}
					break;
				}
				gab_stat[i] = false;
			}
		}


		//draw dos copos
		al_draw_bitmap(copo, X_PRIMEIRO_COPO + 3 * X_DELTA_COPO, Y_COPO, 0);
		al_draw_bitmap(copo, X_PRIMEIRO_COPO + 2 * X_DELTA_COPO, Y_COPO, 0);
		al_draw_bitmap(copo, X_PRIMEIRO_COPO + X_DELTA_COPO, Y_COPO, 0);
		al_draw_bitmap(copo, X_PRIMEIRO_COPO, Y_COPO, 0);

		//HUD
		char num[5];
		_itoa_s(Pontos, num, 10);
		al_draw_text(font, al_map_rgb(255, 255, 255), X_PLACAR, Y_PLACAR, ALLEGRO_ALIGN_LEFT, "Placar: ");
		al_draw_text(font, al_map_rgb(255, 255, 255), X_PLACAR + 130, Y_PLACAR, ALLEGRO_ALIGN_LEFT, num);
		al_draw_text(font, al_map_rgb(255, 255, 255), X_PRIMEIRA_VIDA - 110, Y_PLACAR, ALLEGRO_ALIGN_LEFT, "Vidas:");
		for (i = 0; i < vidas; i++)
		{
			al_draw_bitmap(vida, X_PRIMEIRA_VIDA + i* X_DELTA_VIDA, Y_PRIMEIRA_VIDA, 0);
		}

		//Animação pontos
		if (pontuou[0] >= 0)
		{
			al_draw_bitmap(anim[pontuou[2]][(int)(pontuou[0] / 2)], X_PRIMEIRO_ANIM + pontuou[1] * X_DELTA_COPO, Y_PRIMEIRO_ANIM, 0);
			pontuou[0]++;
			if(pontuou[0]==1)
				al_play_sample(ring, 0.7, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			else if (pontuou[0] == 43)
			{
				pontuou[0] = -1;
				if (vidas < 1)
				{
					fout = true;
				}
			}
		}

		//botao voltar
		al_get_mouse_state(&state);
		if (state.buttons & 1)
			if ((state.y > 9 && state.y < 38) && (state.x > 4 && state.x < 36))
			{
				fadeout(10);
				al_destroy_sample_instance(songInstance);
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
				return true;
			}

		if (fade)
		{
			ALLEGRO_BITMAP *buffer = NULL;
			buffer = al_create_bitmap(DISPLAY_W, DISPLAY_H);
			al_set_target_bitmap(buffer);
			al_draw_bitmap(al_get_backbuffer(display), 0, 0, 0);
			al_set_target_bitmap(al_get_backbuffer(display));
			fadein(buffer, 10);
			fade = false;
		}else if(fout)
		{
			fadeout(10);
			acabou = true;
		}


		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			return false;
		}

		//novo frame
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));

	}
	al_destroy_sample_instance(songInstance);

	//Tela de Fim de jogo
	bool botao = false;
	fadein(gameover, 15);
	while(!botao){
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		al_draw_bitmap(gameover, 0, 0, 0);
		char num[5];
		_itoa_s(Pontos, num, 10);
		al_draw_text(pontos_gameover, al_map_rgb(16, 16, 16), 612, 295, ALLEGRO_ALIGN_CENTER, num);
		al_get_mouse_state(&state);
		if (state.buttons & 1)
			if ((state.y > 395 && state.y < 455) && (state.x > 537 && state.x < 684))
			{
				botao = true;
				fadeout(10);
			}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			return false;
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	} 
	return true;
}

int main()
{
	std::srand(unsigned(std::time(0)));
	bool exit = false;


	/*Inicializações*/
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}


	if (!al_init_image_addon()) {
		fprintf(stderr, "failed to init image addon!\n");
		return -1;
	}

	display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
	al_set_window_title(display, "Empilha Frutas do Moe");


	loading = al_load_bitmap("data/bitmaps/loading.jpg");
	icon = al_load_bitmap("data/bitmaps/icon.png");
	al_draw_bitmap(loading, 0, 0, 0);
	al_set_display_icon(display, icon);


	if (!al_install_mouse()) {
		fprintf(stderr, "Error installing mouse.\n");
		return -1;
	}

	al_init_font_addon();

	al_init_primitives_addon();

	al_init_ttf_addon();


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
	font = al_load_ttf_font("data/KOMIKAX_.ttf", 28, 0);
	pontos_gameover = al_load_ttf_font("data/KOMIKAX_.ttf", 58, 0);
	if (!font||!pontos_gameover) {
		fprintf(stderr, "Could not load 'KOMIKAX_.ttf'.\n");
		return -1;
	}
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	al_init_acodec_addon();


	/*Load de bitmaps*/
	bg = al_load_bitmap("data/bitmaps/bg.jpg");
	gameover = al_load_bitmap("data/bitmaps/gameover.jpg");
	menu = al_load_bitmap("data/bitmaps/menu.jpg");
	ajuda = al_load_bitmap("data/bitmaps/ajuda.jpg");
	copo = al_load_bitmap("data/bitmaps/copo.png");
	bebida[0] = al_load_bitmap("data/bitmaps/frutas/limao.png");
	bebida[1] = al_load_bitmap("data/bitmaps/frutas/tangerina.png");
	bebida[2] = al_load_bitmap("data/bitmaps/frutas/cereja.png");
	bebida[3] = al_load_bitmap("data/bitmaps/frutas/maca.png");
	bebida[4] = al_load_bitmap("data/bitmaps/frutas/uva.png");
	bebida[5] = al_load_bitmap("data/bitmaps/frutas/melancia.png");
	bebida[6] = al_load_bitmap("data/bitmaps/frutas/morango.png");
	tb_bebida[0] = al_load_bitmap("data/bitmaps/frutas/tb-limao.png");
	tb_bebida[1] = al_load_bitmap("data/bitmaps/frutas/tb-tangerina.png");
	tb_bebida[2] = al_load_bitmap("data/bitmaps/frutas/tb-cereja.png");
	tb_bebida[3] = al_load_bitmap("data/bitmaps/frutas/tb-maca.png");
	tb_bebida[4] = al_load_bitmap("data/bitmaps/frutas/tb-uva.png");
	tb_bebida[5] = al_load_bitmap("data/bitmaps/frutas/tb-melancia.png");
	tb_bebida[6] = al_load_bitmap("data/bitmaps/frutas/tb-morango.png");
	vida = al_load_bitmap("data/bitmaps/vida.png");
	if (!copo||!bg||!bebida[0] || !bebida[1] || !bebida[2] || !bebida[3] || !bebida[4] || !bebida[5] || !bebida[6]) {
		fprintf(stderr, "failed to load image!\n");
		al_destroy_display(display);
		return 0;
	}

	for (j = 1; j <= 4; j++)
	{
		for (i = 0; i < 22; i++)
		{
			string nome = "data/anims/" + std::to_string(j*25) +"/" + std::to_string(i) + ".png";
			anim[j-1][i] = al_load_bitmap(nome.c_str());
		}
	}
	

	jump = al_load_sample("data/audio/jump.wav");
	if (!jump) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}
	ring = al_load_sample("data/audio/ring.wav");
	if (!ring) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}
	musicgame = al_load_sample("data/audio/game.ogg");
	if (!musicgame) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}

	/*Display*/
	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	timer = al_create_timer(1.0 / FPS);
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_start_timer(timer);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));



	while(!exit)
	{
		fadein(menu, 10);
		switch(iniciaMenu())
		{
		case 0:
			exit = true;
		break;
		case 1:
			if(!iniciaJogo())
				exit = true;
		break;
		case 2:
			fadein(ajuda, 10);
			if (!iniciaAjuda())
				exit = true;
		break;
		}
	}


	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_sample(jump);
	al_destroy_sample(ring);
	al_destroy_sample(musicgame);



	return 0;
}