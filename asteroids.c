////////////////////////////////////////////////////////////////////////////
//                                 Asteroids
//                           Instituto de Computacao
//                                    UFAL
// Aluno: Lucas Agra de Omena
//
// Testado e executado no Windows 10
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "objects.h"

#define PI 3.1415926

// -------- Constantes ---------

const int LIM_ASTEROIDES = 50;
const int MAX_PROJETEIS = 7;
const int LARGURA_TELA = 1024;
const int ALTURA_TELA = 700;
const int LIM_VEL = 8;
const int VEL_PROJETIL = 30;
const int FPS = 60;
const int MAX_EXPLOSOES = 8;
const int MAX_ESTRELAS = 60;

const char IMGFUNDO[] = "sprites/space.jpg";
const char IMGNAVE[] = "sprites/ship.png";
const char *IMGASTEROIDES[3] = {"sprites/asteroidG.png", "sprites/asteroidM.png", "sprites/asteroidP.png"};
const char EXPLOSAO[] = "sprites/explosion.png";
const char BARREIRA[] = "sprites/barreira.png";
const char IMGMENU[] = "sprites/menu.jpg";
const char ACELNAVE[] = "sprites/shipacc.png";
const char SHOT[] = "sprites/shot2.png";

// -----------------------------

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_BITMAP *imagemfundo = NULL, *imagemnave = NULL, *imagemmenu = NULL, *astG = NULL, *astM = NULL, *astP = NULL, *expImg = NULL, *barreiraImg = NULL, *shipacc = NULL, *tiro = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL, *fila_contador = NULL;
ALLEGRO_FONT *fonte = NULL, *fonte2 = NULL, *fonte3 = NULL, *fonte4 = NULL;
ALLEGRO_TIMER *timer = NULL, *contador = NULL;
ALLEGRO_SAMPLE *audioexplosao = NULL, *audiolaser = NULL, *audioexplosaonave = NULL, *audiofoguete = NULL, *audioescudo = NULL, *songmenu = NULL, *songplay = NULL;
ALLEGRO_SAMPLE_INSTANCE *audioInstance = NULL, *songmenuInstance = NULL, *songplayInstance = NULL;

// -------- Auxiliares --------

enum KEYS{UP, LEFT, RIGHT, SPACE, ENTER};
bool keys[6] = {false, false, false, false, false};
bool redraw = false;

// -------- Variáveis ---------

bool sair = false;
bool menu = true;
bool jogo = false;

int MAX_ASTEROIDES = 4;
int SCORE = 0;
int Comprimento_Nave = 0;
int Altura_Nave = 0;
int seg = 0;
int ultimacolisao = -10;

// -------- Protótipos --------
bool inicializar();
bool finalizar();

void IniciarNave(NaveEspacial *nave);
void DesenharNave(NaveEspacial *nave);
void Acelerar(NaveEspacial *nave);
void RotacionarDir(NaveEspacial *nave);
void RotacionarEsq(NaveEspacial *nave);
void AtualizarNave(NaveEspacial *nave);

void IniciarProjetil(Projetil projetil[], int tamanho);
void DesenharProjetil(Projetil projetil[], int tamanho);
void DispararProjetil(Projetil projetil[], int tamanho, NaveEspacial *nave);
void AtualizarProjetil(Projetil projetil[], int tamanho);

void IniciarAsteroides(Asteroide asteroide[], int tamanho);
void DesenharAsteroides(Asteroide asteroide[], int tamanho);
void GerarAsteroides(Asteroide asteroide[], int tamanho);
void AtualizarAsteroides(Asteroide asteroide[], int tamanho);
void DividirAsteroides(Asteroide asteroide[], int i, int tamanho);

void ColidirProjetil(Projetil projetil[], int tProjetil, Asteroide asteroide[], int tAsteroide, Explosao explosao[], int tExplosao);
void ColidirNave(NaveEspacial *nave, Asteroide asteroide[], int tAsteroide, Explosao explosao[], int tExplosao, Barreira *barreira);

void IniciarExplosoes(Explosao explosao[], int tamanho, ALLEGRO_BITMAP *image);
void DesenharExplosoes(Explosao explosao[], int tamanho);
void ComecarExplosoes(Explosao explosao[], int tamanho, int x, int y);
void AtualizarExplosoes(Explosao explosao[], int tamanho);

void IniciarBarreira(Barreira *barreira, ALLEGRO_BITMAP *image);
void DesenharBarreira(Barreira *barreira, NaveEspacial *nave);
void ComecarBarreira(Barreira *barreira, float x, float y);
void AtualizarBarreira(Barreira *barreira);

void IniciarEstrelas(Estrela estrela[], int tamanho);
void DesenharEstrelas(Estrela estrela[], int tamanho);
void ComecarEstrelas(Estrela estrela[], int tamanho);
void AtualizarEstrelas(Estrela estrela[], int tamanho, NaveEspacial *nave);
// ----------------------------

int main (void){

    bool pause = false;
    bool add = true; //Condição para add asteroide

    NaveEspacial nave;
    Projetil projetil[MAX_PROJETEIS];
    Asteroide asteroide[LIM_ASTEROIDES];
    Explosao explosao[MAX_EXPLOSOES];
    Estrela estrela[MAX_ESTRELAS];
    Barreira barreira;

    if(!inicializar()){
        return -1;
    }

    al_start_timer(timer);
    al_play_sample_instance(songmenuInstance);

    while(!sair){

        while(menu){

            if(!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento4;
                al_wait_for_event(fila_eventos, &evento4);

                if(evento4.type == ALLEGRO_EVENT_TIMER){
                    al_draw_bitmap(imagemmenu,0,0,0);
                    al_draw_textf(fonte3, al_map_rgba(120, 120, 120, .5), LARGURA_TELA/2, 0.1*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "ASTEROIDS");
                    al_draw_textf(fonte, al_map_rgba(53, 68, 81, 150), 0.5*LARGURA_TELA, 0.45*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "JOGAR");
                    al_draw_textf(fonte2, al_map_rgba(53, 68, 81, 150), 0.5*LARGURA_TELA, 0.51*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "(ENTER)");
                    al_draw_textf(fonte4, al_map_rgba(53, 68, 81, 190), 0.5*LARGURA_TELA, 0.55*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "ATIRAR");
                    al_draw_textf(fonte4, al_map_rgba(53, 68, 81, 190), 0.5*LARGURA_TELA, 0.57*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "(spacebar)");
                    al_draw_textf(fonte4, al_map_rgba(53, 68, 81, 190), 0.5*LARGURA_TELA, 0.60*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "MOVIMENTO");
                    al_draw_textf(fonte4, al_map_rgba(53, 68, 81, 190), 0.5*LARGURA_TELA, 0.62*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "(arrow keys)");                                                                        
                    al_draw_textf(fonte, al_map_rgba(120, 120, 120, .5), 0.5*LARGURA_TELA, 0.85*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "SAIR");
                    al_draw_textf(fonte2, al_map_rgba(120, 120, 120, .5), 0.5*LARGURA_TELA, 0.91*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "(ESC)");
                    al_flip_display();
                }

                if (evento4.type == ALLEGRO_EVENT_KEY_DOWN) {
                    switch(evento4.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                        sair = true;
                        menu = false;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        al_start_timer(timer);
                        al_start_timer(contador);
                        IniciarNave(&nave);
                        IniciarProjetil(projetil, MAX_PROJETEIS);
                        IniciarAsteroides(asteroide, LIM_ASTEROIDES);
                        IniciarEstrelas(estrela, MAX_ESTRELAS);
                        IniciarExplosoes(explosao, MAX_EXPLOSOES, expImg);
                        IniciarBarreira(&barreira, barreiraImg);
                        MAX_ASTEROIDES = 4;
                        seg = 0;
                        SCORE = 0;
                        ultimacolisao = -2;
                        al_stop_sample_instance(songmenuInstance);
                        al_play_sample_instance(songplayInstance);
                        menu = false;
                        jogo = true;
                        break;
                    }
                }

                if(evento4.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    sair = true;
                    menu = false;
                }
            }
        }

        while(pause){

            if(!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento3;
                al_wait_for_event(fila_eventos, &evento3);

                if(evento3.type == ALLEGRO_EVENT_TIMER){

                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 1));
                    al_draw_textf(fonte3, al_map_rgba(255, 255, 255, 150), LARGURA_TELA/2, 0.2*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "Jogo Parado");
                    al_draw_textf(fonte, al_map_rgba(255, 255, 255, 150), LARGURA_TELA/2, 0.5*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "Continuar");
                    al_draw_textf(fonte2, al_map_rgba(255, 255, 255, 150), LARGURA_TELA/2, 0.56*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "(ESC)");
                    al_draw_textf(fonte, al_map_rgba(255, 255, 255, 150), LARGURA_TELA/2, 0.64*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "Menu");
                    al_draw_textf(fonte2, al_map_rgba(255, 255, 255, 150), LARGURA_TELA/2, 0.7*ALTURA_TELA, ALLEGRO_ALIGN_CENTRE, "(BACKSPACE)");
                    al_flip_display();
                }

                if (evento3.type == ALLEGRO_EVENT_KEY_DOWN) {
                    switch(evento3.keyboard.keycode) {
                    case ALLEGRO_KEY_BACKSPACE:
                        sair = false;
                        pause = false;
                        jogo = false;
                        menu = true;
                        al_stop_sample_instance(songplayInstance);
                        al_play_sample_instance(songmenuInstance);
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        pause = false;
                        jogo = true;
                        break;
                    }
                }

                if(evento3.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    sair = true;
                    jogo = false;
                    pause = false;
                    menu = false;
                }
            }
        }

        while(jogo){

            if(!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);


                if(evento.type == ALLEGRO_EVENT_TIMER){
                    if(nave.vida){
                        redraw = true;
        		        if(keys[UP]){
                            Acelerar(&nave);
        		        }

        		        if(keys[RIGHT]){
                            RotacionarDir(&nave);
        		        }

        		        if(keys[LEFT]){
                            RotacionarEsq(&nave);
        		        }

                        if(add){
                            if(seg > 0 && (seg % 10) == 0 && MAX_ASTEROIDES < 49 && !pause){
                                MAX_ASTEROIDES++;
                                add = false;
                            }
                        }
                        else if(seg%2 != 0){
                            add = true;
                        }

                        AtualizarEstrelas(estrela, MAX_ESTRELAS, &nave);
                        ComecarEstrelas(estrela, MAX_ESTRELAS);
                        AtualizarNave(&nave);
                        AtualizarProjetil(projetil, MAX_PROJETEIS);
                        GerarAsteroides(asteroide, MAX_ASTEROIDES);
                        AtualizarAsteroides(asteroide, MAX_ASTEROIDES);
                        ColidirProjetil(projetil, MAX_PROJETEIS, asteroide, MAX_ASTEROIDES, explosao, MAX_EXPLOSOES);
                        ColidirNave(&nave, asteroide, MAX_ASTEROIDES, explosao, MAX_EXPLOSOES, &barreira);
                        AtualizarExplosoes(explosao, MAX_EXPLOSOES);
                        AtualizarBarreira(&barreira);
                    }
                    else{
                    	AtualizarProjetil(projetil, MAX_PROJETEIS);
                    	AtualizarEstrelas(estrela, MAX_ESTRELAS, &nave);
                        ComecarEstrelas(estrela, MAX_ESTRELAS);
                        AtualizarAsteroides(asteroide, MAX_ASTEROIDES);
                        AtualizarExplosoes(explosao, MAX_EXPLOSOES);
                        al_stop_timer(contador);

                        if(keys[ENTER]){
                            menu = true;
                            jogo = false;
                            al_stop_sample_instance(songplayInstance);
                            al_play_sample_instance(songmenuInstance);
                        }
                    }
                }

                if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                    switch(evento.keyboard.keycode) {
                        case ALLEGRO_KEY_ESCAPE:
                            jogo = false;
                            pause = true;
                            break;
                        case ALLEGRO_KEY_UP:
                            keys[UP] = true;
                            if(nave.vida){
                            	al_play_sample_instance(audioInstance);
                        	}
                            break;
                        case ALLEGRO_KEY_LEFT:
                            keys[LEFT] = true;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            keys[RIGHT] = true;
                            break;
                        case ALLEGRO_KEY_SPACE:
                            keys[SPACE] = true;
                            DispararProjetil(projetil, MAX_PROJETEIS, &nave);
                            break;
                        case ALLEGRO_KEY_ENTER:
                            keys[ENTER] = true;
                            break;
                    }
                }
                else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
                    switch(evento.keyboard.keycode) {
                        case ALLEGRO_KEY_UP:
                            keys[UP] = false;
                            al_stop_sample_instance(audioInstance);
                            break;
                        case ALLEGRO_KEY_LEFT:
                            keys[LEFT] = false;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            keys[RIGHT] = false;
                            break;
                        case ALLEGRO_KEY_SPACE:
                            keys[SPACE] = false;
                            break;
                        case ALLEGRO_KEY_ENTER:
                            keys[ENTER] = false;
                            break;
                    }
                }

                if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    sair = true;
                    jogo = false;
                }
            }

            if(!al_is_event_queue_empty(fila_contador)){
                ALLEGRO_EVENT evento2;
                al_wait_for_event(fila_contador, &evento2);

                if (evento2.type == ALLEGRO_EVENT_TIMER)
                {
                    seg++;

                    if(pause || menu){
                        seg--;
                    }
                }
            }

            if(redraw && al_is_event_queue_empty(fila_eventos)){

            	if(nave.vida){
                    DesenharNave(&nave);

                    if(keys[UP]){
                        al_draw_rotated_bitmap(shipacc, Comprimento_Nave/2, Altura_Nave/2, nave.x, nave.y, (nave.angulo*PI)/180, 0);
                    }
            	}
            	DesenharEstrelas(estrela, MAX_ESTRELAS);
            	DesenharProjetil(projetil, MAX_PROJETEIS);
            	DesenharAsteroides(asteroide, MAX_ASTEROIDES);
                DesenharExplosoes(explosao, MAX_EXPLOSOES);
                DesenharBarreira(&barreira, &nave);

                al_draw_textf(fonte, al_map_rgba(255, 255, 0, 10), LARGURA_TELA - 270, 10, ALLEGRO_ALIGN_LEFT, "SCORE: %.4d", SCORE);
                al_draw_textf(fonte, al_map_rgba(255, 255, 0, 10), 30, 10, ALLEGRO_ALIGN_LEFT, "VIDAS: %d", nave.vida);
                al_draw_textf(fonte, al_map_rgba(255, 255, 0, 10), LARGURA_TELA/2, 10, ALLEGRO_ALIGN_CENTRE, "NIVEL: %d", MAX_ASTEROIDES);

                if(!nave.vida){
                    al_draw_textf(fonte, al_map_rgba(255, 255, 255, 50), LARGURA_TELA/2, ALTURA_TELA/2 - 120, ALLEGRO_ALIGN_CENTRE, "PONTUACAO FINAL");
                    al_draw_textf(fonte, al_map_rgba(255, 255, 255, 50), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTRE, "%d", SCORE);
                    al_draw_textf(fonte, al_map_rgba(255, 255, 255, 50), LARGURA_TELA/2, ALTURA_TELA/2 + 30, ALLEGRO_ALIGN_CENTRE, "Menu");
                    al_draw_textf(fonte2, al_map_rgba(255, 255, 255, 50), LARGURA_TELA/2, ALTURA_TELA/2 + 75, ALLEGRO_ALIGN_CENTRE, "(Enter)");
                }

    	        al_flip_display();
    	        al_draw_bitmap(imagemfundo, 0, 0, 0);
            }
        }
    }

    finalizar();
    return 0;
}


void IniciarNave(NaveEspacial *nave){

    (*nave).x = LARGURA_TELA/2;
    (*nave).y = ALTURA_TELA/2;
    (*nave).vida = 5;
    (*nave).vel_x = 0;
    (*nave).vel_y = 0;
    (*nave).angulo = 0;
    (*nave).raio = 20;
}

void DesenharNave(NaveEspacial *nave){

	al_draw_rotated_bitmap(imagemnave, Comprimento_Nave/2, Altura_Nave/2, (*nave).x, (*nave).y, ((*nave).angulo*PI)/180, 0);
}

void AtualizarNave(NaveEspacial *nave){
    if((*nave).x >= LARGURA_TELA + Comprimento_Nave/2){
        (*nave).x = 0 - Comprimento_Nave/2;
    }

    else if((*nave).x <= 0 - Comprimento_Nave/2){
        (*nave).x = LARGURA_TELA + Comprimento_Nave/2;
    }

    if((*nave).y >= ALTURA_TELA + Altura_Nave/2){
        (*nave).y = 0 - Altura_Nave/2;
    }

    else if((*nave).y <= 0 - Altura_Nave/2){
        (*nave).y = ALTURA_TELA + Altura_Nave/2;
    }

    (*nave).y = (*nave).y + (*nave).vel_y;
    (*nave).x = (*nave).x + (*nave).vel_x;
    (*nave).vel_x *= 0.98;
    (*nave).vel_y *= 0.98;
}

void Acelerar(NaveEspacial *nave){

	if((*nave).angulo == 0 || (*nave).angulo == 360){
		if((*nave).vel_y > (LIM_VEL*-1)){
			(*nave).vel_y -= 0.5*cos(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo > 0 && (*nave).angulo < 90){
		if((*nave).vel_y > (LIM_VEL*-1)){
			(*nave).vel_y -= 0.5*cos(((*nave).angulo*PI)/180);
		}
		if((*nave).vel_x < LIM_VEL){
			(*nave).vel_x += 0.5*sin(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo == 90){
		if((*nave).vel_x < LIM_VEL){
			(*nave).vel_x += 0.5*sin(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo > 90 && (*nave).angulo < 180){
		if((*nave).vel_y < LIM_VEL){
			(*nave).vel_y -= 0.5*cos(((*nave).angulo*PI)/180);
		}
		if((*nave).vel_x < LIM_VEL){
			(*nave).vel_x += 0.5*sin(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo == 180){
		if((*nave).vel_y < LIM_VEL){
			(*nave).vel_y -= 0.5*cos(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo > 180 && (*nave).angulo < 270){
		if((*nave).vel_y < LIM_VEL){
			(*nave).vel_y -= 0.5*cos(((*nave).angulo*PI)/180);
		}
		if((*nave).vel_x > (LIM_VEL*-1)){
			(*nave).vel_x += 0.5*sin(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo == 270){
		if((*nave).vel_x > (LIM_VEL*-1)){
			(*nave).vel_x += 0.5*sin(((*nave).angulo*PI)/180);
		}
	}
	else if((*nave).angulo > 270 && (*nave).angulo < 360){
		if((*nave).vel_y > (LIM_VEL*-1)){
			(*nave).vel_y -= 0.5*cos(((*nave).angulo*PI)/180);
		}
		if((*nave).vel_x > (LIM_VEL*-1)){
			(*nave).vel_x += 0.5*sin(((*nave).angulo*PI)/180);
		}
	}
}

void RotacionarDir(NaveEspacial *nave){

	(*nave).angulo += 3;
	if((*nave).angulo >= 360){
		(*nave).angulo = 0;
	}
}

void RotacionarEsq(NaveEspacial *nave){

	(*nave).angulo-=3;
	if((*nave).angulo <= 0){
		(*nave).angulo = 360;
	}
}


void IniciarProjetil(Projetil projetil[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        projetil[i].vida = false;
        projetil[i].raio = 5;
    }
}

void DesenharProjetil(Projetil projetil[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(projetil[i].vida){
        	al_draw_rotated_bitmap(tiro, 12, 14, projetil[i].x,  projetil[i].y, (projetil[i].angulo*PI)/180, 0);
        }
    }
}

void DispararProjetil(Projetil projetil[], int tamanho, NaveEspacial *nave){
    int i;
    for(i = 0; i < tamanho; i++){
        if(!projetil[i].vida){
            projetil[i].x = (*nave).x;
            projetil[i].y = (*nave).y;
            projetil[i].angulo = (*nave).angulo;
            projetil[i].vel_x = VEL_PROJETIL*sin(projetil[i].angulo*PI/180);
            projetil[i].vel_y = (-1)*VEL_PROJETIL*cos(projetil[i].angulo*PI/180);
            projetil[i].vida = true;
            al_play_sample(audiolaser, 0.65, 0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
            break;
        }
    }
}

void AtualizarProjetil(Projetil projetil[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(projetil[i].vida){
            projetil[i].x += projetil[i].vel_x;
            projetil[i].y += projetil[i].vel_y;

            if(projetil[i].x > LARGURA_TELA + projetil[i].raio || projetil[i].x < 0 - projetil[i].raio){
                projetil[i].vida = false;
            }
            else if(projetil[i].y > ALTURA_TELA + projetil[i].raio || projetil[i].y < 0 - projetil[i].raio){
                projetil[i].vida = false;
            }
        }
    }
}


void IniciarAsteroides(Asteroide asteroide[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        asteroide[i].vida = false;
    }
}

void DesenharAsteroides(Asteroide asteroide[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(asteroide[i].vida){
        	int framex, framey;

        	framex = (asteroide[i].curFrame % asteroide[i].animationColumns) * asteroide[i].frameWidth;
        	framey = (asteroide[i].curFrame / asteroide[i].animationColumns) * asteroide[i].frameHeight;

        	al_draw_bitmap_region(asteroide[i].image, framex, framey, asteroide[i].frameWidth, asteroide[i].frameHeight,
        		asteroide[i].x - asteroide[i].frameWidth/2, asteroide[i].y - asteroide[i].frameHeight/2, 0);
        }
    }
}

void GerarAsteroides(Asteroide asteroide[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(!asteroide[i].vida){
            if(rand() % 800 == 0){
                int aux = (rand() % 2) + 1;
                int aux2 = rand()%2;
                int aux3 = rand()%3 + 3;
                asteroide[i].vida = true;
                asteroide[i].nivel = aux+1;
                asteroide[i].raio = 15*asteroide[i].nivel;
                asteroide[i].angulo = rand() % 360;
                asteroide[i].vel_x = (aux3*sin(asteroide[i].angulo*PI/180))/asteroide[i].nivel;
                asteroide[i].vel_y = ((-1)*aux3*cos(asteroide[i].angulo*PI/180))/asteroide[i].nivel;

            	asteroide[i].maxFrame = 32;
				asteroide[i].curFrame = rand()%64;
				asteroide[i].frameCount = 0;
				asteroide[i].frameDelay = (rand()%4) + 2;
				asteroide[i].animationColumns = 8;

				if(aux2){
					asteroide[i].animationDirection = 1;
				}
				else {
					asteroide[i].animationDirection = -1;
				}

				if(asteroide[i].nivel == 3){
					asteroide[i].frameWidth = 128;
					asteroide[i].frameHeight = 128;
					asteroide[i].image = astG;
				}
				else if(asteroide[i].nivel == 2){
					asteroide[i].frameWidth = 84;
					asteroide[i].frameHeight = 84;
					asteroide[i].image = astM;
				}

                if(aux == 1){
                    asteroide[i].x = -50;
                    asteroide[i].y = rand() % ALTURA_TELA;
                }
                else{
                    asteroide[i].x = rand() % LARGURA_TELA;
                    asteroide[i].y = -50;
                }
            }
        }
    }
}

void AtualizarAsteroides(Asteroide asteroide[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(asteroide[i].vida){
            asteroide[i].x += asteroide[i].vel_x;
            asteroide[i].y += asteroide[i].vel_y;

            if(asteroide[i].x < 0 - asteroide[i].raio){
                asteroide[i].x = LARGURA_TELA + asteroide[i].raio;
            }
            else if(asteroide[i].x > LARGURA_TELA + asteroide[i].raio){
                asteroide[i].x = 0 - asteroide[i].raio;
            }
            if(asteroide[i].y < 0 - asteroide[i].raio){
                asteroide[i].y = ALTURA_TELA + asteroide[i].raio;
            }
            else if(asteroide[i].y > ALTURA_TELA + asteroide[i].raio){
                asteroide[i].y = 0 - asteroide[i].raio;
            }

            asteroide[i].frameCount++;
            if(asteroide[i].frameCount >= asteroide[i].frameDelay){

                asteroide[i].curFrame += asteroide[i].animationDirection;
                asteroide[i].frameCount = 0;

                if(asteroide[i].curFrame >= asteroide[i].maxFrame -1){
                    asteroide[i].curFrame = 0;
                }
                else if(asteroide[i].curFrame < 0){
                	asteroide[i].curFrame = asteroide[i].maxFrame -1;
                }
                else if(asteroide[i].curFrame >= 2*(asteroide[i].maxFrame)-1){
                    asteroide[i].curFrame = asteroide[i].maxFrame;
                }
                else if(asteroide[i].curFrame == asteroide[i].maxFrame){
                	asteroide[i].curFrame = 2*(asteroide[i].maxFrame) -1;
                }
            }
        }
    }
}

void DividirAsteroides(Asteroide asteroide[], int i, int tamanho){
    if(asteroide[i].nivel > 1){
    	int aux = rand()%3 + 3;
        asteroide[i].vida = true;
        asteroide[i].nivel--;

        asteroide[i].angulo -= (aux*7 + 15);
        asteroide[i].vel_x = (aux*sin(asteroide[i].angulo*PI/180))/asteroide[i].nivel;
        asteroide[i].vel_y = ((-1)*aux*cos(asteroide[i].angulo*PI/180))/asteroide[i].nivel;
        SCORE+= 10*asteroide[i].nivel;

		if(asteroide[i].nivel == 2){
			asteroide[i].frameWidth = 84;
			asteroide[i].frameHeight = 84;
			asteroide[i].image = astM;
            asteroide[i].raio = 15*asteroide[i].nivel;
		}
		else if(asteroide[i].nivel == 1){
			asteroide[i].frameWidth = 55;
			asteroide[i].frameHeight = 55;
			asteroide[i].image = astP;
            asteroide[i].raio = 17;
		}

        int j;
        for(j = 0; j < tamanho; j++){
            if(!asteroide[j].vida){
                asteroide[j].vida = true;
                asteroide[j].x = asteroide[i].x;
                asteroide[j].y = asteroide[i].y;
                asteroide[j].nivel = asteroide[i].nivel;
       
                asteroide[j].angulo = asteroide[i].angulo + (aux*7) + 15;
                asteroide[j].vel_x = (aux*sin(asteroide[j].angulo*PI/180))/asteroide[j].nivel;
                asteroide[j].vel_y = ((-1)*aux*cos(asteroide[j].angulo*PI/180))/asteroide[j].nivel;

                asteroide[j].maxFrame = 32;
				asteroide[j].curFrame = rand()%64;
				asteroide[j].frameCount = 0;
				asteroide[j].frameDelay = (rand()%4) + 2;
				asteroide[j].animationColumns = 8;
				asteroide[j].animationDirection = asteroide[i].animationDirection;

				if(asteroide[j].nivel == 2){
					asteroide[j].frameWidth = 84;
					asteroide[j].frameHeight = 84;
					asteroide[j].image = astM;
                    asteroide[j].raio = 15*asteroide[i].nivel;
				}
				else if(asteroide[j].nivel == 1){
					asteroide[j].frameWidth = 55;
					asteroide[j].frameHeight = 55;
					asteroide[j].image = astP;
                    asteroide[j].raio = 17;
				}
                break;
            }
        }
    }
    else{
        asteroide[i].vida = false;
        SCORE += 10;
    }
}

void ColidirProjetil(Projetil projetil[], int tProjetil, Asteroide asteroide[], int tAsteroide, Explosao explosao[], int tExplosao){
    int i, j;
    float distancia;

    for(i = 0; i < tProjetil; i++){
        if(projetil[i].vida){
            for(j = 0; j< tAsteroide; j++){
                if(asteroide[j].vida){
                    distancia = sqrt( ( (projetil[i].x - asteroide[j].x) * (projetil[i].x - asteroide[j].x) ) + ( (projetil[i].y - asteroide[j].y) * (projetil[i].y - asteroide[j].y) ) );
                    if(distancia < (projetil[i].raio + asteroide[j].raio)){
                        projetil[i].vida = false;
                        DividirAsteroides(asteroide, j, MAX_ASTEROIDES);
                        ComecarExplosoes(explosao, tExplosao, projetil[i].x, projetil[i].y);
                    }
                }
            }
        }
    }
}

void ColidirNave(NaveEspacial *nave, Asteroide asteroide[], int tAsteroide, Explosao explosao[], int tExplosao, Barreira *barreira){
    int i;
    float distancia;

    for(i = 0; i < tAsteroide; i++){
        if(asteroide[i].vida){
            distancia =  sqrt((((*nave).x - asteroide[i].x) * ((*nave).x - asteroide[i].x)) + (((*nave).y - asteroide[i].y)  *  ((*nave).y - asteroide[i].y)));
            if(distancia < ((*nave).raio + asteroide[i].raio)){

                if(ultimacolisao+2 <= seg){
                    ultimacolisao = seg;
                    (*nave).vida--;
                }

                if((*nave).vida){
                    ComecarBarreira(barreira, (*nave).x, (*nave).y);
                    ComecarExplosoes(explosao, tExplosao, asteroide[i].x, asteroide[i].y);
                    al_play_sample(audioescudo, 2, 0, 0.5, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                else{
                    ComecarExplosoes(explosao, tExplosao, (*nave).x+10, (*nave).y-10);
                    ComecarExplosoes(explosao, tExplosao, (*nave).x-10, (*nave).y-10);
                    ComecarExplosoes(explosao, tExplosao, (*nave).x, (*nave).y+10);
                    al_play_sample(audioexplosaonave, 2, 0, 0.8, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                asteroide[i].vida = false;
                break;
            }
        }
    }
}

void IniciarExplosoes(Explosao explosao[], int tamanho, ALLEGRO_BITMAP *image){
	int i;
	for(i = 0; i < tamanho; i++){
        explosao[i].vida = false;
		explosao[i].maxFrame = 16;
		explosao[i].curFrame = 0;
		explosao[i].frameCount = 0;
		explosao[i].frameDelay = 3;
		explosao[i].frameWidth = 128;
		explosao[i].frameHeight = 128;
		explosao[i].animationColumns = 4;
		explosao[i].image = image;
	}
}

void DesenharExplosoes(Explosao explosao[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(explosao[i].vida){
        	int framex, framey;

        	framex = (explosao[i].curFrame % explosao[i].animationColumns) * explosao[i].frameWidth;
        	framey = (explosao[i].curFrame / explosao[i].animationColumns) * explosao[i].frameHeight;
            al_draw_bitmap_region(explosao[i].image, framex, framey, explosao[i].frameWidth, explosao[i].frameHeight,
            	explosao[i].x - (explosao[i].frameWidth/2), explosao[i].y - (explosao[i].frameHeight/2), 0);
        }
    }
}

void ComecarExplosoes(Explosao explosao[], int tamanho, int x, int y){
    int i;
    for(i = 0; i < tamanho; i++){
        if(!explosao[i].vida){
            explosao[i].vida = true;
            explosao[i].x = x;
            explosao[i].y = y;
            al_play_sample(audioexplosao, 0.8, 0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
            break;
        }
    }
}

void AtualizarExplosoes(Explosao explosao[], int tamanho){
    int i;
    for(i = 0; i < tamanho; i++){
        if(explosao[i].vida){
            explosao[i].frameCount++;
            if(explosao[i].frameCount >= explosao[i].frameDelay){
                explosao[i].curFrame++;
                explosao[i].frameCount = 0;
                if(explosao[i].curFrame >= explosao[i].maxFrame){
                    explosao[i].curFrame = 0;
                    explosao[i].vida = false;
                }
            }
        }
    }
}

void IniciarBarreira(Barreira *barreira, ALLEGRO_BITMAP *image){
        (*barreira).vida = false;
        (*barreira).maxFrame = 20;
        (*barreira).curFrame = 0;
        (*barreira).frameCount = 0;
        (*barreira).frameDelay = 3;
        (*barreira).frameWidth = 120;
        (*barreira).frameHeight = 120;
        (*barreira).animationColumns = 4;
        (*barreira).image = image;
}

void DesenharBarreira(Barreira *barreira, NaveEspacial *nave){
    if((*barreira).vida){
        int framex, framey;

        framex = ((*barreira).curFrame % (*barreira).animationColumns) * (*barreira).frameWidth;
        framey = ((*barreira).curFrame / (*barreira).animationColumns) * (*barreira).frameHeight;
        al_draw_bitmap_region((*barreira).image, framex, framey, (*barreira).frameWidth, (*barreira).frameHeight,
            (*nave).x - ((*barreira).frameWidth/2), (*nave).y - ((*barreira).frameHeight/2), 0);
    }
}

void ComecarBarreira(Barreira *barreira, float x, float y){
    if(!(*barreira).vida){
        (*barreira).vida = true;
        (*barreira).x = x;
        (*barreira).y = y;
    }
}

void AtualizarBarreira(Barreira *barreira){
    if((*barreira).vida){
        (*barreira).frameCount++;
        if((*barreira).frameCount >= (*barreira).frameDelay){
            (*barreira).curFrame++;
            (*barreira).frameCount = 0;
            if((*barreira).curFrame >= (*barreira).maxFrame){
                (*barreira).curFrame = 0;
                (*barreira).vida = false;
            }
        }
    }
}

void IniciarEstrelas(Estrela estrela[], int tamanho){
	int i;
	for (i=0;i<tamanho; i++){
		estrela[i].vida = false;
	}
}

void DesenharEstrelas(Estrela estrela[], int tamanho){
	int i;
	for(i=0;i<tamanho;i++){
		if(estrela[i].vida){
			al_draw_filled_circle(estrela[i].x, estrela[i].y, estrela[i].raio, al_map_rgb(255,255,255));
		}
	}
}

void ComecarEstrelas(Estrela estrela[], int tamanho){
	int i;
    for(i = 0; i < tamanho; i++){
        if(!estrela[i].vida){
            if(rand() % 100 == 0){
                int num = rand()%100;

            	estrela[i].vida = true;
            	estrela[i].x = LARGURA_TELA + 5;
            	estrela[i].y = rand() % ALTURA_TELA;
            	//estrela[i].raio = (rand()%3)+0.6;

                if(num > 85){
                    estrela[i].raio = 2.5;
                    estrela[i].vel_x = rand()%5 + 10;

                }
                else if(num > 53){
                    estrela[i].raio = 1.6;
                    estrela[i].vel_x = rand()%8 + 2;
                }
                else{
                    estrela[i].raio = 0.6;
                    estrela[i].vel_x = rand()%6 + 0.4;
                }
            }
        }
    }
}

void AtualizarEstrelas(Estrela estrela[], int tamanho, NaveEspacial *nave){
	int i;
    for(i = 0; i < tamanho; i++){
        if(estrela[i].vida){
            estrela[i].x -= estrela[i].vel_x;

            if(estrela[i].raio >= 2){
                estrela[i].x -= 0.5*(*nave).vel_x;
                estrela[i].y -= 0.3*(*nave).vel_y;
            }
            else if(estrela[i].raio >= 1){
                estrela[i].x -= 0.1*(*nave).vel_x;
                estrela[i].y -= 0.1*(*nave).vel_y;
            }
            else{
                estrela[i].x -= 0.03*(*nave).vel_x;
                estrela[i].y -= 0.03*(*nave).vel_y;
            }
            

            if(estrela[i].y < 0-2){
                estrela[i].y = ALTURA_TELA+2;
            }
            else if(estrela[i].y > ALTURA_TELA+2){
                estrela[i].y = 0-2;
            }

        	if(estrela[i].x < -4){
        		estrela[i].vida = false;
        	}
        }
    }
}

bool inicializar(){

    if (!al_init()){
        fprintf(stderr, "Falha ao inicializar a Allegro 5\n");
        return false;
    }

    al_init_font_addon();

    if(!al_init_ttf_addon()){
        fprintf(stderr, "Falha ao inicialiar add-on allegro_ttf");
        return false;
    }

    if(!al_init_image_addon()){
        fprintf(stderr, "Falha ao inicializar o add-on Allegro_image.\n");
        return false;
    }

    if(!al_init_primitives_addon()){
        fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
        return false;
    }

    if(!al_install_keyboard()){
        fprintf(stderr, "Falha ao inicializar teclado.\n");
        return false;
    }

    if(!al_install_audio()){
        fprintf(stderr, "Falha ao inicializar audio.\n");
        return false;
    }

    if(!al_init_acodec_addon()){
        fprintf(stderr, "Falha ao inicializar acodec_addon.\n");
        return false;
    }

    if(!al_reserve_samples(16)){
        fprintf(stderr, "Falha ao inicializar reserve_samples.\n");
        return false;
    }

    audioexplosao = al_load_sample("audio/bomb.ogg");
    audiofoguete = al_load_sample("audio/foguete.ogg");
    audioexplosaonave = al_load_sample("audio/explosaonave.ogg");
    audiolaser = al_load_sample("audio/lasershot.ogg");
    audioescudo = al_load_sample("audio/forcefield.ogg");
    songplay = al_load_sample("audio/jogo.ogg");
    songmenu = al_load_sample("audio/menu.ogg");

    songmenuInstance = al_create_sample_instance(songmenu);
    songplayInstance = al_create_sample_instance(songplay);
    audioInstance = al_create_sample_instance(audiofoguete);

    al_set_sample_instance_playmode(songmenuInstance, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_playmode(songplayInstance, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(songplayInstance, 0.4);
    al_set_sample_instance_playmode(audioInstance, ALLEGRO_PLAYMODE_BIDIR);

    al_attach_audio_stream_to_mixer(songmenuInstance, al_get_default_mixer());
    al_attach_audio_stream_to_mixer(songplayInstance, al_get_default_mixer());
    al_attach_audio_stream_to_mixer(audioInstance, al_get_default_mixer());

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela){
        fprintf(stderr, "Falha ao criar a janela\n");
        return false;
    }

    al_set_window_title(janela, "Asteroids 1.0.0");

    fonte = al_load_font("fonts/test.ttf", 46, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar \"fonte comic.ttf\".\n");
        al_destroy_display(janela);
        return false;
    }

    fonte2 = al_load_font("fonts/test.ttf", 28, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar \"fonte comic.ttf\".\n");
        al_destroy_display(janela);
        return false;
    }

    fonte3 = al_load_font("fonts/test.ttf", 92, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar \"fonte comic.ttf\".\n");
        al_destroy_display(janela);
        return false;
    }

    fonte4 = al_load_font("fonts/ARCADE.ttf", 22, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar \"fonte comic.ttf\".\n");
        al_destroy_display(janela);
        return false;
    }

    fila_eventos = al_create_event_queue();
    if(!fila_eventos){
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }

    imagemfundo = al_load_bitmap(IMGFUNDO);
    if(!imagemfundo){
        fprintf(stderr, "Falha ao carregar arquivo de imagem do fundo.\n");
        al_destroy_display(janela);
        return false;
    }

    imagemmenu = al_load_bitmap(IMGMENU);
    if(!imagemfundo){
        fprintf(stderr, "Falha ao carregar arquivo de imagem de menu.\n");
        al_destroy_display(janela);
        return false;
    }

    imagemnave = al_load_bitmap(IMGNAVE);
    shipacc = al_load_bitmap(ACELNAVE);
    barreiraImg = al_load_bitmap(BARREIRA);
    astG = al_load_bitmap(IMGASTEROIDES[0]);
    astM = al_load_bitmap(IMGASTEROIDES[1]);
    astP = al_load_bitmap(IMGASTEROIDES[2]);
    expImg = al_load_bitmap(EXPLOSAO);
    tiro = al_load_bitmap(SHOT);

    Comprimento_Nave = al_get_bitmap_width(imagemnave);
    Altura_Nave = al_get_bitmap_height(imagemnave);

    if(!imagemnave || !shipacc || !barreiraImg || !astG || !astM || !astP || !expImg || !tiro){
        fprintf(stderr, "Falha ao carregar arquivo de imagem.\n");
        al_destroy_display(janela);
        return false;
    }

    timer = al_create_timer(1.0/FPS);
    if(!timer){
        fprintf(stderr, "Falha ao criar o timer.\n");
        return false;
    }

    contador = al_create_timer(1.0);
    if(!contador){
        fprintf(stderr, "Falha ao criar o conttador.\n");
        return false;
    }

    fila_contador = al_create_event_queue();
    if(!fila_contador){
        fprintf(stderr, "Falha ao criar a fila do contador.\n");
        return false;
    }

    srand(time(NULL));

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_contador, al_get_timer_event_source(contador));

    return true;
}

bool finalizar(){
	al_destroy_timer(timer);
    al_destroy_timer(contador);
    al_destroy_font(fonte);
    al_destroy_font(fonte2);
    al_destroy_font(fonte3);
    al_destroy_font(fonte4);
    al_destroy_bitmap(imagemfundo);
    al_destroy_bitmap(imagemnave);
    al_destroy_bitmap(imagemmenu);
    al_destroy_bitmap(astM);
    al_destroy_bitmap(shipacc);
    al_destroy_bitmap(barreiraImg);
    al_destroy_bitmap(astP);
    al_destroy_bitmap(astG);
    al_destroy_bitmap(expImg);
    al_destroy_bitmap(tiro);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    al_destroy_event_queue(fila_contador);
    al_destroy_sample(audioexplosao);
    al_destroy_sample(audioexplosaonave);
    al_destroy_sample(audiolaser);
    al_destroy_sample(audiofoguete);
    al_destroy_sample(audioescudo);
    al_destroy_sample(songplay);
    al_destroy_sample(songmenu);
    al_destroy_sample_instance(songmenuInstance);
    al_destroy_sample_instance(songplayInstance);
    al_destroy_sample_instance(audioInstance);
}
