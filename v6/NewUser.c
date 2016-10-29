/*********************\
 *  RPGBBS New User  *
\*********************/
#include "RPGBBS.h"

int EditField(int n)
{
	FILE *fp;
	int i;

	newline();
	switch(n) {
		case 1:
			sprintf(text,"%d.  Your REAL name [max %d]: ",n,sizeof(USER(user)->Name));
			outs(text);
			if(input())
				return(FALSE);
			ToUpLow(ins);
			if(strlen(ins)<5 || strlen(ins)>sizeof(USER(user)->Name) || !isalpha(ins[0]))
				return(TRUE);
			strncpy(USER(user)->Name,ins,sizeof(USER(user)->Name));
			break;
		case 2:
			sprintf(text,"%d.  Your STREET address [max %d]: ",n,sizeof(USER(user)->Street[0]));
			outs(text);
			if(input())
				return(FALSE);
			ToUpLow(ins);
			if(strlen(ins)<1 || strlen(ins)>sizeof(USER(user)->Street[0]))
				return(TRUE);
			strncpy(USER(user)->Street[0],ins,sizeof(USER(user)->Street[0]));
			break;
		case 3:
			sprintf(text,"%d.  Your CITY, STATE & ZIP [max %d]: ",n,sizeof(USER(user)->Street[1]));
			outs(text);
			if(input())
				return(FALSE);
			ToUpLow(ins);
			if(strlen(ins)<1 || strlen(ins)>sizeof(USER(user)->Street[1]))
				return(TRUE);
			strncpy(USER(user)->Street[1],ins,sizeof(USER(user)->Street[1]));
			break;
		case 4:
			sprintf(text,"%d.  Sex <M>ale or <F>emale: ",n);
			outs(text);
			if(input())
				return(FALSE);
			strupr(ins);
			if(strlen(ins)!=sizeof(USER(user)->Sex) || (ins[0]!='M' && ins[0]!='F'))
				return(TRUE);
			USER(user)->Sex=ins[0];
			break;
		case 5:
			sprintf(text,"%d.  Date of Birth (mm/dd/yyyy): ",n);
			outs(text);
			if(input())
				return(FALSE);
			if(strlen(ins)!=10)
				return(TRUE);
			USER(user)->BirthDate=julian(ins);
			if(JulianDate-USER(user)->BirthDate>50000)
				return(TRUE);
			break;
		case 6:
			sprintf(text,"%d.  Your VOICE phone number [###-###-####]: ",n);
			outs(text);
			if(input())
				return(FALSE);
			if(strlen(ins)!=12)
				return(TRUE);
			USER(user)->VoicePhone=10000000.*atof(&ins[0])+10000.*atof(&ins[4])+atof(&ins[8]);
			if(atoi(&ins[0])==900 || atoi(&ins[4])==911) {
				logoff=TRUE;
				break;
			}
			if(USER(user)->VoicePhone<1001000000.)
				return(TRUE);
			break;
		case 7:
			sprintf(line[0],"%010.0f",(USER(user)->DataPhone==0.) ? USER(user)->VoicePhone : USER(user)->DataPhone);
			sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
			sprintf(text,"%d.  Your COMPUTER phone number [%s]: ",n,line[1]);
			outs(text);
			if(input())
				return(FALSE);
			if(strlen(ins)) {
				if(strlen(ins)!=12)
					return(TRUE);
				USER(user)->DataPhone=10000000.*atof(&ins[0])+10000.*atof(&ins[4])+atof(&ins[8]);
				if(USER(user)->DataPhone<1001000000.)
					return(TRUE);
			}
			else {
				USER(user)->DataPhone=USER(user)->VoicePhone;
				strcpy(ins,line[1]);
				outs(ins);
			}
			if(atoi(&ins[0])==900 || atoi(&ins[4])==911) {
				logoff=TRUE;
				break;
			}
			if((fp=fopen("BadPhone.list","r"))) {
				while(!feof(fp)) {
					inputs(text,sizeof(text),fp);
					if(!strncmp(text,ins,12)) {
						USER(user)->DataPhone=0.;
						break;
					}
				}
				fclose(fp);
			}
			break;
		case 8:
			sprintf(text,"%d.  Your HANDLE [max %d]: ",n,sizeof(USER(user)->Handle));
			outs(text);
			if(input())
				return(FALSE);
			ToUpLow(ins);
			if(strlen(ins)<2 || strlen(ins)>sizeof(USER(user)->Handle) || !isalpha(ins[0]))
				return(TRUE);
			memset(USER(user)->Handle,0,sizeof(USER(user)->Handle));
			for(i=1; i<=MAXUSER; i++)
				if(strnicmp(USER(i)->Handle,ins,sizeof(USER(user)->Handle))==0) {
					newline();
					outs("Sorry, that handle already exists.  Please try another.");
					newline();
					return(TRUE);
				}
			strncpy(USER(user)->Handle,ins,sizeof(USER(user)->Handle));
			break;
		case 9:
			sprintf(text,"%d.  Terminal emulation:",n);
			outs(text);
			newline();
			newline();
			sprintf(text,"%s ASCII text",bracket(0));
			outs(text);
			newline();
			sprintf(text,"%s VT100 series",bracket(1));
			outs(text);
			newline();
			sprintf(text,"%s ANSI color",bracket(2));
			outs(text);
			newline();
			newline();
			outs("Select (0-2)? ");
			if(input())
				break;
			newline();
			USER(user)->ANSI=atoi(ins);
			if(USER(user)->ANSI>2)
				USER(user)->ANSI=0;
			ANSI=USER(user)->ANSI;
			sprintf(text,"ANSI emulation is now:%s %s",bracket(USER(user)->ANSI),USER(user)->ANSI ? "ON" : "OFF");
			outs(text);
			newline();
			break;
		case 10:
			USER(user)->Password[0]='A'+dice(26)-1;
			USER(user)->Password[1]='A'+dice(26)-1;
			USER(user)->Password[2]='0'+dice(10)-1;
			USER(user)->Password[3]='0'+dice(10)-1;
			USER(user)->Password[4]='0'+dice(10)-1;
			sprintf(text,"%d.  Your PASSWORD: %s%.5s",n,Fcol(YELLOW),USER(user)->Password);
			outs(text);
			normal();
			outs(" <-- SAVE THIS CODE; you will need it to log on later.");
			break;
	}
	echo=TRUE;
	return(FALSE);
}

int NewUser(void)
{
	int i;

	for(user=1; user<=MAXUSER && USER(user)->Access; user++);
	for(i=100; i>=30 && user>MAXUSER; i-=10)
		for(user=1; user<=MAXUSER; user++) {
			if(JulianDate-USER(user)->LastDate>i && USER(user)->Class==0) {
				DeleteUser(user);
				break;
			}
		}
	if(user>MAXUSER) {
		beep();
		sprintf(text,"Regretfully, there are already the maximum of %d users active.",MAXUSER);
		outs(text);
		newline();
		outs("Please call back another day and try again.  Thanks for calling.");
		newline();
		logoff=TRUE;
		return(FALSE);
	}
	memset(USER(user),0,sizeof(struct User));
	cls();
	outs("--=:)) New User Registration ((:=--");
	newline();
	for(i=1; i<=10; i++)
		while(EditField(i))
			beep();
	newline();
	do {
		newline();
		outs(Fcol(CYN));
		outs("Enter field number (1-10) to change or press RETURN to save: ");
		if(input())
			return(FALSE);
		newline();
		i=atoi(ins);
		if(i>=1 || i<=10)
			while(EditField(i))
				beep();
		newline();
	} while(strlen(ins));
	cls();
	do {
		for(i=50; i>1; i--) {
			sprintf(text,"%s",bracket(i));
			outs(text);
			newline();
		}
		outs("Enter the number that appears at top of screen: ");
		if(input())
			return(FALSE);
		newline();
		i=atoi(ins);
	} while (i<10 || i>50);
	USER(user)->Rows=i-1;

	sprintf(text,"Your usernumber %u is being saved for later validation.  Enjoy yourself!",user);
	outs(text);
	newline();
	outs("Please take some time to read Help & Hints from the Main Menu...");
	newline();
	newline();
	Delay(100);
	RestartChar(user);
	USER(user)->LastBaud=carrier;
	USER(user)->TotalPosts=1;
	USER(user)->Format=1;
	USER(user)->Access=1;
	USER(user)->Ratio=USER(0)->Ratio;
	USER(user)->ULfiles=1;
	PutUser(user);
	strmfp(filename,HELPpath,"Other");
	TypeFile(filename,FALSE);
	return(TRUE);
}
