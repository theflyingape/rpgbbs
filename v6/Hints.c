/******************\
 *  RPGBBS Hints  *
\******************/
#include "RPGBBS.h"

void Hints(void)
{
	static struct RPGmenu __far RPGmenu = { "Help",GRN,8,
							{	{ 'A',"Ability Ratings" },
								{ 'C',"Character Class Definitions" },
								{ 'D',"Deep, Dank Dungeon Guide" },
								{ 'F',"Other Features Worth Knowing" },
								{ 'G',"Gangs & Partying" },
								{ 'M',"Magic Spells, Scrolls, & Wands" },
								{ 'N',"Naval Warships & Fishing" },
								{ 'P',"Poisons from the Apothecary" }
							}
	};
	int i;
	char c;

	outs(Fcol(WHITE));
	outs("--=:)) NOTE ((:=--");
	newline();
	normal();
	outs("Your communications software may have a 'Capture Text Buffer' or 'Printer");
	newline();
	outs("Echo' feature.  You may want to use one of them to read these hints after");
	newline();
	outs("signing off.  There is a 2-minute, input inactivity timeout in effect.");
	newline();
	Pause();

	cls();
	sprintf(OUTbuf,"    %s--=:))%s Help & Hints %s((:=--",Fcol(GRN),Fcol(GREEN),Fcol(GRN));
	outs(OUTbuf);
	newline();
	newline();
	while(!cd()) {
		if((c=Option(&RPGmenu))=='Q')
			break;

		switch(c) {
			case 'A':
				strmfp(filename,HELPpath,"Ability");
				TypeFile(filename,FALSE);
				break;
			case 'C':
				for(i=1; i<MAXCLASS; i++) {
					sprintf(text,"%4s %-10.10s",bracket(i),CLASS(i)->Name);
					outs(text);
					newline();
				}
				newline();
				outs(Fcol(CYN));
				outs("Enter class: ");
				if(input())
					break;
				newline();
				i=atoi(ins);
				if(i>=0 && i<MAXCLASS) {
					strmfp(filename,HELPpath,CLASS(i)->Name);
					if(ANSI)
						strmfe(filename,filename,"ANS");
					else
						strmfe(filename,filename,"TXT");
					TypeFile(filename,FALSE);
				}
				break;
			case 'D':
				strmfp(filename,HELPpath,"Dungeon");
				TypeFile(filename,FALSE);
				break;
			case 'F':
				strmfp(filename,HELPpath,"Other");
				TypeFile(filename,FALSE);
				break;
			case 'G':
				strmfp(filename,HELPpath,"Party");
				TypeFile(filename,FALSE);
				break;
			case 'M':
				strmfp(filename,HELPpath,"Magic");
				TypeFile(filename,FALSE);
				break;
			case 'N':
				strmfp(filename,HELPpath,"Naval");
				TypeFile(filename,FALSE);
				break;
			case 'P':
				strmfp(filename,HELPpath,"Poison");
				TypeFile(filename,FALSE);
				break;
		}
	}
}
