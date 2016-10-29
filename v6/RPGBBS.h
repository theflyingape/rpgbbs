/***************************\
 *  Role Playing Game BBS  *
 *                         *
 *  C Header Include File  *
\***************************/
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/console.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <devices/serial.h>
#include <intuition/intuitionbase.h>
#include <ctype.h>
#include <dos.h>
#include <math.h>
#include <noisy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXDX 15
#define MAXDY 10
#define	BLK		0
#define	RED		1
#define	GRN		2
#define	BRN		3
#define	BLU		4
#define	MAG		5
#define	CYN		6
#define	GRY		7
#define	BLACK	8
#define	BRED	9
#define	GREEN	10
#define	YELLOW	11
#define	BLUE	12
#define	MAGENTA	13
#define	CYAN	14
#define	WHITE	15

#define	GOLD	0
#define	BELL	1
#define	ORANGE	2
#define	LIME	3
#define	PLUM	4
#define	CHERRY	5

#define JOUST(n) modf(pow2((double)(n-1)/2.)*10.,&d);

extern	struct	GfxBase			*GfxBase;
extern	struct	InputEvent		InputEvent;
extern	struct	IntuiMessage	*message;
extern	struct	IntuitionBase	*IntuitionBase;
extern	struct	IOExtSer		*SerialIO;
extern	struct	IOStdReq		*ConsoleIO;
extern	struct	Library			*ConsoleDevice,*DiskfontBase,*IconBase,*NoisyBase;
extern	struct	MsgPort			*CPort,*RPGport,*RPGreply,*SPort;
extern	struct	Process			*process;
extern	struct	Screen			*screen;
extern	struct	TagItem			tags[5];
extern	struct	TextFont		*TextFont[2];
extern	struct	tm				*tp;
extern	struct	ViewPort		*vp;
extern	struct	Window			*window,*holdwindow,*userwindow,*nmewindow,*iconwindow;

extern	struct BBSprefs {
			UBYTE Device[41];
			UBYTE Unit;
			UWORD Baud;
			UBYTE Init[41];
			UBYTE Quit[41];
			UBYTE Delay;
			UBYTE Auto;
			UBYTE Keep;
			UBYTE Depth;
			UBYTE Font[40];
			UWORD IconX;
			UWORD IconY;
		} BBSprefs;
extern	struct RPGserver {
			struct Message message;
			UBYTE user;
			UBYTE command;
			UBYTE parameter;
			UBYTE *result;
		} server,*response;

extern	struct TextAttr __far font[2];
extern	struct ExtNewScreen __far newscreen;
extern	struct NewWindow __far newwindow,newuserwindow,newnmewindow,newiconwindow;
extern	struct Menu __far project;
extern	struct MenuItem __far Mlogon,Mlogoff;
extern	struct Image ME;

extern	ULONG MessageClass,ChatKey;
extern	UWORD code,qualifier,menu,menuitem,subitem,gadgetnr;
extern	FILE *UserFP,*GangFP;

/**********************\
 *  Common variables  *
\**********************/
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

extern	struct Access {
			UBYTE Name[10];
			UBYTE Calls,Minutes,Level;
			UBYTE User,Bulletin,Dungeon,File,Naval,Party,Tavern,Sysop;
		} *Access;

extern	struct Class {
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

extern	struct Store {
			char Name[26];
			double Cost;
		} *Armor,*Weapon,*RealEstate,*Security;

extern	struct Extra {
			char Name[20];
			double Cost[2];
			UWORD Power[2];
		} *Magic,*Poison;

extern	struct AM {
			char Name[17];
			UBYTE Class,Level;
			UBYTE Str,Int,Agl,Chr;
			UWORD HP,SP;
			BYTE Weapon,Armor;
			UWORD Spell;
			DOUBLE Gold;
		} *AM;

extern	struct DM {
			char Name[17];
			UBYTE Class;
		} *DM;

extern	struct SM {
			char Name[17];
			UBYTE Int;
			UWORD Hull,Shot,Power;
			char Ram;
			double Gold;
		} *SM;

extern	struct User {
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
			UWORD LastMSG[16];
			DOUBLE BountyAmount;
			UBYTE BountyWho;
			UBYTE Ratio;
			UWORD ULfiles,DLfiles;
			ULONG ULbytes,DLbytes;
			LONG LastFileScan;
			UWORD Status;
		} *User,NME,ToStart;

extern	struct Gang {
			UBYTE Name[20];
			WORD Member[4];
			UWORD Win,Loss;
		} *Gang;

extern	struct HOF {
			UWORD ValueLeast,ValueMost;
			UBYTE WhoLeast[26],WhoMost[26];
			ULONG DateLeast,DateMost;
		} *HOF;

extern	struct RPGinfo {
			UBYTE LastCaller[3][26];
			UBYTE Reason[3][80];
			UBYTE Border[80];
			UWORD FirstMSG[16],NextMSG[16];
		} *RPGinfo;

extern	struct Rank {
			UBYTE User;
			UWORD Value;
		} *Rank;

extern	struct JumpTable {
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
		} *JumpTable;

struct RPGitem {
	char choice;
	char *name;
};
struct RPGmenu {
	char *option;
	char color;
	int items;
	struct RPGitem item[16];
};
extern	struct RPGmenu __far XPRmenu;

extern	struct card {
			UBYTE *Name;
			UBYTE Value;
		} card[13],*deck[52];

extern	char bandit[3];
extern	char slotcol[6];
extern	char *slotval[6];
extern	char spin[4];
extern	char wheel[3][14];

extern	DOUBLE d;
extern	UWORD palette[4][16];
extern	UWORD MaxFILE,MaxMSG,KeepMSG[16],baud,carrier,Tsp,NMEsp;
extern	WORD online,Time,Thp,Thull,NMEhp,NMEhull;
extern	UBYTE Tstr,Tint,Tagl,Tchr,NMEstr,NMEint,NMEagl,NMEchr,NMEcure;
extern	WORD Thit,Tshield,NMEhit,NMEshield;
extern	UBYTE ANSI,local,logoff,user,nme,sender,joust,fight,brawl,naval,party,from,ww,key[3];
extern	UBYTE UPLOADpath[41];
extern	UBYTE EMAILpath[41],FILEpath[16][41],HELPpath[41],MSGpath[41],NOTESpath[41];
extern	UBYTE ArenaPIXpath[41],DungeonPIXpath[41],PlayerPIXpath[41];
extern	UBYTE FILEname[16][31],MSGname[16][31];
extern	UBYTE INbuf[256],OUTbuf[256],directory[128],filename[128],extension[40],reason[80],*ta[50];
extern	UBYTE echo,title[81],icontitle[41],ins[81],text[128],numline;
extern	UBYTE BoldFlag,Ca,Cb,Cflag,Cflash,Cx,Cy,MAXsx,MAXsy,SRtop,SRbottom,RVSflag;
extern	UBYTE __far ScreenBuf[23][80],AttrBuf[23][80];
extern	UBYTE __far line[50][128];
extern	ULONG JulianDate,Date,LastFileScan;
extern	UBYTE SystemDate[9],SystemTime[9];
extern	UBYTE SystemMonth,weekday;
extern	UBYTE *NMEHe,*NMEhe,*NMEHim,*NMEhim,*NMEHis,*NMEhis;
extern	UBYTE dungeon[MAXDY][MAXDX];
extern	int MaxDX,MaxDY,MaxDir,dx,dy,gift,hx,hy,level,map,escape;
extern	int acclvl,cuss,grace,modifier;
extern	int base,section;
extern	char correct[3];
extern	char *__montbl[],*__daytbl[];

extern	void	DeleteSound(ULONG);
extern	ULONG	LoadSound(char *,int);
extern	void	PlaySound(ULONG);
extern	void	SetVolume(ULONG,int);

extern	void	DecodeANSI(char *);
extern	void	DrawCursor(int);
extern	void	backspace(void);
extern	void	beep(void);
extern	int		cd(void);
extern	void	chat(void);
extern	void	cll(void);
extern	void	cls(void);
extern	char	*Bcol(int);
extern	char	*Fcol(int);
extern	UBYTE	GetKey(void);
extern	void	GetSystemDateTime(void);
extern	char	inkey(void);
extern	int		input(void);
extern	void	inputs(char *,int,FILE *);
extern	void	newline(void);
extern	void	normal(void);
extern	void	OffHook(void);
extern	void	outs(char *);
extern	void	plot(int,int);
extern	void	reverse(void);
extern	void	sameline(void);
extern	void	scroll(int);
extern	void	tab(int);
extern	void	WriteConsole(char *);
extern	void	WriteSerial(char *);

extern	void	AdjustTime(int);
extern	int		BlessAssign(void);
extern	int		BlessRemove(void);
extern	char	*bracket(int);
extern	int		CurseAssign(void);
extern	int		CurseRemove(void);
extern	int		dice(int);
extern	void	DisplayUser(void);
extern	void	DisplayNME(void);
extern	char	Editor(int);
extern	double	ExpNeed(int);
extern	char	*Fdate(char *);
extern	char	*Fnum(double);
extern	char	FullScreenEditor(int);
extern	char	*money(double,int);
extern	char	*Ftime(UWORD);
extern	int		GetUser(void);
extern	void	CheckHOF(int);
extern	void	DisplayHOF(int);
extern	void	Email(int);
extern	void	HallOfFame(void);
extern	char	*jdate(ULONG);
extern	ULONG	julian(char *);
extern	char	*KeyHint(void);
extern	void	ListUsers(void);
extern	void	LogLoser(char *,char *);
extern	void	Logoff(void);
extern	char	*Ltrim(char *);
extern	void	MenuFile(char *);
extern	void	Midnight(void);
extern	char	More(void);
extern	char	Option(struct RPGmenu *);
extern	void	Pause(void);
extern	char	*points(int);
extern	void	PutGang(int);
extern	void	PutUser(int);
extern	char	*RankLevel(int);
extern	void	ReadMail(void);
extern	void	RestoreStat(void);
extern	void	RPGmessage(char *);
extern	int		RPGserver(char,int);
extern	void	Shuffle(void);
extern	int		TimeLeft(void);
extern	void	TimeOut(void);
extern	void	ToUpLow(char *);
extern	void	TypeFile(char *,int);
extern	void	TypeMessage(char *);
extern	void	UserNote(int);
extern	double	value(double,int);
extern	int		OpenGUI(void);
extern	void	CloseGUI(void);

extern	void	Arena(void);
extern	void	ArenaPixFile(void);
extern	int		Attack(BYTE,BYTE,BYTE,WORD,BYTE,BYTE,BYTE,WORD,double);
extern	void	Battle(void);
extern	void	Bulletin(void);
extern	void	Casino(void);
extern	int		CastSpell(void);
extern	void	CharacterStats(int);
extern	void	CreateMonster(struct User *,int,int);
extern	void	DeleteUser(int);
extern	void	Dungeon(int);
extern	void	DungeonPixFile(void);
extern	void	ExamineRoom(void);
extern	void	FileSection(void);
extern	void	Hints(void);
extern	void	Last50(char *);
extern	int		Logon(void);
extern	void	MainMenu(void);
extern	void	Naval(void);
extern	int		NewUser(void);
extern	void	NMEstat(void);
extern	void	Party(void);
extern	void	PoisonWeapon(void);
extern	void	ReRoll(int,int);
extern	void	RestartChar(int);
extern	void	Square(void);
extern	void	Sysop(void);
extern	void	SystemReroll(void);
extern	void	Tavern(void);
extern	void	YourStats(void);
