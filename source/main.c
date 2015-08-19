#include <3ds.h>
#include <sf2d.h> //Yeah to hardware rendering!
#include <stdlib.h>
#include <math.h>
#include "touch.h"
#include "textures.h"

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

void fillArea(int x1, int y1, int x2, int y2, int col){
	int x,y,z;
	if (y1 == y2 || x1 == x2){
		for (x=x1; x<=x2;x++){
			for (y=y1; y<=y2;y++){
				board[x][y]=col;
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

bool updatePossBoard(int col){ //Updates pos board for given col, returns if any move is even possible
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
	return ret;
}

void cpuTurn(int col){
	//canPlay(col);
	u64 timeInSeconds = osGetTime() / 1000;
	srand (timeInSeconds);
	int baseScore=getScore(col);
	int bestImprovement=-9000; //Makes negative moves posible to avoid cpu not making a turn due to bad outcome, as the CPU has to make a turn if it can/
	int tryImprovement;
	int h_col=col-2*col+3;
	int h_bestImprovement=-1000;
	int h_tryImprovement=0;
	int h_baseScore=getScore(h_col);
	int movex=0,movey=0;
	int corner_reward=40;
	if (cpuDifficulty == 2){
		corner_reward=10000; //Evens out if the player can get a corner due to this move;
	}
	int x,y,j,k,m;
	int hp_board[8][8]; //Human players next board with current cpu turn? IDK how to describe it!
	cloneField();
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			for (j=0; j<8; j++){
				for (k=0; k<8; k++){
					board[j][k]=temp_board[j][k];
				}
			}
			if (placeTile(x,y,col)){
				tryImprovement=getScore(col)-baseScore+((rand() % 3)-1);
				if (cpuDifficulty > 0){
					for (j=0; j<8; j++){
						for (k=0; k<8; k++){
							hp_board[j][k]=board[j][k];
						}
					}
				}
				if (cpuDifficulty == 1){
					for (m=0; m<4; m++){
						for (j=0; j<8; j++){
							for (k=0; k<8; k++){
								board[j][k]=hp_board[j][k];
							}
						}
						switch(m){
							case 0:
								if (placeTile(0,0, h_col))
									tryImprovement-=15;
								break;
							case 1:
								if (placeTile(0,7, h_col))
									tryImprovement-=15;
								break;
							case 2:
								if (placeTile(7,0, h_col))
									tryImprovement-=15;
								break;
							case 3:
								if (placeTile(7,7, h_col))
									tryImprovement-=15;
								break;
						}
					}
				}else if (cpuDifficulty == 2){
					int q,w,e,r;
					h_bestImprovement=-1000;
					for (q=0; q<8; q++){
						for (w=0; w<8; w++){
							for (e=0; e<8; e++){
								for (r=0; r<8; r++){
									board[e][r]=hp_board[e][r];
								}
							}
							if (placeTile(q,w,h_col)){
								h_tryImprovement=getScore(h_col)-h_baseScore+((rand() % 3)-1);
							}
							if ((q == 0 && w == 0) || (q == 7 && w == 0) || (q == 7 && w == 7) || (q == 0 && w == 7))
								h_tryImprovement+=500; //If CPU can avoid giving you a corner it will!
							if (h_tryImprovement > h_bestImprovement){
								h_bestImprovement=h_tryImprovement;
							}
						}
					}
					tryImprovement-=h_bestImprovement;
				}
				if ((x == 0 && y == 0) || (x == 7 && y == 0) || (x == 7 && y == 7) || (x == 0 && y == 7))
					tryImprovement+=corner_reward;
				if (tryImprovement > bestImprovement){
					bestImprovement=tryImprovement;
					movex=x;
					movey=y;
				}
			}
		}
	}
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			board[x][y]=temp_board[x][y];
		}
	}
	if (bestImprovement != 0){
		placeTile(movex,movey,col);
		animation();
		if (endTurn(col)){
			cpuTurn(col);
		}
	}
}

void resetGame(){
	int x,y;
	for (x=0;x<8;x++){
		for (y=0;y<8;y++){
			board[x][y]=0;
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
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	sf2d_texture *t_bottom = sf2d_create_texture_mem_RGBA8(tex_bottom.pixel_data, tex_bottom.width, tex_bottom.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_top = sf2d_create_texture_mem_RGBA8(tex_top.pixel_data, tex_top.width, tex_top.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_modes = sf2d_create_texture_mem_RGBA8(tex_modes.pixel_data, tex_modes.width, tex_modes.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_numbers = sf2d_create_texture_mem_RGBA8(tex_numbers.pixel_data, tex_numbers.width, tex_numbers.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_blackdisk = sf2d_create_texture_mem_RGBA8(tex_blackdisk.pixel_data, tex_blackdisk.width, tex_blackdisk.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_whitedisk = sf2d_create_texture_mem_RGBA8(tex_whitedisk.pixel_data, tex_whitedisk.width, tex_whitedisk.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_poss_space = sf2d_create_texture_mem_RGBA8(tex_poss_space.pixel_data, tex_poss_space.width, tex_poss_space.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_gameover = sf2d_create_texture_mem_RGBA8(tex_gameover.pixel_data, tex_gameover.width, tex_gameover.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *t_difficulty = sf2d_create_texture_mem_RGBA8(tex_difficulty.pixel_data, tex_difficulty.width, tex_difficulty.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);

	int b1,b2,w1,w2,b,w,x,y,lal;
	float scale;
	while (aptMainLoop()){
		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		//u32 kHeld = hidKeysHeld();
		if (mode == 0){
			mode=2;//Add menu
			resetGame();
			updatePossBoard(1);
		}else if (mode == 1 && !GameOver){//Local 2-Player
			getTouchBoardXY(touch);
			if (input[0] != -1){
				if (placeTile(input[0], input[1], turn+1)){
					if (!endTurn(turn+1))
						turn=-turn+1;
				}
			}
		}else if (mode == 2 && !GameOver){//Agains CPU
			getTouchBoardXY(touch);
			if (input[0] != -1){
				cloneField();
				if (placeTile(input[0], input[1], 1)){
					animation();
					endTurn(1);
					cpuTurn(2);
				}
			}
		}else if (!GameOver){
			cpuTurn(turn+1);
			turn=-turn+1;
		}
		if (kDown & KEY_START)
			break;
		if (kDown & KEY_UP){
			mode=1;
			resetGame();
		}
		if (kDown & KEY_DOWN){
			mode=2;
			resetGame();
		}
		if (kDown & KEY_A){
			mode=3;
		}
		if (kDown & KEY_B){
			resetGame();
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
					if (board[x][y] == 1){
						sf2d_draw_texture(t_whitedisk, 9+28*x, 9+28*y);
					}else if (board[x][y] == 2){
						sf2d_draw_texture(t_blackdisk, 9+28*x, 9+28*y);
					}
					if (poss_board[x][y] == 1)
						sf2d_draw_texture(t_poss_space, 19+28*x, 19+28*y);
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

	sf2d_fini();
	return 0;
}