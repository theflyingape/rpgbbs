#include "/include/all.h"
#include "global.h"

void emulation(void)
{
	sprintf(outbuf,"%sWhich terminal emulation are you using?",fore(CYN));
	OUT(outbuf);NL;NL;
	sprintf(outbuf," %s%s VT220 w/DEC graphics",bracket(1),fore(GRY));
	OUT(outbuf);NL;
	sprintf(outbuf," %s%s ANSI w/IBM color graphics",bracket(2),fore(GRY));
	OUT(outbuf);NL;
	sprintf(outbuf," %s%s RPGBBS w/AMIGA color graphics",bracket(3),fore(GRY));
	OUT(outbuf);NL;NL;
	sprintf(outbuf,"%sSelect: ",fore(CYN));
	OUT(outbuf);
	FOREVER {
		if(ins(1))
			break;
		ANSI=atoi(inbuf);
		PLAYER.Emulation=ANSI;
		if(ANSI>=1 && ANSI<=3)
			break;
		erase();
	}
	FOREVER {
		cls();
		for(PLAYER.Rows=50; PLAYER.Rows>21; PLAYER.Rows--) {
			OUT(bracket(PLAYER.Rows));NL;
		}
		for(; PLAYER.Rows>1; PLAYER.Rows--) {
			OUT(" ..");NL;
		}
		sprintf(outbuf,"%sEnter the number on the top of your screen (22-50): ",fore(CYN));
		OUT(outbuf);
		if(ins(2))
			break;
		PLAYER.Rows=atoi(inbuf);
		if(PLAYER.Rows>=22 && PLAYER.Rows<=50)
			break;
		beep();
	}
	rows=(local) ? SERCON.MaxRow : PLAYER.Rows;
}

int edituser(int p)
{
	int flag=TRUE,i;
	char *token;

	plot(&SERCON,p+2,23);
	CLL;
	switch(p) {
		case 1:
			if(ins(22))
				return(FALSE);
			proper(inbuf);
			if(strlen(inbuf)>1) {
				strncpy(PLAYER.Handle,inbuf,sizeof(PLAYER.Handle));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.Handle);CLL;
				strncpy(username.Handle,PLAYER.Handle,sizeof(username.Handle));
				if(!(flag=RPGserver(SERVER_GETUSERNAME,(UBYTE *)&username))) {
					strncpy(username.Handle,PLAYER.Handle,sizeof(username.Handle));
					for(token=strtok(username.Handle," "); token; token=strtok(NULL," "))
						for(i=0; i<NUMACCESS; i++)
							if(!stricmp(token,ACCESS(i)->Name))
								flag=TRUE;
					strncpy(username.Handle,PLAYER.Handle,sizeof(username.Handle));
					for(token=strtok(username.Handle," "); token; token=strtok(NULL," "))
						for(i=0; i<NUMARENA; i++)
							if(!stricmp(token,ARENA(i)->Name))
								flag=TRUE;
					strncpy(username.Handle,PLAYER.Handle,sizeof(username.Handle));
					for(token=strtok(username.Handle," "); token; token=strtok(NULL," "))
						for(i=0; i<NUMDUNGEON; i++)
							if(!stricmp(token,DUNGEON(i)->Name))
								flag=TRUE;
				}
				if(flag) {
					beep();
					sprintf(outbuf,"  %s<- already taken",fore(YELLOW));
					OUT(outbuf);
					Delay(50);
				}
			}
			break;
		case 2:
			strcpy(inbuf,PLAYER.Name);
			if(RPGBBSmode==BBS) {
				if(ins(30))
					return(FALSE);
				proper(inbuf);
				if(strlen(inbuf)>4) {
					strncpy(PLAYER.Name,inbuf,sizeof(PLAYER.Name));
					flag=FALSE;
				}
			}
			else
				flag=FALSE;
			plot(&SERCON,p+2,23);
			OUT(PLAYER.Name);CLL;
			break;
		case 3:
			if(ins(30))
				return(FALSE);
			proper(inbuf);
			if(strlen(inbuf)>3) {
				strncpy(PLAYER.Street,inbuf,sizeof(PLAYER.Street));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.Street);CLL;
				flag=FALSE;
			}
			break;
		case 4:
			if(ins(20))
				return(FALSE);
			proper(inbuf);
			if(strlen(inbuf)>1) {
				strncpy(PLAYER.City,inbuf,sizeof(PLAYER.City));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.City);CLL;
				flag=FALSE;
			}
			break;
		case 5:
			if(ins(9))
				return(FALSE);
			proper(inbuf);
			if(strlen(inbuf)>1) {
				if(strlen(inbuf)<3)
					strupr(inbuf);
				strncpy(PLAYER.State,inbuf,sizeof(PLAYER.State));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.State);CLL;
				flag=FALSE;
			}
			break;
		case 6:
			if(ins(10))
				return(FALSE);
			strncpy(PLAYER.Zip,inbuf,sizeof(PLAYER.Zip));
			plot(&SERCON,p+2,23);
			OUT(PLAYER.Zip);CLL;
			flag=FALSE;
			break;
		case 7:
			if(ins(15))
				return(FALSE);
			if(strlen(inbuf))
				phone(inbuf);
			if(BBSprefs.AreaCode>0 && strlen(inbuf)==7) {
				sprintf(outbuf,"%03u%s",BBSprefs.AreaCode,inbuf);
				strcpy(inbuf,outbuf);
				phone(inbuf);
			}
			if(strlen(inbuf)==10) {
				sprintf(outbuf,"%.3s-%.3s-%.4s",&inbuf[0],&inbuf[3],&inbuf[6]);
				strcpy(inbuf,outbuf);
			}
			if(!strlen(inbuf))
				strcpy(inbuf,"<private>");
			if(strlen(inbuf)>6) {
				strncpy(PLAYER.VoicePhone,inbuf,sizeof(PLAYER.VoicePhone));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.VoicePhone);CLL;
				flag=FALSE;
			}
			break;
		case 8:
			if(ins(15))
				return(FALSE);
			if(!strlen(inbuf))
				strcpy(inbuf,strlen(PLAYER.DataPhone) ? PLAYER.DataPhone : PLAYER.VoicePhone);
			phone(inbuf);
			if(BBSprefs.AreaCode>0 && strlen(inbuf)==7) {
				sprintf(outbuf,"%03u%s",BBSprefs.AreaCode,inbuf);
				strcpy(inbuf,outbuf);
				phone(inbuf);
			}
			if(strlen(inbuf)==10) {
				sprintf(outbuf,"%.3s-%.3s-%.4s",&inbuf[0],&inbuf[3],&inbuf[6]);
				strcpy(inbuf,outbuf);
			}
			if(strlen(inbuf)>6) {
				strncpy(PLAYER.DataPhone,inbuf,sizeof(PLAYER.DataPhone));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.DataPhone);CLL;
				flag=FALSE;
			}
			break;
		case 9:
			if(ins(10))
				return(FALSE);
			if(strlen(inbuf)) {
				julian(&PLAYER.BirthDate,inbuf);
				if(PLAYER.BirthDate>696969L && Julian-PLAYER.BirthDate>3000) {
					fdate(inbuf,PLAYER.BirthDate);
					plot(&SERCON,p+2,23);
					OUT(inbuf);CLL;
					flag=FALSE;
				}
			}
			break;
		case 10:
			if(ins(1))
				return(FALSE);
			strupr(inbuf);
			PLAYER.Sex=inbuf[0];
			if(PLAYER.Sex=='M' || PLAYER.Sex=='F') {
				plot(&SERCON,p+2,23);
				OUT(inbuf);
				flag=FALSE;
			}
			break;
		case 11:
			if(ins(9))
				return(FALSE);
			if(strlen(inbuf)>2) {
				proper(inbuf);
				strupr(inbuf);
				strncpy(PLAYER.Password,inbuf,sizeof(PLAYER.Password));
				plot(&SERCON,p+2,23);
				OUT(PLAYER.Password);CLL;
				flag=FALSE;
			}
			break;
	}
	if(cd())
		flag=FALSE;
	return(flag);
}

void NewUser(void)
{
	int i,p;

	title("newuser");
	memcpy(&PLAYER,NEW,sizeof(struct user));
	memset(PLAYER.ID,0,sizeof(PLAYER.ID));
	if(RPGBBSmode==DOOR)
		strncpy(PLAYER.Name,argument,sizeof(PLAYER.Name));
	PLAYER.CreateDate=Julian;
	PLAYER.ExpireDate+=Julian;
	PLAYER.LastDate=Julian;
	strncpy(PLAYER.DataPhone,callerid,sizeof(PLAYER.DataPhone));
	PLAYER.Novice='Y';

	emulation();

	cls();
	sprintf(outbuf,"     %s--=:) %sNew User Registration%s (:=--",fore(BRN),fore(YELLOW),fore(BRN));
	OUT(outbuf);NL;NL;
	sprintf(outbuf,"%s%s Player's Handle: ",bracket(1),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Your REAL Name.: %s%s",bracket(2),fore(CYN),fore(BLACK),PLAYER.Name);
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Street Address.: ",bracket(3),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Town/City Name.: ",bracket(4),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s State/Country..: ",bracket(5),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Postal Zip Code: ",bracket(6),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Voice Telephone: ",bracket(7),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Modem Telephone: %s%s",bracket(8),fore(CYN),fore(BLACK),PLAYER.DataPhone);
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Date of Birth..: ",bracket(9),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Gender (M/F)...: ",bracket(10),fore(CYN));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s%s Secret Password: ",bracket(11),fore(CYN));
	OUT(outbuf);NL;
	for(p=1; p<=11; p++)
		while(edituser(p))
			beep();
	NL;NL;
	sprintf(outbuf,"%sSelect field # to change or <RETURN> to save: ",fore(CYN));
	OUT(outbuf);
	do {
		plot(&SERCON,15,47);CLL;
		ins(2);
		p=atoi(inbuf);
		if(p>=1 && p<=11)
			while(edituser(p))
				beep();
	} while(p);
	NL;NL;
	if(cd())
		return;

	sprintf(outbuf,"%sPlease wait:%s",fore(MAGENTA),fore(YELLOW));
	OUT(outbuf);

	p=0;
	memset(&ENEMY,0,sizeof(struct user));
	for(i=0; RPGserver(SERVER_NEXTUSER,(UBYTE *)&ENEMY); i++) {
		if(RPGBBSmode==DOOR && !stricmp(PLAYER.Name,ENEMY.Name)) {
			sprintf(outbuf,"%sYou are already registered as:%s %s - %s",fore(CYN),fore(WHITE),ENEMY.ID,ENEMY.Handle);
			beep();NL;
			OUT(outbuf);NL;
			pause();
		}
		if(!strcmp(PLAYER.DataPhone,ENEMY.DataPhone)) {
			p++;
			OUT(".");
		}
		else {
			sprintf(outbuf,"%c\10",spin[i%4]);
			OUT(outbuf);
		}
	}
	if(p>1 && RPGBBSmode==DOOR)
		return;
	if(p>=2)
		PLAYER.Access=2;
	if((!stricmp(PLAYER.DataPhone,callerid) && callerid[0]!='O' && p<2) || local || RPGBBSmode==DOOR)
		PLAYER.Access=4;

	for(p=0; p<strlen(PLAYER.Handle) && strlen(PLAYER.ID)<3; p++) {
		PLAYER.ID[strlen(PLAYER.ID)]=toupper(PLAYER.Handle[p]);
		while(++p<strlen(PLAYER.Handle) && PLAYER.Handle[p]!=' ');
	}
	if(strlen(PLAYER.ID)<2) {
		sprintf(PLAYER.ID,"%.3s",PLAYER.Handle);
		strupr(PLAYER.ID);
	}
	strncpy(ENEMY.ID,PLAYER.ID,sizeof(ENEMY.ID));
	for(p='2'; RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY) || !strcmp(ENEMY.ID,"NEW"); p++) {
		if(isdigit(PLAYER.ID[strlen(PLAYER.ID)-1]))
			PLAYER.ID[strlen(PLAYER.ID)-1]=NULL;
		sprintf(PLAYER.ID,"%.3s%c",PLAYER.ID,p);
		strncpy(ENEMY.ID,PLAYER.ID,sizeof(ENEMY.ID));
	}
	LoadRPC(ONLINE);
	sprintf(outbuf,"%sYour User ID is:%s %s",fore(CYN),fore(WHITE),PLAYER.ID);
	NL;OUT(outbuf);NL;
	RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
	numline=0;
	sprintf(line[numline++],"[%s] %s signed on as a new user",PLAYER.ID,PLAYER.Handle);
	note(SYSID);
	pause();
	cls();
	type("Notices/Intro",TRUE);
}
