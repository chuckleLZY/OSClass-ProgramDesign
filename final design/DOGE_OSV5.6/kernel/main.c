/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
							main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
													Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
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
#include "keyboard.h"


/*======================================================================*
							kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	struct task* p_task;
	struct proc* p_proc = proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16   selector_ldt = SELECTOR_LDT_FIRST;
	u8    privilege;
	u8    rpl;
	int   eflags;
	int   i, j;
	int   prio;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++) {
		if (i < NR_TASKS) {     /* ���� */
			p_task = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio = 15;
		}
		else {                  /* �û����� */
			p_task = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202; /* IF=1, bit 2 is always 1 */
			prio = 5;
		}

		strcpy(p_proc->name, p_task->name);    /* name of the process */
		p_proc->pid = i;            /* pid */

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
			sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
			sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		/* p_proc->nr_tty        = 0; */

		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p_proc->filp[j] = 0;

		p_proc->ticks = p_proc->priority = prio;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	/* proc_table[NR_TASKS + 0].nr_tty = 0; */
	/* proc_table[NR_TASKS + 1].nr_tty = 1; */
	/* proc_table[NR_TASKS + 2].nr_tty = 1; */

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;

	init_clock();
	init_keyboard();

	restart();

	while (1) {}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}

/*****************************************************************************
*                                clear
*****************************************************************************/
void clear()
{
	clear_screen(0, console_table[current_console].cursor);
	console_table[current_console].crtc_start = 0;
	console_table[current_console].cursor = 0;
}

/*======================================================================*
							   TestA
 *======================================================================*/
void TestA()
{
	int fd;
	int i, n;

	char filename[MAX_FILENAME_LEN + 1] = "NewFile";

	char tty_name[] = "/dev_tty0";

	int fd_stdin = open(tty_name, O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];

	clear();

	/*================================= �����ǿ������� ===============================*/
	Booting();
	clear();
	/*================================= ��������ʾ�������ϵͳ��Ϣ ===============================*/
	commands();

	while (1) {
		printf("DOGE $ ");
		memset(rdbuf, 0, sizeof(rdbuf));
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;
		if (strcmp(rdbuf, "help")==0)
		{
			clear();
			commands();
			continue;
		}
		if (strcmp(rdbuf, "clear")==0)
		{
			clear();
			continue;
		}
		if (strcmp(rdbuf, "calendar") == 0) {
			clear();
			calendar(fd_stdin, fd_stdout);
			continue;
		}
		if (strcmp(rdbuf, "timer1") == 0) {
			clear();
			timer(fd_stdin);
			continue;
		}
		if (strcmp(rdbuf, "timer2") == 0) {
			clear();
			countdown_timer();
			continue;
		}
		if (strcmp(rdbuf, "game") == 0) {
			clear();
			gamesquare();
			while (1) {
				printf("DOGE GAME SQUARE $ ");
				memset(rdbuf, 0, sizeof(rdbuf));
				int r = read(fd_stdin, rdbuf, 70);
				rdbuf[r] = 0;
				if (strcmp(rdbuf, "help") == 0)
				{
					clear();
					gamesquare();
					continue;
				}
				if (strcmp(rdbuf, "clear") == 0)
				{
					clear();
					continue;
				}
				if (strcmp(rdbuf, "1") == 0)
				{
					clear();
					int numberguessing= number_guessing(fd_stdin, fd_stdout);
					continue;
				}
				if (strcmp(rdbuf, "2") == 0)
				{
					clear();
					int pushbox = push_box(fd_stdin, fd_stdout);
					continue;
				}
				if (strcmp(rdbuf, "3") == 0)
				{
					clear();
					main_c();
					continue;
				}
				if (strcmp(rdbuf, "4") == 0)
				{
					clear();
					int mine = main_();
					continue;
				}
				if (strcmp(rdbuf, "5") == 0)
				{
					clear();
					int game2048 = g2048();
					continue;
				}
				if (strcmp(rdbuf, "6") == 0)
				{
					clear();
					srp();
					continue;
				}
				if (strcmp(rdbuf, "quit") == 0)
				{
					clear();
					break;
				}
				printf("The format of the command you entered is incorrect.\n");
				printf("You can input $ help to know more.\n");
				continue;

			}
			continue;
		}
		if (strcmp(rdbuf, "fs") == 0) {
			clear();
			filemanagerprocess(fd_stdin);
			continue;
		}
		if (strcmp(rdbuf, "pm") == 0) {
			clear();
			ProcessManager(fd_stdin, fd_stdout);
			continue;
		}
		if (strcmp(rdbuf, "about") == 0) {
			clear();
			about();
			continue;
		}
		if (strcmp(rdbuf, "calcu") == 0) {
			clear();
			calmain();
			continue;
		}
		printf("The format of the command you entered is incorrect.\n");
		printf("You can input $ help to know more.\n");
		continue;
	}
	/* never arrive here */
}

/*======================================================================*
							   TestB
 *======================================================================*/
void TestB()
{
	char tty_name[] = "/dev_tty1";

	int fd_stdin = open(tty_name, O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];

	while (1)
	{
		printf("$ ");



		milli_delay(100);
	}
}

/*======================================================================*
							   TestC
 *======================================================================*/
void TestC()
{
	spin("TestC");
	/* assert(0); */
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}








/*======================================================================*
						Functions of PM
 *======================================================================*/
void ProcessInfo();
void ShowCommands();
void CreateProcess(int id);
void KillProcess(int id);
void ProcessManager(int fd_stdin, int fd_stdout)
{
	clear();
	ShowCommands();
	char rdbuf[128];
	while (1)
	{
		printf("DOGE/PROCESS MANAGER $ ");
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;

		if (!strcmp(rdbuf, "ps"))
		{
			ProcessInfo();
		}
		//create:创建进程
		else if (!strcmp(rdbuf, "create"))
		{
			int id = 0;
			printf("please input process id:");
			int r = read(fd_stdin, rdbuf, 70);
			rdbuf[r] = 0;
			if (rdbuf[0] >= '0'&&rdbuf[0] <= '6')
			{
				id = id + (rdbuf[0] - '0');
			}
			if (id)
				CreateProcess(id);
		}
		//kill:结束进程
		else if (!strcmp(rdbuf, "kill"))
		{
			int id = 0;

			printf("please input process id:");
			int r = read(fd_stdin, rdbuf, 70);
			rdbuf[r] = 0;
			if (rdbuf[0] >= '0'&&rdbuf[0] <= '6')
			{
				id = id + (rdbuf[0] - '0');
			}
			if (id)
				KillProcess(id);

		}
		else if (!strcmp(rdbuf, "clear"))
		{
			clear();
			ShowCommands();
		}
		else if (!strcmp(rdbuf, "help"))
		{
			ShowCommands();
		}
		else if (!strcmp(rdbuf, "exit"))
		{
			break;
		}
		else
		{
			printf("Sorry, we can not find your option.You can input \"help\" to get the instruction.\n");
		}
	}
}

//show process
void ProcessInfo()
{
	printf("=============================================================================\n");
	printf("     ProcessID     |     ProcessName     |     Priority     |     Running?\n");
	//                进程号                进程名                    优先级            是否是系统进程，是否在运行
	printf("-----------------------------------------------------------------------------\n");
	int i;
	for (i = 0; i < NR_TASKS + NR_PROCS; ++i) //逐个遍历
	{
		printf("       %d       |         %s         |        %d        |         %s        \n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority, proc_table[i].priority == 0 ? "NO" : "YES");
	}
	printf("=============================================================================\n");
}

//help
void ShowCommands()
{
	printf("      ====================================================================\n");
	printf("      #                                DOGE                              #\n");
	printf("      #                                                                  #\n");
	printf("      #                           PROCESS MANAGER                        #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #             [COMMAND]                 [FUNCTION]                 #\n");
	printf("      #                                                                  #\n");
	printf("      #           $ ps           |     show all process information      #\n");
	printf("      #           $ kill         |     kill a process                    #\n");
	printf("      #           $ create       |     create a new process              #\n");
	printf("      #           $ help         |     show command list of this system  #\n");
	printf("      #           $ clear        |     clear the cmd                     #\n");
	printf("      #           $ exit         |     exit process management system    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                       Powered by chuckle                         #\n");
	printf("      #                       ALL RIGHT REVERSED                         #\n");
	printf("      ====================================================================\n");

	printf("\n\n");
}

//create a new process with id
void CreateProcess(int id)
{
	if (id < 0 || id >= NR_TASKS + NR_PROCS)
	{
		printf("Sorry,we can not find such process.\n");
		return;
	}
	else if (proc_table[id].priority != 0)
	{
		printf("The Process is already running.\n");
		return;
	}
	else if (proc_table[id].priority == 0)
	{
		proc_table[id].priority = 5;
		printf("The Process begins to running.\n");
		return;
	}
	return;
}

//kill a process with id
void KillProcess(int id)
{
	if (id < 0 || id >= NR_TASKS + NR_PROCS)
	{
		printf("Sorry,we can not find such process.\n");
		return;
	}
	else if (proc_table[id].priority == 0)
	{
		printf("The Process is already stopped.\n");
		return;
	}
	else if(id>0&&id<4)
	{
		printf("The Process is a system process.You can not kill it.\n");
	}
	else if(id==4)
	{
		printf("The Process is our main process.You can not kill it.\n");
	}
	else if (proc_table[id].priority > 0)
	{
		proc_table[id].priority = 0;
		printf("The Process is killed.\n");
		return;
	}
	return;
}


void commands() {
	printf("\n");
	printf("                      =============================== \n");
	printf("                               WELCOME DOGE OS                   \n");
	printf("                                   V 5.6              \n");
	printf("                      =============================== \n");

	printf("          ===================================================== \n");
	printf("          #                        MENU                       # \n");
	printf("          #                                                   #\n");
	printf("          #         [COMMAND]                 [FUNCTION]      #\n");
	printf("          #                                                   #\n");
	printf("          #       $ calendar       |     Open calendar        #\n");
	printf("          #       $ timer1         |     Open 1# timer        #\n");
	printf("          #       $ timer2         |     Open 2# timer        #\n");
	printf("          #       $ calcu          |     Open Calculator      #\n");
	printf("          #       $ game           |     Go to game square    #\n");
	printf("          #       $ fs             |     Open file system     #\n");
	printf("          #       $ pm             |     Open process manager #\n");
	printf("          #       $ clear          |     Clear screen         #\n");
	printf("          #       $ help           |     Show commands        #\n");
	printf("          #       $ about          |     About DOGE           #\n");
	printf("          #                                                   # \n");
	printf("          #                     DOGE R&D TEAM                 # \n");
	printf("          #                  ALL RIGHT REVERSED               # \n");
	printf("          ===================================================== \n");
	printf("\n");
}

void about() {
	printf("\n");
	printf("          =========================DOGE======================== \n");
	printf("          #                                                   #\n");
	printf("          #  DOGE R&D TEAM:                                   # \n");
	printf("          #      Yuanfeng Li                                  # \n");
	printf("          #      Ziyi Zhou                                    # \n");
	printf("          #      Ziyuan Lv                                    # \n");
	printf("          #      Xian He                                      # \n");
	printf("          #      Mei Du                                       # \n");
	printf("          #                                                   #\n");
	printf("          #  Historical Version:                              # \n");
	printf("          #      V 1.0 +kernal                                #\n");
	printf("          #      V 2.0 +I/O,+tty                              #\n");
	printf("          #      V 3.0 +filesystem,+game                      #\n");
	printf("          #      V 4.0 +game,+calender                        #\n");
	printf("          #      V 5.0 +game,+process manager                 #\n");
	printf("          #                                                   #\n");
	printf("          #             Copyright (c) DOGE R&D TEAM           #\n");
	printf("          #                  ALL RIGHT REVERSED               # \n");
	printf("          =========================DOGE======================== \n");
	printf("\n");
}

/*======================================================================*
							  DOGE FILE SYSTEM
 *======================================================================*/
 //�༶�ļ�ϵͳ
#define MAX_FILE_PER_LAYER 10
#define MAX_FILE_NAME_LENGTH 20
#define MAX_CONTENT_ 50
#define MAX_FILE_NUM 100

struct fBlock {
	int fID;
	char fName[MAX_FILE_NAME_LENGTH];
	int fType;//0��ͨ�ļ���1�ļ��� 
	char content[MAX_CONTENT_];
	int fatherID;
	int children[MAX_FILE_PER_LAYER];//���ļ�
	int childrenNumber;//���ļ���Ŀ
};

struct fBlock blocks[MAX_FILE_NUM];//�ļ���

int IDLog[MAX_FILE_NUM];//idĿ¼

int currentFileID = 0;//��ǰ����λ��

int fileNum = 0;//��ǰ�ļ�����

void FsShowCmd() {
	printf("\n");
	printf("          ===================================================== \n");
	printf("          #                         DOGE                      # \n");
	printf("          #                                                   #\n");
	printf("          #                     FILE MANAGER                  # \n");
	printf("          #                                                   #\n");
	printf("          #         [COMMAND]                 [FUNCTION]      #\n");
	printf("          #                                                   #\n");
	printf("          #       $ touch [name]   |     Create a file        #\n");
	printf("          #       $ mkdir [name]   |     Create a folder      #\n");
	printf("          #       $ cd [name]      |     Enter sub path       #\n");
	printf("          #       $ bk             |     Return superior path #\n");
	printf("          #       $ ls             |     Show file list       #\n");
	printf("          #       $ rm [name]      |     Remove file          #\n");
	printf("          #       $ sv             |     Save to disk         #\n");
	printf("          #       $ clear          |     Clear screen         #\n");
	printf("          #       $ help           |     Show commands        #\n");
	printf("          #       $ quit           |     quit FILE MANAGER    #\n");
	printf("          #                                                   # \n");
	printf("          #             Supported by DOGE R&D TEAM            # \n");
	printf("          #                  ALL RIGHT REVERSED               # \n");
	printf("          ===================================================== \n");
	printf("\n");
}

/*�ڵ�ǰ�ļ����ʼ������*/
void InitBlock(int fileID, char* fileName, int fileType) {
	blocks[fileID].fID = fileID;
	strcpy(blocks[fileID].fName, fileName);
	blocks[fileID].fType = fileType;
	blocks[fileID].fatherID = currentFileID;
	blocks[fileID].childrenNumber = 0;
}

/*����λ��intת����string��������λ�����Զ���0*/
void toStr3(char* temp, int i) {
	if (i / 100 < 0)
		temp[0] = (char)48;
	else
		temp[0] = (char)(i / 100 + 48);
	if ((i % 100) / 10 < 0)
		temp[1] = '0';
	else
		temp[1] = (char)((i % 100) / 10 + 48);
	temp[2] = (char)(i % 10 + 48);
}

/*д����*/
void WriteDisk(int len) {
	//�ļ�ϵͳ��������Ϣ������temp�ַ����У���tempд������
	char temp[MAX_FILE_NUM * 150 + 103];
	int i = 0;
	temp[i] = '^';
	i++;
	//��¼�ļ���Ŀ
	toStr3(temp + i, fileNum);
	i = i + 3;
	temp[i] = '^';
	i++;
	for (int j = 0; j < MAX_FILE_NUM; j++) {
		if (IDLog[j] == 1) {
			toStr3(temp + i, blocks[j].fID);
			i = i + 3;
			temp[i] = '^';
			i++;
			for (int h = 0; h < strlen(blocks[j].fName); h++) {
				temp[i + h] = blocks[j].fName[h];
				if (blocks[j].fName[h] == '^')
					temp[i + h] = (char)1;
			}
			i = i + strlen(blocks[j].fName);
			temp[i] = '^';
			i++;
			temp[i] = (char)(blocks[j].fType + 48);//48Ϊ0��49Ϊ1
			i++;
			temp[i] = '^';
			i++;
			for (int h = 0; h < strlen(blocks[j].content); h++) {
				temp[i + h] = blocks[j].content[h];
				if (blocks[j].content[h] == '^')
					temp[i + h] = (char)1;
			}
			i = i + strlen(blocks[j].content);
			temp[i] = '^';
			i++;
			toStr3(temp + i, blocks[j].fatherID);
			i = i + 3;
			temp[i] = '^';
			i++;
			for (int m = 0; m < MAX_FILE_PER_LAYER; m++) {
				toStr3(temp + i, blocks[j].children[m]);
				i = i + 3;
			}
			temp[i] = '^';
			i++;
			toStr3(temp + i, blocks[j].childrenNumber);
			i = i + 3;
			temp[i] = '^';
			i++;
		}
	}
	int fd = 0;
	int n1 = 0;
	fd = open("ss", O_RDWR);
	assert(fd != -1);
	n1 = write(fd, temp, strlen(temp));
	assert(n1 == strlen(temp));
	close(fd);
}

/*��֧����λת����atoi*/
int toInt(char* temp) {
	int result = 0;
	for (int i = 0; i < 3; i++)
		result = result * 10 + (int)temp[i] - 48;
	return result;
}

/*�����̺���*/
int ReadDisk() {
	char bufr[1000];
	int fd = 0;
	int n1 = 0;
	fd = open("ss", O_RDWR);
	assert(fd != -1);
	n1 = read(fd, bufr, 1000);
	bufr[n1] = 0;
	int r = 1;//ϵͳ�ļ��±�
	fileNum = toInt(bufr + r);
	r = r + 4;
	for (int i = 0; i < fileNum; i++) {
		int ID = toInt(bufr + r);
		IDLog[ID] = 1;
		blocks[ID].fID = ID;
		r = r + 4;
		for (int i = 0; i < MAX_FILE_NAME_LENGTH; i++) {
			if (bufr[r] == '^')
				break;
			else if (bufr[r] == (char)1)
				blocks[ID].fName[i] = '^';
			else
				blocks[ID].fName[i] = bufr[r];
			r++;
		}
		r++;
		blocks[ID].fType = (int)bufr[r] - 48;
		r = r + 2;
		for (int j = 0; j < MAX_CONTENT_; j++) {
			if (bufr[r] == '^')
				break;
			else if (bufr[r] == (char)1)
				blocks[ID].content[j] = '^';
			else
				blocks[ID].content[j] = bufr[r];
			r++;
		}
		r++;
		blocks[ID].fatherID = toInt(bufr + r);
		r = r + 4;
		for (int j = 0; j < MAX_FILE_PER_LAYER; j++) {
			blocks[ID].children[j] = toInt(bufr + r);
			r = r + 3;
		}
		r++;
		blocks[ID].childrenNumber = toInt(bufr + r);
		r = r + 4;
	}
	return n1;
}

/*�ļ�ϵͳ��ʼ��������һ�����ļ�*/
void FSinit() {
	/*�ļ����ʼ��*/
	for (int i = 0; i < MAX_FILE_NUM; i++) {
		blocks[i].childrenNumber = 0;
		blocks[i].fID = -2;
		IDLog[i] = 0;
	}
	IDLog[0] = 1;
	blocks[0].fID = 0;
	strcpy(blocks[0].fName, "home");
	blocks[0].fType = 2;
	blocks[0].fID = 0;
	blocks[0].childrenNumber = 0;
	currentFileID = 0;
	fileNum = 1;
}

int CreateFIle(char* fileName, int fileType) {
	if (blocks[currentFileID].childrenNumber == MAX_FILE_PER_LAYER) {
		printf("Sorry you cannot add more files in this layer.\n");
		return 0;
	}
	else {
		for (int i = 0; i < MAX_FILE_PER_LAYER; i++) {
			if (blocks[currentFileID].children[i] != -1 && blocks[currentFileID].children[i] != 0) {
				if (strcmp(blocks[blocks[currentFileID].children[i]].fName, fileName) == 0) {
					if (fileType) {
						printf("A folder with the same name already exists.\n");
					}
					else {
						printf("A file with the same name already exists.\n");
					}
					return 0;
				}
			}
		}
		//�����ɹ�
		fileNum++;
		int target = 0;
		//Ѱ��һ���յ��ļ���
		for (int i = 0; i < MAX_FILE_NUM; i++) {
			if (IDLog[i] == 0) {
				target = i;
				break;
			}
		}
		InitBlock(target, fileName, fileType);
		for (int i = 0; i < MAX_FILE_PER_LAYER; i++) {
			if (blocks[currentFileID].children[i] == -1 || blocks[currentFileID].children[i] == 0) {
				blocks[currentFileID].children[i] = target;
				break;
			}
		}
		//blocks[currentFileID].children[blocks[currentFileID].childrenNumber] = target;
		blocks[currentFileID].childrenNumber++;
		if (fileType) {
			printf("Folder [%s] created successfuly.\n", fileName);
		}
		else {
			printf("File [%s] created successfully.\n", fileName);
		}
		IDLog[target] = 1;
		return 1;
	}
}

/*չʾ��ǰ·���������ļ�*/
void showFileList() {
	printf("[%s] File List:", blocks[currentFileID].fName);
	printf("\n");
	printf("                      ============================ \n");
	printf("                         NAME  |   TYPE   |  ID  \n");
	for (int i = 0; i < MAX_FILE_PER_LAYER; i++) {	//����ÿ������
		if (blocks[currentFileID].children[i] != 0 && blocks[currentFileID].children[i] != -1) {
			printf("%30s", blocks[blocks[currentFileID].children[i]].fName);
			if (blocks[blocks[currentFileID].children[i]].fType == 0) {
				printf(" |   .TXT   |");
			}
			else {
				printf(" |  .FOLDER |");
			}
			printf("%3d\n", blocks[blocks[currentFileID].children[i]].fID);
		}
	}
	printf("                      ============================ \n\n");
}

/*�����ļ�*/
int SearchFile(char* name) {
	for (int i = 0; i < blocks[currentFileID].childrenNumber; i++) {
		if (strcmp(name, blocks[blocks[currentFileID].children[i]].fName) == 0) {
			return blocks[currentFileID].children[i];
		}
	}
	return -2;
}

void ReturnFile(int ID) {
	currentFileID = blocks[ID].fatherID;
}

void DeleteFile(int ID) {
	if (blocks[ID].childrenNumber > 0) {
		for (int i = 0; i < blocks[ID].childrenNumber; i++) {
			DeleteFile(blocks[blocks[ID].children[i]].fID);
		}
	}
	IDLog[ID] = 0;
	blocks[ID].fID = -2;
	blocks[ID].childrenNumber = 0;
	for (int i = 0; i < blocks[blocks[ID].fatherID].childrenNumber; i++) {
		if (blocks[blocks[ID].fatherID].children[i] == ID) {
			blocks[blocks[ID].fatherID].children[i] = -1;
		}
	}
	blocks[blocks[ID].fatherID].childrenNumber--;
	for (int i = 0; i < MAX_CONTENT_; i++)
		blocks[ID].content[i] = '\0';
	for (int i = 0; i < MAX_FILE_NAME_LENGTH; i++)
		blocks[ID].fName[i] = '\0';
	blocks[ID].fType = -1;
	for (int i = 0; i < MAX_FILE_PER_LAYER; i++)
		blocks[ID].children[i] = -1;
	blocks[ID].fatherID = -2;
	fileNum--;
}

/*�ļ�ϵͳ���̺���*/
void filemanagerprocess(int fd_stdin) {
	//��ʼ��
	char readbuffer[128];
	char cmd[8];
	char filename[120];
	char _name[MAX_FILE_NAME_LENGTH];
	FSinit();
	int fd = open("ss", O_CREAT | O_RDWR);
	int len = ReadDisk();
	FsShowCmd();

	while (1) {

		//��ʼ���ַ���
		memset(readbuffer, 0x00, sizeof(char) * 128);
		memset(_name, 0x00, sizeof(char)*MAX_FILE_NAME_LENGTH);

		//չʾ��ǰ·��
		int idstack[100];
		for (int i = 0; i < 100; i++) {
			idstack[i] = -1;
		}
		int father = currentFileID;
		int j = -1;
		while (blocks[father].fID != 0) {
			j++;
			idstack[j] = father;
			father = blocks[father].fatherID;
		}
		printf("home");
		for (j; j >= 0; j--) {
			printf("/%s", blocks[idstack[j]].fName);
		}
		printf(" $ ");

		//��ȡ����
		int r = read(fd_stdin, readbuffer, 70);
		readbuffer[r] = 0;
		assert(fd_stdin == 0);

		//����������д���readbuffer��
		//���readbuffer
		//touchָ��
		if (readbuffer[0] == 't' && readbuffer[1] == 'o' && readbuffer[2] == 'u' && readbuffer[3] == 'c' && readbuffer[4] == 'h') {
			if (readbuffer[5] != ' ') {//�����ļ�û�и���ָ����
				printf("The format of the command you entered is incorrect.\n");
				printf("Maybe you want to use the $ touch command, the correct format is \"touch filename\".\n");
				printf("You can input $ help to know more.\n");
				continue;
			}
			//�ӵ�6λ��ʼ�����ζ�ȡ���뻺����ļ���
			for (int i = 0; i < MAX_FILE_NAME_LENGTH && i + 6 < r; i++) {
				_name[i] = readbuffer[i + 6];
			}
			//����һ����ͨ�ļ�
			CreateFIle(_name, 0);
			continue;
		}
		//mkdirָ��
		if (readbuffer[0] == 'm' && readbuffer[1] == 'k' && readbuffer[2] == 'd' && readbuffer[3] == 'i' && readbuffer[4] == 'r') {
			if (readbuffer[5] != ' ') {//�����ļ�û�и���ָ����
				printf("The format of the command you entered is incorrect.\n");
				printf("Maybe you want to use the $ mkdir command, the correct format is \"mkdir dirname\".\n");
				printf("You can input $ help to know more.\n");
				continue;
			}
			//�ӵ�6λ��ʼ�����ζ�ȡ���뻺�������ļ���
			for (int i = 0; i < MAX_FILE_NAME_LENGTH && i + 6 < r; i++) {
				_name[i] = readbuffer[i + 6];
			}
			//����һ���ļ���
			CreateFIle(_name, 1);
			continue;
		}
		//lsָ��
		if (strcmp(readbuffer, "ls") == 0) {
			showFileList();
			continue;
		}
		//cdָ��
		if (readbuffer[0] == 'c'&&readbuffer[1] == 'd') {
			if (readbuffer[2] != ' ') {
				printf("The format of the command you entered is incorrect.\n");
				printf("Maybe you want to use the $ cd command, the correct format is \"cd name\".\n");
				printf("You can input $ help to know more.\n");
				continue;
			}
			for (int i = 0; i < MAX_FILE_NAME_LENGTH && i + 3 < r; i++) {
				_name[i] = readbuffer[i + 3];
			}
			int ID = SearchFile(_name);
			if (ID >= 0) {
				if (blocks[ID].fType == 1) {
					currentFileID = ID;
					continue;
				}
				else {
					while (1) {
						printf("You have opened a TXT file\n");
						printf("You can use the following commands to manipulate the file:\n");
						printf("$ u --- update the file content\n");
						printf("$ c --- check the file content\n");
						printf("$ q --- close the file\n");
						int idstack[100];
						for (int i = 0; i < 100; i++) {
							idstack[i] = -1;
						}
						int father = currentFileID;
						int j = -1;
						while (blocks[father].fID != 0) {
							j++;
							idstack[j] = father;
							father = blocks[father].fatherID;
						}
						printf("home");
						for (j; j >= 0; j--) {
							printf("/%s", blocks[idstack[j]].fName);
						}
						printf("/%s", blocks[ID].fName);
						printf(" $ ");
						//�������뻺����
						int r = read(fd_stdin, readbuffer, 70);
						readbuffer[r] = 0;
						if (strcmp(readbuffer, "u") == 0) {
							printf("Please enter the updated content:\n");
							printf("(The maximum number of characters supported by DOGE is 50)\n");
							int r = read(fd_stdin, blocks[ID].content, MAX_CONTENT_);
							blocks[ID].content[r] = 0;
							printf("Update complete.\n");
							continue;
						}
						if (strcmp(readbuffer, "c") == 0) {
							printf("\n");
							printf("             =======================DOGE======================= \n");
							printf("             %s\n", blocks[ID].content);
							printf("             =======================DOGE======================= \n");
							printf("\n");
							continue;
						}
						if (strcmp(readbuffer, "q") == 0) {
							break;
						}
						printf("The format of the command you entered is incorrect.\n");
					}
					continue;
				}
			}
			else {
				printf("There is no such folder or file in current directory.\n");
				continue;
			}
		}
		//rmָ��
		if (readbuffer[0] == 'r'&&readbuffer[1] == 'm') {
			if (readbuffer[2] != ' ') {
				printf("The format of the command you entered is incorrect.\n");
				printf("Maybe you want to use the $ rm command, the correct format is \"rm name\".\n");
				printf("You can input $ help to know more.\n");
				continue;
			}
			for (int i = 0; i < MAX_FILE_NAME_LENGTH && i + 3 < r; i++) {
				_name[i] = readbuffer[i + 3];
			}
			int ID = SearchFile(_name);
			if (ID > 0) {
				DeleteFile(ID);
				printf("The file or folder has been successfully removed.\n");
				continue;
			}
			else {
				printf("There is no such folder or file in current directory.\n");
				continue;
			}
		}
		//svָ��
		if (strcmp(readbuffer, "sv") == 0) {
			WriteDisk(1000);
			printf("Save to disk successfully.\n");
			continue;
		}
		//helpָ��
		if (strcmp(readbuffer, "help") == 0) {
			FsShowCmd();
			continue;
		}
		//quitָ��
		if (strcmp(readbuffer, "quit") == 0) {
			clear();
			//����ѭ���˳�ϵͳ
			break;
		}
		//clearָ��
		if (strcmp(readbuffer, "clear") == 0) {
			clear();
			continue;
		}
		//bkָ��
		if (strcmp(readbuffer, "bk") == 0) {
			currentFileID = blocks[currentFileID].fatherID;
			continue;
		}
		printf("The format of the command you entered is incorrect.\n");
		printf("You can input $ help to know more.\n");
		continue;
	}
}

void gamesquare() {
	printf("\n");
	printf("                      =============================== \n");
	printf("                               0.0 DOGE 0.0                   \n");
	printf("                             !!!GAME SQUARE!!!       \n");
	printf("                      =============================== \n");

	printf("          ===================================================== \n");
	printf("          #                    HAVE A GOOD TIME!              # \n");
	printf("          #                                                   #\n");
	printf("          #         [COMMAND]                 [GAME]          #\n");
	printf("          #                                                   #\n");
	printf("          #       $ 1              |     Guess number         #\n");
	printf("          #       $ 2              |     Push box             #\n");
	printf("          #       $ 3              |     Five-in-a-row        #\n");
	printf("          #       $ 4              |     Mine clearance       #\n");
	printf("          #       $ 5              |     2048 game            #\n");
	printf("          #       $ 6              |     Rock-paper-scissors  #\n");
	printf("          #       $ clear          |     Clear screen         #\n");
	printf("          #       $ help           |     Show games           #\n");
	printf("          #       $ quit           |     exit GAME SQUARE     #\n");
	printf("          #                                                   # \n");
	printf("          #                     DOGE R&D TEAM                 # \n");
	printf("          #                  ALL RIGHT REVERSED               # \n");
	printf("          ===================================================== \n");
	printf("\n");
}

void Booting() {
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                              *    *                              #\n");
	printf("      #                             ********                             #\n");
	printf("      #                            **      **                            #\n");
	printf("      #                             ********                             #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                         *             *                          #\n");
	printf("      #                       *****         *****                        #\n");
	printf("      #                      *********************                       #\n");
	printf("      #                     **                   **                      #\n");
	printf("      #                    **                     **                     #\n");
	printf("      #                     **                   **                      #\n");
	printf("      #                      *********************                       #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                       *                  *                       #\n");
	printf("      #                      ***                ***                      #\n");
	printf("      #                     *****              *****                     #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                   **                        **                   #\n");
	printf("      #                  **                          **                  #\n");
	printf("      #                 **                            **                 #\n");
	printf("      #                **                              **                #\n");
	printf("      #                 **                            **                 #\n");
	printf("      #                  **                          **                  #\n");
	printf("      #                   **                        **                   #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                       *                  *                       #\n");
	printf("      #                      ***                ***                      #\n");
	printf("      #                     *****              *****                     #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                   **  ******       ******   **                   #\n");
	printf("      #                  **   *    *       *    *    **                  #\n");
	printf("      #                 **    ******       ******     **                 #\n");
	printf("      #                **            *****             **                #\n");
	printf("      #                 **            ***             **                 #\n");
	printf("      #                  **        *********         **                  #\n");
	printf("      #                   **       *       *        **                   #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #            #######                                               #\n");
	printf("      #            ##    ##                                              #\n");
	printf("      #            ##     ##                                             #\n");
	printf("      #            ##    ##                                              #\n");
	printf("      #            #######                                               #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                       *                  *                       #\n");
	printf("      #                      ***                ***                      #\n");
	printf("      #                     *****              *****                     #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                   **                        **                   #\n");
	printf("      #                  **    $$$$          $$$$    **                  #\n");
	printf("      #                 **                            **                 #\n");
	printf("      #                **            *****             **                #\n");
	printf("      #                 **            ***             **                 #\n");
	printf("      #                  **            *             **                  #\n");
	printf("      #                   **           *            **                   #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                        ########                                  #\n");
	printf("      #                        ##    ##                                  #\n");
	printf("      #                        ##    ##                                  #\n");
	printf("      #                        ##    ##                                  #\n");
	printf("      #                        ########                                  #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                       *                  *                       #\n");
	printf("      #                      ***                ***                      #\n");
	printf("      #                     *****              *****                     #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                   **                        **                   #\n");
	printf("      #                  **   ****           ****    **                  #\n");
	printf("      #                 **     **             **      **                 #\n");
	printf("      #                **      **    *****    **       **                #\n");
	printf("      #                 **     **     ***     **      **                 #\n");
	printf("      #                  **    **      *      **     **                  #\n");
	printf("      #                   **           *            **                   #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                   ########                       #\n");
	printf("      #                                   ##                             #\n");
	printf("      #                                   ##   ###                       #\n");
	printf("      #                                   ##    ##                       #\n");
	printf("      #                                   ########                       #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                       *                  *                       #\n");
	printf("      #                      ***                ***                      #\n");
	printf("      #                     *****              *****                     #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                   **   **             **    **                   #\n");
	printf("      #                  **   *  *           *  *    **                  #\n");
	printf("      #                 **   *    *         *    *    **                 #\n");
	printf("      #                **            *****             **                #\n");
	printf("      #                 **            ***             **                 #\n");
	printf("      #                  **         **   **          **                  #\n");
	printf("      #                   **          ***           **                   #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #                                              ########            #\n");
	printf("      #                                              ##                  #\n");
	printf("      #                                              ########            #\n");
	printf("      #                                              ##                  #\n");
	printf("      #                                              ########            #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
	clear();
	printf("\n");
	printf("      ====================================================================\n");
	printf("      #                                                                  #\n");
	printf("      #                       *                  *                       #\n");
	printf("      #                      ***                ***                      #\n");
	printf("      #                     *****              *****                     #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                   **                        **                   #\n");
	printf("      #                  ******************************                  #\n");
	printf("      #                 **   ******         ******    **                 #\n");
	printf("      #                **            *****             **                #\n");
	printf("      #                 **            ***             **                 #\n");
	printf("      #                  **            *             **                  #\n");
	printf("      #                   **           *            **                   #\n");
	printf("      #                    **************************                    #\n");
	printf("      #                                                                  #\n");
	printf("      #                                                                  #\n");
	printf("      #            #######     ########   ########   ########            #\n");
	printf("      #            ##    ##    ##    ##   ##         ##                  #\n");
	printf("      #            ##     ##   ##    ##   ##   ###   ########            #\n");
	printf("      #            ##    ##    ##    ##   ##    ##   ##                  #\n");
	printf("      #            #######     ########   ########   ########            #\n");
	printf("      #                                                                  #\n");
	printf("      ====================================================================\n");
	printf("\n");
	milli_delay(20000);
}
