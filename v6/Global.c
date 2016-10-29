#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <devices/serial.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXDX 15
#define MAXDY 10
#define  BLK      0
#define  RED      1
#define  GRN      2
#define  BRN      3
#define  BLU      4
#define  MAG      5
#define  CYN      6
#define  GRY      7
#define  BLACK 8
#define  BRED  9
#define  GREEN 10
#define  YELLOW   11
#define  BLUE  12
#define  MAGENTA  13
#define  CYAN  14
#define  WHITE 15

#define	GOLD	0
#define	BELL	1
#define	ORANGE	2
#define	LIME	3
#define	PLUM	4
#define	CHERRY	5

char __near *__procname="RPGclient";
long __near __stack=8192;
long __near __priority=0;
long __near __BackGroundIO=0;

/* struct   DosLibrary     *DOSBase; */
struct   GfxBase        *GfxBase;
struct   InputEvent     InputEvent;
struct   IntuiMessage   *message;
struct   IntuitionBase  *IntuitionBase;
struct   IOExtSer    *SerialIO;
struct   IOStdReq    *ConsoleIO;
struct   Library        *ConsoleDevice,*DiskfontBase,*IconBase,*NoisyBase;
struct   MsgPort        *CPort,*RPGport,*RPGreply,*SPort;
struct   Process        *process;
struct   RastPort    *rp;
struct   Screen         *screen;
struct   TagItem        tags[5];
struct   TextFont    *TextFont[2];
struct   tm          *tp;
struct   ViewPort    *vp;
struct   Window         *window,*holdwindow,*userwindow,*nmewindow,*iconwindow;

struct BBSprefs {
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
};
struct BBSprefs BBSprefs = {
   SERIALNAME,0,2400,"ATS0=1","ATZ",5,3,0,3,"venus.font",0,0
};

struct RPGserver {
   struct Message message;
   UBYTE user;
   UBYTE command;
   UBYTE parameter;
   UBYTE *result;
} server,*response;

struct TextAttr __far font[2] = {
   "topaz.font",TOPAZ_EIGHTY,FS_NORMAL,FPF_ROMFONT,
   "venus.font",8,FS_NORMAL|FSF_BOLD|FSF_ITALIC|FSF_UNDERLINED,FPF_DISKFONT
};

struct ExtNewScreen __far newscreen = {
   0,0,                 /* LeftEdge, TopEdge */
   640,200,             /* Width, Height     */
   3,                   /* Depth          */
   0,3,                 /* DetailPen, BlockPen  */
   HIRES,                  /* ViewModes         */
   CUSTOMSCREEN | NS_EXTENDED,   /* Type           */
   &font[0],               /* Font           */
   NULL,
   NULL,                /* Gadgets           */
   NULL,                /* CustomBitMap      */
   tags                 /* Extension         */
};

UBYTE title[81];

struct NewWindow __far newwindow = {
   0,0,640,200,0,3,
   MENUPICK | RAWKEY,
   SMART_REFRESH | ACTIVATE,
   NULL,NULL,
   (UBYTE *)&title,
   NULL,NULL,
   640,40,640,200,
   CUSTOMSCREEN
};

struct NewWindow __far newuserwindow = {
   0,0,320,86,1,0,
   CLOSEWINDOW,
   GIMMEZEROZERO | SMART_REFRESH |
   WINDOWCLOSE | WINDOWDEPTH | WINDOWDRAG,
   NULL,NULL,
   "User Profile",
   NULL,NULL,
   320,86,320,86,
   CUSTOMSCREEN
};

struct NewWindow __far newnmewindow = {
   320,0,320,86,1,0,
   CLOSEWINDOW,
   GIMMEZEROZERO | SMART_REFRESH |
   WINDOWCLOSE | WINDOWDEPTH | WINDOWDRAG,
   NULL,NULL,
   "Enemy Profile",
   NULL,NULL,
   320,86,320,86,
   CUSTOMSCREEN
};

UBYTE icontitle[41];

struct NewWindow __far newiconwindow = {
   0,0,288,11,1,0,
   ACTIVEWINDOW,
   NULL,
   NULL,NULL,
   (UBYTE *)&icontitle,
   NULL,NULL,
   320,10,320,10,
   WBENCHSCREEN
};

ULONG MessageClass,ChatKey;
UWORD code,qualifier,menu,menuitem,subitem,gadgetnr;

struct Access {
   UBYTE Name[10];
   UBYTE Calls,Minutes,Level;
   UBYTE User,Bulletin,Dungeon,File,Naval,Party,Tavern,Sysop;
} *Access;

struct Class {
   UBYTE Name[10];
   BYTE Poison,Vial;                /*  -1=service,0=none,1=1x,2=2x  */
   BYTE Magic,ToCast,ToInt,ToHeal,ToBlast;   /*  0=none,1=wand,2=scroll,3=spell,4=super  */
   BYTE Backstab;                   /*  0=none,2=2x,3=3x,etc.  */
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
   UWORD LastMSG[16];
   DOUBLE BountyAmount;
   UBYTE BountyWho;
   UBYTE Ratio;
   UWORD ULfiles,DLfiles;
   ULONG ULbytes,DLbytes;
   LONG LastFileScan;
   UWORD Status;
} *User,NME,ToStart;

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

struct Rank {
   UBYTE User;
   UWORD Value;
} *Rank;

struct RPGinfo {
   UBYTE LastCaller[3][26];
   UBYTE Reason[3][80];
   UBYTE Border[80];
   UWORD FirstMSG[16],NextMSG[16];
} *RPGinfo;

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
} *JumpTable;

struct card {
   UBYTE *Name;
   UBYTE Value;
};
struct card card[13] = {
   { "Two",2 },{ "Three",3 },{ "Four",4 },{ "Five",5 },{ "Six",6 },{ "Seven",7 },
   { "Eight",8 },{ "Nine",9 },{ "Ten",10 },{"Jack",11 },{ "Queen",12 },{ "King",13 },
   { "Ace",14 }
};
struct card *deck[52];

char bandit[3];
char slotcol[6] = { YELLOW,CYAN,YELLOW,GREEN,MAGENTA,BRED };
char *slotval[6] = { "=GOLD=","-BELL-","ORANGE"," LIME "," PLUM ","CHERRY" };
char spin[4] = { '|','/','-','\\' };
char wheel[3][14] = {
	{ GOLD,PLUM,CHERRY,BELL,LIME,PLUM,CHERRY,ORANGE,LIME,PLUM,CHERRY,BELL,LIME,CHERRY },
	{ ORANGE,GOLD,CHERRY,ORANGE,PLUM,ORANGE,CHERRY,BELL,ORANGE,CHERRY,PLUM,LIME,ORANGE,CHERRY },
	{ BELL,LIME,CHERRY,ORANGE,PLUM,LIME,CHERRY,GOLD,PLUM,CHERRY,LIME,PLUM,LIME,CHERRY }
};

UWORD palette[4][16] = {
   {  0x003,   /* 0 Blue      */
      0xddd /* 1 White     */
   },{
      0x003,   /* 0 Blue      */
      0xbbb,   /* 1 Gray      */
      0x888,   /* 2 Dk Gray   */
      0xfff /* 3 White     */
   },{
      0x000,   /* 0 Black     */
      0xc00,   /* 1 Red    */
      0x0c0,   /* 2 Green     */
      0xcc0,   /* 3 Yellow */
      0x00c,   /* 4 Blue      */
      0xc0c,   /* 5 Purple */
      0x0cc,   /* 6 Cyan      */
      0xccc /* 7 Gray      */
   },{
      0x000,   /* 0 Black     */
      0xa00,   /* 1 Red    */
      0x0a0,   /* 2 Green     */
      0xa80,   /* 3 Brown     */
      0x00a,   /* 4 Blue      */
      0xa0a,   /* 5 Purple */
      0x0aa,   /* 6 Cyan      */
      0xaaa,   /* 7 Gray      */
      0x888,   /* 8 Dark Gray */
      0xf00,   /* 9 Red    */
      0x0f0,   /* A Green     */
      0xff0,   /* B Yellow */
      0x00f,   /* C Blue      */
      0xf0f,   /* D Purple */
      0x0ff,   /* E Cyan      */
      0xfff /* F White     */
   }
};

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
struct RPGmenu __far XPRmenu = { "Protocol",MAG,5,
   {  { 'B',"Kermit binary" },
      { 'C',"Xmodem classic" },
      { 'K',"Kermit text" },
      { 'X',"Xmodem-1k" },
      { 'Z',"Zmodem" }
   }
};
DOUBLE d;
UWORD MaxFILE,MaxMSG,KeepMSG[16],baud,carrier,Tsp,NMEsp;
WORD online,Time,Thp,Thull,NMEhp,NMEhull;
UBYTE Tstr,Tint,Tagl,Tchr,NMEstr,NMEint,NMEagl,NMEchr,NMEcure;
WORD Thit,Tshield,NMEhit,NMEshield;
UBYTE ANSI,local,logoff,user,nme,sender,joust,fight,brawl,naval,party,from,ww,key[3];
UBYTE EMAILpath[41],FILEpath[16][41],HELPpath[41],MSGpath[41],NOTESpath[41];
UBYTE ArenaPIXpath[41],DungeonPIXpath[41],PlayerPIXpath[41],UPLOADpath[41];
UBYTE FILEname[16][31],MSGname[16][31];
UBYTE INbuf[256],OUTbuf[256],directory[128],filename[128],extension[40],reason[80],*ta[50];
UBYTE echo,ins[81],text[128],numline;
UBYTE BoldFlag,Ca,Cb,Cflag,Cflash,Cx,Cy,MAXsx=79,MAXsy=22,SRtop=0,SRbottom=22,RVSflag;
UBYTE __far ScreenBuf[23][80],AttrBuf[23][80];
UBYTE __far line[50][128];
ULONG JulianDate,Date,LastFileScan;
UBYTE SystemDate[9],SystemTime[9];
UBYTE SystemMonth,weekday;
UBYTE *NMEHe,*NMEhe,*NMEHim,*NMEhim,*NMEHis,*NMEhis;
UBYTE dungeon[MAXDY][MAXDX];
int MaxDX,MaxDY,MaxDir,dx,dy,gift,hx,hy,level,map,escape;
int acclvl,cuss,grace,modifier;
int base,section;
char correct[3];
