#include "type.h"
#include "config.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
Calendar
He Xian
*======================================================================*/


int year, month, day;

int day_of_month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
//char wek[7]={'����','��һ','�ܶ�','����','����','����','����'};
int current_year, current_month, current_day;

//�����ж�
int judgement(int y)
{
	if (y % 400 == 0 || (y % 100 != 0 && y % 4 == 0))
		return 1;
	else return 0;
}
int week_print(int year, int month, int day)
{
	/*
	��ʽ:w=(y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1)%7
		*/
	int w, k;   //��¼�ܼ�
	int year_last = year % 100, c = year / 100, m = month;
	if (month == 1)
	{
		year_last -= 1;
		m = 13;
	}
	else if (month == 2)
	{
		year_last -= 1;
		m = 14;
	}
	w = (year_last + year_last / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + day - 1); // abs  ����ֵ
	if (w < 0)
	{
		k = (w % 7 + 7) % 7;
	}
	else k = w % 7;
	return k;
}

//��ӡȫ������
void year_show(int fd_stdin)
{
	int i, k, x, first_week;
	printf("please choose the year (eg:2020):");
	char rdbuf[128];
	int r = read(fd_stdin, rdbuf, 70);
	rdbuf[r] = 0;
	int A = rdbuf[0] - '0';
	int B = rdbuf[1] - '0';
	int C = rdbuf[2] - '0';
	int D = rdbuf[3] - '0';
	year = A * 1000 + B * 100 + C * 10 + D;


	//scanf("%d", &year);
	printf("=================The Calendar of Year %d==================\n", year);
	printf("\n");
	if (judgement(year))
	{
		day_of_month[1] = 29;
	}
	else day_of_month[1] = 28;
	for (i = 1; i < 13; i++)
	{
		first_week = week_print(year, i, 1);
		printf("==========================%d=============================\n", i);
		printf("Sun Mon Tue Wed Thu Fri Sat\n");
		for (x = 1; x <= first_week; x++)
		{
			printf("    ");
			if (x % 7 == 0) printf("\n");
		}
		for (k = 1; k <= day_of_month[i - 1]; k++)
		{
			printf("%d  ", k);
			if (k < 10)printf(" ");
			if (x % 7 == 0) printf("\n");
			x++;
		}
		printf("\n");
		printf("\n");
		printf("\n");
	}
	//printf("%d \n", year);

}
//��ӡ�·�����

void month_show(int fd_stdin)
{
	int tag = 0;
	int k, x, first_week;

	do
	{
		printf("please choose the year (eg:2020):");
		char rdbuf2[128];
		int r2 = read(fd_stdin, rdbuf2, 70);
		rdbuf2[r2] = 0;
		int A = rdbuf2[0] - '0';
		int B = rdbuf2[1] - '0';
		int C = rdbuf2[2] - '0';
		int D = rdbuf2[3] - '0';
		year = A * 1000 + B * 100 + C * 10 + D;

		printf("please choose month (eg:12):");
		char rdbuf3[128];
		int r3 = read(fd_stdin, rdbuf3, 70);
		rdbuf3[r3] = 0;
		int E = rdbuf3[0] - '0';
		int F = rdbuf3[1] - '0';
		month = E * 10 + F;

		//printf("%d %d\n", year, month);
		//scanf("%d %d", &year, &month);
		if (month < 1 || month>12)
		{
			printf("misinput,please choose again\n");

		}
	} while (1 > month || month > 12);
	printf("======================%d %d======================\n", year, month);
	if (judgement(year))
	{
		day_of_month[1] = 29;
	}
	else day_of_month[1] = 28;
	first_week = week_print(year, month, 1);
	printf("Sun Mon Tue Wed Thu Fri Sat\n");
	for (x = 1; x <= first_week; x++)
	{
		printf("    ");
		if (x % 7 == 0) printf("\n");
	}
	for (k = 1; k <= day_of_month[month - 1]; k++)
	{
		printf("%d  ", k);
		if (k < 10)printf(" ");
		if (x % 7 == 0) printf("\n");
		x++;
	}
	printf("\n");
}

//������
int calendar(int fd_stdin, int fd_stdout)
{
	int choice, flag = 1;
	year = 1;
	month = 1;
	day = 1;
	clear();
	char c, k;
	for (; 1;)//��ʾ����˵���Ϊ���棬ÿ�β�����ص�����˵���
	{
		printf("                                  MENU\n");
		printf("*****************************************************************************\n");
		printf("*                      OPTION:                                              *\n");
		printf("*                             1.search for a year                           *\n");
		printf("*                             2.search for a month                          *\n");
		printf("*                             0.exit                                        *\n");
		printf("*****************************************************************************\n");
		printf("choose please:   ");
		while (1)
		{


			while (1)
			{
				char at[70];
				char bt[70];
				if (flag)
				{
					//c = getchar();
					char at[70];
					char bt[70];

					int r1 = read(fd_stdin, at, 70);
					at[r1] = 0;
					c = at[0];
					printf("\n");
				}



				choice = c - '0';
				if (choice >= 0 && choice < 3)
				{
					break;
					//getchar();
				}
				else {
					printf("misinput,please choose a number between 0 and 2\n");
					printf("\n");
					printf("choose please:   ");
				}

			}
			if (choice == 1)
			{
				year_show(fd_stdin);
				//	cls_screen();
				flag = 1;
			}
			else if (choice == 2)
			{
				month_show(fd_stdin);
				//cls_screen();
				flag = 1;
			}
			else if (choice == 0)
				return 0;
			else
			{
				printf("misinput, please choose again\n");
			}
		}
	}

	return 0;
}



/*======================================================================*
GUESS NUMBER
He Xian
*======================================================================*/
/*��������ĸ����ظ� 0~9֮�������
��С��������
λ����ȷ������ȷ A
������ȷλ�ò���ȷ B
*/

char sys_arr[4] = { 3,4,5,6 };
char user_arr[4] = {};
int counta = 0;
int countb = 0;

void numinit() {
	//ɸѡ�����������
//srand((unsigned int)time(NULL));
	for (int i = 0; i < 4; i++) {
		int isExist = 0;
		int temp = 0;
		while (1)
		{
			temp = get_ticks() % 10;
			for (int j = 0; j < i; j++) {
				if (sys_arr[j] == temp) {
					isExist = 1;
					break;
				}
			}
			if (isExist == 0) {
				break;
			}
		}

		//��ʼ����
		int j = 0;
		for (; j < i; j++) {
			//j��Ӧ�����������������temp���бȽ�
			if (temp < sys_arr[j]) {
				//j��������������ƶ� �ڳ��ռ�
				for (int k = i; k > j; k--) {
					sys_arr[k] = sys_arr[k - 1];
				}
				break;
			}
		}
		//���浽��������ȥ
		sys_arr[j] = temp;
	}

}

void welcome() {
	clear();
	printf("                         welcome\n");
	printf("***************************************************************\n");
	printf("GUESS\n");
	printf("direction:\n");
	printf("    system will generate 4 numbers between 0 and 9 in order,\n");
	printf("    try to find out what and where the numbers are!\n");
	printf("    we will give you some hints ��\n");
	printf("    A-> right number and right positon \n");
	printf("    B-> right number but wrong positon \n");
	printf("    press 'q' to quit the game \n");
	printf("***************************************************************\n");
	printf("\n");

}

int number_guessing(int fd_stdin, int fd_stdout)
{
      int flag=0;
	welcome();
	//init();

	//������
	/*for (int i = 0; i < 4; i++) {
		printf("%d  ", sys_arr[i]);

	}*/
	//printf("\n  ");
	//��Ϸ��ʼ
	while (1) {

		for (int i = 0; i < 4; i++) {
			printf("please input number %d:", i + 1);
			char rdbuf[128];
			int r = read(fd_stdin, rdbuf, 70);
			rdbuf[r] = 0;
			if(rdbuf[0]== 'q') 
			{flag =1;
			break;}
			user_arr[i] = rdbuf[0] - '0';
			//scanf("%d", &guessArray[i]);
		}
		if(flag==1) break;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (sys_arr[i] == user_arr[j]) {
					if (i == j) {
						counta++;
					}
					else {
						countb++;
					}
				}
			}
		}
		if (counta == 4) {
			printf("Congratulation!!!\n");
			break;
		}
		else {
			printf("You got %dA %dB! \n", counta, countb);
			printf("Try again\n");
			printf("\n");
			counta = 0;
			countb = 0;
		}
	}

	return 0;
}

/*======================================================================*
PUSH BOX
He Xian
*======================================================================*/
//��ͼ7��8�� ��[0-6] ��[0-7]
//0 ·
//1 ǽ
//2 ����
//3 �յ�
//4 �û�
//7 �û�λ����ֹ
//5 ���ӵ����յ�

int s_map[7][8] =
{
 {1,1,1,1,1,1,1,1},
 {1,0,0,0,0,3,0,1},
 {1,0,2,1,1,0,0,1},
 {1,0,0,3,2,0,1,1},
 {1,0,1,1,0,0,0,1},
 {1,0,0,4,0,1,0,1},
 {0,1,1,1,1,1,1,1}
}, map[7][8] = {};
int boards[7][8]={};
//��¼�û�λ��
int row = 0;
int col = 0;
int cnt = 0;//���Ӹ����������ж���Ϸ�Ƿ������
int flag = 0;

//��ʼ�����
void boxwelcome()
{
	printf("                   welcome\n");
	printf("***************************************************\n");
	printf("Push Box\n");
	printf("direction:\n");
	printf("    use 'wasd' keys to move\n");
	printf("    use 'q' to quit the game\n");
	printf("***************************************************\n");

}


//��ʼ����ͼ
void boxinit()
{
	for (int i = 0; i < 7 * 8; i++)
	{ 
		boards[i / 8][i % 8] = s_map[i / 8][i % 8];
		if (4 == boards[i / 8][i % 8]) //���ﻹ��Ҫ��¼�û���λ��
		{
			row = i / 8;
			col = i % 8;
		}
	}
}

//��ӡ��ͼ
void boxprint()
{
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			switch (boards[i][j])
			{
			case 0:
				printf(" "); break; //�ո����·��
			case 1:
				printf("#"); break; //��ӡǽ
			case 2:
			case 5:
				printf("@"); break; //��ӡ����
			case 3:
				printf("O"); break; //��ӡ�յ�
			case 4:
			case 7:
				printf("$"); break; //��ӡ�û�
			}
		}
		printf("\n");
	}
}

//������
void move(int nrow, int ncol, int nnrow, int nncol)
{
	if (0 == boards[nrow][ncol] || 3 == boards[nrow][ncol]) //�ж��Ƿ�Ϊ·�����յ㣬˵�������ƶ�
	{
		boards[nrow][ncol] += 4;//�������λ��
		boards[row][col] -= 4;//�뿪���λ��
		row = nrow;
		col = ncol;
	}
	else if (2 == boards[nrow][ncol] || 5 == boards[nrow][ncol]) //ǰ��Ϊ����
	{
		if (0 == boards[nnrow][nncol] || 3 == boards[nnrow][nncol]) //�ж�����ǰ�Ƿ��ܹ�ͨ��
		{
			boards[nnrow][nncol] += 2;
			boards[nrow][ncol] -= 2 - 4;//��2���������ˣ���4�û�������
			boards[row][col] -= 4;
			row = nrow;
			col = ncol;
		}
	}
	for (int i = 0; i < 7; i++) //ͳ����Ϸ�Ƿ�����������Ӷ����յ���ʱ�ͽ�����
	{
		for (int j = 0; j < 8; j++)
		{
			if (5 == boards[i][j])
			{
				cnt++;
			}
			if (2 == cnt)
			{
				printf("Congralation��You win��\n");
				return;//ͨ���˳���Ϸ
				flag = 1;
			}
		}
	}
	cnt = 0; //�����Ϸû�н������´λ�����Ҫ��0ͳ��
}



int push_box(int fd_stdin, int fd_stdout)
{
	//��Ϸ��ʼ��

	boxinit();

	//��ʼ��Ϸ
	while (1)
	{
		clear();//�����Ļ
		boxwelcome();
		boxprint();//��Ϸ��ʼʱ��ӡ��ͼ��ÿ�η����ƶ���Ҳ��ӡ��ͼ
		char rdbuf[128];
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;
		int dir = rdbuf[0]; //�Ӽ��̻�ȡ����
		
		switch (dir) //ÿ�ζ���Ҫ�ж�С����ǰ���ǰ���ǰ��������״̬������������move()
		 //������ͳһ���������д��
		{
		case 'w': move(row - 1, col, row - 2, col); break;
		case 's': move(row + 1, col, row + 2, col); break;
		case 'a': move(row, col - 1, row, col - 2); break;
		case 'd': move(row, col + 1, row, col + 2); break;

		case 'q':{flag=1;break;}
			//�����q���˳���Ϸ
		}
		if (flag) break;
	}

	return 0;
}

/*======================================================================*
Chess 5
Zhou Ziyi
*======================================================================*/

int chess_board[10][10];
int color;//0��ʾ����(����)��1��ʾ���壨��ң�
int direction[4][2] = { { 1,0 },{ 0,1 },{ 1,1 },{ -1,1 } };
PRIVATE void draw_chess_board();
PRIVATE void auto_play();
PRIVATE int is_game_over();
PRIVATE int evaluate_score(int i, int j);


void main_c() {
	int com = -1;
	while (1) {
		clear();
		printf("##################################\n");
		printf("###   Welcome to the 4_Chess   ###\n");
		printf("###           0.exit           ###\n");
		printf("###           1.enter          ###\n");
		printf("##################################\n");
		char c[2];
		read(0, c, 2);
		com = c[0] - '0';
		if (com == 0) {
			break;
		}
		else if (com == 1) {
			FourChess();
			break;
		}
		else {
			break;
		}
	}
	return;
}


PUBLIC void FourChess()
{
	int x, y, r;
	char rdxbuf[70];
	char rdybuf[70];
	for (x = 0; x < 10; x++)
		for (y = 0; y < 10; y++)
			chess_board[x][y] = 2;

	while (1)
	{
		//        if (is_game_over()==1){
		//            draw_chess_board();
		//           // printf("You lose\n");
		//            break;
		//        }
		//        else if(is_game_over()==2){
		//
		//            draw_chess_board();
		//           // printf("You win\n");
		//            break;
		//        }
		draw_chess_board();
		x = 0; y = 0;
		printf("Please input the line position where you put your Chess(x) and q to exit: ");
		r = read(0, rdxbuf, 70);
		if (rdxbuf[0] == 'q')
			break;
		rdxbuf[r] = 0;
		x=atoi(rdxbuf);
		printf("Please input the column position where you put your Chess(y): ");
		r = read(0, rdybuf, 70);
		rdybuf[r] = 0;
		y=atoi(rdybuf);
		if (x > 0 && x < 11 && y>0 && y < 11 && chess_board[x - 1][y - 1] == 2)
		{
			color = 1;//��ȡ������������,�������Ϊ����
			chess_board[x - 1][y - 1] = color;//����λ���������Ը�����
			draw_chess_board();

			auto_play(); //AI����
			draw_chess_board();
			if (is_game_over())
				break;
		}
		else
		{
			printf("Input Error!\n");
			//fflush(stdin);
			continue;
		}
	}
}

PRIVATE void draw_chess_board()
/*��������״̬�����̣����л��ƣ���Ե�ر���
�����ֵ0��ʾ����(����)��1��ʾ����(���)��2��ʾ��λ��û����*/
{
	int x, y;
	clear();

	for (x = 1; x <= 10; x++)
	{
		//��1��
		if (x == 1)
		{
			printf("   1 2 3 4 5 6 7 8 9 10 \n");
			printf(" 1 ");
			//��1��
			if (chess_board[x - 1][0] == 1)
				//printf("��");
				printf("%c%c", 7, 196);
			if (chess_board[x - 1][0] == 0)
				//printf("��");
				printf("%c%c", 9, 196);
			if (chess_board[x - 1][0] == 2)
				//printf("�� ");
				printf("%c%c", 218, 196);

			//��2-9��
			for (y = 2; y <= 9; y++)
			{
				if (chess_board[x - 1][y - 1] == 1)
					//printf("��");
					printf("%c%c", 7, 196);
				if (chess_board[x - 1][y - 1] == 0)
					//printf("��");
					printf("%c%c", 9, 196);
				if (chess_board[x - 1][y - 1] == 2)
					//printf("�� ");
					printf("%c%c", 194, 196);
			}

			//��10��
			if (chess_board[x - 1][y - 1] == 1)
				//printf("��");
				printf("%c ", 7);
			if (chess_board[x - 1][y - 1] == 0)
				//printf("��");
				printf("%c ", 9);
			if (chess_board[x - 1][y - 1] == 2)
				//printf("�� ");
				printf("%c ", 191);

			printf("\n");
		}

		//��2-9��
		if (x <= 9 && x >= 2)
		{
			//��1��
			printf(" %d ", x);
			if (chess_board[x - 1][0] == 1)
				//printf("��");
				printf("%c%c", 7, 196);
			if (chess_board[x - 1][0] == 0)
				//printf("��");
				printf("%c%c", 9, 196);
			if (chess_board[x - 1][0] == 2)
				//printf("�� ");
				printf("%c%c", 195, 196);

			//��2-9��
			for (y = 2; y <= 9; y++)
			{
				if (chess_board[x - 1][y - 1] == 1)
					//printf("��");
					printf("%c%c", 7, 196);
				if (chess_board[x - 1][y - 1] == 0)
					//printf("��");
					printf("%c%c", 9, 196);
				if (chess_board[x - 1][y - 1] == 2)
					//printf("�� ");
					printf("%c%c", 197, 196);
			}

			//��10��
			if (chess_board[x - 1][y - 1] == 1)
				//printf("��");
				printf("%c ", 7);
			if (chess_board[x - 1][y - 1] == 0)
				//printf("��");
				printf("%c ", 9);
			if (chess_board[x - 1][y - 1] == 2)
				//printf("�� ");
				printf("%c ", 180);
			printf("\n");
		}

		//��10��
		if (x == 9)
		{
			printf(" 10");
			if (chess_board[x - 1][0] == 1)
				//printf("��");
				printf("%c%c", 7, 196);
			if (chess_board[x - 1][0] == 0)
				//printf("��");
				printf("%c%c", 9, 196);
			if (chess_board[x - 1][0] == 2)
				//printf("�� ");
				printf("%c%c", 192, 196);

			for (y = 2; y <= 9; y++)
			{
				if (chess_board[x - 1][y - 1] == 1)
					//printf("��");
					printf("%c%c", 7, 196);
				if (chess_board[x - 1][y - 1] == 0)
					//printf("��");
					printf("%c%c", 9, 196);
				if (chess_board[x - 1][y - 1] == 2)
					//printf("�� ");
					printf("%c%c", 193, 196);
			}
			//��10��
			if (chess_board[x - 1][y - 1] == 1)
				//printf("��");
				printf("%c ", 7);
			if (chess_board[x - 1][y - 1] == 0)
				//printf("��");
				printf("%c ", 9);
			if (chess_board[x - 1][y - 1] == 2)
				//printf("�� ");
				printf("%c ", 217);
			printf("\n");
		}

	}


}
PRIVATE int is_game_over()//�ж���Ϸ�Ƿ����
{
	int num = 0;
	int x, y, xtemp, ytemp, colorflag;
	int sign = 0;//�ж��Ƿ��пո��־
	for (x = 0; x < 10; x++)
	{
		for (y = 0; y < 10; y++)
		{
			if (2 == chess_board[x][y]) //�пո�ɼ�������
			{
				sign = 1;
			}
			else //�����⵽�������ӣ�������������йص��Ƿ����Ӯ
			{
				for (int z = 0; z < 4; z++) //����ĸ������Ƿ����Ӯ
				{
					colorflag = chess_board[x][y];
					num = 1;
					xtemp = x + direction[z][0];
					ytemp = y + direction[z][1];
					while ((xtemp >= 0) && (xtemp < 10) && (ytemp < 10) && (5 != num))
					{
						if (colorflag == chess_board[xtemp][ytemp])
						{
							//printf("%d,%d\n",xtemp+1,ytemp+1);
							num++;
							//printf("%d\n",count);
							if (5 == num)
							{
								if (0 == colorflag)
								{
									printf("Computer win! Game Over!\n");
									return 1;
								}
								else if (1 == colorflag)
								{
									printf("You win! Game Over!\n");
									return 2;
								}
								//   return 1;
							}
							xtemp = xtemp + direction[z][0];
							ytemp = ytemp + direction[z][1];
						}
						else
						{
							num = 0;
							break;
						}
					}
				}
			}
		}
	}
	if (sign == 0)
	{
		printf("A draw! Game Over!\n");
		return 1;
	}
	else
		return 0;
}
PRIVATE int evaluate_score(int i, int j)
{
	int score;
	int boundary, max_num, num, boutemp;//�߽磬����
	int itemp, jtemp;
	int colorflag = chess_board[i][j];
	max_num = 0; num = 0; boundary = 0; boutemp = 0; score = 0;
	for (int z = 0; z < 4; z++)//�ж����Ӽ��߽����
	{
		num = 1; boutemp = 0;
		itemp = i + direction[z][0]; jtemp = j + direction[z][1];
		while (itemp >= 0 && itemp <= 9 && jtemp >= 0 && jtemp <= 9 && chess_board[itemp][jtemp] == colorflag)
		{
			num++;
			itemp = itemp + direction[z][0];
			jtemp = jtemp + direction[z][1];
		}
		if (itemp < 0 || itemp>9 || jtemp < 0 || jtemp>9 || chess_board[itemp][jtemp] == !colorflag)//������ǽ��Է�����
			boutemp++;
		itemp = i - direction[z][0]; jtemp = j - direction[z][1];
		while (itemp >= 0 && itemp <= 9 && jtemp >= 0 && jtemp <= 9 && chess_board[itemp][jtemp] == colorflag)
		{
			num++;
			itemp = itemp - direction[z][0];
			jtemp = jtemp - direction[z][1];
		}
		if (itemp < 0 || itemp>9 || jtemp < 0 || jtemp>9 || chess_board[itemp][jtemp] == !colorflag)//������ǽ��Է�����
			boutemp++;
		if (num > max_num)
		{
			max_num = num;
			boundary = boutemp;
		}
	}
	/*�����ж��������*/
	if (max_num == 1 && boundary == 2)//��1
		score = 100;
	else if (max_num == 1 && boundary == 1)
		score = 200;
	else if (max_num == 1 && boundary == 0) //��1
		score = 500;
	else if (max_num == 2 && boundary == 2) //��2
		score = 500;
	else if (max_num == 2 && boundary == 1)
		score = 1000;
	else if (max_num == 2 && boundary == 0) //��2
		score = 3000;
	else if (max_num == 3 && boundary == 2) //��3
		score = 1000;
	else if (max_num == 3 && boundary == 1)
		score = 3000;
	else if (max_num == 3 && boundary == 0) //��3
		score = 50000;
	else if (max_num == 4 && boundary == 2)
		score = 2000;//��4
	else if (max_num == 4 && boundary == 1)
		score = 5000;
	else if (max_num == 4 && boundary == 0)
		score = 200000;
	else if (max_num == 5)
		score = 10000000;
	return score;
}
PRIVATE void auto_play()
{
	int maxx, maxy, max_score, att_score, def_score, score;
	int i, j;
	max_score = 0; att_score = 0; def_score = 0; score = 0;
	maxx = 0; maxy = 0;
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (chess_board[i][j] == 2) //�����õ����
			{
				/*�жϹ�����ֵ*/
				chess_board[i][j] = 0;
				att_score = evaluate_score(i, j);
				/*�жϷ�����ֵ*/
				chess_board[i][j] = 1;
				def_score = evaluate_score(i, j);
				chess_board[i][j] = 2;
				score = att_score + 1.5*def_score;
			}
			if (score > max_score)
			{
				max_score = score;
				maxx = i;
				maxy = j;
			}
		}
	}
	chess_board[maxx][maxy] = 0;
}

/*======================================================================*
MINE
Zhou Ziyi
*======================================================================*/
#define ROW 9
#define COL 9
#define MINE 10
#define NULL  ((void *) 0)


void game(void);//��Ϸ��ʼ��������
void display(void);//��ӡͷ��
void init_mine(void);//��ʼ������
void init_show(void);//��ʼ�����û����ĵ�ͼ
int win_judge(void);//�������Ƿ��ʤ�ĺ���
void output_board(char bd[][COL + 2], int flag);//��ӡ���� ���flag=1���Ǵ�ӡmine��������show
void set_mine(int x, int y);//������������õ���
int get_num(int x, int y);//����bd[x][y]��Χ�Ƿ��е��ײ�����
void set_num(void);//��ÿһ�������������֣�0-8
void open_mine(int x, int y);//�Զ�չ������ĺ���
//int getch(void);
//void get_xy(void);

int main_();
char mine[ROW + 2][COL + 2];//���׵���Ϸ��
char show[ROW + 2][COL + 2];//չʾ���û�����Ϸ��
const int dir[8][2] = { {-1,0},{0,1},{1,0},{0,-1},{-1,-1},{-1,1},{1,1},{1,-1} };//��8�����������
int fflag = 0;


int main_() {
	int com = -1;

	while (1) {
		fflag = 0;
		display();
		//scanf("%d",&com);
		//printf(" %d",com);
		char c[2];
		read(0, c, 2);
		com = c[0] - '0';
		if (com == 0)
			break;
		else if (com == 1) {
			game();
			if (fflag == 1)//��������
				break;
		}
		else {
			break;
		}
	}
	return 0;
}

void game(void) {
	init_mine();
	init_show();
	int x, y;
	int flag = 1;
	while (1) {
		if (win_judge()) {//���Ӯ��
			clear();
			output_board(show, 0);
			output_board(mine, 1);
			printf("You win \n");
			break;
		}
		clear();
		output_board(show, 0);
		printf("########################################################## \n");
		printf("Please enter the x and y\n");
		printf("Enter the q to back to exit\n");
		printf("########################################################## \n");

		char cx[20], cy[20];
		printf("Please enter the x :");
		int r1 = read(0, cx, 20);
		if (cx[0] == 'q') {
			fflag = 1;
			return;
		}
		printf("Please enter the y :");
		int r2 = read(0, cy, 20);
		cx[r1] = 0;
		cy[r2] = 0;

		x=atoi(cx);
		y=atoi(cy);
		//scanf("%d%d", &x, &y);


		if (x >= 1 && x <= ROW && y >= 1 && y <= COL) {
			if (flag) {
				flag = 0;
				set_mine(x, y);
				set_num();
			}
			if (mine[x][y] == '*') {
				clear();
				printf("You lose\n");
				output_board(mine, 1);
				break;
			}
			if (mine[x][y] != '0'&&mine[x][y] != '*'&&mine[x][y] != '/') {//����ĵط������ֵ����
				show[x][y] = mine[x][y];
			}
			else {//����0�ĵط�
				open_mine(x, y);
			}
		}
		else {
			printf("Wrong_Input");
		}

	}

}


void display(void) {
	clear();
	printf("##################################\n");
	printf("###     Welcome to the mine    ###\n");
	printf("###           0.exit           ###\n");
	printf("###           1.enter          ###\n");
	printf("##################################\n");
}

void init_mine(void) {
	/*for(int i=0;i<ROW;i++){//������ߵ�����
		mine[i][0]=i-'0';//��߽��ʼ��Ϊ��������
		mine[i][COL+1]='#';//�ұ߽��ʼ��Ϊ#
		for(int j=1;j<=COL;j++){
			mine[i][j]='0';//��ʼ����������
		}
	}
	for(int i=0;i<COL;i++){
		mine[0][i]=i-'0';//��ʼ���ϱ߽�Ϊ��������
		mine[ROW+1][i]='#';//��ʼ���±߽�Ϊ#
	}*/
	for (int i = 1; i <= ROW; i++) {
		for (int j = 1; j <= COL; j++) {
			mine[i][j] = '0';
		}
	}
}

void init_show(void) {
	for (int i = 1; i <= ROW; i++) {
		for (int j = 1; j <= COL; j++) {
			show[i][j] = '-';//��ʼʱȫ�����û�����
		}
	}
}

int win_judge(void) {
	for (int i = 1; i <= ROW; i++) {
		for (int j = 1; j <= COL; j++) {
			if (mine[i][j] != '*') {//���û��
				if (mine[i][j] == '0' || mine[i][j] != show[i][j]) {//���mine�����л��С�0����ʾ��û��ɨ���ף�����mine[i][j] !=show[i][j],��˼�ǻ��еĸ���û�б��㵽
					return 0;
				}
			}
		}
	}
	return 1;
}


void output_board(char bd[][COL + 2], int flag) {//flag�����ʲô���飬��ӡ������Ϸ��
	printf("  ");
	for (int i = 1; i <= COL; i++) {
		printf(" %d", i);
	}
	printf("\n");
	for (int i = 1; i <= ROW; i++) {
		printf(" %d", i);
		for (int j = 1; j <= COL; j++) {
			if (flag == 1) {//Ҫ��ӡmine[][]
				if (bd[i][j] != '*') {
					if (bd[i][j] == '0') {//û���ף���ΧҲû���ף���ʾ�ո�
						printf("  ");
					}
					else {
						printf(" %c", bd[i][j]);//û���׵�����Χ���ף�����ʾ��Χ���׵�����
					}
				}
				else {
					printf(" *");
				}
			}
			else {//Ҫ��ӡshow[][]
				if (bd[i][j] == '0')
					printf("  ");
				else
					printf(" %c", bd[i][j]);
			}
		}
		printf("\n");
	}

}


void set_mine(int x, int y) {//xy��Ϊ�˱�����ҵ�һ����Ͳ���
	//srand((unsigned int)time(NULL));
	int sum = 0;
	int xx, yy;
	while (sum != MINE) {
		xx = get_ticks() % ROW + 1;
		yy = get_ticks() % COL + 1;
		if (mine[xx][yy] == '0'&&xx != x && yy != y) {
			mine[xx][yy] = '*';//����
			sum++;
		}
	}
}


void open_mine(int x, int y) {//������Զ�չ��û���׵ĵط�
	while (mine[x][y] == '0')  //�����ѭ������Ҳ�ǳ�����������������ʱ�����ݹ飬����������ʱ�����ݹ�
	{
		mine[x][y] = '/';  //ֻҪ����˵���������ĸ��Ӳ����ף�����Ѷ�Ӧ�����mine�����Ϊ��/������������ط�ɨ����ˣ���ֹ�������Ų�
		show[x][y] = ' ';  //û���׵Ļ��������û�չʾ�� ���� �����ǡ�0��
		for (int i = 0; i < 8; i++) {
			x += dir[i][0];
			y += dir[i][1];
			open_mine(x, y);
		}
	}
	if (mine[x][y] != '*' && mine[x][y] != '/')   //��������֣���ô����ʾ���֣�������ֱ�ʾ���������Ϊ���ĵĸ�����Χ�м�����
	{
		show[x][y] = mine[x][y];
	}
}

int get_num(int x, int y) {
	int res = 0;
	int xx, yy;
	for (int t = 0; t < 8; t++) {
		xx = x + dir[t][0];
		yy = y + dir[t][1];
		if (mine[xx][yy] == '*') {
			res++;
		}
	}
	return res;
}

void  set_num(void) {//�˺��������û�����֮ǰ�Ͱ���Ҫ�����ֵĸ�����ã������Ͳ���Ҫ��ÿ��ɨ��ʱ�ټ�����
	for (int i = 1; i <= ROW; i++) {
		for (int j = 1; j < +COL; j++) {
			if (mine[i][j] == '0') {
				mine[i][j] = get_num(i, j) + '0';
			}
		}
	}
}

/*======================================================================*
CALCULATE
Zhou Ziyi
*======================================================================*/


typedef enum { FALSE, TRUE } BOOL;
void Init(int s[], int *index);

// �жϿ�ջ
BOOL Empty(int s[]);

// ��ջ
void Push(int s[], int data, int *index);

// ��ջ
void Pop(int s[], int *index);

void Init_p(void);//���ڳ�ʼ�����ȼ��ĺ���
BOOL jud(int s[], int ope, int *index);//�ж��Ƿ�Ӧ����ջ
void calc(int ope_stack[], int num_stack[], int *index_o, int *index_n);//���㺯��
void deal_ope(int ope_stack[], int num_stack[], int ope, int *index_o, int *index_n);//�������������
BOOL isEmpty(int s[], int *index);
int GetTop(int s[], int *index);

int num_stack[100] = { 0 };//ģ��Ĳ�����ջ
int num_stack_count = -1;

int ope_stack[100] = { 0 };
int ope_stack_count = -1;

int isp[128], icp[128];//���ȼ�����

void calmain() {
	int com = -1;
	while (1) {
		printf("--------------------\n");
		printf("   input your choice \n");
		printf("        0.exit       \n");
		printf("        1.enter       \n");
		printf("--------------------\n");

		//scanf("%d",&com);
		//printf(" %d",com);
		char c[2];
		read(0, c, 2);
		com = c[0] - '0';
		if (com == 0)
			break;
		else if (com == 1) {
			calculate();
			break;
		}
		else {
			printf("Input error��please input again!\n");
			break;
		}
	}
	return;
}

int calculate() {
	char buf[100];
	int select = 0;
	while (1) {


		//            printf("Please input your choosed number:");
		//            char temp[70];
		//            int r = read(0, temp, 70);
		//            temp[r] = 0;

		//            if (temp[0]=='q') break;
		//            if (temp[0]!='q'&&temp[0]!='e') {
		//                printf("Input error��please input again!\n");
		//                continue;
		//            }
		//
		printf("########################### \n");
		printf("Please enter your polynomial,and enter = to end\n");
		printf("Enter q to back to exit \n");
		printf("########################### \n");
		read(0, buf, 100);
		buf[strlen(buf)] = '\0';
		if (buf[0] == 'q') {
			break;
		}

		Init_p();
		Init(num_stack, &num_stack_count);
		Init(ope_stack, &ope_stack_count);

		char *p = buf;//�����û�����Ķ���ʽ--��ת��Ϊ��׺���ʽ
		while (*p!='=')
		{
			if (*p >= '0' && *p <= '9')       // ���������
			{
				int num = 0;
				while (*p >= '0' && *p <= '9')
				{
					num = num * 10 + *p - '0';
					p++;//ת��Ϊ����
				}
				//printf("num: %d\n",num);
				Push(num_stack, num, &num_stack_count);      // ��������ջ
				continue;
			}
			deal_ope(ope_stack, num_stack, *p, &ope_stack_count, &num_stack_count);   // ���������
			p++;
		}
		// ����
		while (!isEmpty(ope_stack, &ope_stack_count))
		{
			calc(ope_stack, num_stack, &ope_stack_count, &num_stack_count);
		}


		int res = GetTop(num_stack, &num_stack_count);

		printf("Result %d\n", res);
		// printf("%d\n",num_stack_count);


	}

	return 0;

}
void Init_p(void) {//���ȼ�����
	isp['#'] = icp['#'] = 0;
	isp['('] = 1;
	isp['*'] = isp['/'] = isp['%'] = 5;
	isp['^'] = 7;
	isp['+'] = isp['-'] = 3;
	isp[')'] = 9;
	//   isp['!'] = 9;
	icp['('] = 9;
	icp['*'] = icp['/'] = icp['%'] = 4;
	icp['^'] = 6;
	icp['+'] = icp['-'] = 2;
	icp[')'] = 1;
	// icp['!'] = 8;
	return;
}

void Init(int s[], int *index) {
	if (*index == -1)
		return;
	*index = -1;
}

BOOL isEmpty(int s[], int *index) {
	if (*index == -1)
		return TRUE;
	else
		return FALSE;
}

void Push(int s[], int data, int *index) {
	*index = *index + 1;
	s[*index] = data;
}

void Pop(int s[], int *index) {
	if (*index == -1)
		return;
	*index = *index - 1;
}


int GetTop(int s[], int *index) {
	if (*index == -1)
		return -1;
	return s[*index];
}

BOOL jud(int s[], int ope, int *index) {
	if (isEmpty(s, index))
		return TRUE;
	int top = GetTop(s, index);
	if (isp[top] < icp[ope]) {
		return TRUE;
	}
	else if (isp[top] > icp[ope]) {
		return FALSE;
	}
	else {
		Pop(s, index);
		return TRUE;
	}
}

void calc(int ope_stack[], int num_stack[], int *index_o, int *index_n)
{
	int num1 = GetTop(num_stack, index_n);
	Pop(num_stack, index_n);

	int num2 = GetTop(num_stack, index_n);
	Pop(num_stack, index_n);

	//printf("num1:%d\n",num1);
	//printf("num2:%d\n",num2);
	int ope = GetTop(ope_stack, index_o);
	Pop(ope_stack, index_o);

	//printf("ope:%d",ope);
	int res = 0;
	switch (ope)
	{
	case '+':
		res = num1 + num2;
		break;
	case '-':
		res = num2 - num1;
		break;
	case '*':
		res = num2 * num1;
		break;
	case '/':
		if (num1 == 0)
			printf("Divided by 0!\n");
		else {
			res = num2 / num1;
		}
		break;
	case '^':
		res = 1;
		for (int i = 1; i <= num1; i++) {
			res *= num2;
		}
		break;
	case '%':
		res = num2 % num1;
		break;
	default:
		break;
	}

	Push(num_stack, res, index_n);
}

void deal_ope(int ope_stack[], int num_stack[], int ope, int *index_o, int *index_n) {
	if (jud(ope_stack, ope, index_o) == TRUE) {
		Push(ope_stack, ope, index_o);
	}
	else {
		while (jud(ope_stack, ope, index_o) == FALSE) {
			calc(ope_stack, num_stack, index_o, index_n);
		}

		if (')' != ope) {
			Push(ope_stack, ope, index_o);
		}
	}
}

/*======================================================================*
G2048
Du Mei
*======================================================================*/
/*****************************************************************************
 *                                2048
 *****************************************************************************/

#define GAME_SIZE  4

void left(int(*data)[GAME_SIZE])
{
	for (int row = 0; row < GAME_SIZE; ++row) {
		int cur = -1; //���һ�������ֵ��к�
		int change = 0; //ǰһ�����Ƿ��Ǻϲ������� 
		for (int col = 0; col < GAME_SIZE; ++col) {
			if (data[row][col] != 0) {
				//������ϲ� 
				if (cur >= 0 && !change && data[row][col] == data[row][cur]) {
					data[row][cur] += data[row][col]; //��ȣ���ǰһ�������
					data[row][col] = 0;
					printf("\n%d, %d, add to %d\n", row, cur, data[row][cur]);
					change = 1;
				}
				else {
					data[row][++cur] = data[row][col]; //�����
					change = 0;
					if (cur != col) data[row][col] = 0;
				}
			}
		}
	}
}

void right(int(*data)[GAME_SIZE])
{
	for (int row = 0; row < GAME_SIZE; ++row) {
		int cur = GAME_SIZE; //���һ�������ֵ��к�
		int change = 0; //ǰһ�����Ƿ��Ǻϲ������� 
		for (int col = GAME_SIZE - 1; col >= 0; --col) {
			if (data[row][col] != 0) {
				//������ϲ� 
				if (cur < GAME_SIZE && !change && data[row][col] == data[row][cur]) {
					data[row][cur] += data[row][col]; //��ȣ���ǰһ�������
					data[row][col] = 0;
					printf("\n%d, %d, add to %d\n", row, cur, data[row][cur]);
					change = 1;
				}
				else {
					data[row][--cur] = data[row][col]; //�����
					change = 0;
					if (cur != col) data[row][col] = 0;
				}
			}
		}
	}
}

void up(int(*data)[GAME_SIZE])
{
	for (int col = 0; col < GAME_SIZE; ++col) {
		int cur = -1;
		int change = 0;
		for (int row = 0; row < GAME_SIZE; ++row) {
			if (data[row][col] != 0) {
				if (cur >= 0 && !change && data[row][col] == data[cur][col]) {
					data[cur][col] += data[row][col];
					data[row][col] = 0;
					change = 1;
				}
				else {
					data[++cur][col] = data[row][col];
					change = 0;
					if (cur != row) data[row][col] = 0;
				}
			}
		}
	}
}

void down(int(*data)[GAME_SIZE])
{
	for (int col = 0; col < GAME_SIZE; ++col) {
		int cur = GAME_SIZE;
		int change = 0;
		for (int row = GAME_SIZE - 1; row >= 0; --row) {
			if (data[row][col] != 0) {
				if (cur < GAME_SIZE && !change && data[row][col] == data[cur][col]) {
					data[cur][col] += data[row][col];
					data[row][col] = 0;
					change = 1;
				}
				else {
					data[--cur][col] = data[row][col];
					change = 0;
					if (cur != row) data[row][col] = 0;
				}
			}
		}
	}
}

/* next
  * �����Ӯ�����������һ������20%�ĸ�����4��80%�ĸ�����2
  * return 0 for next turn, 1 for win, -1 for lose
  */
int next(int(*data)[GAME_SIZE])
{
	int empty_index[GAME_SIZE*GAME_SIZE];
	int empty_num = 0;
	//int exist2048 = 0;

	for (int i = 0; i < GAME_SIZE; ++i) {
		for (int j = 0; j < GAME_SIZE; ++j) {
			if (data[i][j] == 0) {
				empty_index[empty_num++] = i * GAME_SIZE + j;
			}
			else if (data[i][j] == 2048) return 1; //win
		}
	}
	if (empty_num == 0) return -1; //lose

	// next turn, random positon for number
	int ranpos = empty_index[get_ticks() % empty_num];
	//int ranpos = empty_index[rand() % empty_num];
	data[ranpos / GAME_SIZE][ranpos%GAME_SIZE] = get_ticks() % 5 ? 2 : 4;
	return 0;
}

/* print game grid */
void output(int(*data)[GAME_SIZE])
{
	clear();
	printf("\n    =========================");
	printf("\n    ======= GAME 2048 =======");
	printf("\n    =========================\n\n");
	printf("    +--------------------+\n\n");
	for (int i = 0; i < GAME_SIZE; ++i) {
		printf("    ");
		for (int j = 0; j < GAME_SIZE; ++j) {
			if (data[i][j]) printf("%4d ", data[i][j]);
			else printf("   . ");
		}
		printf("\n\n");
	}
	printf("    +--------------------+\n\n");
	printf("    PRESS KEY: \n");
	printf("        \'a\' for left  \n");
	printf("        \'d\' for right \n");
	printf("        \'w\' for up    \n");
	printf("        \'s\' for down  \n");
	printf("        \'q\' for exit  \n\n");
}

/* run game 2048 */
int g2048(void)
{
	int data[GAME_SIZE][GAME_SIZE] = { 0 };
	int score = 0; //���� 

	/* begin message */
	output(data);
	printf("    press \'y\' to begin: ");
	//read
	char buffer[16];
	read(0, buffer, 16);
	char *p = buffer;
	if (*p != 'y' && *p != 'Y') {
		printf("\n    Exit the game. thank you!\n");
		return 0;
	}

	/* first turn */
	printf("\n\n");
	next(data);
	output(data);

	/* turns */
	while (1) {
		//int c = getch();
		read(1, buffer, 16);
		switch (*p) {
		case 'a': left(data); break;
		case 's': down(data); break;
		case 'd': right(data); break;
		case 'w': up(data); break;
		case 'q': case 'Q':
			printf("\n    Exit the game.");
			printf("\n    Your score is %d. Thank you!\n\n", score);
			return 0;
		default: continue;
		}
		int ret = next(data);

		output(data);
		++score;

		if (ret == -1) {
			printf("    game over !\n");
			printf("    Your score is %d. Thank you!\n\n", score);
			return 0;
		}
		if (ret == 1) {
			printf("    you  win  !\n");
			printf("    Your score is %d. Thank you!\n\n", score);
			return 0;
		}
	}
}

/*======================================================================*
							  DOGE TIMER
							  Li Yuanfeng
 *======================================================================*/
void start() {
	printf("\n");
	printf("          ===================================================== \n");
	printf("          #                         DOGE                      # \n");
	printf("          #                                                   # \n");
	printf("          #                      DOGE TIMER                   # \n");
	printf("          #                                                   # \n");
	printf("          #         [COMMAND]                 [FUNCTION]      #\n");
	printf("          #                                                   #\n");
	printf("          #            $ set        |        Start timing     #\n");
	printf("          #            $ clear      |        Clear screen     #\n");
	printf("          #            $ help       |        Show commands    #\n");
	printf("          #            $ quit       |      Quit DOGE TIMER    #\n");
	printf("          #                                                   # \n");
	printf("          #             Supported by DOGE R&D TEAM            # \n");
	printf("          #                  ALL RIGHT REVERSED               # \n");
	printf("          ===================================================== \n");
	printf("\n");
}

void showtime(int i) {
	clear();
	printf("\n");
	printf("          =======================DOGE TIMER==================== \n");
	printf("\n");
	printf("                             *                *        \n");
	printf("                            ***              ***   \n");
	printf("                           *****            *****  \n");
	printf("                          ************************ \n");
	printf("                         **                      **\n");
	printf("                        **                        ** \n");
	printf("                       **                          ** \n");
	printf("                                    %d s             \n", i);
	printf("                       **                          ** \n");
	printf("                        **                        **\n");
	printf("                         **                      **\n");
	printf("                          ************************  \n");
	printf("\n");
	printf("          =======================DOGE TIMER==================== \n");
	printf("\n");
	milli_delay(10000);
}

void timer(int fd_stdin)
{
	char rdbuf[128];
	start();
	while (1) {
		memset(rdbuf, 0x00, sizeof(char) * 128);
		printf("TIMER $ ");
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;
		if (strcmp(rdbuf, "set") == 0) {
			while (1) {
				memset(rdbuf, 0x00, sizeof(char) * 128);
				printf("Please set time(unit:second):");
				int r = read(fd_stdin, rdbuf, 70);
				rdbuf[r] = 0;
				int tmp = -1;
				tmp = atoi(rdbuf);
				if (tmp >= 0) {
					int i = tmp;
					for (; i >= 0; i--) {
						showtime(i);
					}
					printf("Time up!\n");
					break;
				}
				else {
					printf("The time you entered is illegal, please re-enter.\n");
					continue;
				}
			}
			continue;
		}
		if (strcmp(rdbuf, "quit") == 0) {
			clear();
			break;
		}
		if (strcmp(rdbuf, "help") == 0) {
			start();
			continue;
		}
		if (strcmp(rdbuf, "clear") == 0) {
			clear();
			continue;
		}
		printf("The format of the command you entered is incorrect, please re-enter.\n");
		printf("You can input $ help to know more.\n");
		continue;
	}
}

/*****************************************************************************
 *                          countdown timer
 //DU MEI
 *****************************************************************************/
void countdown_timer()
{
	int sec = 0;	//countdown seconds 
	clear();
	printf("Please set the countdown(second): ");
	//read countdown time
	char buffer[1024];
	read(0,buffer,1024);
	char *p = buffer;
	while (*p) {
		if (*p >= '0' && *p <= '9') {
			sec = sec*10 + (*p - '0');
			++p;
		}
		else break;
	}
	//countdown
	while(1)
	{
		clear();
		if (sec <= 0) {
			printf("Remaining time: %d\n",sec);
			printf("Time is over!        \n\n"); 
			milli_delay(1000);
			break;
		}
		printf("Remaining time: %d\n",sec);
		milli_delay(1000);
		//delay(1000);
		--sec;
	}
}

/*****************************************************************************
 *                     Scissors, rock and paper(srp)
 *****************************************************************************/
void srp()
{
	clear();
	printf("\n    ==================================");
	printf("\n    ==== Scissors, rock and paper ====");
	printf("\n    ==================================\n\n");
	
	printf("     Press \'s\' for scissors\n");
	printf("     Press \'r\' for rock\n");
	printf("     Press \'p\' for paper\n");
	printf("     Press \'q\' to quit the game\n");
	
	int turn = 0;
	int win = 0;  // Ӯ�ľ��� 
	int lose = 0; // ��ľ��� 
	
	while (1)
	{
		printf("\n     ***TURN %d WIN %d LOSE %d***", ++turn, win, lose);
		
		/* read player's inputment */
		printf("\n     Please input your choice: ");
		char buffer[16];
		char* p = buffer;
		read(0, p, 16);
		
		/* player choice */
		int player;
		switch (*p) {
			case 's': case 'S':
				player = 0;
				break;
			case 'r': case 'R':
				player = 1;
				break;
			case 'p': case 'P':
				player = 2;
				break;
			case 'q': case 'Q':
				printf("\n     You quit the game.");
				printf("\n     %d turns in total, you win %d turns, lose %d turns.\n", --turn, win, lose);
				milli_delay(20000);
				return;
			default:
				printf("     Confusing inputment. Try again.\n");
				--turn;
				continue;
				
				
		}
		
		/* computer choose randomly */
		int cp = get_ticks() % 3;
		switch (cp) {
			case 0:
				printf("     Computer choose scissors");
				break;
			case 1:
				printf("     Computer choose rock");
				break;
			case 2:
				printf("     Computer choose paper");
				break;
		}
		
		/* judge winner */
		switch ((player+3-cp)%3) {
			case 0:
				printf("\n     draw\n");
				break;
			case 1:
				win++;
				printf("\n     you win!\n");
				break;
			case 2:
				lose++;
				printf("\n     you lose\n");
				break;
		}
	}
	
}


