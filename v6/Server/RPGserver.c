/*****************************\
 *    RPGBBS File Server     *
 *                           *
 *  Written by Robert Hurst  *
\*****************************/
#include <proto/exec.h>
#include <proto/intuition.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ISLEAP (((year%4)==0) && (((year%100)!=0) || ((year%400)==0)))

#define ACCESS(n)	(Access+n)
#define	ARENA(n)	(AM+n)
#define ARMOR(n)	(Armor+n)
#define CLASS(n)	(Class+n)
#define DUNGEON(n)	(DM+n)
#define	ESTATE(n)	(RealEstate+n)
#define FAME(i,j)	(HOF+(6*i+j))
#define GANG(n)		(Gang+n)
#define MAGIC(n)	(Magic+n)
#define	POISON(n)	(Poison+n)
#define RANK(n)		(Rank+n)
#define SEA(n)		(SM+n)
#define SECURITY(n)	(Security+n)
#define USER(n)		(User+n)
#define	WEAPON(n)	(Weapon+n)

#define MAXACCESS		USER(0)->Access
#define	MAXAM			USER(0)->Hull
#define MAXARMOR		USER(0)->Armor
#define MAXCLASS		USER(0)->Class
#define MAXDM			USER(0)->Cannon
#define MAXGANG			50
#define MAXLEVEL 		USER(0)->Level
#define MAXLINE 		USER(0)->ExpLevel
#define MAXMAGIC		USER(0)->Spell
#define	MAXPOISON		USER(0)->Poison
#define	MAXREALESTATE	USER(0)->RealEstate
#define MAXSECURITY		USER(0)->Security
#define MAXSM			USER(0)->Ram
#define MAXUSER			200
#define	MAXWEAPON		USER(0)->Weapon

char __near *__procname="RPGserver";
long __near __stack=4096;
long __near __priority=1;
long __near BackGroundIO=0;

struct	IntuitionBase	*IntuitionBase;
struct	MsgPort			*port;
struct	tm				*tp;

struct IntuiText Really = {
	0,1,JAM2,10,10,NULL,"Really shutdown server?",NULL
};
struct IntuiText Warning = {
	0,1,JAM2,10,10,NULL,"Client(s) still active.",NULL
};
struct IntuiText Yes = {
	2,3,JAM2,5,3,NULL,"Shutdown",NULL
};
struct IntuiText No = {
	2,3,JAM2,5,3,NULL,"Cancel",NULL
};

struct Access {
	UBYTE Name[10];
	UBYTE Calls,Minutes,Level;
	UBYTE User,Bulletin,Dungeon,File,Naval,Party,Tavern,Sysop;
} *Access;

struct Class {
	UBYTE Name[10];
	BYTE Poison,Vial;						/*  -1=service,0=none,1=1x,2=2x  */
	BYTE Magic,ToCast,ToInt,ToHeal,ToBlast;	/*  0=none,1=wand,2=scroll,3=spell,4=super  */
	BYTE Backstab;							/*  0=none,2=2x,3=3x,etc.  */
	BYTE ToHit,DefInt,DefAgl;
	UBYTE Recommend[4];
	UBYTE Str,Int,Agl,Chr;
	UBYTE WC,AC,ToSteal;
	UBYTE Note[22];
} *Class;

struct Store {
	char Name[26];
	double Cost;
} *Armor,*Weapon,*RealEstate,*Security;

struct Extra {
	char Name[20];
	double Cost[2];
	UWORD Power[2];
} *Magic,*Poison;

struct AM {
	char Name[17];
	UBYTE Class,Level;
	UBYTE Str,Int,Agl,Chr;
	UWORD HP,SP;
	BYTE Weapon,Armor;
	UWORD Spell;
	DOUBLE Gold;
} *AM;

struct DM {
	char Name[17];
	UBYTE Class;
} *DM;

struct SM {
	char Name[17];
	UBYTE Int;
	UWORD Hull,Shot,Power;
	char Ram;
	double Gold;
} *SM;

struct User {
	UBYTE Handle[26];
	UBYTE Name[30];
	UBYTE Street[2][30];
	DOUBLE VoicePhone,DataPhone;
	UBYTE Password[5];
	UBYTE Sex;
	ULONG BirthDate;
	UBYTE Access,ANSI,Expert,Format;
	ULONG ExpireDate,LastDate;
	UWORD LastTime,LastBaud;
	UWORD TotalCalls,TotalPosts;
	UBYTE CallsToday;
	UBYTE Rows;
	DOUBLE Experience,Gold,Bank,Loan;
	UWORD Immortal,HP,SP;
	UBYTE Class,Level,ExpLevel,Str,Int,Agl,Chr,Blessed,Cursed;
	BYTE Weapon,Armor;
	UBYTE RealEstate,Security;
	UWORD Spell,Poison;
	UWORD Hull,Cannon;
	UBYTE Ram;
	UBYTE Gang;
	UWORD Calls,YouKill,WasKill,YouKO,WasKO,JoustWin,JoustLoss;
	UBYTE KeySequence;
	UWORD LastMessage[16];
	DOUBLE BountyAmount;
	UBYTE BountyWho;
	UBYTE Ratio;
	UWORD ULfiles,DLfiles;
	ULONG ULbytes,DLbytes;
	LONG LastFileScan;
	UWORD Status;
} *User;

struct Gang {
	UBYTE Name[20];
	WORD Member[4];
	UWORD Win,Loss;
} *Gang;

struct HOF {
	UWORD ValueLeast,ValueMost;
	UBYTE WhoLeast[26],WhoMost[26];
	ULONG DateLeast,DateMost;
} *HOF;

struct RPGinfo {
	UBYTE LastCaller[3][26];
	UBYTE Reason[3][80];
	UBYTE Border[80];
	UWORD FirstMSG[16],NextMSG[16];
} *RPGinfo;

struct Rank {
	UBYTE User;
	UWORD Value;
} *Rank;

struct JumpTable {
	struct Access *Access;
	struct Class *Class;
	struct Store *Armor;
	struct Store *Weapon;
	struct Store *RealEstate;
	struct Store *Security;
	struct Extra *Magic;
	struct Extra *Poison;
	struct AM *AM;
	struct DM *DM;
	struct SM *SM;
	struct User *User;
	struct Gang *Gang;
	struct HOF *HOF;
	struct RPGinfo *RPGinfo;
	struct Rank *Rank;
} JumpTable;

struct RPGserver {
	struct Message message;
	UBYTE user;
	UBYTE command;
	UBYTE parameter;
	UBYTE *result;
} *server,quit;

struct LockTable {
	UBYTE user;
	UBYTE entry[4];
	UBYTE *pending,*hold;
} LockTable[16];

UBYTE MaxAccess,MaxClass;
UBYTE MaxArmor,MaxWeapon,MaxRealEstate,MaxSecurity;
UBYTE MaxMagic,MaxPoison;
UBYTE MaxAM,MaxDM,MaxSM;
FILE *UserFP,*GangFP;
ULONG JulianDate;
int Loop=TRUE,online=0;

UWORD Checksum(char *str,int s)
{
	UWORD result;
	int i;

	result=0;
	for(i=0; i<s; i++) {
		result++;
		result^=str[i];
	}
	return(result);
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
	return(days);
}

void GetSystemDateTime(void)
{
	char mmddyyyy[11];
	long t;

	time(&t);
	tp=localtime(&t);

	strftime(mmddyyyy,11,"%m/%d/%Y",tp);		/*  return mm/dd/yyyy  */
	JulianDate=julian(mmddyyyy);
}

void ParseCLI(char *opt)
{
	if(stricmp(opt,"Q")==0) {
		if((port=(struct MsgPort *)FindPort(__procname))) {
			quit.message.mn_Node.ln_Type=NT_MESSAGE;
			quit.message.mn_Length=sizeof(quit);
			quit.message.mn_ReplyPort=NULL;
			quit.command='Q';
			PutMsg(port,(struct Message *)&quit);
		}
		else
			DisplayBeep(NULL);
		Loop=FALSE;
	}
}

void PutGang(int rec)
{
	fseek(GangFP,sizeof(struct Gang)*rec,0);
	fwrite((char *)GANG(rec),sizeof(struct Gang),1,GangFP);
}

void PutSystem(void)
{
	FILE *fp;

	if((fp=fopen("RPGBBS.system","wb"))) {
		fwrite((char *)RPGinfo,sizeof(struct RPGinfo),1,fp);
		fclose(fp);
	}
}

void PutUser(int rec)
{
	if(rec>=0 && rec<=MAXUSER) {
		if(USER(rec)->Gold<1.)
			USER(rec)->Gold=0.;
		if(USER(rec)->Bank<1.)
			USER(rec)->Bank=0.;
		if(USER(rec)->Loan<1.)
			USER(rec)->Loan=0.;
		fseek(UserFP,320*rec,0);
		fwrite((char *)USER(rec),sizeof(struct User),1,UserFP);
	}
}

void UpdateRank(void)
{
	int i,j,k,rec;

	for(i=0; i<3; i++) {
		RANK(i)->User=0;
		RANK(i)->Value=0;
	}

	for(i=1; i<=MAXUSER; i++) {
		j=100*USER(i)->Immortal+USER(i)->Level;
		for(k=0; k<3; k++)
			if(j>RANK(k)->Value) {
				for(rec=2; rec>k; rec--)
					memcpy(RANK(rec),RANK(rec-1),sizeof(struct Rank));
				RANK(k)->User=i;
				RANK(k)->Value=j;
				break;
			}
	}
}

void main(int argc,char **args)
{
	FILE *fp;
	UWORD valid[2];
	int i,j,line,rec;

	if(!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0)))
		exit(5);

	if(argc)
		for(i=1; argc>1; i++, argc--)
			ParseCLI(args[i]);

	if(Loop) {
		Forbid();
		if(!(port=(struct MsgPort *)FindPort(__procname))) {
			if((port=(struct MsgPort *)CreatePort(__procname,0))) {
				Permit();
				GetSystemDateTime();
				memset(LockTable,0,sizeof(LockTable));
				MaxAccess=0;
				if((fp=fopen("RPGBBS.access","rb"))) {
					fread((char *)&MaxAccess,sizeof(MaxAccess),1,fp);
					Access=calloc(MaxAccess,sizeof(struct Access));
					for(i=0; i<MaxAccess; i++)
						fread((char *)ACCESS(i),sizeof(struct Access),1,fp);
					fclose(fp);
				}
				JumpTable.Access=Access;

				MaxClass=0;
				if((fp=fopen("RPGBBS.class","rb"))) {
					fread((char *)&MaxClass,sizeof(MaxClass),1,fp);
					Class=calloc(MaxClass,sizeof(struct Class));
					for(i=0; i<MaxClass; i++)
						fread((char *)CLASS(i),sizeof(struct Class),1,fp);
					fclose(fp);
				}
				JumpTable.Class=Class;

				MaxArmor=0;
				if((fp=fopen("RPGBBS.armor","rb"))) {
					fread((char *)&MaxArmor,sizeof(MaxArmor),1,fp);
					Armor=calloc(MaxArmor+MaxClass,sizeof(struct Store));
					for(i=0; i<MaxArmor; i++)
						fread((char *)ARMOR(i),sizeof(struct Store),1,fp);
					fclose(fp);
					sprintf(ARMOR(MaxArmor)->Name,"Shield of Resistance",CLASS(i)->Name);
					ARMOR(MaxArmor)->Cost=10.*ARMOR(MaxArmor-1)->Cost;
					for(i=1; i<MaxClass; i++) {
						sprintf(ARMOR(MaxArmor+i)->Name,"%s Repellent",CLASS(i)->Name);
						ARMOR(MaxArmor+i)->Cost=ARMOR(MaxArmor*2/3)->Cost;
					}
				}
				JumpTable.Armor=Armor;

				MaxWeapon=0;
				if((fp=fopen("RPGBBS.weapon","rb"))) {
					fread((char *)&MaxWeapon,sizeof(MaxWeapon),1,fp);
					Weapon=calloc(MaxWeapon+MaxClass,sizeof(struct Store));
					for(i=0; i<MaxWeapon; i++)
						fread((char *)WEAPON(i),sizeof(struct Store),1,fp);
					fclose(fp);
					sprintf(WEAPON(MaxWeapon)->Name,"Sword of Omnipotence",CLASS(i)->Name);
					WEAPON(MaxWeapon)->Cost=10.*WEAPON(MaxWeapon-1)->Cost;
					for(i=1; i<MaxClass; i++) {
						sprintf(WEAPON(MaxWeapon+i)->Name,"%s Slayer",CLASS(i)->Name);
						WEAPON(MaxWeapon+i)->Cost=WEAPON(MaxWeapon*2/3)->Cost;
					}
				}
				JumpTable.Weapon=Weapon;

				MaxRealEstate=0;
				if((fp=fopen("RPGBBS.realestate","rb"))) {
					fread((char *)&MaxRealEstate,sizeof(MaxRealEstate),1,fp);
					RealEstate=calloc(MaxRealEstate,sizeof(struct Store));
					for(i=0; i<MaxRealEstate; i++)
						fread((char *)ESTATE(i),sizeof(struct Store),1,fp);
					fclose(fp);
				}
				JumpTable.RealEstate=RealEstate;

				MaxSecurity=0;
				if((fp=fopen("RPGBBS.security","rb"))) {
					fread((char *)&MaxSecurity,sizeof(MaxSecurity),1,fp);
					Security=calloc(MaxSecurity,sizeof(struct Store));
					for(i=0; i<MaxSecurity; i++)
						fread((char *)SECURITY(i),sizeof(struct Store),1,fp);
					fclose(fp);
				}
				JumpTable.Security=Security;

				MaxMagic=0;
				if((fp=fopen("RPGBBS.magic","rb"))) {
					fread((char *)&MaxMagic,sizeof(MaxMagic),1,fp);
					Magic=calloc(MaxMagic,sizeof(struct Extra));
					for(i=0; i<MaxMagic; i++)
						fread((char *)MAGIC(i),sizeof(struct Extra),1,fp);
					fclose(fp);
				}
				JumpTable.Magic=Magic;

				MaxPoison=0;
				if((fp=fopen("RPGBBS.poison","rb"))) {
					fread((char *)&MaxPoison,sizeof(MaxPoison),1,fp);
					Poison=calloc(MaxPoison,sizeof(struct Extra));
					for(i=0; i<MaxPoison; i++)
						fread((char *)POISON(i),sizeof(struct Extra),1,fp);
					fclose(fp);
				}
				JumpTable.Poison=Poison;

				MaxAM=0;
				if((fp=fopen("RPGBBS.monster","rb"))) {
					fread((char *)&MaxAM,sizeof(MaxAM),1,fp);
					AM=calloc(MaxAM,sizeof(struct AM));
					for(i=0; i<MaxAM; i++)
						fread((char *)ARENA(i),sizeof(struct AM),1,fp);
					fclose(fp);
				}
				JumpTable.AM=AM;

				MaxDM=0;
				if((fp=fopen("RPGBBS.dungeon","rb"))) {
					fread((char *)&MaxDM,sizeof(MaxDM),1,fp);
					DM=calloc(MaxDM,sizeof(struct DM));
					for(i=0; i<MaxDM; i++)
						fread((char *)DUNGEON(i),sizeof(struct DM),1,fp);
					fclose(fp);
				}
				JumpTable.DM=DM;

				MaxSM=0;
				if((fp=fopen("RPGBBS.seamonster","rb"))) {
					fread((char *)&MaxSM,sizeof(MaxSM),1,fp);
					SM=calloc(MaxSM,sizeof(struct SM));
					for(i=0; i<MaxSM; i++)
						fread((char *)SEA(i),sizeof(struct SM),1,fp);
					fclose(fp);
				}
				JumpTable.SM=SM;

				User=calloc(MAXUSER+5,sizeof(struct User));
				if((UserFP=fopen("User","rb"))) {
					fread((char *)USER(0),sizeof(struct User),1,UserFP);
					valid[0]=0;
					valid[1]=0;
					if(MAXLEVEL>29) {
						valid[1]=32768-MAXLEVEL;
						valid[1]+=Checksum(USER(0)->Street[0],sizeof(USER(0)->Street[0]));
						valid[1]+=Checksum(USER(0)->Street[1],sizeof(USER(0)->Street[1]));
					}
					if(MAXLINE>1) {
						valid[0]=255-MAXLINE;
						valid[0]+=Checksum(USER(0)->Street[0],sizeof(USER(0)->Street[0]));
						valid[0]+=Checksum(USER(0)->Street[1],sizeof(USER(0)->Street[1]));
					}
					if(USER(0)->LastTime!=valid[0] || USER(0)->LastBaud!=valid[1]) {
						fclose(UserFP);
						UserFP=NULL;
					}
				}
				if(!UserFP) {
					if(!(UserFP=fopen("User","wb")))
						exit(1);
					strncpy(USER(0)->Name,"Hack & Slash",sizeof(USER(0)->Name));
					strncpy(USER(0)->Handle,"Joe Mollicone",sizeof(USER(0)->Handle));
					strncpy(USER(0)->Street[0],"ShareWare Demo",sizeof(USER(0)->Street[1]));
					strncpy(USER(0)->Street[1],"29 Levels of Play",sizeof(USER(0)->Street[1]));
					USER(0)->LastTime=0;				/*  Server Lic#  */
					USER(0)->LastBaud=0;				/*  Client Lic#  */
					USER(0)->ExpireDate=JulianDate+1;	/*  Next reroll date  */
					USER(0)->Experience=.1;				/*  Post/Call ratio  */
					USER(0)->Ratio='Y';
					USER(0)->ULfiles=1;
					USER(0)->DLfiles=10;
					MAXLEVEL=29;
					MAXLINE=1;
					fwrite((char *)USER(0),320,1,UserFP);
					for(i=1; i<=MAXUSER; i++)
						fwrite((char *)USER(1),320,1,UserFP);
				}
				fclose(UserFP);
				if(!(UserFP=fopen("User","rb+")))
					exit(1);
				for(i=0; i<=MAXUSER; i++) {
					fseek(UserFP,320*i,0);
					fread((char *)USER(i),sizeof(struct User),1,UserFP);
				}
				USER(0)->VoicePhone=4017381437.;			/*  BBS number   */
				USER(0)->DataPhone=5083997536.;				/*  Door number  */
				USER(0)->BirthDate=julian("02/27/1994");	/*  Version date */
				USER(0)->Rows=24;
				MAXACCESS=MaxAccess;
				MAXCLASS=MaxClass;
				MAXWEAPON=MaxWeapon;
				MAXARMOR=MaxArmor;
				MAXREALESTATE=MaxRealEstate;
				MAXSECURITY=MaxSecurity;
				MAXMAGIC=MaxMagic;
				MAXPOISON=MaxPoison;
				MAXAM=MaxAM;
				MAXDM=MaxDM;
				MAXSM=MaxSM;
				JumpTable.User=User;

				Gang=calloc(MAXGANG+1,sizeof(struct Gang));
				if(!(GangFP=fopen("Gang","rb"))) {
					if(!(GangFP=fopen("Gang","wb")))
						exit(2);
					strcpy(GANG(0)->Name,"Monster Mash");
					for(i=0; i<=MAXGANG; i++)
						PutGang(i);
				}
				fclose(GangFP);
				if(!(GangFP=fopen("Gang","rb+")))
					exit(2);
				for(i=0; i<=MAXGANG; i++) {
					fseek(GangFP,sizeof(struct Gang)*i,0);
					fread((char *)GANG(i),sizeof(struct Gang),1,GangFP);
				}
				for(i=1; i<=MAXGANG; i++) {
					if(!GANG(i)->Member[0])
						memset(GANG(i),0,sizeof(struct Gang));
					for(j=0; j<4; j++)
						if((GANG(i)->Member[j]>0 && USER(GANG(i)->Member[j])->Gang!=i) || GANG(i)->Member[0]==0)
							GANG(i)->Member[j]=0;
					PutGang(i);
				}
				for(i=1; i<=MAXUSER; i++)
					if(USER(i)->Gang>0) {
						for(j=0; j<4 && GANG(USER(i)->Gang)->Member[j]!=i; j++);
						if(j==4) {
							USER(i)->Gang=0;
							PutUser(i);
						}
					}
				JumpTable.Gang=Gang;

				HOF=calloc(MaxClass*6,sizeof(struct HOF));
				if(!(fp=fopen("RPGBBS.hof","rb"))) {
					if(!(fp=fopen("RPGBBS.hof","wb")))
						exit(3);
					for(i=0; i<MaxClass; i++) {
						for(j=0; j<6; j++) {
							FAME(i,j)->ValueLeast=65535;
							FAME(i,j)->ValueMost=0;
							strcpy(FAME(i,j)->WhoLeast,"Nobody");
							strcpy(FAME(i,j)->WhoMost,"Nobody");
							FAME(i,j)->DateLeast=0;
							FAME(i,j)->DateMost=0;
						}
						FAME(i,5)->ValueLeast=0;	/* for most joust wins */
					}
					fwrite((char *)HOF,sizeof(struct HOF),MaxClass*6,fp);
				}
				fclose(fp);
				if(!(fp=fopen("RPGBBS.hof","rb")))
					exit(3);
				fread((char *)HOF,sizeof(struct HOF),MaxClass*6,fp);
				fclose(fp);
				JumpTable.HOF=HOF;

				RPGinfo=calloc(1,sizeof(struct RPGinfo));
				if(!(fp=fopen("RPGBBS.system","rb")))
					PutSystem();
				else
					fclose(fp);
				if(!(fp=fopen("RPGBBS.system","rb")))
					exit(4);
				fread((char *)RPGinfo,sizeof(struct RPGinfo),1,fp);
				fclose(fp);
				JumpTable.RPGinfo=RPGinfo;

				Rank=calloc(3,sizeof(struct Rank));
				UpdateRank();
				JumpTable.Rank=Rank;

				do {
					WaitPort(port);
					while((server=(struct RPGserver *)GetMsg(port))) {
						for(line=0; line<MAXLINE; line++)
							if(LockTable[line].user==server->user) {
								if(LockTable[line].hold) {
									free(LockTable[line].hold);
									LockTable[line].hold=NULL;
								}
								break;
							}
						rec=server->parameter;
						switch(server->command) {
							case 'C':	/*  Connected  */
								if(!server->user) {		/*  Init connect */
									online++;
									if(online>MAXLINE) {
										server->result=NULL;
										break;
									}
								}
								if(line==MAXLINE)
									for(line=0; line<MAXLINE; line++)
										if(!LockTable[line].user)
											break;
								if(line<MAXLINE) {
									server->result=(UBYTE *)&JumpTable;
									memset(&LockTable[line],0,sizeof(struct LockTable));
									LockTable[line].user=server->user;
								}
								break;
							case 'D':	/*  Disconnected  */
								if(!server->user && online>0) {	/*  Shutdown disconnect  */
									online--;
									break;
								}
								if(line<MAXLINE) {
									if(LockTable[line].pending)
										free(LockTable[line].pending);
									if(LockTable[line].hold)
										free(LockTable[line].hold);
									memset(&LockTable[line],0,sizeof(struct LockTable));
								}
								break;
							case 'G':	/*  Write to gang file  */
								PutGang(rec);
								break;
							case 'H':	/*  Write to Hall of Fame file  */
								if((fp=fopen("RPGBBS.hof","wb"))) {
									fwrite((char *)HOF,sizeof(struct HOF),MaxClass*6,fp);
									fclose(fp);
								}
								break;
							case 'L':	/*  Lock user  */
								for(i=0; i<MAXLINE; i++) {
									if(LockTable[i].user==rec) {
										rec=(i==line) ? -1 : 0;
										break;
									}
									for(j=0; j<4; j++)
										if(LockTable[i].entry[j]==rec)
											rec=0;
								}
								if(rec>0) {
									if(line<MAXLINE)
										for(i=0; i<4; i++)
											if(!LockTable[line].entry[i]) {
												LockTable[line].entry[i]=rec;
												break;
											}
								}
								if(!rec)
									server->result="That user is already engaged in battle elsewhere.";
								break;
							case 'M':	/*  Send message to online users  */
								for(i=0; i<MAXLINE; i++)
									if(LockTable[i].user && i!=line) {
										if(LockTable[i].pending)
											LockTable[i].pending=realloc(LockTable[i].pending,strlen(LockTable[i].pending)+strlen(server->result)+3);
										else
											LockTable[i].pending=calloc(1,strlen(server->result)+3);
										strcat(LockTable[i].pending,"\15\12");
										strcat(LockTable[i].pending,server->result);
									}
								break;
							case 'P':	/*  Write to player file  */
								PutUser(rec);
								break;
							case 'Q':	/*  Quit server */
								if(online) {
									DisplayBeep(NULL);
									if(AutoRequest(NULL,&Warning,&Yes,&No,0L,0L,320L,60L))
										Loop=FALSE;
								}
								else {
									if(AutoRequest(NULL,&Really,&Yes,&No,0L,0L,320L,60L))
										Loop=FALSE;
								}
								break;
							case 'R':	/*  Update Top 3 Immortals  */
								UpdateRank();
								break;
							case 'S':	/*  Write System file  */
								PutSystem();
								break;
							case 'U':	/*  Unlock all  */
								if(line<MAXLINE) {
									server->result=LockTable[line].pending;
									LockTable[line].hold=LockTable[line].pending;
									LockTable[line].pending=NULL;
									memset(&LockTable[line].entry,0,sizeof(LockTable[line].entry));
								}
								break;
							default:
								server->result="?Invalid server command";
								break;
						}
						ReplyMsg((struct Message *)server);
					}
				} while(Loop);
				fclose(UserFP);
				fclose(GangFP);
				PutSystem();
				DeletePort(port);
				free(Access);
				free(Class);
				free(Armor);
				free(Weapon);
				free(RealEstate);
				free(Security);
				free(Magic);
				free(Poison);
				free(AM);
				free(DM);
				free(SM);
				free(User);
				free(Gang);
				free(HOF);
				free(RPGinfo);
				for(line=0; line<MAXLINE; line++) {
					if(LockTable[line].pending)
						free(LockTable[line].pending);
					if(LockTable[line].hold)
						free(LockTable[line].hold);
				}
			}
		}
		else {
			Permit();
			DisplayBeep(NULL);
		}
	}
	CloseLibrary((struct Library *)IntuitionBase);
}
