#ifndef SAVE_H
#define SAVE_H
#include <stdio.h>

void save(int** plateau, int joueur, int gamemode, int save) {
	FILE *fp;
	char filename[] = "./saves/0";
	filename[8] = save + '0';
	fp = fopen(filename, "w+");
	fprintf(fp, "%c", gamemode + '0');
	fprintf(fp, "%c", joueur + '0');
	for(int i=0;i<=9;i++){	
		for(int j=0;j<=9;j++){
			fprintf(fp, "%c", plateau[i][j] + '0');
		}
	}
	fclose(fp);
}

int loadGame(int save, int*** plateau, int* joueur, int* gamemode) {
	FILE *saveFile;
	char filename[] = "./saves/0";
	filename[8] = save + '0';
	
	saveFile = fopen(filename, "r");
	if (saveFile == NULL){
		return 0;
	} else {
		char ch;
		int i = 0;
		while ((ch = fgetc(saveFile)) != EOF){ //EOF représente la fin d'un fichier texte
			if (i == 0) *gamemode = ch - '0';
			else if (i == 1) *joueur = ch - '0';
			else if (i < 102) (*plateau)[(i-2)/10][(i-2)%10] = ch - '0';
			i++;
		}
	}
	fclose(saveFile);
	return 1;
}

#endif
