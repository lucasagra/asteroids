typedef struct {

	float x;
	float y;
	int vida;
	float vel_x;
	float vel_y;
	int angulo;
	int raio;

}NaveEspacial;

typedef struct {

	float x;
	float y;
	bool vida;
	float vel_x;
	float vel_y;
	int angulo;
	int raio;

}Projetil;

typedef struct {

	float x;
	float y;
	bool vida;
	float vel_x;
	float vel_y;
	int angulo;
	int raio;
	int nivel;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	ALLEGRO_BITMAP *image;

}Asteroide;

typedef struct {

	float x;
	float y;
	bool vida;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;

	ALLEGRO_BITMAP *image;

}Explosao;

typedef struct {

	float x;
	float y;
	bool vida;

	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;

	ALLEGRO_BITMAP *image;

}Barreira;

typedef struct {

	float x;
	float y;
	bool vida;
	float raio;
	float vel_x;
	float vel_y;

}Estrela;
