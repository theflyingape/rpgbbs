/******************\
 *  RPGBBS Sysop  *
\******************/
#include "RPGBBS.h"

void DeleteUser(int rec)
{
	int i,j,g;

	g=USER(rec)->Gang;
	if(g)
		for(i=0; i<4; i++)
			if(GANG(g)->Member[i]==rec) {
				GANG(g)->Member[i]=0;
				if(i==0) {
					for(j=1; j<4; j++)
						if(GANG(g)->Member[j]>0) {
							USER(GANG(g)->Member[j])->Gang=0;
							PutUser(GANG(g)->Member[j]);
						}
					memset(GANG(g),0,sizeof(struct Gang));
				}
				PutGang(g);
			}
	memset(USER(rec),0,sizeof(struct User));
	PutUser(rec);
	sprintf(OUTbuf,"Notes.%d",rec);
	strmfp(filename,NOTESpath,OUTbuf);
	remove(filename);
	for(i=1; i<1000; i++) {
		sprintf(OUTbuf,"Email%03d.%03d",rec,i);
		strmfp(filename,EMAILpath,OUTbuf);
		if(remove(filename))
			break;
	}
}

void EditUser(int rec)
{
	int i;
	char c;

	FOREVER {
		plot(1,1);
		sprintf(OUTbuf,"<1> Real Name...: %.26s (%s) %s",USER(rec)->Name,USER(rec)->Sex=='M' ? "male" : USER(rec)->Sex=='F' ? "female" : "n/a",jdate(USER(rec)->BirthDate));
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"<2> Handle Name.: %.26s",USER(rec)->Handle);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"<3> Address #1..: %s",USER(rec)->Street[0]);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"<4> Address #2..: %s",USER(rec)->Street[1]);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(line[0],"%010f",USER(rec)->VoicePhone);
		sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
		sprintf(OUTbuf,"<5> Voice Phone.: %s",line[1]);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(line[0],"%010f",USER(rec)->DataPhone);
		sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
		sprintf(OUTbuf,"<6> Data Phone..: %s",line[1]);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"<7> Password....: xxxxx");
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"<8> Access Level: %d - %s",USER(rec)->Access,ACCESS(USER(rec)->Access)->Name);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"<9> File Ratio?.: %c  %u:%u (%lu:%lu)",USER(rec)->Ratio,USER(rec)->ULfiles,USER(rec)->DLfiles,USER(rec)->ULbytes,USER(rec)->DLbytes);
		outs(OUTbuf);
		cll();
		newline();
		newline();
		sprintf(OUTbuf,"Total calls: %d    Last call on: %s  %s at %u baud",USER(rec)->TotalCalls,jdate(USER(rec)->LastDate),Ftime(USER(rec)->LastTime),USER(rec)->LastBaud);
		outs(OUTbuf);
		newline();
		newline();
		sprintf(OUTbuf,"Class: %d - %s (%d)    Level: %d (%d)",USER(rec)->Class,CLASS(USER(rec)->Class)->Name,USER(rec)->Immortal,USER(rec)->Level,USER(rec)->KeySequence);
		outs(OUTbuf);
		newline();
		newline();
		outs(Fcol(CYAN));
		outs("Change which field (1-9), <D>elete, <S>ave: ");
		echo=TRUE;
		if(input())
			break;
		c=toupper(ins[0]);
		i=atoi(ins);
		if(c=='D') {
			DeleteUser(rec);
			break;
		}
		if(c=='S') {
			PutUser(rec);
			break;
		}
		switch(i) {
			case 1:
				plot(i,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(rec)->Name))
					strncpy(USER(rec)->Name,ins,sizeof(USER(rec)->Name));
				break;
			case 2:
				plot(i,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(rec)->Handle))
					strncpy(USER(rec)->Handle,ins,sizeof(USER(rec)->Handle));
				break;
			case 3:
				plot(i,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(rec)->Street[0]))
					strncpy(USER(rec)->Street[0],ins,sizeof(USER(rec)->Street[0]));
				break;
			case 4:
				plot(i,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(rec)->Street[1]))
					strncpy(USER(rec)->Street[1],ins,sizeof(USER(rec)->Street[1]));
				break;
			case 5:
				plot(i,19);
				cll();
				if(input())
					break;
				if(strlen(ins)!=12 || ins[3]!='-' || ins[7]!='-')
					break;
				USER(rec)->VoicePhone=10000000.*atof(&ins[0])+10000.*atof(&ins[4])+atof(&ins[8]);
				break;
			case 6:
				plot(i,19);
				cll();
				if(input())
					break;
				if(strlen(ins)!=12 || ins[3]!='-' || ins[7]!='-')
					break;
				USER(rec)->DataPhone=10000000.*atof(&ins[0])+10000.*atof(&ins[4])+atof(&ins[8]);
				break;
			case 7:
				plot(i,19);
				cll();
				echo=FALSE;
				if(input())
					break;
				strupr(ins);
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(rec)->Password))
					strncpy(USER(rec)->Password,ins,sizeof(USER(rec)->Password));
				break;
			case 8:
				plot(i,19);
				cll();
				if(input())
					break;
				i=atoi(ins);
				if(i>0 && i<MAXACCESS)
					USER(rec)->Access=i;
				break;
			case 9:
				plot(i,19);
				cll();
				c=inkey();
				if(c=='Y' || c=='N')
					USER(rec)->Ratio=c;
				sprintf(OUTbuf,"%c  ",USER(rec)->Ratio);
				outs(OUTbuf);
				if(input())
					break;
				if(strlen(ins))
					USER(rec)->ULfiles=atoi(ins);
				else {
					sprintf(OUTbuf,"%u",USER(rec)->ULfiles);
					outs(OUTbuf);
				}
				outs(":");
				if(input())
					break;
				if(strlen(ins))
					USER(rec)->DLfiles=atoi(ins);
				else {
					sprintf(OUTbuf,"%u",USER(rec)->DLfiles);
					outs(OUTbuf);
				}
				break;
		}
	}
}

void EditSystem(void)
{
	int i;
	char c;

	FOREVER {
		plot(1,1);
		sprintf(OUTbuf,"%s     Server date:%s %s",Fcol(CYAN),Fcol(WHITE),jdate(USER(0)->BirthDate));
		outs(OUTbuf);
		newline();
		sprintf(OUTbuf,"%s%s BBS Name...:%s %.30s",bracket(1),Fcol(CYAN),Fcol(WHITE),USER(0)->Name);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Licensed To:%s %.30s",bracket(2),Fcol(CYAN),Fcol(WHITE),USER(0)->Street[0]);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Licensed Of:%s %.30s",bracket(3),Fcol(CYAN),Fcol(WHITE),USER(0)->Street[1]);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Server Lic#:%s %-5u   %sOnline:%s %-2u",bracket(4),Fcol(CYAN),Fcol(WHITE),USER(0)->LastTime,Fcol(CYAN),Fcol(WHITE),MAXLINE);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Client Lic#:%s %-5u   %sLevels:%s %-2u",bracket(5),Fcol(CYAN),Fcol(WHITE),USER(0)->LastBaud,Fcol(CYAN),Fcol(WHITE),MAXLEVEL);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Taxman Name:%s %.26s   %sAccrual:%s %s",bracket(6),Fcol(CYAN),Fcol(WHITE),USER(0)->Handle,Fcol(CYAN),Fcol(WHITE),money(USER(0)->Bank,1));
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Post/Call \%:%s %g",bracket(7),Fcol(CYAN),Fcol(WHITE),USER(0)->Experience);
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s Reroll date:%s %s %s",bracket(8),Fcol(CYAN),Fcol(WHITE),jdate(USER(0)->ExpireDate),USER(0)->ExpireDate<JulianDate ? "<-- Is this correct?" : "ok");
		outs(OUTbuf);
		cll();
		newline();
		sprintf(OUTbuf,"%s%s File ratios:%s %c  %u:%u",bracket(9),Fcol(CYAN),Fcol(WHITE),USER(0)->Ratio,USER(0)->ULfiles,USER(0)->DLfiles);
		outs(OUTbuf);
		cll();
		newline();
		newline();
		outs(Fcol(CYAN));
		outs("Change which field (1-9), <S>ave: ");
		echo=TRUE;
		if(input())
			break;
		c=toupper(ins[0]);
		i=atoi(ins);
		if(c=='S') {
			PutUser(0);
			break;
		}
		switch(i) {
			case 1:
				plot(i+1,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(0)->Name))
					strncpy(USER(0)->Name,ins,sizeof(USER(0)->Name));
				break;
			case 2:
				plot(i+1,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(0)->Street[0]))
					strncpy(USER(0)->Street[0],ins,sizeof(USER(0)->Street[0]));
				break;
			case 3:
				plot(i+1,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(0)->Street[1]))
					strncpy(USER(0)->Street[1],ins,sizeof(USER(0)->Street[1]));
				break;
			case 4:
				plot(i+1,19);
				cll();
				if(input())
					break;
				USER(0)->LastTime=(UWORD)atol(ins);
				plot(i+1,27);
				sprintf(OUTbuf,"%sOnline: ",Fcol(CYAN));
				outs(OUTbuf);
				if(input())
					break;
				MAXLINE=atoi(ins);
				break;
			case 5:
				plot(i+1,19);
				cll();
				if(input())
					break;
				USER(0)->LastBaud=(UWORD)atol(ins);
				plot(i+1,27);
				sprintf(OUTbuf,"%sLevels: ",Fcol(CYAN));
				outs(OUTbuf);
				if(input())
					break;
				MAXLEVEL=atoi(ins);
				break;
			case 6:
				plot(i+1,19);
				cll();
				if(input())
					break;
				if(strlen(ins)>0 && strlen(ins)<=sizeof(USER(0)->Handle))
					strncpy(USER(0)->Handle,ins,sizeof(USER(0)->Handle));
				break;
			case 7:
				plot(i+1,19);
				cll();
				if(input())
					break;
				USER(0)->Experience=atof(ins);
				break;
			case 8:
				plot(i+1,19);
				outs("mm/dd/yyyy");
				cll();
				plot(i+1,19);
				if(input())
					break;
				USER(0)->ExpireDate=julian(ins);
				break;
			case 9:
				plot(i+1,19);
				cll();
				c=inkey();
				if(c=='Y' || c=='N')
					USER(0)->Ratio=c;
				sprintf(OUTbuf,"%c  ",USER(0)->Ratio);
				outs(OUTbuf);
				if(input())
					break;
				USER(0)->ULfiles=atoi(ins);
				outs(":");
				if(input())
					break;
				USER(0)->DLfiles=atoi(ins);
				break;
		}
	}
}

void SystemReroll(void)
{
	int active,i,purged,rec;

	USER(0)->ExpireDate+=61;
	USER(0)->Gold=0.;
	USER(0)->Bank=0.;
	USER(0)->Loan=0.;
	USER(0)->Status=0;
	USER(0)->BountyWho=1;
	PutUser(0);

	newline();
	active=0;
	purged=0;
	for(rec=1; rec<=MAXUSER; rec++) {
		if(USER(rec)->Access) {
			if(JulianDate-USER(rec)->LastDate>180) {
				sprintf(OUTbuf,"%s Deleting%s %-26.26s %4d days",Fcol(RED),Fcol(WHITE),USER(rec)->Handle,JulianDate-USER(rec)->LastDate);
				outs(OUTbuf);
				Delay(5);
				newline();
				DeleteUser(rec);
				purged++;
				continue;
			}
			sprintf(OUTbuf,"%sRerolling%s %-26.26s %4d days",Fcol(YELLOW),Fcol(WHITE),USER(rec)->Handle,JulianDate-USER(rec)->LastDate);
			outs(OUTbuf);
			Delay(5);
			newline();
			active++;
		}
		USER(rec)->Blessed=0;
		USER(rec)->Cursed=0;
		USER(rec)->KeySequence=dice(6);
		RestartChar(rec);	/*  the PutUser() is in here  */
		sprintf(OUTbuf,"Notes.%d",rec);
		strmfp(filename,NOTESpath,OUTbuf);
		remove(filename);
	}
	sprintf(OUTbuf,"Ok.  %d active users.  %d deleted users.",active,purged);
	outs(OUTbuf);
	newline();
	for(i=0; i<MAXUSER; i++) {
		rec=dice(MAXUSER);
		if(USER(rec)->Access) {
			USER(rec)->Blessed=1;
			PutUser(rec);
			break;
		}
	}
	for(i=0; i<MAXUSER; i++) {
		rec=dice(MAXUSER);
		if(USER(rec)->Access) {
			USER(rec)->Cursed=1;
			PutUser(rec);
			break;
		}
	}
	outs("Reroll complete.");
	newline();
	Delay(100);
}

void Sysop(void)
{
	static struct RPGmenu __far RPGmenu = { "Sysop",BLU,13,
							{	{ 'A',"Announcements" },
								{ 'B',"Bless a user" },
								{ 'C',"Curse a user" },
								{ 'E',"E-mail" },
								{ 'F',"File Section" },
								{ 'L',"List users report" },
								{ 'M',"Read your mail" },
								{ 'N',"New users report" },
								{ 'S',"Swap user record" },
								{ 'U',"User file editor" },
								{ 'V',"View file log" },
								{ 'X',"Exterminate a user" },
								{ 'Y',"You see user stats" }
							}
	};
	FILE *fp;
	int i,lp,rec,new,m1,m2;
	char c,more;

	from='@';	/* flag to allow sysop to access any user at this time */
	GetSystemDateTime();
	online=Time;

	while(!cd()) {
		WriteConsole("\33c");
		cls();
		if((c=Option(&RPGmenu))=='Q')
			break;
		switch(c) {
			case 'A':
				if(Editor(20)=='S') {
					outs("aving...");
					GetSystemDateTime();
					if((fp=fopen("Announcements","w"))) {
						fprintf(fp,"Date: %s  %s\n",Fdate(SystemDate),Ftime(Time));
						fprintf(fp,"From: %.26s\n\n",USER(user)->Handle);
						for(i=0; i<numline; i++)
							fprintf(fp,"%s\n",line[i]);
						fclose(fp);
						outs("Ok.");
					}
					else
						outs("Error!");
				}
				break;
			case 'B':
				for(rec=1; rec<MAXUSER; rec++) {
					if(USER(rec)->Blessed) {
						sprintf(OUTbuf,"#%d - %.26s is blessed.",rec,USER(rec)->Handle);
						outs(OUTbuf);
						newline();
					}
				}
				newline();
				outs("Bless user or <R>andom? ");
				rec=GetUser();
				if(!stricmp(ins,"R"))
					BlessAssign();
				if(rec>0) {
					if(USER(rec)->Blessed)
						USER(rec)->Blessed=0;
					else
					if(!USER(rec)->Cursed)
						USER(rec)->Blessed=1;
					PutUser(rec);
				}
				break;
			case 'C':
				for(rec=1; rec<MAXUSER; rec++) {
					if(USER(rec)->Cursed) {
						sprintf(OUTbuf,"#%d - %.26s is cursed.",rec,USER(rec)->Handle);
						outs(OUTbuf);
						newline();
					}
				}
				newline();
				outs("Curse user or <R>andom? ");
				rec=GetUser();
				if(!stricmp(ins,"R"))
					CurseAssign();
				if(rec>0) {
					if(USER(rec)->Cursed)
						USER(rec)->Cursed=0;
					else
					if(!USER(rec)->Blessed)
						USER(rec)->Cursed=1;
					PutUser(rec);
				}
				break;
			case 'E':
				outs("Send mail to whom (<S>ysop,?=List)? ");
				i=GetUser();
				if(i==0)
					break;
				if(i<0)
					i=1;
				newline();
				Email(i);
				break;
			case 'F':
				FileSection();
				break;
			case 'L':
				outs(Fcol(CYAN));
				outs("Output file to [PRT:]: ");
				if(input())
					break;
				if(!strlen(ins))
					strcpy(ins,"PRT:");
				lp=66;
				if((fp=fopen(ins,"w"))) {
					GetSystemDateTime();
					for(rec=1; rec<MAXUSER; rec++) {
						if(lp>58) {
							if(lp<66)
								fprintf(fp,"\14");
							fprintf(fp,"\n\n");
							fprintf(fp,"%s User Listing as of %s\n\n",USER(0)->Name,Fdate(SystemDate));
							fprintf(fp,"Num           Username               City, State, Zip      Phone Number   Access \n");
							fprintf(fp,"---  --------------------------  ------------------------  ------------  --------\n");
							lp=6;
						}
						if(USER(rec)->Access) {
							sprintf(line[0],"%010f",USER(rec)->VoicePhone);
							sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
							fprintf(fp,"%3d  %-26.26s  %-24.24s  %s  %s\n",rec,USER(rec)->Name,USER(rec)->Street[1],line[1],ACCESS(USER(rec)->Access)->Name);
							fprintf(fp,"     %26.26s  %-5.5s  (%c)\n",USER(rec)->Handle,USER(rec)->Password,USER(rec)->Sex);
							lp+=2;
						}
					}
					fclose(fp);
				}
				break;
			case 'M':
				ReadMail();
				break;
			case 'N':
				outs("Num           Username             Class    Lvl  Status  Last Call On   Access ");
				newline();
				outs("---  --------------------------  ---------   --  ------  ------------  --------");
				newline();
				lp=2;
				more='Y';
				for(rec=1; rec<MAXUSER && more!='N'; rec++) {
					if((USER(rec)->Access>0 && USER(rec)->Access<4) || (USER(rec)->Access>0 && USER(rec)->Class==0)) {
						sprintf(OUTbuf,"%3d  %-26.26s  %-9s  %3d  %s  %-12s  %s",rec,USER(rec)->Handle,CLASS(USER(rec)->Class)->Name,USER(rec)->Level,USER(rec)->Status ? " Dead " : "Alive!",jdate(USER(rec)->LastDate),ACCESS(USER(rec)->Access)->Name);
						outs(OUTbuf);
						newline();
						lp++;
						if(lp%22==0)
							more=More();
					}
				}
				Pause();
				break;
			case 'S':
				outs(Fcol(CYAN));
				outs("Swap which user (?=List)? ");
				rec=GetUser();
				if(rec<1 || USER(rec)->Access==0)
					break;
				newline();
				sprintf(OUTbuf,"%sWith which usernumber (1-%d)? ",Fcol(CYAN),MAXUSER-1);
				outs(OUTbuf);
				if(input())
					break;
				newline();
				new=atoi(ins);
				if(new<1 || new>=MAXUSER || new==rec)
					break;
				swmem(USER(rec),USER(new),sizeof(struct User));
				swmem(&rec,&new,sizeof(rec));
				PutUser(rec);
				PutUser(new);
				if(USER(rec)->Gang)
					for(i=0; i<4; i++)
						if(GANG(USER(rec)->Gang)->Member[i]==rec)
							m1=i;
				if(USER(new)->Gang)
					for(i=0; i<4; i++)
						if(GANG(USER(new)->Gang)->Member[i]==new)
							m2=i;
				c=GANG(USER(rec)->Gang)->Member[m1];
				GANG(USER(rec)->Gang)->Member[m1]=GANG(USER(new)->Gang)->Member[m2];
				GANG(USER(new)->Gang)->Member[m2]=c;
				if(USER(rec)->Gang)
					PutGang(USER(rec)->Gang);
				if(USER(new)->Gang)
					PutGang(USER(new)->Gang);
				outs("Done.");
				Delay(100);
				break;
			case 'U':
				outs("Edit which user (?=List)? ");
				rec=GetUser();
				if(rec==0)
					break;
				cls();
				if(rec>0)
					EditUser(rec);
				else
					EditSystem();
				break;
			case 'V':
				TypeFile("file.log",TRUE);
				outs(Fcol(CYAN));
				outs("Delete log (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				if(c=='Y')
					remove("file.log");
				break;
			case 'X':
				outs("Reroll which user (?=List)? ");
				rec=GetUser();
				if(rec<1)
					break;
				outs("Are you sure (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				if(c=='Y')
					RestartChar(rec);
				break;
			case 'Y':
				outs("Which user statistics (?=List)? ");
				rec=GetUser();
				if(rec>0 && rec<=MAXUSER) {
					CharacterStats(rec);
					Pause();
				}
				break;
		}
	}
}
