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
		cout<<"編碼   書名   作者   出版年   頁數   狀態"<<endl; 
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
				
				fstream transfile("transinfot.data",ios::binary|ios::in|ios::out);//登記是誰借 
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
	string time;//日期時間 
	cout<<"input the user you want to search(all=1001)"<<endl;
	cin>>sid;//收尋id 
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
	int no=1002;//讀者id從1002開始 
	cout<<"select a command to continue: 1. register 2. log in 3. exit ";
	while(cin>>command){
		switch(command){
			case 1:{ //註冊 
				char username[30];
				char password[30];
				cout<<"input your username ";
				cin>>username;
				file.open("readerst.data",ios::binary|ios::in|ios::out);//打開資料夾，檢查有無重複帳號名 
				if(file){//確保文件確實打開 
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
					file.close();//有開就要關 
					if(ex){//若有重複則不可使用，跳轉回首頁 
						cout<<"this username has been used..."<<endl;
						break;
					}
				}
				cout<<"input your password ";//無重複，設定密碼 
				cin>>password;
				file.open("readerst.data",ios::binary|ios::app);//打開文件，紀錄密碼 
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
			case 2:{ //登入，使用服務 
				char username[30];
				char password[30];
				int cmd;
				int lcmd;
				cout<<"input your username ";
				cin>>username;
				file.open("readerst.data",ios::binary|ios::in);//讀取資料 
				if(file){
					bool check=0;
					while(file.read(reinterpret_cast<char *>(&r),sizeof(reader))){
						if(strcmp(r.name, username) == 0){//檢查帳號是否存在 
							check=1;
							break;
						}
					}
					file.close();
					if(check){//有存在就輸入密碼 
						cout<<"input password ";
						cin>>password;
						if(strcmp(r.pass, password) == 0){//檢查密碼是否正確 
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
			case 3:{ //結束程式 
				cout<<"see you next time!"<<endl;
				return 0;
			}
			case 1001:{//設定管理員 
				char username[30];
				char password[30];
				cout << "Input your username: ";
				cin >> username;//以帳號名更改 
				file.open("readerst.data", ios::binary | ios::in | ios::out);//打開、讀取資料 
				if (file) {
				    bool ex = false;
				    while (file.read(reinterpret_cast<char *>(&r), sizeof(reader))) {
				        if (strcmp(r.name, username) == 0) {
				            ex = true;// 手動指定代碼為1001
				            r.id = 1001;// 將修改后的數據輸入文件
				            file.seekp(file.tellg() - sizeof(reader));//指定位置寫入 
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
