#include<bits/stdc++.h>
using namespace std;

struct reader{
	int id;
	char name[30];
	char pass[30];
	int value;
}r;

struct archive{
	int bookid;
	char title[30];
	char author[30];
	int pubyear;
	int page;
	int abs;
}b;

struct transinfo {
    int bookId;
    int readerId;
}t;

ostream& operator<<(ostream &o, reader &r){
	o<<r.id<<" "<<r.name<<"/ "<<r.name<<"\t"<<r.value;
	return o;
}

void addbook(fstream& file){
	int num=101;
	file.open("bookst.data",ios::binary|ios::in);
	if(file){
		while(file.read(reinterpret_cast<char *>(&b),sizeof(archive)));
		if(b.bookid<100){
			num=101;
		}else{
			num=b.bookid+1;
		}
		file.close();	
	}
	file.open("bookst.data",ios::binary|ios::app);
	if(file){
		b.bookid=num;
		cout<<"input book title ";cin>>b.title;
		cout<<"input author ";cin>>b.author;
		cout<<"input pages ";cin>>b.page;
		cout<<"input public year ";cin>>b.pubyear;
		b.abs=1;
		file.write(reinterpret_cast<char *>(&b),sizeof(archive));
		file.close();
		fstream transfile("transinfot.data",ios::binary|ios::app);
		if(transfile){
		while(file.read(reinterpret_cast<char *>(&t),sizeof(transinfo)));
		if(t.bookId<100){
			num=101;
		}else{
			num=t.bookId+1;
		}
		file.close();	
		}	
		t.readerId=-1;
		t.bookId=num;
		transfile.write(reinterpret_cast<char *>(&t),sizeof(transinfo));
		transfile.close();
	}
}

void showall(fstream& file){
	file.open("bookst.data",ios::binary|ios::in);
	if(file){
		cout<<"�s�X   �ѦW   �@��   �X���~   ����   ���A"<<endl; 
		while(file.read(reinterpret_cast<char *>(&b),sizeof(archive))){
			cout<<b.bookid<<" "<<b.title<<" "<<b.author<<" "<<b.pubyear<<" "<<b.page<<" "<<b.abs<<endl;
		}
		file.close();
	}
}

void borrow(fstream& file){
	int choose;
	file.open("bookst.data",ios::binary|ios::in|ios::out);
	if(file){
		cout<<"input the bookid you want to borrow: ";
		cin>>choose;
		int delta=(choose-101)*sizeof(archive);
		file.seekg(delta);
		file.read(reinterpret_cast <char*>(&b),sizeof(archive));
		if(b.abs==0){
			cout<<"this book is lost "<<endl; 
		}
		else if(b.abs==2){
			cout<<"this book is unvailable to borrow "<<endl;
		}
		else if(b.abs==1){
			cout<<"book borrowing..."<<endl;
			b.abs=2;
			file.seekp(delta);
			file.write(reinterpret_cast <char*>(&b),sizeof(archive));
			file.close();
			fstream transFile("transt.data",ios::binary|ios::app);//from chatGPT
			if(transFile){//from chatGPT
				time_t now=time(0);//let the time back to 1970/1/1 00:00:00, then count seconds from 1970 to now
				tm* ltm=localtime(&now);//change the seconds into local time's structure
				char timestamp[20];
				strftime(timestamp,sizeof(timestamp),"%Y/%m/%d %H:%M",ltm);//format the date and time intospecified format,put result into timestamp. 
				transFile<<timestamp<<" "<<r.id<<" "<<choose<<" 1 "<<endl;
				transFile.close();
				
				fstream transfile("transinfot.data",ios::binary|ios::in|ios::out);//�n�O�O�֭� 
				int afa=(choose-101)*sizeof(transinfo);
				transfile.seekg(afa);
				transfile.read(reinterpret_cast <char*>(&t),sizeof(transinfo));
				t.readerId=r.id;
				transfile.seekp(afa);
				transfile.write(reinterpret_cast<char*>(&t),sizeof(transinfo));
				cout<<"book borrowed successfully"<<endl;
				transfile.close();
			}
		}
		else{
			cout<<"can't find book id..."<<endl;
		}
		file.close();
	}
}

void returnbook(fstream& file) {
	int id,rid;
	file.open("bookst.data",ios::binary|ios::in|ios::out);
	fstream transfile("transinfot.data",ios::binary|ios::in|ios::out);
	if(file){
		cout<<"input the book id you want to return...";
		cin>>id;
		int delta=(id-101)*sizeof(archive);
		file.seekg(delta);
		
		int afa=(id-101)*sizeof(transinfo);
		transfile.seekg(afa);
		
		file.read(reinterpret_cast<char *>(&b),sizeof(archive));
		transfile.read(reinterpret_cast<char *>(&t),sizeof(transinfo));	
		if(b.abs==1){
			cout<<"this book hasn't been borrowed..."<<endl;
		}
		else if(b.abs==0){
			cout<<"the book is gone..."<<endl;
		}
		else if(b.abs==2&&r.id==t.readerId){
			cout<<"returning..."<<endl;
			b.abs=1;
			file.seekp(delta);
			file.write(reinterpret_cast <char*>(&b),sizeof(archive));
			cout<<"book returned successfully..."<<endl;
			fstream transFile("transt.data", ios::binary | ios::app);
			if(transFile){
				time_t now=time(0);
				tm* ltm=localtime(&now);
				char timestamp[20];
				strftime(timestamp,sizeof(timestamp),"%Y/%m/%d %H:%M",ltm);
				int readerId;
				transFile<<timestamp<<" "<<r.id<<" "<<id<<" -1 "<<endl;
				transFile.close();
				
				transfile.seekp(afa);
				t.readerId=-1;
				transfile.write(reinterpret_cast<char*>(&t),sizeof(transinfo));
				transfile.close();				
			}
		}
		else{
			cout<<"you didn't borrowed this book..."<<endl;
		}
		file.close();
		transfile.close();
	}
}


void translist(fstream& file){
	int sid;;
	string time;//����ɶ� 
	cout<<"input the user you want to search(all=1001)"<<endl;
	cin>>sid;//���Mid 
	if(sid==1001){
		file.open("transt.data");
		if(file){
			string s;
			string date;
			string time;
			int id,bid,trans;
			while(getline(file,s)){
				stringstream ss(s);
				ss>>date;
				ss>>time;
				ss>>id;
				ss>>bid;
				ss>>trans;

				fstream rfile("readerst.data",ios::binary|ios::in);
				int name = (id-1002) * sizeof(reader);
				rfile.seekg(name);
				rfile.read(reinterpret_cast <char*>(&r),sizeof(reader));
				fstream bfile("bookst.data",ios::binary|ios::in);
				int book=(bid-101)*sizeof(archive);
				bfile.seekg(book);
				bfile.read(reinterpret_cast<char *>(&b),sizeof(archive));
				cout<<date<<" "<<time<<" "<<id<<" "<<r.name<<" "<<bid<<" "<<b.title<<" "<<trans<<endl;
				rfile.close();
				bfile.close();
				ss.str("");
   				ss.clear();
			}
		file.close();	
		}		
	}
	else if(sid>1001){
		file.open("transt.data");
		if(file){
			string s;
			string date;
			string time;
			int id,bid,trans;
			while(getline(file,s)){
				stringstream ss(s);
				ss>>date;
				ss>>time;
				ss>>id;
				ss>>bid;
				ss>>trans;
				if(id!=sid){
					ss.str("");
   					ss.clear();
   					continue;
				}
				else{
					fstream rfile("readerst.data",ios::binary|ios::in);
					int name = (id-1002) * sizeof(reader);
					rfile.seekg(name);
					rfile.read(reinterpret_cast <char*>(&r),sizeof(reader));
					fstream bfile("bookst.data",ios::binary|ios::in);
					int book=(bid-101)*sizeof(archive);
					bfile.seekg(book);
					bfile.read(reinterpret_cast<char *>(&b),sizeof(archive));
					cout<<date<<" "<<time<<" "<<id<<" "<<r.name<<" "<<bid<<" "<<b.title<<" "<<trans<<endl;
					rfile.close();
					bfile.close();
					ss.str("");
	   				ss.clear();
   				}
			}	
		}
		file.close();		
	}
	else cout<<"can't find the user."<<endl;
}

int main(int argc, char** argv) {
	fstream file;
	int command;
	int no=1002;//Ū��id�q1002�}�l 
	cout<<"select a command to continue: 1. register 2. log in 3. exit ";
	while(cin>>command){
		switch(command){
			case 1:{ //���U 
				char username[30];
				char password[30];
				cout<<"input your username ";
				cin>>username;
				file.open("readerst.data",ios::binary|ios::in|ios::out);//���}��Ƨ��A�ˬd���L���Ʊb���W 
				if(file){//�T�O���T�ꥴ�} 
					bool ex=0;
					while(file.read(reinterpret_cast<char *>(&r),sizeof(reader))){
						if(strcmp(r.name, username) == 0){//from chatGPT,compare two strings,if two strings are same, return 0;
							ex=1;
							break;
						}
						if(r.id>=no){
							no=r.id+1;
						}	
					}
					file.close();//���}�N�n�� 
					if(ex){//�Y�����ƫh���i�ϥΡA����^���� 
						cout<<"this username has been used..."<<endl;
						break;
					}
				}
				cout<<"input your password ";//�L���ơA�]�w�K�X 
				cin>>password;
				file.open("readerst.data",ios::binary|ios::app);//���}���A�����K�X 
				if(file){
					r.id=no++;
					strcpy(r.name,username);//a string copy func.
					strcpy(r.pass,password);//from chatGPT.
					r.value=100;
					file.write(reinterpret_cast<char *>(&r),sizeof(reader));
					file.close();
					cout<<"register success!"<<endl;
				}
				break;
			}
			case 2:{ //�n�J�A�ϥΪA�� 
				char username[30];
				char password[30];
				int cmd;
				int lcmd;
				cout<<"input your username ";
				cin>>username;
				file.open("readerst.data",ios::binary|ios::in);//Ū����� 
				if(file){
					bool check=0;
					while(file.read(reinterpret_cast<char *>(&r),sizeof(reader))){
						if(strcmp(r.name, username) == 0){//�ˬd�b���O�_�s�b 
							check=1;
							break;
						}
					}
					file.close();
					if(check){//���s�b�N��J�K�X 
						cout<<"input password ";
						cin>>password;
						if(strcmp(r.pass, password) == 0){//�ˬd�K�X�O�_���T 
							cout<<"welcome "<<r.name<<"!"<<"/"<<" your id: "<<r.id<<endl;
							cout<<"select a command to continue: 1.borrow books 2.borrowing details 3.return books 4.browse books menu 5.add new book 6.exit ";
							while(cin>>lcmd){
								switch(lcmd){
									case 1:{
										borrow(file);
										break;
									}
									case 2:{
										translist(file);
										break;
									}
									case 3:{
										returnbook(file);
										break;
									}
									case 4:{
										showall(file);
										break;
									}
									case 5:{
										addbook(file);
										break;
									}
									case 6:{
										cout<<"exit..."<<endl;
										break;
									}
									default:{
										cout<<"wrong command..."<<endl;
										break;
									}
								}
								if(lcmd==6){
									break;
								}
								cout<<"select a command to continue: 1.borrow books 2.borrowing details 3.return books 4.browse books menu 5.add new book 6.exit  ";
							}
						}else{
							cout<<"wrong password, please log in again"<<endl;
						}	
					}
					else{
						cout<<"can't find the user"<<endl;
					}
				}
				break;
			} 
			case 3:{ //�����{�� 
				cout<<"see you next time!"<<endl;
				return 0;
			}
			case 1001:{//�]�w�޲z�� 
				char username[30];
				char password[30];
				cout << "Input your username: ";
				cin >> username;//�H�b���W��� 
				file.open("readerst.data", ios::binary | ios::in | ios::out);//���}�BŪ����� 
				if (file) {
				    bool ex = false;
				    while (file.read(reinterpret_cast<char *>(&r), sizeof(reader))) {
				        if (strcmp(r.name, username) == 0) {
				            ex = true;// ��ʫ��w�N�X��1001
				            r.id = 1001;// �N�ק�Z���ƾڿ�J���
				            file.seekp(file.tellg() - sizeof(reader));//���w��m�g�J 
				            file.write(reinterpret_cast<char *>(&r), sizeof(reader));
				            cout << "ID for " << r.name << " has been set to 1001." << endl;
				            break;
				        }
				    }
				    if (!ex) {
				        cout << "Username not found." << endl;
				    }
				    file.close();
				}

				break;
			}
			default: 
				cout<<"please input the correct command!"<<endl;
		}
		cout<<"select a command to continue: 1. register 2. log in 3. exit ";
	}

return 0;
}	
