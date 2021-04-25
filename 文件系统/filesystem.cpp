#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<fstream>
#include<iomanip>
using namespace std;

//��������20MB ���СΪ4kb ��5*1024=5120��
//inode��СΪ128B ÿ����32����������20480������
//#1 mainblock ������Ϣ
//#2-#641 inode��
//#642 inodeλʾͼ 
//#643 blockλʾͼ 
//#644-#5119 ���ݿ� 4476��
#define VOLUME 20971520
#define BLOCK_SIZE 4096
#define BLOCK_NUM 5120
#define INODE_NUM 20480
#define DATABLOCK_NUM 4476

//������Ϣ
class Mainblock 
{
public:
	const int volume = VOLUME;//�����ļ���Ĵ�С
	const int inode_num = INODE_NUM ;//�ļ�������Ĵ�С
	const int block_size = BLOCK_SIZE;//�������ݿ��С
	int freeblock_num;//���п�����
	Mainblock()
	{
		freeblock_num = DATABLOCK_NUM;
	}
};

//�ļ�
class Inode 
{
public:
	string name;//�ļ���
	int node;//�ļ����ͣ� -1Ϊ�գ�0ΪĿ¼��1Ϊ�ļ�
	int number;//�ļ���
	string filetype;//�ļ�����(node==1)
	int address;//�ļ���ַ
	int length;//�ļ���С
	vector<int>blockused;
	vector<int>Inode_num;//��Ŀ¼���ļ�(node==0)
	int parentinode;//���ڵ�

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

//���ݿ�
class Block
{
public:
	vector<char>data;

	Block()
	{
		data = vector<char>();
	}
};

//�ļ�����
class FileManager
{
public:
	Mainblock mainblock;//������Ϣ
	vector<int>inodebitmap;//�ļ�λ��ͼ
	vector<int>blockbitmap;//��λ��ͼ
	vector<Inode*>inodetable;//�ļ���
	vector<Block*>blocktable;//���

	int current;//��ǰĿ¼

	FileManager()
	{
		Format();
		readlocaldata();
		//for (int i = 0; i < INODE_NUM; i++)
		//	cout<<inodebitmap[i];
	}

	//��ʽ��
	bool Format()
	{
		//��ʼ��
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
	
	//��ʾ��ǰĿ¼����
	bool showdirname()
	{
		Inode* parent_inode = inodetable[current];
		cout << parent_inode->name;
		return true;
	}

	//���ҿ�����
	int findfreeinode()
	{
		for (int i = 0; i < INODE_NUM; i++)
			if (inodebitmap[i] == 0)
				return i;
		return -1;
	}

	//������Ŀ¼
	bool createsubdir(string dirname)
	{
		int temp = findfreeinode();
		if (temp == -1)
		{
			cout << "������������ɾ��һЩ����" << endl;
			return false;
		}
		else if (temp == 0)//�ļ�Ϊ��
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

	//�����ļ�
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

	//ɾ����Ŀ¼
	bool deletesubdir(string dirname)
	{
		int temp = findfile(dirname,"dir");//�ļ�����
		if (temp == -1)
		{
			cout << "��Ŀ¼���޴�Ŀ¼��"<<endl;
			return false;
		}
		else
		{
			inodebitmap[temp] = 0;
			Inode* temp_inode = inodetable[temp];
			Inode* parent_inode = inodetable[temp_inode->parentinode];

			//�ڸ�Ŀ¼��ɾ�����ļ�����
			for (int i = 0; i < parent_inode->Inode_num.size(); i++)
			{
				if (parent_inode->Inode_num[i] == temp)
				{
					parent_inode->Inode_num.erase(parent_inode->Inode_num.begin() + i);
					break;
				}
			}
			//ɾ����Ŀ¼���ļ��Լ���Ŀ¼
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

	//��ʾĿ¼
	bool showdir()
	{
		Inode* parent_inode = inodetable[current];
		if (parent_inode->Inode_num.empty())
		{
			cout << "��Ŀ¼��û���ļ���" << endl;
			return false;
		}
		cout << parent_inode->name<<"Ŀ¼���ļ�Ϊ��" << endl;
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

	//���ĵ�ǰĿ¼
	bool gotodir(string dirname)
	{
		int temp = findfile(dirname,"dir");
		if (temp == -1)
		{
			cout << "��Ŀ¼���޴�Ŀ¼��" << endl;
			return false;
		}
		else
		{
			current = temp;
		}
		return true;
	}

	//�ص���Ŀ¼
	bool backdir()
	{
		if (current == 0)
		{
			cout << "���ڸ�Ŀ¼��" << endl;
			return false;
		}
		Inode* parent_inode = inodetable[current];
		current = parent_inode->parentinode;
		return true;
	}

	//�����ļ�
	bool createfile(string filename,string type_name)
	{
		int temp = findfreeinode();
		if (temp == -1)
		{
			cout << "������������ɾ��һЩ����" << endl;
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

	//���ļ�
	bool open(string filename, string type_name)
	{
		int temp = findfile(filename, type_name);
		if (temp == -1)
		{
			cout << "Ŀ¼����" <<filename<<'.'<<type_name<<'!'<< endl;
			return false;
		}
		else if (temp >= 0)//���ж���д���رղ���
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
					cout << "��������ȷ�Ķ�дָ�" << endl;
				}
			}
			
		}
		
	}
	
	//�ر��ļ�
	bool close()
	{
		Inode* temp_inode = inodetable[current];
		if (temp_inode->parentinode < 0)
			return false;
		current = temp_inode->parentinode;
		return true;
	}

	//���ļ�
	bool read()
	{
		//����ļ����ݿ�������
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

	//���ҿ����ݿ�
	int findfreeblock()
	{
		for (int i = 0; i < BLOCK_NUM; i++)
			if (blockbitmap[i] == 0)
				return i;
		return -1;
	}

	//д�ļ�
	bool write()
	{
		cout << "(EOF=CTRL+Z���밴CTRL+Z��������)" << endl;//��ʾ
		char temp;
		Inode* temp_inode = inodetable[current];
		vector<char> temp_data = vector<char>();
		while ((temp = getchar()) != EOF)//EOF=ctrl+Z
		{
			temp_data.push_back(temp);
		}
		int data_num = temp_data.size();
		int block_num = data_num / BLOCK_SIZE + 1;//������Ҫ���ݿ�����
		if (block_num > mainblock.freeblock_num)
		{
			cout << "���̿ռ䲻��!" << endl;
			return false;
		}
		int i = 0;
		//�����ݽ���д��
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

	//ɾ���ļ�
	bool deletefile(string filename,string type_name)
	{
		int temp = findfile(filename, type_name);
		if (temp == -1)
		{
			cout << "Ŀ¼����" << filename << '.' << type_name << '!' << endl;
			return false;
		}
		else if (temp >= 0)
		{
			inodebitmap[temp] = 0;
			Inode* temp_inode = inodetable[temp];
			Inode* parent_inode = inodetable[temp_inode->parentinode];
			//ɾ��Ŀ¼�е�����
			for (int i = 0; i < parent_inode->Inode_num.size(); i++)
			{
				if (parent_inode->Inode_num[i] == temp)
				{
					parent_inode->Inode_num.erase(parent_inode->Inode_num.begin() + i);
					break;
				}
			}
			//ɾ���ļ����ݿ�
			for (int i = 0; i < temp_inode->blockused.size(); i++)
			{
				Block* del_block = blocktable[temp_inode->blockused[i]];
				del_block->data = vector<char>();
				blockbitmap[temp_inode->blockused[i]] = 0;
			}
			temp_inode->DeleteInode(temp);
		}
	}

	//�����ļ�����
	bool save()
	{
		ofstream block;
		block.open("block.txt");
		//�����ļ�λ��ͼ
		for (int i = 0; i < inodebitmap.size(); i++) {
			block << inodebitmap[i];
		}
		block << endl << endl;
		//�����λ��ͼ
		for (int i = 0; i < blockbitmap.size(); i++) {
			block << blockbitmap[i];
		}
		block << endl << endl;
		//�����ļ���Ŀ¼��Ϣ
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
		//�����ļ�������Ϣ
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
		if (flag)//û����Ϣ
			data << -1;
		block.close();
		data.close();
		return true;
	}

	//��ȡ�ļ�����
	bool readlocaldata()
	{
		ifstream block;
		block.open("block.txt");
		ifstream data;
		data.open("data.txt");

		string temp;
		//��ȡ�ļ�λ��ͼ
		getline(block, temp);		
		for (int i = 0; i < INODE_NUM; i++)
			inodebitmap[i] = temp[i]-48;

		getline(block, temp);

		//��ȡ��λ��ͼ
		getline(block, temp);
		for (int i = 0; i < DATABLOCK_NUM; i++)
			blockbitmap[i] = temp[i]-48;

		getline(block, temp);
		
		//��ȡ�ļ���Ŀ¼��Ϣ
		while (!block.eof())
		{
			int number;
			block >> number;

			Inode*temp_inode = inodetable[number];
			block >> temp_inode->node;
			if (temp_inode->node == 0)//Ŀ¼
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
			else if (temp_inode->node == 1)//�ļ�
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

		//��ȡ�ļ�������Ϣ
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

//��ʾָ��
void showhelp()
{
	cout << "format --- ��ʽ��" << endl;
	cout << "cdir --- ������Ŀ¼" << endl;
	cout << "ddir --- ɾ����Ŀ¼" << endl;
	cout << "ls --- ��ʾĿ¼" << endl;
	cout << "cd --- ������Ŀ¼" << endl;
	cout << "back --- �ص���Ŀ¼" << endl;
	cout << "create --- �����ļ�" << endl;
	cout << "open --- ���ļ�" << endl;
	cout << "delete --- ɾ���ļ�" << endl;
	cout << "exit --- �˳�" << endl;
	cout << "help --- ָ�����" << endl;
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
			cout << "��������ȷ��ָ�" << endl;
		}

	}
	
	return 0;
}