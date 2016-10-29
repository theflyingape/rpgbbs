/*****************************\
 *   RPGBBS Deluxe Client    *
 *                           *
 *  Written by Robert Hurst  *
\*****************************/
#include "/include/all.h"
#include "global.h"

void __regargs _CXBRK(void)
{
	logoff=TRUE;
}

void assign(int max)
{
	int i,p;
	UBYTE *v,def;

	NORMAL;NL;
	while(ONLINE->class_origin==0 && ONLINE->class_type==0) {
		OUT("You have been rerolled.  You must pick a class.");NL;NL;
		reroll(ONLINE);
		for(i=0; i<MAXCLASS(0) && stricmp(ServerTable->class[ONLINE->class_origin]->Character[i]->Name,"Novice"); i++);
		p=i;
		if(PLAYER.Novice!='Y' || p==MAXCLASS(0)) {
			for(i=(p==MAXCLASS(0)) ? 1 : 2; i<MAXCLASS(0); i++) {
				ONLINE->class_type=i;
				sprintf(outbuf,"%s%s  %s",bracket(i-(p==MAXCLASS(0) ? 0 : 1)),fore(WHITE),CLASS(ONLINE)->Name);
				OUT(outbuf);NL;
			}
			sprintf(prompt,"%sEnter class (1-%u): ",fore(CYN),MAXCLASS(0)-(p==MAXCLASS(0) ? 1 : 2));
			NL;OUT(prompt);
			if(ins(2))
				strcpy(inbuf,"1");
			NL;NL;
			i=atoi(inbuf);
			if(p!=MAXCLASS(0) && i>0)
				i++;
		}
		else {
			OUT("Since you are a new user here, you are automatically assigned a character");NL;
			OUT("class.  At the Main Menu, press <Y> to see all your character information.");NL;NL;
			pause();
		}
		ONLINE->class_type=(i>0 && i<MAXCLASS(0)) ? i : FALSE;
		strmfe(PLAYER.Class,ServerTable->class[ONLINE->class_origin]->Origin,CLASS(ONLINE)->Name);
	}
	if(CLASS(ONLINE)->Magic<2) {
		PLAYER.SP=FALSE;
		PLAYER.Spell=FALSE;
	}
	if(CLASS(ONLINE)->Poison<1)
		PLAYER.Poison=FALSE;
	cls();
	strmfp(filename,"PIX/Player",CLASS(ONLINE)->Name);
	type_pix(filename,PLAYER.Gender);
	strmfp(filename,"Help",CLASS(ONLINE)->Name);
	type(filename,FALSE);
	if(PLAYER.Novice=='Y') {
		PLAYER.STR=CLASS(ONLINE)->BaseSTR;
		PLAYER.INT=CLASS(ONLINE)->BaseINT;
		PLAYER.DEX=CLASS(ONLINE)->BaseDEX;
		PLAYER.CHA=CLASS(ONLINE)->BaseCHA;
		pause();
	}
	else
		do {
			NL;
			sprintf(outbuf,"%sYou have %s%d%s ability points to distribute between 4 abilities: Strength,",fore(BRN),fore(YELLOW),max,fore(BRN));
			OUT(outbuf);NL;
			sprintf(outbuf,"Intellect, Dexterity, and Charisma.  Each ability must be between %s20%s and %s80%s.",fore(YELLOW),fore(BRN),fore(YELLOW),fore(BRN));
			OUT(outbuf);NL;NL;
			p=max;
			v=&PLAYER.STR;
			for(i=0; i<4; i++) {
				if(i>0 && i<3) {
					sprintf(outbuf,"You have %d ability points left.",p);
					OUT(outbuf);NL;NL;
				}
				switch(i) {
					case 0:
						def=CLASS(ONLINE)->BaseSTR;
						sprintf(outbuf,"%sEnter your Strength  %s: ",fore(CYN),bracket(def));
						break;
					case 1:
						def=CLASS(ONLINE)->BaseINT;
						sprintf(outbuf,"%sEnter your Intellect %s: ",fore(CYN),bracket(def));
						break;
					case 2:
						def=CLASS(ONLINE)->BaseDEX;
						if(def+20>p)
							def=p-20;
						sprintf(outbuf,"%sEnter your Dexterity %s: ",fore(CYN),bracket(def));
						break;
					case 3:
						def=p;
						sprintf(outbuf,"%sEnter your Charisma  %s: ",fore(CYN),bracket(def));
						break;
				}
				OUT(outbuf);
				FOREVER {
					if(ins(2))
						sprintf(inbuf,"%u",def);
					v[i]=atoi(inbuf);
					if(!strlen(inbuf)) {
						v[i]=def;
						sprintf(inbuf,"%s%u%s",fore(WHITE),def,fore(GRY));
						OUT(inbuf);
					}
					if(v[i]>=20 && v[i]<=80)
						break;
					erase();
				}
				NL;NL;
				p-=v[i];
			}
		} while(p);
	RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
	LoadRPC(ONLINE);
}

void beep(void)
{
	if(local)
		OUT("\7");
	else
		OUTSER("\7");
}

char *back(int code)
{
	static int sp=0;
	static char result[16][6];

	sp++;
	if(sp>15)
		sp=0;
	if(ANSI==2 || ANSI==3)
		sprintf(result[sp],"\33[%dm",40+(code & 7));
	else
		result[sp][0]=NULL;
	return(result[sp]);
}

char *bracket(int n)
{
	static char r[24];

	sprintf(r,"%s%s<%s%d%s>",(n<10) ? " " : "",fore(BLACK),fore(WHITE),n,fore(BLACK));
	return(r);
}

void broadcast(char *msg)
{
	sprintf(line[99],"%s[%s%s%s]%s %s %s",fore(CYN),fore(CYAN),PLAYER.ID,fore(CYN),fore(CYAN),PLAYER.Handle,msg);
	RPGserver(SERVER_BROADCAST,line[99]);
}

void bump(char *where)
{
	if(PLAYER.Gold>0. && ACCESS(acclvl)->RolePlay=='Y' && PLAYER.Novice!='Y') {
		RPGserver(SERVER_RNDUSER,(UBYTE *)&ENEMY);
		if(strcmp(PLAYER.ID,ENEMY.ID) && !strlen(ENEMY.Status)) {
			LoadRPC(RPC[1][0]);		//	if he is smarter than your luck...
			if(dice(ENEMY.Level)*CLASS(RPC[1][0])->Steal/2>PLAYER.Level && dice(RPC[1][0]->INT)>dice(ONLINE->CHA)) {
				sprintf(outbuf,"You bump into %s on the %s.",ENEMY.Handle,where);
				OUT(outbuf);NL;
				Delay(50);			//	if he is quicker than your attention...
				if(dice(RPC[1][0]->DEX>dice(PLAYER.INT))) {
					ENEMY.Gold+=PLAYER.Gold;
					RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
					numline=0;
					sprintf(line[numline++],"You picked %s's pocket and got %s!",PLAYER.Handle,money(PLAYER.Gold,ENEMY.Emulation));
					note(ENEMY.ID);
					PLAYER.Gold=0.;
				}
				sprintf(outbuf,"You exchange greetings with %s and go on your way.",RPC[1][0]->him);
				OUT(outbuf);NL;
				Delay(50);
				paws=!PLAYER.Expert;
			}
		}
	}
}

int cd(void)
{
	int dcd;

	dcd=0;
	if(SERCON.SerialIO) {
		SERCON.SerialIO->IOSer.io_Command=SDCMD_QUERY;
		DoIO((struct IORequest *)SERCON.SerialIO);
		dcd=!(SERCON.SerialIO->io_Status & 32);
		if(!dcd)
			strcpy(reason,"carrier lost");
	}
	return(!(dcd || local==TRUE) || logoff);
}

void chatmode(void)
{
	int i,lastsr=2,sub;
	int row[2]={2,12},col[2]={1,1};
	char c[2]={ '\0','\0' };

	chat=FALSE;
	cls();
	title("chatmode");
	NORMAL;
	if(ANSI==1) {
		REVERSE;
		sprintf(outbuf,"    %s (Sysop)    ",SYS->Handle);
	}
	else
		sprintf(outbuf,"%s°±²%s%s %s (Sysop) %s%s²±°",fore(BLU),back(BLU),fore(WHITE),SYS->Handle,back(BLK),fore(BLU));
	OUT(outbuf);NORMAL;NL;
	plot(&SERCON,11,1);
	if(ANSI==1) {
		REVERSE;
		sprintf(outbuf,"    %s    ",PLAYER.Handle);
	}
	else
		sprintf(outbuf,"%s°±²%s%s %s %s%s²±°",fore(BRN),back(BRN),fore(YELLOW),PLAYER.Handle,back(BLK),fore(BRN));
	OUT(outbuf);NORMAL;NL;
	OUT("\33[2;9r\33[1m");
	plot(&SERCON,2,1);
	memset(inbuf,0,INBUF_SIZE);
	memset(line[0],0,128);
	memset(line[1],0,128);
	while((c[0]=getkey())) {
		if(source!=lastsr) {
			lastsr=source;
			sub=source-1;
			row[sub]=SERCON.Row+1;
			col[sub]=SERCON.Col+1;
			strcpy(line[sub],inbuf);
			sub^=1;
			if(source==1)
				sprintf(outbuf,"\33[12;%ur%s",(rows>SERCON.MaxRow ? SERCON.MaxRow : rows),fore(YELLOW));
			else
				sprintf(outbuf,"\33[2;9r%s",fore(WHITE));
			OUT(outbuf);
			plot(&SERCON,row[sub],col[sub]);
			strcpy(inbuf,line[sub]);
		}
		if(c[0]==8)
			c[0]=127;
		switch(c[0]) {
			case 13:
				memset(inbuf,0,INBUF_SIZE);
	 			NL;
				break;
			case 24:
				for(i=strlen(inbuf); i>0; i--)
					RUBOUT;
				memset(inbuf,0,INBUF_SIZE);
				break;
			case 127:
				if((i=strlen(inbuf))) {
					inbuf[i-1]=NULL;
					RUBOUT;
				}
				break;
			default:
				if(c[0]>=' ') {
					strcat(inbuf,c);
					OUT(c);
				}
				if((i=strlen(inbuf))>70) {
					for(; i>=0 && inbuf[i]!=' '; i--)
						RUBOUT;
					strcpy(inbuf,&inbuf[i+1]);
					NL;OUT(inbuf);
				}
				break;
		}
	}
	sprintf(outbuf,"\33[1;%ur",rows);
	OUT(outbuf);
	cls();
	NORMAL;
	OUT(prompt);
}

void chkexp(void)
{
	int i;
	int ahp=0,asp=0,astr=PLAYER.STR,aint=PLAYER.INT,adex=PLAYER.DEX,acha=PLAYER.CHA;

	while(PLAYER.Experience>=EXP(PLAYER.Level) && ACCESS(acclvl)->RolePlay=='Y') {
		PLAYER.Level++;
		if(ACCESS(PLAYER.Access)->Promote==PLAYER.Level) {
			PLAYER.Access++;
			NORMAL;
			sprintf(outbuf,"%sThe king is pleased with your accomplishments and makes you %s%s!",fore(YELLOW),AN(ACCESS(PLAYER.Access)->Name),ACCESS(PLAYER.Access)->Name);
			NL;Delay(5);
			OUT(outbuf);
			NL;Delay(5);
		}
		ahp+=PLAYER.Level+dice(PLAYER.Level)+PLAYER.STR/10+(PLAYER.STR>90 ? PLAYER.STR-90 : 0);
		if(PLAYER.SP)
			asp+=PLAYER.Level+dice(PLAYER.Level)+PLAYER.INT/10+(PLAYER.INT>90 ? PLAYER.INT-90 : 0);
		i=CLASS(ONLINE)->BonusSTR;
		if(PLAYER.STR+i>CLASS(ONLINE)->MaxSTR)
			i=CLASS(ONLINE)->MaxSTR-PLAYER.STR;
		if(i<0)
			i=0;
		if(i) {
			PLAYER.STR+=i;
			if(ONLINE->STR+i>CLASS(ONLINE)->MaxSTR) {
				i=CLASS(ONLINE)->MaxSTR;
				if(strlen(PLAYER.Blessed)) {
					i+=10;
					if(i>100)
						i=100;
				}
				i-=ONLINE->STR;
				if(i<0)
					i=0;
			}
			ONLINE->STR+=i;
		}
		i=CLASS(ONLINE)->BonusINT;
		if(PLAYER.INT+i>CLASS(ONLINE)->MaxINT)
			i=CLASS(ONLINE)->MaxINT-PLAYER.INT;
		if(i<0)
			i=0;
		if(i) {
			PLAYER.INT+=i;
			if(ONLINE->INT+i>CLASS(ONLINE)->MaxINT) {
				i=CLASS(ONLINE)->MaxINT;
				if(strlen(PLAYER.Blessed)) {
					i+=10;
					if(i>100)
						i=100;
				}
				i-=ONLINE->INT;
				if(i<0)
					i=0;
			}
			ONLINE->INT+=i;
		}
		i=CLASS(ONLINE)->BonusDEX;
		if(PLAYER.DEX+i>CLASS(ONLINE)->MaxDEX)
			i=CLASS(ONLINE)->MaxDEX-PLAYER.DEX;
		if(i<0)
			i=0;
		if(i) {
			PLAYER.DEX+=i;
			if(ONLINE->DEX+i>CLASS(ONLINE)->MaxDEX) {
				i=CLASS(ONLINE)->MaxDEX;
				if(strlen(PLAYER.Blessed)) {
					i+=10;
					if(i>100)
						i=100;
				}
				i-=ONLINE->DEX;
				if(i<0)
					i=0;
			}
			ONLINE->DEX+=i;
		}
		i=CLASS(ONLINE)->BonusCHA;
		if(PLAYER.CHA+i>CLASS(ONLINE)->MaxCHA)
			i=CLASS(ONLINE)->MaxCHA-PLAYER.CHA;
		if(i<0)
			i=0;
		if(i) {
			PLAYER.CHA+=i;
			if(ONLINE->CHA+i>CLASS(ONLINE)->MaxCHA) {
				i=CLASS(ONLINE)->MaxCHA;
				if(strlen(PLAYER.Blessed)) {
					i+=10;
					if(i>100)
						i=100;
				}
				i-=ONLINE->CHA;
				if(i<0)
					i=0;
			}
			ONLINE->CHA+=i;
		}
	}
	if(ahp) {
		sound("level",64);
		PLAYER.ExpLevel=PLAYER.Level;
		NL;Delay(5);
		sprintf(outbuf,"      %s-=%s>%s*%s<%s=-",fore(MAG),fore(BLU),fore(YELLOW),fore(BLU),fore(MAG));
		OUT(outbuf);
		NL;Delay(5);
		NL;Delay(5);
		sprintf(outbuf,"%sWelcome to level %hu!",fore(YELLOW),PLAYER.Level);
		OUT(outbuf);
		NL;Delay(5);
		NL;Delay(5);
		if(PLAYER.Level<=SYS->Level) {
			if(ahp) {
				PLAYER.HP+=ahp;
				ONLINE->HP+=ahp;
				sprintf(outbuf,"%s%+6d%s Hit points",fore(WHITE),ahp,fore(GRY));
				OUT(outbuf);
				NL;Delay(5);
			}
			if(asp) {
				PLAYER.SP+=asp;
				ONLINE->SP+=asp;
				sprintf(outbuf,"%s%+6d%s Spell power",fore(WHITE),asp,fore(GRY));
				OUT(outbuf);
				NL;Delay(5);
			}
			if((i=PLAYER.STR-astr)) {
				sprintf(outbuf,"%s%+6d%s Strength",fore(WHITE),i,fore(GRY));
				OUT(outbuf);
				NL;Delay(5);
			}
			if((i=PLAYER.INT-aint)) {
				sprintf(outbuf,"%s%+6d%s Intellect",fore(WHITE),i,fore(GRY));
				OUT(outbuf);
				NL;Delay(5);
			}
			if((i=PLAYER.DEX-adex)) {
				sprintf(outbuf,"%s%+6d%s Dexterity",fore(WHITE),i,fore(GRY));
				OUT(outbuf);
				NL;Delay(5);
			}
			if((i=PLAYER.CHA-acha)) {
				sprintf(outbuf,"%s%+6d%s Charisma",fore(WHITE),i,fore(GRY));
				OUT(outbuf);
				NL;Delay(5);
			}
			NL;
			RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
			pause();
		}
		else
			immortalize();
		displayview();
	}
}

void cls(void)
{
	if(paws)
		pause();
	OUT("\33[H\33[J");
}

int dice(int n)
{
	int r;

	if(n>0)
		r=rand()/10%n+1;
	else
		r=1;
	return(r);
}

void displaykeys(int n)
{
	int i;
	char c,keys[4]={'P','G','S','C'};

	for(i=0; i<3; i++)
		if((c=PLAYER.KeyHints[n][i]))
			switch(c) {
				case 'P':
					keys[0]=NULL;
					break;
				case 'G':
					keys[1]=NULL;
					break;
				case 'S':
					keys[2]=NULL;
					break;
				case 'C':
					keys[3]=NULL;
					break;
			}
	for(i=0; i<4; i++) {
		if((c=keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf,"%s{%sP%s}  ",fore(MAG),fore(MAGENTA),fore(MAG));
					break;
				case 'G':
					sprintf(outbuf,"%s{%sG%s}  ",fore(BRN),fore(YELLOW),fore(BRN));
					break;
				case 'S':
					sprintf(outbuf,"%s{%sS%s}  ",fore(CYN),fore(CYAN),fore(CYN));
					break;
				case 'C':
					sprintf(outbuf,"%s{%sC%s}  ",fore(RED),fore(BRED),fore(RED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
	for(i=0; i<4; i++) {
		if((c=keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf," %s|   ",fore(MAG));
					break;
				case 'G':
					sprintf(outbuf," %s|   ",fore(BRN));
					break;
				case 'S':
					sprintf(outbuf," %s|   ",fore(CYN));
					break;
				case 'C':
					sprintf(outbuf," %s|   ",fore(RED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
	for(i=0; i<4; i++) {
		if((c=keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf," %s|%s-  ",fore(MAG),fore(MAGENTA));
					break;
				case 'G':
					sprintf(outbuf," %s|%s-  ",fore(BRN),fore(YELLOW));
					break;
				case 'S':
					sprintf(outbuf," %s|%s-  ",fore(CYN),fore(CYAN));
					break;
				case 'C':
					sprintf(outbuf," %s|%s-  ",fore(RED),fore(BRED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
	for(i=0; i<4; i++) {
		if((c=keys[i])) {
			switch(c) {
				case 'P':
					sprintf(outbuf," %s|%s=  ",fore(MAG),fore(MAGENTA));
					break;
				case 'G':
					sprintf(outbuf," %s|%s=  ",fore(BRN),fore(YELLOW));
					break;
				case 'S':
					sprintf(outbuf," %s|%s=  ",fore(CYN),fore(CYAN));
					break;
				case 'C':
					sprintf(outbuf," %s|%s=  ",fore(RED),fore(BRED));
					break;
			}
			OUT(outbuf);
		}
	}
	NL;
}

void displayview(void)
{
	int i,j,AC,WC;

	if(viewwindow) {
		AC=0;
		for(i=0; i<2; i++)
			for(j=1; j<5; j++)
				if(strlen(RPC[i][j]->user.Handle) && j>AC)
					AC=j;
		ChangeWindowBox(viewwindow,viewwindow->LeftEdge,viewwindow->TopEdge,viewwindow->Width,4*SERCON.TextFont->tf_YSize+AC*(3*SERCON.TextFont->tf_YSize+1));
		Delay(5);
		SetRast(viewwindow->RPort,SERCON.Window->WScreen->BitMap.Depth<4 ? 3 : WHITE);
		SetAPen(viewwindow->RPort,SERCON.Window->WScreen->BitMap.Depth<4 ? 2 : BLU);
		SetDrMd(viewwindow->RPort,JAM1);
		for(j=0; j<5; j++) {
			memset(line[1],0,128);
			memset(line[2],0,128);
			memset(line[3],0,128);
			for(i=0; i<2; i++) {
				if(strlen(RPC[i][j]->user.Handle)) {
					sprintf(outbuf,"%s - %u %s",RPC[i][j]->user.Handle,RPC[i][j]->user.Level,CLASS(RPC[i][j])->Name);
					sprintf(line[0],"%-38.38s",outbuf);
					strcat(line[1],line[0]);
			 		WC=(RPC[i][j]->weapon_origin<NUMWEAPON) ? WEAPON(RPC[i][j])->Class : RPC[i][j]->weapon_type;
			 		AC=(RPC[i][j]->armor_origin<NUMARMOR) ? ARMOR(RPC[i][j])->Class : RPC[i][j]->armor_type;
					sprintf(outbuf,"  WC:%u%+d  AC:%u%+d  HP:%d  SP:%d",WC,RPC[i][j]->user.WCmod+RPC[i][j]->ToWC,AC,RPC[i][j]->user.ACmod+RPC[i][j]->ToAC,RPC[i][j]->HP,RPC[i][j]->SP);
					sprintf(line[0],"%-38.38s",outbuf);
					strcat(line[2],line[0]);
					sprintf(outbuf,"  STR:%u  INT:%u  DEX:%u",RPC[i][j]->STR,RPC[i][j]->INT,RPC[i][j]->DEX);
					sprintf(line[0],"%-38.38s",outbuf);
					strcat(line[3],line[0]);
				}
				else {
					sprintf(line[0],"%-38s","");
					strcat(line[1],line[0]);
					strcat(line[2],line[0]);
					strcat(line[3],line[0]);
				}
			}
			for(i=1; i<=3; i++) {
				Move(viewwindow->RPort,0,j*(3*SERCON.TextFont->tf_YSize+1)+SERCON.TextFont->tf_YSize*i);
				Text(viewwindow->RPort,line[i],strlen(line[i]));
			}
		}
	}
}

void erase(void)
{
	int i;

	beep();
	for(i=0; i<strlen(inbuf); i++)
		RUBOUT;
}

void finputs(char *str,int n,FILE *file)
{
	if(!fgets(str,n,file))
		memset(str,0,n);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
}

char *fore(int code)
{
	static int sp=0;
	static char result[16][10];

	sp++;
	if(sp>15)
		sp=0;
	if(ANSI==2 || ANSI==3) {
		if(code & 8) {
			sprintf(result[sp],"\33[%s%dm",(bold ? "" : "1;"),30+(code & 7));
			bold=TRUE;
		}
		else {
			sprintf(result[sp],"\33[%s%dm",(bold ? "22;" : ""),30+(code & 7));
			bold=FALSE;
		}
	}
	else
	if(ANSI==1) {
		if(code & 8) {
			strcpy(result[sp],(code==8) ? "\33[2m" : (bold ? "" : "\33[1m"));
			bold=(code>8);
		}
		else {
			strcpy(result[sp],(bold ? "\33[22m" : ""));
			bold=FALSE;
		}
	}
	else
		result[sp][0]=NULL;
	return(result[sp]);
}

char getkey(void)
{
	static char cm=FALSE;
	int i;
	char c=NULL;

	chkabort();
	if(!Serbuf[0] && !logoff) {
		for(i=1; i<=4; i<<=1) {
			INPUT(local ? 0 : timeout);
			if(source || cd())
				break;
			beep();
			timeout>>=1;
		}
		if(i>4) {
			sprintf(outbuf,"%s*** Inactive Timeout ***%s",fore(YELLOW),fore(GRY));
			NL;NL;OUT(outbuf);NL;NL;
			logoff=TRUE;
			strcpy(reason,"went to sleep");
		}
	}
	else
		source=1;
	if(source==1 || source==2) {
		if(source==2 && !strcmp(Serbuf,"\33[28~")) {
			modem_clr();
			if(cm)
				return(NULL);
			cm=TRUE;
			chatmode();
			cm=FALSE;
			return('?');
		}
		c=Serbuf[0];
		memmove(Serbuf,&Serbuf[1],strlen(Serbuf));
	}
	if(source==3) {
		menu=Serbuf[0];
		menuitem=Serbuf[1];
		subitem=Serbuf[2];
		switch(menu) {
			case 0:
				switch(menuitem) {
					case 0:
						if(viewwindow) {
							CloseWindow(viewwindow);
							viewwindow=NULL;
						}
						CloseConsole(&SERCON);
						SERCON.Window=NULL;
						ClearMenuStrip(window);
						CloseWindow(window);
						window=NULL;
						CloseFont(SERCON.TextFont);
						CloseLibrary(DiskfontBase);
						OpenConsole(&SERCON);
						if((appobj=GetDiskObject(IconName))) {
							appobj->do_Type=NULL;
							appobj->do_CurrentX=NO_ICON_POSITION;
							appobj->do_CurrentY=NO_ICON_POSITION;
							SERCON.IconMP=(struct MsgPort *)CreatePort(NULL,0);
							appicon=AddAppIconA(NULL,NULL,IconName,SERCON.IconMP,NULL,appobj,NULL);
						}
						else
							source=5;
						break;
					case 1:
						logoff=TRUE;
						strcpy(reason,"logoff");
						break;
					case 2:
						if(viewwindow) {
							CloseWindow(viewwindow);
							viewwindow=NULL;
						}
						else
						if((viewwindow=(struct Window *)OpenWindowTags(NULL,
							WA_PubScreen, pubscreen,
							WA_Top, window->TopEdge+SERCON.TextFont->tf_YSize, WA_Left, pubscreen->Width-78*SERCON.TextFont->tf_XSize,
							WA_Width, 77*SERCON.TextFont->tf_XSize, WA_Height, 4*SERCON.TextFont->tf_YSize,
							WA_MaxWidth, ~0, WA_MaxHeight, ~0,
							WA_Activate, FALSE,
							WA_Borderless, FALSE,
							WA_CloseGadget, FALSE,
							WA_DepthGadget, FALSE,
							WA_DragBar, FALSE,
							WA_SizeGadget, FALSE,
							WA_SizeBBottom, FALSE,
							WA_GimmeZeroZero, TRUE,
							WA_IDCMP, NULL,
							WA_NoCareRefresh, TRUE,
							WA_SmartRefresh, TRUE,
							TAG_END))) {
							SetFont(viewwindow->RPort,SERCON.TextFont);
							displayview();
						}
						break;
					case 3:
						logoff=TRUE;
						strcpy(reason,"quit");
						break;
				}
				break;
		}
	}
	if(source==4) {
		logoff=TRUE;
		strcpy(reason,"closewindow");
	}
	if(source==5) {
		if(appicon) {
			RemoveAppIcon(appicon);
			DeletePort(SERCON.IconMP);
			SERCON.IconMP=NULL;
			FreeDiskObject(appobj);
			appobj=NULL;
		}
		CloseConsole(&SERCON);
		OpenGUI();
	}
	return(c);
}

int hallcheck(int o,int t)
{
	int h=FALSE,l=FALSE;

	if(PLAYER.Current.Calls<=ServerTable->class[o]->Character[t]->Calls[0].What) {
		ServerTable->class[o]->Character[t]->Calls[0].What=PLAYER.Current.Calls;
		ServerTable->class[o]->Character[t]->Calls[0].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Calls[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality in the least calls %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Calls,fore(CYN));
		OUT(outbuf);NL;
		h=1;
	}
	if(PLAYER.Current.Kills<=ServerTable->class[o]->Character[t]->Kills[0].What) {
		ServerTable->class[o]->Character[t]->Kills[0].What=PLAYER.Current.Kills;
		ServerTable->class[o]->Character[t]->Kills[0].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Kills[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality with the least kills %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Kills,fore(CYN));
		OUT(outbuf);NL;
		h=1;
	}
	if(PLAYER.Current.Killed<=ServerTable->class[o]->Character[t]->Killed[0].What) {
		ServerTable->class[o]->Character[t]->Killed[0].What=PLAYER.Current.Killed;
		ServerTable->class[o]->Character[t]->Killed[0].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Killed[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality getting killed the least %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Killed,fore(CYN));
		OUT(outbuf);NL;
		h=1;
	}
	if(PLAYER.Current.Retreats<=ServerTable->class[o]->Character[t]->Retreats[0].What) {
		ServerTable->class[o]->Character[t]->Retreats[0].What=PLAYER.Current.Retreats;
		ServerTable->class[o]->Character[t]->Retreats[0].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Retreats[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality with the least retreats %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Retreats,fore(CYN));
		OUT(outbuf);NL;
		h=1;
	}
	if(PLAYER.Current.KOs>=ServerTable->class[o]->Character[t]->Brawls[0].What) {
		ServerTable->class[o]->Character[t]->Brawls[0].What=PLAYER.Current.KOs;
		ServerTable->class[o]->Character[t]->Brawls[0].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Brawls[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality with the most brawls won %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.KOs,fore(CYN));
		OUT(outbuf);NL;
		h=1;
	}
	if(PLAYER.Current.JoustWin>=ServerTable->class[o]->Character[t]->Jousts[0].What) {
		ServerTable->class[o]->Character[t]->Jousts[0].What=PLAYER.Current.JoustWin;
		ServerTable->class[o]->Character[t]->Jousts[0].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Jousts[0].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality with the most jousts won %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.JoustWin,fore(CYN));
		OUT(outbuf);NL;
		h=1;
	}
	if(h)
		RPGserver(SERVER_PUTHOF,NULL);

	if(PLAYER.Current.Calls>=ServerTable->class[o]->Character[t]->Calls[1].What) {
		ServerTable->class[o]->Character[t]->Calls[1].What=PLAYER.Current.Calls;
		ServerTable->class[o]->Character[t]->Calls[1].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Calls[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality taking the most calls %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Calls,fore(CYN));
		OUT(outbuf);NL;
		l=1;
	}
	if(PLAYER.Current.Kills>=ServerTable->class[o]->Character[t]->Kills[1].What) {
		ServerTable->class[o]->Character[t]->Kills[1].What=PLAYER.Current.Kills;
		ServerTable->class[o]->Character[t]->Kills[1].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Kills[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality by killing the most %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Kills,fore(CYN));
		OUT(outbuf);NL;
		l=1;
	}
	if(PLAYER.Current.Killed>=ServerTable->class[o]->Character[t]->Killed[1].What) {
		ServerTable->class[o]->Character[t]->Killed[1].What=PLAYER.Current.Killed;
		ServerTable->class[o]->Character[t]->Killed[1].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Killed[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality getting killed the most %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Killed,fore(CYN));
		OUT(outbuf);NL;
		l=1;
	}
	if(PLAYER.Current.Retreats>=ServerTable->class[o]->Character[t]->Retreats[1].What) {
		ServerTable->class[o]->Character[t]->Retreats[1].What=PLAYER.Current.Retreats;
		ServerTable->class[o]->Character[t]->Retreats[1].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Retreats[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality retreating the most %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.Retreats,fore(CYN));
		OUT(outbuf);NL;
		l=1;
	}
	if(PLAYER.Current.KOed>=ServerTable->class[o]->Character[t]->Brawls[1].What) {
		ServerTable->class[o]->Character[t]->Brawls[1].What=PLAYER.Current.KOed;
		ServerTable->class[o]->Character[t]->Brawls[1].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Brawls[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality getting knocked-out the most %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.KOed,fore(CYN));
		OUT(outbuf);NL;
		l=1;
	}
	if(PLAYER.Current.JoustLoss>=ServerTable->class[o]->Character[t]->Jousts[1].What) {
		ServerTable->class[o]->Character[t]->Jousts[1].What=PLAYER.Current.JoustLoss;
		ServerTable->class[o]->Character[t]->Jousts[1].When=Julian;
		strcpy(ServerTable->class[o]->Character[t]->Jousts[1].Who,PLAYER.Handle);
		sprintf(outbuf,"%sYou have reached immortality with the most joust losses %s(%s%u%s)",fore(GRY),fore(CYN),fore(WHITE),PLAYER.Current.JoustLoss,fore(CYN));
		OUT(outbuf);NL;
		l=1;
	}
	if(l)
		RPGserver(SERVER_PUTHOL,NULL);

	if(h+l)
		pause();

	return(h+l);
}

void immortalize(void)
{
	FILE *fp;
	int h,i,immortal,j,level,max;
	char c;

	if(PLAYER.Novice=='Y') {
		PLAYER.Novice='N';
		sprintf(outbuf,"%sYou are no longer a novice player.  Welcome to the next level of play.",fore(YELLOW));
		OUT(outbuf);NL;NL;
		pause();
	}
	h=1;
	sprintf(outbuf,"%sChecking your statistics against All-Time...",fore(CYN));
	OUT(outbuf);NL;
	if(hallcheck(0,0))
		h++;
	sprintf(outbuf,"%sChecking your statistics against %s...",fore(CYN),CLASS(ONLINE)->Name);
	OUT(outbuf);NL;
	if(hallcheck(ONLINE->class_origin,ONLINE->class_type))
		h++;

	PLAYER.Immortal++;
	OUT("You have become so powerful that you are now immortal.");NL;
	memset(&ENEMY,0,sizeof(struct user));
	while(RPGserver(SERVER_NEXTUSER,(UBYTE *)&ENEMY)) {
		if(ENEMY.Bounty>0. && !strcmp(PLAYER.ID,ENEMY.BountyWho)) {
			ENEMY.Bounty=0.;
			memset(ENEMY.BountyWho,0,sizeof(ENEMY.BountyWho));
			RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
		}
	}
	sprintf(prompt,"%sPress RETURN to start over in this world: ",fore(CYN));
	OUT(prompt);
	modem_clr();
	ins(1);NL;NL;

	for(immortal=ServerTable->NumClass-1; immortal>0 && stricmp("IMMORTAL",ServerTable->class[immortal]->Origin); immortal--);
	max=(stricmp(ServerTable->class[ONLINE->class_origin]->Origin,"IMMORTAL")) ? FALSE : ONLINE->class_type+1;
	h+=max;
	if(max==ServerTable->class[ONLINE->class_origin]->NumCharacter) {
		sprintf(outbuf,"%sCONGRATULATIONS!!%s  You have won the game!",fore(YELLOW),fore(GRY));
		OUT(outbuf);NL;
		Delay(100);
		PLAYER.Wins++;
		RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
		if((fp=fopen("Notices/Winners","a"))) {
			fdate(dated,Julian);
			fprintf(fp,"%s  %-22.22s  (%lu days)\n",dated,PLAYER.Handle,PLAYER.LastDate-(PLAYER.CreateDate>SYS->BirthDate ? PLAYER.CreateDate : SYS->BirthDate)+1);
			fclose(fp);
			OUT("Your character has now joined the Winner's List");NL;
			Delay(100);
		}
		sprintf(outbuf,"%sThe board will now be reset...%s",fore(BRN),fore(YELLOW));
		OUT(outbuf);
		memset(&RPC[1][1]->user,0,sizeof(struct user));
		for(i=0; RPGserver(SERVER_NEXTUSER,(UBYTE *)&RPC[1][1]->user); i++) {
			ReKeySequence(RPC[1][1]);
			reroll(RPC[1][1]);
			sprintf(outbuf,"%c\10",spin[i%4]);
			OUT(outbuf);
		}
		OUT("Happy Hunting!");NL;
		Delay(100);
		SYS->BirthDate=Julian+1;
		SYS->Gold=0.;
		RPGserver(SERVER_PUTUSER,(UBYTE *)SYS);
		reroll(ONLINE);
		strcpy(reason,"won the game!");
		for(i=0; i<3; i++)
			if(stricmp(CALLERS->Last[i],PLAYER.Handle)==0) {
				strcpy(CALLERS->Reason[i],reason);
				break;
			}
		logoff=TRUE;
		return;
	}

	OUT("Ol' Mighty One!  Solve the Ancient Riddle of the Keys and you will become");NL;
	OUT("an immortal being.");NL;
	for(i=0; i<4; i++)
		for(j=0; j<3; j++)
			if((c=PLAYER.KeyHints[i][j])) {
				if(strchr("CSGP",c)) {
					sprintf(outbuf,"Key #%d is NOT %s.",i+1,c=='C' ? "Copper" : c=='S' ? "Silver" : c=='G' ? "Gold" : "Platinum");
					OUT(outbuf);NL;
				}
				else
					PLAYER.KeyHints[i][j]=NULL;
			}
	for(i=0; i<h; i++)
		keyhint();

	NL;
	level=0;
	reroll(ONLINE);
	while(!cd() && level<=max) {
		displaykeys(level);
		sprintf(prompt,"%sInsert key #%d? ",fore(CYN),level+1);
		do {
			NL;OUT(prompt);
			modem_clr();
			c=inkey('?',NULL);
		} while(!strchr("CSGP",c));
		OUT("...you insert and twist the key...");
		Delay(100);
		if(PLAYER.KeySequence[level]==c) {
			sprintf(outbuf,"%s{%sClick!%s}",fore(CYN),fore(CYAN),fore(CYN));
			OUT(outbuf);NL;
			strmfe(PLAYER.Class,ServerTable->class[immortal]->Origin,ServerTable->class[immortal]->Character[level]->Name);
			sprintf(outbuf,"You are now %s%s.",AN(ServerTable->class[immortal]->Character[level]->Name),ServerTable->class[immortal]->Character[level]->Name);
			OUT(outbuf);NL;
			LoadRPC(ONLINE);
			level++;
			paws=!PLAYER.Expert;
			for(i=level; i<4; i++)
				for(j=0; j<3; j++)
					if(!PLAYER.KeyHints[i][j] || !strchr("CSGP",PLAYER.KeyHints[i][j])) {
						PLAYER.KeyHints[i][j]=c;
						break;
					}
		}
		else {
			sprintf(outbuf,"%s^%sBoom!%s^",fore(BLACK),fore(WHITE),fore(BLACK));
			OUT(outbuf);NL;
			for(i=0; i<3; i++)
				if(!PLAYER.KeyHints[level][i] || !strchr("CSGP",PLAYER.KeyHints[level][i])) {
					PLAYER.KeyHints[level][i]=c;
					break;
				}
			break;
		}
	}
	if(level)
		ReKeySequence(ONLINE);
	if((max=200+dice(PLAYER.Immortal))>240)
		max=240;
	assign(max);
	PLAYER.CallsToday=0;
	sprintf(outbuf,"%sYou are rewarded with %u more calls for today.",fore(YELLOW),ACCESS(PLAYER.Access)->Calls);
	OUT(outbuf);NL;
	NORMAL;NL;
	strcpy(reason,"became immortal");
	logoff=TRUE;
}

char inkey(char def,char loss)
{
	char str[18];
	char c,esc=FALSE;

	timeout=60;
	do {
		c=getkey();
		c=toupper(c);
		if(c=='\15' && def)
			c=def;
		else
		if(cd()) {
			c=loss;
			strcpy(reason,"lost carrier");
			logoff=TRUE;
			break;
		}
		else
		if(c=='\33' || c=='[' || c=='\233') {
			c=NULL;
			esc=TRUE;
		}
		else
		if(c<' ' || c>'Z')
			c=NULL;
		else
		if(esc)
			c=(c=='A') ? 'N' : (c=='B') ? 'S' : (c=='C') ? 'E' : (c=='D') ? 'W' : c;
	} while(!c);

	sprintf(str,"%s%c%s",fore(WHITE),c,fore(GRY));
	OUT(str);

	return(c);
}

int ins(int max)
{
	int i,strokes=(max<20) ? 30 : 3*max;
	char c[2]={ '\0','\0' };

	if(!timeout)
		timeout=60;
	OUT(fore(WHITE));
	memset(inbuf,0,INBUF_SIZE);
	while(strokes) {
		if(!(c[0]=getkey()))
			break;
		strokes--;
		if(!strokes)
			logoff=TRUE;
		if(strokes==5 || strokes==10)
			beep();
		switch(c[0]) {
			case 8:
				if((i=strlen(inbuf))) {
					inbuf[i-1]=NULL;
					RUBOUT;
				}
				break;
			case 13:
 				strokes=FALSE;
				break;
			case 24:
				for(i=strlen(inbuf); i>0; i--)
					RUBOUT;
				inbuf[0]=NULL;
				break;
			case 127:
				if((i=strlen(inbuf))) {
					inbuf[i-1]=NULL;
					RUBOUT;
				}
				break;
			default:
				if((i=strlen(inbuf))<max && c[0]>=' ') {
					strcat(inbuf,c);
					if(!echo)
						c[0]='.';
					OUT(c);
				}
				else
					beep();
				break;
		}
	}
	OUT(fore(GRY));
	if(cuss(inbuf)) {
		logoff=TRUE;
		memset(inbuf,0,INBUF_SIZE);
		NL;
		sprintf(outbuf,"%sSwearing is not allowed here!",fore(YELLOW));
		OUT(outbuf);NORMAL;NL;
		strcpy(reason,"was bad-mouthing");
	}
	timeout=0;
	return(strokes);
}

void jazz(char *str)
{
	int i,j,n;
	char cc[10],ca[4];

	for(i=0; i<strlen(str); i++) {
		if(str[i]=='\\') {
			n=0;
			for(j=i+1; j<strlen(str) && isdigit(str[j]); j++) {
				n*=10;
				n+=(str[j]-'0');
			}
			if(n%10<8) {
				strcpy(cc,"\33[");
				if(n>99)
					strcat(cc,"5;");
				if(n/10%10)
					strcat(cc,"1;");
				else
					strcat(cc,"0;");
				sprintf(ca,"%dm",30+(n%10));
				strcat(cc,ca);
				for(n=i; n<j; n++)
					memmove(&str[i],&str[i+1],strlen(str)-i);
				memmove(&str[i+strlen(cc)],&str[i],strlen(str)-i+1);
				strncpy(&str[i],cc,strlen(cc));
			}
		}
	}
}

void keyhint(void)
{
	static UBYTE *keys[] = {"Copper","Silver","Gold","Platinum"};
	int i,j=3,k,l;
	char c;

	for(i=0; i<4; i++)
		if(!strchr("CSGP",PLAYER.KeySequence[i]))
			ReKeySequence(ONLINE);

	for(k=0; k<ONLINE->CHA && j==3; k++) {
		i=dice(4)-1;
		for(j=0; j<3; j++)
			if(!PLAYER.KeyHints[i][j] || !strchr("CSGP",PLAYER.KeyHints[i][j])) {
				PLAYER.KeyHints[i][j]=NULL;
				break;
			}
	}
	if(k==ONLINE->CHA)
		return;
	do {
		do {
			l=dice(4)-1;
			c=*keys[l];
		} while(c==PLAYER.KeySequence[i]);
		for(k=0; k<j && PLAYER.KeyHints[i][k]!=c; k++);
		if(k==j)
			PLAYER.KeyHints[i][j]=c;
	} while(!PLAYER.KeyHints[i][j]);
	sprintf(outbuf,"Key #%d is NOT %s",i+1,keys[l]);
	OUT(outbuf);NL;
}

void modem_clr(void)
{
	Delay(1);
	memset(Serbuf,0,SERBUF_SIZE);
	if(SERCON.SerialIO) {
		SERCON.SerialIO->IOSer.io_Command=CMD_CLEAR;
		DoIO((struct IORequest *)SERCON.SerialIO);
	}
}

void modem_cmd(char *str)
{
	int i,loop=TRUE;

	Delay(5);
	OUTSER("\15");
	modem_clr();
	for(i=0; i<strlen(str); i++) {
		sprintf(inbuf,"%c",str[i]);
		OUTSER(inbuf);
		Delay(1);
	}
	OUTSER("\15");
	Delay(1);
	memset(inbuf,0,INBUF_SIZE);
	while(loop) {	//	until we get a modem response...
		INPUT(0);
		if(source==1) {
			OUTCON(Serbuf);
			for(i=0; i<strlen(Serbuf); i++) {
				if(!iscntrl(Serbuf[i]))
					inbuf[strlen(inbuf)]=Serbuf[i];
				if(Serbuf[i]=='\15' && strlen(inbuf)) {
						if(!strnicmp(inbuf,"OK",2)
						|| !strnicmp(inbuf,"ERROR",5)
						|| !strnicmp(inbuf,"CONNECT",7)
						|| !strnicmp(inbuf,"VOICE",5)
						|| !strnicmp(inbuf,"NO C",4)
						|| !strnicmp(inbuf,"BUSY",4)
						|| !strnicmp(inbuf,"RING",4))
						{
							loop=FALSE;
							if(Serbuf[strlen(Serbuf)-1]!='\12')
								OUTCON("\12");
							break;
						}
						if(stricmp(str,inbuf)) {	// repeat
							modem_clr();
							OUTSER(str);
							OUTSER("\15");
						}
					memset(inbuf,0,INBUF_SIZE);
				}
			}
		}
		else
			loop=FALSE;
	}
	modem_clr();
}

char *money(double n,int a)
{
	static char fnum[48];
	double r,s;
	int h,i;

	h=ANSI;
	ANSI=a;
	modf(n,&r);
	if(n<1e+05)
		sprintf(fnum,"%s%.5g%sc",fore(WHITE),r,fore(RED));
	else
	if(n<1e+09) {
		modf(n/1e+05,&r);
		sprintf(fnum,"%s%.5g%ss",fore(WHITE),r,fore(CYAN));
		modf(n-r*1e+05,&s);
		if(s>0.) {
			i=strlen(fnum);
			sprintf(&fnum[i],"%s,%.5g%sc",fore(WHITE),s,fore(RED));
		}
	}
	else
	if(n<1e+13) {
		modf(n/1e+09,&r);
		sprintf(fnum,"%s%.4g%sg",fore(WHITE),r,fore(YELLOW));
		modf((n-r*1e+09)/1e+05,&s);
		if(s>0.) {
			i=strlen(fnum);
			sprintf(&fnum[i],"%s,%.5g%ss",fore(WHITE),s,fore(CYAN));
		}
	}
	else {
		modf(n/1e+13,&r);
		sprintf(fnum,"%s%.8g%sp",fore(WHITE),r,fore(MAGENTA));
		modf((n-r*1e+13)/1e+09,&s);
		if(s>0.) {
			i=strlen(fnum);
			sprintf(&fnum[i],"%s,%.5g%sg",fore(WHITE),s,fore(YELLOW));
		}
	}
	strcat(fnum,fore(GRY));
	ANSI=h;
	return(fnum);
}

char more(void)
{
	char c;

	modem_clr();
	NORMAL;REVERSE;
	OUT("-more-");
	NORMAL;
	do {
		c=inkey('Y','N');
	} while(c!='N' && c!='Y');
	SAMELINE;CLL;
	return(c);
}

void news(char *item)
{
	FILE *fp;

	strmfp(filename,"T:",PLAYER.ID);
	if((fp=fopen(filename,"a"))) {
		fprintf(fp,"    %s\33[0;37m\n",item);
		fclose(fp);
	}
}

void note(char *who)
{
	FILE *fp;
	int i,j;
	char timestamp[20];
	char *p;

	if(strcmp(PLAYER.ID,who) && *who!='_') {
		strmfp(filename,NOTESpath,who);
		if((fp=fopen(filename,"a"))) {
			GetSystemDate(Today);
			GetSystemTime(Time);
			julian(&Julian,Today);
			fdate(timestamp,Julian);
			timestamp[10]=' ';
			ftime(&timestamp[11],Time);
			for(i=0; i<numline; i++) {
				p=line[i];
				if(strlen(p)>60) {
					for(j=60; j>0 && p[j]!=' '; j--);
					p[j++]=NULL;
					strcpy(outbuf,&p[j]);
				}
				else
					outbuf[0]=NULL;
				fprintf(fp,"%18.18s  %s\n",timestamp,p);
				memset(timestamp,0,sizeof(timestamp));
				if(strlen(outbuf))
					fprintf(fp,"                    %s\n",outbuf);
			}
			fprintf(fp,"\n");
			fclose(fp);
		}
	}
}

char option(struct RPGmenu *RPGmenu)
{
	static char c;
	char menufile[41];
	int i;

	title(RPGmenu->option);
	chkexp();
	if(!ONLINE->HP && !strlen(reason)) {
		strcpy(reason,"fought bravely?");
		logoff=TRUE;
	}
	timeleft();
	if(cd())
		return('Q');
	if(!PLAYER.Expert || c=='?') {
		cls();
		strmfp(menufile,"Menu",RPGmenu->option);
		if(!type(menufile,FALSE)) {
			for(i=0; i<RPGmenu->items; i++) {
				sprintf(outbuf,"%s<%s%c%s>%s %s",fore(RPGmenu->color),fore(WHITE),RPGmenu->item[i].choice,fore(RPGmenu->color),fore(CYN),RPGmenu->item[i].name);
				OUT(outbuf);NL;
			}
		}
	}
	RPGserver(SERVER_UNLOCK,NULL);
	if(!stricmp(RPGmenu->option,"main")) {
		sprintf(outbuf,"%sTime Left:%s %d min.",fore(MAG),fore(GRY),timeleft());
		NL;OUT(outbuf);
	}
	sprintf(prompt,"%s[%s%s%s]%s Option (Q=Quit): ",fore(RPGmenu->color),fore(YELLOW),RPGmenu->option,fore(RPGmenu->color),fore(CYN));
	NL;OUT(prompt);
	while((c=inkey('?','Q'))) {
		for(i=0; i<RPGmenu->items && c!=RPGmenu->item[i].choice; i++);
		if(i==RPGmenu->items && c!='?' && c!='Q') {
			modem_clr();
			beep();
			RUBOUT;
		}
		else
			break;
	}
	NL;NL;
	return(c);
}

void pause(void)
{
	modem_clr();
	NORMAL;REVERSE;
	OUT("-pause-");
	NORMAL;
	getkey();
	SAMELINE;CLL;
	paws=FALSE;
}

void ReKeySequence(struct RPC *rpc)
{
	static UBYTE keys[24][4] = {
		{ 'C','S','G','P' },{ 'C','S','P','G' },{ 'C','G','S','P' },{ 'C','G','P','S' },{ 'C','P','S','G' },{ 'C','P','G','S' },
		{ 'S','C','G','P' },{ 'S','C','P','G' },{ 'S','G','C','P' },{ 'S','G','P','C' },{ 'S','P','C','G' },{ 'S','P','G','C' },
		{ 'G','C','S','P' },{ 'G','C','P','S' },{ 'G','S','C','P' },{ 'G','S','P','C' },{ 'G','P','C','S' },{ 'G','P','S','C' },
		{ 'P','C','S','G' },{ 'P','C','G','S' },{ 'P','S','C','G' },{ 'P','S','G','C' },{ 'P','G','C','S' },{ 'P','G','S','C' }
	};
	memcpy(rpc->user.KeySequence,&keys[dice(24)-1][0],4);
	memset(rpc->user.KeyHints,0,sizeof(rpc->user.KeyHints));
}

void reroll(struct RPC *rpc)
{
	strcpy(rpc->user.Status,NEW->Status);
	strcpy(rpc->user.Class,NEW->Class);
	rpc->user.Gender=rpc->user.Sex;
	rpc->user.Level=1;
	rpc->user.ExpLevel=1;
	rpc->user.Experience=NEW->Experience;
	rpc->user.Gold=NEW->Gold;
	rpc->user.Bank=NEW->Bank;
	if(rpc->user.Novice=='Y')
		rpc->user.Bank*=10.;
	rpc->user.Loan=NEW->Loan;
	rpc->user.HP=NEW->HP;
	rpc->user.SP=NEW->SP;
	rpc->user.STR=NEW->STR;
	rpc->user.INT=NEW->INT;
	rpc->user.DEX=NEW->DEX;
	rpc->user.CHA=NEW->CHA;
	memset(rpc->user.Blessed,0,sizeof(rpc->user.Blessed));
	memset(rpc->user.Cursed,0,sizeof(rpc->user.Cursed));
	strcpy(rpc->user.Weapon,NEW->Weapon);
	rpc->user.WCmod=NEW->WCmod;
	strcpy(rpc->user.Armor,NEW->Armor);
	rpc->user.ACmod=NEW->ACmod;
	rpc->user.RealEstate=NEW->RealEstate;
	rpc->user.Security=NEW->Security;
	rpc->user.Spell=NEW->Spell;
	rpc->user.XSpell=NEW->XSpell;
	rpc->user.Poison=NEW->Poison;
	rpc->user.Hull=NEW->Hull;
	rpc->user.Cannon=NEW->Cannon;
	rpc->user.Ram=NEW->Ram;
	rpc->user.Current.Calls=0;
	rpc->user.Current.Kills=0;
	rpc->user.Current.Killed=0;
	rpc->user.Current.Retreats=0;
	rpc->user.Current.KOs=0;
	rpc->user.Current.KOed=0;
	rpc->user.Current.JoustWin=0;
	rpc->user.Current.JoustLoss=0;
	rpc->user.Bounty=0.;
	memset(rpc->user.BountyWho,0,sizeof(rpc->user.BountyWho));
	RPGserver(SERVER_PUTUSER,(UBYTE *)rpc);
	LoadRPC(rpc);
}

void sound(char *name,int vol)
{
	FILE *fp;

	if(local || !vol) {
		if(sample1->data) {
//			while(gs_sound_check())
//				Delay(1);
			gs_stop_sound(CHANNEL1);
			gs_stop_sound(CHANNEL3);
			gs_close_sound();
			gs_free_sound(sample1);
			sample1->data=NULL;
			if(sample2->data) {
				gs_free_sound(sample2);
				sample2->data=NULL;
			}
		}
		sample1->flags=SND_FAST;
		sample2->flags=SND_FAST;
		strmfp(filename,"Sounds",name);
		if((fp=fopen(filename,"rb"))) {
			fclose(fp);
			if(gs_load_iff_sound(sample1,sample2,filename)==0) {
				sample1->volcnt=vol ? vol : 64;
				sample2->volcnt=vol ? vol : 64;
				if(gs_open_sound(0,0,0,(4096/2))==0) {
					Disable();
					gs_start_sound(sample1,CHANNEL1);
					if(sample2->data)
						gs_start_sound(sample2,CHANNEL3);
					else
						gs_start_sound(sample1,CHANNEL3);
					Enable();
				}
			}
		}
	}
}

int timeleft(void)
{
	int result;

	GetSystemDate(Today);
	GetSystemTime(Time);
	julian(&Julian,Today);

	if(local && ACCESS(acclvl)->Sysop=='Y')
		result=1440;
	else
		result=ACCESS(acclvl)->Minutes-(1440*(Julian-PLAYER.LastDate)+(atol(Time)/10000*60+(atol(Time)/100)%100-PLAYER.LastTime/100*60-PLAYER.LastTime%100));
	if(result<0 && !cd()) {
		beep();NL;
		sprintf(outbuf,"%sYour %d minute time limit has expired!",fore(YELLOW),ACCESS(acclvl)->Minutes);
		OUT(outbuf);NL;
		NORMAL;NL;
		logoff=TRUE;
		strcpy(reason,"was hogging the phone line");
	}
	return(result);
}

void title(char *str)
{
	static char title[96];

	if(window) {
		strcpy(title,BBSprefs.Title);
		if(strlen(PLAYER.Name)) {
			sprintf(outbuf," | %.30s",PLAYER.Name);
			strcat(title,outbuf);
		}
		if(baud>75) {
			sprintf(outbuf," | %lu",baud);
			strcat(title,outbuf);
		}
		if(local)
			strcat(title," | local");
		if(chat)
			strcat(title," | CHAT");
		strcat(title," | ");
		strcat(title,str);
		SetWindowTitles(window,title,(UBYTE *)-1);
	}
}

int type(char *name,int paws)
{
	FILE *fp;
	int lp,size;
	char *b,c;

	strmfe(inbuf,name,ANSI==1 ? "" : ANSI==2 ? "IBM" : "PXL");
	if(!(fp=fopen(inbuf,"rb"))) {
		if(ANSI==3) {
			strmfe(inbuf,name,"IBM");
			fp=fopen(inbuf,"rb");
		}
		if(!fp)
			if(!(fp=fopen(name,"rb")))
				return(FALSE);
	}
	memset(extension,0,sizeof(extension));
	strsfn(inbuf,NULL,NULL,NULL,extension);
	if(stricmp(extension,"PXL")) {
		c='Y';
		lp=0;
		NORMAL;
		while(!feof(fp) && c!='N') {
			if(++lp%rows) {
				if(fgets(inbuf,INBUF_SIZE,fp)) {
					OUT(inbuf);
					OUT("\15");
				}
			}
			else {
				if((c=more())=='Y')
					paws=TRUE;
				else
					paws=FALSE;
			}
		}
	}
	else {
		fseek(fp,0,SEEK_END);
		size=ftell(fp);
		if((b=calloc(1,size+1))) {
			fseek(fp,0,SEEK_SET);
			fread(b,size,1,fp);
			SendPXL(&SERCON,b);
			free(b);
		}
	}
	fclose(fp);
	NORMAL;
	if(paws)
		pause();
	return(TRUE);
}

int type_pix(char *name,char sex)
{
	FILE *fp=NULL;
	int size;
	char *b;

	if(sex=='F') {
		strcpy(inbuf,name);
		strcat(inbuf,"_F");
		strcat(inbuf,ANSI==1 ? "" : ANSI==2 ? ".IBM" : ".PXL");
		if(!(fp=fopen(inbuf,"rb"))) {
			if(ANSI==3) {
				strcpy(inbuf,name);
				strcat(inbuf,"_F");
				strcat(inbuf,".IBM");
				fp=fopen(inbuf,"rb");
			}
		}
	}
	if(!fp) {
		strmfe(inbuf,name,ANSI==1 ? "" : ANSI==2 ? "IBM" : "PXL");
		if(!(fp=fopen(inbuf,"rb"))) {
			if(ANSI==3) {
				strmfe(inbuf,name,"IBM");
				fp=fopen(inbuf,"rb");
			}
		}
		if(!fp)
			if(!(fp=fopen(name,"rb")))
				return(FALSE);
	}
	memset(extension,0,sizeof(extension));
	strsfn(inbuf,NULL,NULL,NULL,extension);
	if(stricmp(extension,"PXL")) {
		NORMAL;
		while(!feof(fp)) {
			if(fgets(inbuf,INBUF_SIZE,fp)) {
				OUT(inbuf);
				OUT("\15");
			}
		}
	}
	else {
		fseek(fp,0,SEEK_END);
		size=ftell(fp);
		if((b=calloc(1,size+1))) {
			fseek(fp,0,SEEK_SET);
			fread(b,size,1,fp);
			SendPXL(&SERCON,b);
			free(b);
		}
	}
	fclose(fp);
	NORMAL;
	return(TRUE);
}

DOUBLE value(DOUBLE cost,UBYTE charisma)
{
	double r;

	modf(cost*charisma/100.,&r);
	return(r);
}

int GetRPC(struct RPC *rec)
{
	int i,hi,lo,result;
	char c,saveprompt[80];

	strcpy(saveprompt,prompt);
	memset(rec,0,sizeof(struct RPC));
	FOREVER {
		RPGserver(SERVER_UNLOCK,NULL);
		echo=TRUE;
		result=FALSE;
		OUT(saveprompt);
		if(ins(sizeof(PLAYER.Handle)))
			break;
		NL;
		if(*inbuf=='?' && PLAYER.Level) {
			lo=(PLAYER.Level>3) ? PLAYER.Level-3 : 1;
			sprintf(prompt,"%sStarting level %s%s: ",fore(CYN),bracket(lo),fore(CYN));
			OUT(prompt);
			if(ins(2))
				break;
			if(!strlen(inbuf)) {
				sprintf(outbuf,"%s%d",fore(WHITE),lo);
				OUT(outbuf);
			}
			else
				lo=atoi(inbuf);
			NL;
			hi=(PLAYER.Level<97) ? PLAYER.Level+3 : 99;
			sprintf(prompt,"%s  Ending level %s%s: ",fore(CYN),bracket(hi),fore(CYN));
			OUT(prompt);
			if(ins(2))
				break;
			if(!strlen(inbuf)) {
				sprintf(outbuf,"%s%d",fore(WHITE),hi);
				OUT(outbuf);
			}
			else
				hi=atoi(inbuf);
			NL;
			sprintf(outbuf,"%s%s ID   Player's Handle           Class    Lvl    Last On     Access Level  %s",back(BLU),fore(WHITE),back(BLK));
			OUT(outbuf);NL;
			sprintf(outbuf,"%s%s--------------------------------------------------------------------------%s",back(BLU),fore(GRY),back(BLK));
			OUT(outbuf);NL;
			i=2;
			sprintf(userlevel.Current,"%03u",hi+1);
			while(RPGserver(SERVER_GETUSERLEVEL,(UBYTE *)&userlevel)) {
				if(atoi(userlevel.Current)<lo || atoi(userlevel.Current)>hi)
					break;
				if(++i%rows==0)
					if((c=more())=='N')
						break;
				strcpy(rec->user.ID,userlevel.ID);
				RPGserver(SERVER_GETUSER,(UBYTE *)&rec->user);
				LoadRPC(rec);
				fdate(dated,rec->user.LastDate);
				sprintf(outbuf,"%s%-4.4s  %-22s  %-9.9s  %3u  %-11.11s  %-15.15s",
					(strlen(rec->user.Status) ? fore(BLACK) : (strncmp(rec->user.ID,PLAYER.ID,sizeof(rec->user.ID)) ? fore(GRY) : fore(WHITE))),
					rec->user.ID,rec->user.Handle,
					CLASS(rec)->Name,rec->user.Level,
					&dated[4],ACCESS(rec->user.Access)->Name);
				OUT(outbuf);NL;
				NORMAL;
			}
			NL;
			continue;
		}
		if(strlen(inbuf)<sizeof(PLAYER.ID)) {
			strupr(inbuf);
			strcpy(rec->user.ID,inbuf);
			if(RPGserver(SERVER_GETUSER,(UBYTE *)&rec->user))
				result=TRUE;
		}
		if(!result) {
			proper(inbuf);
			strcpy(username.Handle,inbuf);
			if(RPGserver(SERVER_GETUSERNAME,(UBYTE *)&username)) {
				strcpy(rec->user.ID,username.ID);
				if(RPGserver(SERVER_GETUSER,(UBYTE *)&rec->user))
					result=TRUE;
			}
		}
		if(result)
			if(rec!=ONLINE)
				if(!RPGserver(SERVER_LOCK,rec->user.ID))
					continue;
		break;
	}
	NL;
	if(result)
		LoadRPC(rec);
	else
		memset(rec,0,sizeof(struct RPC));
	displayview();
	return(result);
}

void LoadRPC(struct RPC *rpc)
{
	char origin[9],name[32];

	strsfn(rpc->user.Class,NULL,NULL,origin,name);
	for(rpc->class_origin=ServerTable->NumClass-1; rpc->class_origin>0 && stricmp(origin,ServerTable->class[rpc->class_origin]->Origin); rpc->class_origin--);
	for(rpc->class_type=ServerTable->class[rpc->class_origin]->NumCharacter-1; rpc->class_type>0 && stricmp(name,CLASS(rpc)->Name); rpc->class_type--);

	strsfn(rpc->user.Weapon,NULL,NULL,origin,name);
	for(rpc->weapon_origin=0; rpc->weapon_origin<NUMWEAPON && stricmp(origin,ServerTable->weapon[rpc->weapon_origin]->Origin); rpc->weapon_origin++);
	if(rpc->weapon_origin<NUMWEAPON)
		for(rpc->weapon_type=0; rpc->weapon_type<ServerTable->weapon[rpc->weapon_origin]->NumWeapon && stricmp(name,WEAPON(rpc)->Name); rpc->weapon_type++);
	else
		rpc->weapon_type=atoi(name);

	strsfn(rpc->user.Armor,NULL,NULL,origin,name);
	for(rpc->armor_origin=0; rpc->armor_origin<NUMARMOR && stricmp(origin,ServerTable->armor[rpc->armor_origin]->Origin); rpc->armor_origin++);
	if(rpc->armor_origin<NUMARMOR)
		for(rpc->armor_type=0; rpc->armor_type<ServerTable->armor[rpc->armor_origin]->NumArmor && stricmp(name,ARMOR(rpc)->Name); rpc->armor_type++);
	else
		rpc->armor_type=atoi(name);

	if(rpc==ONLINE)
		switch(rpc->user.Gender) {
			case 'M':
				rpc->He="You";
				rpc->His="Your";
				rpc->he="you";
				rpc->his="your";
				rpc->him="you";
				break;
			case 'F':
				rpc->He="You";
				rpc->His="Your";
				rpc->he="you";
				rpc->his="your";
				rpc->him="you";
				break;
			default:
				rpc->He="You";
				rpc->His="Your";
				rpc->he="you";
				rpc->his="your";
				rpc->him="you";
				break;
		}
	else
		switch(rpc->user.Gender) {
			case 'M':
				rpc->He="He";
				rpc->His="His";
				rpc->he="he";
				rpc->his="his";
				rpc->him="him";
				break;
			case 'F':
				rpc->He="She";
				rpc->His="Her";
				rpc->he="she";
				rpc->his="her";
				rpc->him="her";
				break;
			default:
				rpc->He="It";
				rpc->His="Its";
				rpc->he="it";
				rpc->his="its";
				rpc->him="it";
				break;
		}
	rpc->BP=rpc->user.HP/10;
	rpc->HP=rpc->user.HP;
	rpc->SP=rpc->user.SP;
	rpc->HULL=rpc->user.Hull;
	rpc->STR=rpc->user.STR;
	rpc->INT=rpc->user.INT;
	rpc->DEX=rpc->user.DEX;
	rpc->CHA=rpc->user.CHA;
	rpc->ToAC=0;
	rpc->ToWC=0;
	if(strlen(rpc->user.Blessed)) {
		if((rpc->STR+=10)>100)
			rpc->STR=100;
		if((rpc->INT+=10)>100)
			rpc->INT=100;
		if((rpc->DEX+=10)>100)
			rpc->DEX=100;
		if((rpc->CHA+=10)>100)
			rpc->CHA=100;
	}
	if(strlen(rpc->user.Cursed)) {
		if((rpc->STR-=10)<1)
			rpc->STR=0;
		if((rpc->INT-=10)<1)
			rpc->INT=0;
		if((rpc->DEX-=10)<1)
			rpc->DEX=0;
		if((rpc->CHA-=10)<1)
			rpc->CHA=0;
	}
	rpc->Confused=FALSE;
}

int RPGserver(char command,UBYTE *p)
{
	static UBYTE flag;

	strcpy(server.client,PLAYER.ID);
	server.command=command;
	server.parameter=p;
	server.result=&flag;
	PutMsg(RPGport,(struct Message *)&server);
	WaitPort(RPGreply);
	if((response=(struct RPGserver *)GetMsg(RPGreply))) {
		switch(command) {
			case SERVER_CONNECT:
				ServerTable=(struct ServerTable *)server.parameter;
				break;
			case SERVER_LOCK:
				if(!flag) {
					NL;beep();
					sprintf(outbuf,"%s%s is engaged in battle at this time.",fore(CYAN),p);
					OUT(outbuf);NL;
					NORMAL;NL;
				}
				break;
			case SERVER_UNLOCK:
				if(*response->result) {
					OUT(response->result);
					NORMAL;NL;NL;
				}
				break;
		}
	}
	return(flag);
}

int OpenGUI(void)
{
	struct DrawInfo *pubscreen_drawinfo;
	UBYTE *PubScreenName="RPGBBS Deluxe";
	int titlebar;

	if(!(DiskfontBase=(struct Library *)OpenLibrary("diskfont.library",0L)))
		return(FALSE);
	SERCON.TextAttr.ta_Name=BBSprefs.Font;
	SERCON.TextAttr.ta_YSize=BBSprefs.Size;
	SERCON.TextAttr.ta_Style=FS_NORMAL;
	SERCON.TextAttr.ta_Flags=FPF_DISKFONT;
	if(!(SERCON.TextFont=(struct TextFont *)OpenDiskFont(&SERCON.TextAttr)))
		return(FALSE);
	if(!(pubscreen=(struct Screen *)LockPubScreen(PubScreenName)))
		return(FALSE);
	if((pubscreen_drawinfo=GetScreenDrawInfo(pubscreen))) {
		ScreenToFront(pubscreen);
		titlebar=pubscreen_drawinfo->dri_Font->tf_YSize+pubscreen->WBorTop+1;
		zoom[0]=0;
		zoom[1]=titlebar*BBSprefs.Offset;
		zoom[2]=pubscreen->Width;
		zoom[3]=pubscreen->Height/2;
		if(!(window=(struct Window *)OpenWindowTags(NULL,
					WA_PubScreen, pubscreen,
					WA_Left,0, WA_Top,titlebar*BBSprefs.Offset,
					WA_Width, pubscreen->Width, WA_Height, pubscreen->Height-titlebar*BBSprefs.Offset,
					WA_MaxWidth, ~0, WA_MaxHeight, ~0,
					WA_MinWidth, pubscreen->Width, WA_MinHeight, (pubscreen->Height-titlebar*BBSprefs.Offset)/2,
					WA_Activate, TRUE,
					WA_Borderless, FALSE,
					WA_CloseGadget, TRUE,
					WA_DepthGadget, TRUE,
					WA_DragBar, TRUE,
					WA_SizeGadget, FALSE,
					WA_SizeBBottom, FALSE,
					WA_Zoom, &zoom[0],
					WA_GimmeZeroZero, TRUE,
					WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK | IDCMP_NEWSIZE | IDCMP_RAWKEY,
					WA_NoCareRefresh, TRUE,
					WA_SmartRefresh, TRUE,
					TAG_END)))
			return(FALSE);
		SetFont(window->RPort,SERCON.TextFont);
		project.Width=10*pubscreen_drawinfo->dri_Font->tf_XSize;
		project.Height=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		iconify.TopEdge=0;
		iconify.Width=16*pubscreen_drawinfo->dri_Font->tf_XSize;
		iconify.Height=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		Mlogoff.TopEdge=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		Mlogoff.Width=16*pubscreen_drawinfo->dri_Font->tf_XSize;
		Mlogoff.Height=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		Mlogon.TopEdge=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		Mlogon.Width=16*pubscreen_drawinfo->dri_Font->tf_XSize;
		Mlogon.Height=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		view.TopEdge=2*(pubscreen_drawinfo->dri_Font->tf_YSize+2);
		view.Width=16*pubscreen_drawinfo->dri_Font->tf_XSize;
		view.Height=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		shutdown.TopEdge=3*(pubscreen_drawinfo->dri_Font->tf_YSize+2);
		shutdown.Width=16*pubscreen_drawinfo->dri_Font->tf_XSize;
		shutdown.Height=pubscreen_drawinfo->dri_Font->tf_YSize+2;
		SetMenuStrip(window,&project);
		UnlockPubScreen(PubScreenName,pubscreen);
		SERCON.Window=window;
		OpenConsole(&SERCON);
	}
	else
		return(FALSE);
	return(TRUE);
}

void CloseGUI(void)
{
	if(viewwindow) {
		CloseWindow(viewwindow);
		viewwindow=NULL;
	}
	if(SERCON.ScreenBuffer)
		CloseConsole(&SERCON);
	if(window) {
		ClearMenuStrip(window);
		CloseWindow(window);
	}
	if(SERCON.TextFont)
		CloseFont(SERCON.TextFont);
	if(DiskfontBase)
		CloseLibrary(DiskfontBase);
}

int Init(void)
{
	FILE *fp;
	int i,j;

	if(!(DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0L)))
		return(FALSE);
	if(!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0L)))
		return(FALSE);
	if(!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0L)))
		return(FALSE);
	if(!(RPGBase=OpenLibrary("rpgbbs.library",0L))) {
		printf("rpgbbs.library is missing!\n");
		return(FALSE);
	}
	DesTrackerBase=(struct DTL_Base *)OpenLibrary("destracker.library",0L);

	if(!(Serbuf=calloc(1,SERBUF_SIZE)))
		return(FALSE);
	if(!(outbuf=calloc(1,OUTBUF_SIZE)))
		return(FALSE);
	if(!(prompt=calloc(1,PROMPT_SIZE)))
		return(FALSE);
	if(!(inbuf=calloc(1,INBUF_SIZE)))
		return(FALSE);
	if(!(filename=calloc(1,FILE_SIZE)))
		return(FALSE);
	for(i=0; i<2; i++)
		for(j=0; j<5; j++)
			if(!(RPC[i][j]=calloc(1,sizeof(struct RPC))))
				return(FALSE);
	line=calloc(100,sizeof(char *));
	for(i=0; i<100; i++)
		line[i]=calloc(1,128);
	if(!(sample1=calloc(1,sizeof(struct sound_struct))))
		return(FALSE);
	if(!(sample2=calloc(1,sizeof(struct sound_struct))))
		return(FALSE);

	if(!(RPGreply=(struct MsgPort *)CreatePort(NULL,0)))
		return(TRUE);
	server.message.mn_Node.ln_Type=NT_MESSAGE;
	server.message.mn_Length=sizeof(struct RPGserver);
	server.message.mn_ReplyPort=RPGreply;
	Forbid();
	RPGport=(struct MsgPort *)FindPort("RPGBBS Deluxe Server");
	Permit();
	if(!RPGport) {
		printf("RPGBBS Deluxe Server is not running!\n");
		return(FALSE);
	}
	if(!RPGserver(SERVER_CONNECT,NULL)) {
		printf("Server is not accepting client connection.\n");
		return(FALSE);
	}

	getcwd(directory,sizeof(directory));
	if((fp=fopen("RPGBBS.paths","r"))) {
		finputs(EMAILpath,sizeof(EMAILpath),fp);
		finputs(MSGpath,sizeof(MSGpath),fp);
		finputs(NOTESpath,sizeof(NOTESpath),fp);
		finputs(UPLOADpath,sizeof(UPLOADpath),fp);
		finputs(inbuf,INBUF_SIZE,fp);
		MaxFILE=atoi(inbuf);
		for(i=0; i<MaxFILE; i++) {
			finputs(FILEname[i],sizeof(FILEname[i]),fp);
			finputs(FILEpath[i],sizeof(FILEpath[i]),fp);
		}
		fclose(fp);
	}

	GetSystemTime(Time);
	for(i=0; i<atoi(&Time[2]); i++)
		dice(100);

	if(RPGBBSmode==BBS) {
		if(!OpenGUI())
			return(FALSE);
		if(!(SERCON.TPort=CreatePort(NULL,0)))
			return(FALSE);
		if(!(SERCON.TimerIO=(struct timerequest *)CreateExtIO(SERCON.TPort,sizeof(struct timerequest))))
			return(FALSE);
		if(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)SERCON.TimerIO,0L))
			return(FALSE);
	}
	if(RPGBBSmode==DOOR) {
		memset(&SERCON,0,sizeof(SERCON));
		OpenConsole(&SERCON);
	}

	return(TRUE);
}

void Shutdown(void)
{
	int i,j;

	if(sample1) {
		if(sample1->data) {
			gs_stop_sound(CHANNEL1);
			gs_stop_sound(CHANNEL3);
			gs_close_sound();
			gs_free_sound(sample1);
		}
		free(sample1);
	}
	if(sample2) {
		if(sample2->data)
			gs_free_sound(sample2);
		free(sample2);
	}

	for(i=0; i<100; i++)
		if(line[i])
			free(line[i]);
	if(line)
		free(line);
	for(i=0; i<2; i++)
		for(j=0; j<5; j++)
			if(RPC[i][j])
				free(RPC[i][j]);
	if(filename)
		free(filename);
	if(inbuf)
		free(inbuf);
	if(prompt)
		free(prompt);
	if(outbuf)
		free(outbuf);
	if(Serbuf)
		free(Serbuf);
	if(RPGport)
		RPGserver(SERVER_DISCONNECT,NULL);
	if(RPGreply)
		DeletePort(RPGreply);
	if(SERCON.TimerIO)
		CloseDevice((struct IORequest *)SERCON.TimerIO);
	if(SERCON.TPort)
		DeletePort(SERCON.TPort);
	CloseGUI();
	if(DesTrackerBase)
		CloseLibrary((struct Library *)DesTrackerBase);
	if(RPGBase)
		CloseLibrary(RPGBase);
	if(GfxBase)
		CloseLibrary((struct Library *)GfxBase);
	if(IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);
	if(DOSBase)
		CloseLibrary((struct Library *)DOSBase);
}

void main(int argc,char **argv)
{
	int i,result;
	char *s,**tool;

	if(argc) {	/*  Shell  */
		RPGBBSmode=DOOR;
		s=argv[argc-1];
		strcpy(argument,s);
		root=Lock("",ACCESS_READ);
		CurrentDir(root);
	}
	else {		/*  Workbench  */
		WBenchMsg=(struct WBStartup *)argv;
		for(i=0,wbarg=WBenchMsg->sm_ArgList; i<WBenchMsg->sm_NumArgs; i++,wbarg++) {
			if(wbarg->wa_Lock && *wbarg->wa_Name)
				root=CurrentDir(wbarg->wa_Lock);
			s=wbarg->wa_Name;
		}
	}
	if((WorkbenchBase=OpenLibrary("workbench.library",0))) {
		if((IconBase=OpenLibrary("icon.library",0))) {
			strcpy(IconName,s);
			if((diskobj=GetDiskObject(IconName))) {
				RPGBBSmode=BBS;
				tool=(char **)diskobj->do_ToolTypes;
				if((s=(char *)FindToolType(tool,"TITLE")))
					strcpy(BBSprefs.Title,s);
				if((s=(char *)FindToolType(tool,"FONT")))
					strncpy(BBSprefs.Font,s,sizeof(BBSprefs.Font));
				if((s=(char *)FindToolType(tool,"SIZE")))
					BBSprefs.Size=atoi(s);
				if((s=(char *)FindToolType(tool,"DEVICE"))) {
					if(stricmp(s,"LOCAL"))
						strcpy(BBSprefs.Device,s);
					else {
						memset(BBSprefs.Device,0,sizeof(BBSprefs.Device));
						iconify.Flags&=~ITEMENABLED;
					}
				}
				if((s=(char *)FindToolType(tool,"UNIT")))
					BBSprefs.Unit=atoi(s);
				if((s=(char *)FindToolType(tool,"BAUD")))
					BBSprefs.Baud=atol(s);
				if((s=(char *)FindToolType(tool,"INIT")))
					strcpy(BBSprefs.Init,s);
				if((s=(char *)FindToolType(tool,"QUIT")))
					strcpy(BBSprefs.Quit,s);
				if((s=(char *)FindToolType(tool,"AREACODE")))
					BBSprefs.AreaCode=atoi(s);
				if((s=(char *)FindToolType(tool,"CALLBACK")))
					BBSprefs.Auto=atoi(s);
				if((s=(char *)FindToolType(tool,"CALLERID")))
					BBSprefs.CallerID=atoi(s);
				if((s=(char *)FindToolType(tool,"DELAY")))
					BBSprefs.Delay=atoi(s);
				if((s=(char *)FindToolType(tool,"OFFSET")))
					BBSprefs.Offset=atoi(s);
				FreeDiskObject(diskobj);
			}
		}
		else
			exit(1);
	}

	if(Init()) {
		do {
			result=Logon();
			if(strlen(PLAYER.ID)) {
				broadcast("logged off");
				RPGserver(SERVER_DISCONNECT,PLAYER.ID);
				memset(PLAYER.ID,0,sizeof(PLAYER.ID));
			}
			if(local) {
				SERCON.SerialIO=HoldSerialIO;
				SERCON.SerialMP=HoldSerialMP;
			}
			if(SERCON.SerialIO) {
				if(!result) {
					title("shutdown");
					sprintf(outbuf,"%sShutting down...\15\12%s",fore(CYN),fore(GRY));
					OUTCON(outbuf);
					modem_cmd(BBSprefs.Quit);
				}
				CloseDevice((struct IORequest *)SERCON.SerialIO);
				DeleteExtIO((struct IORequest *)SERCON.SerialIO);
				SERCON.SerialIO=NULL;
				Delay(result ? 125 : 50);
			}
			if(SERCON.SerialMP) {
				DeletePort(SERCON.SerialMP);
				SERCON.SerialMP=NULL;
			}
		} while(result);
	}
	Shutdown();
	if(IconBase)
		CloseLibrary(IconBase);
	if(WorkbenchBase)
		CloseLibrary(WorkbenchBase);
}
