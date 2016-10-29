/*************************\
 *  RPGBBS I/O Routines  *
\*************************/
#include "RPGBBS.h"

#define MOVE(y,x) Move(window->RPort,8*x,8*y+21);

void DecodeANSI(char *str)
{
	int csi,i,n;
	UBYTE params[16],sp;
	
	if(str[1]=='[')
		str++;
	csi=(*str==0x9B || *str=='[');
	sp=0;

	do {	/*  process multiple parameters  */
		str++;
		n=0;
		while(isdigit(*str)) {
			n=10*n+*str-'0';
			str++;
		}
		params[sp++]=n;
	} while(*str==';' && sp<16);

	if(csi) {	/*  control-sequence introduced  */
		switch(*str) {
			case '?':	/*  setups  */
				do {	/*  process multiple parameters  */
					str++;
					n=0;
					while(isdigit(*str)) {
						n=10*n+*str-'0';
						str++;
					}
					params[sp++]=n;
				} while(*str==';' && sp<16);
				switch(*str) {
					case 'h':	/*  setup on  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 25:	/*  cursor visible */
									Cflag=TRUE;
									break;
							}
						break;
					case 'l':	/*  setup off  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 25:	/*  cursor visible */
									Cflag=FALSE;
									break;
							}
						break;
				}
				break;
			case 'A':	/*  up  */
				if(!params[0])
					params[0]=1;
				Cy=(Cy<params[0]) ? 0 : Cy-params[0];
				break;
			case 'B':	/*  down  */
				if(!params[0])
					params[0]=1;
				Cy=(Cy+params[0]>MAXsy) ? MAXsy : Cy+params[0];
				break;
			case 'C':	/*  right  */
				if(!params[0])
					params[0]=1;
				Cx=(Cx+params[0]>MAXsx) ? MAXsx : Cx+params[0];
				break;
			case 'D':	/*  left  */
				if(!params[0])
					params[0]=1;
				Cx=(Cx<params[0]) ? 0 : Cx-params[0];
				break;
			case 'H':	/*  plot  */
				Cy=(params[0]>0 && params[0]<=MAXsy-1) ? params[0]-1 : 0;
				if(sp>1)
					Cx=(params[1]>0 && params[1]<=MAXsx-1) ? params[1]-1 : 0;
				break;
			case 'J':	/*  clear screen  */
				switch(n) {
					case 0:	/*  from cursor to end  */
						memset(&ScreenBuf[Cy][Cx],' ',sizeof(ScreenBuf)-80*Cy-Cx);
						memset(&AttrBuf[Cy][Cx],7,sizeof(AttrBuf)-80*Cy-Cx);
						SetRast(window->RPort,Cb);
						break;
					case 1: /*  from top to cursor  */
						memset(ScreenBuf,' ',80*Cy+Cx);
						memset(AttrBuf,7,80*Cy+Cx);
						SetRast(window->RPort,Cb);
						break;
					case 2: /*  entire display  */
						memset(ScreenBuf,' ',sizeof(ScreenBuf));
						memset(AttrBuf,7,sizeof(AttrBuf));
						SetRast(window->RPort,Cb);
						break;
				}
			case 'K':	/*  clear line  */
				memset(&ScreenBuf[Cy][Cx],' ',80-Cx);
				memset(&AttrBuf[Cy][Cx],7,80-Cx);
				Text(window->RPort,"                                                                                ",80-Cx);
				break;
			case 'f':	/*  plot  */
				Cy=(params[0]>0 && params[0]<=MAXsy-1) ? params[0]-1 : 0;
				if(sp>1)
					Cx=(params[1]>0 && params[1]<=MAXsx-1) ? params[1]-1 : 0;
				break;
			case 'm':	/*  cursor attribute  */
				for(i=0; i<sp; i++) {
					n=params[i];
					if(n==0) {	/*  normal  */
						SetSoftStyle(window->RPort,FS_NORMAL,0xFF);
						Ca=(BBSprefs.Depth<3) ? 1 : 7;
						Cb=0;
						RVSflag=FALSE;
					}
					else
					if(n==1) {	/*  bold  */
						if(BBSprefs.Depth & 1)
							SetSoftStyle(window->RPort,FSF_BOLD,0xFF);
						else
							Ca|=(BBSprefs.Depth==2) ? 2 : 8;
					}
					else
					if(n==2) {	/*  dim  */
						SetSoftStyle(window->RPort,FS_NORMAL,0xFF);
						Ca&=(BBSprefs.Depth==2) ? 1 : 7;
					}
					else
					if(n==4)	/*  underline  */
						SetSoftStyle(window->RPort,FSF_UNDERLINED,0xFF);
					else
					if(n==5)	/*  blink  */
						SetSoftStyle(window->RPort,FSF_ITALIC,0xFF);
					else
					if(n==7)	/*  reverse  */
						RVSflag=0x80;
					else
					if(n==22) {	/*  bold off  */
						if(BBSprefs.Depth & 1)
							SetSoftStyle(window->RPort,FS_NORMAL,0xFF);
						else
							Ca=(BBSprefs.Depth==2) ? 1 : (Ca & 7);
					}
					else
					if(n>=30 && n<=37) {	/*  foreground color  */
						if(BBSprefs.Depth>2)
							Ca=(Ca & 8)+(n-30);
						else
							Ca=(Ca & BBSprefs.Depth)|1;
					}
					else
					if(n>=40 && n<=47) {	/*  background color  */
						if(n==40)
							Cb=0;
						else if(BBSprefs.Depth>2)
							Cb=n-40;
						else if(BBSprefs.Depth==2)
							Cb=(Ca==2) ? 3 : 2;
						else
							Cb=Ca^1;
					}
				}
				if(RVSflag) {
					SetAPen(window->RPort,Cb);
					SetBPen(window->RPort,Ca);
				}
				else {
					SetAPen(window->RPort,Ca);
					SetBPen(window->RPort,Cb);
				}
				break;
			case 'r':
				SRtop=(params[0]>0 && params[0]<=MAXsy-1) ? params[0]-1 : 0;
				if(sp>1)
					SRbottom=(params[1]>0 && params[1]<=MAXsx-1) ? params[1]-1 : MAXsy;
				Cy=SRtop;
				Cx=0;
				break;
		}
	}
	else {
		switch(*str) {
			case 'D':	/*  index  */
				if(Cy==MAXsy)
					scroll(1);
				else
					Cy++;
				break;
			case 'M':	/*  reverse index  */
				if(Cy==0)
					scroll(-1);
				else
					Cy--;
				break;
			case 'c':	/*  reset  */
				MAXsx=79;
				MAXsy=22;
				SRtop=0;
				SRbottom=22;
				normal();
				cls();
				Cflag=TRUE;
				break;
		}
	}
	MOVE(Cy,Cx);
}

void DrawCursor(int sw)
{
	if(sw) {
		SetAPen(window->RPort,AttrBuf[Cy][Cx]>>4 & 0x07);
		SetBPen(window->RPort,AttrBuf[Cy][Cx] & 0x0f);
	}
	else {
		if(AttrBuf[Cy][Cx] & 0x80) {
			SetAPen(window->RPort,AttrBuf[Cy][Cx]>>4 & 0x07);
			SetBPen(window->RPort,AttrBuf[Cy][Cx] & 0x0f);
		}
		else {
			SetAPen(window->RPort,AttrBuf[Cy][Cx] & 0x0f);
			SetBPen(window->RPort,AttrBuf[Cy][Cx]>>4 & 0x07);
		}
	}
	Text(window->RPort,&ScreenBuf[Cy][Cx],1);
	if(RVSflag) {
		SetAPen(window->RPort,Cb);
		SetBPen(window->RPort,Ca);
	}
	else {
		SetAPen(window->RPort,Ca);
		SetBPen(window->RPort,Cb);
	}
	MOVE(Cy,Cx);
}

void backspace(void)
{
	outs("\10 \10");
}

void beep(void)
{
	outs("\7");
}

int cd(void)
{
	int dcd;

	dcd=0;
	if(SerialIO) {
		SerialIO->IOSer.io_Command=SDCMD_QUERY;
		DoIO((struct IORequest *)SerialIO);
		dcd=!(SerialIO->io_Status & 32);
		if(!dcd && !local)
			strcpy(reason,"carrier lost");
	}
	return(!(dcd || local==TRUE) || logoff);
}

void chat(void)
{
	static char word[81];
	int col,i,hold;
	char c,w;

	GetSystemDateTime();
	hold=Time;
	newline();
	newline();
	sprintf(OUTbuf,"%s<<%s Sysop now online %s>>%s",Fcol(GRN),Fcol(YELLOW),Fcol(GRN),Fcol(GRY));
	outs(OUTbuf);
	newline();
	newline();
	w=0;
	col=1;
	c='\0';
	while(c!='\33' && !cd()) {
		if(!(c=GetKey()))
			continue;
		if(c=='\30') {
			for(i=1; i<col; i++)
				backspace();
			col=1;
			continue;
		}
		if((c=='\10' || c=='\177') && col>1) {
			backspace();
			col--;
			continue;
		}
		if(c=='\15') {
			newline();
			newline();
			GetSystemDateTime();
			if(Time!=hold) {
				hold=Time;
				AdjustTime(+1);
			}
			w=0;
			col=1;
		}
		if(c<' ' || c>'\176')
			continue;
		if(c!=' ') {
			if(col==MAXsx) {
				for(i=0; i<strlen(word); i++)
					backspace();
				newline();
				outs(word);
				col=strlen(word);
			}
			word[w++]=c;
		}
		else {
			if(col==MAXsx) {
				newline();
				col=0;
				c='\0';
			}
			w=0;
		}
		word[w]='\0';
		sprintf(text,"%c",c);
		outs(text);
		col++;
	}
	newline();
	sprintf(OUTbuf,"%s>>%s Sysop now offline %s<<%s",Fcol(GRN),Fcol(YELLOW),Fcol(GRN),Fcol(GRY));
	outs(OUTbuf);
	newline();
	newline();
	AdjustTime(+1);
}

void cll(void)
{
	memset(&ScreenBuf[Cy][Cx],' ',MAXsx-Cx+1);
	memset(&AttrBuf[Cy][Cx],Cb<<4 | Ca,MAXsx-Cx+1);
	if(ANSI)
		outs("\33[K");
	else
		outs("       \10\10\10\10\10\10\10");
}

void cls(void)
{
	if(user) {
		sprintf(line[0],"%010.f",USER(user)->DataPhone);
		sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
		sprintf(line[2],"%d",carrier);
		sprintf(text,"%.26s | %.30s | %s | %s",USER(user)->Name,USER(user)->Street[1],line[1],(carrier>0) ? line[2] : "local");
		sprintf(title," %-78.78s ",text);
		if(window)
			SetWindowTitles(window,(UBYTE *)-1,title);
		sprintf(icontitle,"RPGBBS #%d %.26s",BBSprefs.Unit,USER(user)->Handle);
		if(iconwindow)
			SetWindowTitles(iconwindow,icontitle,(UBYTE *)-1);
	}
	Cx=0;
	Cy=0;
	MOVE(Cy,Cx);
	SetRast(window->RPort,Cb);
	memset(ScreenBuf,' ',sizeof(ScreenBuf));
	memset(AttrBuf,Cb<<4 | Ca,sizeof(AttrBuf));
	if(ANSI)
		WriteSerial("\33[H\33[J");
	else
		WriteSerial("\14");
}

char *Bcol(int code)
{
	static int sp=0;
	static char result[10][6];

	sp++;
	if(sp>9)
		sp=0;
	if(ANSI==2)
		sprintf(result[sp],"\33[%dm",40+(code & 7));
	else
		result[sp][0]='\0';
	return(result[sp]);
}

char *Fcol(int code)
{
	static int sp=0;
	static char result[10][8];
	char color[3];

	sp++;
	if(sp>9)
		sp=0;
	switch(ANSI) {
		case 0:
			result[sp][0]='\0';
			break;
		case 1:
			if(code & 8) {
				strcpy(result[sp],(BoldFlag ? "" : "\33[1m"));
				BoldFlag=TRUE;
			}
			else {
				strcpy(result[sp],(BoldFlag ? "\33[m" : ""));
				BoldFlag=FALSE;
			}
			break;
		case 2:
			sprintf(color,"%d",30+(code & 7));
			if(code & 8) {
				sprintf(result[sp],"\33[%s%sm",(BoldFlag ? "" : "1;"),color);
				BoldFlag=TRUE;
			}
			else {
				sprintf(result[sp],"\33[%s%sm",(BoldFlag ? "0;" : ""),color);
				BoldFlag=FALSE;
			}
			break;
	}
	return(result[sp]);
}

UBYTE GetKey(void)
{
	static int lastcolor;

	UBYTE sub,holduser;

	sub='\0';
	if(!local) {
		SerialIO->IOSer.io_Command=SDCMD_QUERY;
		DoIO((struct IORequest *)SerialIO);
		if(SerialIO->IOSer.io_Actual) {
			SerialIO->IOSer.io_Command=CMD_READ;
			SerialIO->IOSer.io_Length=1;
			SerialIO->IOSer.io_Data=(APTR)INbuf;
			DoIO((struct IORequest *)SerialIO);
			sub=INbuf[0];
			if(sub=='\33')
				sub='\0';
			if(lastcolor!=WHITE) {
				lastcolor=WHITE;
				outs(Fcol(lastcolor));
			}
			Cflash=0x0f;
			return(sub);
		}
	}
	if(window)
		if((message=(struct IntuiMessage *)GetMsg(window->UserPort))) {
			MessageClass=message->Class;
			code=message->Code;
			qualifier=message->Qualifier;
			ReplyMsg((struct Message *)message);
			if(MessageClass==IDCMP_RAWKEY) {
				InputEvent.ie_Class=IECLASS_RAWKEY;
				InputEvent.ie_Code=code;
				InputEvent.ie_Qualifier=qualifier;
				InputEvent.ie_position.ie_addr=*((APTR*)message->IAddress);
				if(RawKeyConvert(&InputEvent,INbuf,sizeof(INbuf),0))
					 if(!(code & IECODE_UP_PREFIX)) {
					 	if(INbuf[0]=='\233') {
					 		if(!grace && INbuf[1]!='?')
					 			return('\0');
							if(grace && INbuf[1]=='?') {
						 		grace=FALSE;
						 		return('\0');
						 	}
							if(!grace) {
						 		grace=TRUE;
						 		WriteConsole("\7");
						 		return('\0');
						 	}
					 		switch(INbuf[1]) {
								case '0':
							 		outs("The sysop intervenes and gives you ");
									if(USER(user)->CallsToday) {
										USER(user)->CallsToday--;
										outs("an extra call.");
									}
									else {
										RestoreStat();
										Thull=USER(user)->Hull;
										brawl=0;
										fight=0;
										joust=0;
										naval=0;
										party=0;
										outs("this call over again.");
									}
							 		newline();
									break;
					 			case '4':
									AdjustTime(+5);
									outs("The sysop intervenes and gives you five extra minutes.");
							 		newline();
					 				break;
					 			case '5':
					 				AdjustTime(-1);
					 				WriteConsole("You steal a minute.");
									break;
					 			case '8':
					 				if(NME.Sex=='d') {
						 				NME.Spell=65535;
						 				WriteConsole("You arm the creature with weapons of destruction.");
						 			}
					 				break;
								case '9':
									modf(USER(user)->Gold/10.,&d);
									USER(user)->Gold=d;
									WriteConsole("You steal 10% gold.");
									break;
					 		}
							normal();
					 		sub='\30';
					 	}
					 	else {
							Cflash=0x0f;
							sub=INbuf[0];
						}
					}
			}
			if(sub>0 && sub<128) {
				Cflash=0x0f;
				if(!local && sub!='\33' && lastcolor!=YELLOW) {
					lastcolor=YELLOW;
					outs(Fcol(lastcolor));
				}
				return(sub);
			}
			if(MessageClass & MENUPICK) {
				menu=MENUNUM(code);
				menuitem=ITEMNUM(code);
				subitem=SUBNUM(code);
				switch(menu) {
					case 0:	/* Project */
						switch(menuitem) {
							case 0:	/* Logoff */
								AdjustTime(-TimeLeft()-1);
								return('\0');
								break;
							case 1:	/* Sysop */
								sprintf(OUTbuf,"%sSysop logged on -- please wait.",Fcol(BLUE));
								outs(OUTbuf);
								sub=local;
								local=TRUE;
								newline();
								sprintf(text,"%s%.26s, type your password: ",Fcol(CYN),USER(1)->Handle);
								outs(text);
								echo=FALSE;
								if(input())
									logoff=TRUE;
								echo=TRUE;
								newline();
								if(strnicmp(USER(1)->Password,ins,sizeof(USER(1)->Password))==0) {
									holduser=user;
									user=1;
									Sysop();
									user=holduser;
								}
								local=sub;
								newline();
								sprintf(OUTbuf,"%sSysop logged off -- please resume play.%s",Fcol(BLUE),Fcol(WHITE));
								outs(OUTbuf);
								newline();
								newline();
								AdjustTime(+2);
								break;
							case 2:	/* View */
								switch(subitem) {
									case 0:	/* User */
										if(userwindow) {
											CloseWindow(userwindow);
											userwindow=NULL;
											ActivateWindow(window);
										}
										else {
											userwindow=(struct Window *)OpenWindow(&newuserwindow);
											DisplayUser();
										}
										break;
									case 1: /* Enemy */
										if(nmewindow) {
											CloseWindow(nmewindow);
											nmewindow=NULL;
											ActivateWindow(window);
										}
										else {
											nmewindow=(struct Window *)OpenWindow(&newnmewindow);
											DisplayNME();
										}
										break;
								}
								break;
							case 3:	/* Iconify */
								CloseGUI();
								newiconwindow.LeftEdge=BBSprefs.IconX;
								newiconwindow.TopEdge=BBSprefs.IconY;
								iconwindow=(struct Window *)OpenWindow(&newiconwindow);
								break;
						}
						break;
				}
			}
		}
	if(userwindow)
		if((message=(struct IntuiMessage *)GetMsg(userwindow->UserPort))) {
			MessageClass=message->Class;
			code=message->Code;
			ReplyMsg((struct Message *)message);
			if(MessageClass & CLOSEWINDOW) {
				CloseWindow(userwindow);
				userwindow=NULL;
				ActivateWindow(window);
			}
		}
	if(nmewindow)
		if((message=(struct IntuiMessage *)GetMsg(nmewindow->UserPort))) {
			MessageClass=message->Class;
			code=message->Code;
			ReplyMsg((struct Message *)message);
			if(MessageClass & CLOSEWINDOW) {
				CloseWindow(nmewindow);
				nmewindow=NULL;
				ActivateWindow(window);
			}
		}
	if(iconwindow)
		if((message=(struct IntuiMessage *)GetMsg(iconwindow->UserPort))) {
			MessageClass=message->Class;
			code=message->Code;
			ReplyMsg((struct Message *)message);
			if(MessageClass & ACTIVEWINDOW) {
				CloseWindow(iconwindow);
				iconwindow=NULL;
				OpenGUI();
			}
		}
	if(Cflag)
		if(!(++Cflash%0x08))
			DrawCursor(Cflash & 0x10);
	Delay(1);
	return('\0');
}

char inkey(void)
{
	static char c;
	int timeout;

	WriteConsole("\33[?25h");
	outs(Fcol(WHITE));
	timeout=4000;
	do {
		c=GetKey();
		timeout--;
		if(timeout%2000==0)
			beep();
		if(!timeout) {
			TimeOut();
			break;
		}
		if(c=='\33') {
			chat();
			c='?';
		}
		if(isalpha(c) || c=='?') {
			c=toupper(c);
			sprintf(text,"%c",c);
			outs(text);
			if(c=='O') {
				outs("FF");
				newline();
				newline();
				outs("Are you sure (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				newline();
				if(c=='Y') {
					strcpy(reason,"typed OFF");
					logoff=TRUE;
					c='\0';
				}
				else
					c='?';
			}
		}
		if(c=='\24') {
			newline();
			newline();
			GetSystemDateTime();
			sprintf(text,"%s%s  %s   Time left: %d min.",Fcol(YELLOW),Fdate(SystemDate),SystemTime,TimeLeft());
			outs(text);
			newline();
			c='?';
		}
	} while(!isalpha(c) && c!='\15' && c!='?' && c!='@' && !cd());
	normal();
	if(cd())
		c='\0';
	WriteConsole("\33[?25l");
	return(c);
}

int input(void)
{
	static char word[81];
	int b,i,timeout,w;
	UBYTE c;
	char *fuck;

	WriteConsole("\33[?25h");
	outs(Fcol(WHITE));
	c='\0';
	i=0;
	timeout=4000;
	memset(ins,0,sizeof(ins));
	if(ww)
		strcpy(ins,word);
	else
		memset(word,0,sizeof(word));
	outs(ins);
	while(c!='\15' && !cd()) {
		i=strlen(ins);
		w=strlen(word);
		c=GetKey();
		timeout--;
		if(timeout%2000==0)
			beep();
		if(!timeout) {
			TimeOut();
			break;
		}
		if(c=='\33') {
			chat();
			outs(ins);
		}
		if(c=='\30') {
			for(b=0; b<strlen(ins); b++)
				backspace();
			memset(ins,0,sizeof(ins));
			memset(word,0,sizeof(word));
			continue;
		}
		if((c=='\10' || c=='\177') && i>0) {
			ins[i-1]='\0';
			if(w)
				word[w-1]='\0';
			backspace();
			continue;
		}
		if(c=='\24') {
			GetSystemDateTime();
			newline();
			newline();
			sprintf(text,"%s  %s   Time left: %d min.",Fdate(SystemDate),SystemTime,TimeLeft());
			outs(text);
			newline();
			c='\15';
		}
		if(c=='\15')
			memset(word,0,sizeof(word));
		if(c<' ' || c>'\176' || strlen(ins)>78)
			continue;
		if(ww) {
			if(c!=' ') {
				word[w++]=c;
				word[w]='\0';
				if(strlen(ins)==75) {
					for(b=0; b<strlen(word); b++) {
						ins[i-b]='\0';
						backspace();
					}
					c='\15';
					continue;
				}
			}
			else {
				w=0;
				word[w]='\0';
				if(strlen(ins)==75) {
					c='\15';
					continue;
				}
			}
		}
		ins[i]=c;
		sprintf(text,"%c",c);
		outs(echo ? text : ".");
	}
	do {
		ins[i--]='\0';
	} while(ins[i]==' ');
	if(stricmp(ins,"off")==0) {
		newline();
		strcpy(reason,"typed OFF");
		logoff=TRUE;
	}
	strcpy(text,ins);
	strlwr(text);
	fuck=strchr(text,'f');
	while(fuck) {
		if(strncmp(fuck,"fuck",4)==0) {
			newline();
			newline();
			outs("Please refrain from using the F-word in future calls.  Bye!");
			newline();
			Delay(100);
			strcpy(reason,"used profanity");
			logoff=TRUE;
		}
		fuck=strchr(++fuck,'f');
	}
	normal();
	WriteConsole("\33[?25l");
	return(cd());
}

void newline(void)
{
	outs("\15\12");
}

void normal(void)
{
	if(ANSI)
		outs((BoldFlag || RVSflag) ? "\33[0;37m" : "\33[37m");
	BoldFlag=FALSE;
}

void outs(char *str)
{
	WriteConsole(str);
	WriteSerial(str);
}

void plot(int r,int c)
{
	char str[12];

	sprintf(str,"\33[%d;%dH",r,c);
	WriteConsole(str);
	if(ANSI)
		WriteSerial(str);
}

void reverse(void)
{
	if(ANSI)
		outs("\33[7m");
}

void sameline(void)
{
	outs("\15");
}

void scroll(int l)
{
	if(l>0) {
		memmove(&ScreenBuf[SRtop],&ScreenBuf[SRtop+l],80*(SRbottom-SRtop+l-1));
		memmove(&AttrBuf[SRtop],&AttrBuf[SRtop+l],80*(SRbottom-SRtop+l-1));
		memset(&ScreenBuf[SRbottom-l+1],' ',80*l);
		memset(&AttrBuf[SRbottom-l+1],Cb<<4 | 7,80*l);
	}
	if(l<0) {
		memmove(&ScreenBuf[SRtop+l],&ScreenBuf[SRtop],80*(SRbottom-SRtop-l+1));
		memmove(&AttrBuf[SRtop+l],&AttrBuf[SRtop],80*(SRbottom-SRtop-l+1));
		memset(&ScreenBuf[SRtop],' ',80*l);
		memset(&AttrBuf[SRtop],Cb<<4 | 7,80*l);
	}
	ScrollRaster(window->RPort,0,8*l,0,8*SRtop+15,640,8*SRbottom+22);
}

void tab(int n)
{
	char spaces[81];

	if(Cx<n) {
		sprintf(spaces,"%*s",n-Cx,"");
		outs(spaces);
	}
}

void WriteConsole(UBYTE *str)
{
	static char esc[64];
	int l;

	if(window) {
		if(Cflag)
			DrawCursor(FALSE);
		while(*str) {
			switch(*str) {
				case 0x07:	/*  bel  */
					if(local)
						DisplayBeep(NULL);
					break;
				case 0x08:	/*  bs  */
					if(Cx)
						Cx--;
					MOVE(Cy,Cx);
					break;
				case 0x09:	/*  tab  */
					Cx+=(8-Cx%8);
					if(Cx>MAXsx)
						Cx=MAXsx;
					MOVE(Cy,Cx);
					break;
				case 0x0A:	/*  lf  */
					Cx=0;
					if(Cy==SRbottom)
						scroll(1);
					else
					if(Cy<MAXsy)
						Cy++;
					MOVE(Cy,Cx);
					break;
				case 0x0C:	/*  ff  */
					cls();
					break;
				case 0x0D:	/*  cr  */
					Cx=0;
					MOVE(Cy,Cx);
					break;
				case 0x1B:	/*  esc  */
					esc[0]=*str;
					esc[1]=NULL;
					break;
				case 0x9B:	/*  csi  */
					esc[0]=*str;
					esc[1]=NULL;
					break;
				default:
					if(esc[0]) {
						l=strlen(esc);
						esc[l++]=*str;
						esc[l]=NULL;
						if(!strchr("0123456789;?[",*str) || l==sizeof(esc)) {
							DecodeANSI(esc);
							esc[0]=NULL;
						}
						break;
					}
					if(*str<' ')
						break;
					l=0;
					do {
						ScreenBuf[Cy][Cx]=str[l];
						AttrBuf[Cy][Cx++]=RVSflag | (Cb<<4) | Ca;
					} while(isprint(str[++l]) && Cx<=MAXsx);
					Text(window->RPort,str,l);
					str+=l-1;
					if(Cx>MAXsx)
						WriteConsole("\15\12");
					break;
			}
			str++;
		}
		if(Cflag)
			DrawCursor(TRUE);
	}
}

void WriteSerial(char *str)
{
	if(!local && SerialIO)
		if((SerialIO->IOSer.io_Length=strlen(str))) {
			SerialIO->IOSer.io_Command=CMD_WRITE;
			SerialIO->IOSer.io_Data=(APTR)str;
			DoIO((struct IORequest *)SerialIO);
		}
}
