extern	struct	AppIcon			*appicon;
extern	struct	DiskObject		*appobj,*diskobj;
extern	struct	IntuitionBase	*IntuitionBase;
extern	struct	Library			*DiskfontBase,*IconBase,*RPGBase,*WorkbenchBase;
extern	struct	MsgPort			*RPGport,*RPGreply;
extern	struct	Screen			*pubscreen;
extern	struct	Window			*viewwindow,*window;
extern	struct	FileInfoBlock	infoBlock;
extern	struct	sound_struct	*sample1,*sample2;
extern	struct	WBStartup		*WBenchMsg;
extern	struct	WBArg			*wbarg;
extern	struct	IOExtSer		*HoldSerialIO;
extern	struct	MsgPort			*HoldSerialMP;

extern	BPTR	root,lock;

extern	struct MenuItem __far iconify,Mlogon,Mlogoff,view,shutdown;
extern	struct Menu __far project;

extern	struct RPGserver server,*response;
extern	struct SERCON SERCON;
extern	struct ServerTable *ServerTable;
extern	struct gang gm[3];
extern	struct RPC *RPC[2][5];
extern	struct userlevel userlevel;
extern	struct username username;

extern	struct BBSprefs {
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
extern	struct BBSprefs BBSprefs;

extern	UBYTE box[2][11];

extern	struct card {
			UBYTE *Name;
			UBYTE Value;
		} __far card[13],*deck[52];

extern	char __far bandit[3];
extern	char __far slotcol[6];
extern	char __far *slotval[6];
extern	char __far spin[4];
extern	char __far wheel[3][14];

extern	UBYTE __far keycolor[4];
extern	UBYTE __far keytype[4];
extern	UBYTE __far keycode[4];

extern	struct RPGmenu __far XPRmenu;

extern	DOUBLE d;
extern	JDATE Julian,LastFileScan;
extern	UWORD DriPens[2][10];
extern	UWORD menu,menuitem,subitem,baud,zoom[4];
extern	UBYTE *Serbuf,*outbuf,*prompt,*inbuf,*filename;
extern	UBYTE SYSID[5],argument[41];
extern	UBYTE Today[9],Time[7];
extern	UBYTE dated[16],timed[8];
extern	UBYTE acclvl,ANSI,bold,chat,echo,from,local,logoff,paws,rows,source,timeout;
extern	UBYTE RPGBBSmode;
extern	UBYTE arena,bail,brawl,charity,dungeon,joust,naval,party,tiny;
extern	UBYTE callerid[16];
extern	UBYTE reason[64];
extern	UBYTE **line,numline;
extern	UBYTE base,MaxDir,MaxFILE,section;
extern	UBYTE IconName[41];
extern	UBYTE EMAILpath[41];
extern	UBYTE MSGpath[41];
extern	UBYTE NOTESpath[41];
extern	UBYTE UPLOADpath[41];
extern	UBYTE FILEpath[16][41];
extern	UBYTE FILEname[16][31];
extern	UBYTE directory[128];
extern	UBYTE extension[40];
extern	UBYTE *ta[50];
