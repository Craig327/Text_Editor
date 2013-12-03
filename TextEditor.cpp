#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <string>
#include <fstream>

using namespace std;

const int buffersize = 1920;

class Buffer{

public:
	Buffer();
	~Buffer();
	int point,end,y,cursor,numlines;
	string searchstring;
	ofstream savefile;
	bool found,overstrike;
	char arr[buffersize];
	void gotoxy(int, int);
	void SetPointA(int);
	void SetPointR(int);
	int GetPoint();
	void Insert(char);
	void Delete(int);
	int SearchF(string);
	void redisplay();
	void save();
};

Buffer::Buffer(){
	point = 0;
	cursor = 1;
	end = 0;
	y=1;
	numlines = 1;
	overstrike = false;
}

Buffer::~Buffer(){
}

void Buffer::gotoxy(int x, int y)
{ 
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD newpoint;
   	newpoint.X = x-1;
	newpoint.Y = y-1;     
   	SetConsoleCursorPosition(hConsole, newpoint);
   	return;
}

void Buffer::SetPointA(int newpoint){
	point = newpoint;
}

int Buffer::GetPoint(){
	return point;
}

void Buffer::SetPointR(int count){
	int newplace = GetPoint() + count; 
	SetPointA(newplace);
}

void Buffer::Insert(char newchar){
	if(overstrike){
		if(point <= sizeof(arr)-1)arr[point] = newchar;
		else cout << "ERROR, Buffer is full";						//Insert in Overstrike Mode
		
		if(point-1 == end)end++;
	}
	else{
		if(point <= sizeof(arr)-1){
			if(point-1 == end){
				arr[point] = newchar;								//Insert in "Insert" Mode
				end++;
			}
			else {
				end++;
				int i = end;
				for(i; i >=point ; i--)arr[i+2] = arr[i+1];
				arr[point] = newchar;
			}
		}
		else cout << "ERROR, Buffer is full";
	}
	if((int)arr[point] == 13)cursor = 0;
		cursor++;
		point++;
}

void Buffer::Delete(int count){
	int i;
	if(count < 0){
		count++;
		for(i = point-1; i <= end ; i++)arr[i] = arr[i+1];
		arr[i] = NULL;											//Backspace button
		if (point>=1)point--;
		if (cursor>1)cursor--;
		end--;
		Delete(count);
	}
	else if(count > 0){
		count--;
		if(point != end){
		for(i = point-1; i <= end ; i++)arr[i+1] = arr[i+2];			//Delete button
		end--;}
		Delete(count);
	}
	else;
}

int Buffer::SearchF(string query){
	found = false;
	int i,j,m = query.length();
	for (i=0,j=0; j<m && i<end; i++,j++){
		while(arr[i] != query[j] && query[j] != '?'){
			i -= j-1;
			j=0;
		}
	}
	if(j==m){
		found = true;
		return i-m;	
	}
	else return i;
}

int getch(void)
{ 
     int response;
     cout << flush;
     response = _getch();
     HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
     FlushConsoleInputBuffer(hConsole);
     return response;
}

int getche(void)
{
     int response;
     response = _getche();
     HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
     FlushConsoleInputBuffer(hConsole);
     return response;
}

void Buffer::save(){
	int i=0;
	savefile.open("thesavefile.txt");
	while(i<end) {
		if((int)arr[i] != 13)savefile << arr[i];
		else savefile << '\n';
		i++;
	}
	savefile.close();
	cout << "File Saved";
}

void Buffer::redisplay(){
	int i,m,j,k,p;
	cout<<flush;
	system("CLS");	//Clear Screen
	i=0;
	numlines = 1;
	while(i < end){
		if((int)arr[i] == 13){	//If it contains the Enter character, move down a line.
		cout<<'\n'; 
		numlines++;
		}
	else cout << arr[i];			//Display array	
	i++;
	}
	cout<<endl<<"Line Number: "<< y << " Column Number: "<<cursor;  //Status Line	
	gotoxy(cursor,y);									//Set Cursor Position
	j = getch();							//Read first ANSI code
	if(j >= 32 && j <= 126 || j == 13){
		if(j==13)y++;
		Insert(j);							//Regular chars/digits OR Enter
		redisplay();
	}	
	else if(j==19)save();			//CTRL- s - Saves the file
	else if(j == 8)Delete(-1);		//Backspace
	else if(j==6){
		int temp = point;
		k=0;
		searchstring = "";
		bool fin=false,esc =false;
		cout <<endl<<endl<<" Enter a string to search for: ";
		while(!fin){
			m = getche();						//CTRL-f - Search for a string
			if(m == 27){
				esc = true;
				fin = true;}
			else if(m==13)fin = true;
			else if(m >= 32 && m <= 126)searchstring += m;
		}
		if(!esc)k = SearchF(searchstring);								
		if(found){
		point =k;
		k=0;
		cursor =1;
		y=1;
		while(k != point){
			if(arr[k] == 13){
				cursor = 1;
				y++;
			}
			k++;
			cursor++;
		}
		}
		else {
			gotoxy(cursor,y);
			point = temp;
		}
	}
	else if (j == 224){
		m=getch();	
		if(m == 83)Delete(1);									//Delete Button
		else if(m == 75){if(point>=1 && cursor >=1){
			point--;
			cursor--;
		if (cursor == 1 && point >= 1) {
			y--;									//Left Arrow
			point= point -2;
			while((int)arr[point+1]!= 13 && point != end)point++;
			while((int)arr[cursor]!= 13)cursor++;
		}
		}	
		
		}		
		else if(m == 77){if(point < end){
		cursor++;
		point++;									//Right Arrow
		if(arr[point] == 13){
			cursor=1;
			y++;
		point++;}}	}	
		else if(m == 72){if(y>1){
			k = numlines;
			point = 0;
			bool done= false;
			while(!done){
				point++;
				if((int)arr[point] == 13)k--;		//Up Arrow
				if(k == y){
					y--;
					point = point + cursor;
					done =true;
				}}}	}
		else if(m == 80){if(y<numlines){
			k = numlines;
			point = 0;
			bool done= false;
			while(!done){
				point++;
				if((int)arr[point] == 13)k--;	//Down Arrow
				if(k == y){
					y++;
					point = point + cursor;
					done =true;
				}}}	}	
		else if(m == 71){
			while((int)arr[point]!= 13 && point > 0)point--;  //Home key
			cursor = 1;	}		
		else if(m == 79){
			while((int)arr[point]!= 13 && point != end){
				point++;									//End key
				cursor++;
			}}
		else if(m == 119){
			point = 0;					//CTRL-HOME
			cursor = 1;
			y = 1;
		}
		else if(m==117){
		k = numlines;
		point = 0;
		cursor = 0;						//CTRL-END
		y=numlines;
		while(point < end)
		{
			if(arr[point]==13)cursor = 0;
			point++;
			cursor++;
		}
		}
		else if(m==82){
			if(!overstrike)overstrike = true;		//Insert - Changes between Insert and Overstrike mode
			else overstrike =false;
		}
	}
	redisplay();
}

void main(){
	Buffer MyBuffer;
	MyBuffer.redisplay();
}
