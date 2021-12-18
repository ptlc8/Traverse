#ifndef TRAVERSE_H
#define TRAVERSE_H

#include <math.h>

float rac2 = sqrt(2); //définit la racine de 2

float rayon(int posX, int posY, int destX, int destY){
	/* Calcul la distance parcourue par une forme */
	float r = sqrtf((destX - posX)*(destX - posX) + (destY - posY)*(destY - posY)); //Pythagore
	return r;
}

int oqp(int** plateau, int x, int y){
	/* Indique si une case est occupée*/
	if (plateau[y][x] != 0){
		return 1;
	} else {
		return 0;
	}
}

int canMove(int** plateau, int j, int posX, int posY, int destX, int destY) {
	/* Vérifie tous les cas de figures pour savoir si une forme peut-être déplacée */
	
	float r = rayon(posX, posY, destX, destY);
	//Définit la distance parcourue par la forme
	
	if (j != (plateau[posY][posX]-1)/4){
		//printf("mauvais joueur\n");
		return 0;
	}
	/*Vérifie que le joueur joue bien sa forme respective */
	
	if (destX < 0 || destY < 0 || destX > 9 || destY > 9) {
		//printf("en dehors du terrain\n");
		return 0;
	}
	// Vérifie que la forme ne tombe pas parterre
	
	if (oqp(plateau, destX, destY) == 1){ //Ne peut pas arriver sur une autre forme
		//printf("oqp\n");
		return 0;
	}
	
	if (r != 1 && r != rac2 && r != 2 && r != 2*rac2){ //Ne peut se déplacer que d'une ou 2 case alignéee ou en diagonales.
		//printf("mouvement impossible\n");
		return 0;
	}

	int d;
	if (r == 1 || r == rac2) { //Déplacement de 1 case
         	d=1;
		//printf("mouvement simple\n");
	} else { //Déplacement de 2 cases (Nécessite de sauter au dessus d'une autre forme)
		int obstacleX = (destX + posX)/2; // Position x de la où devrait se trouver l'autre forme
		int obstacleY = (destY + posY)/2; // Position y de la où devrait se trouver l'autre forme
		
		if (oqp(plateau, obstacleX, obstacleY) == 0) { //Si la forme jouée ne peut en aucun cas sauter au dessus d'une autre
			//printf("saut impossible\n");
			return 0;
                 }
		 d=2;
	         //printf("mouvement sauté\n");
	}
        switch ((plateau[posY][posX]-1)%4) { // vérifie que ça correspond bien au move de la forme
			case 0: // rond
				return d;
			case 1: // carré
				if (((destY == posY+d) || (destY == posY-d)) ^ ((destX == posX+d) || (destX == posX-d))) {
					return d; // bon déplacement
				} else {
					return 0; // mauvais déplacement
				}
				break;
			case 2: // losange
				if ((destY == posY+d || destY == posY-d) && (destX == posX+d || destX == posX-d)) {
					return d; // bon déplacement
				} else {
					return 0; // mauvais déplacement
				}
			case 3: // triangle
				if (plateau[posY][posX] == 4) { // triangle j1
					if ((destY == posY+d && destX == posX) ^ ((destY == posY-d) && (destX == posX+d)) ^ ((destY == posY-d) && (destX == posX-d))) {
						return d; // bon déplacement
					} else {
						return 0; // mauvais déplacement
					}
				}
				if (plateau[posY][posX] == 8) { // triangle j2
					if ((destY == posY-d && destX == posX) ^ ((destY == posY+d) && (destX == posX+d)) ^ ((destY == posY+d) && (destX == posX-d))) {
						return d; // bon déplacement
					} else {
						return 0; // mauvais déplacement
					}
				}
				if (plateau[posY][posX] == 12) { // triangle j3
                    if ((destY == posY && destX == posX+d) ^ ((destY == posY-d) && (destX == posX-d)) ^ ((destY == posY+d) && (destX == posX-d))) {
                        return d; // bon déplacement
                    } else {
                        return 0; // mauvais déplacement
                    }
                }
                if (plateau[posY][posX] == 16) { // triangle j4
                    if ((destY == posY && destX == posX-d) ^ ((destY == posY-d) && (destX == posX+d)) ^ ((destY == posY+d) && (destX == posX+d))) {
                        return d; // bon déplacement
                    } else {
                        return 0; // mauvais déplacement
                    }
                }
				break;
		}
}




int canJumpAgain(int** plateau, int obstacleX, int obstacleY, int x, int y){
/* Vérifie si un joueur peut sauter de nouveau au dessus d'une autre forme */
  for (int i = y-1; i <= x+1 ; i++) {

    for (int j = x-1; j <= x+1; j++){

      if (i < 0){
        i = 0;
      } else if (i > 9){
        i = 9;
      }
      if (j < 0){
        i = 0;
      } else if (j > 9){
        i = 9;
      }
      /* Fait en sorte que les valeurs de i et j ne sortent pas du tableau */

      if (plateau[i][j] != 0 && (i != obstacleY || j != obstacleX) && (i != y || j != x)){
        return 1; //Si il existe une autre forme aux alentours différent de la forme déjà sautée
      }
    }
  }
  return 0;
}

//Si renvoie 1, demande au joueur si il veut sauter par dessus une autre forme
//Si oui il doit préciser la nouvelle destination de la forme, vérifiée par canMove()7


// Fonction test de victoire
int win(int **plateau){
    int arrivedP1 = 0, arrivedP2 = 0, arrivedP3 = 0, arrivedP4 = 0;

    for (int i = 0; i < 10; i++){
        if (plateau[0][i] >= 1 && plateau[0][i] < 5){
            arrivedP1 += 1;
        }
    }

    for (int j = 0; j < 10; j++){
        if (plateau[9][j] >= 5 && plateau[9][j] < 9 ){
            arrivedP2 += 1;
        }
    }

    for (int k = 0; k < 10; k++){
        if (plateau[k][0] >= 9 && plateau[k][0] < 13){
            arrivedP3 += 1;
        }
    }

    for (int l = 0; l < 10; l++){
        if (plateau[l][9] >= 13 && plateau[l][9] < 17){
            arrivedP4 += 1;
        }
    }

    if (arrivedP1 == 8){
        return 0;
    } else if (arrivedP2 == 8){
        return 1;
    } else if (arrivedP3 == 8){
        return 2;
    } else if (arrivedP4 == 8){
        return 3;
    } else{
        return -1;
    }
}


//Fonction IA

int ia(int** plateau, int IA, int joueur, int howfar, int* fromX, int* fromY, int* destX, int* destY){
    if(howfar==0){
        int score=0;
        for(int i=0;i<=9;i++){	
            for(int j=0;j<=9;j++){
                if(plateau[i][j]!=0){
                    if((plateau[i][j]-1)/4==IA){
                        if(IA==1){
                           score=score+i;
                        }
                        //if(IA==0){
                            //score=score-i;
                        //}
                    }
                    else{
                        if(IA==1){
                           score=score-9+i;
                        }
                        //if(IA==0){
                            //score=score-9+i;
                        //}
                    }
                }
            }
        }
    *fromX=*fromY=*destX=*destY=-1;
    return score;
    }
    int scoreMAX=-100;
    int A, B, C, D;
    for(int i=0;i<=9;i++){	
        for(int j=0;j<=9;j++){
            if(plateau[i][j]!=0 && (plateau[i][j]-1)/4==joueur){
                for(int k=i-2;k<=i+2;k++){
                    for(int l=j-2;l<=j+2;l++){
                        if(canMove(plateau, joueur, j, i, l, k)){
                            plateau[k][l]=plateau[i][j];
                            plateau[i][j]=0;
			    int a, b, c, d;
                            int score=ia(plateau, IA, joueur==0?1:0, howfar-1, &a, &b, &c, &d);
                            if(score>scoreMAX){
                                scoreMAX=score;
                                A=j;
                                B=i;
                                C=l;
                                D=k;
                            }
                            plateau[i][j]=plateau[k][l];
                            plateau[k][l]=0;
                        }
                    }
                } 
            }
        }
    }
    *fromX=A;
    *fromY=B;
    *destX=C;
    *destY=D;
    return scoreMAX;
}

#endif
