/**********************************************\
 *  RPGBBS Deluxe External Emulation Library  *
 *                                            *
 *           Written by Robert Hurst          *
\**********************************************/
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/keymap.h>
#include <proto/dos.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/include/XEM.h"

extern BOOL __asm XEmulatorPreferences(register __a0 struct XEM_IO *, register __a1 STRPTR, register __d0 ULONG);
extern BOOL __asm XEmulatorResetConsole(register __a0 struct XEM_IO *);

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

struct Attribute {
	UBYTE Alternate:1;
	UBYTE Blink:1;
	UBYTE Bold:1;
	UBYTE Dim:1;
	UBYTE Protect:1;
	UBYTE Rvs:1;
	UBYTE Ul:1;
};

struct CellColor {
	UBYTE Fore:4;
	UBYTE Back:4;
};

struct Mode {
	UBYTE AutoWrap:1;
	UBYTE CursorSteady:1;
	UBYTE CursorUnderScore:1;
	UBYTE CursorVisible:1;
	UBYTE Draw:1;
	UBYTE Font:1,G0:1,G1:1;
	UBYTE Insert:1;
	UBYTE Keypad:1;
	UBYTE Newline:1;
};

struct SERCON {
	struct XEM_IO *io;
	struct List *macrokeys;
	struct RastPort *rp;
	struct TextFont *TextFont;
	struct TextAttr TextAttr;
	struct Window *Window;
	struct Attribute *AttrBuffer,CA,SaveCA;
	struct CellColor *ColorBuffer,CC,SaveCC;
	struct Mode Mode,SaveMode;
	FILE *Printer;
	UBYTE *ScreenBuffer;
	UBYTE *EscapeBuffer;
	UBYTE *AnswerBack;
	UBYTE MaxRow,Row,SaveRow;
	UBYTE MaxCol,Col,SaveCol;
	UWORD PXLrow,PXLcol,PXLstart;
	UBYTE SRtop,SRbottom;
	UBYTE Font[29],Size;
};

#define MOVE(y,x) Move(con->rp,con->TextFont->tf_XSize*((x<con->MaxCol) ? x : con->MaxCol-1),con->TextFont->tf_YSize*y+con->TextFont->tf_Baseline);
extern void WriteConsole(struct SERCON *,UBYTE *, LONG);

const UBYTE __far alternate[] = {
	' ','\4','\261','h','f','c','l','\370',
	'\361','n','v','\331','\277','\332','\300','\305',
	'\304','\304','\304','\304','\304','\303','\264','\301',
	'\302','\263','\363','\362','\343','\35','\234','\372'
};

VOID __asm XEmulatorExpu(VOID)
{
}

BOOL __asm XEmulatorInit(VOID)
{
	return(TRUE);
}

enum { ARG_FN,ARG_FS,ARG_AW,ARG_NL,ARGS };

STATIC BOOL RPG_parseoptions(struct SERCON *con, STRPTR optionsBuffer)
{
	STATIC UBYTE *PreferencesTemplate = "FN=FONT/K,FS=SIZE/K,AW=AUTOWRAP/K,NL=NEWLINE/K";
	UBYTE **ArgArray;
	UBYTE *ArgBuffer;
	BOOL success=FALSE;

	if(ArgBuffer = (UBYTE *)calloc(1,1024))
	{
		strcpy(ArgBuffer, optionsBuffer);
		strcat(ArgBuffer, "\n");

		if(ArgArray = (UBYTE **)calloc(1,sizeof(UBYTE *) * ARGS))
		{
			struct RDArgs *ArgsPtr;

			if(ArgsPtr = (struct RDArgs *)AllocDosObject(DOS_RDARGS, TAG_DONE))
			{
				ArgsPtr->RDA_Source.CS_Buffer = ArgBuffer;
				ArgsPtr->RDA_Source.CS_Length = strlen(ArgBuffer);
				ArgsPtr->RDA_Source.CS_CurChr = 0;
				ArgsPtr->RDA_DAList	= NULL;
				ArgsPtr->RDA_Buffer	= NULL;
				ArgsPtr->RDA_BufSiz	= 0;
				ArgsPtr->RDA_ExtHelp	= NULL;
				ArgsPtr->RDA_Flags	= RDAF_NOPROMPT;

				if(ReadArgs(PreferencesTemplate, (LONG *)ArgArray, ArgsPtr))
				{
/*					VOID KPrintF(UBYTE *, ...); */
					success = TRUE;

					if(ArgArray[ARG_FN])
						strcpy(con->Font,ArgArray[ARG_FN]);

					if(ArgArray[ARG_FS])
						con->Size=atoi(ArgArray[ARG_FS]);

					if(ArgArray[ARG_AW])
						con->Mode.AutoWrap=strnicmp(ArgArray[ARG_AW],"OFF",3) ? TRUE : FALSE;

					if(ArgArray[ARG_NL])
						con->Mode.Newline=strnicmp(ArgArray[ARG_NL],"OFF",3) ? TRUE : FALSE;

					FreeArgs(ArgsPtr);
				}
				FreeDosObject(DOS_RDARGS, ArgsPtr);
			}
			free(ArgArray);
		}
		free(ArgBuffer);
	}
	return(success);
}

BOOL __asm XEmulatorSetup(register __a0 struct XEM_IO *io)
{
	struct SERCON *con;

	io->xem_console=NULL;
	if((con=calloc(1,sizeof(struct SERCON)))) {
		con->io=io;
		con->Window=io->xem_window;
		con->rp=io->xem_window->RPort;
		io->xem_console=con;
		*con->io->xem_signal=0;
		return(TRUE);
	}
	return(FALSE);
}

BOOL __asm XEmulatorOpenConsole(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con) {
		con->Window=io->xem_window;
		con->rp=io->xem_window->RPort;
		if(!XEmulatorPreferences(io,"ENV:xemrpgbbs",XEM_PREFS_LOAD))
			XEmulatorPreferences(io,NULL,XEM_PREFS_RESET);
		con->TextAttr.ta_Name=con->Font;
		con->TextAttr.ta_YSize=con->Size;
		con->TextAttr.ta_Style=FS_NORMAL;
		con->TextAttr.ta_Flags=FPF_DISKFONT | FPF_DESIGNED;
		if(!(con->TextFont=(struct TextFont *)OpenFont(&con->TextAttr)))
			con->TextFont=(struct TextFont *)OpenDiskFont(&con->TextAttr);
		if(con->TextFont) {
			SetFont(con->rp,con->TextFont);
			SetAPen(con->rp,15);
			SetBPen(con->rp,0);
			SetDrMd(con->rp,JAM2);
			con->MaxRow=(io->xem_window) ? io->xem_window->Height/con->TextFont->tf_YSize-1 : 24;
			con->MaxCol=(io->xem_window) ? io->xem_window->Width/con->TextFont->tf_XSize : 80;
			con->AttrBuffer=calloc(con->MaxRow,con->MaxCol);
			con->ColorBuffer=calloc(con->MaxRow,con->MaxCol);
			con->ScreenBuffer=calloc(con->MaxRow,con->MaxCol);
			con->EscapeBuffer=calloc(1,256);
			XEmulatorResetConsole(io);
			return(TRUE);
		}
	}
	return(FALSE);
}

VOID __asm XEmulatorCloseConsole(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con) {
		if(con->Printer) {
			fclose(con->Printer);
			con->Printer=NULL;
		}
		if(con->TextFont)
			CloseFont(con->TextFont);
		if(con->EscapeBuffer)
			free(con->EscapeBuffer);
		if(con->ScreenBuffer)
			free(con->ScreenBuffer);
		if(con->ColorBuffer)
			free(con->ColorBuffer);
		if(con->AttrBuffer)
			free(con->AttrBuffer);
		SetFont(io->xem_window->RPort,io->xem_font);
	}
	return;
}

VOID __asm XEmulatorCleanup(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con)
		free(con);
}

VOID __asm XEmulatorWrite(register __a0 struct XEM_IO *io, register __a1 UBYTE *string, register __d0 LONG len)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con)
		WriteConsole(con,string,len);
}

BOOL __asm XEmulatorSignal(register __a0 struct XEM_IO *io, register __d0 ULONG sig)
{
	return(TRUE);
}

ULONG __asm XEmulatorHostMon(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorHostData *hd, register __d0 ULONG actual)
{
	if(hd->Destination != NULL)
	{
		ULONG cnt;
		UBYTE *read, *write;
		REGISTER UBYTE c;

		read	= hd->Source;
		write = hd->Destination;
		for(cnt=0; cnt < actual; cnt++)
		{
			c = *read++;

			if(c == '\x18'  ||  c == '\x1A')	/* CAN  ||  SUB */
			{
				hd->InESC = FALSE;
				hd->InCSI = FALSE;
				continue;
			}

			if(hd->InESC)		/* Escape - Befehl ? */
			{
				if(c == '[')	/* ist's ein verkappter CSI ? */
				{
					hd->InCSI = TRUE;
					hd->InESC = FALSE;
				}
				else
				{
					if(c >= '0')
						hd->InESC = FALSE;
				}
				continue;
			}

			if(hd->InCSI)		/* CSI - Befehl ? */
			{
				if(c >= '@')
					hd->InCSI = FALSE;
				continue;
			}


			if(c == '\033')
			{
				hd->InESC = TRUE;
				hd->InCSI = FALSE;
				continue;
			}

			if(c == '\233')
			{
				hd->InESC = FALSE;
				hd->InCSI = TRUE;
				continue;
			}

			*write++ = c;
		}

		return((ULONG)(write - hd->Destination));
	}
	else
		return(0);
}

STATIC BOOL HandleMacroKeys(struct SERCON *con, struct IntuiMessage *imsg, UBYTE chr)
{
	struct Node *node;

	if(con->macrokeys  &&  con->io->xem_process_macrokeys)
	{
		struct XEmulatorMacroKey *key;
		BOOL shift, alt, ctrl;
		UWORD qual;

		shift	= imsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
		ctrl	= imsg->Qualifier & IEQUALIFIER_CONTROL;
		alt	= imsg->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);

		if(shift)
			qual = XMKQ_SHIFT;
		else
		{
			if(alt)
				qual = XMKQ_ALTERNATE;
			else
			{
				if(ctrl)
					qual = XMKQ_CONTROL;
				else
					qual = XMKQ_NONE;
			}
		}

//		if(con->macrokeys->lh_TailPred!=con->macrokeys)
		for(node=con->macrokeys->lh_Head; node->ln_Succ; node=node->ln_Succ) {
			key=(struct XEmulatorMacroKey *)node;
				if(key->xmk_Qualifier == qual)
				{
					BOOL match=FALSE;

					if(key->xmk_Type == XMKT_RAWKEY)
					{
						if(key->xmk_Code == imsg->Code)
							match = TRUE;
					}

					if(key->xmk_Type == XMKT_COOKED)
					{
						if(key->xmk_Code == chr)
							match = TRUE;
					}

					if(match != FALSE)
					{
						con->io->xem_process_macrokeys(key);
						return(TRUE);
					}
				}
		}
	}
	return(FALSE);
}
ULONG __asm XEmulatorUserMon(register __a0 struct XEM_IO *io, register __a1 STRPTR retstr, register __d0 ULONG maxlen, register __a2 struct IntuiMessage *imsg)
{
	struct SERCON *con = (struct SERCON *)io->xem_console;
	struct InputEvent ie;
	ULONG length;
	UWORD code;

	code = imsg->Code;
	if(code & IECODE_UP_PREFIX)
		return(0);

	retstr[0] = '\0';

	ie.ie_Class			= IECLASS_RAWKEY;
	ie.ie_SubClass		= 0;
	ie.ie_Code			= code;
	ie.ie_Qualifier	= imsg->Qualifier;
	ie.ie_position.ie_addr = *((APTR *)imsg->IAddress);

	if((length = MapRawKey(&ie, retstr, 20, NULL)) <= 0)
		return(0);

	if(retstr[0] == '\15'  &&  con->Mode.Newline  &&
	  (code == 0x0043  ||  code == 0x0044))
	{
		retstr[1] = '\12';
		retstr[2] = '\0';
		length = 2;
	}
	else
	{
		if(retstr[0] == '\233')
		{
			auto UBYTE mybuf[40];

			if(length > (40-1))
				return(0);

			strcpy(&mybuf[1], retstr);
			mybuf[0] = '\33';
			mybuf[1] = '[';
			strcpy(retstr, mybuf);
			length++;
		}
	}

	retstr[length] = '\0';

	if(HandleMacroKeys(con, imsg, retstr[0]))
		length = 0;

	return(length);
}

BOOL __asm XEmulatorOptions(register __a0 struct XEM_IO *io)
{
	struct SERCON *con = (struct SERCON *)io->xem_console;
	UBYTE opt_header, opt_font, opt_size, opt_aw, opt_nl;
	ULONG numopts;

	struct xem_option *opti[6];
	UBYTE buff[6][30];

	struct xem_option opt0;
	struct xem_option opt1;
	struct xem_option opt2;
	struct xem_option opt3;
	struct xem_option opt4;
	struct xem_option opt5;

	opti[0] = &opt0;
	opti[1] = &opt1;
	opti[2] = &opt2;
	opti[3] = &opt3;
	opti[4] = &opt4;
	opti[5] = &opt5;

	numopts = 0;

	opt_header = numopts;
	opti[opt_header]->xemo_description = "RPGBBS options:";
	opti[opt_header]->xemo_type	= XEMO_HEADER;
	opti[opt_header]->xemo_value	= NULL;
	opti[opt_header]->xemo_length	= NULL;

	opt_font = ++numopts;
	opti[opt_font]->xemo_value = buff[opt_font];
	strcpy(opti[opt_font]->xemo_value, con->Font);
	opti[opt_font]->xemo_description = "Font name";
	opti[opt_font]->xemo_type		= XEMO_STRING;
	opti[opt_font]->xemo_length	= sizeof(buff[opt_font]);

	opt_size = ++numopts;
	opti[opt_size]->xemo_value = buff[opt_size];
	sprintf(opti[opt_size]->xemo_value,"%u",con->Size);
	opti[opt_size]->xemo_description = "Point size";
	opti[opt_size]->xemo_type		= XEMO_LONG;
	opti[opt_size]->xemo_length	= 3;

	opt_aw = ++numopts;
	opti[opt_aw]->xemo_value = buff[opt_aw];
	if(con->Mode.AutoWrap)
		strcpy(opti[opt_aw]->xemo_value, "on");
	else
		strcpy(opti[opt_aw]->xemo_value, "off");
	opti[opt_aw]->xemo_description = "Auto Line Wrap";
	opti[opt_aw]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_aw]->xemo_length	= sizeof(buff[opt_aw]);

	opt_nl = ++numopts;
	opti[opt_nl]->xemo_value = buff[opt_nl];
	if(con->Mode.Newline)
		strcpy(opti[opt_nl]->xemo_value, "on");
	else
		strcpy(opti[opt_nl]->xemo_value, "off");
	opti[opt_nl]->xemo_description = "Newline";
	opti[opt_nl]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_nl]->xemo_length	= sizeof(buff[opt_nl]);


	if(con->io->xem_toptions(++numopts, opti)) {
		XEmulatorCloseConsole(io);

		if(!stricmp(opti[opt_nl]->xemo_value, "yes") || !stricmp(opti[opt_nl]->xemo_value, "on"))
			con->Mode.Newline=TRUE;
		else
			con->Mode.Newline=FALSE;

		if(!stricmp(opti[opt_aw]->xemo_value, "yes") || !stricmp(opti[opt_aw]->xemo_value, "on"))
			con->Mode.AutoWrap=TRUE;
		else
			con->Mode.AutoWrap=FALSE;

		con->Size=atoi(opti[opt_size]->xemo_value);

		strcpy(con->Font,opti[opt_font]->xemo_value);

		XEmulatorPreferences(io,"ENVARC:xemrpgbbs",XEM_PREFS_SAVE);
		XEmulatorPreferences(io,"ENV:xemrpgbbs",XEM_PREFS_SAVE);
		XEmulatorOpenConsole(io);
	}

	return(TRUE);
}

BOOL __asm XEmulatorClearConsole(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con) {
		WriteConsole(con,"\33[H\33[J",6);
		return(TRUE);
	}
	return(FALSE);
}

BOOL __asm XEmulatorResetConsole(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con) {
		WriteConsole(con,"\33c",2);
		return(TRUE);
	}
	return(FALSE);
}

BOOL __asm XEmulatorResetTextStyles(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con) {
		WriteConsole(con,"\33[0;37m",7);
		return(TRUE);
	}
	return(FALSE);
}

BOOL __asm XEmulatorResetCharset(register __a0 struct XEM_IO *io)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con) {
		WriteConsole(con,"\33[0;37m",7);
		return(TRUE);
	}
	return(FALSE);
}

ULONG __asm XEmulatorGetFreeMacroKeys(register __a0 struct XEM_IO *io, register __d0 ULONG qualifier)
{
	return(512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1);
}

BOOL __asm XEmulatorMacroKeyFilter(register __a0 struct XEM_IO *io, register __a1 struct List *keys)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;

	if(con!=NULL  &&  con->io->xem_process_macrokeys) {
		con->macrokeys=keys;
		return(TRUE);
	}

	return(FALSE);
}

LONG __asm XEmulatorInfo(register __a0 struct XEM_IO *io, register __d0 ULONG type)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;
	LONG result=-1;

	if(con)
		switch(type) {
			case XEMI_CURSOR_POSITION:
				result=XEMI_CREATE_POSITION(con->Row,con->Col);
				break;
			case XEMI_CONSOLE_DIMENSIONS:
				result=XEMI_CREATE_DIMENSIONS((LONG)con->MaxCol,(LONG)con->MaxRow);
				break;
		}
	return(result);
}

BOOL __asm XEmulatorPreferences(register __a0 struct XEM_IO *io, register __a1 STRPTR fileName, register __d0 ULONG mode)
{
	struct SERCON *con=(struct SERCON *)io->xem_console;
	FILE *fp;
	UBYTE buffer[256];
	BOOL success=FALSE;

	if(con) {
		switch(mode) {
			case XEM_PREFS_RESET:
				strcpy(con->Font,"IBM.font");
				con->Size=8;
				con->Mode.AutoWrap=TRUE;
				con->Mode.Newline=FALSE;
				success=TRUE;
				break;
			case XEM_PREFS_LOAD:
				if((fp=fopen(fileName,"r"))) {
					fread(buffer,sizeof(buffer),1,fp);
					RPG_parseoptions(con,buffer);
					fclose(fp);
					success=TRUE;
				}
				break;
			case XEM_PREFS_SAVE:
				if((fp=fopen(fileName,"w"))) {
					fprintf(fp,"FONT=%s",con->Font);
					fprintf(fp," SIZE=%u",con->Size);
					fprintf(fp," AUTOWRAP=%s",con->Mode.AutoWrap ? "ON" : "OFF");
					fprintf(fp," NEWLINE=%s\n",con->Mode.Newline ? "ON" : "OFF");
					fclose(fp);
					success=TRUE;
				}
				break;
		}
	}
	return(success);
}

void __stdargs _XCEXIT(long l)
{
}

void current(struct SERCON *con)
{
	int b,f;

	b=con->CC.Back;
	f=con->CC.Fore;
	if(con->CA.Bold)
		f|=(con->io->xem_screendepth==3) ? 3 : 8;
	if(con->CA.Dim)
		f=(con->io->xem_screendepth==3) ? 2 : 8;
	if(con->CA.Blink)
		f|=(con->io->xem_screendepth==3) ? 4 : 16;
	if(con->CA.Rvs) {
		SetAPen(con->rp,b);
		SetBPen(con->rp,f);
	}
	else {
		SetAPen(con->rp,f);
		SetBPen(con->rp,b);
	}
	SetSoftStyle(con->rp,FS_NORMAL | ((con->CA.Ul) ? FSF_UNDERLINED : 0),0xFF);
	MOVE(con->Row,con->Col);
}
void DrawCursor(struct SERCON *con,UBYTE sw)
{
	struct Attribute a=con->CA;
	struct CellColor c=con->CC;
	int p=con->Row*con->MaxCol+((con->Col<con->MaxCol) ? con->Col : con->MaxCol-1);

	if(!con->Mode.CursorVisible || con->Mode.Draw || !con->ScreenBuffer)
		return;
	if(sw) {
		con->CA.Bold=FALSE;
		con->CA.Dim=FALSE;
		con->CA.Rvs=FALSE;
		con->CC.Back=(con->io->xem_screendepth==3) ? 2 : BLACK;
		con->CC.Fore=(con->io->xem_screendepth==3) ? 3 : WHITE;
	}
	else {
		con->CA=con->AttrBuffer[p];
		con->CC=con->ColorBuffer[p];
	}
	current(con);
	Text(con->Window->RPort,&con->ScreenBuffer[p],1);
	con->CA=a;
	con->CC=c;
	current(con);
}
void scroll(struct SERCON *con, int l)
{
	UWORD from,to,line,size;

	if(l>0) {
		from=(con->SRtop+l)*con->MaxCol;
		to=con->SRtop*con->MaxCol;
		line=(con->SRbottom-l+1)*con->MaxCol;
	}
	if(l<0) {
		from=con->SRtop*con->MaxCol;
		to=(con->SRtop-l)*con->MaxCol;
		line=con->SRtop*con->MaxCol;
	}
	size=(con->SRbottom-con->SRtop-abs(l)+1)*con->MaxCol;
	memmove(&con->ScreenBuffer[to],&con->ScreenBuffer[from],size);
	memmove(&con->AttrBuffer[to],&con->AttrBuffer[from],size);
	memmove(&con->ColorBuffer[to],&con->ColorBuffer[from],size);
	size=abs(l)*con->MaxCol;
	memset(&con->ScreenBuffer[line],' ',size);
	memset(&con->AttrBuffer[line],0,size);
	memset(&con->ColorBuffer[line],0,size);
	SetBPen(con->rp,BLK);
	ScrollRaster(con->rp,0,con->TextFont->tf_YSize*l,0,con->TextFont->tf_YSize*con->SRtop,con->Window->Width,con->TextFont->tf_YSize*(con->SRbottom+1));
	current(con);
}
void DecodeANSI(struct SERCON *con, UBYTE *str)
{
	int i,n;
	UBYTE params[32],sp;

	if(str[1]=='[')
		str++;
	if(*str==0x9B || *str=='[') {	/*  control-sequence introduced  */
		sp=0;
		do {	/*  process multiple parameters  */
			str++;
			n=0;
			while(isdigit(*str)) {
				n=10*n+*str-'0';
				str++;
			}
			params[sp++]=n;
		} while(*str==';' && sp<32);
		switch(*str) {
			case '"':
				str++;
				if(*str=='q')
					con->CA.Protect=(params[0]==1);
				break;
			case '?':	//	setups
				sp=0;
				do {	//	process multiple parameters
					str++;
					n=0;
					while(isdigit(*str)) {
						n=10*n+*str-'0';
						str++;
					}
					params[sp++]=n;
				} while(*str==';' && sp<32);
				switch(*str) {
					case 'J':	/*  Clear erasable characters  */
						SetBPen(con->rp,BLK);
						switch(params[0]) {
							case 0:	/*  from cursor to end-of-screen  */
								n=con->Col;
								for(i=con->Row; i<con->MaxRow; i++) {
									sp=0;
									for(; n<con->MaxCol; n++) {
										if(!con->AttrBuffer[i*con->MaxCol+n].Protect)
											sp++;
										if(sp && (con->AttrBuffer[i*con->MaxCol+n].Protect || n+1==con->MaxCol)) {
											if(!con->AttrBuffer[i*con->MaxCol+n].Protect && n+1==con->MaxCol)
												n++;
											memset(&con->ScreenBuffer[i*con->MaxCol+n-sp],' ',sp);
											memset(&con->AttrBuffer[i*con->MaxCol+n-sp],0,sp);
											memset(&con->ColorBuffer[i*con->MaxCol+n-sp],0,sp);
											MOVE(i,n-sp);
											Text(con->rp,&con->ScreenBuffer[i*con->MaxCol+n-sp],sp);
											sp=0;
										}
									}
									n=0;
								}
								break;
							case 1: /*  from top to cursor  */
								for(i=con->Row; i>=0; i--) {
									sp=0;
									for(n=0; n<(i==con->Row ? con->Col : con->MaxCol); n++) {
										if(!con->AttrBuffer[i*con->MaxCol+n].Protect)
											sp++;
										if(sp && (con->AttrBuffer[i*con->MaxCol+n].Protect || n+1==con->MaxCol)) {
											if(!con->AttrBuffer[i*con->MaxCol+n].Protect && n+1==con->MaxCol)
												n++;
											memset(&con->ScreenBuffer[i*con->MaxCol+n-sp],' ',sp);
											memset(&con->AttrBuffer[i*con->MaxCol+n-sp],0,sp);
											memset(&con->ColorBuffer[i*con->MaxCol+n-sp],0,sp);
											MOVE(i,n-sp);
											Text(con->rp,&con->ScreenBuffer[i*con->MaxCol+n-sp],sp);
											sp=0;
										}
									}
								}
								break;
							case 2: /*  entire display  */
								for(i=0; i<con->MaxRow; i++) {
									sp=0;
									for(n=0; n<con->MaxCol; n++) {
										if(!con->AttrBuffer[i*con->MaxCol+n].Protect)
											sp++;
										if(sp && (con->AttrBuffer[i*con->MaxCol+n].Protect || n+1==con->MaxCol)) {
											if(!con->AttrBuffer[i*con->MaxCol+n].Protect && n+1==con->MaxCol)
												n++;
											memset(&con->ScreenBuffer[i*con->MaxCol+n-sp],' ',sp);
											memset(&con->AttrBuffer[i*con->MaxCol+n-sp],0,sp);
											memset(&con->ColorBuffer[i*con->MaxCol+n-sp],0,sp);
											MOVE(i,n-sp);
											Text(con->rp,&con->ScreenBuffer[i*con->MaxCol+n-sp],sp);
											sp=0;
										}
									}
								}
								break;
						}
						current(con);
						break;
					case 'K':	/*  clear line  */
						SetBPen(con->rp,BLK);
						i=con->Row*con->MaxCol;
						sp=0;
						switch(params[0]) {
							case 0:	/*  from cursor to end-of-line  */
								for(n=con->Col; n<con->MaxCol; n++) {
									if(!con->AttrBuffer[i+n].Protect)
										sp++;
									if(sp && (con->AttrBuffer[i+n].Protect || n+1==con->MaxCol)) {
										if(!con->AttrBuffer[i+n].Protect && n+1==con->MaxCol)
											n++;
										memset(&con->ScreenBuffer[i+n-sp],' ',sp);
										memset(&con->AttrBuffer[i+n-sp],0,sp);
										memset(&con->ColorBuffer[i+n-sp],0,sp);
										MOVE(i,n-sp);
										Text(con->rp,&con->ScreenBuffer[i+n-sp],sp);
										sp=0;
									}
								}
								break;
							case 1:	/*  from beginning-of-line to cursor  */
								for(n=0; n<con->Col; n++) {
									if(!con->AttrBuffer[i+n].Protect)
										sp++;
									if(sp && (con->AttrBuffer[i+n].Protect || n+1==con->Col)) {
										if(!con->AttrBuffer[i+n].Protect && n+1==con->Col)
											n++;
										memset(&con->ScreenBuffer[i+n-sp],' ',sp);
										memset(&con->AttrBuffer[i+n-sp],0,sp);
										memset(&con->ColorBuffer[i+n-sp],0,sp);
										MOVE(i,n-sp);
										Text(con->rp,&con->ScreenBuffer[i+n-sp],sp);
										sp=0;
									}
								}
								break;
							case 2:	/*  entire line  */
								for(n=0; n<con->MaxCol; n++) {
									if(!con->AttrBuffer[i+n].Protect)
										sp++;
									if(sp && (con->AttrBuffer[i+n].Protect || n+1==con->MaxCol)) {
										if(!con->AttrBuffer[i+n].Protect && n+1==con->MaxCol)
											n++;
										memset(&con->ScreenBuffer[i+n-sp],' ',sp);
										memset(&con->AttrBuffer[i+n-sp],0,sp);
										memset(&con->ColorBuffer[i+n-sp],0,sp);
										MOVE(i,n-sp);
										Text(con->rp,&con->ScreenBuffer[i+n-sp],sp);
										sp=0;
									}
								}
								break;
						}
						current(con);
						break;
					case 'h':	/*  setup on  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 7:
									con->Mode.AutoWrap=TRUE;
									break;
								case 25:
									con->Mode.CursorVisible=TRUE;
									break;
								case 66:
									con->Mode.Keypad=TRUE;
									break;
							}
						break;
					case 'i':	/*  Autoprint mode  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 4:
									if(con->Printer)
										fclose(con->Printer);
									con->Printer=NULL;
									break;
								case 5:
									con->Printer=fopen("PRT:","w");
									break;
							}
						break;
					case 'l':	/*  setup off  */
						for(i=0; i<sp; i++)
							switch(params[i]) {
								case 7:
									con->Mode.AutoWrap=FALSE;
									break;
								case 25:
									con->Mode.CursorVisible=FALSE;
									break;
								case 66:
									con->Mode.Keypad=FALSE;
									break;
							}
						break;
				}
				break;
			case '@':	/*  insert character  */
				if(!params[0])
					params[0]=1;
				i=con->Row*con->MaxCol+con->Col;
				n=con->MaxCol-con->Col-params[0];
				if(n>0) {
					memmove(&con->ScreenBuffer[i+params[0]],&con->ScreenBuffer[i],n);
					memmove(&con->AttrBuffer[i+params[0]],&con->AttrBuffer[i],n);
					memmove(&con->ColorBuffer[i+params[0]],&con->ColorBuffer[i],n);
				}
				n=(n>0) ? params[0] : con->MaxCol-con->Col;
				memset(&con->ScreenBuffer[i],' ',n);
				memset(&con->AttrBuffer[i],0,n);
				memset(&con->ColorBuffer[i],0,n);
				SetBPen(con->rp,BLK);
				ScrollRaster(con->rp,-con->TextFont->tf_XSize*n,0,con->TextFont->tf_XSize*(con->Col+1),con->TextFont->tf_YSize*(con->Row+1),con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+2));
				current(con);
				break;
			case 'A':	/*  up  */
				if(!params[0])
					params[0]=1;
				con->Row=(con->Row<params[0]) ? 0 : con->Row-params[0];
				break;
			case 'B':	/*  down  */
				if(!params[0])
					params[0]=1;
				con->Row=(con->Row+params[0]>=con->MaxRow) ? con->MaxRow-1 : con->Row+params[0];
				break;
			case 'C':	/*  right  */
				if(!params[0])
					params[0]=1;
				con->Col=(con->Col+params[0]>=con->MaxCol) ? con->MaxCol-1 : con->Col+params[0];
				break;
			case 'D':	/*  left  */
				if(!params[0])
					params[0]=1;
				con->Col=(con->Col<params[0]) ? 0 : con->Col-params[0];
				break;
			case 'E':
				if(!params[0])
					params[0]=1;
				con->Row=(con->Row+params[0]>=con->MaxRow) ? con->MaxRow-1 : con->Row+params[0];
				con->Col=0;
				break;
			case 'H':	/*  plot  */
				con->Row=(params[0]<1 ? 1 : params[0]>con->MaxRow ? con->MaxRow : params[0])-1;
				if(sp>1)
					con->Col=(params[1]<1 ? 1 : params[1]>con->MaxCol ? con->MaxCol : params[1])-1;
				else
					con->Col=0;
				break;
			case 'J':	/*  clear screen  */
				SetAPen(con->rp,BLK);
				if(n==0 && con->Row==0 && con->Col==0)
					n=2;
				switch(n) {
					case 0:	/*  from cursor to end-of-screen  */
						n=(con->MaxRow*con->MaxCol)-(con->Row*con->MaxCol+con->Col)+1;
						memset(&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],' ',n);
						memset(&con->AttrBuffer[con->Row*con->MaxCol+con->Col],0,n);
						memset(&con->ColorBuffer[con->Row*con->MaxCol+con->Col],0,n);
						n=con->Col;
						for(i=con->Row; i<con->MaxRow; i++) {
							RectFill(con->rp,con->TextFont->tf_XSize*n,con->TextFont->tf_YSize*i,con->TextFont->tf_XSize*(con->MaxCol+1),con->TextFont->tf_YSize*(i+1)-1);
							n=0;
						}
						break;
					case 1: /*  from top to cursor  */
						n=(con->Row*con->MaxCol+con->Col)+1;
						memset(&con->ScreenBuffer[0],' ',n);
						memset(&con->AttrBuffer[0],0,n);
						memset(&con->ColorBuffer[0],0,n);
						for(i=0; i<con->Row; i++)
							RectFill(con->rp,0,con->TextFont->tf_YSize*i,con->TextFont->tf_XSize*(con->MaxCol+1),con->TextFont->tf_YSize*(i+1)-1);
						MOVE(con->Row,0);
						Text(con->rp,&con->ScreenBuffer[con->Row*con->MaxCol],con->Col+1);
						break;
					case 2: /*  entire display  */
						n=con->MaxRow*con->MaxCol;
						memset(con->ScreenBuffer,' ',n);
						memset(con->AttrBuffer,0,n);
						memset(con->ColorBuffer,0,n);
						RectFill(con->rp,0,0,con->TextFont->tf_XSize*(con->MaxCol+1),con->TextFont->tf_YSize*con->MaxRow-1);
						break;
				}
				current(con);
				break;
			case 'K':	/*  clear line  */
				SetAPen(con->rp,BLK);
				switch(n) {
					case 0:	/*  from cursor to end-of-line  */
						n=con->MaxCol-con->Col;
						memset(&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],' ',n);
						memset(&con->AttrBuffer[con->Row*con->MaxCol+con->Col],0,n);
						memset(&con->ColorBuffer[con->Row*con->MaxCol+con->Col],0,n);
						RectFill(con->rp,con->TextFont->tf_XSize*con->Col,con->TextFont->tf_YSize*con->Row,con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+1)-1);
						break;
					case 1:	/*  from beginning-of-line to cursor  */
						memset(&con->ScreenBuffer[con->Row*con->MaxCol],' ',con->MaxCol);
						memset(&con->AttrBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						memset(&con->ColorBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						RectFill(con->rp,0,con->TextFont->tf_YSize*con->Row,con->TextFont->tf_XSize*(con->Col+1),con->TextFont->tf_YSize*(con->Row+1)-1);
						break;
					case 2:	/*  entire line  */
						memset(&con->ScreenBuffer[con->Row*con->MaxCol],' ',con->MaxCol);
						memset(&con->AttrBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						memset(&con->ColorBuffer[con->Row*con->MaxCol],0,con->MaxCol);
						RectFill(con->rp,0,con->TextFont->tf_YSize*con->Row,con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+1)-1);
						break;
				}
				current(con);
				break;
			case 'L':	/*  insert line  */
				if(!params[0])
					params[0]=1;
				params[1]=con->SRtop;
				params[2]=con->SRbottom;
				con->SRtop=con->Row;
				con->SRbottom=con->MaxRow-1;
				scroll(con,-(int)params[0]);
				con->SRtop=params[1];
				con->SRbottom=params[2];
				break;
			case 'M':	/*  delete line  */
				if(!params[0])
					params[0]=1;
				params[1]=con->SRtop;
				params[2]=con->SRbottom;
				con->SRtop=con->Row;
				con->SRbottom=con->MaxRow-1;
				scroll(con,params[0]);
				con->SRtop=params[1];
				con->SRbottom=params[2];
				break;
			case 'P':	/*  delete character  */
				if(!params[0])
					params[0]=1;
				i=con->Row*con->MaxCol+con->Col;
				n=con->MaxCol-con->Col-params[0];
				if(n>0) {
					memmove(&con->ScreenBuffer[i],&con->ScreenBuffer[i+params[0]],n);
					memmove(&con->AttrBuffer[i],&con->AttrBuffer[i+params[0]],n);
					memmove(&con->ColorBuffer[i],&con->ColorBuffer[i+params[0]],n);
				}
				i=con->Row*con->MaxCol-con->Col+((n>0) ? n : 0);
				n=(n>0) ? params[0] : con->MaxCol-con->Col;
				memset(&con->ScreenBuffer[i],' ',n);
				memset(&con->AttrBuffer[i],0,n);
				memset(&con->ColorBuffer[i],0,n);
				SetBPen(con->rp,BLK);
				ScrollRaster(con->rp,con->TextFont->tf_XSize*n,0,con->TextFont->tf_XSize*(con->Col+1),con->TextFont->tf_YSize*(con->Row+1),con->TextFont->tf_XSize*con->MaxCol,con->TextFont->tf_YSize*(con->Row+2));
				current(con);
				break;
			case 'X':	/*  erase  */
				SetBPen(con->rp,BLK);
				if((n=con->MaxCol-con->Col)<params[0])
					params[0]=n;
				memset(&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],' ',params[0]);
				memset(&con->AttrBuffer[con->Row*con->MaxCol+con->Col],0,params[0]);
				memset(&con->ColorBuffer[con->Row*con->MaxCol+con->Col],0,params[0]);
				Text(con->rp,&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],params[0]);
				current(con);
				break;
			case 'Z':	/*  reverse tab  */
				if(con->Col)
					con->Col-=(8-con->Col%8);
				break;
			case 'c':	/*  identify  */
				break;
			case 'f':	/*  plot  */
				con->Row=(params[0]<1 ? 1 : params[0]>con->MaxRow ? con->MaxRow : params[0])-1;
				if(sp>1)
					con->Col=(params[1]<1 ? 1 : params[1]>con->MaxCol ? con->MaxCol : params[1])-1;
				else
					con->Col=0;
				break;
			case 'h':	/*  mode set  */
				for(i=0; i<sp; i++)
					switch(params[i]) {
						case 4:
							con->Mode.Insert=TRUE;
							break;
						case 20:
							con->Mode.Newline=TRUE;
							break;
						case 33:
							con->Mode.CursorSteady=TRUE;
							break;
						case 34:
							break;
					}
				break;
			case 'i':	/*  controller print mode  */
				for(i=0; i<sp; i++)
					switch(params[i]) {
						case 4:
							if(con->Printer)
								fclose(con->Printer);
							con->Printer=NULL;
							break;
						case 5:
							con->Printer=fopen("PRT:","w");
							break;
					}
				break;
			case 'l':	/*  mode reset  */
				for(i=0; i<sp; i++)
					switch(params[i]) {
						case 4:
							con->Mode.Insert=FALSE;
							break;
						case 20:
							con->Mode.Newline=FALSE;
							break;
						case 33:
							con->Mode.CursorSteady=FALSE;
							break;
						case 34:
							break;
					}
				break;
			case 'm':	/*  cursor attribute  */
				for(i=0; i<sp; i++) {
					n=params[i];
					if(n==0) {	/*  normal  */
						con->CC.Fore=(con->io->xem_screendepth==3) ? 1 : GRY;
						con->CA.Bold=FALSE;
						con->CC.Back=0;
						con->CA.Rvs=FALSE;
						con->CA.Blink=FALSE;
						con->CA.Dim=FALSE;
						con->CA.Ul=FALSE;
					}
					else
					if(n==1) {	/*  bold  */
						con->CA.Bold=TRUE;
						con->CA.Dim=FALSE;
					}
					else
					if(n==2) {	/*  dim  */
						con->CA.Bold=FALSE;
						con->CA.Dim=TRUE;
					}
					else
					if(n==4)	/*  underline  */
						con->CA.Ul=TRUE;
					else
					if(n==5)	/*  blink  */
						con->CA.Blink=TRUE;
					else
					if(n==7)
						con->CA.Rvs=TRUE;
					else
					if(n==22) {
						con->CA.Bold=FALSE;
						con->CA.Dim=FALSE;
					}
					else
					if(n==25)
						con->CA.Blink=FALSE;
					else
					if(n==27)
						con->CA.Rvs=FALSE;
					else
					if(n>=30 && n<=37) {	/*  foreground color  */
						n-=30;
						if(n)
							if(con->io->xem_screendepth==3)
								n=(con->CA.Bold) ? 3 : (con->CA.Dim) ? 2 : 1;
						con->CA.Dim=FALSE;
						con->CC.Fore=n;
					}
					else
					if(n>=40 && n<=47)	/*  background color  */
						con->CC.Back=(con->io->xem_screendepth==3 && n>40) ? 2 : (n-40);
				}
				current(con);
				break;
			case 'r':
				con->SRtop=(params[0]<1 ? 1 : params[0]>con->MaxRow ? con->MaxRow : params[0])-1;
				if(sp>1)
					con->SRbottom=(params[1]<=con->SRtop ? con->SRtop+1 : params[1]>con->MaxRow ? con->MaxRow : params[1])-1;
				con->Row=0;
				con->Col=0;
				break;
			case 's':
				con->SaveCA=con->CA;
				con->SaveCC=con->CC;
				con->SaveMode=con->Mode;
				con->SaveRow=con->Row;
				con->SaveCol=con->Col;
				break;
			case 'u':
				con->CA=con->SaveCA;
				con->CC=con->SaveCC;
				con->Mode=con->SaveMode;
				con->Row=con->SaveRow;
				con->Col=con->SaveCol;
				current(con);
				break;
		}
	}
	else {
		switch(str[1]) {
			case '=':
				con->Mode.Keypad=TRUE;
				break;
			case '>':
				con->Mode.Keypad=FALSE;
				break;
			case 'D':	/*  index  */
				if(con->Row==con->SRbottom)
					scroll(con,1);
				else
					con->Row++;
				if(con->Row>=con->MaxRow)
					con->Row=con->MaxRow-1;
				break;
			case 'M':	/*  reverse index  */
				if(con->Row==con->SRtop)
					scroll(con,-1);
				else
				if(con->Row)
					con->Row--;
				break;
			case 'c':	/*  reset  */
				con->SRtop=0;
				con->SRbottom=con->MaxRow-1;
				con->CA.Alternate=FALSE;
				con->CA.Blink=FALSE;
				con->CA.Bold=FALSE;
				con->CA.Dim=FALSE;
				con->CA.Protect=FALSE;
				con->CA.Rvs=FALSE;
				con->CA.Ul=FALSE;
				con->CC.Back=BLK;
				con->CC.Fore=(con->io->xem_screendepth==3) ? 1 : GRY;
				con->Mode.CursorSteady=FALSE;
				con->Mode.CursorUnderScore=FALSE;
				con->Mode.CursorVisible=TRUE;
				con->Mode.Draw=FALSE;
				con->Mode.Font=0;
				con->Mode.G0=0;
				con->Mode.G1=0;
				con->Mode.Insert=FALSE;
				con->Mode.Keypad=FALSE;
				WriteConsole(con,"\233H\233J",4);
				break;
			case 'Z':	/*  identify  */
				break;
		}
	}
	MOVE(con->Row,con->Col);
}
void WriteConsole(struct SERCON *con, UBYTE *buf, LONG l)
{
	LONG p;
//	UWORD b
	UWORD i;
//	UBYTE *base
	UBYTE *str;
	UBYTE c,color;

	str=buf;
	if(l==-1)
		l=strlen(str);

	if(con->Printer)
		fprintf(con->Printer,"%*s",l,str);

	if(!con->ScreenBuffer)
		return;

	DrawCursor(con,FALSE);
	while(l--) {
		c=*str++;
		if(con->Mode.Draw) {
			switch(c) {
				case 0x0a:
					con->PXLcol=con->TextFont->tf_XSize*(con->Col+1);
					con->PXLrow++;
					if(con->PXLrow>=con->TextFont->tf_YSize) {
						if(con->Row==con->SRbottom)
							scroll(con,1);
						else
							con->Row++;
						if(con->Row>=con->MaxRow)
							con->Row=con->MaxRow-1;
						con->PXLrow=0;
					}
					con->PXLstart=0;
					break;
				case 0x0b:
					con->io->xem_swrite("\6",1);
					break;
				case 0x0d:
					con->Mode.Draw=FALSE;
					if(con->Row==con->SRbottom)
						scroll(con,1);
					else
						con->Row++;
					if(con->Row>=con->MaxRow)
						con->Row=con->MaxRow-1;
					con->Col=0;
					current(con);
					break;
				default:
					if(c<8) {
						con->PXLstart+=c*15;
						break;
					}
					color=c & 0x0F;
					if(con->io->xem_screendepth==3)
						color=(color==BLK) ? 0 : (color==RED || color==GRN || color==BLU || color==BLACK) ? 2 : (color & 0x08) ? 3 : 1;
					Move(con->rp,con->PXLcol,con->TextFont->tf_YSize*con->Row+con->PXLrow);
//					b=con->PXLcol;
					con->PXLcol+=con->PXLstart+(c>>4);
					SetAPen(con->rp,color);
					Draw(con->rp,con->PXLcol,con->TextFont->tf_YSize*con->Row+con->PXLrow);
					con->PXLstart=0;
/*					for(p=0; p<con->io->xem_screendepth; p++)
						if(color & (1<<p)) {
							base=con->rp->BitMap->Planes[p];
							base+=(15+con->TextFont->tf_YSize*con->Row+con->PXLrow)*con->rp->BitMap->BytesPerRow;
							for(i=b; i<con->PXLcol; i++)
								base[i/8]|=128>>(i%8);
						}
*/					break;
			}
			continue;
		}
		switch(c) {
			case 0x05:	/*  Enq  */
				break;
			case 0x07:	/*  bel  */
				con->io->xem_tbeep(1, 0);
				break;
			case 0x08:	/*  bs  */
				if(con->Col)
					con->Col--;
				MOVE(con->Row,con->Col);
				break;
			case 0x09:	/*  tab  */
				con->Col+=(8-con->Col%8);
				if(con->Col>=con->MaxCol)
					con->Col=con->MaxCol-1;
				MOVE(con->Row,con->Col);
				break;
			case 0x0A:	/*  lf  */
				if(con->Row==con->SRbottom)
					scroll(con,1);
				else
					con->Row++;
				if(con->Row>=con->MaxRow)
					con->Row=con->MaxRow-1;
				if(con->Mode.Newline)
					con->Col=0;
				MOVE(con->Row,con->Col);
				break;
			case 0x0C:	/*  ff  */
				WriteConsole(con,"\33[H\33[J",6);
				break;
			case 0x0D:	/*  cr  */
				con->Col=0;
				MOVE(con->Row,con->Col);
				break;
			case 0x0E:	/*  SO  */
				con->Mode.Font=1;
				con->CA.Alternate=(con->Mode.G1);
				break;
			case 0x0F:	/*  SI  */
				con->Mode.Font=0;
				con->CA.Alternate=(con->Mode.G0);
				break;
			case 0x18:	/*  CAN  */
				con->EscapeBuffer[0]=NULL;
				break;
			case 0x1B:	/*  esc  */
				con->EscapeBuffer[0]=c;
				con->EscapeBuffer[1]=NULL;
				break;
			case 0x9B:	/*  csi  */
				con->EscapeBuffer[0]=c;
				con->EscapeBuffer[1]=NULL;
				break;
			default:
				if(con->EscapeBuffer[0]) {
					p=strlen(con->EscapeBuffer);
					con->EscapeBuffer[p++]=c;
					con->EscapeBuffer[p]=NULL;
					if(p==2) {
						if(!strncmp(con->EscapeBuffer,"\33$",2)) {
							con->Mode.Draw=TRUE;
							con->PXLcol=con->TextFont->tf_XSize*(con->Col+1);
							con->PXLrow=0;
							con->PXLstart=0;
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33\67",2)) {
							con->SaveCA=con->CA;
							con->SaveCC=con->CC;
							con->SaveMode=con->Mode;
							con->SaveRow=con->Row;
							con->SaveCol=con->Col;
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33\70",2)) {
							con->CA=con->SaveCA;
							con->CC=con->SaveCC;
							con->Mode=con->SaveMode;
							con->Row=con->SaveRow;
							con->Col=con->SaveCol;
							current(con);
							con->EscapeBuffer[0]=NULL;
							break;
						}
					}
					if(p==3) {
						if(!strncmp(con->EscapeBuffer,"\33#",2)) {
							switch(c) {
								case 3:	/*  top half  */
									break;
								case 4:	/*  bottom half  */
									break;
								case 5:	/*  normal line  */
									break;
								case 6:	/*  double-width  */
									break;
							}
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33(",2)) {
							con->Mode.G0=(c=='0') ? TRUE : FALSE;
							if(con->Mode.Font==0)
								con->CA.Alternate=(con->Mode.G0);
							con->EscapeBuffer[0]=NULL;
							break;
						}
						if(!strncmp(con->EscapeBuffer,"\33)",2)) {
							con->Mode.G1=(c=='0') ? TRUE : FALSE;
							if(con->Mode.Font==1)
								con->CA.Alternate=(con->Mode.G1);
							con->EscapeBuffer[0]=NULL;
							break;
						}
					}
					if(!strchr("\"#()0123456789;?[",c) || p==255) {
						DecodeANSI(con,con->EscapeBuffer);
						con->EscapeBuffer[0]=NULL;
					}
					break;
				}
				if(con->Col>=con->MaxCol)
					WriteConsole(con,"\15\12",2);
				if(c<' ' || c==0x9b)
					break;
				for(p=0; p<l && str[p]>=' ' && str[p]!=0x9b && con->Col+1+p<con->MaxCol; p++);
				p++;
				if(con->Mode.Insert) {
					char ins[8];
					sprintf(ins,"\233%u@",p);
					WriteConsole(con,ins,strlen(ins));
				}
				for(i=0; i<p; i++) {
					con->ScreenBuffer[con->Row*con->MaxCol+con->Col+i]=(con->CA.Alternate && c>='\137' && c<='\176') ? alternate[c-'\137'] : c;
					con->AttrBuffer[con->Row*con->MaxCol+con->Col+i]=con->CA;
					con->ColorBuffer[con->Row*con->MaxCol+con->Col+i]=con->CC;
					c=*str++;
				}
				Text(con->rp,&con->ScreenBuffer[con->Row*con->MaxCol+con->Col],p);
				con->Col+=p;
				l-=p-1;str--;
				break;
		}
	}
	DrawCursor(con,TRUE);
}
