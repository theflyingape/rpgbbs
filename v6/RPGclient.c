/*****************************\
 *   Role Playing Game BBS   *
 *                           *
 *  Written by Robert Hurst  *
\*****************************/
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include "RPGBBS.h"

#define ISLEAP (((year%4)==0) && (((year%100)!=0) || ((year%400)==0)))

/***********************\
 *  Support Functions  *
\***********************/
void AdjustTime(int min)
{
	if(min>0) {
		online+=min;
		if(online%100>59) {
			online+=100;
			online=100*(online/100)+(60-online%100);
		}
	}
	if(min<0) {
		online+=min;
		if(online%100>59)
			online=100*(online/100)+(60-(100-online%100));
		if(online<0)
			online=0;
	}
}

int BlessAssign(void)
{
	int i,j;

	for(i=1; i<=MAXUSER; i++) {
		j=dice(MAXUSER);
		if(ACCESS(USER(j)->Access)->User=='Y' && !USER(j)->Blessed && !USER(j)->Cursed) {
			USER(j)->Blessed=user;
			PutUser(j);
			return(j);
		}
	}
	for(i=1; i<=MAXUSER; i++)
		if(ACCESS(USER(i)->Access)->User=='Y' && !USER(i)->Blessed && !USER(i)->Cursed) {
			USER(i)->Blessed=user;
			PutUser(i);
			return(i);
		}
}

int BlessRemove(void)
{
	int i,j;

	for(i=1; i<=MAXUSER; i++) {
		j=dice(MAXUSER);
		if(USER(j)->Blessed) {
			USER(j)->Blessed=0;
			PutUser(j);
			return(j);
		}
	}
	for(i=MAXUSER; i; i--)
		if(USER(i)->Blessed) {
			USER(i)->Blessed=0;
			PutUser(i);
			return(i);
		}
}

char *bracket(int n)
{
	static char r[32];

	sprintf(r,"%s%s<%s%d%s>%s",(n<10) ? " " : "",Fcol(BLU),Fcol(WHITE),n,Fcol(BLU),Fcol(GRY));
	return(r);
}

int CurseAssign(void)
{
	int i,j;

	for(i=1; i<=MAXUSER; i++) {
		j=dice(MAXUSER);
		if(ACCESS(USER(j)->Access)->User=='Y' && !USER(j)->Blessed && !USER(j)->Cursed) {
			USER(j)->Cursed=user;
			PutUser(j);
			return(j);
		}
	}
	for(i=MAXUSER; i; i--)
		if(ACCESS(USER(i)->Access)->User=='Y' && !USER(i)->Blessed && !USER(i)->Cursed) {
			USER(i)->Cursed=user;
			PutUser(i);
			return(i);
		}
}

int CurseRemove(void)
{
	int i,j;

	for(i=1; i<=MAXUSER; i++) {
		j=dice(MAXUSER);
		if(USER(j)->Cursed) {
			USER(j)->Cursed=0;
			PutUser(j);
			return(j);
		}
	}
	for(i=1; i<=MAXUSER; i++)
		if(USER(i)->Cursed) {
			USER(i)->Cursed=0;
			PutUser(i);
			return(i);
		}
}

int dice(int n)
{
	int r;

	if(n)
		r=rand()/10%n+1;
	else
		r=1;
	return(r);
}

void DisplayUser(void)
{
	int i;
	char magic[16];

	if(userwindow) {
		SetRast(userwindow->RPort,WHITE);
		if(user) {
			SetDrMd(userwindow->RPort,JAM1);
			SetAPen(userwindow->RPort,BLUE);
			sprintf(text," Name: %-26s",USER(user)->Handle);
			Move(userwindow->RPort,0,7);
			Text(userwindow->RPort,text,strlen(text));
			sprintf(text,"Class: %-.9s (%d)",CLASS(USER(user)->Class)->Name,USER(user)->Immortal);
			Move(userwindow->RPort,0,16);
			Text(userwindow->RPort,text,strlen(text));
			sprintf(text,"Level: %-2d   Experience: %s",USER(user)->Level,Fnum(USER(user)->Experience));
			Move(userwindow->RPort,0,25);
			Text(userwindow->RPort,text,strlen(text));
			sprintf(text,"Str:%3d   Int:%3d   Agl:%3d   Chr:%3d",Tstr,Tint,Tagl,Tchr);
			Move(userwindow->RPort,0,34);
			Text(userwindow->RPort,text,strlen(text));
			for(i=0; i<sizeof(magic); i++)
				magic[i]=(USER(user)->Spell & (int)pow2((double)i)) ? 'Y' : '-';
			sprintf(text,"HP: %-4d   SP: %-4d   %16.16s",Thp,Tsp,magic);
			Move(userwindow->RPort,0,43);
			Text(userwindow->RPort,text,strlen(text));
			sprintf(text,"Weapon:%3d  %-.20s (%+d)",USER(user)->Weapon,(USER(user)->Weapon<0) ? "Natural" : WEAPON(USER(user)->Weapon)->Name,Thit);
			Move(userwindow->RPort,0,52);
			Text(userwindow->RPort,text,strlen(text));
			sprintf(text," Armor:%3d  %-.20s (%+d)",USER(user)->Armor,(USER(user)->Armor<0) ? "Natural" : ARMOR(USER(user)->Armor)->Name,Tshield);
			Move(userwindow->RPort,0,61);
			Text(userwindow->RPort,text,strlen(text));
			sprintf(text,"Hull: %-4d   Cannon: %-3d   Ram: %s",Thull,USER(user)->Cannon,(USER(user)->Ram=='Y') ? "Yes" : "No");
			Move(userwindow->RPort,0,70);
			Text(userwindow->RPort,text,strlen(text));
		}
	}
}

void DisplayNME(void)
{
	int i;
	char magic[16];

	if(nmewindow) {
		SetRast(nmewindow->RPort,WHITE);
		SetDrMd(nmewindow->RPort,JAM1);
		SetAPen(nmewindow->RPort,BLUE);
		sprintf(text," Name: %-26s",NME.Handle);
		Move(nmewindow->RPort,0,7);
		Text(nmewindow->RPort,text,strlen(text));
		sprintf(text,"Class: %-.9s (%d)",CLASS(NME.Class)->Name,NME.Immortal);
		Move(nmewindow->RPort,0,16);
		Text(nmewindow->RPort,text,strlen(text));
		sprintf(text,"Level: %-2d   Experience: %s",NME.Level,Fnum(NME.Experience));
		Move(nmewindow->RPort,0,25);
		Text(nmewindow->RPort,text,strlen(text));
		sprintf(text,"Str:%3d   Int:%3d   Agl:%3d   Chr:%3d",NMEstr,NMEint,NMEagl,NMEchr);
		Move(nmewindow->RPort,0,34);
		Text(nmewindow->RPort,text,strlen(text));
		for(i=0; i<sizeof(magic); i++)
			magic[i]=(NME.Spell & (int)pow2((double)i)) ? 'Y' : '-';
		sprintf(text,"HP: %-4d   SP: %-4d   %16.16s",NMEhp,NMEsp,magic);
		Move(nmewindow->RPort,0,43);
		Text(nmewindow->RPort,text,strlen(text));
		sprintf(text,"Weapon:%3d  %-.20s (%+d)",NME.Weapon,(NME.Weapon<0) ? "Natural" : WEAPON(NME.Weapon)->Name,NMEhit);
		Move(nmewindow->RPort,0,52);
		Text(nmewindow->RPort,text,strlen(text));
		sprintf(text," Armor:%3d  %-.20s (%+d)",NME.Armor,(NME.Armor<0) ? "Natural" : ARMOR(NME.Armor)->Name,NMEshield);
		Move(nmewindow->RPort,0,61);
		Text(nmewindow->RPort,text,strlen(text));
		sprintf(text,"Hull: %-4d   Cannon: %-3d   Ram: %s",NMEhull,USER(nme)->Cannon,(USER(nme)->Ram=='Y') ? "Yes" : "No");
		Move(nmewindow->RPort,0,70);
		Text(nmewindow->RPort,text,strlen(text));
	}
}

char Editor(int n)
{
	int i,j;
	char c;

	ww=TRUE;
	numline=0;
/*	if(ANSI)
		return(FullScreenEditor(n));
*/	sprintf(text,"%sEnter up to %d lines of text below.  Enter %s%s/%s%s on blank line for options.",Fcol(CYN),n,Bcol(BLU),Fcol(WHITE),Bcol(BLK),Fcol(CYN));
	outs(text);
	newline();
	for(numline=0; numline<n; numline++) {
		sprintf(text,"%s%2d%s] ",Fcol(YELLOW),numline+1,Fcol(BLU));
		outs(text);
		if(input())
			break;
		if(ins[0]=='/') {
			c=toupper(ins[1]);
			break;
		}
		newline();
		strcpy(line[numline],ins);
	}
	ww=FALSE;
	for(i=0; i<numline && strlen(line[i])==0; i++) {
		for(j=i+1; j<numline; j++)
			strcpy(line[j-1],line[j]);
		numline--;
	}
	for(i=numline; i>0 && strlen(line[i-1])==0; i--)
		numline--;
	if(numline<1)
		c='Q';
	while(c!='S' && c!='Q') {
		newline();
		sprintf(text,"%s<%sS%s>ave or <%sQ%s>uit: ",Fcol(CYN),Fcol(WHITE),Fcol(CYN),Fcol(WHITE),Fcol(CYN));
		outs(text);
		if(!(c=inkey()))
			c='Q';
	}
	return(c);
}

double ExpNeed(int level)
{
	double r;

	r=pow(2.,(double)level-1);
	r*=(1100.-2.*USER(user)->Int);
	return(r);
}

char *Fdate(char *str)
{
	static char fdate[13];
	int mm;

	mm=atol(str)/1000000-1;
	if(mm<0 || mm>11)
		mm=SystemMonth;
	sprintf(fdate,"%.3s %.2s, %.4s",__montbl[mm],&str[2],&str[4]);
	return(fdate);
}

char *Fnum(double n)
{
	static char fnum[16];
	double r;

	modf(n,&r);
	sprintf(fnum,"%.8g",r);
	return(fnum);
}

char FullScreenEditor(int n)
{
	return('S');
}

void GetSystemDateTime(void)
{
	char mmddyyyy[11];
	long t;

	time(&t);
	tp=localtime(&t);

	strftime(SystemDate,9,"%m%d%Y",tp);			/*  return mmddyyyy  */
	strftime(SystemTime,12,"%H:%M:%S",tp);		/*  return hh:mm:ss  */
	SystemMonth=atol(SystemDate)/1000000;
	Time=100*atoi(&SystemTime[0])+atoi(&SystemTime[3]);
	sprintf(mmddyyyy,"%.2s/%.2s/%.4s",&SystemDate[0],&SystemDate[2],&SystemDate[4]);
	JulianDate=julian(mmddyyyy);
}

void inputs(char *str,int n,FILE *fp)
{
	if(!fgets(str,n,fp))
		memset(str,0,n);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
}

char *money(double n,int rec)
{
	static char fnum[64];
	double r,s;
	int h,i;

	if(rec!=user) {
		h=ANSI;
		ANSI=USER(rec)->ANSI;
	}
	modf(n,&r);
	if(USER(rec)->Format) {
		if(n<1e+05)
			sprintf(fnum,"%s%.5g%sc",Fcol(WHITE),r,Fcol(RED));
		else
		if(n<1e+09) {
			modf(n/1e+05,&r);
			sprintf(fnum,"%s%.5g%ss",Fcol(WHITE),r,Fcol(CYAN));
			modf(n-r*1e+05,&s);
			if(s>0.) {
				i=strlen(fnum);
				sprintf(&fnum[i],"%s,%.5g%sc",Fcol(WHITE),s,Fcol(RED));
			}
		}
		else
		if(n<1e+13) {
			modf(n/1e+09,&r);
			sprintf(fnum,"%s%.4g%sg",Fcol(WHITE),r,Fcol(YELLOW));
			modf((n-r*1e+09)/1e+05,&s);
			if(s>0.) {
				i=strlen(fnum);
				sprintf(&fnum[i],"%s,%.5g%ss",Fcol(WHITE),s,Fcol(CYAN));
			}
		}
		else {
			modf(n/1e+13,&r);
			sprintf(fnum,"%s%.8g%sp",Fcol(WHITE),r,Fcol(MAGENTA));
			modf((n-r*1e+13)/1e+09,&s);
			if(s>0.) {
				i=strlen(fnum);
				sprintf(&fnum[i],"%s,%.5g%sg",Fcol(WHITE),s,Fcol(YELLOW));
			}
		}
	}
	else
		sprintf(fnum,"%s%.8g%sgp",Fcol(WHITE),r,Fcol(YELLOW));
	strcat(fnum,Fcol(GRY));
	if(rec!=user)
		ANSI=h;
	return(fnum);
}

void OffHook(void)
{
	if(SerialIO) {
		SerialIO->IOSer.io_Command=CMD_WRITE;
		SerialIO->IOSer.io_Length=-1;
		SerialIO->IOSer.io_Data=(APTR)"ATH1\15";
		DoIO((struct IORequest *)SerialIO);
		Delay(50);

		SerialIO->IOSer.io_Command=SDCMD_QUERY;
		DoIO((struct IORequest *)SerialIO);
		while(SerialIO->IOSer.io_Actual) {
			SerialIO->IOSer.io_Command=CMD_READ;
			SerialIO->IOSer.io_Length=SerialIO->IOSer.io_Actual;
			SerialIO->IOSer.io_Data=(APTR)INbuf;
			DoIO((struct IORequest *)SerialIO);
			Delay(1);
			SerialIO->IOSer.io_Command=SDCMD_QUERY;
			DoIO((struct IORequest *)SerialIO);
		}
		outs("modem is now off-hook");
	}
}

char *Ftime(UWORD t)
{
	static char ftime[9];
	int h,m,c;

	h=t/100;
	m=t%100;
	c='A';
	if(h>=12) {
		if(h>12)
			h-=12;
		c='P';
	}
	if(h==0)
		h=12;
	sprintf(ftime,"%02u:%02u %cM",h,m,c);
	return(ftime);
}

int GetUser(void)
{
	int rec;

	RPGserver('U',0);
	echo=TRUE;
	if(input())
		return(0);
	newline();
	strupr(Ltrim(ins));
	if(!strlen(ins))
		return(0);
	if(ins[0]=='?') {
		ListUsers();
		newline();
		outs(Fcol(CYN));
		outs("User? ");
		return(GetUser());
	}
	rec=atoi(ins);
	if(rec>0 && rec<=MAXUSER && USER(rec)->Access>0) {
		if(RPGserver('L',rec) && from!='@')
			rec=0;
		else
			memcpy(&NME,USER(rec),sizeof(struct User));
		newline();
		return(rec);
	}
	for(rec=1; rec<=MAXUSER && (strnicmp(ins,USER(rec)->Handle,sizeof(USER(rec)->Handle)) || USER(rec)->Access==0); rec++);
	if(rec>MAXUSER) {
		if(stricmp(ins,"NEW")==0 || stricmp(ins,"*")==0)
			return(-1);
		beep();
		newline();
		return(0);
	}
	if(RPGserver('L',rec) && from!='@')
		rec=0;
	else
		memcpy(&NME,USER(rec),sizeof(struct User));
	newline();
	return(rec);
}

void CheckHOF(int class)
{
	sprintf(text,"Checking your stats against %s Hall of Fame...",(class) ? CLASS(class)->Name : "All-Time");
	outs(text);
	Delay(50);
	newline();
	if(USER(user)->Calls<=FAME(class,0)->ValueLeast) {
		outs("You reached immortality with the fewest calls.");
		newline();
		FAME(class,0)->ValueLeast=USER(user)->Calls;
		FAME(class,0)->DateLeast=JulianDate;
		strncpy(FAME(class,0)->WhoLeast,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->Calls>=FAME(class,0)->ValueMost) {
		outs("You reached immortality with the most calls.");
		newline();
		FAME(class,0)->ValueMost=USER(user)->Calls;
		FAME(class,0)->DateMost=JulianDate;
		strncpy(FAME(class,0)->WhoMost,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->YouKill<=FAME(class,1)->ValueLeast) {
		outs("You reached immortality with the fewest kills.");
		newline();
		FAME(class,1)->ValueLeast=USER(user)->YouKill;
		FAME(class,1)->DateLeast=JulianDate;
		strncpy(FAME(class,1)->WhoLeast,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->YouKill>=FAME(class,1)->ValueMost) {
		outs("You reached immortality with the most kills.");
		newline();
		FAME(class,1)->ValueMost=USER(user)->YouKill;
		FAME(class,1)->DateMost=JulianDate;
		strncpy(FAME(class,1)->WhoMost,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->WasKill<=FAME(class,2)->ValueLeast) {
		outs("You reached immortality with the fewest times being killed.");
		newline();
		FAME(class,2)->ValueLeast=USER(user)->WasKill;
		FAME(class,2)->DateLeast=JulianDate;
		strncpy(FAME(class,2)->WhoLeast,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->WasKill>=FAME(class,2)->ValueMost) {
		outs("You reached immortality with the most times being killed.");
		newline();
		FAME(class,2)->ValueMost=USER(user)->WasKill;
		FAME(class,2)->DateMost=JulianDate;
		strncpy(FAME(class,2)->WhoMost,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->YouKO<=FAME(class,3)->ValueLeast) {
		outs("You reached immortality with the fewest knock outs.");
		newline();
		FAME(class,3)->ValueLeast=USER(user)->YouKO;
		FAME(class,3)->DateLeast=JulianDate;
		strncpy(FAME(class,3)->WhoLeast,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->YouKO>=FAME(class,3)->ValueMost) {
		outs("You reached immortality with the most knock outs.");
		newline();
		FAME(class,3)->ValueMost=USER(user)->YouKO;
		FAME(class,3)->DateMost=JulianDate;
		strncpy(FAME(class,3)->WhoMost,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->WasKO<=FAME(class,4)->ValueLeast) {
		outs("You reached immortality with the fewest times being knocked out.");
		newline();
		FAME(class,4)->ValueLeast=USER(user)->WasKO;
		FAME(class,4)->DateLeast=JulianDate;
		strncpy(FAME(class,4)->WhoLeast,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->WasKO>=FAME(class,4)->ValueMost) {
		outs("You reached immortality with the most times being knocked out.");
		newline();
		FAME(class,4)->ValueMost=USER(user)->WasKO;
		FAME(class,4)->DateMost=JulianDate;
		strncpy(FAME(class,4)->WhoMost,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->JoustWin>=FAME(class,5)->ValueLeast) {
		outs("You reached immortality with the most joust wins.");
		newline();
		FAME(class,5)->ValueLeast=USER(user)->JoustWin;
		FAME(class,5)->DateLeast=JulianDate;
		strncpy(FAME(class,5)->WhoLeast,USER(user)->Handle,26);
		nme=TRUE;
	}
	if(USER(user)->JoustLoss>=FAME(class,5)->ValueMost) {
		outs("You reached immortality with the most joust losses.");
		newline();
		FAME(class,5)->ValueMost=USER(user)->JoustLoss;
		FAME(class,5)->DateMost=JulianDate;
		strncpy(FAME(class,5)->WhoMost,USER(user)->Handle,26);
		nme=TRUE;
	}
}

void DisplayHOF(int class)
{
	cls();
	sprintf(text,"          %s--=:))%s%s %s Hall of Fame %s%s((:=--",Fcol(BLU),Fcol(WHITE),Bcol(BLU),(class) ? CLASS(class)->Name : "All-Time",Fcol(BLU),Bcol(BLK));
	outs(text);
	normal();
	newline();
	newline();
	outs("These mortals upon reaching immortality obtained the following notables:");
	newline();
	newline();
	sprintf(text,"%sTook the fewest calls:%s  %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,0)->ValueLeast,FAME(class,0)->WhoLeast,jdate(FAME(class,0)->DateLeast));
	outs(text);
	newline();
	sprintf(text,"%sTook the most calls:%s    %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,0)->ValueMost,FAME(class,0)->WhoMost,jdate(FAME(class,0)->DateMost));
	outs(text);
	newline();
	newline();
	sprintf(text,"%sThe fewest kills:%s       %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,1)->ValueLeast,FAME(class,1)->WhoLeast,jdate(FAME(class,1)->DateLeast));
	outs(text);
	newline();
	sprintf(text,"%sThe most kills:%s         %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,1)->ValueMost,FAME(class,1)->WhoMost,jdate(FAME(class,1)->DateMost));
	outs(text);
	newline();
	newline();
	sprintf(text,"%sThe fewest killed by:%s   %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,2)->ValueLeast,FAME(class,2)->WhoLeast,jdate(FAME(class,2)->DateLeast));
	outs(text);
	newline();
	sprintf(text,"%sThe most killed by:%s     %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,2)->ValueMost,FAME(class,2)->WhoMost,jdate(FAME(class,2)->DateMost));
	outs(text);
	newline();
	newline();
	sprintf(text,"%sThe fewest knock outs:%s  %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,3)->ValueLeast,FAME(class,3)->WhoLeast,jdate(FAME(class,3)->DateLeast));
	outs(text);
	newline();
	sprintf(text,"%sThe most knock outs:%s    %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,3)->ValueMost,FAME(class,3)->WhoMost,jdate(FAME(class,3)->DateMost));
	outs(text);
	newline();
	newline();
	sprintf(text,"%sThe fewest knocked out:%s %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,4)->ValueLeast,FAME(class,4)->WhoLeast,jdate(FAME(class,4)->DateLeast));
	outs(text);
	newline();
	sprintf(text,"%sThe most knocked out:%s   %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,4)->ValueMost,FAME(class,4)->WhoMost,jdate(FAME(class,4)->DateMost));
	outs(text);
	newline();
	newline();
	sprintf(text,"%sThe most joust wins:%s    %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,5)->ValueLeast,FAME(class,5)->WhoLeast,jdate(FAME(class,5)->DateLeast));
	outs(text);
	newline();
	sprintf(text,"%sThe most joust losses:%s  %-6u %26.26s on %s",Fcol(CYN),Fcol(WHITE),FAME(class,5)->ValueMost,FAME(class,5)->WhoMost,jdate(FAME(class,5)->DateMost));
	outs(text);
	newline();
}

void Email(int who)
{
	FILE *fp;
	int i;

	GetSystemDateTime();
	sprintf(text,"%sDate:%s %s  %s",Fcol(CYN),Fcol(YELLOW),Fdate(SystemDate),Ftime(Time));
	outs(text);
	newline();
	sprintf(text,"%sFrom:%s %.26s",Fcol(CYN),Fcol(YELLOW),USER(user)->Handle);
	outs(text);
	newline();
	sprintf(text,"%s  To:%s %.26s",Fcol(CYN),Fcol(YELLOW),USER(who)->Handle);
	outs(text);
	newline();
	newline();
	if(Editor(50)=='S') {
		outs("aving...");
		for(i=1; i<1000; i++) {
			sprintf(text,"Email%03d.%03d",who,i);
			strmfp(filename,EMAILpath,text);
			if(!(fp=fopen(filename,"r")))
				break;
			fclose(fp);
		}
		if((fp=fopen(filename,"w"))) {
			fprintf(fp,"%.8s%04u%03u%03u\n",SystemDate,Time,user,who);
			for(i=0; i<numline; i++)
				fprintf(fp,"%s\n",line[i]);
			fputs("\n",fp);
			fclose(fp);
			outs("Ok.");
		}
		else
			outs("Error!");
		newline();
		newline();
	}
}

void HallOfFame(void)
{
	sprintf(text,"%s     Calls made:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->Calls);
	outs(text);
	newline();
	sprintf(text,"%s     Kills made:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->YouKill);
	outs(text);
	newline();
	sprintf(text,"%s     Was killed:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->WasKill);
	outs(text);
	newline();
	sprintf(text,"%sKnock outs made:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->YouKO);
	outs(text);
	newline();
	sprintf(text,"%sWas knocked out:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->WasKO);
	outs(text);
	newline();
	sprintf(text,"%s     Jousts won:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->JoustWin);
	outs(text);
	newline();
	sprintf(text,"%s    Jousts lost:%s %u",Fcol(CYN),Fcol(WHITE),USER(user)->JoustLoss);
	outs(text);
	newline();
	newline();
	Pause();
	nme=FALSE;
	CheckHOF(0);
	newline();
	CheckHOF(USER(user)->Class);
	newline();
	if(nme) {
		RPGserver('H',0);
		newline();
		outs("Congratulations!  For making the board, you are rewarded this hint:");
		newline();
		Delay(250);
		outs(KeyHint());
		Delay(250);
		normal();
	}
}

char *jdate(ULONG days)
{
	static char result[9];
	unsigned int md[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
	unsigned int month,day,year;

	year=days/1461*4+days%1461/365;
	days=days-(year*365L+year/4)+1;
	month=0;
	while(days>md[month+1]-(ISLEAP && month==0 ? 1 : 0) && month<11)
		month++;
	days-=md[month++];
	day=days;
	if(ISLEAP && month<3)
		day++;
	sprintf(result,"%02u%02u%04u",month,day,year);
	return(Fdate(result));
}

ULONG julian(char *str)
{
	ULONG days;
	unsigned int md[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
	unsigned int month,day,year;

	sscanf(str,"%d/%d/%d",&month,&day,&year);
	days=year*365L+year/4+md[month-1]+day-1;
	if(ISLEAP && month<3)
		days--;
	weekday=(days-((year/100)-(year/400)))%7;
	return(days);
}

char *KeyHint(void)
{
	static char result[24];

	int i;
	char *c;

	i=dice(3)-1;
	c="given";
	switch(correct[i]) {
		case 'G':
			c=(dice(2)==1) ? "Silver" : "Copper";
			break;
		case 'S':
			c=(dice(2)==1) ? "Gold" : "Copper";
			break;
		case 'C':
			c=(dice(2)==1) ? "Gold" : "Silver";
			break;
	}
	sprintf(result,"Key #%d is NOT %s...",i+1,c);
	return(result);
}

void ListUsers(void)
{
	int i,lo,hi,lp;
	char more;

	if(user) {
		lo=USER(user)->Level-3;
		if(lo<1)
			lo=1;
		hi=USER(user)->Level+3;
		if(hi>99)
			hi=99;
		sprintf(text,"%sStarting level %s: ",Fcol(CYN),bracket(lo));
		outs(text);
		if(input())
			return;
		if(!strlen(ins)) {
			sprintf(text,"%d",lo);
			outs(text);
		}
		newline();
		i=atoi(ins);
		if(i>0 && i<100)
			lo=i;
		sprintf(text,"%s  Ending level %s: ",Fcol(CYN),bracket(hi));
		outs(text);
		if(input())
			return;
		if(!strlen(ins)) {
			sprintf(text,"%d",hi);
			outs(text);
		}
		newline();
		i=atoi(ins);
		if(i>0 && i<100)
			hi=i;
	}
	else {
		lo=1;
		hi=99;
	}
	newline();
	sprintf(OUTbuf,"%s%sNum           Username             Class   Lvl  Status  Last Call On   Access %s",Bcol(BLU),Fcol(WHITE),Bcol(BLK));
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"%s---  --------------------------  ---------  --  ------  ------------  --------%s",Bcol(BLU),Bcol(BLK));
	outs(OUTbuf);
	newline();
	normal();
	lp=2;
	more='Y';
	for(i=1; i<=MAXUSER && more!='N' && !cd(); i++) {
		if(USER(i)->Access && USER(i)->Level>=lo && USER(i)->Level<=hi) {
			sprintf(text,"%3d  %-26.26s  %-9s %s %s  %-12s  %-s",i,USER(i)->Handle,CLASS(USER(i)->Class)->Name,RankLevel(i),USER(i)->Status ? " Dead " : "Alive!",jdate(USER(i)->LastDate),ACCESS(USER(i)->Access)->Name);
			outs(text);
			newline();
			lp++;
			if(lp%USER(user)->Rows==0)
				more=More();
		}
	}
}

void LogLoser(char *who,char *what)
{
	FILE *fp;

	GetSystemDateTime();
	sprintf(filename,"News.%lu",JulianDate);
	if((fp=fopen(filename,"a"))) {
		fprintf(fp,"          %.26s lost to %.26s in a %s.\n",USER(user)->Handle,who,what);
		fclose(fp);
	}
}

void Logoff(void)
{
	FILE *fp;
	int i;

	newline();
	if(user) {
		GetSystemDateTime();
		USER(user)->LastDate=JulianDate;
		USER(user)->LastTime=Time;
		USER(user)->LastFileScan=LastFileScan;
		PutUser(user);
		sprintf(filename,"News.%lu",JulianDate);
		if((fp=fopen(filename,"a"))) {
			if(USER(user)->Level>=ToStart.Level) {
				if(USER(user)->Weapon<ToStart.Weapon)
					fprintf(fp,"          %.26s weapon to start: %u.  Left with %u.\n",USER(user)->Handle,ToStart.Weapon,USER(user)->Weapon);
				if(USER(user)->Armor<ToStart.Armor)
					fprintf(fp,"          %.26s armor to start: %u.  Left with %u.\n",USER(user)->Handle,ToStart.Armor,USER(user)->Armor);
				if(USER(user)->RealEstate<ToStart.RealEstate)
					fprintf(fp,"          %.26s real estate to start: %u.  Left with %u.\n",USER(user)->Handle,ToStart.RealEstate,USER(user)->RealEstate);
				if(USER(user)->Security<ToStart.Security)
					fprintf(fp,"          %.26s security to start: %u.  Left with %u.\n",USER(user)->Handle,ToStart.Security,USER(user)->Security);
			}
			fprintf(fp,"%s  %.26s logged off level %u (%s).\n",Ftime(Time),USER(user)->Handle,USER(user)->Level,reason);
			fclose(fp);
			for(i=0; i<3; i++)
				if(strnicmp(RPGinfo->LastCaller[i],USER(user)->Handle,sizeof(USER(user)->Handle))==0) {
					strncpy(RPGinfo->Reason[i],reason,sizeof(RPGinfo->Reason[i]));
					break;
				}
			sprintf(text,"[#%u] %.26s logged off.",user,USER(user)->Handle);
			RPGmessage(text);
			RPGserver('D',user);
		}
		MenuFile("Logoff");
		sprintf(OUTbuf,"RPGBBS %s written by Robert Hurst",__AMIGADATE__);
		outs(OUTbuf);
		newline();
	}
	local=FALSE;
	logoff=TRUE;
}

char *Ltrim(char *str)
{
	int i;

	while(str[0]==' ')
		for(i=1; i<=strlen(str); i++)
			str[i-1]=str[i];
	return(str);
}

void MenuFile(char *name)
{
	FILE *fp;

	if(user) {
		if(ANSI)
			strmfe(filename,name,"ANS");
		else
			strmfe(filename,name,"TXT");
	}
	else
		strmfe(filename,name,"TXT");

	if(!(fp=fopen(filename,"rb")))
		return;
	while(!feof(fp)) {
		inputs(line[0],sizeof(line),fp);
		outs(line[0]);
		newline();
	}
	fclose(fp);
	normal();
}

void Midnight(void)
{
	int i;

	GetSystemDateTime();
	if(USER(0)->LastDate!=JulianDate) {
		USER(0)->LastDate=JulianDate;
		outs(Fcol(MAGENTA));
		outs("Midnight crossed");
		RPGserver('S',0);
		for(i=1; i<=MAXUSER; i++) {
			USER(i)->Status=0;
			USER(i)->CallsToday=0;
			PutUser(i);
			if(i%10==0)
				outs(".");
		}
		if(user) {
			outs("new call");
			online=Time;
			acclvl=USER(user)->Access;
			USER(user)->LastDate=JulianDate;
			USER(user)->LastTime=online;
			USER(user)->CallsToday++;
			USER(user)->TotalCalls++;
			USER(0)->TotalCalls++;
			PutUser(user);
			RestoreStat();
			Thull=USER(user)->Hull;
			brawl=0;
			fight=0;
			joust=0;
			naval=0;
			party=0;
		}
		newline();
		normal();
		PutUser(0);
		sprintf(filename,"News.%lu",JulianDate-2);
		remove(filename);
		if(JulianDate>=USER(0)->ExpireDate)
			SystemReroll();
	}
	if(user>0 && TimeLeft()<0) {
		beep();
		newline();
		sprintf(text,"%sYour %d minute time limit has expired!",Fcol(YELLOW),ACCESS(acclvl)->Minutes);
		outs(text);
		newline();
		strcpy(reason,"time expired");
		logoff=TRUE;
	}
}

char More(void)
{
	static char c;

	reverse();
	outs("-more-");
	normal();
	do {
		c=inkey();
	} while(c!='\15' && c!='N' && c!='Q' && c!='Y' && !cd());
	if(c=='\15')
		c='Y';
	if(c=='Q')
		c='N';
	sameline();
	cll();
	return(c);
}

char Option(struct RPGmenu *RPGmenu)
{
	static char c;
	int i;

	if(cd())
		return('Q');
	Midnight();
	if(cd())
		return('Q');
	RPGserver('U',0);
	DisplayUser();
	if(!USER(user)->Expert || c=='?') {
		newline();
		for(i=0; i<RPGmenu->items; i++) {
			sprintf(OUTbuf,"%s<%s%c%s>%s %s",Fcol(RPGmenu->color),Fcol(WHITE),RPGmenu->item[i].choice,Fcol(RPGmenu->color),Fcol(CYN),RPGmenu->item[i].name);
			outs(OUTbuf);
			newline();
		}
	}
	newline();
	sprintf(OUTbuf,"%s[%s%s%s]%s Option (Q=Quit): ",Fcol(RPGmenu->color),Fcol(YELLOW),RPGmenu->option,Fcol(RPGmenu->color),Fcol(CYN));
	outs(OUTbuf);
	do {
		c=inkey();
		if(c) {
			for(i=0; i<RPGmenu->items && c!=RPGmenu->item[i].choice; i++);
			if(i==RPGmenu->items && c!='?' && c!='@' && c!='Q') {
				beep();
				if(c!='\15')
					backspace();
				c='\0';
			}
			else {
				newline();
				newline();
			}
		}
	} while(!c && !cd());
	return(c);
}

void Pause(void)
{
	char c;

	reverse();
	outs("-pause-");
	normal();
	do {
		c=inkey();
	} while(!c && !cd());
	sameline();
	cll();
}

char *points(int n)
{
	static char r[32];

	sprintf(r,"%s(%s%+d%s)%s",Fcol(MAG),(n>0) ? Fcol(YELLOW) : Fcol(BRED),n,Fcol(MAG),Fcol(GRY));
	return(r);
}

void PutGang(int rec)
{
	RPGserver('G',rec);
}

void PutUser(int rec)
{
	RPGserver('P',rec);
}

char *RankLevel(int rec)
{
	static char result[16];
	int i;

	for(i=0; i<3; i++)
		if(RANK(i)->User==rec)
			break;
	switch(i) {
		case 0:
			sprintf(result,"%s*%2d*%s",Fcol(YELLOW),USER(rec)->Level,Fcol(GRY));
			break;
		case 1:
			sprintf(result,"%s=%2d=%s",Fcol(CYAN),USER(rec)->Level,Fcol(GRY));
			break;
		case 2:
			sprintf(result,"%s-%2d-%s",Fcol(BRED),USER(rec)->Level,Fcol(GRY));
			break;
		default:
			sprintf(result," %2d ",USER(rec)->Level);
			break;
	}
	return(result);
}

void ReadMail(void)
{
	FILE *fp;
	int i;
	char c;

	for(i=1; i<1000; i++) {
		sprintf(text,"Email%03d.%03d",user,i);
		strmfp(filename,EMAILpath,text);
		if(!(fp=fopen(filename,"r")))
			break;
		fclose(fp);
		cls();
		TypeMessage(filename);
		remove(filename);
		sprintf(text,"%sReply (Y/N)? ",Fcol(CYN));
		outs(text);
		if(!(c=inkey()))
			break;
		newline();
		newline();
		if(c=='Y')
			Email(sender);
	}
}

void RestoreStat(void)
{
	USER(user)->ExpLevel=USER(user)->Level;
	Thp=USER(user)->HP;
	Tsp=USER(user)->SP;
	Tstr=USER(user)->Str;
	Tint=USER(user)->Int;
	Tagl=USER(user)->Agl;
	Tchr=USER(user)->Chr;
	Thit=0;
	Tshield=0;
	if(USER(user)->Blessed) {
		Tstr+=10;
		if(Tstr>100)
			Tstr=100;
		Tint+=10;
		if(Tint>100)
			Tint=100;
		Tagl+=10;
		if(Tagl>100)
			Tagl=100;
		Tchr+=10;
		if(Tchr>100)
			Tchr=100;
		Thit=USER(user)->Weapon/10;
		Tshield=USER(user)->Armor/10;
	}
	if(USER(user)->Cursed) {
		Tstr-=10;
		Tint-=10;
		Tagl-=10;
		Tchr-=10;
		Thit-=USER(user)->Weapon/10;
		Tshield-=USER(user)->Armor/10;
	}
}

void RPGmessage(char *broadcast)
{
	server.command='M';
	server.parameter=user;
	server.result=broadcast;
	PutMsg(RPGport,(struct Message *)&server);
	WaitPort(RPGreply);
	response=(struct RPGserver *)GetMsg(RPGreply);
}

int RPGserver(char command,int rec)
{
	server.user=user;
	server.command=command;
	server.parameter=rec;
	server.result=NULL;
	PutMsg(RPGport,(struct Message *)&server);
	WaitPort(RPGreply);
	response=(struct RPGserver *)GetMsg(RPGreply);
	if(command=='U' && response->result) {
		outs(Fcol(CYAN));
		newline();
		outs(response->result);
		normal();
		newline();
		newline();
	}
	else
	if(command!='C' && response->result) {
		beep();
		newline();
		outs(response->result);
		newline();
		return(TRUE);
	}
	return(FALSE);
}

void Shuffle(void)
{
	struct card *swap;
	int i,j;

	outs("Shuffling new deck...");
	for(i=0; i<4; i++)
		for(j=0; j<13; j++)
			deck[13*i+j]=&card[j];
	for(i=0; i<52; i++) {
		swap=deck[i];
		j=dice(52)-1;
		deck[i]=deck[j];
		deck[j]=swap;
	}
	Delay(10);
	outs("Ok.");
	newline();
	newline();
}

int TimeLeft(void)
{
	int result;

	GetSystemDateTime();
	if(online>Time)
		online=Time;
	if(from=='@')
		result=2;
	else
		result=ACCESS(acclvl)->Minutes-(60*(Time/100-online/100)+(Time%100-online%100));
	return(result);
}

void TimeOut(void)
{
	newline();
	newline();
	outs(Fcol(YELLOW));
	outs("** 2-minute inactivity timeout **");
	normal();
	newline();
	newline();
	strcpy(reason,"inactive timeout");
	logoff=TRUE;
}

void ToUpLow(char *str)
{
	int i,f;

	strlwr(Ltrim(str));
	f=TRUE;
	for(i=0; i<strlen(str); i++) {
		if(f==TRUE && islower(str[i])) {
			str[i]=toupper(str[i]);
			f=FALSE;
		}
		if(str[i]<'A' && str[i]!='\'')
			f=TRUE;
	}
}

void TypeFile(char *name,int alert)
{
	FILE *fp;
	int lp;
	char more;

	if(!(fp=fopen(name,"rb"))) {
		if(alert) {
			sprintf(text,"File not found: %s",name);
			outs(text);
			newline();
		}
		return;
	}
	lp=0;
	more='Y';
	while(!feof(fp) && more!='N') {
		if(!local) {
			if(GetKey()=='\30') {
				normal();
				outs("<Abort>");
				newline();
				more='N';
				break;
			}
		}
		inputs(line[0],sizeof(line),fp);
		outs(line[0]);
		newline();
		lp++;
		if(lp%USER(user)->Rows==0)
			more=More();
	}
	fclose(fp);
	if((more!='N' && lp>22) || (!alert && lp>0))
		Pause();
}

void TypeMessage(char *name)
{
	FILE *fp;
	int lp,to;
	char more;

	if(!(fp=fopen(name,"rb"))) {
		sprintf(text,"File not found: %s",name);
		outs(text);
		newline();
		return;
	}
	inputs(INbuf,sizeof(INbuf),fp);
	sprintf(text,"%.4s",&INbuf[8]);
	Time=atoi(text);
	INbuf[8]='\0';
	sprintf(text,"%sDate:%s %s  %s",Fcol(CYN),Fcol(YELLOW),Fdate(INbuf),Ftime(Time));
	outs(text);
	newline();
	sprintf(text,"%.3s",&INbuf[12]);
	sender=atoi(text);
	sprintf(text,"%sFrom:%s %.26s",Fcol(CYN),Fcol(YELLOW),USER(sender)->Handle);
	outs(text);
	newline();
	sprintf(text,"%.3s",&INbuf[15]);
	to=atoi(text);
	sprintf(text,"%s  To:%s %.26s",Fcol(CYN),Fcol(YELLOW),(to) ? USER(to)->Handle : "All");
	outs(text);
	normal();
	newline();
	newline();
	lp=5;
	more='Y';
	while(!feof(fp) && more!='N') {
		inputs(line[0],sizeof(line),fp);
		outs(line[0]);
		newline();
		lp++;
		if(lp%USER(user)->Rows==0)
			more=More();
	}
	fclose(fp);
	if((more!='N' && lp>22))
		Pause();
}

void UserNote(int rec)
{
	FILE *fp;
	int h,i;
	char file[12];

	if(rec<1 || rec>MAXUSER || rec==user)
		return;
	h=ANSI;
	ANSI=USER(rec)->ANSI;
	sprintf(file,"Notes.%d",rec);
	strmfp(filename,NOTESpath,file);
	if((fp=fopen(filename,"a"))) {
		sprintf(text,"%s-=%s>%s*%s<%s=-%s\n\n",Fcol(BLU),Fcol(YELLOW),Fcol(RED),Fcol(YELLOW),Fcol(BLU),Fcol(GRY));
		fputs(text,fp);
		for(i=0; i<numline; i++) {
			fputs(line[i],fp);
			fputs("\n",fp);
		}
		fputs("\n",fp);
		fclose(fp);
	}
	ANSI=h;
}

double value(double n,int chr)
{
	double r;

	modf(n*chr/100.,&r);
	return(r);
}

/************************************\
 *  Task Initialization Procedures  *
\************************************/
int OpenGUI(void)
{
	struct Rectangle rect;
	ULONG screen_modeID;
	UWORD pens[] = { 0,2,5,7,4,2,0,(UWORD)~0 };
	int i;

	if((DiskfontBase=(struct Library *)OpenLibrary("diskfont.library",0L))==NULL)
		return(TRUE);
	if((TextFont[0]=(struct TextFont *)OpenFont(&font[0]))==NULL)
		return(TRUE);
	font[1].ta_Name=BBSprefs.Font;
	if((TextFont[1]=(struct TextFont *)OpenDiskFont(&font[1]))==NULL)
		return(TRUE);

	i=0;
	tags[i].ti_Tag=SA_Pens;
	tags[i++].ti_Data=(ULONG)pens;
/*	tags[i].ti_Tag=SA_Overscan;
	tags[i++].ti_Data=OSCAN_TEXT;
	tags[i].ti_Tag=SA_Width;
	tags[i++].ti_Data=STDSCREENWIDTH;
	tags[i].ti_Tag=SA_Height;
	tags[i++].ti_Data=STDSCREENHEIGHT;
*/	tags[i].ti_Tag=TAG_DONE;
	newscreen.Depth=BBSprefs.Depth;
	if((screen=(struct Screen *)OpenScreen((struct NewScreen *)&newscreen))==NULL)
		return(TRUE);
	newwindow.Screen=screen;
	newwindow.Flags|=BACKDROP | BORDERLESS;
	if(IntuitionBase->LibNode.lib_Version>=37)
		if((screen_modeID=GetVPModeID(&screen->ViewPort))!=INVALID_ID)
			if(QueryOverscan(screen_modeID,&rect,OSCAN_TEXT)) {
/*				newwindow.Width=rect.MaxX-rect.MinX+1;
				newwindow.Height=rect.MaxY-rect.MinY+1;
*/			}
	newuserwindow.Screen=screen;
	newnmewindow.Screen=screen;
	newiconwindow.Screen=screen;
	if((window=(struct Window *)OpenWindow(&newwindow))==NULL)
		return(TRUE);
	vp=(struct ViewPort *)ViewPortAddress(window);
	LoadRGB4(vp,&palette[BBSprefs.Depth-1][0],1<<BBSprefs.Depth);
	process=(struct Process *)FindTask(0);
	holdwindow=(struct Window *)process->pr_WindowPtr;
	process->pr_WindowPtr=(APTR)window;
	if(!(CPort=(struct MsgPort *)CreatePort("RPGBBS.console",0)))
		return(TRUE);
	if(!(ConsoleIO=(struct IOStdReq *)CreateExtIO(CPort,sizeof(struct IOStdReq))))
		return(TRUE);
	ConsoleIO->io_Data=(APTR)window;
	ConsoleIO->io_Length=sizeof(struct Window);
	if(OpenDevice("console.device",0,(struct IORequest *)ConsoleIO,0))
		return(TRUE);
	ConsoleDevice=(struct Library *)ConsoleIO->io_Device;
	SetFont(window->RPort,TextFont[1]);
	SetMenuStrip(window,&project);
	WriteConsole("\33c");
	return(FALSE);
}

void CloseGUI(void)
{
	int i;

	if(userwindow)
		CloseWindow(userwindow);
	userwindow=NULL;
	if(nmewindow)
		CloseWindow(nmewindow);
	nmewindow=NULL;
	if(ConsoleIO)
		CloseDevice((struct IORequest *)ConsoleIO);
	ConsoleIO=NULL;
	if(CPort)
		DeletePort(CPort);
	CPort=NULL;
	if(window) {
		ClearMenuStrip(window);
		CloseWindow(window);
		process->pr_WindowPtr=(APTR)holdwindow;
	}
	window=NULL;
	if(screen)
		CloseScreen(screen);
	screen=NULL;
	for(i=0; i<3; i++) {
		if(TextFont[i])
			CloseFont(TextFont[i]);
		TextFont[i]=NULL;
	}
	if(DiskfontBase)
		CloseLibrary(DiskfontBase);
	DiskfontBase=NULL;
}

int Init(void)
{
	FILE *fp;
	unsigned int seed;
	int i;

	if(!(DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0L)))
		return(TRUE);
	if(!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0L)))
		return(TRUE);
	if(!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0L)))
		return(TRUE);
	if(!(NoisyBase=OpenLibrary("noisy.library",0)))
		return(TRUE);

	if(OpenGUI())
		return(TRUE);
	WriteConsole("Intuition interface established.\n\n");
	local=TRUE;

	ChatKey=LoadSound("Chat.snd",0);
	if(ChatKey)
		SetVolume(ChatKey,32);

	WriteConsole("Loading config file...");
	if(!(fp=fopen("RPGBBS.config","r")))
		return(TRUE);
	inputs(EMAILpath,sizeof(EMAILpath),fp);
	inputs(HELPpath,sizeof(HELPpath),fp);
	inputs(MSGpath,sizeof(MSGpath),fp);
	inputs(NOTESpath,sizeof(NOTESpath),fp);
	inputs(ArenaPIXpath,sizeof(ArenaPIXpath),fp);
	inputs(DungeonPIXpath,sizeof(DungeonPIXpath),fp);
	inputs(PlayerPIXpath,sizeof(PlayerPIXpath),fp);
	inputs(UPLOADpath,sizeof(UPLOADpath),fp);
	fgets(text,sizeof(text),fp);
	MaxFILE=atoi(text);
	for(i=0; i<MaxFILE; i++) {
		inputs(FILEname[i],sizeof(FILEname[i]),fp);
		inputs(FILEpath[i],sizeof(FILEpath[i]),fp);
	}
	fgets(text,sizeof(text),fp);
	MaxMSG=atoi(text);
	for(i=0; i<MaxMSG; i++) {
		inputs(MSGname[i],sizeof(MSGname[i]),fp);
		fgets(text,sizeof(text),fp);
		KeepMSG[i]=atoi(text);
	}
	fclose(fp);
	WriteConsole("Ok.\n");

	if(strlen(BBSprefs.Device)) {
		i=strlen(BBSprefs.Init);
		BBSprefs.Init[i++]='\15';
		BBSprefs.Init[i]='\0';
		i=strlen(BBSprefs.Quit);
		BBSprefs.Quit[i++]='\15';
		BBSprefs.Quit[i]='\0';
	}

	WriteConsole("Establishing link with RPGserver...");
	if(!(RPGreply=(struct MsgPort *)CreatePort(NULL,0)))
		return(TRUE);
	server.message.mn_Node.ln_Type=NT_MESSAGE;
	server.message.mn_Length=sizeof(struct RPGserver);
	server.message.mn_ReplyPort=RPGreply;
	Forbid();
	RPGport=(struct MsgPort *)FindPort("RPGserver");
	Permit();
	if(!RPGport)
		return(TRUE);
	user=0;
	RPGserver('C',0);
	JumpTable=(struct JumpTable *)response->result;
	if(!JumpTable)
		return(TRUE);
	Access=JumpTable->Access;
	Class=JumpTable->Class;
	Armor=JumpTable->Armor;
	Weapon=JumpTable->Weapon;
	RealEstate=JumpTable->RealEstate;
	Security=JumpTable->Security;
	Magic=JumpTable->Magic;
	Poison=JumpTable->Poison;
	AM=JumpTable->AM;
	DM=JumpTable->DM;
	SM=JumpTable->SM;
	User=JumpTable->User;
	Gang=JumpTable->Gang;
	HOF=JumpTable->HOF;
	RPGinfo=JumpTable->RPGinfo;
	Rank=JumpTable->Rank;
	WriteConsole("Ok.\n");

	GetSystemDateTime();
	seed=10000*atoi(&SystemTime[6])+100*atoi(&SystemTime[3])+atoi(SystemTime);
	seed+=atol(SystemDate);
	srand(seed);
	for(i=0; i<USER(0)->TotalCalls%1000; i++)
		dice(i);

	return(FALSE);
}

void Shutdown(void)
{
	CloseGUI();
	if(RPGport) {
		user=0;
		RPGserver('S',0);
		RPGserver('D',0);
	}
	if(RPGreply)
		DeletePort(RPGreply);
	if(ChatKey)
		DeleteSound(ChatKey);
	if(NoisyBase)
		CloseLibrary(NoisyBase);
	if(IconBase)
		CloseLibrary(IconBase);
	if(GfxBase)
		CloseLibrary((struct Library *)GfxBase);
	if(IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);
	if(DOSBase)
		CloseLibrary((struct Library *)DOSBase);
	exit(0);
}

void main(int argc,char **argv)
{
	struct DiskObject *diskobj;
	struct WBStartup *WBenchMsg;
	struct WBArg *wbarg;
	long startdir=-1;
	int i,result;
	char *s,**tool;

	if(argc)	/*  Shell  */
		s=argv[argc-1];
	else {		/*  Workbench  */
		WBenchMsg=(struct WBStartup *)argv;
		for(i=0,wbarg=WBenchMsg->sm_ArgList; i<WBenchMsg->sm_NumArgs; i++,wbarg++) {
			if(wbarg->wa_Lock && *wbarg->wa_Name)
				startdir=CurrentDir(wbarg->wa_Lock);
			s=wbarg->wa_Name;
		}
	}
	if((IconBase=OpenLibrary("icon.library",0))) {
		if((diskobj=GetDiskObject(s))) {
			tool=(char **)diskobj->do_ToolTypes;
			if((s=(char *)FindToolType(tool,"DEVICE"))) {
				if(stricmp(s,"LOCAL"))
					strcpy(BBSprefs.Device,s);
				else
					memset(BBSprefs.Device,0,sizeof(BBSprefs.Device));
			}
			if((s=(char *)FindToolType(tool,"UNIT")))
				BBSprefs.Unit=atoi(s);
			if((s=(char *)FindToolType(tool,"BAUD")))
				BBSprefs.Baud=atol(s);
			if((s=(char *)FindToolType(tool,"INIT")))
				strcpy(BBSprefs.Init,s);
			if((s=(char *)FindToolType(tool,"QUIT")))
				strcpy(BBSprefs.Quit,s);
			if((s=(char *)FindToolType(tool,"DELAY")))
				BBSprefs.Delay=atoi(s);
			if((s=(char *)FindToolType(tool,"AUTO")))
				BBSprefs.Auto=atoi(s);
			if((s=(char *)FindToolType(tool,"KEEP")))
				BBSprefs.Keep=atoi(s);
			if((s=(char *)FindToolType(tool,"DEPTH")))
				BBSprefs.Depth=atoi(s);
			if((s=(char *)FindToolType(tool,"FONT")))
				strcpy(BBSprefs.Font,s);
			if((s=(char *)FindToolType(tool,"ICONX")))
				BBSprefs.IconX=atoi(s);
			if((s=(char *)FindToolType(tool,"ICONY")))
				BBSprefs.IconY=atoi(s);
			FreeDiskObject(diskobj);
		}
	}
	else
		exit(1);

	if(Init()==FALSE) {
		do {
			result=Logon();
			if(user)
				RPGserver('D',user);
			if(SerialIO) {
				if(!result) {
					WriteConsole("Shutting down...\n");
					SerialIO->IOSer.io_Command=CMD_WRITE;
					SerialIO->IOSer.io_Length=-1;
					SerialIO->IOSer.io_Data=(APTR)BBSprefs.Quit;
					DoIO((struct IORequest *)SerialIO);
					Delay(100);
					SerialIO->IOSer.io_Command=SDCMD_QUERY;
					DoIO((struct IORequest *)SerialIO);
					if(SerialIO->IOSer.io_Actual) {
						SerialIO->IOSer.io_Command=CMD_READ;
						SerialIO->IOSer.io_Length=SerialIO->IOSer.io_Actual;
						SerialIO->IOSer.io_Data=(APTR)INbuf;
						DoIO((struct IORequest *)SerialIO);
					}
				}
				CloseDevice((struct IORequest *)SerialIO);
				DeleteExtIO((struct IORequest *)SerialIO);
				SerialIO=NULL;
				Delay(125);
			}
			if(SPort)
				DeletePort(SPort);
		} while(result);
	}
	Shutdown();
	if(startdir!=-1)
		CurrentDir(startdir);
}
