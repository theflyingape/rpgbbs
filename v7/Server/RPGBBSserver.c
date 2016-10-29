/*******************************\
 *  RPGBBS Deluxe File Server  *
 *                             *
 *   Written by Robert Hurst   *
\*******************************/
#include "/include/all.h"
#include <proto/asl.h>
#include <proto/diskfont.h>
#include <exec/interrupts.h>
#include <graphics/monitor.h>
#include "GameSmith:include/proto/all.h"

char __near *__procname="RPGBBSserver";
long __near __stack=4096;
long __near __priority=1;

struct 	DateTime		DateTime;
struct	DiskObject		*diskobj;
struct	DrawInfo		*pubscreen_drawinfo;
struct	Interrupt		*interrupt;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*AslBase;
struct	Library			*BtreeBase;
struct	Library			*DiskfontBase;
struct	Library			*IconBase;
struct	Library			*RPGBase;
struct	MsgPort			*port;
struct	Screen			*pubscreen;
struct	Screen			*screen;
struct	SERCON			SERCON;
struct	WBArg			*wbarg;
struct	WBStartup		*WBenchMsg;
struct	Window			*window;
struct	FileInfoBlock	infoBlock;
BPTR	lockdir,lockfile;

struct ColorSpec FourColor[] = {
	 0, 0x00, 0x00, 0x03,	//	Blue
	 1, 0x0c, 0x0c, 0x0c, 	//	Gray
	 2, 0x08, 0x08, 0x08,	//	Dk Gray
	 3, 0x0f, 0x0f, 0x0f,	//	White
	~0
};
struct ColorSpec SixteenColor[] = {
	 0, 0x00, 0x00, 0x00,	//	Black
	 1, 0x0a, 0x00, 0x00,	//	Red
	 2, 0x00, 0x0a, 0x00,	//	Green
	 3, 0x0a, 0x06, 0x00,	//	Brown
	 4, 0x00, 0x00, 0x0a,	//	Blue
	 5, 0x0a, 0x00, 0x0a,	//	Purple
	 6, 0x00, 0x0a, 0x0a,	//	Cyan
	 7, 0x0a, 0x0a, 0x0a,	//	Gray
	 8, 0x06, 0x06, 0x06,	//	Dk Gray
	 9, 0x0f, 0x00, 0x00,	//	Red
	10, 0x00, 0x0f, 0x00,	//	Green
	11, 0x0f, 0x0f, 0x00,	//	Yellow
	12, 0x00, 0x00, 0x0f,	//	Blue
	13, 0x0f, 0x00, 0x0f,	//	Purple
	14, 0x00, 0x0f, 0x0f,	//	Cyan
	15, 0x0f, 0x0f, 0x0f,	//	White
	~0
};
UWORD DriPens[2][10] = {
	{ 0,1,2,3,2,1,0,0,1,(UWORD)~0 },
	{ BLK,RED,CYN,WHITE,BLACK,GRY,BLU,BLK,RED,(UWORD)~0 }
};
UBYTE RPGBBSmode=BBS;
ULONG Mode=0L;
UWORD Depth,Width,Height;
UBYTE Font[32] = { "" };
UBYTE Size;
UBYTE Version[80];

struct IntuiText project_shutdown = {
	0,0,JAM1,1,1,NULL,"Quit",NULL
};
struct MenuItem shutdown = {
	NULL,					//	NextItem
	0,0,					//	LeftEdge, TopEdge
	88,10,					//	Width, Height
	ITEMTEXT |				//	Flags
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						//	MutualExclude
	(APTR)&project_shutdown,//	ItemFill
	NULL,					//	SelectFill
	'Q',					//	Command
	NULL,					//	SubItem
	0						//	NextSelect
};
struct Menu project = {
	NULL,					//	NextMenu
	0,0,					//	LeftEdge, TopEdge
	72,10,					//	Width, Height
	MENUENABLED,			//	Flags
	" Server ",				//	MenuName
	&shutdown				//	MenuItem
};

FILE *user_fp;
struct B gang_master,userid_index,userlevel_index,username_index;
struct gang gang;
struct user *temp,*user,*NEW,*IRS,*SYS;
struct userid userid;
struct userlevel userlevel;
struct username username;

struct RPGserver *server,quit;
struct ServerTable ServerTable;
struct LockTable {
	UBYTE client[5];
	UBYTE entry[4][5];
	UBYTE *pending,*hold;
} LockTable[16];
UBYTE MaxClient=1,client;
int Loop=TRUE,online=0;
FILE *fp;
JDATE horolog;
UBYTE today[9],yesterday[16];

void __interrupt __saveds blink(void)
{
	static const UWORD OLD[2][8] = {
		{ 0x003,0xCCC,0x888,0xFFF,0x003,0xCCC,0x888,0xFFF },
		{ 0x003,0xCCC,0x888,0xFFF,0x003,0x888,0x003,0xCCC }
	};
	static const UWORD AGA[2][32] = {
		{ 0x000,0xA00,0x0A0,0xA60,0x00A,0xA0A,0x0AA,0xAAA,0x666,0xF00,0x0F0,0xFF0,0x00F,0xF0F,0x0FF,0xFFF,0x000,0xA00,0x0A0,0xA60,0x00A,0xA0A,0x0AA,0xAAA,0x666,0xF00,0x0F0,0xFF0,0x00F,0xF0F,0x0FF,0xFFF },
		{ 0x000,0xA00,0x0A0,0xA60,0x00A,0xA0A,0x0AA,0xAAA,0x666,0xF00,0x0F0,0xFF0,0x00F,0xF0F,0x0FF,0xFFF,0x000,0x500,0x050,0x530,0x005,0x505,0x055,0x666,0x000,0xA00,0x0A0,0xAA0,0x00A,0xA0A,0x0AA,0xAAA }
	};
	static ULONG count;

	count++;
	if(window) {
		if(count%50==0) {
			if(Depth==3)
				LoadRGB4(&window->WScreen->ViewPort,(UWORD *)OLD[0],8);
			else
			if(Depth>4)
				LoadRGB4(&window->WScreen->ViewPort,(UWORD *)AGA[0],32);
		}
		else
		if(count%50==25) {
			if(Depth==3)
				LoadRGB4(&window->WScreen->ViewPort,(UWORD *)OLD[1],8);
			else
			if(Depth>4)
				LoadRGB4(&window->WScreen->ViewPort,(UWORD *)AGA[1],32);
		}
	}
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

void UnPackString(UBYTE *result,UBYTE *str,UBYTE size)
{
	struct radix {
		UBYTE one:6;
		UBYTE two:6;
		UBYTE three:6;
		UBYTE four:6;
	} radix;
	int i,j;

	memset(result,0,size);
	for(i=0,j=0; i<size; i+=3) {
		memcpy(&radix,&str[i],3);
		result[j+0]=radix.one+' ';
		result[j+1]=radix.two+' ';
		result[j+2]=radix.three+' ';
		result[j+3]=radix.four+' ';
		j+=4;
	}
	result[j-1]=NULL;
}

UWORD CheckOne(char *str)
{
	UWORD i,result=0;

	for(i=0; i<12; i++) {
		result-=i;
		result-=(str[i]-i)*(str[23-i]-i);
	}
	return(result);
}

UWORD CheckTwo(char *str)
{
	UWORD i,result=0;

	for(i=0; i<24; i+=2) {
		result+=i;
		result+=(str[i]+i)*(str[i+1]+i);
	}

	return(result);
}

void finputs(char *str,int n,FILE *file)
{
	if(!fgets(str,n,file))
		memset(str,0,n);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
}

void Broadcast(UBYTE *str)
{
	int i;

	for(i=0; i<MaxClient; i++)
		if(strlen(LockTable[i].client) && i!=client) {
			if(LockTable[i].pending)
				LockTable[i].pending=realloc(LockTable[i].pending,strlen(LockTable[i].pending)+strlen(str)+3);
			else
				LockTable[i].pending=calloc(1,strlen(str)+3);
			strcat(LockTable[i].pending,"\15\12");
			strcat(LockTable[i].pending,str);
		}
}

void HiScore(struct user *rec)
{
	double n;
	if(*rec->ID=='_' || rec->History.Calls<10)
		return;
	rec->Rating=(5.*(1000.*rec->Wins+100.*rec->Immortal+rec->Level)
				-rec->History.Kills/5
				-rec->History.Retreats*2.
				-rec->History.Killed*3.
				+rec->History.Joust+rec->History.KO
				+rec->History.Calls)
				/rec->History.Calls;
	if(rec->Rating>=ServerTable.HiScore->Rating) {
		ServerTable.HiScore->Rating=rec->Rating;
		strncpy(ServerTable.HiScore->Who[0],rec->Handle,sizeof(rec->Handle));
	}
	if(rec->History.HP>=ServerTable.HiScore->HP) {
		ServerTable.HiScore->HP=rec->History.HP;
		strncpy(ServerTable.HiScore->Who[1],rec->Handle,sizeof(rec->Handle));
	}
	if(rec->History.Hull>=ServerTable.HiScore->Hull) {
		ServerTable.HiScore->Hull=rec->History.Hull;
		strncpy(ServerTable.HiScore->Who[2],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Kills/rec->History.Calls;
	if(n>=ServerTable.HiScore->Kills) {
		ServerTable.HiScore->Kills=n;
		strncpy(ServerTable.HiScore->Who[3],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Calls/(horolog-rec->CreateDate+1);
	if(n>=ServerTable.HiScore->Calls) {
		ServerTable.HiScore->Calls=n;
		strncpy(ServerTable.HiScore->Who[4],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Killed/rec->History.Calls;
	if(n>=ServerTable.HiScore->Killed) {
		ServerTable.HiScore->Killed=n;
		strncpy(ServerTable.HiScore->Who[5],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Retreats/rec->History.Calls;
	if(n>=ServerTable.HiScore->Retreats) {
		ServerTable.HiScore->Retreats=n;
		strncpy(ServerTable.HiScore->Who[6],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.KO/rec->History.Calls;
	if(n>=ServerTable.HiScore->KO) {
		ServerTable.HiScore->KO=n;
		strncpy(ServerTable.HiScore->Who[7],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Joust/rec->History.Calls;
	if(n>=ServerTable.HiScore->Joust) {
		ServerTable.HiScore->Joust=n;
		strncpy(ServerTable.HiScore->Who[8],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Stole/rec->History.Calls;
	if(n>=ServerTable.HiScore->Stole) {
		ServerTable.HiScore->Stole=n;
		strncpy(ServerTable.HiScore->Who[9],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Gamble/rec->History.Calls;
	if(n>=ServerTable.HiScore->Gamble) {
		ServerTable.HiScore->Gamble=n;
		strncpy(ServerTable.HiScore->Who[10],rec->Handle,sizeof(rec->Handle));
	}
	n=(double)rec->History.Fish/rec->History.Calls;
	if(n>=ServerTable.HiScore->Fish) {
		ServerTable.HiScore->Fish=n;
		strncpy(ServerTable.HiScore->Who[11],rec->Handle,sizeof(rec->Handle));
	}
}

void PutCallers(void)
{
	int i;

	if((fp=fopen("RPGBBS.callers","w"))) {
		for(i=0; i<3; i++) {
			fprintf(fp,"%s\n",ServerTable.callers->Last[i]);
			fprintf(fp,"%s\n",ServerTable.callers->Reason[i]);
		}
		fclose(fp);
	}
}

void PutHOF(void)
{
	int o,t;
	char dated[9];

	if((fp=fopen("RPGBBS.hof","w"))) {
		for(o=0; o<ServerTable.NumClass; o++)
			for(t=0; t<ServerTable.class[o]->NumCharacter; t++) {
				fprintf(fp,"%s.%s",ServerTable.class[o]->Origin,ServerTable.class[o]->Character[t]->Name);
				jdate(dated,ServerTable.class[o]->Character[t]->Calls[0].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Calls[0].Who,ServerTable.class[o]->Character[t]->Calls[0].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Kills[0].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Kills[0].Who,ServerTable.class[o]->Character[t]->Kills[0].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Killed[0].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Killed[0].Who,ServerTable.class[o]->Character[t]->Killed[0].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Retreats[0].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Retreats[0].Who,ServerTable.class[o]->Character[t]->Retreats[0].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Brawls[0].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Brawls[0].Who,ServerTable.class[o]->Character[t]->Brawls[0].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Jousts[0].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Jousts[0].Who,ServerTable.class[o]->Character[t]->Jousts[0].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->melee.When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->melee.Who,ServerTable.class[o]->Character[t]->melee.What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->blast.When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->blast.Who,ServerTable.class[o]->Character[t]->blast.What,dated);
				fprintf(fp,"\n");
			}
		fclose(fp);
	}
}

void PutHOL(void)
{
	int o,t;
	char dated[9];

	if((fp=fopen("RPGBBS.hol","w"))) {
		for(o=0; o<ServerTable.NumClass; o++)
			for(t=0; t<ServerTable.class[o]->NumCharacter; t++) {
				fprintf(fp,"%s.%s",ServerTable.class[o]->Origin,ServerTable.class[o]->Character[t]->Name);
				jdate(dated,ServerTable.class[o]->Character[t]->Calls[1].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Calls[1].Who,ServerTable.class[o]->Character[t]->Calls[1].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Kills[1].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Kills[1].Who,ServerTable.class[o]->Character[t]->Kills[1].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Killed[1].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Killed[1].Who,ServerTable.class[o]->Character[t]->Killed[1].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Retreats[1].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Retreats[1].Who,ServerTable.class[o]->Character[t]->Retreats[1].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Brawls[1].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Brawls[1].Who,ServerTable.class[o]->Character[t]->Brawls[1].What,dated);
				jdate(dated,ServerTable.class[o]->Character[t]->Jousts[1].When);
				fprintf(fp,"^%s^%u^%s",ServerTable.class[o]->Character[t]->Jousts[1].Who,ServerTable.class[o]->Character[t]->Jousts[1].What,dated);
				fprintf(fp,"\n");
			}
		fclose(fp);
	}
}

int Init(void)
{
	struct ScreenModeRequester *smr;
	struct FontRequester *fr;
	UBYTE **tool,*tooltype;
	UBYTE new,newmode[32],newdepth[16],newwidth[16],newheight[16],newfont[32],newsize[16];

	if(!(DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0L)))
		return(FALSE);
	if(!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0)))
		return(FALSE);
	if(!(DiskfontBase=(struct Library *)OpenLibrary("diskfont.library",0L)))
		return(FALSE);
	if(!(AslBase=OpenLibrary(AslName,0L)))
		return(FALSE);
	if(!(BtreeBase=(struct Library *)OpenLibrary("btree.library",0L)))
		return(FALSE);
	if(!(RPGBase=(struct Library *)OpenLibrary("rpgbbs.library",0L)))
		return(FALSE);

	pubscreen=(struct Screen *)LockPubScreen(NULL);
	if((IconBase=OpenLibrary("icon.library",0))) {
		new=FALSE;
		if((diskobj=GetDiskObject(__procname))) {
			tool=(char **)diskobj->do_ToolTypes;
			if((tooltype=(char *)FindToolType(tool,"RPG")))
				RPGBBSmode=MatchToolValue(tooltype,"BBS") ? BBS : MatchToolValue(tooltype,"DOOR") ? DOOR : FALSE;
			if((tooltype=(char *)FindToolType(tool,"MODE")))
				stch_l(tooltype,(long *)&Mode);
			if((tooltype=(char *)FindToolType(tool,"DEPTH")))
				Depth=atoi(tooltype);
			if((tooltype=(char *)FindToolType(tool,"Width")))
				Width=atoi(tooltype);
			if((tooltype=(char *)FindToolType(tool,"Height")))
				Height=atoi(tooltype);
			if((tooltype=(char *)FindToolType(tool,"FONT")))
				strcpy(Font,tooltype);
			if((tooltype=(char *)FindToolType(tool,"SIZE")))
				Size=atoi(tooltype);
			if(!Mode) {
				Mode=GetVPModeID(&(pubscreen->ViewPort));
				Depth=pubscreen->BitMap.Depth<3 ? 3 : pubscreen->BitMap.Depth;
				Width=pubscreen->Width;
				Height=pubscreen->Height;
				if((smr=AllocAslRequestTags(ASL_ScreenModeRequest,
						ASLSM_Screen, pubscreen,
						ASLSM_TitleText, "Select a ScreenMode:",
						ASLSM_InitialLeftEdge, (pubscreen->Width-260)/2,
						ASLSM_InitialTopEdge, pubscreen->BarHeight,
						ASLSM_InitialWidth, 260L,
						ASLSM_InitialHeight, (pubscreen->Height-(pubscreen->BarHeight+2))/2,
						ASLSM_InitialDisplayID, Mode,
						ASLSM_InitialDisplayDepth, Depth,
						ASLSM_InitialDisplayWidth, Width,
						ASLSM_InitialDisplayHeight, Height,
						ASLSM_DoDepth, TRUE,
						ASLSM_DoWidth, TRUE,
						ASLSM_DoHeight, TRUE,
						ASLSM_DoOverscanType, TRUE,
						ASLSM_PropertyFlags, DIPF_IS_WB,
						ASLSM_PropertyMask, DIPF_IS_FOREIGN | DIPF_IS_DUALPF | DIPF_IS_EXTRAHALFBRITE | DIPF_IS_HAM,
						ASLSM_MinDepth, 3,
						ASLSM_MaxDepth, 8,
						TAG_DONE))) {
					if(AslRequest((APTR)smr,0L)) {
						Mode=smr->sm_DisplayID;
						Depth=smr->sm_DisplayDepth;
						Width=smr->sm_DisplayWidth;
						Height=smr->sm_DisplayHeight;
						new=TRUE;
					}
					FreeAslRequest(smr);
				}
			}
			if(!strlen(Font)) {
				strcpy(Font,"IBM.font");
				Size=8;
				if((fr=AllocAslRequestTags(ASL_FontRequest,
						ASLFO_Screen, pubscreen,
						ASLFO_TitleText, "Select a Font:",
						ASLFO_InitialLeftEdge, (pubscreen->Width-260)/2,
						ASLFO_InitialTopEdge, pubscreen->BarHeight,
						ASLFO_InitialWidth, 260L,
						ASLFO_InitialHeight, (pubscreen->Height-(pubscreen->BarHeight+2))/2,
						ASLFO_InitialName, Font,
						ASLFO_InitialSize, Size,
						ASLFO_MinHeight, 8L,
						ASLFO_MaxHeight, 32L,
						ASLFO_FixedWidthOnly, TRUE,
						TAG_DONE))) {
					if(AslRequest((APTR)fr,0L)) {
						strcpy(Font,fr->fo_Attr.ta_Name);
						Size=fr->fo_Attr.ta_YSize;
						new=TRUE;
					}
					FreeAslRequest(smr);
				}
			}
			if(new) {
				sprintf(newmode,"Mode=%lX",Mode);
				sprintf(newdepth,"Depth=%d",Depth);
				sprintf(newwidth,"Width=%d",Width);
				sprintf(newheight,"Height=%d",Height);
				sprintf(newfont,"Font=%s",Font);
				sprintf(newsize,"Size=%d",Size);
				tool=calloc(8,sizeof(UBYTE *));
				tool[0]="RPG=BBS";
				tool[1]=newmode;
				tool[2]=newdepth;
				tool[3]=newwidth;
				tool[4]=newheight;
				tool[5]=newfont;
				tool[6]=newsize;
				tool[7]=NULL;
				diskobj->do_ToolTypes=tool;
				PutDiskObject(__procname,diskobj);
				free(tool);
			}
			FreeDiskObject(diskobj);
		}
		CloseLibrary(IconBase);
	}
	UnlockPubScreen(NULL,pubscreen);

	if(RPGBBSmode==BBS) {
		SERCON.TextAttr.ta_Name=Font;
		SERCON.TextAttr.ta_YSize=Size;
		SERCON.TextAttr.ta_Style=FS_NORMAL;
		SERCON.TextAttr.ta_Flags=FPF_DISKFONT;
		if(!(SERCON.TextFont=(struct TextFont *)OpenDiskFont(&SERCON.TextAttr)))
			return(FALSE);
		sprintf(Version,"RPGBBS Deluxe %s %s  *  Copyright 1995 Robert Hurst",VERSION,__AMIGADATE__);
		if(!(screen=(struct Screen *)OpenScreenTags(NULL,
					SA_DisplayID, Mode,
					SA_Depth, Depth,
					SA_Width, Width,
					SA_Height, Height,
					SA_AutoScroll, TRUE,
					SA_Pens, &DriPens[(Depth==3) ? 0 : 1],
					SA_DetailPen, 0,
					SA_BlockPen, 3,
					SA_Colors, (Depth==3) ? FourColor : SixteenColor,
					SA_Font, &SERCON.TextAttr,
					SA_Title, Version,
					SA_Type, PUBLICSCREEN,
					SA_PubName, "RPGBBS Deluxe\0",
					TAG_DONE)))
			return(FALSE);
		if((pubscreen_drawinfo=GetScreenDrawInfo(screen))) {
			if(!(window=(struct Window *)OpenWindowTags(NULL,
						WA_Top,0,WA_Left,0,
						WA_Width, Width ? Width : screen->Width,
						WA_Height, Height ? Height : screen->Height,
						WA_IDCMP, MENUPICK,
						WA_PubScreen, screen,
						WA_Activate, TRUE,
						WA_Backdrop, TRUE,
						WA_Borderless, TRUE,
						WA_NoCareRefresh, TRUE,
						WA_SmartRefresh, TRUE,
						TAG_END)))
				return(FALSE);
			SetFont(window->RPort,SERCON.TextFont);
			project.Width=9*SERCON.TextFont->tf_XSize;
			project.Height=SERCON.TextFont->tf_YSize+2;
			shutdown.Width=11*SERCON.TextFont->tf_XSize;
			shutdown.Height=SERCON.TextFont->tf_YSize+2;
			SetMenuStrip(window,&project);
		}
		PubScreenStatus(screen,0);
		interrupt=gs_add_vb_server(&blink,0);
		SERCON.Window=window;
		OpenConsole(&SERCON);
	}
	GetSystemDate(today);
	julian(&horolog,today);
	return(TRUE);
}

void Shutdown(void)
{
	if(RPGBBSmode==BBS) {
		if(interrupt)
			gs_remove_vb_server(interrupt);
		if(window) {
			ClearMenuStrip(window);
			CloseWindow(window);
		}
		if(screen)
			CloseScreen(screen);
	}
	if(BtreeBase)
		CloseLibrary((struct Library *)BtreeBase);
	if(RPGBase) {
		CloseConsole(&SERCON);
		CloseLibrary((struct Library *)RPGBase);
	}
	if(SERCON.TextFont)
		CloseFont(SERCON.TextFont);
	if(AslBase)
		CloseLibrary(AslBase);
	if(DiskfontBase)
		CloseLibrary(DiskfontBase);
	if(IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);
	if(DOSBase)
		CloseLibrary((struct Library *)DOSBase);
}

int DelGang(struct gang *gp)
{
	memcpy(&gang,gp,sizeof(gang));
	Bget(&gang_master,(char *)&gang);
	if(gang_master.Bstatus==B_SUCCESSFUL) {
		Bdelete(&gang_master,(char *)&gang);
		return(TRUE);
	}
	return(FALSE);
}

int GetGang(struct gang *gp)
{
	memcpy(&gang,gp,sizeof(gang));
	Bget(&gang_master,(char *)&gang);
	if(gang_master.Bstatus==B_SUCCESSFUL) {
		memcpy(gp,&gang,sizeof(gang));
		return(TRUE);
	}
	memset(gp,0,sizeof(gang));
	return(FALSE);
}

int PutGang(struct gang *gp)
{
	memcpy(&gang,gp,sizeof(gang));
	if(GetGang(&gang)) {
		memcpy(&gang,gp,sizeof(gang));
		Bupdate(&gang_master,(char *)&gang);
	}
	else {
		memcpy(&gang,gp,sizeof(gang));
		Bstore(&gang_master,(char *)&gang);
	}
	return(TRUE);
}

int DelUser(struct user *rec)
{
	memcpy(userid.ID,rec->ID,sizeof(rec->ID));
	userid.record=0;
	Bget(&userid_index,(char *)&userid);
	fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
	fread((char *)user,USER_RECORD_SIZE,1,user_fp);
	if(userid_index.Bstatus==B_SUCCESSFUL) {
		sprintf(userlevel.Current,"%03u",user->Level);
		memcpy(userlevel.ID,user->ID,sizeof(user->ID));
		Bget(&userlevel_index,(char *)&userlevel);
		if(userlevel_index.Bstatus==B_SUCCESSFUL)
			Bdelete(&userlevel_index,(char *)&userlevel);
		memcpy(username.Handle,user->Handle,sizeof(user->Handle));
		Bget(&username_index,(char *)&username);
		if(username_index.Bstatus==B_SUCCESSFUL)
			Bdelete(&username_index,(char *)&username);
		memset(user,0,USER_RECORD_SIZE);
		user->LastTime=SYS->LastTime;
		fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
		fwrite((char *)user,USER_RECORD_SIZE,1,user_fp);
		SYS->LastTime=userid.record;
		fseek(user_fp,0,SEEK_SET);
		fwrite((char *)SYS,USER_RECORD_SIZE,1,user_fp);
		Bdelete(&userid_index,(char *)&userid);
		fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
		fread((char *)user,USER_RECORD_SIZE,1,user_fp);
		return(TRUE);
	}
	return(FALSE);
}

int GetUser(struct user *rec)
{
	memcpy(userid.ID,rec->ID,sizeof(rec->ID));
	userid.record=0;
	Bget(&userid_index,(char *)&userid);
	if(userid_index.Bstatus) {
		memset(rec,0,sizeof(struct user));
		return(FALSE);
	}
	fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
	fread((char *)temp,USER_RECORD_SIZE,1,user_fp);
	memcpy(rec,temp,sizeof(struct user));
	return(TRUE);
}

int NextUser(struct user *rec)
{
	if(strlen(rec->ID)) {
		memcpy(userid.ID,rec->ID,sizeof(rec->ID));
		userid.record=0;
		Bget(&userid_index,(char *)&userid);
		if(userid_index.Bstatus==B_SUCCESSFUL)
			Bnext(&userid_index,(char *)&userid);
	}
	else
		Bfirst(&userid_index,(char *)&userid);
	if(userid_index.Bstatus || userid.ID[0]=='_') {
		memset(rec,0,sizeof(struct user));
		return(FALSE);
	}
	fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
	fread((char *)temp,USER_RECORD_SIZE,1,user_fp);
	memcpy(rec,temp,sizeof(struct user));
	return(TRUE);
}

int PrevUser(struct user *rec)
{
	if(strlen(rec->ID))
		memcpy(userid.ID,rec->ID,sizeof(rec->ID));
	else
		userid.ID[0]='_';
	Bget(&userid_index,(char *)&userid);
	Bprev(&userid_index,(char *)&userid);
	if(userid_index.Bstatus || userid.ID[0]=='_') {
		memset(rec,0,sizeof(struct user));
		return(FALSE);
	}
	fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
	fread((char *)temp,USER_RECORD_SIZE,1,user_fp);
	memcpy(rec,temp,sizeof(struct user));
	return(TRUE);
}

void PutUser(struct user *rec)
{
	memcpy(userid.ID,rec->ID,sizeof(rec->ID));
	userid.record=0;
	Bget(&userid_index,(char *)&userid);
	if(userid_index.Bstatus==B_SUCCESSFUL) {
		fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
		fread((char *)temp,USER_RECORD_SIZE,1,user_fp);
		fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
		fwrite((char *)rec,USER_RECORD_SIZE,1,user_fp);
		if(rec->Level!=temp->Level && *rec->ID!='_') {
			sprintf(userlevel.Current,"%03u",temp->Level);
			memcpy(userlevel.ID,temp->ID,sizeof(temp->ID));
			Bget(&userlevel_index,(char *)&userlevel);
			if(userlevel_index.Bstatus==B_SUCCESSFUL)
				Bdelete(&userlevel_index,(char *)&userlevel);
			sprintf(userlevel.Current,"%03u",rec->Level);
			memcpy(userlevel.ID,rec->ID,sizeof(rec->ID));
			Bstore(&userlevel_index,(char *)&userlevel);
		}
	}
	else {
		memcpy(userid.ID,rec->ID,sizeof(rec->ID));
		userid.record=SYS->LastTime;
		fseek(user_fp,userid.record*USER_RECORD_SIZE,userid.record ? SEEK_SET : SEEK_END);
		if(userid.record) {
			fread((char *)temp,USER_RECORD_SIZE,1,user_fp);
			SYS->LastTime=temp->LastTime;
			fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
		}
		else
			userid.record=ftell(user_fp)/USER_RECORD_SIZE;
		fwrite((char *)rec,USER_RECORD_SIZE,1,user_fp);
		fseek(user_fp,0,SEEK_SET);
		fwrite((char *)SYS,USER_RECORD_SIZE,1,user_fp);
		Bstore(&userid_index,(char *)&userid);
		if(rec->ID[0]!='_') {		//  don't include system-records
			sprintf(userlevel.Current,"%03u",rec->Level);
			memcpy(userlevel.ID,rec->ID,sizeof(rec->ID));
			Bstore(&userlevel_index,(char *)&userlevel);
			memcpy(username.Handle,rec->Handle,sizeof(rec->Handle));
			memcpy(username.ID,rec->ID,sizeof(rec->ID));
			Bstore(&username_index,(char *)&username);
		}
	}
}

int RndUser(struct user *rec)
{
	ULONG node,key;

	do {
		do {
			node=dice((userid_index.Bhp->filesize-sizeof(struct Bheader))/userid_index.Bhp->nodesize)-1;
			Bgetnode(&userid_index,node*userid_index.Bhp->nodesize+sizeof(struct Bheader),2);
		} while(*userid_index.Bnp[2]->numkeys==0);
		key=dice(*userid_index.Bnp[2]->numkeys)-1;
		memcpy(userid.ID,userid_index.Bnp[2]->key[key],sizeof(temp->ID));
		Bget(&userid_index,(char *)&userid);
		fseek(user_fp,userid.record*USER_RECORD_SIZE,SEEK_SET);
		fread((char *)temp,USER_RECORD_SIZE,1,user_fp);
		memcpy(rec,temp,sizeof(struct user));
	} while(*rec->ID=='_');
	return(TRUE);
}

UBYTE *GetFile(UBYTE *filename)
{
	UBYTE *buffer=NULL;
	int size;

	if((fp=fopen(filename,"r"))) {
		fseek(fp,0,SEEK_END);
		size=ftell(fp);
		if((buffer=calloc(1,size+2))) {
			fseek(fp,0,SEEK_SET);
			fread(buffer,size,1,fp);
			strcpy(&buffer[size],"\n");
		}
		else {
			fclose(fp);
			fp=NULL;
		}
	}
	return(buffer);
}

void LoadTables(void)
{
	int i,n,o,t;
	UWORD *checksum;
	UBYTE field[128];
	UBYTE *buffer,*c,*p,*key[4];
	char result[4][32];

	memset(LockTable,0,sizeof(LockTable));

	temp=calloc(1,USER_RECORD_SIZE);
	user=calloc(1,USER_RECORD_SIZE);
	IRS=calloc(1,USER_RECORD_SIZE);
	NEW=calloc(1,USER_RECORD_SIZE);
	SYS=calloc(1,USER_RECORD_SIZE);

	ServerTable.HiScore=calloc(1,sizeof(struct HiScore));

	ServerTable.NumAccess=0;
	ServerTable.access=NULL;
	if((buffer=GetFile("RPGBBS.access"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.access=realloc(ServerTable.access,(ServerTable.NumAccess+1)*sizeof(struct access *));
						ServerTable.access[ServerTable.NumAccess]=calloc(1,sizeof(struct access));
						sprintf(ServerTable.access[ServerTable.NumAccess]->Name,"%.15s",field);
						break;
					case 2:
						ServerTable.access[ServerTable.NumAccess]->Calls=atoi(field);
						break;
					case 3:
						ServerTable.access[ServerTable.NumAccess]->Minutes=atoi(field);
						break;
					case 4:
						ServerTable.access[ServerTable.NumAccess]->Promote=atoi(field);
						break;
					case 5:
						ServerTable.access[ServerTable.NumAccess]->Verify=toupper(field[0]);
						break;
					case 6:
						ServerTable.access[ServerTable.NumAccess]->Bulletin=toupper(field[0]);
						break;
					case 7:
						ServerTable.access[ServerTable.NumAccess]->File=toupper(field[0]);
						break;
					case 8:
						ServerTable.access[ServerTable.NumAccess]->RolePlay=toupper(field[0]);
						break;
					case 9:
						ServerTable.access[ServerTable.NumAccess]->Weekend=toupper(field[0]);
						break;
					case 10:
						ServerTable.access[ServerTable.NumAccess]->Sysop=toupper(field[0]);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumAccess++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumArmor=0;
	ServerTable.armor=NULL;
	if((buffer=GetFile("RPGBBS.armor"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						if(field[0]=='*') {
							if(ServerTable.armor)
								ServerTable.NumArmor++;
							ServerTable.armor=realloc(ServerTable.armor,(ServerTable.NumArmor+1)*sizeof(struct armor *));
							ServerTable.armor[ServerTable.NumArmor]=calloc(1,sizeof(struct armor));
							sprintf(ServerTable.armor[ServerTable.NumArmor]->Origin,"%.8s",&field[1]);
							n=0;
							break;
						}
						ServerTable.armor[ServerTable.NumArmor]->Item=realloc(ServerTable.armor[ServerTable.NumArmor]->Item,(ServerTable.armor[ServerTable.NumArmor]->NumArmor+1)*sizeof(struct armor_item *));
						ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]=calloc(1,sizeof(struct armor_item));
						sprintf(ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->Name,"%.23s",field);
						ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoMelee=-1;
						ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoMagic=-1;
						break;
					case 2:
						gold(&ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->Value,field);
						break;
					case 3:
						ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->Class=atoi(field);
						break;
					case 4:
						strcpy(ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoClass,field);
						break;
					case 5:
						if(strlen(field))
							ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoMelee=atoi(field);
						break;
					case 6:
						if(strlen(field))
							ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoMagic=atoi(field);
						break;
					case 7:
						strcpy(ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoBearer,field);
						break;
					case 8:
						strcpy(ServerTable.armor[ServerTable.NumArmor]->Item[ServerTable.armor[ServerTable.NumArmor]->NumArmor]->EgoCurse,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.armor[ServerTable.NumArmor]->NumArmor++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		if(ServerTable.armor)
			ServerTable.NumArmor++;
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumClass=0;
	ServerTable.class=NULL;
	if((buffer=GetFile("RPGBBS.class"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						if(field[0]=='*') {
							if(ServerTable.class)
								ServerTable.NumClass++;
							ServerTable.class=realloc(ServerTable.class,(ServerTable.NumClass+1)*sizeof(struct class *));
							ServerTable.class[ServerTable.NumClass]=calloc(1,sizeof(struct class));
							sprintf(ServerTable.class[ServerTable.NumClass]->Origin,"%.8s",&field[1]);
							n=0;
							break;
						}
						ServerTable.class[ServerTable.NumClass]->Character=realloc(ServerTable.class[ServerTable.NumClass]->Character,(ServerTable.class[ServerTable.NumClass]->NumCharacter+1)*sizeof(struct class_item *));
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]=calloc(1,sizeof(struct class_character));
						sprintf(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Name,"%.9s",field);
						for(i=0; i<2; i++) {
							strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Calls[i].Who,"-nobody-");
							ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Calls[i].When=horolog;
							strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Kills[i].Who,"-nobody-");
							ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Kills[i].When=horolog;
							strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Killed[i].Who,"-nobody-");
							ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Killed[i].When=horolog;
							strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Retreats[i].Who,"-nobody-");
							ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Retreats[i].When=horolog;
							strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Brawls[i].Who,"-nobody-");
							ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Brawls[i].When=horolog;
							strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Jousts[i].Who,"-nobody-");
							ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Jousts[i].When=horolog;
						}
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Calls[0].What=999;
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Kills[0].What=999;
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Killed[0].What=999;
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Retreats[0].What=999;
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Brawls[1].What=0;
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Jousts[1].What=0;
						strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->melee.Who,"-nobody-");
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->melee.When=horolog;
						strcpy(ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->blast.Who,"-nobody-");
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->blast.When=horolog;
						break;
					case 2:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Melee=atoi(field);
						break;
					case 3:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Backstab=atoi(field);
						break;
					case 4:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Poison=atoi(field);
						break;
					case 5:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Magic=atoi(field);
						break;
					case 6:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->Steal=atoi(field);
						break;
					case 7:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BaseSTR=atoi(field);
						break;
					case 8:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BaseINT=atoi(field);
						break;
					case 9:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BaseDEX=atoi(field);
						break;
					case 10:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BaseCHA=atoi(field);
						break;
					case 11:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BonusSTR=atoi(field);
						break;
					case 12:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BonusINT=atoi(field);
						break;
					case 13:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BonusDEX=atoi(field);
						break;
					case 14:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->BonusCHA=atoi(field);
						break;
					case 15:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->MaxSTR=atoi(field);
						break;
					case 16:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->MaxINT=atoi(field);
						break;
					case 17:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->MaxDEX=atoi(field);
						break;
					case 18:
						ServerTable.class[ServerTable.NumClass]->Character[ServerTable.class[ServerTable.NumClass]->NumCharacter]->MaxCHA=atoi(field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.class[ServerTable.NumClass]->NumCharacter++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		if(ServerTable.class)
			ServerTable.NumClass++;
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumMagic=0;
	ServerTable.magic=NULL;
	if((buffer=GetFile("RPGBBS.magic"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.magic=realloc(ServerTable.magic,(ServerTable.NumMagic+1)*sizeof(struct magic *));
						ServerTable.magic[ServerTable.NumMagic]=calloc(1,sizeof(struct magic));
						sprintf(ServerTable.magic[ServerTable.NumMagic]->Name,"%.19s",field);
						break;
					case 2:
						ServerTable.magic[ServerTable.NumMagic]->Power[0]=atoi(field);
						break;
					case 3:
						ServerTable.magic[ServerTable.NumMagic]->Power[1]=atoi(field);
						break;
					case 4:
						gold(&ServerTable.magic[ServerTable.NumMagic]->Cost[0],field);
						break;
					case 5:
						gold(&ServerTable.magic[ServerTable.NumMagic]->Cost[1],field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumMagic++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumPoison=0;
	ServerTable.poison=NULL;
	if((buffer=GetFile("RPGBBS.poison"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.poison=realloc(ServerTable.poison,(ServerTable.NumPoison+1)*sizeof(struct poison *));
						ServerTable.poison[ServerTable.NumPoison]=calloc(1,sizeof(struct poison));
						sprintf(ServerTable.poison[ServerTable.NumPoison]->Name,"%.19s",field);
						break;
					case 2:
						gold(&ServerTable.poison[ServerTable.NumPoison]->Cost[0],field);
						break;
					case 3:
						gold(&ServerTable.poison[ServerTable.NumPoison]->Cost[1],field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumPoison++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumRealEstate=0;
	ServerTable.realestate=NULL;
	if((buffer=GetFile("RPGBBS.realestate"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.realestate=realloc(ServerTable.realestate,(ServerTable.NumRealEstate+1)*sizeof(struct realestate *));
						ServerTable.realestate[ServerTable.NumRealEstate]=calloc(1,sizeof(struct realestate));
						sprintf(ServerTable.realestate[ServerTable.NumRealEstate]->Name,"%.19s",field);
						break;
					case 2:
						gold(&ServerTable.realestate[ServerTable.NumRealEstate]->Value,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumRealEstate++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumSecurity=0;
	ServerTable.security=NULL;
	if((buffer=GetFile("RPGBBS.security"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.security=realloc(ServerTable.security,(ServerTable.NumSecurity+1)*sizeof(struct security *));
						ServerTable.security[ServerTable.NumSecurity]=calloc(1,sizeof(struct security));
						sprintf(ServerTable.security[ServerTable.NumSecurity]->Name,"%.19s",field);
						break;
					case 2:
						gold(&ServerTable.security[ServerTable.NumSecurity]->Value,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumSecurity++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumWeapon=0;
	ServerTable.weapon=NULL;
	if((buffer=GetFile("RPGBBS.weapon"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						if(field[0]=='*') {
							if(ServerTable.weapon)
								ServerTable.NumWeapon++;
							ServerTable.weapon=realloc(ServerTable.weapon,(ServerTable.NumWeapon+1)*sizeof(struct weapon *));
							ServerTable.weapon[ServerTable.NumWeapon]=calloc(1,sizeof(struct weapon));
							sprintf(ServerTable.weapon[ServerTable.NumWeapon]->Origin,"%.8s",&field[1]);
							n=0;
							break;
						}
						ServerTable.weapon[ServerTable.NumWeapon]->Item=realloc(ServerTable.weapon[ServerTable.NumWeapon]->Item,(ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon+1)*sizeof(struct weapon_item *));
						ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]=calloc(1,sizeof(struct weapon_item));
						sprintf(ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->Name,"%.23s",field);
						ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoMelee=-1;
						ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoMagic=-1;
						break;
					case 2:
						gold(&ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->Value,field);
						break;
					case 3:
						ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->Class=atoi(field);
						break;
					case 4:
						ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->Action=malloc(strlen(field)+1);
						strcpy(ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->Action,field);
						break;
					case 5:
						strcpy(ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoClass,field);
						break;
					case 6:
						if(strlen(field))
							ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoMelee=atoi(field);
						break;
					case 7:
						if(strlen(field))
							ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoMagic=atoi(field);
						break;
					case 8:
						strcpy(ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoBearer,field);
						break;
					case 9:
						strcpy(ServerTable.weapon[ServerTable.NumWeapon]->Item[ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon]->EgoCurse,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.weapon[ServerTable.NumWeapon]->NumWeapon++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		if(ServerTable.weapon)
			ServerTable.NumWeapon++;
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumArena=0;
	ServerTable.arena=NULL;
	if((buffer=GetFile("RPGBBS.arena"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.arena=realloc(ServerTable.arena,(ServerTable.NumArena+1)*sizeof(struct arena *));
						ServerTable.arena[ServerTable.NumArena]=calloc(1,sizeof(struct arena));
						sprintf(ServerTable.arena[ServerTable.NumArena]->Name,"%.19s",field);
						break;
					case 2:
						sprintf(ServerTable.arena[ServerTable.NumArena]->Class,"%.18s",field);
						break;
					case 3:
						ServerTable.arena[ServerTable.NumArena]->Level=atoi(field);
						break;
					case 4:
						sprintf(ServerTable.arena[ServerTable.NumArena]->Weapon,"%.33s",field);
						break;
					case 5:
						sprintf(ServerTable.arena[ServerTable.NumArena]->Armor,"%.33s",field);
						break;
					case 6:
						gold(&ServerTable.arena[ServerTable.NumArena]->Gold,field);
						break;
					case 7:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=HEAL_SPELL;
						break;
					case 8:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=TELEPORT_SPELL;
						break;
					case 9:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=BLAST_SPELL;
						break;
					case 10:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=CONFUSION_SPELL;
						break;
					case 11:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=TRANSMUTE_SPELL;
						break;
					case 12:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=CURE_SPELL;
						break;
					case 13:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=ILLUSION_SPELL;
						break;
					case 14:
						if(field[0]=='Y')
							ServerTable.arena[ServerTable.NumArena]->Spell|=DISINTEGRATE_SPELL;
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumArena++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumDungeon=0;
	ServerTable.dungeon=NULL;
	if((buffer=GetFile("RPGBBS.dungeon"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.dungeon=realloc(ServerTable.dungeon,(ServerTable.NumDungeon+1)*sizeof(struct dungeon *));
						ServerTable.dungeon[ServerTable.NumDungeon]=calloc(1,sizeof(struct dungeon));
						sprintf(ServerTable.dungeon[ServerTable.NumDungeon]->Name,"%.19s",field);
						break;
					case 2:
						sprintf(ServerTable.dungeon[ServerTable.NumDungeon]->Class,"%.18s",field);
						break;
					case 3:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->Spell|=HEAL_SPELL;
						break;
					case 4:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->Spell|=TELEPORT_SPELL;
						break;
					case 5:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->Spell|=BLAST_SPELL;
						break;
					case 6:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->XSpell|=ARMOR_RUSTING_XSPELL;
						break;
					case 7:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->XSpell|=WEAPON_DECAY_XSPELL;
						break;
					case 8:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->XSpell|=BIG_BLAST_XSPELL;
						break;
					case 9:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->XSpell|=MANA_STEALING_XSPELL;
						break;
					case 10:
						if(field[0]=='Y')
							ServerTable.dungeon[ServerTable.NumDungeon]->XSpell|=LIFE_STEALING_XSPELL;
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumDungeon++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.NumNaval=0;
	ServerTable.naval=NULL;
	if((buffer=GetFile("RPGBBS.naval"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						ServerTable.naval=realloc(ServerTable.naval,(ServerTable.NumNaval+1)*sizeof(struct naval *));
						ServerTable.naval[ServerTable.NumNaval]=calloc(1,sizeof(struct naval));
						sprintf(ServerTable.naval[ServerTable.NumNaval]->Name,"%.19s",field);
						break;
					case 2:
						ServerTable.naval[ServerTable.NumNaval]->INT=atoi(field);
						break;
					case 3:
						ServerTable.naval[ServerTable.NumNaval]->Hull=atoi(field);
						break;
					case 4:
						ServerTable.naval[ServerTable.NumNaval]->Shot=atoi(field);
						break;
					case 5:
						ServerTable.naval[ServerTable.NumNaval]->Power=atoi(field);
						break;
					case 6:
						ServerTable.naval[ServerTable.NumNaval]->Ram=toupper(field[0]);
						break;
					case 7:
						gold(&ServerTable.naval[ServerTable.NumNaval]->Value,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^') {
					ServerTable.NumNaval++;
					n=0;
				}
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}

	ServerTable.callers=calloc(1,sizeof(struct callers));
	if((fp=fopen("RPGBBS.callers","r"))) {
		for(i=0; i<3; i++) {
			finputs(field,sizeof(field),fp);
			strcpy(ServerTable.callers->Last[i],field);
			finputs(field,sizeof(field),fp);
			strcpy(ServerTable.callers->Reason[i],field);
		}
		fclose(fp);
	}

	if(!(user_fp=fopen("USER.master","r+"))) {
		user_fp=fopen("USER.master","w+");
		strcpy(SYS->ID,"_SYS");
		strcpy(SYS->Handle,"-sysop handle-");
		strcpy(SYS->Name,"Hack & Slash");
		SYS->BirthDate=horolog;
		SYS->CreateDate=horolog;
		SYS->ExpireDate=horolog+91;
		SYS->Level=33;
		fwrite((char *)SYS,USER_RECORD_SIZE,1,user_fp);
	}

	fseek(user_fp,0,SEEK_SET);
	fread((char *)SYS,USER_RECORD_SIZE,1,user_fp);
	if(SYS->Expert) {
		Bopen(&userid_index,"USERID.index");
		if(userid_index.Bstatus)
			SYS->Expert=FALSE;
		else {
			Bopen(&userlevel_index,"USERLEVEL.index");
			if(userlevel_index.Bstatus) {
				Bclose(&userid_index);
				SYS->Expert=FALSE;
			}
			else {
				Bopen(&username_index,"USERNAME.index");
				if(username_index.Bstatus) {
					Bclose(&userid_index);
					Bclose(&userlevel_index);
					SYS->Expert=FALSE;
				}
			}
		}
	}
	if(!SYS->Expert) {
		Bcreate(&userid_index,"USERID.index",sizeof(struct userid));
		Bopen(&userid_index,"USERID.index");
		strcpy(userid.ID,"_SYS");
		userid.record=0;
		Bstore(&userid_index,(char *)&userid);
		Bcreate(&userlevel_index,"USERLEVEL.index",sizeof(struct userlevel));
		Bopen(&userlevel_index,"USERLEVEL.index");
		Bcreate(&username_index,"USERNAME.index",sizeof(struct username));
		Bopen(&username_index,"USERNAME.index");
	}
	for(n=1; fread((char *)user,USER_RECORD_SIZE,1,user_fp); n++) {
		if(strlen(user->ID)) {
			if(!SYS->Expert) {
				memcpy(userid.ID,user->ID,sizeof(user->ID));
				userid.record=n;
				Bstore(&userid_index,(char *)&userid);
			}
			if(*user->ID!='_') {
				if(!SYS->Expert) {
					sprintf(userlevel.Current,"%03u",user->Level);
					memcpy(userlevel.ID,user->ID,sizeof(user->ID));
					Bstore(&userlevel_index,(char *)&userlevel);
					memcpy(username.Handle,user->Handle,sizeof(user->Handle));
					memcpy(username.ID,user->ID,sizeof(user->ID));
					Bstore(&username_index,(char *)&username);
				}
				HiScore(user);
			}
		}
	}

	strcpy(NEW->ID,"_NEW");
	if(!GetUser(NEW)) {
		strcpy(NEW->ID,"_NEW");
		strcpy(NEW->Handle,"New User Record");
		NEW->ExpireDate=365;
		NEW->Access=1;
		NEW->Emulation=1;
		NEW->Rows=24;
		strmfe(NEW->Class,ServerTable.class[0]->Origin,ServerTable.class[0]->Character[0]->Name);
		NEW->Level=1;
		NEW->ExpLevel=1;
		NEW->Gold=1.;
		NEW->Bank=5.;
		NEW->HP=15;
		NEW->SP=15;
		NEW->STR=50;
		NEW->INT=50;
		NEW->DEX=50;
		NEW->CHA=50;
		strmfe(NEW->Weapon,ServerTable.weapon[0]->Origin,ServerTable.weapon[0]->Item[1]->Name);
		strmfe(NEW->Armor,ServerTable.armor[0]->Origin,ServerTable.armor[0]->Item[1]->Name);
		NEW->Ram='N';
		NEW->Posts=1;
		NEW->Uploads=1;
		NEW->UpByte=0;
		NEW->FileRatio=10;
		NEW->ByteRatio=0;
		NEW->Novice='Y';
		PutUser(NEW);
	}

	strcpy(IRS->ID,"_IRS");
	if(!GetUser(IRS)) {
		strcpy(IRS->ID,"_IRS");
		strcpy(IRS->Handle,"Irwin R. Scheister");
		strcpy(IRS->Class,"IMMORTAL.Paladin");
		IRS->Sex='M';
		IRS->Gender='M';
		IRS->CreateDate=horolog;
		IRS->Posts=10;
		PutUser(IRS);
	}

	Bopen(&gang_master,"GANG.master");
	if(gang_master.Bstatus) {
		Bcreate(&gang_master,"GANG.master",sizeof(struct gang));
		Bopen(&gang_master,"GANG.master");
		memset(&gang,0,sizeof(gang));
		strcpy(gang.Name,"Monster Mash");
		for(i=0; i<4; i++)
			sprintf(gang.MemberID[i],"_MM%d",i+1);
		gang.BannerType=0;
		gang.BannerFore=BLK;
		gang.BannerBack=GRN;
		gang.BannerTrim=BLACK;
		Bstore(&gang_master,(char *)&gang);
	}
	Bfirst(&gang_master,(char *)&gang);
	while(!gang_master.Bstatus) {
		if(gang.MemberID[0][0]!='_') {
			strcpy(userid.ID,gang.MemberID[0]);
			Bget(&userid_index,(char *)&userid);
			if(userid_index.Bstatus) {
				Bdelete(&gang_master,(char *)&gang);
				for(i=1; i<4; i++) {
					strcpy(user->ID,gang.MemberID[i]);
					if(GetUser(user)) {
						memset(user->Gang,0,sizeof(user->Gang));
						PutUser(user);
					}
				}
			}
			else
				Bnext(&gang_master,(char *)&gang);
		}
		else
			Bnext(&gang_master,(char *)&gang);
	}

	strcpy(SYS->ID,"_SYS");
	GetUser(SYS);
	SYS->Expert=FALSE;
	if((buffer=GetFile("RPGBBS.license"))) {
		if((p=strchr(buffer,'\15'))) {
			*p=NULL;p++;
			if(SERCON.Window) {
				plot(&SERCON,4,17);
				SendPXL(&SERCON,buffer);
			}
			n=TRUE;
			memset(result,0,sizeof(result));
			for(i=0; i<4; i++) {
				key[i]=p;p+=24;
				UnPackString(result[i],key[i],24);
			}
			checksum=(UWORD *)p;p+=sizeof(UWORD);
			if(*checksum!=CheckOne(key[0]))
				n=FALSE;
			checksum=(UWORD *)p;p+=sizeof(UWORD);
			if(*checksum!=CheckTwo(key[1]))
				n=FALSE;
			checksum=(UWORD *)p;p+=sizeof(UWORD);
			if(*checksum!=CheckOne(key[2]))
				n=FALSE;
			checksum=(UWORD *)p;
			if(*checksum!=CheckTwo(key[3]))
				n=FALSE;
			if(n) {
				for(i=0; i<4; i++) {
					proper(result[i]);
					if(SERCON.Window) {
						plot(&SERCON,11+i,40-strlen(result[i])/2);
						WriteConsole(&SERCON,result[i]);
					}
				}
				SYS->ExpireDate=horolog+365;
				SYS->Level=99;
				PutUser(SYS);
				MaxClient=16;
			}
			else {
				SYS->ExpireDate=horolog-1;
				SYS->Level=33;
				PutUser(SYS);
			}
		}
		free(buffer);
		fclose(fp);
	}
	if(horolog>SYS->ExpireDate)
		Loop=FALSE;

	if((buffer=GetFile("RPGBBS.hof"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						strsfn(field,NULL,NULL,result[0],result[1]);
						for(o=ServerTable.NumClass-1; o>0 && stricmp(result[0],ServerTable.class[o]->Origin); o--);
						for(t=ServerTable.class[o]->NumCharacter-1; t>0 && stricmp(result[1],ServerTable.class[o]->Character[t]->Name); t--);
						break;
					case 2:
						strcpy(ServerTable.class[o]->Character[t]->Calls[0].Who,field);
						break;
					case 3:
						ServerTable.class[o]->Character[t]->Calls[0].What=atoi(field);
						break;
					case 4:
						julian(&ServerTable.class[o]->Character[t]->Calls[0].When,field);
						break;
					case 5:
						strcpy(ServerTable.class[o]->Character[t]->Kills[0].Who,field);
						break;
					case 6:
						ServerTable.class[o]->Character[t]->Kills[0].What=atoi(field);
						break;
					case 7:
						julian(&ServerTable.class[o]->Character[t]->Kills[0].When,field);
						break;
					case 8:
						strcpy(ServerTable.class[o]->Character[t]->Killed[0].Who,field);
						break;
					case 9:
						ServerTable.class[o]->Character[t]->Killed[0].What=atoi(field);
						break;
					case 10:
						julian(&ServerTable.class[o]->Character[t]->Killed[0].When,field);
						break;
					case 11:
						strcpy(ServerTable.class[o]->Character[t]->Retreats[0].Who,field);
						break;
					case 12:
						ServerTable.class[o]->Character[t]->Retreats[0].What=atoi(field);
						break;
					case 13:
						julian(&ServerTable.class[o]->Character[t]->Retreats[0].When,field);
						break;
					case 14:
						strcpy(ServerTable.class[o]->Character[t]->Brawls[0].Who,field);
						break;
					case 15:
						ServerTable.class[o]->Character[t]->Brawls[0].What=atoi(field);
						break;
					case 16:
						julian(&ServerTable.class[o]->Character[t]->Brawls[0].When,field);
						break;
					case 17:
						strcpy(ServerTable.class[o]->Character[t]->Jousts[0].Who,field);
						break;
					case 18:
						ServerTable.class[o]->Character[t]->Jousts[0].What=atoi(field);
						break;
					case 19:
						julian(&ServerTable.class[o]->Character[t]->Jousts[0].When,field);
						break;
					case 20:
						strcpy(ServerTable.class[o]->Character[t]->melee.Who,field);
						break;
					case 21:
						ServerTable.class[o]->Character[t]->melee.What=atoi(field);
						break;
					case 22:
						julian(&ServerTable.class[o]->Character[t]->melee.When,field);
						break;
					case 23:
						strcpy(ServerTable.class[o]->Character[t]->blast.Who,field);
						break;
					case 24:
						ServerTable.class[o]->Character[t]->blast.What=atoi(field);
						break;
					case 25:
						julian(&ServerTable.class[o]->Character[t]->blast.When,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^')
					n=0;
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}
	else
		PutHOF();
	if((buffer=GetFile("RPGBBS.hol"))) {
		memset(field,0,sizeof(field));
		n=0;
		for(c=buffer; *c; c++) {
			if(*c=='^' || *c=='\n' || *c==';') {
				if(*c=='^' || strlen(field))
					n++;
				for(i=strlen(field)-1; i>0 && isspace(field[i]); i--)
					field[i]=NULL;
				switch(n) {
					case 1:
						strsfn(field,NULL,NULL,result[0],result[1]);
						for(o=ServerTable.NumClass-1; o>0 && stricmp(result[0],ServerTable.class[o]->Origin); o--);
						for(t=ServerTable.class[o]->NumCharacter-1; t>0 && stricmp(result[1],ServerTable.class[o]->Character[t]->Name); t--);
						break;
					case 2:
						strcpy(ServerTable.class[o]->Character[t]->Calls[1].Who,field);
						break;
					case 3:
						ServerTable.class[o]->Character[t]->Calls[1].What=atoi(field);
						break;
					case 4:
						julian(&ServerTable.class[o]->Character[t]->Calls[1].When,field);
						break;
					case 5:
						strcpy(ServerTable.class[o]->Character[t]->Kills[1].Who,field);
						break;
					case 6:
						ServerTable.class[o]->Character[t]->Kills[1].What=atoi(field);
						break;
					case 7:
						julian(&ServerTable.class[o]->Character[t]->Kills[1].When,field);
						break;
					case 8:
						strcpy(ServerTable.class[o]->Character[t]->Killed[1].Who,field);
						break;
					case 9:
						ServerTable.class[o]->Character[t]->Killed[1].What=atoi(field);
						break;
					case 10:
						julian(&ServerTable.class[o]->Character[t]->Killed[1].When,field);
						break;
					case 11:
						strcpy(ServerTable.class[o]->Character[t]->Retreats[1].Who,field);
						break;
					case 12:
						ServerTable.class[o]->Character[t]->Retreats[1].What=atoi(field);
						break;
					case 13:
						julian(&ServerTable.class[o]->Character[t]->Retreats[1].When,field);
						break;
					case 14:
						strcpy(ServerTable.class[o]->Character[t]->Brawls[1].Who,field);
						break;
					case 15:
						ServerTable.class[o]->Character[t]->Brawls[1].What=atoi(field);
						break;
					case 16:
						julian(&ServerTable.class[o]->Character[t]->Brawls[1].When,field);
						break;
					case 17:
						strcpy(ServerTable.class[o]->Character[t]->Jousts[1].Who,field);
						break;
					case 18:
						ServerTable.class[o]->Character[t]->Jousts[1].What=atoi(field);
						break;
					case 19:
						julian(&ServerTable.class[o]->Character[t]->Jousts[1].When,field);
						break;
				}
				memset(field,0,sizeof(field));
				if(n>0 && *c!='^')
					n=0;
				if(*c==';')
					while(*c!='\n' && *c!=NULL)
						c++;
			}
			else
				if(strlen(field) || !isspace(*c))
					field[strlen(field)]=*c;
		}
		free(buffer);
		fclose(fp);
	}
	else
		PutHOL();
	ServerTable.IRS=IRS;
	ServerTable.NEW=NEW;
	ServerTable.SYS=SYS;
}
	
void UnloadTables(void)
{
	int i,j;

	PutCallers();
	DeletePort(port);

	SYS->Expert=TRUE;
	fseek(user_fp,0,SEEK_SET);
	fwrite((char *)SYS,USER_RECORD_SIZE,1,user_fp);
	fclose(user_fp);
	Bclose(&userid_index);
	Bclose(&userlevel_index);
	Bclose(&username_index);

	Bclose(&gang_master);

	for(i=0; i<ServerTable.NumAccess; i++)
		free(ServerTable.access[i]);
	free(ServerTable.access);

	for(i=0; i<ServerTable.NumArmor; i++) {
		for(j=0; j<ServerTable.armor[i]->NumArmor; j++)
			free(ServerTable.armor[i]->Item[j]);
		free(ServerTable.armor[i]->Item);
		free(ServerTable.armor[i]);
	}
	free(ServerTable.armor);

	for(i=0; i<ServerTable.NumClass; i++) {
		for(j=0; j<ServerTable.class[i]->NumCharacter; j++)
			free(ServerTable.class[i]->Character[j]);
		free(ServerTable.class[i]->Character);
		free(ServerTable.class[i]);
	}
	free(ServerTable.armor);

	for(i=0; i<ServerTable.NumArena; i++)
		free(ServerTable.arena[i]);
	free(ServerTable.arena);

	for(i=0; i<ServerTable.NumDungeon; i++)
		free(ServerTable.dungeon[i]);
	free(ServerTable.dungeon);

	for(i=0; i<ServerTable.NumNaval; i++)
		free(ServerTable.naval[i]);
	free(ServerTable.naval);

	for(i=0; i<ServerTable.NumPoison; i++)
		free(ServerTable.poison[i]);
	free(ServerTable.poison);

	for(i=0; i<ServerTable.NumRealEstate; i++)
		free(ServerTable.realestate[i]);
	free(ServerTable.realestate);

	for(i=0; i<ServerTable.NumSecurity; i++)
		free(ServerTable.security[i]);
	free(ServerTable.security);

	for(i=0; i<ServerTable.NumWeapon; i++) {
		for(j=0; j<ServerTable.weapon[i]->NumWeapon; j++) {
			free(ServerTable.weapon[i]->Item[j]->Action);
			free(ServerTable.weapon[i]->Item[j]);
		}
		free(ServerTable.weapon[i]->Item);
		free(ServerTable.weapon[i]);
	}
	free(ServerTable.weapon);

	free(ServerTable.callers);

	free(ServerTable.HiScore);

	free(SYS);
	free(NEW);
	free(IRS);
	free(user);
	free(temp);

	for(client=0; client<MaxClient; client++) {
		if(LockTable[client].pending)
			free(LockTable[client].pending);
		if(LockTable[client].hold)
			free(LockTable[client].hold);
	}
}

void QuitServer(void)
{
	struct IntuiText Really = {
		0,1,JAM2,10,10,NULL,"Really shutdown server?",NULL
	};
	struct IntuiText Warning = {
		0,1,JAM2,10,10,NULL,"Client(s) still active!",NULL
	};
	struct IntuiText Yes = {
		2,3,JAM2,5,3,NULL,"Shutdown",NULL
	};
	struct IntuiText No = {
		2,3,JAM2,5,3,NULL,"Cancel",NULL
	};

	if(online) {
		DisplayBeep(NULL);
		if(AutoRequest(NULL,&Warning,&Yes,&No,0L,0L,320L,60L))
			Loop=FALSE;
	}
	else {
		if(AutoRequest(NULL,&Really,&Yes,&No,0L,0L,320L,60L))
			Loop=FALSE;
	}
}

void ProcessConsoleSignal(void)
{
	struct IntuiMessage *message;
	ULONG MessageClass;
	UWORD code,menu,menuitem;

	while((message=(struct IntuiMessage *)GetMsg(window->UserPort))) {
		MessageClass=message->Class;
		code=message->Code;
		ReplyMsg((struct Message *)message);
		if(MessageClass & MENUPICK) {
			menu=MENUNUM(code);
			menuitem=ITEMNUM(code);
			switch(menu) {
				case 0:		//	Server
					switch(menuitem) {
						case 0:		//	Quit
							QuitServer();
							break;
					}
					break;
			}
		}
	}
}

void ProcessServerSignal(void)
{
	int i,j,rec;

	while((server=(struct RPGserver *)GetMsg(port))) {
		for(client=0; client<MaxClient; client++)
			if(!stricmp(LockTable[client].client,server->client)) {
				if(LockTable[client].hold) {
					free(LockTable[client].hold);
					LockTable[client].hold=NULL;
				}
				break;
			}
		switch(server->command) {
			case SERVER_SHUTDOWN:
				QuitServer();
				break;
			case SERVER_CONNECT:
				if(!strlen(server->client)) {	//	Client-startup connect
					if(online>=MaxClient) {
						*server->result=FALSE;
						break;
					}
					online++;
				}
				if(client==MaxClient)
					for(client=0; client<MaxClient && strlen(LockTable[client].client); client++);
				if(client<MaxClient) {
					server->parameter=(UBYTE *)&ServerTable;
					*server->result=TRUE;
					memset(&LockTable[client],0,sizeof(struct LockTable));
					strcpy(LockTable[client].client,server->client);
				}
				break;
			case SERVER_DISCONNECT:
				if(!strlen(server->client) && online>0) {	//  Client-shutdown disconnect
					online--;
					break;
				}
				if(client<MaxClient) {
					if(LockTable[client].pending)
						free(LockTable[client].pending);
					if(LockTable[client].hold)
						free(LockTable[client].hold);
					memset(&LockTable[client],0,sizeof(struct LockTable));
				}
				break;
			case SERVER_BROADCAST:
				Broadcast(server->parameter);
				break;
			case SERVER_DELUSER:
				*server->result=DelUser((struct user *)server->parameter);
				break;
			case SERVER_GETUSER:
				*server->result=GetUser((struct user *)server->parameter);
				break;
			case SERVER_NEXTUSER:
				*server->result=NextUser((struct user *)server->parameter);
				break;
			case SERVER_PREVUSER:
				*server->result=PrevUser((struct user *)server->parameter);
				break;
			case SERVER_PUTUSER:
				memset(user,0,USER_RECORD_SIZE);
				if(((struct user *)server->parameter)->Gold<1.)
					((struct user *)server->parameter)->Gold=0.;
				if(((struct user *)server->parameter)->Gold>=1e22)
					((struct user *)server->parameter)->Gold=1e22-1.;
				if(((struct user *)server->parameter)->Bank<1.)
					((struct user *)server->parameter)->Bank=0.;
				if(((struct user *)server->parameter)->Bank>=1e22)
					((struct user *)server->parameter)->Bank=1e22-1.;
				if(((struct user *)server->parameter)->Loan<1.)
					((struct user *)server->parameter)->Loan=0.;
				if(((struct user *)server->parameter)->Loan>=1e22)
					((struct user *)server->parameter)->Loan=1e22-1.;
				memcpy(user,server->parameter,sizeof(struct user));
				HiScore(user);
				PutUser(user);
				*server->result=TRUE;
				break;
			case SERVER_RNDUSER:
				*server->result=RndUser((struct user *)server->parameter);
				break;
			case SERVER_GETUSERLEVEL:
				Bget(&userlevel_index,server->parameter);
				Bprev(&userlevel_index,server->parameter);
				*server->result=(userlevel_index.Bstatus==B_END_OF_FILE) ? FALSE : TRUE;
				break;
			case SERVER_GETUSERNAME:
				Bget(&username_index,server->parameter);
				*server->result=(username_index.Bstatus) ? FALSE : TRUE;
				break;
			case SERVER_LOCK:
				*server->result=TRUE;
				rec=1;
				for(i=0; i<MaxClient; i++) {
					if(!stricmp(LockTable[i].client,server->parameter)) {
						rec=(i==client) ? -1 : 0;
						break;
					}
					for(j=0; j<4; j++)
						if(!stricmp(LockTable[i].entry[j],server->parameter))
							rec=0;
				}
				if(rec>0) {
					if(client<MaxClient)
						for(i=0; i<4; i++)
							if(!strlen(LockTable[client].entry[i])) {
								strcpy(LockTable[client].entry[i],server->parameter);
								break;
							}
				}
				if(!rec)
					*server->result=FALSE;
				break;
			case SERVER_UNLOCK:
				*server->result=NULL;
				if(client<MaxClient) {
					if(LockTable[client].pending) {
						server->result=LockTable[client].pending;
						LockTable[client].hold=LockTable[client].pending;
					}
					LockTable[client].pending=NULL;
					memset(&LockTable[client].entry,0,sizeof(LockTable[client].entry));
				}
				break;
			case SERVER_DELGANG:
				*server->result=DelGang((struct gang *)server->parameter);
				break;
			case SERVER_GETGANG:
				*server->result=GetGang((struct gang *)server->parameter);
				break;
			case SERVER_NEXTGANG:
				Bget(&gang_master,server->parameter);
				if(!gang_master.Bstatus)
					Bnext(&gang_master,server->parameter);
				*server->result=(gang_master.Bstatus==B_END_OF_FILE) ? FALSE : TRUE;
				break;
			case SERVER_PREVGANG:
				Bget(&gang_master,server->parameter);
				Bprev(&gang_master,server->parameter);
				*server->result=(gang_master.Bstatus==B_END_OF_FILE) ? FALSE : TRUE;
				break;
			case SERVER_PUTGANG:
				*server->result=PutGang((struct gang *)server->parameter);
				break;
			case SERVER_PUTHOF:
				PutHOF();
				*server->result=TRUE;
				break;
			case SERVER_PUTHOL:
				PutHOL();
				*server->result=TRUE;
				break;
			case SERVER_WAKEUP:
				*server->result=TRUE;
				break;
			default:
				server->result="?Invalid server command";
				break;
		}
		ReplyMsg((struct Message *)server);
	}
}

void main(int argc,char **args)
{
	ULONG ConsoleSignal,ServerSignal,signal;
	long root;
	int i;

	if(argc) {	//  CLI-launched
		for(i=1; argc>1; i++, argc--) {
			if(!stricmp(args[i],"Q") || !stricmp(args[i],"QUIT")) {
				if((port=(struct MsgPort *)FindPort("RPGBBS Deluxe Server"))) {
					quit.message.mn_Node.ln_Type=NT_MESSAGE;
					quit.message.mn_Length=sizeof(quit);
					quit.message.mn_ReplyPort=NULL;
					quit.command=SERVER_SHUTDOWN;
					PutMsg(port,(struct Message *)&quit);
				}
				else
					DisplayBeep(NULL);
				exit(0);
			}
		}
	}
	else {	//  Workbench-launched
		WBenchMsg=(struct WBStartup *)args;
		for(i=0,wbarg=WBenchMsg->sm_ArgList; i<WBenchMsg->sm_NumArgs; i++,wbarg++)
			if(wbarg->wa_Lock && *wbarg->wa_Name)
				CurrentDir(wbarg->wa_Lock);
	}

	if(!Init()) {
		Shutdown();
		exit(1);
	}

	GetSystemDate(yesterday);
	julian(&horolog,yesterday);
	if((lockdir=Lock("Notices",ACCESS_READ))) {
		root=CurrentDir(lockdir);
		if((lockfile=Lock("Today",ACCESS_READ))) {
			if((Examine(lockfile,&infoBlock))) {
				memcpy(&DateTime.dat_Stamp,&infoBlock.fib_Date,sizeof(struct DateStamp));
				DateTime.dat_Format=FORMAT_USA;
				DateTime.dat_Flags=DTF_FUTURE;
				DateTime.dat_StrDay=NULL;
				DateTime.dat_StrDate=yesterday;
				DateTime.dat_StrTime=NULL;
				DateToStr(&DateTime);
				julian(&horolog,yesterday);
				jdate(yesterday,horolog);
			}
			UnLock(lockfile);
		}
		UnLock(lockdir);
		CurrentDir(root);
	}

	Forbid();
	if(!(port=(struct MsgPort *)FindPort("RPGBBS Deluxe Server"))) {
		if((port=(struct MsgPort *)CreatePort("RPGBBS Deluxe Server",0))) {
			Permit();
			LoadTables();
			ConsoleSignal=(RPGBBSmode==BBS) ? 1L << window->UserPort->mp_SigBit : 0L;
			ServerSignal=1L << port->mp_SigBit;
			while(Loop) {
				signal=Wait(ConsoleSignal | ServerSignal);
				if(signal & ConsoleSignal)
					ProcessConsoleSignal();
				if(signal & ServerSignal)
					ProcessServerSignal();
				GetSystemDate(today);
				julian(&horolog,today);
				if(strcmp(today,yesterday)) {
					strcpy(yesterday,today);
					remove("Notices/Yesterday");
					rename("Notices/Today","Notices/Yesterday");
					remove("Notices/Yesterday.IBM");
					rename("Notices/Today.IBM","Notices/Yesterday.IBM");
					PutCallers();
					for(i=1; i<500000; i++) {
						fseek(user_fp,i*USER_RECORD_SIZE,SEEK_SET);
						if(!fread((char *)user,USER_RECORD_SIZE,1,user_fp))
							break;
						if(strlen(user->ID))
							if(*user->ID!='_') {
								if(user->Gold>0.) {
									if(user->Loan>0.) {
										user->Loan-=user->Gold;
										if(user->Loan<0.) {
											user->Gold-=user->Loan;
											user->Loan=0.;
										}
									}
									user->Bank+=user->Gold;
									user->Gold=0.;
									fseek(user_fp,i*USER_RECORD_SIZE,SEEK_SET);
									fwrite((char *)user,USER_RECORD_SIZE,1,user_fp);
								}
								if((WORD)(horolog-user->LastDate)-user->Level>30 || horolog-30>user->ExpireDate)
									DelUser(user);
							}
					}
				}
			}
			UnloadTables();
		}
	}
	Shutdown();
}
