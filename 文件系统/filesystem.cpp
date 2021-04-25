#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>
#include<iomanip>
using namespace std;

//磁盘容量20MB 块大小为4kb 共5*1024=5120块
//inode大小为128B 每块有32个索引，共20480个索引
//#1 mainblock 卷总信息
//#2-#641 inode表
//#642 inode位示图 
//#643 block位示图 
//#644-#5119 数据块 4476块
#define VOLUME 20971520
#define BLOCK_SIZE 4096
#define BLOCK_NUM 5120
#define INODE_NUM 20480
#define DATABLOCK_NUM 4476

//卷总信息
class Mainblock 
{
public:
	const int volume = VOLUME;//磁盘文件卷的大小
	const int inode_num = INODE_NUM ;//文件索引表的大小
	const int block_size = BLOCK_SIZE;//物理数据块大小
	int freeblock_num;//空闲块数量
	Mainblock()
	{
		freeblock_num = DATABLOCK_NUM;
	}
};

//文件
class Inode 
{
public:
	string name;//文件名
	int node;//文件类型， -1为空，0为目录，1为文件
	int number;//文件号
	string filetype;//文件类型(node==1)
	int address;//文件地址
	int length;//文件大小
	vector<int>blockused;
	vector<int>Inode_num;//该目录下文件(node==0)
	int parentinode;//父节点

	Inode(int num)
	{
		DeleteInode(num);
	}

	void DeleteInode(int num)
	{
		name = "";
		node = -1;
		number = num;
		filetype = "";
		address = -1;
		length = 0;
		blockused = vector<int>();
		Inode_num = vector<int>();
		parentinode = -1;
	}
};

//数据块
class Block
{
public:
	vector<char>data;

	Block()
	{
		data = vector<char>();
	}
};

//文件处理
class FileManager
{
public:
	Mainblock mainblock;//卷总信息
	vector<int>inodebitmap;//文件位视图
	vector<int>blockbitmap;//块位视图
	vector<Inode*>inodetable;//文件表
	vector<Block*>blocktable;//块表

	int current;//当前目录

	FileManager()
	{
		Format();
		readlocaldata();
		//for (int i = 0; i < INODE_NUM; i++)
		//	cout<<inodebitmap[i];
	}

	//格式化
	bool Format()
	{
		//初始化
		mainblock.freeblock_num = DATABLOCK_NUM;

		inodebitmap.clear();
		for (int i = 0; i < INODE_NUM; i++)
			inodebitmap.push_back(0);

		blockbitmap.clear();
		for (int i = 0; i < DATABLOCK_NUM; i++)
			blockbitmap.push_back(0);		

		inodetable.clear();
		for (int i = 0; i < INODE_NUM; i++)
		{
			Inode*temp = new Inode(i);
			inodetable.push_back(temp);
		}


		blocktable.clear();
		for (int i = 0; i < DATABLOCK_NUM; i++)
		{
			Block* temp = new Block();
			blocktable.push_back(temp);
		}


		Inode* root = inodetable[0];
		inodebitmap[0] = 1;
		root->name = "root";
		root->node = 0;
		root->filetype = "dir";
		root->Inode_num = vector<int>();
		root->parentinode = -1;


		current = 0;
		return true;
	}
	
	//显示当前目录名称
	bool showdirname()
	{
		Inode* parent_inode = inodetable[current];
		cout << parent_inode->name;
		return true;
	}

	//查找空索引
	int findfreeinode()
	{
		for (int i = 0; i < INODE_NUM; i++)
			if (inodebitmap[i] == 0)
				return i;
		return -1;
	}

	//创建子目录
	bool createsubdir(string dirname)
	{
		int temp = findfreeinode();
		if (temp == -1)
		{
			cout << "缓存已满，请删除一些缓存" << endl;
			return false;
		}
		else if (temp == 0)//文件为空
		{
			Format();
			return false;
		}
		else 
		{
			Inode* temp_inode = inodetable[temp];
			inodebitmap[temp] = 1;
			temp_inode->name = dirname;
			temp_inode->node = 0;
			temp_inode->filetype = "dir";
			temp_inode->Inode_num = vector<int>();
			temp_inode->parentinode = current;

			Inode* parent_inode = inodetable[current];
			parent_inode->Inode_num.push_back(temp);
		}
	}

	//查找文件
	int findfile(string filename,string filetype)
	{
		Inode* parent_inode = inodetable[current];
		for (int i = 0; i < parent_inode->Inode_num.size(); i++)
		{
			Inode* temp_inode = inodetable[parent_inode->Inode_num[i]];
			if (filename == temp_inode->name && filetype == temp_inode->filetype)
				return parent_inode->Inode_num[i];
		}
		return -1;
	}

	//删除子目录
	bool deletesubdir(string dirname)
	{
		int temp = findfile(dirname,"dir");//文件索引
		if (temp == -1)
		{
			cout << "该目录下无此目录！"<<endl;
			return false;
		}
		else
		{
			inodebitmap[temp] = 0;
			Inode* temp_inode = inodetable[temp];
			Inode* parent_inode = inodetable[temp_inode->parentinode];

			//在父目录下删除子文件索引
			for (int i = 0; i < parent_inode->Inode_num.size(); i++)
			{
				if (parent_inode->Inode_num[i] == temp)
				{
					parent_inode->Inode_num.erase(parent_inode->Inode_num.begin() + i);
					break;
				}
			}
			//删除本目录中文件以及子目录
			for (int i = 0; i < temp_inode->Inode_num.size(); i++)
			{
				Inode* del_inode = inodetable[temp_inode->Inode_num[i]];
				if (del_inode->node == 0)
					deletesubdir(del_inode->name);
				else if (del_inode->node == 1)
					deletefile(del_inode->name,del_inode->filetype);
			}			
			temp_inode->DeleteInode(temp);

		}
		return true;
	}

	//显示目录
	bool showdir()
	{
		Inode* parent_inode = inodetable[current];
		if (parent_inode->Inode_num.empty())
		{
			cout << "本目录下没有文件！" << endl;
			return false;
		}
		cout << parent_inode->name<<"目录下文件为：" << endl;
		for (int i = 0; i < parent_inode->Inode_num.size(); i++)
		{
			Inode* temp_inode = inodetable[parent_inode->Inode_num[i]];
			if (temp_inode->node == 0)
				cout << temp_inode->name << endl;
			else if (temp_inode->node == 1)
				cout << temp_inode->name << '.' << temp_inode->filetype << endl;
			else if (temp_inode->node == -1)
				return false;
		}
		return true;
	}

	//更改当前目录
	bool gotodir(string dirname)
	{
		int temp = findfile(dirname,"dir");
		if (temp == -1)
		{
			cout << "该目录下无此目录！" << endl;
			return false;
		}
		else
		{
			current = temp;
		}
		return true;
	}

	//回到父目录
	bool backdir()
	{
		if (current == 0)
		{
			cout << "已在根目录！" << endl;
			return false;
		}
		Inode* parent_inode = inodetable[current];
		current = parent_inode->parentinode;
		return true;
	}

	//创建文件
	bool createfile(string filename,string type_name)
	{
		int temp = findfreeinode();
		if (temp == -1)
		{
			cout << "缓存已满，请删除一些缓存" << endl;
			return false;
		}
		else if (temp == 0)
		{
			return false;
		}
		else
		{
			Inode* temp_inode = inodetable[temp];
			inodebitmap[temp] = 1;
			temp_inode->name = filename;
			temp_inode->node = 1;
			temp_inode->filetype = type_name;
			temp_inode->parentinode = current;

			Inode* parent_inode = inodetable[current];
			parent_inode->Inode_num.push_back(temp);
		}
	}

	//打开文件
	bool open(string filename, string type_name)
	{
		int temp = findfile(filename, type_name);
		if (temp == -1)
		{
			cout << "目录下无" <<filename<<'.'<<type_name<<'!'<< endl;
			return false;
		}
		else if (temp >= 0)//进行读、写、关闭操作
		{
			current = temp;
			Inode* temp_inode = inodetable[temp];
			bool flag = true;
			while (flag)
			{
				string r_w;
				cout << "read/write/close:";
				cin >> r_w;
				if (r_w == "read" || r_w == "READ" || r_w == "Read")
				{
					read();
				}
				else if (r_w == "write" || r_w == "WRITE" || r_w == "Write")
				{
					write();

				}
				else if (r_w == "close" || r_w == "CLOSE" || r_w == "Close")
				{
				
					close();
					flag = false;
				}
				else {
					cout << "请输入正确的读写指令！" << endl;
				}
			}
			
		}
		
	}
	
	//关闭文件
	bool close()
	{
		Inode* temp_inode = inodetable[current];
		if (temp_inode->parentinode < 0)
			return false;
		current = temp_inode->parentinode;
		return true;
	}

	//读文件
	bool read()
	{
		//输出文件数据块中内容
		Inode* temp_inode = inodetable[current];
		for (int i = 0; i < temp_inode->blockused.size(); i++)
		{
			Block* temp_block = blocktable[temp_inode->blockused[i]];
			for (int j = 0; j < temp_block->data.size(); j++)
				cout << temp_block->data[j];
		}
		cout << endl;
		return true;
	}

	//查找空数据块
	int findfreeblock()
	{
		for (int i = 0; i < BLOCK_NUM; i++)
			if (blockbitmap[i] == 0)
				return i;
		return -1;
	}

	//写文件
	bool write()
	{
		cout << "(EOF=CTRL+Z，请按CTRL+Z结束输入)" << endl;//提示
		char temp;
		Inode* temp_inode = inodetable[current];
		vector<char> temp_data = vector<char>();
		while ((temp = getchar()) != EOF)//EOF=ctrl+Z
		{
			temp_data.push_back(temp);
		}
		int data_num = temp_data.size();
		int block_num = data_num / BLOCK_SIZE + 1;//计算需要数据块数量
		if (block_num > mainblock.freeblock_num)
		{
			cout << "磁盘空间不足!" << endl;
			return false;
		}
		int i = 0;
		//对数据进行写入
		while (block_num != 0)
		{
			int freenum = findfreeblock();
			char extra = 0;
			Block* temp_block = blocktable[freenum];
			for (int a = 0; a < BLOCK_SIZE; a++)
			{
				if (i >= temp_data.size())
				{
					temp_block->data.push_back(extra);
					break;
				}
				if(a==BLOCK_SIZE-1)
					temp_block->data.push_back(extra);
				else
					temp_block->data.push_back(temp_data[i++]);
			}
			block_num--;
			mainblock.freeblock_num--;
			blockbitmap[freenum] = 1;
			temp_inode->blockused.push_back(freenum);
		}
		temp_inode->address = current;
		temp_inode->length = block_num;
		return true;

	}

	//删除文件
	bool deletefile(string filename,string type_name)
	{
		int temp = findfile(filename, type_name);
		if (temp == -1)
		{
			cout << "目录下无" << filename << '.' << type_name << '!' << endl;
			return false;
		}
		else if (temp >= 0)
		{
			inodebitmap[temp] = 0;
			Inode* temp_inode = inodetable[temp];
			Inode* parent_inode = inodetable[temp_inode->parentinode];
			//删除目录中的索引
			for (int i = 0; i < parent_inode->Inode_num.size(); i++)
			{
				if (parent_inode->Inode_num[i] == temp)
				{
					parent_inode->Inode_num.erase(parent_inode->Inode_num.begin() + i);
					break;
				}
			}
			//删除文件数据块
			for (int i = 0; i < temp_inode->blockused.size(); i++)
			{
				Block* del_block = blocktable[temp_inode->blockused[i]];
				del_block->data = vector<char>();
				blockbitmap[temp_inode->blockused[i]] = 0;
			}
			temp_inode->DeleteInode(temp);
		}
	}

	//保存文件数据
	bool save()
	{
		ofstream block;
		block.open("block.txt");
		//保存文件位视图
		for (int i = 0; i < inodebitmap.size(); i++) {
			block << inodebitmap[i];
		}
		block << endl << endl;
		//保存块位视图
		for (int i = 0; i < blockbitmap.size(); i++) {
			block << blockbitmap[i];
		}
		block << endl << endl;
		//保存文件、目录信息
		for (int i = 0; i < inodetable.size(); i++)
		{
			Inode* temp_inode = inodetable[i];
			if (temp_inode->node == -1)
				continue;
			else if (temp_inode->node == 0)
			{
				block << temp_inode->number << ' ' << temp_inode->node << ' ' << temp_inode->name << ' ' << temp_inode->parentinode << ' ' << temp_inode->Inode_num.size();
				if (temp_inode->Inode_num.size() != 0)
				{
					block << ' ';
					for (int j = 0; j < temp_inode->Inode_num.size(); j++)
					{
						block << temp_inode->Inode_num[j] << ' ';
						if (j != temp_inode->Inode_num.size() - 1)
							block << ' ';
					}
				}
			}
			else if (temp_inode->node == 1)
			{
				block << temp_inode->number << ' ' << temp_inode->node << ' ' << temp_inode->name << ' ' << temp_inode->filetype << ' ' << temp_inode->parentinode << ' ' << temp_inode->blockused.size();
				if (temp_inode->blockused.size() != 0)
				{
					block << ' ';
					for (int j = 0; j < temp_inode->blockused.size(); j++)
					{
						block << temp_inode->blockused[j];
						if (j != temp_inode->blockused.size())
							block << ' ';
					}
				}
			}
			block << endl;
		}

		ofstream data;
		data.open("data.txt");
		bool flag = true;
		//保存文件数据信息
		for (int i = 0; i < blocktable.size(); i++) {
			if (blockbitmap[i] == 1)
			{
				flag = false;
				Block* temp_block = blocktable[i];
				data << i << ' ';
				for (int j = 0; j < temp_block->data.size(); j++)
					data << temp_block->data[j];
				data << endl;
			}
		}
		if (flag)//没有信息
			data << -1;
		block.close();
		data.close();
		return true;
	}

	//读取文件数据
	bool readlocaldata()
	{
		ifstream block;
		block.open("block.txt");
		ifstream data;
		data.open("data.txt");

		string temp;
		//读取文件位视图
		getline(block, temp);		
		for (int i = 0; i < INODE_NUM; i++)
			inodebitmap[i] = temp[i]-48;

		getline(block, temp);

		//读取块位视图
		getline(block, temp);
		for (int i = 0; i < DATABLOCK_NUM; i++)
			blockbitmap[i] = temp[i]-48;

		getline(block, temp);
		
		//读取文件、目录信息
		while (!block.eof())
		{
			int number;
			block >> number;

			Inode*temp_inode = inodetable[number];
			block >> temp_inode->node;
			if (temp_inode->node == 0)//目录
			{
				block >> temp_inode->name >> temp_inode->parentinode;
				temp_inode->filetype = "dir";
				int inodenum;
				block >> inodenum;
				for (int i = 0; i < inodenum; i++)
				{
					int temp;
					block >> temp;
					temp_inode->Inode_num.push_back(temp);
				}
			}
			else if (temp_inode->node == 1)//文件
			{
				block >> temp_inode->name >>temp_inode->filetype >> temp_inode->parentinode;
				int blocknum;
				block >> blocknum;
				for (int i = 0; i < blocknum; i++)
				{
					int temp;
					block >> temp;
					temp_inode->blockused.push_back(temp);
				}

			}
		}

		//读取文件数据信息
		while (!data.eof())
		{
			int number;
			char c;
			data >> number;
			if (number == -1)
				break;
			Block* temp_block = blocktable[number];
			data >> c;
			while (c !=0)
			{
				temp_block->data.push_back(c);
				data >> c;
			}
			getline(data, temp);
		}
		data.close();
		block.close();
		return true;

	}

};

//显示指令
void showhelp()
{
	cout << "format --- 格式化" << endl;
	cout << "cdir --- 创建子目录" << endl;
	cout << "ddir --- 删除子目录" << endl;
	cout << "ls --- 显示目录" << endl;
	cout << "cd --- 进入子目录" << endl;
	cout << "back --- 回到父目录" << endl;
	cout << "create --- 创建文件" << endl;
	cout << "open --- 打开文件" << endl;
	cout << "delete --- 删除文件" << endl;
	cout << "exit --- 退出" << endl;
	cout << "help --- 指令帮助" << endl;
}


int main()
{
	FileManager filemanager;
	string cmd;
	showhelp();
	while (true)
	{
		cout << '\\';
		filemanager.showdirname();
		cout << '>';

		cin >> cmd;
		if (cmd == "format")
		{
			filemanager.Format();
		}
		else if (cmd == "cdir")
		{
			string name;
			cin >> name;
			filemanager.createsubdir(name);
		}
		else if (cmd == "ddir")
		{
			string name;
			cin >> name;
			filemanager.deletesubdir(name);
		}
		else if (cmd == "ls")
		{
			filemanager.showdir();
		}
		else if (cmd == "cd")
		{
			string name;
			cin >> name;
			filemanager.gotodir(name);
		}
		else if (cmd == "back")
		{
			filemanager.backdir();
		}
		else if (cmd == "create")
		{
			string name;
			cin >> name;
			string filename = "", filetype = "";
			bool flag = true;
			for (int i = 0; i < name.length(); i++)
			{
				if (name[i] == '.')
				{
					flag = false;
					continue;
				}

				if (flag)
					filename += name[i];
				else
					filetype += name[i];
			}
			filemanager.createfile(filename,filetype);
		}
		else if (cmd == "open")
		{
			string name;
			cin >> name;
			string filename = "", filetype = "";
			bool flag = true;
			for (int i = 0; i < name.length(); i++)
			{
				if (name[i] == '.')
				{
					flag = false;
					continue;
				}

				if (flag)
					filename += name[i];
				else
					filetype += name[i];
			}
			filemanager.open(filename, filetype);
		}
		else if (cmd == "delete")
		{
			string name;
			cin >> name;
			string filename = "", filetype = "";
			bool flag = true;
			for (int i = 0; i < name.length(); i++)
			{
				if (name[i] == '.')
				{
					flag = false;
					continue;
				}

				if (flag)
					filename += name[i];
				else
					filetype += name[i];
			}
			filemanager.deletefile(filename, filetype);
		}
		else if (cmd == "exit")
		{
			filemanager.save();
			return 0;
		}
		else if (cmd == "help")
		{
			showhelp();
		}
		else
		{
			cout << "请输入正确的指令！" << endl;
		}

	}
	
	return 0;
}