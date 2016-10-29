/******************\
 *  RPGBBS Logon  *
\******************/
#include "RPGBBS.h"

UWORD __chip MEData[] = {
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x00f8,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x03fe,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x07ff,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x01fc,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x00f8,0x0000,0x8000,0x0000,
	0x0000,0x0000,0x0222,0x0003,0xe000,0x0000,
	0x0000,0x0000,0x0800,0x8001,0x4000,0x0000,
	0x0000,0x0000,0x0850,0x8000,0x0000,0x0000,
	0x0000,0x0000,0x0c01,0x8000,0x0000,0x0000,
	0x0000,0x0000,0x1e03,0xc000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0070,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x00ff,0xf000,0x0000,0x0000,
	0x0000,0x0000,0x0070,0xa000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	/**/
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0xe000,0x0000,
	0x0000,0x0000,0x0202,0x0007,0xf000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0xe000,0x0000,
	0x0000,0x0000,0x0050,0x0001,0xc000,0x0000,
	0x0000,0x0000,0x0000,0x0001,0xc000,0x0000,
	0x0000,0x0000,0x0000,0x0001,0xc000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0007,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0007,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0007,0x0000,0x0000,
	0x0000,0x007f,0xc000,0x000e,0x0000,0x0000,
	0x0000,0x0380,0x3800,0x000c,0x0000,0x0000,
	0x0000,0x0404,0x0400,0x001c,0x0000,0x0000,
	0x0000,0x0840,0x4200,0x0038,0x0000,0x0000,
	0x0000,0x1004,0x0100,0x0070,0x0000,0x0000,
	0x0000,0x111b,0x1100,0x0070,0x0000,0x0000,
	0x0000,0x1004,0x0100,0x0070,0x0000,0x0000,
	0x0000,0x0840,0x4370,0x0070,0x0000,0x0000,
	0x0000,0x0404,0x0400,0x0000,0x0000,0x0000,
	0x0000,0x0380,0x3800,0x0000,0x0000,0x0000,
	0x0000,0x007f,0xc000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0070,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0070,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0038,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x001c,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x000e,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0007,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0003,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x0007,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x000e,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x001c,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x003c,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x003e,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x003e,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x001e,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x000c,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	/**/
	0x0000,0x0ff0,0x0000,0x007f,0x8000,0x0000,
	0x0000,0x755c,0x0000,0x01d5,0x7000,0x0000,
	0x0001,0xaaab,0x0000,0x06aa,0xac00,0x0000,
	0x0007,0x5555,0x8000,0x0d55,0x5700,0x0000,
	0x001a,0xaaaa,0xce03,0x9aaa,0xaac0,0x0000,
	0x0075,0x5555,0x5104,0x5557,0xf570,0x0000,
	0x01aa,0xaaaa,0xd0d8,0x5aaf,0xfaac,0x0000,
	0x0355,0x5555,0x6707,0x3557,0xf556,0x0000,
	0x06aa,0xaaaa,0xa451,0x2aab,0x6aab,0x0000,
	0x0d55,0x5555,0x6202,0x3554,0x1555,0x8000,
	0x1aaa,0xaaaa,0xa104,0x2aaa,0x2aaa,0xc000,
	0x3555,0x5555,0x48f9,0xd554,0x5555,0x6000,
	0x2aaa,0xabff,0xe000,0x7ffc,0x2aaa,0xa000,
	0x3555,0x5691,0x2244,0x8910,0x5555,0x6000,
	0x6aaa,0xa800,0x0000,0x0000,0xaaaa,0xb000,
	0x5555,0x5a24,0x4891,0x2240,0xd555,0x5000,
	0x6aaa,0xa804,0x0000,0x0100,0xaaaa,0xb000,
	0x5555,0x54ff,0xe244,0x8911,0x5555,0x5000,
	0x6aaa,0xaf80,0x3800,0x0201,0xaaaa,0xb000,
	0xd555,0x5c04,0x0491,0x2240,0xd555,0x5800,
	0xaaaa,0xb840,0x4200,0x0200,0x6aaa,0xa800,
	0xd555,0x7004,0x0144,0x8d02,0x3555,0x5800,
	0xaaab,0xf11b,0x1100,0x0880,0x3eaa,0xa800,
	0xd556,0x3004,0x01f1,0x3004,0xa355,0x5800,
	0xaab8,0x1840,0x428f,0xf00f,0xc0ea,0xa800,
	0xd560,0x0c04,0x07ff,0xf8ff,0x8035,0x5800,
	0xaac0,0x0380,0x3fff,0xf9fc,0x001a,0xa800,
	0xd580,0x007f,0xffff,0xf0f8,0x000d,0x5800,
	0xab00,0x0000,0x87ff,0x0800,0x0006,0xa800,
	0x5600,0x0000,0xa2fc,0x8800,0x0003,0x5000,
	0x6c00,0x0001,0x0070,0x0400,0x0001,0xb000,
	0x1800,0x0002,0x48b1,0x2200,0x0000,0xc000,
	0x0000,0x0002,0x0020,0x0200,0x0000,0x0000,
	0x0000,0x0003,0x2254,0x8a00,0x0000,0x0000,
	0x0000,0x0002,0x0050,0x0200,0x0000,0x0000,
	0x0000,0x0002,0x4889,0x2200,0x0000,0x0000,
	0x0000,0x0001,0x0088,0x0400,0x0000,0x0000,
	0x0000,0x0001,0x2304,0x8c00,0x0000,0x0000,
	0x0000,0x0001,0x3104,0x6400,0x0000,0x0000,
	0x0000,0x0002,0xfa03,0xfa00,0x0000,0x0000,
	0x0000,0x0007,0xff07,0xff00,0x0000,0x0000,
	0x0000,0x0003,0xfe03,0xfe00,0x0000,0x0000,
	0x0000,0x0001,0xfc01,0xfc00,0x0000,0x0000,
	0x0000,0x0000,0xf800,0xf800,0x0000,0x0000,
	0x0000,0x0001,0xfc01,0xfc00,0x0000,0x0000,
	0x0000,0x0003,0xe000,0x3e00,0x0000,0x0000,
};

struct Image ME = {
	0,0,
	85,46,3,
	&MEData[0],
	7,0,
	NULL
};

struct IntuiText __far project_shutdown = {
	0,0,JAM1,1,1,NULL,"Quit",NULL
};
struct MenuItem __far shutdown = {
	NULL,					/*	NextItem			*/
	0,48,					/*	LeftEdge, TopEdge	*/
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
struct IntuiText __far project_iconify = {
	0,0,JAM1,1,1,NULL,"Iconify",NULL
};
struct MenuItem __far iconify = {
	&shutdown,					/*	NextItem			*/
	0,36,					/*	LeftEdge, TopEdge	*/
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
struct IntuiText __far project_viewnme = {
	0,0,JAM1,1,1,NULL,"Enemy",NULL
};
struct MenuItem viewnme = {
	NULL,					/*	NextItem			*/
	72,12,					/*	LeftEdge, TopEdge	*/
	96,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_viewnme,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'2',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_viewuser = {
	0,0,JAM1,1,1,NULL,"User",NULL
};
struct MenuItem __far viewuser = {
	&viewnme,				/*	NextItem			*/
	72,0,					/*	LeftEdge, TopEdge	*/
	96,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_viewuser,/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'1',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_view = {
	0,0,JAM1,1,1,NULL,"View         »",NULL
};
struct MenuItem __far view = {
	&iconify,				/*	NextItem			*/
	0,24,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_view,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	NULL,					/*	Command				*/
	&viewuser,				/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_sysop = {
	0,0,JAM1,1,1,NULL,"Sysop",NULL
};
struct MenuItem __far sysop = {
	&view,					/*	NextItem			*/
	0,12,					/*	LeftEdge, TopEdge	*/
	128,10,					/*	Width, Height		*/
	ITEMTEXT |				/*	Flags				*/
	ITEMENABLED |
	COMMSEQ |
	HIGHCOMP,
	0,						/*	MutualExclude		*/
	(APTR)&project_sysop,	/*	ItemFill			*/
	NULL,					/*	SelectFill			*/
	'S',					/*	Command				*/
	NULL,					/*	SubItem				*/
	0						/*	NextSelect			*/
};
struct IntuiText __far project_logon = {
	0,0,JAM1,1,1,NULL,"Logon",NULL
};
struct MenuItem __far Mlogon = {
	&sysop,					/*	NextItem			*/
	0,0,					/*	LeftEdge, TopEdge	*/
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
	&sysop,					/*	NextItem			*/
	0,0,					/*	LeftEdge, TopEdge	*/
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
struct Menu __far project = {
	NULL,					/*	NextMenu			*/
	0,0,					/*	LeftEdge, TopEdge	*/
	80,10,					/*	Width, Height		*/
	MENUENABLED,			/*	Flags				*/
	" Project ",			/*	MenuName			*/
	&Mlogon					/*	MenuItem			*/
};

void AutoVerify(void)
{
	FILE *fp;
	int area,i,retry;
	char c,dial[41];

	if(USER(user)->DataPhone==0. || local)
		return;
	c=0;
	for(i=1; i<=MAXUSER; i++)
		if(USER(i)->DataPhone==USER(user)->DataPhone)
			c++;
	if(c>2)
		return;
	area=USER(user)->DataPhone/10000000.;
	if(area==900)
		return;
	area=USER(user)->DataPhone/10000.;
	area%=1000;
	if(area==911)
		return;
	newline();
	outs("You can be auto-verified by letting me call your computer now.");
	newline();
	outs("You would need to put your modem in Auto-Answer mode (AT S0=1)");
	newline();
	newline();
	do {
		outs("Do you wish to be auto-verified now (Y/N)? ");
		if(!(c=inkey()))
			break;
		newline();
	} while(c!='Y' && c!='N');
	newline();
	if(c=='N')
		return;
	outs("Ok.  Please tell me which dial command I will need to use:");
	newline();
	newline();
	sprintf(line[0],"%010f",(USER(user)->DataPhone==0.) ? USER(user)->VoicePhone : USER(user)->DataPhone);
	sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
	sprintf(text,"<1> Local: ATDT %s",&line[1][4]);
	outs(text);
	newline();
	if(BBSprefs.Auto>1) {
		sprintf(text,"<2> Toll: ATDT 1-%s",&line[1][4]);
		outs(text);
		newline();
	}
	if(BBSprefs.Auto>2) {
		sprintf(text,"<3> Long distance: ATDT 1-%s",&line[1][0]);
		outs(text);
		newline();
	}
	outs("<4> Forget it.");
	newline();
	newline();
	do {
		outs("Select (1-4): ");
		if(input())
			break;
		i=atoi(ins);
		newline();
	} while(i<1 || i>4);
	newline();
	switch(i) {
		case 1:
			sprintf(dial,"ATDT %s\15",&line[1][4]);
			break;
		case 2:
			if(BBSprefs.Auto>1)
				sprintf(dial,"ATDT 1-%s\15",&line[1][4]);
			break;
		case 3:
			if(BBSprefs.Auto>2)
				sprintf(dial,"ATDT 1-%s\15",&line[1][0]);
			break;
		default:
			memset(dial,0,sizeof(dial));
			break;
	}
	if(!strlen(dial))
		return;
	outs("Ok, please HANG UP your modem NOW and don't forget to set your modem to");
	newline();
	outs("Auto-Answer mode by typing ATS0=1 at the OK prompt IMMEDIATELY.");
	newline();
	outs("-OR- if you start getting RING messages, type ATA to answer.");
	newline();
	outs("-OR- wait a minute to abort entire procedure...");
	for(i=1000; i>0 && !cd(); i--) {
		if(i%100==0)
			beep();
		Delay(5);
	}
	if(i) {
		for(retry=0; retry<2 && cd(); retry++) {
			Delay(50);
			WriteSerial("ATS0=0\15");
			Delay(50);
			WriteConsole("\nClear buffer...");
			SerialIO->IOSer.io_Command=SDCMD_QUERY;
			DoIO((struct IORequest *)SerialIO);
			if(SerialIO->IOSer.io_Actual) {
				SerialIO->IOSer.io_Command=CMD_READ;
				SerialIO->IOSer.io_Length=SerialIO->IOSer.io_Actual;
				SerialIO->IOSer.io_Data=(APTR)INbuf;
				DoIO((struct IORequest *)SerialIO);
			}
			WriteConsole("\nSend dial command...");
			WriteSerial(dial);
			for(i=2000; i>0 && cd(); i--)
				Delay(1);
		}
		Delay(50);
		WriteConsole("\nClear buffer...");
		SerialIO->IOSer.io_Command=SDCMD_QUERY;
		DoIO((struct IORequest *)SerialIO);
		if(SerialIO->IOSer.io_Actual) {
			SerialIO->IOSer.io_Command=CMD_READ;
			SerialIO->IOSer.io_Length=SerialIO->IOSer.io_Actual;
			SerialIO->IOSer.io_Data=(APTR)INbuf;
			DoIO((struct IORequest *)SerialIO);
		}
		if(!cd()) {
			GetSystemDateTime();
			newline();
			sprintf(text,"%.2s/%.2s/%.4s",&SystemDate[0],&SystemDate[2],&SystemDate[4]);
			julian(text);
			sprintf(text,"Calling from %s.  %s  %s  %s",USER(0)->Name,__daytbl[weekday],Fdate(SystemDate),SystemTime);
			outs(text);
			for(i=0; i<3; i++) {
				newline();
				sprintf(text,"%s%.26s, type your password: ",Fcol(CYN),USER(user)->Handle);
				outs(text);
				echo=FALSE;
				if(input()) {
					i=3;
					break;
				}
				newline();
				if(!strnicmp(USER(user)->Password,ins,sizeof(USER(user)->Password))) {
					USER(user)->Access=4;
					PutUser(user);
					newline();
					outs("THANK YOU!  You have been verified to play.  Call back again!");
					newline();
					newline();
					Delay(100);
					break;
				}
			}
			if(i==3) {
				if((fp=fopen("BadPhone.list","a"))) {
					fprintf(fp,"%s  ;%s  %.30s\n",&line[1],Fdate(SystemDate),USER(user)->Name);
					fclose(fp);
					USER(user)->Access=2;
					PutUser(user);
				}
			}
		}
		logoff=TRUE;
	}
	else {
		beep();
		outs("Auto-verify aborted.");
	}
}

void TaxMan(void)
{
	double tax;
	int i;
	char c;

	if((double)USER(user)->TotalPosts/(double)USER(user)->TotalCalls<USER(0)->Experience) {
		tax=USER(user)->Gold+USER(user)->Bank;
		JOUST(USER(user)->Level);
		if(tax<d)
			tax=d;
		d=tax*(USER(0)->Experience-(double)USER(user)->TotalPosts/(double)USER(user)->TotalCalls);
		modf(d,&tax);
		if(tax<1.)
			tax=1.;
		sprintf(text,"%.26s, the tax collector, says, \"You must do your part to keep",USER(0)->Handle);
		outs(text);
		newline();
		outs("this fine city running!\"  You check out the two burly guards who stand ready");
		newline();
		outs("to enforce the king's will and decide to pay the tax.");
		newline();
		newline();
		Delay(50);
		sprintf(text,"The tax will cost you %s.",money(tax,user));
		outs(text);
		newline();
		newline();
		Delay(50);
		USER(user)->Bank-=tax;
		if(USER(user)->Bank<0.) {
			USER(user)->Gold+=USER(user)->Bank;
			USER(user)->Bank=0.;
		}
		if(USER(user)->Gold<0.) {
			USER(user)->Loan-=USER(user)->Gold;
			USER(user)->Gold=0.;
		}
		PutUser(user);
		USER(0)->Bank+=tax;
		sprintf(text,"Your post/call ratio = %g",(double)USER(user)->TotalPosts/(double)USER(user)->TotalCalls);
		outs(text);
		newline();
		newline();
	}
	JOUST(USER(user)->Level);
	d*=1000.;
	if(USER(user)->Gold+USER(user)->Bank>d) {
		tax=USER(user)->Gold+USER(user)->Bank-d;
		sprintf(text,"%.26s, the tax collector, looks at your bulging sacks of gold",USER(0)->Handle);
		newline();
		outs(text);
		newline();
		outs("and says, \"Ah, it is time to pay your taxes!\"  You check out the two burly");
		newline();
		outs("guards who stand ready to enforce the king's will.");
		newline();
		newline();
		Delay(100);
		sprintf(text,"The tax will cost you %s.",money(tax,user));
		outs(text);
		newline();
		newline();
		Delay(100);
		do {
			outs(Fcol(CYN));
			outs("Will you pay the tax (Y/N)? ");
			if(!(c=inkey()))
				c='Y';
			newline();
		} while(c!='Y' && c!='N');
		newline();
		if(c=='N') {
			nme=0;
			outs("The tax collector orders his first guard, \"Run this outlaw through!\"");
			newline();
			newline();
			Delay(100);
			for(i=0; i<MAXAM-1; i++)
				if(ARENA(i+1)->Level>((int)USER(user)->Level/3))
					break;
			memset(&NME,0,sizeof(struct User));
			strcpy(NME.Handle,"City Guard #1");
			NME.Sex='M';
			NME.Class=ARENA(i)->Class;
			NME.Level=ARENA(i)->Level;
			NME.ExpLevel=ARENA(i)->Level;
			NME.HP=ARENA(i)->HP;
			NME.SP=ARENA(i)->SP;
			NME.Str=ARENA(i)->Str;
			NME.Int=ARENA(i)->Int;
			NME.Agl=(ARENA(i)->Agl>Tagl) ? ARENA(i)->Agl : Tagl;
			NME.Weapon=ARENA(i)->Weapon;
			NME.Armor=ARENA(i)->Armor;
			NME.Spell=ARENA(i)->Spell;
			NME.Gold=ARENA(i)->Gold;
			ArenaPixFile();
			Battle();
			if(Thp>0 && NMEhp>0)
				c='Y';
		}
		if(c=='N') {
			nme=0;
			outs("The tax collector shouts at his second guard, \"Kill!\"");
			newline();
			newline();
			Delay(100);
			for(i=0; i<MAXAM-1; i++)
				if(ARENA(i+1)->Level>((int)USER(user)->Level/2))
					break;
			memset(&NME,0,sizeof(struct User));
			strcpy(NME.Handle,"City Guard #2");
			NME.Sex='M';
			NME.Class=ARENA(i)->Class;
			NME.Level=ARENA(i)->Level;
			NME.ExpLevel=ARENA(i)->Level;
			NME.HP=ARENA(i)->HP;
			NME.SP=ARENA(i)->SP;
			NME.Str=ARENA(i)->Str;
			NME.Int=ARENA(i)->Int;
			NME.Agl=(ARENA(i)->Agl>Tagl) ? ARENA(i)->Agl : Tagl;
			NME.Weapon=ARENA(i)->Weapon;
			NME.Armor=ARENA(i)->Armor;
			NME.Spell=ARENA(i)->Spell;
			NME.Gold=ARENA(i)->Gold;
			ArenaPixFile();
			Battle();
			if(Thp>0 && NMEhp>0)
				c='Y';
		}
		if(c=='N') {
			nme=0;
			memset(&NME,0,sizeof(struct User));
			CreateMonster(&NME,MAXCLASS-3,(int)USER(user)->Level*4/5+1);
			strncpy(NME.Handle,USER(0)->Handle,sizeof(NME.Handle));
			NME.Sex='M';
			NME.Str=99;
			NME.Int=99;
			NME.Agl=99;
			NME.Gold=USER(0)->Bank;
			if(NME.Gold>tax)
				NME.Gold=tax;
			for(i=0; i<MAXWEAPON; i++)
				if(NME.Gold<WEAPON(i)->Cost)
					break;
			if(i>NME.Weapon)
				NME.Weapon=i-1;
			for(i=0; i<MAXARMOR; i++)
				if(NME.Gold<ARMOR(i)->Cost)
					break;
			if(i>NME.Armor)
				NME.Armor=i-1;
			outs("The tax collector ");
			switch(dice(3)) {
				case 1:
					outs("mutters, \"Good help is hard to find these days...\"");
					break;
				case 2:
					outs("sighs, \"If you want a job done right...\"");
					break;
				case 3:
					outs("swears, \"That's gonna cost you.\"");
					break;
			}
			newline();
			newline();
			Delay(100);
			sprintf(text,"He pulls out his %s.",WEAPON(NME.Weapon)->Name);
			outs(text);
			newline();
			newline();
			Delay(100);
			sprintf(text,"He is wearing his %s.",ARMOR(NME.Armor)->Name);
			outs(text);			
			newline();
			newline();
			Delay(100);
			from='L';	/*  You cannot retreat from the tax collector */
			Battle();
			if(Thp>0) {
				USER(0)->Bank-=tax;
				if(USER(0)->Bank<0.)
					USER(0)->Bank=0.;
			}
			else {
				outs("\"Thanks for the taxes!\"");
				newline();
				newline();
				c='Y';
			}
		}
		if(c=='Y') {
			USER(user)->Bank-=tax;
			if(USER(user)->Bank<0.) {
				USER(user)->Gold+=USER(user)->Bank;
				USER(user)->Bank=0.;
			}
			if(USER(user)->Gold<0.) {
				USER(user)->Loan-=USER(user)->Gold;
				USER(user)->Gold=0.;
			}
			PutUser(user);
			USER(0)->Bank+=tax;
			PutUser(0);
			outs("...and you pay the tax.");
			newline();
			Delay(100);
		}
	}
}

int Logon(void)
{
	FILE *fp;
	ULONG SerialSignal,ConsoleSignal,UserSignal,NMESignal,IconSignal,signal;
	int i,l,queue,retry,ring;
	char c,*connect,callerid[12],result[64];

	sprintf(title,"RPGclient %s                             ©1992-1994 Robert Hurst",__AMIGADATE__);
	if(window) {
		SetWindowTitles(window,title,title);
		ClearMenuStrip(window);
	}
	project.FirstItem=&Mlogon;
	if(window)
		SetMenuStrip(window,&project);
	if(strlen(BBSprefs.Device))
		sprintf(icontitle,"RPGBBS #%d %s",BBSprefs.Unit,BBSprefs.Device);
	else
		strcpy(icontitle,"RPGBBS local console");
	if(iconwindow)
		SetWindowTitles(iconwindow,icontitle,(UBYTE *)-1);
	user=0;
	local=(strlen(BBSprefs.Device)) ? FALSE : TRUE;
	baud=BBSprefs.Baud;
	BoldFlag=FALSE;
	grace=FALSE;
	memset(result,0,sizeof(result));

	ANSI=2;
	WriteConsole("\33c\n");
	sprintf(text,"%s%*s°±²%s%s %s %s%s²±°\n",Fcol(BLU),36-strlen(USER(0)->Name)/2,"",Bcol(BLU),Fcol(WHITE),USER(0)->Name,Bcol(BLK),Fcol(BLU));
	WriteConsole(text);
	sprintf(text,"%lu calls",USER(0)->TotalCalls);
	sprintf(OUTbuf,"%*s%s%s\n\n",40-strlen(text)/2,"",Fcol(CYN),text);
	WriteConsole(OUTbuf);
	sprintf(OUTbuf,"%*s%s\n",40-strlen(USER(0)->Street[0])/2,"",USER(0)->Street[0]);
	WriteConsole(OUTbuf);
	sprintf(OUTbuf,"%*s%s\n\n\n",40-strlen(USER(0)->Street[1])/2,"",USER(0)->Street[1]);
	WriteConsole(OUTbuf);
	if(window && BBSprefs.Depth>2) {
		DrawImage(window->RPort,&ME,72+8*strlen(USER(0)->Name)/2,17);
		DrawImage(window->RPort,&ME,490-8*strlen(USER(0)->Name)/2,17);
	}
	if(!local) {
		sprintf(OUTbuf,"%sPort:%s %s (#%u)",Fcol(CYN),Fcol(GRY),BBSprefs.Device,BBSprefs.Unit);
		WriteConsole(OUTbuf);
		if(!(SPort=(struct MsgPort *)CreatePort(0,0)))
			return(FALSE);
		if(!(SerialIO=(struct IOExtSer *)CreateExtIO(SPort,sizeof(struct IOExtSer))))
			return(FALSE);
		SerialIO->io_SerFlags=SERF_SHARED | SERF_7WIRE;
		if(OpenDevice(BBSprefs.Device,BBSprefs.Unit,(struct IORequest *)SerialIO,0))
			return(FALSE);
		sprintf(OUTbuf,", %u baud, 8-bit word, No parity, 1-stop bit\n\n",baud);
		WriteConsole(OUTbuf);
		SerialIO->IOSer.io_Command=SDCMD_SETPARAMS;
		SerialIO->io_CtlChar=SER_DEFAULT_CTLCHAR;
		SerialIO->io_RBufLen=sizeof(INbuf);
		SerialIO->io_Baud=baud;
		SerialIO->io_ReadLen=8;
		SerialIO->io_WriteLen=8;
		SerialIO->io_SerFlags=SERF_XDISABLED | SERF_SHARED | SERF_7WIRE;
		SerialIO->io_StopBits=1;
		if((i=DoIO((struct IORequest *)SerialIO))) {
			sprintf(OUTbuf,"Serial error #%u",i);
			WriteConsole(OUTbuf);
			newline();
			Delay(100);
			return(FALSE);
		}
		logoff=FALSE;
		for(i=0; i<3 && !cd(); i++) {
			WriteSerial("+++");
			Delay(75);
			WriteSerial("ATH\15");
			Delay(75);
		}
		WriteSerial("\15ATZ\15");
		Delay(75);
		WriteSerial(BBSprefs.Init);
	}
	else {
		sprintf(OUTbuf,"%s>>>%s Local Console %s<<<\n\n",Fcol(CYN),Fcol(GRY),Fcol(CYN));
		WriteConsole(OUTbuf);
	}
	sprintf(OUTbuf,"%sLast callers were:%s ",Fcol(CYN),Fcol(GRY));
	WriteConsole(OUTbuf);
	for(i=0; i<3; i++) {
		sprintf(OUTbuf,"%.26s %s(%s%s%s)%s\n",RPGinfo->LastCaller[i],Fcol(CYN),Fcol(GRY),RPGinfo->Reason[i],Fcol(CYN),Fcol(GRY));
		WriteConsole(OUTbuf);
		WriteConsole("                   ");
	}
	WriteConsole("\n");
	GetSystemDateTime();
	WriteConsole(Fdate(SystemDate));
	WriteConsole("  ");
	WriteConsole(SystemTime);
	WriteConsole("  Waiting for a caller...\n\n");
	WriteConsole("\33[17;23r");

	local=TRUE;
	Midnight();
	local=(strlen(BBSprefs.Device)) ? FALSE : TRUE;
	logoff=TRUE;
	carrier=0;
	memset(callerid,0,sizeof(callerid));
	ring=0;
	SerialSignal=(SerialIO) ? (1L << SPort->mp_SigBit) : 0;
	queue=SerialSignal;
	while(logoff) {
		if(queue) {	/*  queue a request for Wait()  */
			SerialIO->IOSer.io_Command=CMD_READ;
			SerialIO->IOSer.io_Length=1;
			SerialIO->IOSer.io_Data=(APTR)INbuf;
			SendIO((struct IORequest *)SerialIO);
			queue=FALSE;
		}
		ConsoleSignal=(window) ? (1L << window->UserPort->mp_SigBit) : 0;
		UserSignal=(userwindow) ? (1L << userwindow->UserPort->mp_SigBit) : 0;
		NMESignal=(nmewindow) ? (1L << nmewindow->UserPort->mp_SigBit) : 0;
		IconSignal=(iconwindow) ? (1L << iconwindow->UserPort->mp_SigBit) : 0;
		signal=Wait(SerialSignal | ConsoleSignal | UserSignal | NMESignal | IconSignal);
		if(signal & SerialSignal) {
			if(CheckIO((struct IORequest *)SerialIO)) {
				WaitIO((struct IORequest *)SerialIO);
				queue=TRUE;
				if(INbuf[0]) {
					c=toupper(INbuf[0]);
					sprintf(OUTbuf,"%c",INbuf[0]);
					WriteConsole(OUTbuf);
					if(iscntrl(c)) {
						baud=0;
						connect=strchr(result,'C');
						while(connect && !baud) {
							if(strncmp(connect,"CARRIER",7)==0)
								carrier=atoi(connect+7);
							if(strncmp(connect,"CONNECT",7)==0) {
								baud=atoi(connect+7);
								if(!baud)
									baud=300;
								if(!carrier)
									carrier=baud;
							}
							else
								connect=strchr(++connect,'C');
						}
						if((connect=strchr(result,'N')))
							if(strncmp(connect,"NMBR",4)==0)
								strncpy(callerid,connect+7,10);
						if((connect=strchr(result,'R')))
							if(strncmp(connect,"RING",4)==0) {
								if(++ring>3) {
									ring=0;
									WriteSerial("ATA\15");
								}
							}
						memset(result,0,sizeof(result));
						if(baud>75 && BBSprefs.Baud<=2400) {
							SerialIO->IOSer.io_Command=SDCMD_SETPARAMS;
							SerialIO->io_Baud=baud;
							if(DoIO((struct IORequest *)SerialIO)) {
								sprintf(OUTbuf,"\nERROR setting device to %d baud!\n",baud);
								WriteConsole(OUTbuf);
							}
							for(retry=0; retry<50; retry++) {
								Delay(1);	/*  wait for A1000 to catch its breath...  */
								SerialIO->IOSer.io_Command=SDCMD_QUERY;
								DoIO((struct IORequest *)SerialIO);
								if(!(SerialIO->io_Status & 32))
									break;
							}
						}
					}
					else {
						l=strlen(result);
						result[l++]=c;
						result[l]='\0';
					}
				}
			}
			SerialIO->IOSer.io_Command=SDCMD_QUERY;
			DoIO((struct IORequest *)SerialIO);
			if(!(SerialIO->io_Status & 32) && baud) {
				local=FALSE;
				logoff=FALSE;
				newline();
				sprintf(text,"%d baud connection established.",carrier);
				outs(text);
				newline();
				newline();
				Delay(50);
			}
		}
		if(signal & ConsoleSignal)
			if(window)
				while((message=(struct IntuiMessage *)GetMsg(window->UserPort))) {
					MessageClass=message->Class;
					code=message->Code;
					qualifier=message->Qualifier;
					ReplyMsg((struct Message *)message);
					if(MessageClass & MENUPICK) {
						menu=MENUNUM(code);
						menuitem=ITEMNUM(code);
						subitem=SUBNUM(code);
						switch(menu) {
							case 0:	/* Project */
								switch(menuitem) {
									case 0:	/* Logon */
										if(SerialSignal) {
											AbortIO((struct IORequest *)SerialIO);
											WaitIO((struct IORequest *)SerialIO);
										}
										local=TRUE;
										logoff=FALSE;
										baud=0;
										carrier=0;
										newline();
										outs(">> Local logon << ");
										OffHook();
										break;
									case 1:	/* Sysop */
										if(SerialSignal) {
											AbortIO((struct IORequest *)SerialIO);
											WaitIO((struct IORequest *)SerialIO);
										}
										local=TRUE;
										logoff=FALSE;
										baud=0;
										carrier=0;
										newline();
										outs(">> Sysop logon << ");
										OffHook();
										newline();
										newline();
										sprintf(text,"%s%.26s, type your password: ",Fcol(CYN),USER(1)->Handle);
										outs(text);
										echo=FALSE;
										if(input())
											logoff=TRUE;
										echo=TRUE;
										newline();
										if(strnicmp(USER(1)->Password,ins,sizeof(USER(1)->Password))==0) {
											user=1;
											acclvl=USER(user)->Access;
											Sysop();
											user=0;
										}
										Logoff();
										return(TRUE);
										break;
									case 2:	/* View */
										switch(subitem) {
											case 0:	/* User */
												if(userwindow) {
													CloseWindow(userwindow);
													userwindow=NULL;
													ActivateWindow(window);
												}
												else {
													userwindow=(struct Window *)OpenWindow(&newuserwindow);
													DisplayUser();
												}
												break;
											case 1: /* Enemy */
												if(nmewindow) {
													CloseWindow(nmewindow);
													nmewindow=NULL;
													ActivateWindow(window);
												}
												else {
													nmewindow=(struct Window *)OpenWindow(&newnmewindow);
													DisplayNME();
												}
												break;
										}
										break;
									case 3:	/* Iconify */
										CloseGUI();
										newiconwindow.LeftEdge=BBSprefs.IconX;
										newiconwindow.TopEdge=BBSprefs.IconY;
										iconwindow=(struct Window *)OpenWindow(&newiconwindow);
										break;
									case 4:	/* Shutdown */
										if(SerialIO) {
											if(!CheckIO((struct IORequest *)SerialIO))
												AbortIO((struct IORequest *)SerialIO);
											WaitIO((struct IORequest *)SerialIO);
										}
										return(FALSE);
										break;
								}
								break;
						}
					}
				}
		if(signal & UserSignal)
			if(userwindow)
				while((message=(struct IntuiMessage *)GetMsg(userwindow->UserPort))) {
					MessageClass=message->Class;
					code=message->Code;
					ReplyMsg((struct Message *)message);
					if(MessageClass & CLOSEWINDOW) {
						CloseWindow(userwindow);
						userwindow=NULL;
					}
					ActivateWindow(window);
				}
		if(signal & NMESignal)
			if(nmewindow)
				while((message=(struct IntuiMessage *)GetMsg(nmewindow->UserPort))) {
					MessageClass=message->Class;
					code=message->Code;
					ReplyMsg((struct Message *)message);
					if(MessageClass & CLOSEWINDOW) {
						CloseWindow(nmewindow);
						nmewindow=NULL;
					}
					ActivateWindow(window);
				}
		if(signal & IconSignal)
			if(iconwindow)
				while((message=(struct IntuiMessage *)GetMsg(iconwindow->UserPort))) {
					MessageClass=message->Class;
					code=message->Code;
					ReplyMsg((struct Message *)message);
					if(MessageClass & ACTIVEWINDOW) {
						CloseWindow(iconwindow);
						iconwindow=NULL;
						OpenGUI();
						return(TRUE);
					}
				}
	}

	WriteConsole("\33c");
	if(window)
		ClearMenuStrip(window);
	project.FirstItem=&Mlogoff;
	if(window)
		SetMenuStrip(window,&project);
	Midnight();
	ANSI=FALSE;

	GetSystemDateTime();
	if(carrier>=2400 || local)
		TypeFile("Logon",TRUE);
	else {
		sprintf(text,"%.2s/%.2s/%.4s",&SystemDate[0],&SystemDate[2],&SystemDate[4]);
		julian(text);
		sprintf(text,"Welcome to %s.  %s  %s  %s",USER(0)->Name,__daytbl[weekday],Fdate(SystemDate),SystemTime);
		outs(text);
		newline();
	}
	retry=3;
	while((user<1 || user>MAXUSER) && !cd()) {
		switch(retry) {
			case 0:
				outs("The last thing you ever feel is several quarrels cutting deep into your chest.");
				newline();
				Delay(50);
				logoff=TRUE;
				continue;
				break;
			case 1:
				outs("The guards aim their crossbows at you.");
				newline();
				break;
			case 2:
				outs("The guards eye you suspiciously.");
				newline();
				break;
			case 3:
				break;
		}
		newline();
		outs(Fcol(CYN));
		outs("Who dares to enter my dank domain <or NEW>? \5");
		i=GetUser();
		if(i<0) {
			if(!NewUser())
				return(TRUE);
			if(!cd())
				RPGserver('C',user);
			else
				user=0;
			break;
		}
		else {
			user=i;
			if(i==0) {
				retry--;
				outs("Invalid response.");
				newline();
				newline();
				continue;
			}
		}
		RPGserver('C',user);
		ANSI=(local) ? 2 : USER(user)->ANSI;
		sprintf(text,"%s%.26s, type your password: ",Fcol(CYN),USER(user)->Handle);
		outs(text);
		echo=FALSE;
		if(input())
			logoff=TRUE;
		echo=TRUE;
		newline();
		if(strnicmp(USER(user)->Password,ins,sizeof(USER(user)->Password))) {
			RPGserver('D',user);
			user=0;
			retry--;
			newline();
		}
	}
	if(!user)
		return(TRUE);

	memcpy(&ToStart,USER(user),sizeof(struct User));
	DisplayUser();
	GetSystemDateTime();
	online=Time;
	Date=USER(user)->LastDate;
	Time=USER(user)->LastTime;
	if(USER(user)->LastDate!=JulianDate)
		USER(user)->CallsToday=0;

	acclvl=USER(user)->Access;
	if(USER(user)->CallsToday>=ACCESS(acclvl)->Calls) {
		beep();
		sprintf(text,"You have already used all %d of your calls today.",ACCESS(acclvl)->Calls);
		outs(text);
		newline();
		Delay(30);
		acclvl=0;
	}
	if(BBSprefs.Delay) {
		i=60*(online/100-Time/100)+(online%100-Time%100);
		if(USER(user)->CallsToday>0 && i<BBSprefs.Delay) {
			beep();
			sprintf(text,"You were last on just %d minutes ago.",i);
			outs(text);
			newline();
			sprintf(text,"Please wait at least %d minutes between calls.",BBSprefs.Delay);
			outs(text);
			newline();
			Delay(50);
			return(TRUE);
		}
	}
	if(acclvl==1)
		AutoVerify();
	if(cd())
		return(TRUE);
	if(strlen(callerid) && strcmp(callerid,"O")) {
		sprintf(result,"%10.0f",USER(user)->DataPhone);
		if(strncmp(callerid,result,10))
			if((fp=fopen("callerid.log","a"))) {
				fprintf(fp,"%s %s   %s  %s   %.26s\n",jdate(JulianDate),Ftime(online),callerid,result,USER(user)->Handle);
				fclose(fp);
			}
	}
	sprintf(filename,"News.%lu",JulianDate);
	if((fp=fopen(filename,"a"))) {
		fprintf(fp,"%s  %.26s logged on level %u, ",Ftime(online),USER(user)->Handle,USER(user)->Level);
		if(!carrier)
			fprintf(fp,"locally.\n");
		else
		if(USER(user)->LastBaud!=carrier)
			fprintf(fp,"using %lu, not %lu, baud.\n",carrier,USER(user)->LastBaud);
		else
			fprintf(fp,"using %lu baud.\n",carrier);
		fclose(fp);
	}
	USER(user)->LastDate=JulianDate;
	USER(user)->LastTime=online;
	if(USER(user)->Rows<9 || USER(user)->Rows>49)
		USER(user)->Rows=22;
	if(carrier)
		USER(user)->LastBaud=carrier;
	if(acclvl) {
		USER(user)->CallsToday++;
		USER(user)->TotalCalls++;
		USER(user)->Calls++;
		USER(user)->Status=0;
	}
	nme=USER(user)->BountyWho;
	if(nme>0 && !USER(nme)->Access) {
		USER(user)->BountyWho=0;
		USER(user)->BountyAmount=0.;
		nme=0;
	}
	LastFileScan=USER(user)->LastFileScan;
	PutUser(user);
	RestoreStat();
	Thull=USER(user)->Hull;
	if(acclvl) {
		brawl=0;
		fight=0;
		joust=0;
		naval=0;
		party=0;
	}
	else {
		brawl=3;
		fight=3;
		joust=3;
		naval=3;
		party=1;
	}

	sprintf(text,"[#%u] %.26s logged on.",user,USER(user)->Handle);
	RPGmessage(text);
	cls();
	sprintf(text,"%s--=:))%s%s %s %s%s((:=--",Fcol(RED),Fcol(WHITE),Bcol(RED),USER(0)->Name,Bcol(BLACK),Fcol(RED));
	outs(text);
	newline();
	newline();
	normal();
	sprintf(text,"%s Online:%s %s",Fcol(CYN),Fcol(WHITE),USER(user)->Handle);
	outs(text);
	newline();
	sprintf(text,"%s Access:%s %s",Fcol(CYN),Fcol(WHITE),ACCESS(USER(user)->Access)->Name);
	outs(text);
	newline();
	sprintf(text,"%sLast on:%s %s  %s",Fcol(CYN),Fcol(WHITE),jdate(Date),Ftime(Time));
	outs(text);
	newline();
	if(acclvl)
		USER(0)->TotalCalls++;
	sprintf(text,"%sCaller#:%s %lu",Fcol(CYN),Fcol(WHITE),USER(0)->TotalCalls);
	outs(text);
	newline();
	i=USER(0)->ExpireDate-JulianDate;
	if(i<=7) {
		sprintf(text,"%s Reroll:%s %s (in %d day%s)",Fcol(CYN),Fcol(WHITE),jdate(USER(0)->ExpireDate),i,(i>1) ? "s" : "");
		outs(text);
		newline();
	}
	newline();
	if(acclvl) {
		sprintf(text,"%sYou have %d calls remaining.",Fcol(CYN),ACCESS(acclvl)->Calls-USER(user)->CallsToday);
		outs(text);
		newline();
		newline();
	}
	outs("Last callers were: ");
	for(i=0; i<3; i++) {
		sprintf(OUTbuf,"%s%.26s %s(%s%s%s)",Fcol(WHITE),RPGinfo->LastCaller[i],Fcol(CYN),Fcol(GRY),RPGinfo->Reason[i],Fcol(CYN));
		outs(OUTbuf);
		newline();
		outs("                   ");
	}
	newline();
	normal();
	strncpy(RPGinfo->LastCaller[2],RPGinfo->LastCaller[1],sizeof(RPGinfo->LastCaller[2]));
	strncpy(RPGinfo->LastCaller[1],RPGinfo->LastCaller[0],sizeof(RPGinfo->LastCaller[1]));
	strncpy(RPGinfo->LastCaller[0],USER(user)->Handle,sizeof(RPGinfo->LastCaller[0]));
	strncpy(RPGinfo->Reason[2],RPGinfo->Reason[1],sizeof(RPGinfo->Reason[2]));
	strncpy(RPGinfo->Reason[1],RPGinfo->Reason[0],sizeof(RPGinfo->Reason[1]));
	strncpy(RPGinfo->Reason[0],"currently online",sizeof(RPGinfo->Reason[0]));
	memset(reason,0,sizeof(reason));

	sprintf(line[0],"%.6s",jdate(USER(user)->BirthDate));
	sprintf(line[1],"%.6s",Fdate(SystemDate));

	if(acclvl) {
		if(USER(user)->CallsToday==1 && !strcmp(line[0],line[1])) {
			modf(pow2((double)(USER(user)->Level-1)/2.)*500.,&d);
			USER(user)->Gold+=d;
			sprintf(text,"%sHappy Birthday!!  You get %s for a present.",Fcol(WHITE),money(d,user));
			outs(text);
			newline();
			normal();
		}
		if(USER(0)->TotalCalls%1000==0) {
			modf(pow2((double)(USER(user)->Level-1)/2.)*500.,&d);
			USER(user)->Gold+=d;
			sprintf(text,"%sYou are the %luth caller; you win %s!",Fcol(WHITE),USER(0)->TotalCalls,money(d,user));
			outs(text);
			newline();
			normal();
		}
		if(USER(user)->Bank>0. && USER(user)->TotalCalls%5==0) {
			modf(USER(user)->Bank*.05,&d);
			if(d>0.) {
				USER(user)->Bank+=d;
				sprintf(text,"Interest on savings: %s",money(d,user));
				outs(text);
				newline();
			}
		}
		if(USER(user)->Loan>0. && USER(user)->TotalCalls%3==0) {
			modf(USER(user)->Loan*.10,&d);
			if(d>0.) {
				USER(user)->Loan+=d;
				sprintf(text,"Interest on loan: %s",money(d,user));
				outs(text);
				newline();
			}
		}
	}
	PutUser(user);
	newline();
	Pause();
	sprintf(text,"Notes.%d",user);
	strmfp(filename,NOTESpath,text);
	TypeFile(filename,FALSE);
	remove(filename);

	cls();
	sprintf(text,"%s       -------------",Fcol(BLU));
	outs(text);
	newline();
	sprintf(text,"--=:))%s Announcements %s((:=--",Fcol(YELLOW),Fcol(BLU));
	outs(text);
	newline();
	outs("       -------------");
	newline();
	normal();
	TypeFile("Announcements",TRUE);
	sprintf(text,"%s       ------------",Fcol(BLU));
	outs(text);
	newline();
	sprintf(text,"--=:))%s Auto-Message %s((:=--",Fcol(YELLOW),Fcol(BLU));
	outs(text);
	newline();
	outs("       ------------");
	newline();
	normal();
	TypeFile("Auto-Message",TRUE);
	if(acclvl) {
		outs(Fcol(CYN));
		outs("Change Auto-Message (Y/N)? ");
		if(!(c=inkey()))
			return(TRUE);
		newline();
		newline();
		if(c=='Y')
			if(Editor(5)=='S') {
				outs("aving...");
				if((fp=fopen("Auto-Message","w"))) {
					fprintf(fp,"Date: %s  %s\n",jdate(JulianDate),Ftime(Time));
					fprintf(fp,"From: %.26s\n\n",USER(user)->Handle);
					for(i=0; i<numline; i++)
						fprintf(fp,"%s\n",line[i]);
					fclose(fp);
					outs("Ok.");
				}
				else
					outs("Error!");
				newline();
				newline();
			}
	}
	else
		Pause();
	ReadMail();
	if(acclvl)
		TaxMan();
	PutUser(0);
	MainMenu();
	Logoff();
	if(!local && baud>2400 && carrier<baud && carrier>0)
		Delay(20*19200/baud*baud/carrier);
	if(!local && baud<=2400) {
		WriteSerial("+++");
		Delay(75);
		WriteSerial("ATH\15");
		Delay(75);
	}
	return(TRUE);
}
