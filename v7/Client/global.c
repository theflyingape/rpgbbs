#include "/include/all.h"

char __near *__procname="RPGBBSclient";
long __near __stack=8192;
long __near __priority=0;

struct	AppIcon			*appicon;
struct	DiskObject		*appobj,*diskobj;
struct	DTL_Base		*DesTrackerBase;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*DiskfontBase,*IconBase,*RPGBase,*WorkbenchBase;
struct	MsgPort			*RPGport,*RPGreply;
struct	Screen			*pubscreen;
struct	Window			*viewwindow,*window;
struct	FileInfoBlock	infoBlock;
struct	sound_struct	*sample1,*sample2;
struct	WBStartup		*WBenchMsg;
struct	WBArg			*wbarg;
struct	IOExtSer		*HoldSerialIO;
struct	MsgPort			*HoldSerialMP;

BPTR	root,lock;

struct IntuiText __far project_shutdown = {
	0,0,JAM1,1,1,NULL,"Quit",NULL
};
struct MenuItem __far shutdown = {
	NULL,					/*	NextItem			*/
	0,36,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_shutdown,/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'Q',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_view = {
	0,0,JAM1,1,1,NULL,"View",NULL
};
struct MenuItem __far view = {
	&shutdown,				/*	NextItem			*/
	0,24,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_view,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'V',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_logon = {
	0,0,JAM1,1,1,NULL,"Logon",NULL
};
struct MenuItem __far Mlogon = {
	&view,					/*	NextItem			*/
	0,12,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_logon,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'L',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_logoff = {
	0,0,JAM1,1,1,NULL,"Logoff",NULL
};
struct MenuItem __far Mlogoff = {
	&view,					/*	NextItem			*/
	0,12,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_logoff,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'L',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_iconify = {
	0,0,JAM1,1,1,NULL,"Iconify",NULL
};
struct MenuItem __far iconify = {
	&Mlogon,				/*	NextItem			*/
	0,0,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_iconify,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'I',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct Menu __far project = {
	NULL,					/*	NextMenu			*/
	0,0,					/*	LeftEdge, TopEdge	*/
	80,10,					/*	Width, Height		*/
	MENUENABLED,			/*	Flags				*/
	" Client ",				/*	MenuName			*/
	&iconify				/*	MenuItem			*/
};

struct RPGserver server,*response;
struct SERCON SERCON;
struct ServerTable *ServerTable;
struct gang gm[3];
struct RPC *RPC[2][5];
struct userlevel userlevel;
struct username username;

struct BBSprefs {
	UBYTE Title[41];
	UBYTE Font[40];
	UBYTE Size;
	UBYTE Device[41];
	UBYTE Unit;
	ULONG Baud;
	UBYTE Init[41];
	UBYTE Quit[41];
	UWORD AreaCode;
	UBYTE Auto;
	UBYTE CallerID;
	UBYTE Delay;
	UBYTE Offset;
};
struct BBSprefs BBSprefs = {
	"Client","venus.font",8,SERIALNAME,0,2400,"ATS0=1","ATZ",0,2,0,0,1
};

UWORD DriPens[2][10] = {
	{ 0,1,2,3,2,1,0,0,1,(UWORD)~0 },
	{ BLK,RED,CYN,WHITE,BLACK,GRY,BLU,BLK,RED,(UWORD)~0 }
};

UBYTE box[2][11] = {
	{ 'q','m','v','j','t','n','u','l','w','k','x' },
	{ 'Ä','À','Á','Ù','Ã','Å','´','Ú','Â','¿','³' }
};

struct card {
   UBYTE *Name;
   UBYTE Value;
};
struct card __far card[13] = {
   { "Two",2 },{ "Three",3 },{ "Four",4 },{ "Five",5 },{ "Six",6 },{ "Seven",7 },
   { "Eight",8 },{ "Nine",9 },{ "Ten",10 },{"Jack",11 },{ "Queen",12 },{ "King",13 },
   { "Ace",14 }
};
struct card __far *deck[52];
char __far bandit[3];
char __far slotcol[6] = { YELLOW,CYAN,YELLOW,GREEN,MAGENTA,BRED };
char __far *slotval[6] = { "=GOLD=","-BELL-","ORANGE"," LIME "," PLUM ","CHERRY" };
char __far spin[4] = { '|','/','-','\\' };
char __far wheel[3][14] = {
	{ GOLD,PLUM,CHERRY,BELL,LIME,PLUM,CHERRY,ORANGE,LIME,PLUM,CHERRY,BELL,LIME,CHERRY },
	{ ORANGE,GOLD,CHERRY,ORANGE,PLUM,ORANGE,CHERRY,BELL,ORANGE,CHERRY,PLUM,LIME,ORANGE,CHERRY },
	{ BELL,LIME,CHERRY,ORANGE,PLUM,LIME,CHERRY,GOLD,PLUM,CHERRY,LIME,PLUM,LIME,CHERRY }
};

UBYTE __far keycolor[4] = {MAGENTA,YELLOW,CYAN,BRED};
UBYTE __far keytype[4] = {'p','g','s','c'};
UBYTE __far keycode[4];

struct RPGmenu __far XPRmenu = { "Protocol",MAG,5,
   {  { 'B',"Kermit binary" },
      { 'C',"Xmodem classic" },
      { 'K',"Kermit text" },
      { 'X',"Xmodem-1k" },
      { 'Z',"Zmodem" }
   }
};

DOUBLE d;
JDATE Julian,LastFileScan;
ULONG baud;
UWORD menu,menuitem,subitem,zoom[4];
UBYTE *Serbuf,*outbuf,*prompt,*inbuf,*filename;
UBYTE SYSID[5],argument[41];
UBYTE Today[9],Time[7];
UBYTE dated[16],timed[8];
UBYTE RPGBBSmode;
UBYTE acclvl,ANSI,bold,chat,echo,from,local,logoff,paws,rows,source,timeout;
UBYTE arena,bail,brawl,charity,dungeon,joust,naval,party,tiny;
UBYTE callerid[16];
UBYTE reason[64];
UBYTE **line,numline;
UBYTE base,MaxDir,MaxFILE,section;
UBYTE IconName[41];
UBYTE EMAILpath[41];
UBYTE MSGpath[41];
UBYTE NOTESpath[41];
UBYTE UPLOADpath[41];
UBYTE FILEpath[16][41];
UBYTE FILEname[16][31];
UBYTE directory[128];
UBYTE extension[40];
UBYTE *ta[50];
