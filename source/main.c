#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <stdlib.h>
#include <math.h>
#include "touch.h"

//Reviewing this piece of code, this could have been done a lot better (especially the AI)
//I learned from this project, but you probably won't. 3DS specific stuff is pretty ok though
//so if that's why you are here go ahead and sniff around how I did certain 3DS related things.


//Init vars
int board[8][8];
int temp_board[8][8];
int poss_board[8][8]; //To render possible places for player to place the disk.
// ^          Board starts bottom left ala 3DS (software) rendering
// |
// |
// 0,0 --->
int turn=0; //Who's turn is it?
int score[2]; //Score for each player/CPU
int mode=0; //0: menu 1: 2-Player 2: agains CPU
bool renderPoss=true; //Should hint be rendered
int input[2];
int cpuDifficulty=2; //0=easy 1=medium 2=hard
bool GameOver=false;
int turns=0;
int scores[90][2];
int win; //Color of winner
int action=1; //Make new animation system work in worst way possible ;)
float scale_board[8][8];
float scale;
float animateTime=0;
int menu=1;
int menstate=0;
int posVal[8][8]={
	{50 ,-50,5,5,5,5,-50,50 },
	{-50,-50,0,0,0,0,-50,-50},
	{5  ,0  ,6,0,0,0,6  ,5  },
	{5  ,0  ,0,0,0,0,0  ,5  },
	{5  ,0  ,0,0,0,0,0  ,5  },
	{5  ,0  ,6,0,0,0,6  ,5  },
	{-50,-50,0,0,0,0,-50,-50},
	{50 ,-50,5,5,5,5,-50,50 }
};

touchPosition touch;

void animation(){
	int x,y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			//if (board[x][y] > 2)
			//	board[x][y]-=2;
		}
	}
}

void cloneField(){
	int x,y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			temp_board[x][y]=board[x][y];
		}
	}
}

void clearScaleBoard(){
	int x,y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			scale_board[x][y]=1;
		}
	}
}

void fillArea(int x1, int y1, int x2, int y2, int col){
	int x,y,z;
	if (y1 == y2 || x1 == x2){
		for (x=x1; x<=x2;x++){
			for (y=y1; y<=y2;y++){
				board[x][y]=col;
				scale_board[x][y]=0;
				animateTime=0;
			}
		}
	}else{
		x=x1;
		y=y1;
		int dirx=-1;
		int diry=-1;
		if (y1<y2)
			diry=1;
		if (x1<x2)
			dirx=1;
		for (z=0; z<=(x2-x1); z++){
			board[x+z*dirx][y+z*diry]=col;
			scale_board[x+z*dirx][y+z*diry]=0;
			animateTime=0;
		}
	}
}

bool placeTile(int x, int y, int col){
	//Col color to place
	//x pos of placement
	//y pos of placement
	int tx,ty;
	if (board[x][y] == 0){
		bool succes=false;
		int k;
		for (k=x-1; k>=0; k--){
			if (board[k][y] == 0)
				break;
			if (board[k][y] == col){
				if (k == x-1)
					break;
				succes=true;
				fillArea(k,y,x,y,col);
				break;
			}
		}
		for (k=x+1; k<8; k++){
			if (board[k][y] == 0)
				break;
			if (board[k][y] == col){
				if (k == x+1)
					break;
				succes=true;
				fillArea(x,y,k,y,col);
				break;
			}
		}
		for (k=y-1; k>=0; k--){
			if (board[x][k] == 0)
				break;
			if (board[x][k] == col){
				if (k == y-1)
					break;
				succes=true;
				fillArea(x,k,x,y,col);
				break;
			}
		}
		for (k=y+1; k<8; k++){
			if (board[x][k] == 0)
				break;
			if (board[x][k] == col){
				if (k == y+1)
					break;
				succes=true;
				fillArea(x,y,x,k,col);
				break;
			}
		}
		for (k=1; k<8; k++){
			tx=x+k;
			ty=y+k;
			if (tx>=0 && tx<8 && ty>=0 && ty<8){
				if (board[tx][ty] == 0)
					break;
				if (board[tx][ty] == col){
					if (k == 1)
						break;
					succes=true;
					fillArea(x,y,tx,ty,col);
					break;
				}
			}else{
				break;
			}
		}
		for (k=1; k<8; k++){
			tx=x+k;
			ty=y-k;
			if (tx>=0 && tx<8 && ty>=0 && ty<8){
				if (board[tx][ty] == 0)
					break;
				if (board[tx][ty] == col){
					if (k == 1)
						break;
					succes=true;
					fillArea(x,y,tx,ty,col);
					break;
				}
			}else{
				break;
			}
		}
		for (k=1; k<8; k++){
			tx=x-k;
			ty=y-k;
			if (tx>=0 && tx<8 && ty>=0 && ty<8){
				if (board[tx][ty] == 0)
					break;
				if (board[tx][ty] == col){
					if (k == 1)
						break;
					succes=true;
					fillArea(tx,ty,x,y,col);
					break;
				}
			}else{
				break;
			}
		}
		for (k=1; k<8; k++){
			tx=x-k;
			ty=y+k;
			if (tx>=0 && tx<8 && ty>=0 && ty<8){
				if (board[tx][ty] == 0)
					break;
				if (board[tx][ty] == col){
					if (k == 1)
						break;
					succes=true;
					fillArea(tx,ty,x,y,col);
					break;
				}
			}else{
				break;
			}
		}
		return succes;
	}
	return false;
}

int getScore(int col){
	int ret=0;
	int x,y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			if (board[x][y] == col || board[x][y] == col+2){
				ret+=1;
			}
		}
	}
	return ret;
}

bool updatePossBoard(int col){ //Updates pos board for given col, returns true if any move is even possible
	int x,y,j,k;
	bool ret=false;
	cloneField();
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			for (j=0; j<8; j++){
				for (k=0; k<8; k++){
					board[j][k]=temp_board[j][k];
				}
			}
			poss_board[x][y]=0;
			if (board[x][y] == 0){
				if (placeTile(x,y,col)){
					poss_board[x][y]=1;
					ret=true;
				}
			}
		}
	}
	for (j=0; j<8; j++){
		for (k=0; k<8; k++){
			board[j][k]=temp_board[j][k];
		}
	}
	return ret;
}

bool endTurn(int col){
	bool ret=false;
	if (turns <= 60)
		turns+=1;
	int x=floor(turns/2);
	int y=turns-x*2;
	if (y == 0){
		scores[x][0]=getScore(2);
		scores[x][1]=getScore(1);
	}
	float temp_scale_board[8][8];
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			temp_scale_board[x][y]=scale_board[x][y];
		}
	}
	int col2=col-2*col+3;
	if (!updatePossBoard(col2)){
		ret=true;
		if (!updatePossBoard(col)){
			GameOver=true;
			ret=false;
			if (getScore(2)>getScore(1)){
				win=1;
			}else if (getScore(2) != getScore(1)){
				win=2;
			}else{
				win=0;
			}
		}
	}
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			scale_board[x][y]=temp_scale_board[x][y];
		}
	}
	animateTime=0;
	return ret;
}

void resetFieldFromTemp(){
	int x,y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			board[x][y]=temp_board[x][y];
		}
	}
}

int evalPos(int x, int y, int cpuDifficulty){
	if (cpuDifficulty == 0){
		if (x == 0 || y == 0 || x == 7 || y == y){
			return 5;
		}else{
			return 0;
		}
	}else if (cpuDifficulty == 1){
		if ((x == 0 && y == 0) || (x == 7 && y == 7) || (x == 0 && y == 7) || (x == 7 && y == 0)){
			return 50;
		}else{
			return 0;
		}
	}else{
		return posVal[x][y];
	}
}

void cpuTurn(int col){
	//canPlay(col);
	u64 timeInSeconds = osGetTime() / 1000;
	srand (timeInSeconds);
	int o_col=col-2*col+3;

	int baseScore=getScore(col);
	int o_baseScore=getScore(o_col);
	int bonus=0;
	int o_bonus=0;
	int curAttempt=0;
	int o_curAttempt=0;
	int bestAttempt=-9000;
	int o_bestAttempt=-9000;

	int movex=0,movey=0;
	int x,y;
	int j,k,w,e;
	int o_board[8][8]; //Oponents board with current cpu turn? IDK how to describe it!
	cloneField();
	
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			resetFieldFromTemp();
			if (board[x][y] == 0){
				if (placeTile(x,y,col)){
					bonus=evalPos(x,y, cpuDifficulty);
					if (cpuDifficulty == 2){
						o_curAttempt=0;
						o_bestAttempt=-9000;
						for (w=0; w<8; w++){
							for (e=0; e<8; e++){
								for (j=0; j<8; j++){
									for (k=0; k<8; k++){
										o_board[x][y]=board[x][y];
									}
								}
								if (placeTile(x,y,o_col)){
									o_curAttempt=evalPos(x,y,2);
									if (o_curAttempt > o_bestAttempt){
										o_bestAttempt=o_curAttempt;
									}
								}
							}
						}
						if (o_bestAttempt == -9000)
							o_bestAttempt=-100; //If the oponent cannot make a move this is of advantage to the CPU
						bonus-=o_bestAttempt;
					}
					curAttempt=getScore(col)-baseScore+bonus;
					if (curAttempt > bestAttempt){
						movex=x;
						movey=y;
						bestAttempt=curAttempt;
					}
				}
			}
		}
	}

	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			board[x][y]=temp_board[x][y];
		}
	}
	if (bestAttempt > -9000){
		clearScaleBoard();
		placeTile(movex,movey,col);
		animateTime=0;
	}
}

void resetGame(){
	int x,y;
	for (x=0;x<8;x++){
		for (y=0;y<8;y++){
			board[x][y]=0;
			temp_board[x][y]=0;
			poss_board[x][y]=0;
		}
	}
	for (x=0; x<30; x++){
		for (y=0; y<2; y++){
			scores[x][y]=0;
		}
	}
	turns=0;
	GameOver=false;
	board[3][3]=1;
	board[4][4]=1;
	board[3][4]=2;
	board[4][3]=2;
	clearScaleBoard();
	updatePossBoard(1);
}

touchPosition touch;

void getTouchBoardXY(touchPosition touch){
	input[0]=-1;
	int tx=touch.px-8; //Board pos on touch screen
	int ty=touch.py-8;
	u32 kUp = hidKeysDown();
	if (kUp & KEY_TOUCH){
		input[0]=floor(tx/28);
		input[1]=floor(ty/28);
		if (input[0] > 7 || input[1] > 7 || input[0] < 0 || input[1] < 0){
			input[0]=-1;
		}
	}
}

void drawLine(int w, int q, int r, int e, u32 col){
	if (r<w)
		sf2d_draw_line(r,e,w,q,col);
	if (w<r)
		sf2d_draw_line(w,q,r,e,col);
}


int main(int argc, char * argv[])
{
	romfsInit();

	sf2d_init();
	sf2d_set_clear_color(RGBA8(235, 232, 227, 255));


	sf2d_texture *t_loading = sfil_load_PNG_file("romfs:/Loading.png", SF2D_PLACE_RAM);

	sf2d_start_frame(GFX_TOP, GFX_LEFT);

	sf2d_end_frame();

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(t_loading, 200, 190);
	sf2d_end_frame();


	sf2d_free_texture(t_loading);

	sf2d_swapbuffers();

	sf2d_texture *t_bottom = sfil_load_PNG_file("romfs:/Bottom.png", SF2D_PLACE_RAM);
	sf2d_texture *t_top = sfil_load_PNG_file("romfs:/Top.png", SF2D_PLACE_RAM);
	sf2d_texture *t_modes = sfil_load_PNG_file("romfs:/Modes.png", SF2D_PLACE_RAM);
	sf2d_texture *t_numbers = sfil_load_PNG_file("romfs:/Numbers.png", SF2D_PLACE_RAM);
	sf2d_texture *t_blackdisk = sfil_load_PNG_file("romfs:/DiskBlack.png", SF2D_PLACE_RAM);
	sf2d_texture *t_whitedisk = sfil_load_PNG_file("romfs:/DiskWhite.png", SF2D_PLACE_RAM);
	sf2d_texture *t_poss_space = sfil_load_PNG_file("romfs:/PossSpace.png", SF2D_PLACE_RAM);
	sf2d_texture *t_gameover = sfil_load_PNG_file("romfs:/GameOver.png", SF2D_PLACE_RAM);
	sf2d_texture *t_difficulty = sfil_load_PNG_file("romfs:/Difficulty.png", SF2D_PLACE_RAM);
	sf2d_texture *t_menu = sfil_load_PNG_file("romfs:/MenuButtons.png", SF2D_PLACE_RAM);
	sf2d_texture *t_turn = sfil_load_PNG_file("romfs:/Turn.png", SF2D_PLACE_RAM);

	int x,y;
	while (aptMainLoop()){
		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		//u32 kHeld = hidKeysHeld();
		if (menu == 1){
			if (menstate == 0){
				if (touchInBox(touch, 41, 73, 130, 48)){
					resetGame();
					clearScaleBoard();
					mode=1;
					menu=0;
					action=1;
					turn=0;
				}
				if (touchInBox(touch, 41, 124, 130,48))
					menstate=1;
			}else{
				if (touchInBox(touch, 41, 77, 130, 30)){
					resetGame();
					clearScaleBoard();
					cpuDifficulty=0;
					mode=2;
					menu=0;
					action=1;
				}
				if (touchInBox(touch, 41, 109, 130, 30)){
					resetGame();
					clearScaleBoard();
					cpuDifficulty=1;
					mode=2;
					menu=0;
					action=1;
				}
				if (touchInBox(touch, 41, 141, 130, 30)){
					resetGame();
					clearScaleBoard();
					cpuDifficulty=2;
					mode=2;
					menu=0;
					action=1;
				}
				if (touchInBox(touch, 42, 68, 26, 7)){
					menstate=0;
				}
			}
			if (touchInBox(touch, 165, 67, 7, 7)){
				menu=0;
			}
		}else if (mode == 1 && !GameOver){//Local 2-Player
			if (action == 1){
				getTouchBoardXY(touch);
				if (input[0] != -1){
					if (placeTile(input[0], input[1], turn+1)){
						if (!endTurn(turn+1)){
							turn=-turn+1;
							action=3;
						}
					}
				}
			}
		}else if (mode == 2 && !GameOver){//Agains CPU
			if (action == 1){
				getTouchBoardXY(touch);
				if (input[0] != -1){
					cloneField();
					clearScaleBoard();
					if (placeTile(input[0], input[1], 1)){
						if (!endTurn(1)){
							action=4;
						}else{
							action=3;
						}
					}
				}
			}else if (action == 2){
				cpuTurn(2);
				if (!endTurn(2)){
					action=3;
				}else{
					action=4;
				}
			}
		}else if (!GameOver){
			if (action == 1){
				cpuTurn(turn+1);
				turn=-turn+1;
				action=3;
			}
		}
		if (kDown & KEY_START)
			break;
		if (touchInBox(touch, 239, 216, 81, 24)){
			menu=1;
			menstate=0;
		}
		if (touchInBox(touch, 239, 0, 81, 23)){
			resetGame();
			clearScaleBoard();
			action=1;
			turn=0;
		}
		//render();
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture(t_top, 0, 0);
			int x2,b,w,b1,b2,w1,w2;
			b=getScore(2);
			w=getScore(1);
			for (x=0; x<=turns; x++){
				y=floor(x/2);
				x2=x-y*2;
				if (y > 0){
					int q,w,e,r;
					q=round(220-scores[y-1][x2]*1.796);
					w=16+(y-1)*7.866;
					e=round(220-scores[y][x2]*1.796);
					r=16+y*7.866;
					if (x2 == 0)
						drawLine(w,q,r,e,RGBA8(0x00, 0x00, 0x00, 0xFF));
					if (x2 == 1)
						drawLine(w,q,r,e,RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
				}
			}
			if (mode >= 1 && mode <= 3)
				sf2d_draw_texture_part(t_modes, 265, 212, 135*(mode-1), 0, 135, 28);
			if (mode == 2)
				sf2d_draw_texture_part(t_difficulty, 292, 182, 108*cpuDifficulty, 0, 108, 30);
			if (GameOver)
				sf2d_draw_texture_part(t_gameover, 110, 10, 180*win, 0, 180,70);
		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(t_bottom, 0, 0);
			for (x=0; x<8; x++){
				for (y=0; y<8; y++){
					if ((action == 1 && mode == 2) || (mode == 1 && action <=2)){
						if (poss_board[x][y] == 1)
							sf2d_draw_texture(t_poss_space, 19+28*x, 19+28*y);
					}
					scale=scale_board[x][y];
					if (scale < 1){
						scale_board[x][y]+=0.05;
						animateTime=scale_board[x][y];
					}else if (action > 2 && animateTime >= 1){
						action-=2;
						clearScaleBoard();
					}
					scale=1.5-scale/2;
					if (board[x][y] == 1){
						sf2d_draw_texture_scale(t_whitedisk, 9+28*x+(14-scale*14), 9+28*y+(14-scale*14), scale, scale);
					}else if (board[x][y] == 2){
						sf2d_draw_texture_scale(t_blackdisk, 9+28*x+(14-scale*14), 9+28*y+(14-scale*14), scale ,scale);
					}
				}
			}
			if (mode == 1){
				sf2d_draw_texture(t_turn, 237, 35);
				if (turn == 1){
					sf2d_draw_texture(t_blackdisk, 265, 77);
				}else{
					sf2d_draw_texture(t_whitedisk, 265, 77);
				}
			}
			b=getScore(2);
			w=getScore(1);
			b1=floor(b/10);
			b2=b-b1*10;
			w1=floor(w/10);
			w2=w-w1*10;
			if (b1 != 0)
				sf2d_draw_texture_part(t_numbers, 297, 172, 10*b1, 0, 10, 14);
			sf2d_draw_texture_part(t_numbers, 307, 172, 10*b2, 0, 10, 14);
			if (w1 != 0)
				sf2d_draw_texture_part(t_numbers, 297, 196, 10*w1, 0, 10, 14);
			sf2d_draw_texture_part(t_numbers, 307, 196, 10*w2, 0, 10, 14);

			if (menu != 0){
				sf2d_draw_texture_part(t_menu, 36, 64, 0, 0, 142, 114);
				if (menu == 1){
					if (menstate == 0){
						sf2d_draw_texture_part(t_menu, 41, 73, 0, 114, 130, 48);
						sf2d_draw_texture_part(t_menu, 41, 124, 0, 162, 130, 48);
					}else{
						sf2d_draw_texture_part(t_menu, 41, 77, 0, 210, 130, 94);
						sf2d_draw_texture_part(t_menu, 42, 67, 0, 304, 26, 7);
					}
				}
			}
		sf2d_end_frame();

		sf2d_swapbuffers();
	}
	sf2d_free_texture(t_bottom);
	sf2d_free_texture(t_top);
	sf2d_free_texture(t_whitedisk);
	sf2d_free_texture(t_blackdisk);
	sf2d_free_texture(t_poss_space);
	sf2d_free_texture(t_gameover);
	sf2d_free_texture(t_difficulty);
	sf2d_free_texture(t_modes);
	sf2d_free_texture(t_numbers);
	sf2d_free_texture(t_menu);
	sf2d_free_texture(t_turn);

	sf2d_fini();

	romfsExit();
	return 0;
}