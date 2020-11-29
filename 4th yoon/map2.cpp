#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GL/glut.h>
#include <unistd.h>
#include <math.h>
#include <jpeglib.h>

using namespace std;
#define FULL 10001
#define WIDTH 1000
#define HEIGHT 1000
#define MOVE 1
#define QUITE 2
#define GHOST 666
#define PACMAN 999
#define PI 3.1416
typedef struct n{
	int x;
	int y;
} q;

q queue[FULL];
int front, rear, max;
int row, col;
int i,j;
int** map;
int f_cnt=0;
int f_left;
int w_cnt=0;
int enemy_cnt;
int unit = 30;
int anglealpha = 0;
int anglebeta = 0;
////up left down rigt{{{
int vecX[4] = {-1,0,1,0};
int vecY[4] = {0,-1,0,1};

int keyflag = 0;

/* 
   7 0 1
   6   2
   5 4 3 */

int checkX[8] ={-1,-1,0,1,1,1,0,-1};
int checkY[8] ={0,1,1,1,0,-1,-1,-1};
/*}}}*/

bool G_collision (int nx, int ny);
bool collision (int nx, int ny);
void PositionObserver(float alpha,float beta,int radi);
void LoadTexture(char *filename,int dim);
void ReadJPEG(char *filename,unsigned char **image,int *width, int *height);

class Feed{/*{{{*/
	private:
		int r,c;
		int state = MOVE;
	public :
		Feed(){
		}
		Feed(int r, int c){
			this->r = r;
			this->c = c;
			this->state = MOVE;
		}
		void init(int r, int c){
			this->r = r;
			this->c = c;
		}
		void draw(){
			if(this->state == MOVE){
			glColor3f(0.8,0.8,0);
			glBegin(GL_QUADS);
			glVertex3i((c+0.6)*unit-unit*col/2,10,((r+0.3)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,10,((r+0.3)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,5,((r+0.3)*unit)-unit*row/2); 
			glVertex3i((c+0.6)*unit-unit*col/2,5,((r+0.3)*unit)-unit*row/2); 		
			glEnd();
	
			glColor3f(0.8,0.8,0);
			glBegin(GL_QUADS);
			glVertex3i((c+0.6)*unit-unit*col/2,5,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,5,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,10,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.6)*unit-unit*col/2,10,((r+0.6)*unit)-unit*row/2); 		
			glEnd();
	
			glColor3f(0.8,0.8,0);
			glBegin(GL_QUADS);
			glVertex3i((c+0.6)*unit-unit*col/2,5,((r+0.3)*unit)-unit*row/2); 
			glVertex3i((c+0.6)*unit-unit*col/2,5,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.6)*unit-unit*col/2,10,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.6)*unit-unit*col/2,10,((r+0.3)*unit)-unit*row/2); 		
			glEnd();

			glColor3f(0.8,0.8,0);
			glBegin(GL_QUADS);
			glVertex3i((c+0.3)*unit-unit*col/2,10,((r+0.3)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,10,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,5,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,5,((r+0.3)*unit)-unit*row/2); 		
			glEnd();
		

			glBegin(GL_QUADS);
			 glVertex3i((c+0.6)*unit-unit*col/2,10,((r+0.3)*unit)-unit*row/2); 
			 glVertex3i((c+0.6)*unit-unit*col/2,10,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,10,((r+0.6)*unit)-unit*row/2); 
			glVertex3i((c+0.3)*unit-unit*col/2,10,((r+0.3)*unit)-unit*row/2); 		
			glEnd();



			}
		}
		void setState(int state){
			if(this->state == MOVE){
				this->state = state;
				f_left--;
			}
		}
		q getinfo(){
			q temp;
			temp.x = r;
			temp.y = c;
			return temp;
		}

};/*}}}*/
class Ghost{/*{{{*/
	private:
		double x,y;
		int vx, vy;
		int identity = GHOST;
		int ghost_id;
		GLUquadric *ghost = gluNewQuadric();
		long time_remaining;
	public :
		Ghost(){
			this->x = 0;
			this->y = 0;
		}
		void position(double x, double y){
			this->x = x;
			this->y = y;
		}
		void movement(q pac,int duration){
			double nextx, nexty;
			double dx = 0;
			double dy = 0;
			dx = pac.x - x;
			dy = pac.y - y;
			srand(time(NULL));
	
			for(int i=0; i<4; i++){
				if(dx>=0 && dy>=0){
					if(rand()%2 ==1){
						vx=1; vy=0;
					}
					else{
						vx=0; vy=1;
					}
				}
				else if(dx>=0 && dy<0){
					if(rand()%2 ==1){
						vx=1; vy=0;}
					else{
						vx=0; vy=-1;
					}
				}
				else if(dx<0 && dy>=0){
					if(rand()%2 ==1){
						vx=-1; vy=0;}
					else{
						vx=0; vy=1;
					}
				}
				else if(dx<0 && dy<0){
					if(rand()%2 ==1){
						vx=-1; vy=0;}
					else{
						vx=0; vy=-1;
					}
				}
			}
			//printf("vx : %dd vy : %d\n", vx,vy);
			time_remaining = duration;
		}
		void integrate(long t){
			double nx, ny;//double nx, ny;
			if(t<time_remaining){
				nx = x+ (double)vx*(t*0.8);
				ny = y+ (double)vy*t*0.8;
				if(G_collision(nx,ny)==false){
					x = nx;
					y = ny;
				}

				time_remaining-=t;
			}
			else if(t>=time_remaining){
				nx = x+ vx*(time_remaining*0.8);
				ny = y+ vy*time_remaining*0.8;
				
				if(G_collision(nx,ny)==false){
					x = nx;
					y = ny;
				}
			}

		}
		void draw(double r, double g, double b){
			glColor3f(0.7f,0.5f,0.7f);
			//glTranslatef(0.0f,10,0.0f);
			gluSphere(ghost, 20, 10, 10);
			/*
			glColor3f(r,g,b);
			glBegin(GL_QUADS);
			glVertex3i(x-WIDTH/row*0.2, y-HEIGHT/col*0.3,100);
			glVertex3i(x+WIDTH/row*0.2, y-HEIGHT/col*0.3,100);
			glVertex3i(x+WIDTH/row*0.2, y+HEIGHT/col*0.3,100);
			glVertex3i(x-WIDTH/row*0.2, y+HEIGHT/col*0.3,100);
			glEnd();*/
		}
		int getIdentity(){
			return identity;
		}
};/*}}}*/
class Pacman{/*{{{*/
	private:
		double x,y;
		double vx, vy;
		int state;
		int identity = PACMAN;
		long time_remaining;
		GLUquadric *man = gluNewQuadric();
			
	public :
		Pacman(){
			this->x = 0;
			this->y = unit*(row/2-2);
			this->state = QUITE;
		}
		void position(int x, int y){
			this->x = x;
			this->y = y;
		}
		void movement(int vx, int vy, int duration){
			this->vx = vx;
			this->vy = vy;
			time_remaining = duration;
			
			state=MOVE;
		}
		void integrate(long t){
			double nx, ny;//double nx, ny;
			if(state == MOVE && t<time_remaining){
				nx = x+ vx*t;
				ny = y+ vy*t;
				if(collision(nx,ny)==false){
					x = nx;
					y = ny;
				}

				time_remaining-=t;
			}
			else if(state==MOVE && t>=time_remaining){
				nx = x+ vx*time_remaining;
				ny = y+ vy*time_remaining;
				
				if(collision(nx,ny)==false){
					x = nx;
					y = ny;
				}
				state=QUITE;
			}

		}
		void draw(double r, double g, double b){
			glColor3f(0.7f,0.4f,0.7f);
			glTranslatef(x*unit, 0.0f,0.0f);
			gluSphere(man, 20, 10, 10);
			glLoadIdentity();
			/*	glBegin(GL_QUADS);
			glVertex3i(x-WIDTH/row*0.2, y-HEIGHT/col*0.3,100);
			glVertex3i(x+WIDTH/row*0.2, y-HEIGHT/col*0.3,100);
			glVertex3i(x+WIDTH/row*0.2, y+HEIGHT/col*0.3,100);
			glVertex3i(x-WIDTH/row*0.2, y+HEIGHT/col*0.3,100);
			glEnd();*/
		}
		int getIdentity(){
			return identity;
		}
		q getPosition(){
			q temp;
			temp.x = x;
			temp.y = y;
			return temp;
		}
};/*}}}*/
class Wall{/*{{{*/
	private : 
		int r, c;
	public :
		Wall(){
		}
		Wall(int r,int c){
			this->r = r;
			this->c = c;
		}
		void init(int r, int c){
			this->r = r;
			this->c = c;
		}
		void draw(){
					glColor3f(0.2,0.2,0.2);
			glBegin(GL_QUADS);
		glVertex3i((c+1)*unit-unit*col/2,10,((r)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,10,((r)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,-unit,((r)*unit)-unit*row/2); 
			glVertex3i((c+1)*unit-unit*col/2,-unit,((r)*unit)-unit*row/2); 		
			glEnd();
	
			glColor3f(0.2,0.2,0.2);
			glBegin(GL_QUADS);
			glVertex3i((c+1)*unit-unit*col/2,-unit,((r+1)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,-unit,((r+1)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,10,((r+1)*unit)-unit*row/2); 
			glVertex3i((c+1)*unit-unit*col/2,10,((r+1)*unit)-unit*row/2); 		
			glEnd();
	
		
		//	glColor3f(0.2,0.2,0.2);
			glBegin(GL_QUADS);
			glVertex3i((c+1)*unit-unit*col/2,-unit,((r)*unit)-unit*row/2); 
			glVertex3i((c+1)*unit-unit*col/2,-unit,((r+1)*unit)-unit*row/2); 
			glVertex3i((c+1)*unit-unit*col/2,10,((r+1)*unit)-unit*row/2); 
			glVertex3i((c+1)*unit-unit*col/2,10,((r)*unit)-unit*row/2); 		
			glEnd();

			glColor3f(0.2,0.2,0.2);
			glBegin(GL_QUADS);
			glVertex3i((c)*unit-unit*col/2,10,((r)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,10,((r+1)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,-unit,((r+1)*unit)-unit*row/2); 
			glVertex3i((c)*unit-unit*col/2,-unit,((r)*unit)-unit*row/2); 		
			glEnd();
		
			glBindTexture(GL_TEXTURE_2D,1);
			glBegin(GL_QUADS);
			glTexCoord2f(-4.0,0.0);  glVertex3i((c+1)*unit-unit*col/2,10,((r)*unit)-unit*row/2); 
			glTexCoord2f(4.0,0.0); glVertex3i((c+1)*unit-unit*col/2,10,((r+1)*unit)-unit*row/2); 
			glTexCoord2f(4.0,4.0); glVertex3i((c)*unit-unit*col/2,10,((r+1)*unit)-unit*row/2); 
			glTexCoord2f(-4.0,4.0); glVertex3i((c)*unit-unit*col/2,10,((r)*unit)-unit*row/2); 		
			glEnd();


		}
};/*}}}*/

void display();
void ckeyboard(unsigned char c,int x,int y);
void keyboard(int key,int x,int y);
void idle();

void enqueue(int x, int y){/*{{{*/
	q temp;
	temp.x =x; temp.y = y;
	queue[rear++] = temp;
}

q deque(){
	q temp = queue[front++];
	return temp;
}

int isEmpty(){
	if(front == rear)
		return 0;
	else return 1;
}/*}}}*/


Wall wall[FULL];
Feed feed[FULL];
Pacman player; 
Ghost ghost[10];
long last_t = 0;

bool G_collision (int nx, int ny){/*{{{*/
	//nx , ny가 갈 곳에 해당하는 r, c를 찾음
	//map[r][c] 가 9인지 확인

	int idx[4] = {-1,1,-1,1};
	int idy[4] = {-1,-1,1,1};
	double dx = WIDTH/row*0.2;
	double dy = HEIGHT/col*0.3;
	double wx = WIDTH/row; 
	double wy = WIDTH/row;
	int c , r;	
	//formula X : c < (nx+idx[i]*dx[i])*col/WIDTH < c+1
	//formula Y : r < (HEIGHT-(ny+idy[i]*dy[i]))*row/HEIGHT < r+1

	for (int i=0; i<4; i++){
		c = floor(((nx+idx[i]*dx)*col/WIDTH));
		r = floor((HEIGHT-(ny+idy[i]*dy))*row/HEIGHT);
		if (map[r][c]==9 /*&& ((nx>c*wx)&&(nx<(c+1)*wx))&&((ny>r*wy)&&(ny<(r+1)*wy))*/){
			return true;
		}
		/* pacman이랑마주쳤을때 처리부분
		   q temp = feed[j].getinfo();
		if(temp.x== r && temp.y ==c)
				feed[j].setState(QUITE);
			}
		 */
	}
	return false;
}/*}}}*/
bool collision (int nx, int ny){/*{{{*/
	//nx , ny가 갈 곳에 해당하는 r, c를 찾음
	//map[r][c] 가 9인지 확인

	int idx[4] = {-1,1,-1,1};
	int idy[4] = {-1,-1,1,1};
	double dx = WIDTH/row*0.2;
	double dy = HEIGHT/col*0.3;
	double wx = WIDTH/row; 
	double wy = WIDTH/row;
	int c , r;	
	//formula X : c < (nx+idx[i]*dx[i])*col/WIDTH < c+1
	//formula Y : r < (HEIGHT-(ny+idy[i]*dy[i]))*row/HEIGHT < r+1

	for (int i=0; i<4; i++){
		c = floor(((nx+idx[i]*dx)*col/WIDTH));
		r = floor((HEIGHT-(ny+idy[i]*dy))*row/HEIGHT);
		if (map[r][c]==9 /*&& ((nx>c*wx)&&(nx<(c+1)*wx))&&((ny>r*wy)&&(ny<(r+1)*wy))*/){
			return true;
		}
		for(j=0; j<f_cnt; j++){
			q temp = feed[j].getinfo();
			if(temp.x== r && temp.y ==c)
				feed[j].setState(QUITE);
			}
	}
	return false;
}/*}}}*/
//---------map generation-------------------------------
//------------------------------------------------------
//return 0: plaza X   return 1 : plaza O
int plaza_check(int nextX, int nextY){/*{{{*/
	for(int i=0; i<8; i=i+2){
		if (map[nextX+checkX[i]][nextY+checkY[i]]==1 && map[nextX+checkX[i+1]][nextY+checkY[i+1]]==1&&map[nextX+checkX[(i+2)%8]][nextY+checkY[(i+2)%8]]==1)
			return 1;
	}
	return 0;
}


//return 0: block x   return 1 : block o
int block_check(int nextx, int nexty){
	for(int i=1; i<8; i=i+2){
		if(map[nextx+checkX[i]][nexty+checkY[i]]==9&& map[nextx+checkX[(i+2)%8]][nexty+checkY[(i+2)%8]]==9){
			return 1;
		}
	}

	return 0;
}

//return 0: blockX return 1 : blockO
int block (int curX, int curY){
	int cnt =2;
	//printf("avail : %d\n", cnt);

	for(int i=0; i<8;i=i+2){
		if(map[curX+checkX[i]][curY+checkY[i]]==9)
			cnt--;
	}
	if(cnt <= 0 ) 
		return 1;
	else
		return 0;

}/*}}}*/
void bfs(){/*{{{*/
	int nextX, nextY;
	while(isEmpty()){
		q pop=deque();
		for (i = 0; i<4 ;i++){
			nextX = pop.x + vecX[i];
			nextY = pop.y + vecY[i];
			if(nextX>=1 && nextX<=row && nextY>=1 && nextY<=col/2 ) //next point is inside the map
				if(map[nextX][nextY]==0)//non visited and not border
					if (plaza_check(nextX,nextY)!=1){
						enqueue(nextX,nextY);
						map[nextX][nextY]=1;

					} 
					else
						map[nextX][nextY] =9;
		}			 
	}



}/*}}}*/
void map_gen(){/*{{{*/
	int nextX, nextY;
	srand(time(NULL));
	while(isEmpty()){
		q pop=deque();
		//printf("deque %d %d\n", pop.x, pop.y);
		for (i = 0; i<4 ;i++){
			nextX = pop.x + vecX[i];
			nextY = pop.y + vecY[i];
			if(nextX>=1 && nextX<=row && nextY>=1 && nextY<=col/2 ){ //next point is inside the map
				if(map[nextX][nextY]==0){	//non visited and not border
					//최소점 찾기 : plaza check	
					if(plaza_check(nextX, nextY) ==1)
						///wall - neccesary
						map[nextX][nextY]=9;	
					else{
						//corridor-neccesary
						//현재 이미 2개가 막혀있으면 나머지는 무조건 복도로 뚫어야됨
						if(block(pop.x, pop.y)==1||block_check(nextX,nextY)==1 ){
							map[nextX][nextY]=1;
							enqueue(nextX,nextY);
						}
						//random part
						else {
							if(rand()%2==1){
								map[nextX][nextY]=1;
								enqueue(nextX,nextY);
							}
							else
								map[nextX][nextY]=9;
						}
					}
				}
			}
		}			 
	}


	for (int i=row/2-1; i<row/2+2; i++){
		for (int j=col/2-2; j<col/2+3; j++){
			map[i][j] =2;
		}
	}
	map[row/2-2][col/2]=1;
}/*}}}*/
void init_map(){/*{{{*/
	int i, j, mid;
	int x,y;

	x = row/2;
	y = col/2;

	//border : edge + monster box
	for (i=0; i<row; i++){
		for (j=0; j<col; j++){
			if(i==0 || i==row-1||j==0||j==col/2+1)
				map[i][j] = 9;
			if(i>x-3 && i<x+3 && j>y-4 &&j<y+4)
				map[i][j] =9;

		}
	}

	//start point enqueue
	enqueue(x-3,y);
	map[x-3][y] =1;
}
/*}}}*/
void print_map(){/*{{{*/
	int i,j;
	for(i=0; i<row; i++){
		for(j=0; j<col; j++){
			if (map[i][j] == 1)
				printf("  ");
			else if (map[i][j] == 9)
				printf("# ");
			else printf("%d ",map[i][j]);

			//printf("%d ", map[i][j]);
		}
		printf("\n");
	}
}


void show_map(){
	int i,j, mid;
	//mirroring
	mid = (int)(col/2)+1;
	for (i=0; i<row ; i++){
		for (j=mid; j<col; j++){
			map[i][j] = map[i][col-(j+1)]; 	
		}
	}
	//print
	print_map();
}/*}}}*/

//-----------------------------------------------
int main(int argc, char* argv[]){/*{{{*/
	row = atoi(argv[1]);
	col = atoi(argv[2]);
	//check arguments{{{i
	if (col%2==0 || (row<13 &&col<13)) {
		printf("row and col num need to be odd number bigger than 11 and 13");
		exit(0);
	}

	printf("Enter the numer of ghost (1~10)\n");
	scanf("%d",&enemy_cnt);
	//map initialize
	map = (int**)calloc(row,sizeof(int*));
	for (int i=0; i<row; i++){
		map[i] = (int*)calloc(col,sizeof(int));
	}

	init_map();	
	bfs();

	for (int i=0; i<row; i++){
		for(int j=0; j<col; j++){
			if (map[i][j]==1)
				map[i][j]=0;
		}	
	}
	enqueue(row/2-3,col/2);
	map[row/2-3][col/2] =1;

	map_gen();

	show_map();
	//print_map();}}}
	//feed = new Feed[f_cnt+1];
	for (int i=0; i<row; i++){
		for (int j=0; j<col; j++){
			if(map[i][j] == 9)
				wall[w_cnt++].init(i,j);
			if(map[i][j] == 1)
				feed[f_cnt++].init(i,j);
		}
	}
	f_left = f_cnt;
	for (int i=0; i<enemy_cnt; i++){
		ghost[i].position(WIDTH/2-WIDTH/col*2+2*2*i*WIDTH/col, HEIGHT/2);
	}
	player.position(0,0);
	
	anglealpha=90;
	anglebeta =30;
	//----------------------------------------------
	glutInit(&argc, argv);/*{{{*/
	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB|GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("packman");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glutDisplayFunc(display);
	glutKeyboardFunc(ckeyboard);
	glutSpecialFunc(keyboard);
	glutIdleFunc(idle);

	glBindTexture(GL_TEXTURE_2D,0);
	LoadTexture("grass.jpg",64);
	
	glBindTexture(GL_TEXTURE_2D,1);
	LoadTexture("pared.jpg",64);


	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,WIDTH-1,0,HEIGHT-1);
	/*}}}*/
	if(f_left == 0 ){
		printf(" you win\n");
	//	glutLeaveMainLoop();
		exit(0);
	}
	glutMainLoop();

	return 0;

}/*}}}*/
void PositionObserver(float alpha,float beta,int radi){/*{{{*/
  float x,y,z;
  float upx,upy,upz;
  float modul;

  x = (float)radi*cos(alpha*2*PI/360.0)*cos(beta*2*PI/360.0);
  y = (float)radi*sin(beta*2*PI/360.0);
  z = (float)radi*sin(alpha*2*PI/360.0)*cos(beta*2*PI/360.0);
  if (beta>0){
      upx=-x;
      upz=-z;
      upy=(x*x+z*z)/y;
    }
  else if(beta==0){
      upx=0;
      upy=1;
      upz=0;
    }
  else{
      upx=x;
      upz=z;
      upy=-(x*x+z*z)/y;
    }
  modul=sqrt(upx*upx+upy*upy+upz*upz);
  upx=upx/modul;
  upy=upy/modul;
  upz=upz/modul;
  gluLookAt(x,y,z,    0.0, 0.0, 0.0,     upx,upy,upz);
  //printf("%lf, %lf, %lf\n", upx,upy, upz);
}/*}}}*/
void display()
{
	int i,j;

	glClearColor(0.8,0.8,0.8,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	PositionObserver(anglealpha, anglebeta, 450);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WIDTH*0.6, WIDTH*0.6, -HEIGHT*0.6,HEIGHT*0.6,10,2000);

	glMatrixMode(GL_MODELVIEW);
	
	//corridor{{{
	glColor3f(0.1,0.5,0.6);
 
	/*
	glBindTexture(GL_TEXTURE_2D,0);
	glBegin(GL_QUADS);
  glTexCoord2f(-4.0,0.0); glVertex3i(-200,0,0);
  glTexCoord2f(4.0,0.0); glVertex3i(200,0,0);
  glTexCoord2f(4.0,4.0); glVertex3i(200,200,0);
  glTexCoord2f(-4.0,4.0); glVertex3i(-200,200,0);

	*/
	glBindTexture(GL_TEXTURE_2D,0);
	glBegin(GL_QUADS);
	glTexCoord2f(-4.0,0.0);	glVertex3i(-unit*col/2,-unit,unit*row/2); 
	glTexCoord2f(4.0,0.0); glVertex3i(-unit*col/2,-unit,-unit*row/2); 
	glTexCoord2f(4.0,4.0); glVertex3i(unit*col/2,-unit,-unit*row/2); 
	glTexCoord2f(-4.0,4.0); glVertex3i(unit*col/2,-unit,unit*row/2); 	

	glEnd();
/*}}}*/

	glPolygonMode(GL_FRONT,GL_FILL);
	//glPolygonMode(GL_BACK,GL_LINE);
	for(i=0; i<f_cnt; i++)
		feed[i].draw();	
	for(i=0; i<w_cnt; i++)
		wall[i].draw();
	player.draw(0.8,0.8,0.3);
	for(i=0; i<enemy_cnt; i++)
		ghost[i].draw(0.3,0.5,0.3);
		
	glutSwapBuffers();

}
void ckeyboard(unsigned char c,int x,int y)/*{{{*/
{
  int i,j;

  if (c=='i' && anglebeta<=(90-4))
    anglebeta=(anglebeta+3);
  else if (c=='k' && anglebeta>=(-90+4))
    anglebeta=anglebeta-3;
  else if (c=='j')
    anglealpha=(anglealpha+3)%360;
  else if (c=='l')
  glutPostRedisplay();
}/*}}}*/
void keyboard(int key,int x,int y){/*{{{*/

	switch(key){

		case GLUT_KEY_UP:
			//ghost.movement(player.getPosition(),5);
			player.movement(0,1,5);
			break;
		case GLUT_KEY_DOWN:
			//ghost.movement(player.getPosition(),5);			
			player.movement(0,-1,5);
			break;
		case GLUT_KEY_LEFT:
			//ghost.movement(player.getPosition(),5);
			player.movement(-1,0,5);
			break;
		case GLUT_KEY_RIGHT:
			//ghost.movement(player.getPosition(),5);
			player.movement(1,0,5);
			break;
	}
	glutPostRedisplay();

};/*}}}*/
void idle(){/*{{{*/
	long t;

	t = glutGet(GLUT_ELAPSED_TIME);
	player.integrate(t-last_t);
	for(int i=0; i<enemy_cnt; i++){
	ghost[i].movement(player.getPosition(),5);
	ghost[i].integrate(t-last_t);
	}
	last_t = t;
	glutPostRedisplay();
}/*}}}*/
void ReadJPEG(char *filename,unsigned char **image,int *width, int *height)/*{{{*/
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * infile;
  unsigned char **buffer;
  int i,j;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);


  if ((infile = fopen(filename, "rb")) == NULL) {
    printf("Unable to open file %s\n",filename);
    exit(1);
  }

  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_calc_output_dimensions(&cinfo);
  jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width;
  *height  = cinfo.output_height;


  *image=(unsigned char*)malloc(cinfo.output_width*cinfo.output_height*cinfo.output_components);

  buffer=(unsigned char **)malloc(1*sizeof(unsigned char **));
  buffer[0]=(unsigned char *)malloc(cinfo.output_width*cinfo.output_components);


  i=0;
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);

    for(j=0;j<cinfo.output_width*cinfo.output_components;j++)
      {
	(*image)[i]=buffer[0][j];
	i++;
      }   

    }

  free(buffer);
  jpeg_finish_decompress(&cinfo);
} /*}}}*/
void LoadTexture(char *filename,int dim)/*{{{*/
{
  unsigned char *buffer;
  unsigned char *buffer2;
  int width,height;
  long i,j;
  long k,h;

  ReadJPEG(filename,&buffer,&width,&height);

  buffer2=(unsigned char*)malloc(dim*dim*3);

  //-- The texture pattern is subsampled so that its dimensions become dim x dim --
  for(i=0;i<dim;i++)
    for(j=0;j<dim;j++)
      {
	k=i*height/dim;
	h=j*width/dim;
	
	buffer2[3*(i*dim+j)]=buffer[3*(k*width +h)];
	buffer2[3*(i*dim+j)+1]=buffer[3*(k*width +h)+1];
	buffer2[3*(i*dim+j)+2]=buffer[3*(k*width +h)+2];

      }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dim,dim,0,GL_RGB,GL_UNSIGNED_BYTE,buffer2);

  free(buffer);
  free(buffer2);
}/*}}}*/
