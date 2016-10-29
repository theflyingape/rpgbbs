/***********************************************\
 *  RPGBBS Deluxe Library of Common Functions  *
 *                                             *
 *           Written by Robert Hurst           *
\***********************************************/
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <exec/memory.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef	ULONG	JDATE;
#define RPGBBSLIB TRUE
#include "/include/rpgbbs.h"

#define ISLEAP (((year%4)==0) && (((year%100)!=0) || ((year%400)==0)))
#define MOVE(y,x) Move(con->Window->RPort,con->TextFont->tf_XSize*((x<con->MaxCol) ? x : con->MaxCol-1),con->TextFont->tf_YSize*y+con->TextFont->tf_Baseline);

extern	char *__montbl[],*__daytbl[];

const UBYTE __far alternate[] = {
	' ','\4','\261','h','f','c','l','\370',
	'\361','n','v','\331','\277','\332','\300','\305',
	'\304','\304','\304','\304','\304','\303','\264','\301',
	'\302','\263','\363','\362','\343','\35','\234','\372'
};
const UBYTE __far keyboard[5][104] = {
	{		/* Standard keymap */
	'`','1','2','3','4','5','6','7','8','9','0','-','=','\\','\0','0',
	'q','w','e','r','t','y','u','i','o','p','[',']','\0','1','2','3',
	'a','s','d','f','g','h','j','k','l',';','\'','\0','\0','4','5','6',
	'\0','z','x','c','v','b','n','m',',','.','/','\0','.','7','8','9',
	' ','\177','\11','\15','\15','\33','\0','\0','\0','\0','-','\0','\0','\0','\0','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','+','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0'
	}, {	/* CTRL */
	'~','1','2','3','4','5','6','7','8','9','0','-','=','\34','\0','0',
	'\21','\27','\5','\22','\24','\31','\25','\11','\17','\20','\33','\35','\0','1','2','3',
	'\1','\23','\4','\6','\7','\10','\12','\13','\14',';','\'','\0','\0','4','5','6',
	'\0','\32','\30','\3','\26','\2','\16','\15',',','.','/','\0','.','7','8','9',
	' ','\177','\11','\15','\15','\33','\0','\0','\0','\0','-','\0','\0','\0','\0','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','+','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0'
	}, {	/* ALT */
	'`','¹','²','³','¢','¼','½','¾','·','«','»','-','=','\\','\0','0',
	'å','°','©','®','þ','¤','µ','¡','ø','¶','[',']','\0','\0','\0','\0',
	'æ','ß','ð','f','g','h','j','k','£',';','\'','\0','\0','\0','\0','\0',
	'\0','±','×','ç','ª','º','­','¸',',','.','/','\0','.','\0','\0','\0',
	' ','\10','\11','\15','\15','\33','\0','\0','\0','\0','-','\0','\0','\0','\0','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','+','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0'
	}, {	/* SHIFT */
	'~','!','@','#','$','%','^','&','*','(',')','_','+','|','\0','0',
	'Q','W','E','R','T','Y','U','I','O','P','{','}','\0','1','2','3',
	'A','S','D','F','G','H','J','K','L',':','"','\0','\0','4','5','6',
	'\0','Z','X','C','V','B','N','M','<','>','?','\0','.','7','8','9',
	' ','\30','\0','\15','\15','\33','\0','\0','\0','\0','-','\0','\0','\0','\0','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','+','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0'
	}, {	/* CAPS */
	'`','1','2','3','4','5','6','7','8','9','0','-','=','\\','\0','0',
	'Q','W','E','R','T','Y','U','I','O','P','[',']','\0','1','2','3',
	'A','S','D','F','G','H','J','K','L',';','\'','\0','\0','4','5','6',
	'\0','Z','X','C','V','B','N','M',',','.','/','\0','.','7','8','9',
	' ','\177','\11','\15','\15','\33','\0','\0','\0','\0','-','\0','\0','\0','\0','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','+','\0',
	'\0','\0','\0','\0','\0','\0','\0','\0'
	}
};

void __asm GetSystemDate(register __a0 UBYTE *result)
{
	LONG t;

	time(&t);
	strftime(result,9,"%Y%m%d",localtime(&t));
}

void __asm GetSystemTime(register __a0 UBYTE *result)
{
	struct tm *tp;
	LONG t;

	time(&t);
	tp=localtime(&t);
	strftime(result,7,"%H%M%S",tp);
}

void __asm jdate(register __a0 UBYTE *result, register __d0 JDATE days)
{
	UWORD md[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
	UWORD month,day,year;

	year=days/1461*4+days%1461/365;
	days=days-(year*365L+year/4)+1;
	month=0;
	while(days>md[month+1]-(ISLEAP && month==0 ? 1 : 0) && month<11)
		month++;
	days-=md[month++];
	day=days;
	if(ISLEAP && month<3)
		day++;
	sprintf(result,"%04u%02u%02u",year,month,day);
}

void __asm fdate(register __a0 UBYTE *result, register __d0 JDATE days)
{
	UBYTE date[9];

	jdate(date,days);
	sprintf(result,"%.3s %.2s-%.3s-%.4s",__daytbl[(days-1)%7],&date[6],__montbl[10*(date[4]-'0')+(date[5]-'0')-1],&date[0]);
}

void __asm ftime(register __a0 UBYTE *result, register __a1 UBYTE *time)
{
	ULONG t;
	UBYTE hour[3],ap;

	t=atol(time)/10000L;
	ap=t<12 ? 'a' : 'p';
	sprintf(hour,"%2lu",(t>12 ? t-12 : (t<1 ? 12 : t)));
	sprintf(result,"%s:%.2s%cm",hour,&time[2],ap);
}

void __asm julian(register __a0 JDATE *result, register __a1 UBYTE *str)
{
	UWORD md[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
	ULONG month,day,year;
	UBYTE date[9],*p;

	GetSystemDate(date);
	sscanf(date,"%04d%02d%02d",&month,&day,&year);
	if((p=strchr(str,'/'))) {
		month=atoi(str);
		day=atoi(++p);
		if((p=strchr(p,'/')))
			year=atoi(++p);
	}
	else
	if((p=strchr(str,'-'))) {
		month=atoi(str);
		day=atoi(++p);
		if(!day) {
			day=month;
			for(month=0; month<12 && strnicmp(p,__montbl[month],3); month++);
			month++;
		}
		if((p=strchr(p,'-')))
			year=atoi(++p);
	}
	else
	if(atol(str)>19000000L)
		sscanf(str,"%04d%02d%02d",&year,&month,&day);
	else
		sscanf(str,"%02d%02d%d",&month,&day,&year);
	month=(month<1) ? 1 : (month>12) ? 12 : month;
	day=(day<1) ? 1 : (day>31) ? 31 : day;
	if(year<100)
		year+=1900;
	*result=year*365L+year/4+md[month-1]+day-1;
	if(ISLEAP && month<3)
		(*result)--;
}

void __asm gold(register __a0 DOUBLE *result, register __a1 UBYTE *str)
{
	DOUBLE gold;
	UBYTE *piece;

	*result=0.;
	gold=0.;
	for(piece=str; *piece; piece++) {
		switch(toupper(*piece)) {
			case 'C':
				*result+=gold;
				break;
			case 'S':
				*result+=gold*1e+05;
				break;
			case 'G':
				*result+=gold*1e+09;
				break;
			case 'P':
				*result+=gold*1e+13;
				break;
		}
		if(isdigit(*piece)) {
			gold*=10.;
			gold+=(*piece-'0');
		}
		else
			gold=0.;
	}
	*result+=gold;
}

void __asm phone(register __a0 UBYTE *str)
{
	int i;
	char check[5];

	i=0;
	if(str[i]=='+')
		i++;
	while(i<strlen(str)) {
		if(!isdigit(str[i]))
			memmove(&str[i],&str[i+1],strlen(&str[i]));
		else
			i++;
	}
	sprintf(check,"%.4s",str);
	i=atoi(check);
	if(i<10 || i/10==900 || i/10==911 || i==1900) {
		memset(str,0,strlen(str));
		*str='*';
	}
	if(strlen(str)>7) {
		sprintf(check,"%.3s",&str[3]);
		i=atoi(check);
		if(i<10 || i==900 || i==911) {
			memset(str,0,strlen(str));
			*str='*';
		}
	}
}

void __asm proper(register __a0 UBYTE *str)
{
	int i,f=TRUE;

	while(*str>0 && !isalnum(*str))
		memmove(str,&str[1],strlen(str));
	while((i=strlen(str))>0 && !isalnum(str[i-1]))
		str[i-1]=NULL;
	for(i=0; i<strlen(str); i++)
		if(str[i]==' ' && str[i+1]==' ')
			memmove(&str[i],&str[i+1],strlen(&str[i]));
	strlwr(str);
	for(i=0; i<strlen(str); i++) {
		if(f) {
			str[i]=toupper(str[i]);
			f=FALSE;
		}
		if(str[i]<'A')
			f=TRUE;
	}
}

int __asm cuss(register __a0 UBYTE *str)
{
	int i,result=FALSE;
	char buffer[120];
	char *cuss;

	strncpy(buffer,str,sizeof(buffer));
	buffer[sizeof(buffer)-1]=NULL;
	for(i=0; i<strlen(buffer); i++) {
		if(buffer[i]=='$')
			buffer[i]='s';
		while(ispunct(buffer[i]) || isspace(buffer[i]))
			memmove(&buffer[i],&buffer[i+1],strlen(&buffer[i]));
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'a'))) {
		if(!strncmp(cuss,"asshole",7))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'c'))) {
		if(!strncmp(cuss,"cunt",4))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'f'))) {
		if(!strncmp(cuss,"fuck",4))
			result=TRUE;
		if(!strncmp(cuss,"fck",3))
			result=TRUE;
		if(!strncmp(cuss,"fuk",3))
			result=TRUE;
		cuss++;
	}
	cuss=buffer;
	while((cuss=strchr(cuss,'p'))) {
		if(!strncmp(cuss,"phuc",4))
			result=TRUE;
		if(!strncmp(cuss,"phuk",4))
			result=TRUE;
		cuss++;
	}
	return(result);
}

void __asm current(register __a0 struct SERCON *con)
{
	int b,f;

	b=con->CC.Back;
	f=con->CC.Fore;
	if(con->CA.Bold)
		f|=(con->Window->WScreen->BitMap.Depth==3) ? 3 : 8;
	if(con->CA.Dim)
		f=(con->Window->WScreen->BitMap.Depth==3) ? 2 : 8;
	if(con->CA.Blink)
		f|=(con->Window->WScreen->BitMap.Depth==3) ? 4 : 16;
	if(con->CA.Rvs) {
		SetAPen(con->Window->RPort,b);
		SetBPen(con->Window->RPort,f);
	}
	else {
		SetAPen(con->Window->RPort,f);
		SetBPen(con->Window->RPort,b);
	}
	SetSoftStyle(con->Window->RPort,FS_NORMAL | ((con->CA.Ul) ? FSF_UNDERLINED : 0),0xFF);
	MOVE(con->Row,con->Col);
}

void __asm scroll(register __a0 struct SERCON *con, register __d0 int l)
{
	UWORD from,to,line,size;

	if(l>0) {
		from=(con->SRtop+l)*con->MaxCol;
		to=con->SRtop*con->MaxCol;
		line=(con->SRbottom-l+1)*con->MaxCol;
	}
	if(l<0) {
		from=con->SRtop*con->MaxCol;
		to=(con->SRtop-l)*con->MaxCol;
		line=con->SRtop*con->MaxCol;
	}
	size=(con->SRbottom-con->SRtop-abs(l)+1)*con->MaxCol;
	memmove(&con->ScreenBuffer[to],&con->ScreenBuffer[from],size);
	memmove(&con->AttrBuffer[to],&con->AttrBuffer[from],size);
	memmove(&con->ColorBuffer[to],&con->ColorBuffer[from],size);
	size=abs(l)*con->MaxCol;
	memset(&con->ScreenBuffer[line],' ',size);
	memset(&con->AttrBuffer[line],0,size);
	memset(&con->ColorBuffer[line],0,size);
	SetBPen(con->Window->RPort,BLK);
	ScrollRaster(con->Window->RPort,0,con->TextFont->tf_YSize*l,0,con->TextFont->tf_YSize*con->SRtop,con->Window->Width,con->TextFont->tf_YSize*(con->SRbottom+1));
	current(con);
}

void __asm DecodeANSI(register __a0 struct SERCON *con, register __a1 UBYTE *str)
{
	int i,n;
	UBYTE params[32],sp;

	if(str[1]=='[')
		str++;
	if(*str==0x9B || *str=='[') {	/*  control-sequence introduced  */
		sp=0;
		do {	/*  process multiple parameters  */
			str++;
			n=0;
			while(isdigit(*str)) {
				n=10*n+*str-'0';
				str++;
			}
			params[sp++]=n;
		} while(*str==';' && sp<32);
		switch(*str) {
			case '"':
				str++;
				if(*str=='q')
					con->CA.Protect=(params[0]==1);
				break;
			case '?':	//	setups
				sp=0;
				do {	//	process multiple parameters
					str++;
					n=0;
					while(isdigit(*str)) {
						n=10*n+*str-'0';
						str++;
					}
					params[sp++]=n;
				} while(*str==';' && sp<32);
				switch(*str) {
					case 'J':	/*  Clear erasable characters  */
						SetBPen(con->Window->RPort,BLK);
						switch(params[0]) {
							case 0:	/*  from cursor to end-of-screen  */
								n=con->Col;
								for(i=con->Row; i<con->MaxRow; i++) {
									sp=0;
									for(; n<con->MaxCol; n++) {
										if(!con->AttrBuffer[i*con->MaxCol+n].Protect)
											sp++;
										if(sp && (con->AttrBuffer[i*con->MaxCol+n].Protect || n+1==con->MaxCol)) {
											if(!con->AttrBuffer[i*con->MaxCol+n].Protect && n+1==con->MaxCol)
												n++;
											memset(&con->ScreenBuffer[i*con->MaxCol+n-sp],' ',sp);
											memset(&con->AttrBuffer[i*con->MaxCol+n-sp],0,sp);
											memset(&con->ColorBuffer[i*con->MaxCol+n-sp],0,sp);
											MOVE(i,n-sp);
											Text(con->Window->RPort,&con->ScreenBuffer[i*con->MaxCol+n-sp],sp);
											sp=0;
										}
									}
									n=0;
								}
								break;
							case 1: /*  from top to cursor  */
								for(i=con->Row; i>=0; i--) {
									sp=0;
									for(n=0; n<(i==con->Row ? con->Col : con->MaxCol); n++) {
										if(!con->AttrBuffer[i*con->MaxCol+n].Protect)
											sp++;
										if(sp && (con->AttrBuffer[i*con->MaxCol+n].Protect || n+1==con->MaxCol)) {
											if(!con->AttrBuffer[i*con->MaxCol+n].Protect && n+1==con->MaxCol)
												n++;
											memset(&con->ScreenBuffer[i*con->MaxCol+n-sp],' ',sp);
											memset(&con->AttrBuffer[i*con->MaxCol+n-sp],0,sp);
											memset(&con->ColorBuffer[i*con->MaxCol+n-sp],0,sp);
											MOVE(i,n-sp);
											Text(con->Window->RPort,&con->ScreenBuffer[i*con->MaxCol+n-sp],sp);
											sp=0;
										}
									}
								}
								break;
							case 2: /*  entire display  */
								for(i=0; i<con->MaxRow; i++) {
									sp=0;
									for(n=0; n<con->MaxCol; n++) {
										if(!con->AttrBuffer[i*con->MaxCol+n].Protect)
											sp++;
										if(sp && (con->AttrBuffer[i*con->MaxCol+n].Protect || n+1==con->MaxCol)) {
											if(!con->AttrBuffer[i*con->MaxCol+n].Protect && n+1==con->MaxCol)
												n++;
											memset(&con->ScreenBuffer[i*con->MaxCol+n-sp],' ',sp);
											memset(&con->AttrBuffer[i*con->MaxCol+n-sp],0,sp);
											memset(&con->ColorBuffer[i*con->MaxCol+n-sp],0,sp);
											MOVE(i,n-sp);
											Text(con->Window->RPort,&con->ScreenBuffer[i*con->MaxCol+n-sp],sp);
											sp=0;
										}
									}
								}
								break;
						}
						current(con);
						break;
					case 'K':	/*  clear line  */
						SetBPen(con->Window->RPort,BLK);
						i=con->Row*con->MaxCol;
						sp=0;
						switch(params[0]) {
							case 0:	/*  from cursor to end-of-line  */
								for(n=con->Col; n<con->MaxCol; n++) {
									if(!con->AttrBuffer[i+n].Protect)
										sp++;
									if(sp && (con->AttrBuffer[i+n].Protect || n+1==con->MaxCol)) {
										if(!con->AttrBuffer[i+n].Protect && n+1==con->MaxCol)
											n++;
										memset(&con->ScreenBuffer[i+n-sp],' ',sp);
										memset(&con->AttrBuffer[i+n-sp],0,sp);
										memset(&con->ColorBuffer[i+n-sp],0,sp);
										MOVE(i,n-sp);
										Text(con->Window->RPort,&con->ScreenBuffer[i+n-sp],sp);
										sp=0;
									}
								}
								break;
							case 1:	/*  from beginning-of-line to cursor  */
								for(n=0; n<con->Col; n++) {
									if(!con->AttrBuffer[i+n].Protect)
										sp++;
									if(sp && (con->AttrBuffer[i+n].Protect || n+1==con->Col)) {
										if(!con->AttrBuffer[i+n].Protect && n+1==con->Col)
											n++;
										memset(&con->ScreenBuffer[i+n-sp],' ',sp);
										memset(&con->AttrBuffer[i+n-sp],0,sp);
										memset(&con->ColorBuffer[i+n-sp],0,sp);
										MOVE(i,n-sp);
										Text(con->Window->RPort,&con->ScreenBuffer[i+n-sp],sp);
										sp=0;
									}
								}
								break;
							case 2:	/*  entire line  */
								for(n=0; n<con->MaxCol; n++) {
									if(!con->AttrBuffer[i+n].Protect)
										sp++;
									if(sp && (con->AttrBuffer[i+n].Protect || n+1==con->MaxCol)) {
										if(!con->AttrBuffer[i+n].Protect && n+1==con->MaxCol)
											n++;
										memset(&con->ScreenBuffer[i+n-sp],' ',sp);
										memset(&con->AttrBuffer[i+n-sp],0,sp);
										memset(&con->ColorBuffer[i+n-sp],0,sp);
										MOVE(i,n-sp);
										Text(con->Window->RPort,&con->ScreenBuffer[i+n-sp],sp);
										sp=0;
									}
								}
								break;
						}
						current(con);
						break;
					case 'h':	/*  setup on  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 7:
									con->Mode.AutoWrap=TRUE;
									break;
								case 25:
									con->Mode.CursorVisible=TRUE;
									break;
								case 66:
									con->Mode.Keypad=TRUE;
									break;
							}
						break;
					case 'i':	/*  Autoprint mode  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 4:
									if(con->Printer)
										fclose(con->Printer);
									con->Printer=NULL;
									break;
								case 5:
									con->Printer=fopen("PRT:","w");
									break;
							}
						break;
					case 'l':	/*  setup off  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 7:
									con->Mode.AutoWrap=FALSE;
									break;
								case 25:
									con->Mode.CursorVisible=FALSE;
									break;
								case 66:
									con->Mode.Keypad=FALSE;
									break;
							}
						break;
				}
				break;
			case '@':	/*  insert character  */
				if(!params[0])
					params[0]=1;
				i=con->Row*con->MaxCol+con->Col;
				n=con->MaxCol-con->Col-params[0];
				if(n>0) {
					memmove(&con->ScreenBuffer[i+params[0]],&con->ScreenBuffer[i],n);
					memmove(&con->AttrBuffer[i+params[0]],&con->AttrBuffer[i],n);
					memmove(&con->ColorBuffer[i+params[0]],&con->ColorBuffer[i],n);
				}
				n=(n>0) ? params[0] : con->MaxCol-con->Col;
				memset(&con->ScreenBuffer[i],' ',n);
				memset(&con->AttrBuffer[i],0,n);
				memset(&con->ColorBuffer[i],0,n);
				SetBPen(con->Window->RPort,BLK);
				ScrollRaster(con->Window->RPort,-con->TextFont->tf_XSize*n,0,con->TextFont->tf_XSize*(con->Col+1),con->TextFont->tf_YSize*(con->Row+1),con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+2));
				current(con);
				break;
			case 'A':	/*  up  */
				if(!params[0])
					params[0]=1;
				con->Row=(con->Row<params[0]) ? 0 : con->Row-params[0];
				break;
			case 'B':	/*  down  */
				if(!params[0])
					params[0]=1;
				con->Row=(con->Row+params[0]>=con->MaxRow) ? con->MaxRow-1 : con->Row+params[0];
				break;
			case 'C':	/*  right  */
				if(!params[0])
					params[0]=1;
				con->Col=(con->Col+params[0]>=con->MaxCol) ? con->MaxCol-1 : con->Col+params[0];
				break;
			case 'D':	/*  left  */
				if(!params[0])
					params[0]=1;
				con->Col=(con->Col<params[0]) ? 0 : con->Col-params[0];
				break;
			case 'E':
				if(!params[0])
					params[0]=1;
				con->Row=(con->Row+params[0]>=con->MaxRow) ? con->MaxRow-1 : con->Row+params[0];
				con->Col=0;
				break;
			case 'H':	/*  plot  */
				con->Row=(params[0]<1 ? 1 : params[0]>con->MaxRow ? con->MaxRow : params[0])-1;
				if(sp>1)
					con->Col=(params[1]<1 ? 1 : params[1]>con->MaxCol ? con->MaxCol : params[1])-1;
				else
					con->Col=0;
				break;
			case 'J':	/*  clear screen  */
				SetAPen(con->Window->RPort,BLK);
				if(n==0 && con->Row==0 && con->Col==0)
					n=2;
				switch(n) {
					case 0:	/*  from cursor to end-of-screen  */
						n=(con->MaxRow*con->MaxCol)-(con->Row*con->MaxCol+con->Col)+1;
						memset(&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],' ',n);
						memset(&con->AttrBuffer[con->Row*con->MaxCol+con->Col],0,n);
						memset(&con->ColorBuffer[con->Row*con->MaxCol+con->Col],0,n);
						n=con->Col;
						for(i=con->Row; i<con->MaxRow; i++) {
							RectFill(con->Window->RPort,con->TextFont->tf_XSize*n,con->TextFont->tf_YSize*i,con->TextFont->tf_XSize*(con->MaxCol+1),con->TextFont->tf_YSize*(i+1)-1);
							n=0;
						}
						break;
					case 1: /*  from top to cursor  */
						n=(con->Row*con->MaxCol+con->Col)+1;
						memset(&con->ScreenBuffer[0],' ',n);
						memset(&con->AttrBuffer[0],0,n);
						memset(&con->ColorBuffer[0],0,n);
						for(i=0; i<con->Row; i++)
							RectFill(con->Window->RPort,0,con->TextFont->tf_YSize*i,con->TextFont->tf_XSize*(con->MaxCol+1),con->TextFont->tf_YSize*(i+1)-1);
						MOVE(con->Row,0);
						Text(con->Window->RPort,&con->ScreenBuffer[con->Row*con->MaxCol],con->Col+1);
						break;
					case 2: /*  entire display  */
						n=con->MaxRow*con->MaxCol;
						memset(con->ScreenBuffer,' ',n);
						memset(con->AttrBuffer,0,n);
						memset(con->ColorBuffer,0,n);
						RectFill(con->Window->RPort,0,0,con->TextFont->tf_XSize*(con->MaxCol+1),con->TextFont->tf_YSize*con->MaxRow-1);
						break;
				}
				current(con);
				break;
			case 'K':	/*  clear line  */
				SetAPen(con->Window->RPort,BLK);
				switch(n) {
					case 0:	/*  from cursor to end-of-line  */
						n=con->MaxCol-con->Col;
						memset(&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],' ',n);
						memset(&con->AttrBuffer[con->Row*con->MaxCol+con->Col],0,n);
						memset(&con->ColorBuffer[con->Row*con->MaxCol+con->Col],0,n);
						RectFill(con->Window->RPort,con->TextFont->tf_XSize*con->Col,con->TextFont->tf_YSize*con->Row,con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+1)-1);
						break;
					case 1:	/*  from beginning-of-line to cursor  */
						memset(&con->ScreenBuffer[con->Row*con->MaxCol],' ',con->MaxCol);
						memset(&con->AttrBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						memset(&con->ColorBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						RectFill(con->Window->RPort,0,con->TextFont->tf_YSize*con->Row,con->TextFont->tf_XSize*(con->Col+1),con->TextFont->tf_YSize*(con->Row+1)-1);
						break;
					case 2:	/*  entire line  */
						memset(&con->ScreenBuffer[con->Row*con->MaxCol],' ',con->MaxCol);
						memset(&con->AttrBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						memset(&con->ColorBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						RectFill(con->Window->RPort,0,con->TextFont->tf_YSize*con->Row,con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+1)-1);
						break;
				}
				current(con);
				break;
			case 'L':	/*  insert line  */
				if(!params[0])
					params[0]=1;
				params[1]=con->SRtop;
				params[2]=con->SRbottom;
				con->SRtop=con->Row;
				con->SRbottom=con->MaxRow-1;
				scroll(con,-(int)params[0]);
				con->SRtop=params[1];
				con->SRbottom=params[2];
				break;
			case 'M':	/*  delete line  */
				if(!params[0])
					params[0]=1;
				params[1]=con->SRtop;
				params[2]=con->SRbottom;
				con->SRtop=con->Row;
				con->SRbottom=con->MaxRow-1;
				scroll(con,params[0]);
				con->SRtop=params[1];
				con->SRbottom=params[2];
				break;
			case 'P':	/*  delete character  */
				if(!params[0])
					params[0]=1;
				i=con->Row*con->MaxCol+con->Col;
				n=con->MaxCol-con->Col-params[0];
				if(n>0) {
					memmove(&con->ScreenBuffer[i],&con->ScreenBuffer[i+params[0]],n);
					memmove(&con->AttrBuffer[i],&con->AttrBuffer[i+params[0]],n);
					memmove(&con->ColorBuffer[i],&con->ColorBuffer[i+params[0]],n);
				}
				i=con->Row*con->MaxCol-con->Col+((n>0) ? n : 0);
				n=(n>0) ? params[0] : con->MaxCol-con->Col;
				memset(&con->ScreenBuffer[i],' ',n);
				memset(&con->AttrBuffer[i],0,n);
				memset(&con->ColorBuffer[i],0,n);
				SetBPen(con->Window->RPort,BLK);
				ScrollRaster(con->Window->RPort,con->TextFont->tf_XSize*n,0,con->TextFont->tf_XSize*(con->Col+1),con->TextFont->tf_YSize*(con->Row+1),con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+2));
				current(con);
				break;
			case 'X':	/*  erase  */
				SetBPen(con->Window->RPort,BLK);
				if((n=con->MaxCol-con->Col)<params[0])
					params[0]=n;
				memset(&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],' ',params[0]);
				memset(&con->AttrBuffer[con->Row*con->MaxCol+con->Col],0,params[0]);
				memset(&con->ColorBuffer[con->Row*con->MaxCol+con->Col],0,params[0]);
				Text(con->Window->RPort,&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],params[0]);
				current(con);
				break;
			case 'Z':	/*  reverse tab  */
				if(con->Col)
					con->Col-=(8-con->Col%8);
				break;
			case 'c':	/*  identify  */
				if(con->SerialIO)
					WriteSerial(con,"\33[?62c");
				break;
			case 'f':	/*  plot  */
				con->Row=(params[0]<1 ? 1 : params[0]>con->MaxRow ? con->MaxRow : params[0])-1;
				if(sp>1)
					con->Col=(params[1]<1 ? 1 : params[1]>con->MaxCol ? con->MaxCol : params[1])-1;
				else
					con->Col=0;
				break;
			case 'h':	/*  mode set  */
				for(i=0; i<sp; i++)
					switch(params[i]) {
						case 4:
							con->Mode.Insert=TRUE;
							break;
						case 20:
							con->Mode.Newline=TRUE;
							break;
						case 33:
							con->Mode.CursorSteady=TRUE;
							break;
						case 34:
							break;
					}
				break;
			case 'i':	/*  controller print mode  */
				for(i=0; i<sp; i++)
					switch(params[i]) {
						case 4:
							if(con->Printer)
								fclose(con->Printer);
							con->Printer=NULL;
							break;
						case 5:
							con->Printer=fopen("PRT:","w");
							break;
					}
				break;
			case 'l':	/*  mode reset  */
				for(i=0; i<sp; i++)
					switch(params[i]) {
						case 4:
							con->Mode.Insert=FALSE;
							break;
						case 20:
							con->Mode.Newline=FALSE;
							break;
						case 33:
							con->Mode.CursorSteady=FALSE;
							break;
						case 34:
							break;
					}
				break;
			case 'm':	/*  cursor attribute  */
				for(i=0; i<sp; i++) {
					n=params[i];
					if(n==0) {	/*  normal  */
						con->CC.Fore=(con->Window->WScreen->BitMap.Depth==3) ? 1 : GRY;
						con->CA.Bold=FALSE;
						con->CC.Back=0;
						con->CA.Rvs=FALSE;
						con->CA.Blink=FALSE;
						con->CA.Dim=FALSE;
						con->CA.Ul=FALSE;
					}
					else
					if(n==1) {	/*  bold  */
						con->CA.Bold=TRUE;
						con->CA.Dim=FALSE;
					}
					else
					if(n==2) {	/*  dim  */
						con->CA.Bold=FALSE;
						con->CA.Dim=TRUE;
					}
					else
					if(n==4)	/*  underline  */
						con->CA.Ul=TRUE;
					else
					if(n==5)	/*  blink  */
						con->CA.Blink=TRUE;
					else
					if(n==7)
						con->CA.Rvs=TRUE;
					else
					if(n==22) {
						con->CA.Bold=FALSE;
						con->CA.Dim=FALSE;
					}
					else
					if(n==25)
						con->CA.Blink=FALSE;
					else
					if(n==27)
						con->CA.Rvs=FALSE;
					else
					if(n>=30 && n<=37) {	/*  foreground color  */
						n-=30;
						if(n)
							if(con->Window->WScreen->BitMap.Depth==3)
								n=(con->CA.Bold) ? 3 : (con->CA.Dim) ? 2 : 1;
						con->CA.Dim=FALSE;
						con->CC.Fore=n;
					}
					else
					if(n>=40 && n<=47)	/*  background color  */
						con->CC.Back=(con->Window->WScreen->BitMap.Depth==3 && n>40) ? 2 : (n-40);
				}
				current(con);
				break;
			case 'r':
				con->SRtop=(params[0]<1 ? 1 : params[0]>con->MaxRow ? con->MaxRow : params[0])-1;
				if(sp>1)
					con->SRbottom=(params[1]<=con->SRtop ? con->SRtop+1 : params[1]>con->MaxRow ? con->MaxRow : params[1])-1;
				con->Row=0;
				con->Col=0;
				break;
			case 's':
				con->SaveCA=con->CA;
				con->SaveCC=con->CC;
				con->SaveMode=con->Mode;
				con->SaveRow=con->Row;
				con->SaveCol=con->Col;
				break;
			case 'u':
				con->CA=con->SaveCA;
				con->CC=con->SaveCC;
				con->Mode=con->SaveMode;
				con->Row=con->SaveRow;
				con->Col=con->SaveCol;
				current(con);
				break;
		}
	}
	else {
		switch(str[1]) {
			case '=':
				con->Mode.Keypad=TRUE;
				break;
			case '>':
				con->Mode.Keypad=FALSE;
				break;
			case 'D':	/*  index  */
				if(con->Row==con->SRbottom)
					scroll(con,1);
				else
					con->Row++;
				if(con->Row>=con->MaxRow)
					con->Row=con->MaxRow-1;
				break;
			case 'M':	/*  reverse index  */
				if(con->Row==con->SRtop)
					scroll(con,-1);
				else
				if(con->Row)
					con->Row--;
				break;
			case 'c':	/*  reset  */
				con->SRtop=0;
				con->SRbottom=con->MaxRow-1;
				con->CA.Alternate=FALSE;
				con->CA.Blink=FALSE;
				con->CA.Bold=FALSE;
				con->CA.Dim=FALSE;
				con->CA.Protect=FALSE;
				con->CA.Rvs=FALSE;
				con->CA.Ul=FALSE;
				con->CC.Back=BLK;
				con->CC.Fore=GRY;
				con->Mode.AutoWrap=TRUE;
				con->Mode.CursorSteady=FALSE;
				con->Mode.CursorUnderScore=FALSE;
				con->Mode.CursorVisible=TRUE;
				con->Mode.Draw=FALSE;
				con->Mode.Font=0;
				con->Mode.G0=0;
				con->Mode.G1=0;
				con->Mode.Insert=FALSE;
				con->Mode.Keypad=FALSE;
				if(con->Window) {
					con->CC.Fore=(con->Window->WScreen->BitMap.Depth==3) ? 1 : GRY;
					WriteConsole(con,"\233H\233J");
				}
				break;
			case 'Z':	/*  identify  */
				if(con->SerialIO)
					WriteSerial(con,"\33[?62c");
				break;
		}
	}
	MOVE(con->Row,con->Col);
}

void __asm DrawCursor(register __a0 struct SERCON *con, register __d0 UBYTE sw)
{
	struct Attribute a=con->CA;
	struct CellColor c=con->CC;
	int p=con->Row*con->MaxCol+((con->Col<con->MaxCol) ? con->Col : con->MaxCol-1);

	if(!con->Window || !con->ScreenBuffer || !con->Mode.CursorVisible || con->Mode.Draw)
		return;
	if(sw) {
		con->CA.Bold=FALSE;
		con->CA.Dim=FALSE;
		con->CA.Rvs=FALSE;
		con->CC.Back=(con->Window->WScreen->BitMap.Depth==3) ? 2 : BLACK;
		con->CC.Fore=(con->Window->WScreen->BitMap.Depth==3) ? 3 : WHITE;
	}
	else {
		con->CA=con->AttrBuffer[p];
		con->CC=con->ColorBuffer[p];
	}
	current(con);
	Text(con->Window->RPort,&con->ScreenBuffer[p],1);
	con->CA=a;
	con->CC=c;
	current(con);
}

void __asm input(register __a0 struct SERCON *con, register __a1 UBYTE *buffer, register __a2 UBYTE *source,register __d0 ULONG num,register __d1 ULONG timeout)
{
	struct AppMessage *appmessage;
	struct IntuiMessage *message;
	ULONG bytes,i,ticks;
	ULONG ConsoleSignal,IconSignal,SerialSignal,TimerSignal,signal;
	ULONG MessageClass;
	UWORD code,mousex,mousey,qualifier;
	UBYTE flash=FALSE,Qserial;

	memset(buffer,0,num);
	*source=0;
	ticks=2*timeout;
	ConsoleSignal=(con->Window!=NULL) ? 1L << con->Window->UserPort->mp_SigBit : 0L;
	IconSignal=(con->IconMP!=NULL) ? 1L << con->IconMP->mp_SigBit : 0L;
	SerialSignal=(con->SerialMP!=NULL) ? 1L << con->SerialMP->mp_SigBit : 0L;
	TimerSignal=(con->TPort!=NULL) ? 1L << con->TPort->mp_SigBit : 0L;
	Qserial=SerialSignal;
	signal=ConsoleSignal | IconSignal | SerialSignal | TimerSignal;
	if(!signal) {
		*buffer=getch();
		*source=1;
		return;
	}
	FOREVER {
		if(signal & TimerSignal) {
			con->TimerIO->tr_node.io_Command=TR_ADDREQUEST;
			con->TimerIO->tr_time.tv_secs=0;
			con->TimerIO->tr_time.tv_micro=500000L;
			SendIO((struct IORequest *)con->TimerIO);
			ticks--;
			if(!ticks)
				break;
			flash^=1;
			DrawCursor(con,(flash || con->Mode.CursorSteady));
			if(timeout)
				signal|=SerialSignal;	//  force it to check carrier
		}
		if(ConsoleSignal && !strlen(buffer))
			if((message=(struct IntuiMessage *)GetMsg(con->Window->UserPort))) {
				MessageClass=message->Class;
				qualifier=message->Qualifier;
				code=message->Code;
				mousex=con->Window->GZZMouseX;
				mousey=con->Window->GZZMouseY;
				ReplyMsg((struct Message *)message);
				if((MessageClass & IDCMP_MOUSEBUTTONS) && (code==SELECTDOWN)) {
					bytes=mousey/con->TextFont->tf_YSize*con->MaxCol+mousex/con->TextFont->tf_XSize;
					sprintf(buffer,"%c",con->ScreenBuffer[bytes]);
					if(con->ScreenBuffer[bytes]=='?')
						strcat(buffer,"\15");
					for(i=0; isdigit(con->ScreenBuffer[bytes+i]); i++)
						sprintf(&buffer[i],"%c\15",con->ScreenBuffer[bytes+i]);
					if(isdigit(con->ScreenBuffer[bytes]) && strchr("csgp",con->ScreenBuffer[bytes+i]))
						sprintf(&buffer[i],"%c\15",con->ScreenBuffer[bytes+i]);
					*source=2;
					break;
				}
				if((MessageClass & IDCMP_RAWKEY) && !(code & IECODE_UP_PREFIX)) {
					bytes=(qualifier & IEQUALIFIER_CONTROL) ? 1
						: (qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT)) ? 2
						: (qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) ? 3
						: (qualifier & IEQUALIFIER_CAPSLOCK) ? 4
						: 0;
					*buffer=keyboard[bytes][code];
					if(*buffer==NULL || con->Mode.Keypad)
						switch(code) {
							case 0x0f:	/*  KP0  */
								strcpy(buffer,"\33Op");
								break;
							case 0x1d:	/*  KP1  */
								strcpy(buffer,"\33Oq");
								break;
							case 0x1e:	/*  KP2  */
								strcpy(buffer,"\33Or");
								break;
							case 0x1f:	/*  KP3  */
								strcpy(buffer,"\33Os");
								break;
							case 0x2d:	/*  KP4  */
								strcpy(buffer,"\33Ot");
								break;
							case 0x2e:	/*  KP5  */
								strcpy(buffer,"\33Ou");
								break;
							case 0x2f:	/*  KP6  */
								strcpy(buffer,"\33Ov");
								break;
							case 0x3c:	/*  KP.  */
								strcpy(buffer,"\33On");
								break;
							case 0x3d:	/*  KP7  */
								strcpy(buffer,"\33Ow");
								break;
							case 0x3e:	/*  KP8  */
								strcpy(buffer,"\33Ox");
								break;
							case 0x3f:	/*  KP9  */
								strcpy(buffer,"\33Oy");
								break;
							case 0x43:	/*  Enter  */
								strcpy(buffer,"\33OM");
								break;
							case 0x4a:	/*  KP-  */
								strcpy(buffer,"\33Om");
								break;
							case 0x5e:	/*  KP+  */
								strcpy(buffer,"\33Ol");
								break;
						}
					if(*buffer==NULL)
						switch(code) {
							case 0x42:	/*  shift-tab  */
								strcpy(buffer,"\33[Z");
								break;
							case 0x46:	/*  Do  */
								strcpy(buffer,"\33[29~");
								break;
							case 0x4c:	/*  Up  */
								strcpy(buffer,"\33[A");
								break;
							case 0x4d:	/*  Down  */
								strcpy(buffer,"\33[B");
								break;
							case 0x4e:	/*  Right  */
								strcpy(buffer,"\33[C");
								break;
							case 0x4f:	/*  Left  */
								strcpy(buffer,"\33[D");
								break;
							case 0x5a:	/*  PF1  */
								strcpy(buffer,"\33OP");
								break;
							case 0x5b:	/*  PF2  */
								strcpy(buffer,"\33OQ");
								break;
							case 0x5c:	/*  PF3  */
								strcpy(buffer,"\33OR");
								break;
							case 0x5d:	/*  PF4  */
								strcpy(buffer,"\33OS");
								break;
							case 0x5f:	/*  Help  */
								strcpy(buffer,"\33[28~");
								break;
						}
					if(*buffer) {
						*source=2;
						break;
					}
				}
				if(MessageClass & IDCMP_MENUPICK) {
					buffer[0]=MENUNUM(code);
					buffer[1]=ITEMNUM(code);
					buffer[2]=SUBNUM(code);
					buffer[3]=NULL;
					*source=3;
					break;
				}
				if(MessageClass & IDCMP_CLOSEWINDOW) {
					*source=4;
					break;
				}
				if(MessageClass & IDCMP_NEWSIZE) {
					CloseConsole(con);
					OpenConsole(con);
				}
			}
		if(signal & IconSignal)
			if((appmessage=(struct AppMessage *)GetMsg(con->IconMP))) {
				ReplyMsg((struct Message *)appmessage);
				*source=5;
				break;
			}
		if(signal & SerialSignal) {
			if(!Qserial) {
				if(!CheckIO((struct IORequest *)con->SerialIO))
					AbortIO((struct IORequest *)con->SerialIO);
				WaitIO((struct IORequest *)con->SerialIO);
			}
			con->SerialIO->IOSer.io_Command=SDCMD_QUERY;
			DoIO((struct IORequest *)con->SerialIO);
			Qserial=TRUE;
			if(timeout)
				if(con->SerialIO->io_Status & 32)
					break;
			i=strlen(buffer);
			bytes=(con->SerialIO->IOSer.io_Actual<num-i-1) ? con->SerialIO->IOSer.io_Actual : num-i-1;
			if(bytes) {
				con->SerialIO->IOSer.io_Command=CMD_READ;
				con->SerialIO->IOSer.io_Length=bytes;
				con->SerialIO->IOSer.io_Data=(APTR)&buffer[i];
				DoIO((struct IORequest *)con->SerialIO);
			}
			if(strlen(buffer)) {
				*source=1;
				break;
			}
			con->SerialIO->IOSer.io_Command=CMD_READ;
			con->SerialIO->IOSer.io_Length=1;
			con->SerialIO->IOSer.io_Data=(APTR)buffer;
			SetSignal(0,SerialSignal);
			SendIO((struct IORequest *)con->SerialIO);
			Qserial=FALSE;
		}
		signal=Wait(ConsoleSignal | IconSignal | SerialSignal | TimerSignal);
	}
	if(con->TimerIO) {
		if(!CheckIO((struct IORequest *)con->TimerIO))
			AbortIO((struct IORequest *)con->TimerIO);
		WaitIO((struct IORequest *)con->TimerIO);
	}
	if(!Qserial && con->SerialIO) {
		if(!CheckIO((struct IORequest *)con->SerialIO))
			AbortIO((struct IORequest *)con->SerialIO);
		WaitIO((struct IORequest *)con->SerialIO);
	}
	if(flash || con->Mode.CursorSteady)
		DrawCursor(con,FALSE);
}

void __asm output(register __a0 struct SERCON *con, register __a1 UBYTE *str)
{
	if(con->Window)
		WriteConsole(con,str);
	if(con->SerialIO)
		WriteSerial(con,str);
	if(!con->Window && !con->SerialIO) {
		printf(str);
		fflush(stdout);
	}
}

void __asm plot(register __a0 struct SERCON *con, register __d0 UBYTE row, register __d1 UBYTE col)
{
	UBYTE str[11];

	sprintf(str,"\33[%hu;%huH",row,col);
	output(con,str);
}

void __asm OpenConsole(register __a0 struct SERCON *con)
{
	con->MaxRow=(con->Window) ? (con->Window->Height-con->Window->WScreen->WBorTop-con->Window->WScreen->WBorBottom-con->Window->WScreen->Font->ta_YSize)/con->TextFont->tf_YSize : 24;
	con->MaxCol=(con->Window) ? (con->Window->Width-con->Window->WScreen->WBorLeft-con->Window->WScreen->WBorRight)/con->TextFont->tf_XSize : 80;
	con->AttrBuffer=calloc(con->MaxRow,con->MaxCol);
	con->ColorBuffer=calloc(con->MaxRow,con->MaxCol);
	con->ScreenBuffer=calloc(con->MaxRow,con->MaxCol);
	memset(con->ScreenBuffer,' ',con->MaxRow*con->MaxCol);
	con->EscapeBuffer=calloc(1,256);
	con->Printer=NULL;
	con->Row=0;
	con->Col=0;
	con->SRtop=0;
	con->SRbottom=con->MaxRow-1;
	con->CA.Alternate=FALSE;
	con->CA.Blink=FALSE;
	con->CA.Bold=FALSE;
	con->CA.Dim=FALSE;
	con->CA.Protect=FALSE;
	con->CA.Rvs=FALSE;
	con->CA.Ul=FALSE;
	con->CC.Back=BLK;
	con->CC.Fore=GRY;
	con->Mode.AutoWrap=TRUE;
	con->Mode.CursorSteady=FALSE;
	con->Mode.CursorUnderScore=FALSE;
	con->Mode.CursorVisible=TRUE;
	con->Mode.Draw=FALSE;
	con->Mode.Font=0;
	con->Mode.G0=0;
	con->Mode.G1=0;
	con->Mode.Insert=FALSE;
	con->Mode.Keypad=FALSE;
}

void __asm CloseConsole(register __a0 struct SERCON *con)
{
	if(con->Printer) {
		fclose(con->Printer);
		con->Printer=NULL;
	}
	if(con->EscapeBuffer)
		free(con->EscapeBuffer);
	if(con->ScreenBuffer)
		free(con->ScreenBuffer);
	if(con->ColorBuffer)
		free(con->ColorBuffer);
	if(con->AttrBuffer)
		free(con->AttrBuffer);
}

void __asm WriteConsole(register __a0 struct SERCON *con, register __a1 UBYTE *str)
{
//	register UBYTE *base;
//	UWORD b;
	UWORD i,p;
	UBYTE color;

	if(!con->Window) {
		printf(str);
		fflush(stdout);
		return;
	}

	if(con->Printer)
		fprintf(con->Printer,str);

	if(!con->ScreenBuffer)
		return;

	while(*str) {
		if(con->Mode.Draw) {
			switch(*str) {
				case 0x0a:
					con->PXLcol=con->TextFont->tf_XSize*(con->Col+1);
					con->PXLrow++;
					if(con->PXLrow>=con->TextFont->tf_YSize) {
						if(con->Row==con->SRbottom)
							scroll(con,1);
						else
							con->Row++;
						if(con->Row>=con->MaxRow)
							con->Row=con->MaxRow-1;
						con->PXLrow=0;
					}
					con->PXLstart=0;
					break;
				case 0x0b:
					if(con->SerialIO)
						WriteSerial(con,"\6");
					break;
				case 0x0d:
					con->Mode.Draw=FALSE;
					if(con->Row==con->SRbottom)
						scroll(con,1);
					else
						con->Row++;
					if(con->Row>=con->MaxRow)
						con->Row=con->MaxRow-1;
					con->Col=0;
					current(con);
					break;
				default:
					if(*str<8) {
						con->PXLstart+=*str*15;
						break;
					}
					color=*str & 0x0F;
					if(con->Window->WScreen->BitMap.Depth==3)
						color=(color==BLK) ? 0 : (color==RED || color==GRN || color==BLU || color==BLACK) ? 2 : (color & 0x08) ? 3 : 1;
					Move(con->Window->RPort,con->PXLcol,con->TextFont->tf_YSize*con->Row+con->PXLrow);
//					b=con->PXLcol;
					con->PXLcol+=con->PXLstart+(*str>>4);
					SetAPen(con->Window->RPort,color);
					Draw(con->Window->RPort,con->PXLcol,con->TextFont->tf_YSize*con->Row+con->PXLrow);
					con->PXLstart=0;
/*					for(p=0; p<con->Window->WScreen->BitMap.Depth; p++)
						if(color & (1<<p)) {
							base=con->Window->RPort->BitMap->Planes[p];
							base+=(15+8*con->Row+con->PXLrow)*con->Window->RPort->BitMap->BytesPerRow;
							for(i=b; i<con->PXLcol; i++)
								base[i/8]|=128>>(i%8);
						}
*/					break;
			}
			str++;
			continue;
		}
		switch(*str) {
			case 0x05:	/*  Enq  */
				if(con->SerialIO) {
					WriteSerial(con,"\30");
					if(con->AnswerBack)
						WriteSerial(con,con->AnswerBack);
				}
				break;
			case 0x07:	/*  bel  */
				DisplayBeep(NULL);
				break;
			case 0x08:	/*  bs  */
				if(con->Col)
					con->Col--;
				MOVE(con->Row,con->Col);
				break;
			case 0x09:	/*  tab  */
				con->Col+=(8-con->Col%8);
				if(con->Col>=con->MaxCol)
					con->Col=con->MaxCol-1;
				MOVE(con->Row,con->Col);
				break;
			case 0x0A:	/*  lf  */
				if(con->Row==con->SRbottom)
					scroll(con,1);
				else
					con->Row++;
				if(con->Row>=con->MaxRow)
					con->Row=con->MaxRow-1;
				if(con->Mode.Newline)
					con->Col=0;
				MOVE(con->Row,con->Col);
				break;
			case 0x0C:	/*  ff  */
				if(con->Window)
					WriteConsole(con,"\33[H\33[J");
				break;
			case 0x0D:	/*  cr  */
				con->Col=0;
				MOVE(con->Row,con->Col);
				break;
			case 0x0E:	/*  SO  */
				con->Mode.Font=1;
				con->CA.Alternate=(con->Mode.G1);
				break;
			case 0x0F:	/*  SI  */
				con->Mode.Font=0;
				con->CA.Alternate=(con->Mode.G0);
				break;
			case 0x18:	/*  CAN  */
				con->EscapeBuffer[0]=NULL;
				break;
			case 0x1B:	/*  esc  */
				con->EscapeBuffer[0]=*str;
				con->EscapeBuffer[1]=NULL;
				break;
			case 0x9B:	/*  csi  */
				con->EscapeBuffer[0]=*str;
				con->EscapeBuffer[1]=NULL;
				break;
			default:
				if(con->EscapeBuffer[0]) {
					p=strlen(con->EscapeBuffer);
					con->EscapeBuffer[p++]=*str;
					con->EscapeBuffer[p]=NULL;
					if(p==2) {
						if(!strncmp(con->EscapeBuffer,"\33$",2)) {
							con->Mode.Draw=TRUE;
							con->PXLcol=8*con->Col+8;
							con->PXLrow=0;
							con->PXLstart=0;
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33\67",2)) {
							con->SaveCA=con->CA;
							con->SaveCC=con->CC;
							con->SaveMode=con->Mode;
							con->SaveRow=con->Row;
							con->SaveCol=con->Col;
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33\70",2)) {
							con->CA=con->SaveCA;
							con->CC=con->SaveCC;
							con->Mode=con->SaveMode;
							con->Row=con->SaveRow;
							con->Col=con->SaveCol;
							current(con);
							con->EscapeBuffer[0]=NULL;
							break;
						}
					}
					if(p==3) {
						if(!strncmp(con->EscapeBuffer,"\33#",2)) {
							switch(*str) {
								case 3:	/*  top half  */
									break;
								case 4:	/*  bottom half  */
									break;
								case 5:	/*  normal line  */
									break;
								case 6:	/*  double-width  */
									break;
							}
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33(",2)) {
							con->Mode.G0=(*str=='0') ? TRUE : FALSE;
							if(con->Mode.Font==0)
								con->CA.Alternate=(con->Mode.G0);
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33)",2)) {
							con->Mode.G1=(*str=='0') ? TRUE : FALSE;
							if(con->Mode.Font==1)
								con->CA.Alternate=(con->Mode.G1);
							con->EscapeBuffer[0]=NULL;
							break;
						}
					}
					if(!strchr("\"#()0123456789;?[",*str) || p==255) {
						DecodeANSI(con,con->EscapeBuffer);
						con->EscapeBuffer[0]=NULL;
					}
					break;
				}
				if(con->Col>=con->MaxCol && con->Window)
					WriteConsole(con,"\15\12");
				for(p=0; str[p]>=' ' && str[p]!=0x9b && con->Col+p<con->MaxCol; p++);
				if(p) {
					if(con->Mode.Insert && con->Window) {
						char ins[8];
						sprintf(ins,"\233%u@",p);
						WriteConsole(con,ins);
					}
					for(i=0; i<p; i++) {
						con->ScreenBuffer[con->Row*con->MaxCol+con->Col+i]=(con->CA.Alternate && str[i]>='\137' && str[i]<='\176') ? alternate[str[i]-'\137'] : str[i];
						con->AttrBuffer[con->Row*con->MaxCol+con->Col+i]=con->CA;
						con->ColorBuffer[con->Row*con->MaxCol+con->Col+i]=con->CC;
					}
					Text(con->Window->RPort,&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],p);
					con->Col+=p;
					str+=p-1;
				}
				break;
		}
		str++;
	}
}

void __asm WriteSerial(register __a0 struct SERCON *con, register __a1 UBYTE *str)
{
	if((con->SerialIO->IOSer.io_Length=strlen(str))) {
		con->SerialIO->IOSer.io_Command=CMD_WRITE;
		con->SerialIO->IOSer.io_Data=(APTR)str;
		DoIO((struct IORequest *)con->SerialIO);
	}
}

void __asm SendPXL(register __a0 struct SERCON *con, register __a1 UBYTE *pxl)
{
	UWORD d;
	UBYTE *b,*c;
	UBYTE filler[4],source;

	if(con->SerialIO) {
		WriteSerial(con,"\33$");
		b=pxl;
		d=0;
		while((c=strchr(b,'\12'))) {
			*c=NULL;
			d+=strlen(b);
			WriteSerial(con,b);
			WriteSerial(con,"\12");
			if(d>4000) {
				d=0;
				WriteSerial(con,"\13");
				memset(filler,0,sizeof(filler));
				input(con,filler,&source,sizeof(filler),5);
				if(!source)
					break;
			}
			b=c+1;
		}
		WriteSerial(con,"\15");
	}
	else {
		WriteConsole(con,"\33$");
		WriteConsole(con,pxl);
		WriteConsole(con,"\15");
	}
}

void __stdargs _XCEXIT(long l)
{
}
