#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <dirent.h>
#include <string.h>
#include "traverse.h"
#include "save.h"

#define MENU 0
#define GAME 1
#define PAUSE 2
#define WIN 3
#define VERSUS_2 0
#define VERSUS_IA 1
#define VERSUS_4 2

typedef struct Color {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} Color;

typedef struct Theme {
	Color color1;
	Color color2;
	SDL_Texture* bg;
	SDL_Texture* sprites[16];
	SDL_Texture* buttons[15];
	SDL_Texture* wins[5];
} Theme;

char** getThemesNames(int* pN) {
	DIR *dp;
	dp = opendir ("./assets");
	struct dirent *ep;
	char** names;
	int n = 0;
	if (dp != NULL) {
		while (ep = readdir(dp))
			if (ep->d_type == DT_DIR && ep->d_name[0] != '.')
				n++;
		printf("%d thèmes trouvés :\n", n);
		names = malloc(n*sizeof(char*));
		dp = opendir ("./assets");
		int i = 0;
		while (ep = readdir(dp))
			if (ep->d_type == DT_DIR && ep->d_name[0] != '.') {
				names[i] = malloc(10*sizeof(char));
				strcpy(names[i], ep->d_name);
				printf("\t%s (%d)\n", names[i], i);
				i++;
			}
		(void) closedir(dp);
	} else printf("Erreur : impossible d'acceder aux thèmes\n");
	if (pN != NULL) *pN = n;
	return names;
}

Theme loadTheme(SDL_Renderer *pRenderer, Color color1, Color color2, char* name) {
	printf("Chargement du thème : \"%s\"\n", name);
	Theme theme;
	theme.color1 = color1;
	theme.color2 = color2;
	char fileName[25];
	for (int i = 0; i < 16; i++) {
		snprintf(fileName, 25, "./assets/%s/s%d.bmp", name, i+1);
		SDL_Surface* pSprite = SDL_LoadBMP(fileName); // Charger un sprite
		theme.sprites[i] = SDL_CreateTextureFromSurface(pRenderer, pSprite); // Préparation du sprite
		SDL_FreeSurface(pSprite); // Libération
	}
	for (int i = 0; i < 15; i++) {
		snprintf(fileName, 25, "./assets/default/b%d.bmp", i);
		SDL_Surface* pSprite = SDL_LoadBMP(fileName); // Charger un sprite
		theme.buttons[i] = SDL_CreateTextureFromSurface(pRenderer, pSprite); // Préparation du sprite
		SDL_FreeSurface(pSprite); // Libération
	}
	for (int i = 0; i < 5; i++) {
		snprintf(fileName, 25, "./assets/default/w%d.bmp", i);
		SDL_Surface* pSprite = SDL_LoadBMP(fileName); // Charger un sprite
		theme.wins[i] = SDL_CreateTextureFromSurface(pRenderer, pSprite); // Préparation du sprite
		SDL_FreeSurface(pSprite); // Libération
	}
	snprintf(fileName, 25, "./assets/%s/bg.bmp", name);
	SDL_Surface* pBgSprite = SDL_LoadBMP(fileName);
	theme.bg = SDL_CreateTextureFromSurface(pRenderer, pBgSprite);
	SDL_FreeSurface(pBgSprite);
	return theme;
}

void destroyTheme(Theme theme) {
	for (int i = 0; i < 8; i++) {
		SDL_DestroyTexture(theme.sprites[i]);
	}
	for (int i = 0; i < 11; i++) {
		SDL_DestroyTexture(theme.buttons[i]);
	}
	SDL_DestroyTexture(theme.bg);
}

void printMenu(SDL_Renderer *pRenderer, int w, int h, Theme theme) {
	SDL_Rect destBg = {0, 0, w, w};
	SDL_RenderCopy(pRenderer, theme.bg, NULL, &destBg);
	for (int i = 0; i < 3; i++) { // les 3 (ou 4) boutons de jeu
		SDL_Rect dest = {w/11+(i%2)*w/2, 3*h/10+(i/2)*h/5, 3*w/11, h/10};
		SDL_RenderCopy(pRenderer, theme.buttons[i], NULL, &dest);
	}
	for (int i = 0; i < 3; i++) { // les 3 boutons de chargement de parties
		SDL_Rect dest = {384+i*96, 320, 64, 64};
		SDL_RenderCopy(pRenderer, theme.buttons[12+i], NULL, &dest);
	}
	SDL_Rect dest1 = {w/11, 4*h/5, w/11, h/10}; // Bouton thème précédent
	SDL_RenderCopy(pRenderer, theme.buttons[4], NULL, &dest1);
	SDL_Rect dest2 = {9*w/11, 4*h/5, w/11, h/10}; // Bouton thème suivant
	SDL_RenderCopy(pRenderer, theme.buttons[5], NULL, &dest2);
}

void printQuadrillage(SDL_Renderer *pRenderer, int w, Theme theme, int** plateau, int ignoreX, int ignoreY) {
	SDL_SetRenderDrawColor(pRenderer, theme.color1.r, theme.color1.g, theme.color1.b, theme.color1.a);
	w /= 10;
	for (int i = 0; i < 10; i++) for (int j = i%2; j < 10; j+=2) {
		SDL_Rect r = {i*w, j*w, w, w};
		SDL_RenderFillRect(pRenderer, &r);
	}
	SDL_SetRenderDrawColor(pRenderer, theme.color2.r, theme.color2.g, theme.color2.b, theme.color2.a);
	for (int i = 0; i < 10; i++) for (int j = (i+1)%2; j < 10; j+=2) {
		SDL_Rect r = {i*w, j*w, w, w};
		SDL_RenderFillRect(pRenderer, &r);
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (plateau[i][j] != 0) {
				if (i==ignoreY && j==ignoreX) continue;
				SDL_Rect dest = {j*w, i*w, w, w}; // Destination du sprite
				SDL_RenderCopy(pRenderer, theme.sprites[plateau[i][j]-1], NULL, &dest); // Copie du sprite grâce au SDL_Renderer
			}
		}
	}
	SDL_Rect dest1 = {10*w, 9*w, w, w}; // bouton pause
	SDL_RenderCopy(pRenderer, theme.buttons[7], NULL, &dest1);

	SDL_Rect dest2 = {10*w ,0*w, w, w}; // bouton menu
	SDL_RenderCopy(pRenderer, theme.buttons[6], NULL, &dest2);

	for (int i = 0; i < 3; i++) { // boutons de sauvegardes
		SDL_Rect dest = {10*w, (2+i)*w, w, w};
		SDL_RenderCopy(pRenderer, theme.buttons[9+i], NULL, &dest);
	}
}

void resetPlateau(int*** p, int players) {
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			(*p)[i][j] = 0;
	if (players >= 1) {
		(*p)[9][1] = (*p)[9][8] = 2;
		(*p)[9][2] = (*p)[9][7] = 4;
		(*p)[9][3] = (*p)[9][6] = 3;
		(*p)[9][4] = (*p)[9][5] = 1;
	}
	if (players >= 2) {
		(*p)[0][1] = (*p)[0][8] = 6;
		(*p)[0][2] = (*p)[0][7] = 8;
		(*p)[0][3] = (*p)[0][6] = 7;
		(*p)[0][4] = (*p)[0][5] = 5;
	}
	if (players >= 3) {
		(*p)[1][9] = (*p)[8][9] = 10;
		(*p)[2][9] = (*p)[7][9] = 12;
		(*p)[3][9] = (*p)[6][9] = 11;
		(*p)[4][9] = (*p)[5][9] = 9;
	}
	if (players >= 4) {
		(*p)[1][0] = (*p)[8][0] = 14;
		(*p)[2][0] = (*p)[7][0] = 16;
		(*p)[3][0] = (*p)[6][0] = 15;
		(*p)[4][0] = (*p)[5][0] = 13;
	}
}

int main(int argc, char *argv[]) {
	
	if (SDL_Init(SDL_INIT_VIDEO) == -1) { // Démarrage de la SDL // SDL_INIT_EVENTS ?
		fprintf(stderr, "Erreur d'initialisation de la SDL");
		exit(EXIT_FAILURE);
	}
	
	SDL_Window* pWindow = NULL; // Création de la fenêtre
	
	pWindow = SDL_CreateWindow("Traverse - JJ x Flo x Kéké", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 704, 640, SDL_WINDOW_SHOWN);
	
	SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow,-1,SDL_RENDERER_ACCELERATED); // Création du renderer (pour afficher)
	
	// Variables de thèmes
	int themesN;
	char** themesNames = getThemesNames(&themesN);
	int themeIndex = 2;
	//Color color1 = {200, 255, 150, 255};
	//Color color2 = {100, 255, 100, 255};
	Color color1 = {255, 255, 255, 255};
	Color color2 = {0, 0, 0, 255};
	Theme theme = loadTheme(pRenderer, color1, color2, themesNames[themeIndex]);
	
	// Variables de jeu
	int joueur = 0;
	int** plateau = malloc(10*sizeof(int*));
	for (int i = 0; i < 10; i++) plateau[i] = malloc(10*sizeof(int));
	int grabbed = 0; // si un pion est pris
	int grabbedX = 0, grabbedY = 0;
	int mouseX = 0, mouseY = 0;
	
	int state = MENU; // Etat du programme
	int gamemode;
	
	while (1) {
		
		SDL_RenderClear(pRenderer); // Netoyage du SDL_Renderer
		
		int endProgram = 0;
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) { // les events
			switch (event.type) {
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE) endProgram = 1;
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == 27) endProgram = 1;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (state == GAME && event.button.button == SDL_BUTTON_LEFT) {
						if(event.motion.x>640)break;
						grabbed = plateau[event.motion.y/64][event.motion.x/64];
						grabbedX = event.motion.x/64;
						grabbedY = event.motion.y/64;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (state == GAME && event.button.button == SDL_BUTTON_LEFT) {
						if (sqrt((event.motion.x-672)*(event.motion.x-672)+(event.motion.y-608)*(event.motion.y-608))<=32){ //clic sur bouton pause
							state = PAUSE;
						}
						for (int i = 1; i <= 3; i++) {
							if (sqrt((event.motion.x-672)*(event.motion.x-672)+(event.motion.y-96-i*64)*(event.motion.y-96-i*64))<=32){ //clic sur bouton de sauvegarde i
								printf("Sauvegarde dans l'emplacement %d...", i);
								save(plateau, joueur, gamemode, i);
							}
						}
						if (sqrt((event.motion.x-672)*(event.motion.x-672)+(event.motion.y-32)*(event.motion.y-32))<=32){ //clic sur bouton pause
							state = MENU;
						}
						if(event.motion.x>640)break;
						if (grabbed) {
							printf("Le joueur %d veut déplacer un pion de (%d;%d) vers (%d;%d)\n", joueur, grabbedX, grabbedY, mouseX/64, mouseY/64);
							if (canMove(plateau, joueur, grabbedX, grabbedY, mouseX/64, mouseY/64)) {
								plateau[mouseY/64][mouseX/64] = plateau[grabbedY][grabbedX];
								plateau[grabbedY][grabbedX] = 0;
								printf("Le joueur %d a joué !\n", joueur);
								if (win(plateau) != -1) {
									joueur = win(plateau);
									state = WIN;
								}
								if (gamemode == VERSUS_2) {
									if (joueur == 1) joueur = 0;
									else joueur++;
								} else if (gamemode == VERSUS_IA) {
									int fromX, fromY, destX, destY;
									ia(plateau, joueur==0?1:0, joueur==0?1:0, 4, &fromX, &fromY, &destX, &destY);
									plateau[destY][destX] = plateau[fromY][fromX];
									plateau[fromY][fromX] = 0;
									if (win(plateau) != -1) {
										joueur = win(plateau);
										state = WIN;
									}
								} else if (gamemode == VERSUS_4) {
									if (joueur == 3) joueur = 0;
									else joueur++;
								}
							}
						}
						grabbed = 0;
					}
					else if (state == WIN) {
						state = MENU;
					}
					if (state == MENU) {
						if (sqrt((event.motion.x-96)*(event.motion.x-96)+(event.motion.y-544)*(event.motion.y-544)) <= 32) { // clic sur bouton gauche
							destroyTheme(theme);
							if (themeIndex == 0) themeIndex = themesN-1;
							else themeIndex--;
							theme = loadTheme(pRenderer, color1, color2, themesNames[themeIndex]);
						}
						if (sqrt((event.motion.x-604)*(event.motion.x-608)+(event.motion.y-544)*(event.motion.y-544)) <= 32) { // clic sur bouton droit
							destroyTheme(theme);
							themeIndex++;
							if (themeIndex == themesN) themeIndex = 0;
							theme = loadTheme(pRenderer, color1, color2, themesNames[themeIndex]);
						}
						if (event.motion.y >= 192 && 256 >= event.motion.y) { // boutons du haut
							if (event.motion.x >= 64 && 256 >= event.motion.x) { // bleu : VERSUS_2
								state = GAME;
								gamemode = VERSUS_2;
								resetPlateau(&plateau, 2);
								joueur = 0;
							} else if (event.motion.x >= 64+352 && 256+352 >= event.motion.x) {// vert : VERSUS_4
								state = GAME;
								gamemode = VERSUS_4;
								resetPlateau(&plateau, 4);
								joueur = 0;
							}
						} else if (event.motion.y >= 320 && 384 >= event.motion.y) { // boutons du bas
							if (event.motion.x >= 64 && 256 >= event.motion.x) { // jaune : VERSUS_IA
								state = GAME;
								gamemode = VERSUS_IA;
								resetPlateau(&plateau, 2);
								joueur = 0; 
							} else if (sqrt((event.motion.x-384-32)*(event.motion.x-384-32)+(event.motion.y-320-32)*(event.motion.y-320-32))<=32) { // chargement 1
								if (loadGame(1, &plateau, &joueur, &gamemode)) {
									state = GAME;
									printf("Partie chargée depuis l'emplacement %d\n", 1);
								} else printf("Cette sauvegarde est vide.\n");
							} else if (sqrt((event.motion.x-384-32-96)*(event.motion.x-384-32-96)+(event.motion.y-320-32)*(event.motion.y-320-32))<=32) { // chargement 2
								if (loadGame(2, &plateau, &joueur, &gamemode)) {
									state = GAME;
									printf("Partie chargée depuis l'emplacement %d\n", 2);
								} else printf("Cette sauvegarde est vide.\n");
							} else if (sqrt((event.motion.x-384-32-192)*(event.motion.x-384-32-192)+(event.motion.y-320-32)*(event.motion.y-320-32))<=32) { // chargement 3
								if (loadGame(3, &plateau, &joueur, &gamemode)) {
									state = GAME;
									printf("Partie chargée depuis l'emplacement %d\n", 3);
								} else printf("Cette sauvegarde est vide.\n");
							}
						}
					}
					if (state == PAUSE){
						if (sqrt((event.motion.x-352)*(event.motion.x-352)+(event.motion.y-320)*(event.motion.y-320))<=64){ //clic sur bouton play
							state = GAME;
						}
					}
					break;
				case SDL_MOUSEMOTION:
					mouseX = event.motion.x;
					mouseY = event.motion.y;
					break;
			}
		}
		
		if (state == MENU) {
			printMenu(pRenderer, 704, 640, theme);
		}
		
		if (state == GAME) {
			printQuadrillage(pRenderer, 640, theme, plateau, grabbed?grabbedX:-1, grabbed?grabbedY:-1);
			if (grabbed) {
				SDL_Rect depl = {mouseX-32, mouseY-32, 64, 64}; // Destination du sprite
				SDL_RenderCopy(pRenderer, theme.sprites[grabbed-1], NULL, &depl); // Copie du sprite grâce au SDL_Renderer
			}
		}

		if (state == PAUSE) {
			SDL_Rect pause = {288, 256, 128, 128};
			SDL_RenderCopy(pRenderer, theme.buttons[8], NULL, &pause);
		}
		
		if (state == WIN) {
			SDL_Rect dest = {32, 0, 640, 640};
			SDL_RenderCopy(pRenderer, theme.wins[joueur], NULL, &dest);
		}

		if (endProgram) break; // Condition d'arrêt du programme
		
		SDL_RenderPresent(pRenderer); // Affichage
		
		SDL_Delay(1000/30); // Attendre 1/30 seconde
		
	}
	
	destroyTheme(theme); // Libération de la mémoire associée au theme
	
	SDL_DestroyRenderer(pRenderer); // Libération de la mémoire du SDL_Renderer
	
	SDL_DestroyWindow(pWindow); // Détruire la fenêtre
	
	SDL_Quit(); // Arrêt de la SDL
	
	return EXIT_SUCCESS;
}
