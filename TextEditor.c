#include<stdio.h>
#include<alloc.h>
#include<conio.h>
#include<string.h>
#include<dos.h>
#include<stdlib.h>
#include<process.h>
#include<ctype.h>
#define CGA 1
#define ESC 27
#define NO_SHADOW 0
#define HALF_SHADOW -1


char far *vid_mem;
int ascii,scan;
int choice;
char *buf[50];
void drawbox(int,int,int,int,int);
void writechar(int,int,char,int);
void writestring(char *,int,int,int);
void size(int,int);
void write_fname();
void menubox(int,int,int,int,char,char);

void displaymenuh(char**,int);
void displaymenuv(char**,int,int,int);
int getresponsev(char**,char *,int,int,int,int);
int getresponseh(char**,char *,int);

int popupmenuv(char **,int,int,int,char*,int);
void mainscreen();
void getkey();

void savevideo(int,int,int,int,char*);
void restorevideo(int,int,int,int,char*);

int mainmenuserver();
int fileserver();
int helpserver();
int searchserver();
int exitserver();


int get_name(int,int,char*);
int ask_name(char*,char*);
int load();
int new1();
int save();

char *mainmenu[]={"file","edit","search","Help","exit"};
char *filemenu[]={"New","Save","open","SaveAs"};
char *searchmenu[]={"Find","Replace","GotoLine"};
char *helpmenu[5];
char *exitmenu;
char filespec[]="VOC.TXT";

void main()
{
clrscr();
 #ifdef CGA
 {
  vid_mem=(char far *)0xb8000000L;
  textmode(3);
 }
 #else
 {
  vid_mem=(char far *)0xb0000000L;
  textmode(7);
 }
 #endif
 drawbox(1,0,22,79,112);
 menubox(2,1,22,80,28,NO_SHADOW);
 /*write_fname();

 displaymenuh(mainmenu,5);
 getresponseh(mainmenu,"FESE",4);
popupmenuv(filemenu,3,0,2,"NSL",0);
popupmenuv(searchmenu,3,0,25,"FRG",0);  */

mainmenuserver();


getch();
}


void drawbox(int sr,int sc,int er,int ec,int attr)
{
 int i;
 for(i=sc+1;i<ec;i++)
 {
  writechar(sr,i,205,attr);
  writechar(er,i,205,attr);
 }
 for(i=sr+1;i<er;i++)
 {
  writechar(i,sc,186,attr);
  writechar(i,ec,186,attr);
 }
 writechar(sr,sc,201,attr);
 writechar(sr,ec,187,attr);
 writechar(er,sc,200,attr);
 writechar(er,ec,188,attr);
}



void writechar(int r,int c,char ch,int attb)
{
 char far *v;
 v=vid_mem+r*160+c*2;
 *v=ch;
 v++; *v=attb;
}



void size(int ssl,int esl)
{
 union REGS ii,oo;
 ii.h.ah=1;
 ii.h.ch=ssl;
 ii.h.cl=esl;
 ii.h.bh=0;
 int86(16,&ii,&oo);
}




void menubox(int sr,int sc,int er,int ec,char fil,char shad)
{
 int i,j;
 for(i=sr;i<er;i++)
 {
  for(j=sc;j<(ec-1);j++)
	writechar(i,j,' ',fil);
 }
 printf("%c",shad);
}






void mainscreen()
{
	int i,j;
	drawbox(1,0,23,79,7);
	drawbox(3,0,21,79,7);

	writechar(3,0,204,7);

	writechar(3,79,185,7);
	writechar(21,0,204,7);
	writechar(21,79,185,7);
	for(i=4;i<=20;i++)
	{
	 for(j=1;j<=78;j++)
	 {
	  writechar(i,j,177,7);
	  writechar(i,j+1,177,7);
	 }
	}
	writestring("helpmate",2,32,112);
	writestring("enter a key",22,14,112);
}




void writestring(char *s,int r,int c,int attb)
{
 while(*s!='\0')
 {
  if(*s=='^')
  {
   s++;
   writechar(r,c,*s,126);
  }
  else
  writechar(r,c,*s,attb);
  s++;
  c++;
 }
}




int popupmenuv(char **menu,int count,int sr,int sc,char *hotkeys,int helpnumber)
{
 int er,i,ec,len=0,area,l;
 char *p;
 size(32,0);
 er=sr+count+2;
 for(i=0;i<count;i++)
 {
  l=strlen(menu[i]);
  if(l<len)
  len=l;
 }
 ec=sc+len;
 area=(er-sr+1)*(ec-sc+1)*2;
 p=malloc(area);

 savevideo(sr,sc,er,ec,p);
 menubox(sr,sc,er,ec,112,15);


 displaymenuv(menu,count,sr+1,sc);
 choice=getresponsev(menu,hotkeys,sr,sc+2,count,helpnumber);

 restorevideo(sr,sc,er,ec,p);

 free(p);
 size(5,7);
return(choice);
}





void displaymenuv(char **menu,int count,int sr,int sc)
{
 int i;
 for(i=0;i<count;i++)
 {
  writestring(menu[i],sr,sc+3,112);
  sr++;
 }
}




void displaymenuh(char **menu,int count)
{
 int col=2,i;
 size(32,0);
 menubox(0,0,0,79,112,NO_SHADOW);
 for(i=0;i<count;i++)
 {
  writestring(menu[i],0,col,112);
  col=col+(strlen(menu[i]))+7;
 }
 size(5,7);
}




void write_fname()
{
 int len;
 char drive[2],ext[5],*filename;
 char fname[5]={'a','b','.','c'};
 size(32,0);
 drawbox(1,0,23,79,27);
 drive[0]=getdisk()+65;
 drive[1]='\0';
 fnsplit(filespec,",",fname,ext);
 strcpy(filename," ");
 strcat(filename,drive);
 strcat(filename,":");
 strcat(filename,fname);
 if(ext[0])
 strcat(filename,ext);
 strcat(filename," ");
 strupr(filename);
 len=strlen(filename);
 writestring(filename,1,39-len/2,27);
 size(5,7);
}


int getresponsev(char **menu,char *hotkeys,int sr,int sc,int count,int helpnumber)
{
	int choice=1,len;
	len=strlen(hotkeys);
	printf("%d",len);
	writestring(menu[choice-1],sr+choice,sc+1,15);
	while(1)
	{
		getkey();
		if(ascii==0)
		{
			switch(scan)
			{
			case 80:
				writestring(menu[choice-1],sr+choice,sc+1,112);
				choice++;
				break;
			case 72:

				writestring(menu[choice-1],sr+choice,sc+1,112);
				choice--;
				break;
			case 77:
				return(77);
			case 75:
				return(75);
			}
		if(choice==0)
			choice=count;
		if(choice>count)
			choice=1;
		writestring(menu[choice-1],sr+choice,sc+1,26);
		}
	else
	{
		if(ascii==13)
			return(choice);
		if(ascii==ESC)
			displaymenuh(mainmenu,5);
			return (ESC);

	}
	}
	return (choice);
}



int getresponseh(char **menu,char *hotkeys,int count)
{
 int choice=1,len,col,hotkeychoice;
 size(32,0);
 col=2;
 //len=strlen(hotkeys);
 writestring(menu[choice-1],0,col,15);
 while(1)
 {
  getkey();
  if(ascii==0)
  {
   switch(scan)
   {
    case 77:writestring(menu[choice-1],0,col,112);
    col+=strlen(menu[choice-1])+7;
    choice++;
    break;
    case 75:writestring(menu[choice-1],0,col,112);
    col-=strlen(menu[choice-2])+7;
    choice--;
    break;
   }
   if(choice==0)
   {
	choice=count;
	col=37;
   }
if(choice>count)
{
	choice=1;
	col=2;
}
   writestring(menu[choice-1],0,col,15);
 }
	 else
	{
		if(ascii==13)
		{
			size(5,7);
			return(choice);
		}
		if(ascii==ESC)
		{
			writestring(menu[choice-1],0,col,112);
			size(5,7);
			return(ESC);
		}

	}
 }
}




void getkey()
{
 union REGS ii,oo;
 while(!kbhit());
  ii.h.ah=0;
  int86(22,&ii,&oo);
  ascii=oo.h.al;
  scan=oo.h.ah;
}






void savevideo(int sr,int sc,int er,int ec,char *buffer)
{
	char far *v;
	int i,j;
	for(i=sr;i<=er;i++)
	{
		for(j=sc;j<=ec;j++)
		{
			v=vid_mem+i*160+j*2;
			*buffer=*v;
			v++;buffer++;
			*buffer=*v++;
			buffer++;
		}
	}

}


void restorevideo(int sr,int sc,int er,int ec,char *buffer)
{
char far *v;
int i,j;
	for(i=sr;i<=er;i++)
	{
		for(j=sc;j<=ec;j++)
		{
			v=vid_mem+i*160+j*2;
			*v=*buffer;
			v++;
			buffer++;
			*v=*buffer;
			buffer++;
		}
	}
}

int mainmenuserver()
{
	int mchoice,esc_flag;
	while(1)
	{
		displaymenuh(mainmenu,5);
		mchoice=getresponseh(mainmenu,"fvkj",5);
		switch(mchoice)
		{
			case 1:fileserver();
				break;
			case 2:searchserver();
				break;
			case 3:helpserver();
				break;
			case 4:exitserver();
				break;
		}
		if(esc_flag==ESC)
			return(esc_flag);
	}

}

int fileserver()
{
	int fchoice,esc_flag=0,flag;
	char fname[30];
	fchoice=popupmenuv(filemenu,3,0,2,"NSL",0);
	switch(fchoice)
	{
		case 1:	new1();
			break;
		case 2:	save();
			break;
		case 3:	/*strcpy(fname,filespec);
			esc_flag=ask_name("enter filename",filespec);
			if(esc_flag==ESC)
				break;
			*/
			flag=  load();
			if(flag==0)
			{
				strcpy(filespec,fname);
				write_fname();
			}
			break;
		case ESC:	esc_flag=ESC;
	}
		return(esc_flag);
}


int ask_name(char *str,char *name)
{
	int area,esc_flag,len;
	char *p,currentdir[30];
	area=(17-7+1)*(62-17+1)*2;
	p=malloc(area);
	savevideo(7,17,17,62,p);
	menubox(7,17,17,62,112,7);
	drawbox(7,17,16,60,112);

	len=strlen(str);
	writestring(str,9,39-len/2,112);
	menubox(11,21,12,56,32,NO_SHADOW);

	if(strcmp(str,"enter directory name")==0)
	{
		getcwd(currentdir,30);
		writestring(currentdir,11,22,47);
	}

	menubox(14,27,15,51,32,NO_SHADOW);

	esc_flag=get_name(11,22,name);
	restorevideo(7,17,17,62,p);
	free(p);
	return (esc_flag);
}



int get_name(int row,int col,char *p)
{
	int i=0;
	char str[30];
	size(5,7);
	getkey();
	while(1)
	{
		if(ascii==27)
			return (ESC);
		if(ascii==13)
			break;
		if(isascii(ascii) && !iscntrl(ascii))
		{
			str[i]=ascii;
			writechar(row,col,ascii,27);
		}

	}
	str[i]='\0';
	strcpy(p,str);
	size(32,0);
	return 0;
}


int load()
{
	FILE *fp;
	int i=0,flag=0,j=0;
	char ch[30];
	menubox(24,0,24,79,112,NO_SHADOW);
	write_fname();
	fp=fopen(filespec,"r");
	if(fp==NULL)
	{
		menubox(24,0,24,79,26,NO_SHADOW);
		writestring("create new file",24,1,112);
	}
	else
	{

		while(ch[i]!=EOF)
		{  ch[i]=fgetc(fp);
		writechar(9,7+i,ch[i],112);
			i++;
		}

	}

	  flag=1;
	fclose(fp);
	return flag;
}

int new1()
{
  int i=0;
  if(i==0)
	return 5;
}
int save()
{
int i=0;
if(i==0)
return 5;
}



int helpserver()
{ int i=0;
if(i==0)
return 0;}
int searchserver()
{int i=0;
if(i==0)
return 0;
}
int exitserver()
{int i=0;
if(i==0)
return 5;
}
