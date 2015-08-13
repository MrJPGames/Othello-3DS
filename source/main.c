#include <3ds.h>
#include <stdlib.h>
#include <math.h>
#include "touch.h"
#include "gfx.h"

#include "bottom_bin.h"
#include "bottom_disks_bin.h"

//Init vars
int board[8][8];
int temp_board[8][8];
// ^          Board starts bottom left ala 3DS (software) rendering
// |
// |
// 0,0 --->
int turn=0; //Who's turn is it?
int score[2]; //Score for each player/CPU
int mode=0; //0: menu 1: 2-Player 2: agains CPU
int input[2];

void render(){
	gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)bottom_bin, 240, 320, 0, 0);
	int x;
	int y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			if (board[x][y] != 0)
				gfxDrawSpriteAlphaBlend(GFX_BOTTOM, GFX_LEFT, (u8*)bottom_disks_bin+2704*(board[x][y]-1), 26, 26, 9+28*y, 49+28*x);
		}
	}
	gfxDrawSpriteAlphaBlend(GFX_TOP, GFX_LEFT, (u8*)bottom_disks_bin+2704*turn, 26, 26, 0, 0);
}

void animation(){
	int fadeval=0;
	int x;
	int y;
	int z;
	for (z=0; z<=30; z++){
		fadeval=round(8.5*z);
		gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)bottom_bin, 240, 320, 0, 0);
		for (x=0; x<8; x++){
			for (y=0; y<8; y++){
				if (board[x][y] != 0 && board[x][y] <= 2)
					gfxDrawSpriteAlphaBlend(GFX_BOTTOM, GFX_LEFT, (u8*)bottom_disks_bin+2704*(board[x][y]-1), 26, 26, 9+28*y, 49+28*x);
				else if (board[x][y] > 2)
					gfxDrawSpriteBlend(GFX_BOTTOM, GFX_LEFT, (u8*)bottom_disks_bin+2704*(board[x][y]-3), 26, 26, 9+28*y, 49+28*x,fadeval);
			}
		}
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForEvent(GSPEVENT_VBlank0, false);
	}
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			if (board[x][y] > 2)
				board[x][y]-=2;
		}
	}
}

void fillArea(int x1, int y1, int x2, int y2, int col){
	int x;
	int y;
	int z;
	if (y1 == y2 || x1 == x2){
		for (x=x1; x<=x2;x++){
			for (y=y1; y<=y2;y++){
				board[x][y]=col+2;
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
			board[x+z*dirx][y+z*diry]=col+2;
		}
	}
}

bool placeTile(int x, int y, int col){
	//Col color to place
	//x pos of placement
	//y pos of placement
	int tx;
	int ty;
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
	int x;
	int y;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			if (board[x][y] == col || board[x][y] == col+2){
				ret+=1;
			}
		}
	}
	return ret;
}

void cpuTurn(int col){
	//canPlay(col);
	int baseScore=getScore(col);
	int bestImprovement=0;
	int tryImprovement;
	int movex;
	int movey;
	int x;
	int y;
	int j;
	int k;
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			temp_board[x][y]=board[x][y];
		}
	}
	for (x=0; x<8; x++){
		for (y=0; y<8; y++){
			for (j=0; j<8; j++){
				for (k=0; k<8; k++){
					board[j][k]=temp_board[j][k];
				}
			}
			if (placeTile(x,y,col)){
				tryImprovement=getScore(col)-baseScore;
				if ((x == 0 && y == 0) || (x == 8 && y == 0) || (x == 8 && y == 8) || (x == 0 && y == 8))
					tryImprovement+=15;
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
	}
}

void resetGame(){
	int x;
	for (x=0;x<8;x++){
		fillArea(x,0,x,7,-2);
	}
	board[3][3]=1;
	board[4][4]=1;
	board[3][4]=2;
	board[4][3]=2;
}

touchPosition touch;

void getTouchBoardXY(touchPosition touch){
	input[0]=-1;
	int tx=touch.px-48; //Board pos on touch screen
	int ty=touch.py-8;
	u32 kUp = hidKeysDown();
	if (kUp & KEY_TOUCH){
		input[0]=floor(tx/28);
		input[1]=7-floor(ty/28);
		if (input[0] > 7 || input[1] > 7 || input[0] < 0 || input[1] < 0){
			input[0]=-1;
		}
	}
}


int main(int argc, char * argv[])
{
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInitDefault();
	while (aptMainLoop()){
		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		//u32 kHeld = hidKeysHeld();
		if (mode == 0){
			mode=2;//Add menu
			resetGame();
		}else if (mode == 1){//Local 2-Player
			getTouchBoardXY(touch);
			if (input[0] != -1){
				if (placeTile(input[0], input[1], turn+1)){
					turn=-turn+1; //Flip value
					animation();
				}
			}
		}else{//Agains CPU
			getTouchBoardXY(touch);
			if (input[0] != -1){
				if (placeTile(input[0], input[1], 1)){
					animation();
					cpuTurn(2);
					animation();
				}
			}
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
		render();
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForEvent(GSPEVENT_VBlank0, false);
	}
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}