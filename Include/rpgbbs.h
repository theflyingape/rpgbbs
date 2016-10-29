#include <devices/serial.h>
#include <devices/timer.h>
#include "/include/rpgbbs_proto.h"
#ifndef RPGBBSLIB
	#include "/include/rpgbbs_pragma.h"
#endif

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
	struct IOExtSer *SerialIO;
	struct MsgPort *SerialMP,*TPort,*IconMP;
	struct timerequest *TimerIO;
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
};
