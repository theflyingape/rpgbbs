#include "/include/all.h"
#include "global.h"

void AutoVerify(void)
{
	static char dial[2][24];
	int d,i,retry;
	char c;

	if(local || !BBSprefs.Auto)
		return;
	NL;
	OUT("You can be auto-verified by letting me call your computer now.");NL;
	OUT("You would need to put your modem in Auto-Answer mode (AT S0=1)");NL;NL;
	sprintf(prompt,"%sDo you wish to be auto-verified now (Y/N)? ",fore(CYN));
	do {
		OUT(prompt);
		if(!(c=inkey(NULL,'N')))
			break;
		NL;
	} while(c!='Y' && c!='N');
	NL;
	if(c=='N')
		return;
	OUT("Ok.  Please tell me which dial command I will need to use:");NL;NL;
	memset(dial,0,sizeof(dial));
	strcpy(dial[0],&PLAYER.DataPhone[4]);
	sprintf(outbuf,"%s%s Local: ATDT %s",bracket(1),fore(GRY),dial[0]);
	OUT(outbuf);NL;
	if(BBSprefs.Auto>1) {
		sprintf(dial[1],"1-%s",PLAYER.DataPhone);
		sprintf(outbuf,"%s%s Toll: ATDT %s",bracket(2),fore(GRY),dial[1]);
		OUT(outbuf);NL;
	}
	sprintf(outbuf,"%s%s Forget it.",bracket(3),fore(GRY));
	OUT(outbuf);NL;NL;
	sprintf(prompt,"%sSelect (1-3): ",fore(CYN));
	do {
		OUT(prompt);
		if(ins(1))
			break;
		i=atoi(inbuf);
		NL;
	} while(i<1 || i>3);
	NL;
	if(!strlen(dial[i-1]) || i==3)
		return;
	d=i-1;
	OUT("Ok, please HANG UP your modem NOW and don't forget to set your modem to");NL;
	OUT("Auto-Answer mode by typing ATS0=1 at the OK prompt IMMEDIATELY.");NL;
	OUT("-OR- if you start getting RING messages, type ATA to answer.");NL;
	OUT("-OR- wait a minute to abort entire procedure.");NL;
	for(retry=0; retry<60 && !cd(); retry++)
		Delay(50);
	if(cd()) {
		sprintf(outbuf,"%sDisabling Auto-Answer mode...%s\15\12",fore(CYN),fore(GRY));
		OUTCON(outbuf);
		modem_cmd("ATS0=0");
		for(retry=0; retry<3 && cd(); retry++) {
			Delay(50);
			sprintf(outbuf,"%sNow calling %s back...%s\15\12",fore(CYN),PLAYER.Handle,fore(GRY));
			OUTCON(outbuf);
			sprintf(outbuf,"ATDT %s",dial[d]);
			modem_cmd(outbuf);
		}
		Delay(50);
		modem_clr();
		if(!cd()) {
			NORMAL;NL;
			sprintf(outbuf,"Calling from %s.",SYS->Name);
			OUT(outbuf);
			for(i=0; i<3; i++) {
				NL;
				sprintf(outbuf,"%s%s, type your password: ",fore(CYN),PLAYER.Handle);
				OUT(outbuf);
				echo=FALSE;
				if(ins(sizeof(PLAYER.Password))) {
					i=3;
					break;
				}
				NL;
				if(!strnicmp(PLAYER.Password,inbuf,sizeof(PLAYER.Password))) {
					PLAYER.Access=4;
					RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
					NL;
					OUT("THANK YOU!  You have been verified to play.  Call back again!");NL;NL;
					Delay(100);
					break;
				}
			}
			if(i==3) {
				PLAYER.Access=2;
				RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
			}
		}
		logoff=TRUE;
	}
	else {
		beep();
		OUT("Auto-verify aborted.");NL;
		modem_clr();
	}
}

void TaxMan(void)
{
	double ratio,tax;
	int i;
	char c;

	if(IRS->Posts) {
		ratio=1./(double)IRS->Posts;
		if((double)PLAYER.Posts/(double)PLAYER.History.Calls<ratio) {
			NL;
			tax=PLAYER.Gold+PLAYER.Bank;
			JOUST(PLAYER.Level);
			if(tax<d)
				tax=d;
			d=tax*(ratio-(double)PLAYER.Posts/(double)PLAYER.History.Calls);
			modf(d,&tax);
			if(tax<1.)
				tax=1.;
			sprintf(outbuf,"%s, the tax collector, says, \"You must do your part to keep",IRS->Handle);
			OUT(outbuf);NL;
			OUT("this fine city running!\"  You check out the two burly guards who stand ready");NL;
			OUT("to enforce the king's will and decide to pay the tax.");NL;NL;
			Delay(50);
			sprintf(outbuf,"The tax will cost you %s.",money(tax,ANSI));
			OUT(outbuf);NL;NL;
			Delay(50);
			PLAYER.Bank-=tax;
			if(PLAYER.Bank<0.) {
				PLAYER.Gold+=PLAYER.Bank;
				PLAYER.Bank=0.;
			}
			if(PLAYER.Gold<0.) {
				PLAYER.Loan-=PLAYER.Gold;
				PLAYER.Gold=0.;
			}
			sprintf(outbuf,"Your post/call ratio = %g",(double)PLAYER.Posts/(double)PLAYER.History.Calls);
			OUT(outbuf);NL;NL;
			paws=!PLAYER.Expert;
			IRS->Bank+=tax;
			RPGserver(SERVER_PUTUSER,(UBYTE *)IRS);
		}
	}
	JOUST(PLAYER.Level);
	d*=1000.;
	if(PLAYER.Gold+PLAYER.Bank>d) {
		NL;
		tax=PLAYER.Gold+PLAYER.Bank-d;
		sprintf(outbuf,"%s, the tax collector, looks at your bulging sacks of gold",IRS->Handle);
		OUT(outbuf);NL;
		OUT("and says, \"Ah, it is time to pay your taxes!\"  You check out the two burly");NL;
		OUT("guards who stand ready to enforce the king's will.");NL;NL;
		Delay(100);
		sprintf(outbuf,"The tax will cost you %s.",money(tax,ANSI));
		OUT(outbuf);NL;NL;
		Delay(100);
		sprintf(prompt,"%sWill you pay the tax (Y/N)? ",fore(CYN));
		do {
			OUT(prompt);
			c=inkey('Y','Y');
			NL;
		} while(c!='Y' && c!='N');
		NL;
		if(c=='N') {
			OUT("The tax collector orders his guards, \"Run this outlaw through!\"");NL;NL;
			Delay(100);
			for(i=0; i<2; i++) {
				memset(RPC[1][i],0,sizeof(struct RPC));
				sprintf(RPC[1][i]->user.Handle,"City Guard #%d",i+1);
				strmfe(RPC[1][i]->user.Class,ServerTable->class[0]->Origin,ServerTable->class[0]->Character[dice(MAXCLASS(0))-1]->Name);
				RPC[1][i]->user.Level=PLAYER.Level/2+1;
				RPC[1][i]->user.Sex='M';
				sprintf(RPC[1][i]->user.Weapon,"NATURAL.%u",RPC[1][i]->user.Level/2+dice(RPC[1][i]->user.Level/2));
				sprintf(RPC[1][i]->user.Armor,"NATURAL.%u",RPC[1][i]->user.Level/4+dice(RPC[1][i]->user.Level/4));
				CreateRPC(RPC[1][i]);
				RPC[1][i]->user.Poison=(UWORD)~0;
				if(RPC[1][i]->SP)
					RPC[1][i]->user.Spell=HEAL_SPELL | BLAST_SPELL;
			}
			from=NULL;
			Battle();
			if(RPC[1][0]->HP>0 || RPC[1][1]->HP>0 || !ONLINE->HP)
				c='Y';
		}
		memcpy(&ENEMY,IRS,sizeof(struct user));
		if(c=='N') {
			RPC[1][0]->user.Level=PLAYER.Level*3/4+1;
			strcpy(ENEMY.Class,"IMMORTAL.Paladin");
			for(i=0; i<MAXWEAPON(0) && tax>=ServerTable->weapon[0]->Item[i]->Value; i++);
			i+=dice(3)-3;
			i=(i<1) ? 1 : (i>=MAXWEAPON(0)) ? MAXWEAPON(0)-1 : i;
			strmfe(ENEMY.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[i]->Name);
			for(i=0; i<MAXARMOR(0) && tax>=ServerTable->armor[0]->Item[i]->Value; i++);
			i+=dice(3)-3;
			i=(i<1) ? 1 : (i>=MAXARMOR(0)) ? MAXARMOR(0)-1 : i;
			strmfe(ENEMY.Armor,ServerTable->armor[0]->Origin,ServerTable->armor[0]->Item[i]->Name);
			CreateRPC(RPC[1][0]);
			ENEMY.WCmod=0;ENEMY.ACmod=0;
			RPC[1][0]->ToWC=0;RPC[1][0]->ToAC=0;
			RPC[1][0]->user.Poison=(UWORD)~0;
			if(RPC[1][0]->SP)
				RPC[1][0]->user.Spell=HEAL_SPELL | BLAST_SPELL | CONFUSION_SPELL | TRANSMUTE_SPELL | CURE_SPELL;
			OUT("The tax collector ");
			switch(dice(3)) {
				case 1:
					OUT("mutters, \"Good help is hard to find these days...\"");
					break;
				case 2:
					OUT("sighs, \"If you want a job done right...\"");
					break;
				case 3:
					OUT("swears, \"That's gonna cost you.\"");
					break;
			}
			NL;NL;
			Delay(100);
			sprintf(outbuf,"He pulls out his %s.",WEAPON(RPC[1][0])->Name);
			OUT(outbuf);NL;NL;
			Delay(100);
			sprintf(outbuf,"He is wearing his %s.",ARMOR(RPC[1][0])->Name);
			OUT(outbuf);NL;NL;
			Delay(100);
			from='L';
			Battle();
			from=NULL;
			if(ONLINE->HP>0) {
				IRS->Gold=tax;
				IRS->Bank-=tax;
				if(IRS->Bank<0.) {
					IRS->Gold+=IRS->Bank;
					IRS->Bank=0.;
				}
				if(IRS->Gold>0.) {
					PLAYER.Gold+=IRS->Gold;
					sprintf(outbuf,"You take %s he was carrying.",money(IRS->Gold,ANSI));
					OUT(outbuf);NL;
				}
			}
			if(RPC[1][0]->HP>0) {
				c='Y';
				if(PLAYER.Gold>0.) {
					IRS->Bank+=PLAYER.Gold;
					sprintf(outbuf,"%s takes %s you were carrying.",RPC[1][0]->He,money(PLAYER.Gold,ANSI));
					OUT(outbuf);NL;
					PLAYER.Gold=0.;
				}
				ExchangeWeapon(RPC[1][0],ONLINE);
				ExchangeArmor(RPC[1][0],ONLINE);
				OUT("\"Thanks for the taxes!\"");NL;NL;
				Delay(100);
			}
		}
		if(c=='Y') {
			PLAYER.Bank-=tax;
			if(PLAYER.Bank<0.) {
				PLAYER.Gold+=PLAYER.Bank;
				PLAYER.Bank=0.;
			}
			if(PLAYER.Gold<0.) {
				PLAYER.Loan-=PLAYER.Gold;
				PLAYER.Gold=0.;
			}
			ENEMY.Bank+=tax;
			OUT("...and you pay the tax.");NL;
			paws=!PLAYER.Expert;
			Delay(100);
		}
		RPGserver(SERVER_PUTUSER,(UBYTE *)IRS);
	}
}

int Logon(void)
{
	FILE *fp,*newfp;
	UWORD t;
	int i,loop=TRUE,ring=5,retry=3;
	char c;
	char *m,*p;

	HoldSerialIO=NULL;
	HoldSerialMP=NULL;
	strcpy(username.Handle,SYS->Handle);
	RPGserver(SERVER_GETUSERNAME,(UBYTE *)&username);
	strcpy(SYSID,username.ID);
	for(i=0; i<5; i++) {
		memset(RPC[0][i],0,sizeof(struct RPC));
		memset(RPC[1][i],0,sizeof(struct RPC));
	}
	ANSI=2;
	baud=FALSE;
	bold=FALSE;
	chat=FALSE;
	echo=TRUE;
	local=FALSE;
	logoff=FALSE;
	paws=FALSE;
	rows=25;
	memset(callerid,0,sizeof(callerid));
	memset(inbuf,0,INBUF_SIZE);

	if(RPGBBSmode==BBS) {
		OUTCON("\33c");
		title("logon");
		iconify.NextItem=&Mlogon;
		if(window)
			SetMenuStrip(window,&project);
		sprintf(outbuf," %s°±²\33[7m %lu calls \33[27m²±°",fore(BLACK),SYS->History.Calls);
		OUTCON(outbuf);
		sprintf(outbuf," %s°±²Û%s²±° %s°±²%s\33[7m %s \33[27m%s²±°%s °±²%sÛ²±°",fore(BLACK),back(GRY),fore(WHITE),back(BLU),SYS->Name,back(GRY),fore(BLACK),back(BLK));
		OUTCON(outbuf);
		sprintf(outbuf," %s°±²\33[7m %u today \33[27m²±°",fore(BLACK),SYS->CallsToday);
		OUTCON(outbuf);NL;NL;
		sprintf(outbuf,"%sLast callers were:%s ",fore(CYN),fore(GRY));
		OUTCON(outbuf);
		for(i=0; i<3; i++) {
			sprintf(outbuf,"%s %s(%s%s%s)%s\15\12",CALLERS->Last[i],fore(CYN),fore(GRY),CALLERS->Reason[i],fore(CYN),fore(GRY));
			OUTCON(outbuf);
			OUTCON("\33[19C");
		}
		OUTCON("\15\12");
		if(strlen(BBSprefs.Device)) {
			sprintf(outbuf,"%sDriver:%s%s  %sUnit:%s%u  %sBaud:%s%lu\15\12\12",fore(CYN),fore(GRY),BBSprefs.Device,fore(CYN),fore(GRY),BBSprefs.Unit,fore(CYN),fore(GRY),BBSprefs.Baud);
			OUTCON(outbuf);
			if(!(SERCON.SerialMP=(struct MsgPort *)CreatePort(NULL,0)))
				return(FALSE);
			if(!(SERCON.SerialIO=(struct IOExtSer *)CreateExtIO(SERCON.SerialMP,sizeof(struct IOExtSer))))
				return(FALSE);
			SERCON.SerialIO->io_SerFlags=SERF_XDISABLED | SERF_SHARED | SERF_7WIRE;
			if(OpenDevice(BBSprefs.Device,BBSprefs.Unit,(struct IORequest *)SERCON.SerialIO,0))
				return(FALSE);
			SERCON.SerialIO->IOSer.io_Command=SDCMD_SETPARAMS;
			SERCON.SerialIO->io_CtlChar=SER_DEFAULT_CTLCHAR;
			SERCON.SerialIO->io_RBufLen=sizeof(Serbuf);
			SERCON.SerialIO->io_Baud=BBSprefs.Baud;
			SERCON.SerialIO->io_ReadLen=8;
			SERCON.SerialIO->io_WriteLen=8;
			SERCON.SerialIO->io_SerFlags=SERF_XDISABLED | SERF_SHARED | SERF_7WIRE;
			SERCON.SerialIO->io_StopBits=1;
			DoIO((struct IORequest *)SERCON.SerialIO);
			sprintf(outbuf,"%sResetting modem...%s\15\12",fore(CYN),fore(GRY));
			OUTCON(outbuf);
			modem_cmd("ATZ");
			sprintf(outbuf,"%sInitializing modem...%s\15\12",fore(CYN),fore(GRY));
			OUTCON(outbuf);
			modem_cmd(BBSprefs.Init);
		}
		else {
			SERCON.SerialIO=NULL;
			SERCON.SerialMP=NULL;
		}
	
		sprintf(outbuf,"%s\15\12Waiting for a caller...%s\15\12",fore(CYN),fore(GRY));
		OUTCON(outbuf);

/*		if(BBSprefs.Sleep) {
			struct MsgPort *faxport;
			Forbid();
			if((faxport=(struct MsgPort *)CreatePort("RPGBBSfax",0))) {
				Permit();
				OUTCON("{SLEEPING}");
				FaxSignal=1L << faxport->mp_SigBit;
				FOREVER {
					signal=Wait(ConsoleSignal | FaxSignal);
					if(signal & ConsoleSignal) {
					}
					if(signal & FaxSignal) {
						while((faxmsg=(struct faxmsg *)GetMsg(faxport))) {
							ReplyMsg((struct Message *)faxmsg);
						}
					}
				}
				DeletePort(faxport);
			}
			else {
				Permit();
				OUTCON("{fax port?}");
			}
		}
*/
		while(loop) {
			INPUT(0);
			switch(source) {
				case 1:
					OUTCON(Serbuf);
					for(i=0; i<strlen(Serbuf); i++) {
						if(iscntrl(Serbuf[i]) || strlen(inbuf)+1==INBUF_SIZE) {
							if(!strnicmp(inbuf,"NMBR = ",7)) {
								strcpy(callerid,&inbuf[7]);
								if(strlen(callerid)==10) {
									sprintf(outbuf,"%.3s-%.3s-%.4s",&callerid[0],&callerid[3],&callerid[6]);
									strcpy(callerid,outbuf);
								}
							}
							if(!strnicmp(inbuf,"RING",4)) {
								ring--;
								if(!ring) {
									DisplayBeep(NULL);
									return(TRUE);
								}
								RPGserver(SERVER_WAKEUP,NULL);
							}
							while((p=strchr(inbuf,'C'))) {
								if(!strncmp(p,"CARRIER",7)) {
									baud=atol(p+7);
									if(!baud)
										baud=1;
									loop=FALSE;
								}
								if(!strncmp(p,"CONNECT",7)) {
									if(!baud) {
										baud=atol(p+7);
										if(!baud)
											baud=300;
									}
									loop=FALSE;
								}
								*p=' ';
							}
							memset(inbuf,0,INBUF_SIZE);
						}
						else
							inbuf[strlen(inbuf)]=toupper(Serbuf[i]);
					}
					break;
				case 2:
					if(SERCON.SerialIO)
						OUTSER(Serbuf);
					break;
				case 3:
					menu=Serbuf[0];
					menuitem=Serbuf[1];
					subitem=Serbuf[2];
					switch(menu) {
						case 0:	/*  Project  */
							switch(menuitem) {
								case 0:
									if(viewwindow) {
										CloseWindow(viewwindow);
										viewwindow=NULL;
									}
									CloseConsole(&SERCON);
									ClearMenuStrip(window);
									CloseWindow(window);
									CloseFont(SERCON.TextFont);
									CloseLibrary(DiskfontBase);
									window=NULL;SERCON.Window=NULL;
									OpenConsole(&SERCON);
									if((appobj=GetDiskObject(IconName))) {
										appobj->do_Type=NULL;
										appobj->do_CurrentX=NO_ICON_POSITION;
										appobj->do_CurrentY=NO_ICON_POSITION;
										SERCON.IconMP=(struct MsgPort *)CreatePort(NULL,0);
										appicon=AddAppIconA(NULL,NULL,IconName,SERCON.IconMP,NULL,appobj,NULL);
									}
									break;
								case 1:
									if(SERCON.SerialIO) {
										sprintf(outbuf,"%sTaking modem offline...%s\15\12",fore(CYN),fore(GRY));
										OUTCON(outbuf);
										modem_cmd("ATH1M0");
										Delay(50);
										HoldSerialIO=SERCON.SerialIO;
										HoldSerialMP=SERCON.SerialMP;
										SERCON.SerialIO=NULL;
										SERCON.SerialMP=NULL;
									}
									local=TRUE;
									loop=FALSE;
									break;
								case 2:
									if(viewwindow) {
										CloseWindow(viewwindow);
										viewwindow=NULL;
									}
									break;
								case 3:
									loop=FALSE;
									break;
							}
							break;
					}
					break;
				case 4:
					loop=FALSE;
					break;
				case 5:
					RemoveAppIcon(appicon);
					DeletePort(SERCON.IconMP);
					SERCON.IconMP=NULL;
					FreeDiskObject(appobj);
					appobj=NULL;
					CloseConsole(&SERCON);
					OpenGUI();
					baud=5;
					loop=FALSE;
					break;
			}
		}
		if(baud<300 && !local)
			return(baud);
		if(!local && BBSprefs.Baud<9600) {
			SERCON.SerialIO->IOSer.io_Command=SDCMD_SETPARAMS;
			SERCON.SerialIO->io_Baud=baud;
			DoIO((struct IORequest *)SERCON.SerialIO);
		}
		iconify.NextItem=&Mlogoff;
		if(window)
			SetMenuStrip(window,&project);
		OUTCON("\33c");
		if(baud) {
			sprintf(outbuf,"%u baud connection established.",baud);
			OUT(outbuf);NL;NL;
			Delay(50);
		}
	}
	if(RPGBBSmode==DOOR) {
		rawcon(TRUE);
		local=TRUE;
	}

	cls();

	if(BBSprefs.CallerID && !local) {
		if(!strlen(callerid))
			return(TRUE);
		if(callerid[0]=='P') {
			type("Notices/CallerID",TRUE);
			pause();
			if(cd())
				return(TRUE);
		}
	}

	type("Notices/Logon",FALSE);
	memset(Serbuf,0,sizeof(Serbuf));
	GetSystemDate(Today);
	GetSystemTime(Time);
	julian(&Julian,Today);
	t=atol(Time)/100;
	if(!local)
		OUTSER("\5");

	while(retry) {
		NL;
		sprintf(prompt,"%sWho dares to enter my dank domain <or NEW>? ",fore(CYN));
		timeout=16;
		if(GetRPC(ONLINE)) {
			ANSI=(RPGBBSmode==DOOR || !local) ? PLAYER.Emulation : 3;
			rows=(RPGBBSmode==DOOR || !local) ? PLAYER.Rows : SERCON.MaxRow;
			if(RPGBBSmode==BBS) {
				sprintf(outbuf,"%s%s, enter your password: ",fore(CYN),PLAYER.Handle);
				OUT(outbuf);
				echo=FALSE;
				timeout=16;
				ins(9);NL;
				echo=TRUE;
				if(!stricmp(inbuf,PLAYER.Password))
					break;
			}
			if(RPGBBSmode==DOOR && !stricmp(PLAYER.Name,argument))
				break;
		}
		else
		if(cd()) {
			retry=0;
			continue;
		}
		else
		if(!stricmp(inbuf,"new")) {
			NewUser();
			if(!strlen(PLAYER.ID))
				retry=0;
			break;
		}
		else {
			beep();
			OUT(fore(GRY));
			OUT("Invalid response.");NL;
		}
		NL;
		switch(--retry) {
			case 2:
				OUT("The guards eye you suspiciously.");
				break;
			case 1:
				OUT("The guards aim their crossbows at you.");
				break;
			default:
				OUT("The last thing you ever feel is several quarrels cutting deep into your chest.");NL;
				Delay(50);
				break;
		}
		NL;
	}
	if(!retry)
		return(RPGBBSmode==BBS);

	if(!RPGserver(SERVER_CONNECT,NULL))
		return(RPGBBSmode==BBS);
	broadcast("logged on");

	if(BBSprefs.Delay) {
		i=1440*(Julian-PLAYER.LastDate)+(t/100*60+t%100)-(PLAYER.LastTime/100*60+PLAYER.LastTime%100);
		if(PLAYER.CallsToday>0 && i<BBSprefs.Delay) {
			beep();
			sprintf(outbuf,"You were last on just %d minutes ago.",i);
			OUT(outbuf);NL;
			sprintf(outbuf,"Please wait at least %d minutes between calls.",BBSprefs.Delay);
			OUT(outbuf);NL;
			Delay(50);
			return(RPGBBSmode==BBS);
		}
	}

	if(PLAYER.Access==1) {
		AutoVerify();
		if(cd())
			return(RPGBBSmode==BBS);
	}

	if(SYS->LastDate!=Julian) {
		SYS->LastDate=Julian;
		SYS->CallsToday=0;
	}
	SYS->CallsToday++;
	SYS->History.Calls++;
	if(SYS->CallsToday>=SYS->Current.Calls)
		SYS->Current.Calls=SYS->CallsToday;
	RPGserver(SERVER_PUTUSER,(UBYTE *)SYS);

	if(PLAYER.LastDate!=Julian)
		PLAYER.CallsToday=0;

	if(PLAYER.CallsToday>ACCESS(PLAYER.Access)->Calls && !local) {
		beep();
		OUT("You have already used all your calls for today.  Please call back tomorrow!");NL;
		return(RPGBBSmode==BBS);
	}
	if(ACCESS(PLAYER.Access)->Weekend=='Y' && (Julian-1)%7!=0 && (Julian-1)%7!=6) {
		beep();
		OUT("You are restricted to Weekend play only.  Please call back then!");NL;
		return(RPGBBSmode==BBS);
	}

	title("logon");
	cls();
	sprintf(outbuf,"%s--=:))%s%s %s %s%s((:=--",fore(RED),fore(WHITE),back(RED),SYS->Name,back(BLK),fore(RED));
	OUT(outbuf);NL;NL;
	sprintf(outbuf,"%sCaller#:%s %lu",fore(CYN),fore(WHITE),SYS->History.Calls);
	OUT(outbuf);NL;
	sprintf(outbuf,"%s Online:%s %s",fore(CYN),fore(WHITE),PLAYER.Handle);
	OUT(outbuf);NL;
	sprintf(outbuf,"%s Access:%s %s",fore(CYN),fore(WHITE),ACCESS(PLAYER.Access)->Name);
	OUT(outbuf);
	acclvl=0;m=NULL;
	if(Julian>=PLAYER.ExpireDate) {
		sprintf(outbuf," %s(%sEXPIRED%s)",fore(BLACK),fore(BRED),fore(BLACK));
		OUT(outbuf);
		m="deleted user";
	}
	else
	if(PLAYER.CallsToday>=ACCESS(PLAYER.Access)->Calls || Julian<SYS->BirthDate) {
		PLAYER.CallsToday++;
		sprintf(outbuf," %s(%sVISITING%s)",fore(BLACK),fore(YELLOW),fore(BLACK));
		OUT(outbuf);
		m="visitor";
	}
	else
	if(BBSprefs.CallerID && strlen(PLAYER.DataPhone) && stricmp(PLAYER.DataPhone,callerid) && callerid[0]!='O' && callerid[0]!='P') {
		sprintf(outbuf," %s(%sELSEWHERE @ %s%s)",fore(BLACK),fore(YELLOW),callerid,fore(BLACK));
		OUT(outbuf);
		m="remote user";
	}
	else
	if(BBSprefs.CallerID && callerid[0]=='P') {
		sprintf(outbuf," %s(%sPRIVACY%s)",fore(BLACK),fore(YELLOW),fore(BLACK));
		OUT(outbuf);
		m="private call";
	}
	else {
		PLAYER.CallsToday++;
		PLAYER.Current.Calls++;
		PLAYER.History.Calls++;
		if(!strcmp(PLAYER.Status,"jail") && dice(100)>PLAYER.CHA) {
			JOUST(PLAYER.Level);
			modf(d*(100-PLAYER.CHA+1)/100.,&d);
			NORMAL;NL;NL;
			OUT("You are still locked-up in jail.");NL;
			sprintf(outbuf,"It will cost you %s to get bailed-out and to continue play.",money(d,ANSI));
			OUT(outbuf);NL;NL;
			sprintf(prompt,"%sWill you pay (Y/N)? ",fore(CYN));
			OUT(prompt);
			c=inkey('Y','N');
			NL;NL;
			if(c=='Y') {
				OUT("You are set free to continue your adventuring.");NL;
				acclvl=PLAYER.Access;
				memset(PLAYER.Status,0,sizeof(PLAYER.Status));
				PLAYER.Gold-=d;
				if(PLAYER.Gold<0.) {
					PLAYER.Bank+=PLAYER.Gold;
					PLAYER.Gold=0.;
					if(PLAYER.Bank<0.) {
						PLAYER.Loan-=PLAYER.Bank;
						PLAYER.Bank=0.;
					}
				}
			}
			else {
				OUT("You are left brooding with your fellow cellmates.");NL;
				m="prisoner";
			}
		}
		else {
			acclvl=PLAYER.Access;
			memset(PLAYER.Status,0,sizeof(PLAYER.Status));
		}
	}
	NL;
	if(ACCESS(acclvl)->RolePlay=='Y') {
		arena=3;
		bail=1;
		brawl=3;
		charity=1;
		dungeon=3;
		joust=3;
		naval=3;
		party=1;
		tiny=0;
		PLAYER.ExpLevel=PLAYER.Level;
		if(PLAYER.Level==1 && PLAYER.Current.JoustLoss>2*PLAYER.Current.JoustWin) {
			PLAYER.Current.JoustWin=0;
			PLAYER.Current.JoustLoss=0;
			OUT("Your jousting stats have been reset.");NL;
		}
		m=NULL;
	}
	else {
		arena=0;
		bail=0;
		brawl=0;
		charity=0;
		dungeon=0;
		joust=0;
		naval=0;
		party=0;
		tiny=3;
	}

	if(ACCESS(acclvl)->RolePlay=='Y')
		if(!ONLINE->class_origin && !ONLINE->class_type) {
			ReKeySequence(ONLINE);
			reroll(ONLINE);
			assign(200);
		}

	fdate(dated,PLAYER.LastDate);
	sprintf(inbuf,"%06lu",PLAYER.LastTime*100L);
	ftime(timed,inbuf);
	sprintf(outbuf,"%sLast on:%s %s ",fore(CYN),fore(WHITE),dated,timed);
	OUT(outbuf);NL;NL;
	PLAYER.LastDate=Julian;
	PLAYER.LastTime=t;
	ftime(timed,Time);
	strmfp(filename,"T:",PLAYER.ID);
	if((newfp=fopen(filename,"w"))) {
		fprintf(newfp,"%s-=>%s %s %s<=-\n",fore(BLU),fore(YELLOW),(timed[0]==' ' ? &timed[1] : timed),fore(BLU));
		fprintf(newfp,"%s%s%s logged on as a ",fore(CYAN),PLAYER.Handle,fore(CYN));
		if(m)
			fprintf(newfp,"%s\n",m);
		else
			fprintf(newfp,"level %s%u %s\33[0;37m\n",fore(WHITE),PLAYER.Level,CLASS(ONLINE)->Name);
		fclose(newfp);
	}

	if(acclvl) {
		sprintf(outbuf,"%sYou have %d calls remaining.",fore(CYN),ACCESS(acclvl)->Calls-PLAYER.CallsToday);
		OUT(outbuf);NL;NL;
		jdate(dated,PLAYER.BirthDate);
		if(!strncmp(&Today[4],&dated[4],4) && PLAYER.CallsToday==1) {
			JOUST(PLAYER.Level);
			d*=50.;
			sprintf(outbuf,"%sHappy Birthday!!  You receive %s%s as a present.",fore(CYAN),money(d,ANSI),fore(CYAN));
			OUT(outbuf);NL;NL;
			PLAYER.Gold+=d;
			i=atol(Today)/10000-atol(dated)/10000;
			sprintf(outbuf,"%scelebrating %s%u%s%s birthday",fore(CYN),fore(YELLOW),i,(i%10==1 ? "st" : i%10==2 ? "nd" : i%10==3 ? "rd" : "th"),fore(CYN));
			news(outbuf);
		}
	}
	strcpy(gm[0].Name,PLAYER.Gang);
	if(!RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0]))
		memset(PLAYER.Gang,0,sizeof(PLAYER.Gang));
	RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);

	sprintf(outbuf,"%sLast callers were:%s ",fore(CYN),fore(GRY));
	OUT(outbuf);
	for(i=0; i<3; i++) {
		sprintf(outbuf,"%s %s(%s%s%s)%s",CALLERS->Last[i],fore(CYN),fore(GRY),CALLERS->Reason[i],fore(CYN),fore(GRY));
		OUT(outbuf);NL;
		OUT("                   ");
	}
	NL;
	memset(reason,0,sizeof(reason));
	if(Julian<SYS->BirthDate) {
		beep();
		fdate(dated,SYS->BirthDate);
		sprintf(outbuf,"%sThe game will resume %s%s",fore(CYN),fore(CYAN),dated);
		OUT(outbuf);NL;
	}
	if(ACCESS(acclvl)->RolePlay=='Y') {
		strcpy(CALLERS->Last[2],CALLERS->Last[1]);strcpy(CALLERS->Reason[2],CALLERS->Reason[1]);
		strcpy(CALLERS->Last[1],CALLERS->Last[0]);strcpy(CALLERS->Reason[1],CALLERS->Reason[0]);
		strcpy(CALLERS->Last[0],PLAYER.Handle);strcpy(CALLERS->Reason[0],"currently online");
	}
	pause();

	strmfp(filename,NOTESpath,PLAYER.ID);
	if(type(filename,TRUE))
		remove(filename);
	if(cd())
		return(RPGBBSmode==BBS);

	cls();
	sprintf(outbuf,"       %s------------",fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"--=:))%s Announcement %s((:=--",fore(YELLOW),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"       ------------");
	OUT(outbuf);NORMAL;NL;
	type("Notices/Announcement",FALSE);NL;
	if(ACCESS(acclvl)->Sysop=='Y') {
		sprintf(prompt,"%sChange Announcement (Y/N)? ",fore(CYN));
		OUT(prompt);
		if(inkey('N','N')=='Y') {
			NL;NL;
			Editor(NULL,10);
			if(numline) {
				if((fp=fopen("Notices/Announcement","w"))) {
					fdate(dated,Julian);
					ftime(timed,Time);
					fprintf(fp,"%sDate:%s %s %s\n",fore(CYN),fore(GRY),dated,timed);
					fprintf(fp,"%sFrom:%s %s%s\n\n",fore(CYN),fore(GRY),PLAYER.Handle,fore(WHITE));
					for(i=0; i<numline; i++)
						fprintf(fp,"%s\n",line[i]);
					fclose(fp);
				}
			}
		}
		NL;
	}
	sprintf(outbuf,"       %s------------",fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"--=:))%s Auto-Message %s((:=--",fore(YELLOW),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"       ------------");
	OUT(outbuf);NORMAL;NL;
	type("Notices/Auto-Message",FALSE);NL;
	if(ACCESS(acclvl)->RolePlay=='Y') {
		sprintf(prompt,"%sChange Auto-Message (Y/N)? ",fore(CYN));
		OUT(prompt);
		if(inkey('N','N')=='Y') {
			NL;NL;
			Editor(NULL,5);
			if(numline) {
				if((fp=fopen("Notices/Auto-Message","w"))) {
					fdate(dated,Julian);
					ftime(timed,Time);
					fprintf(fp,"%sDate:%s %s %s\n",fore(CYN),fore(GRY),dated,timed);
					fprintf(fp,"%sFrom:%s %s%s\n\n",fore(CYN),fore(GRY),PLAYER.Handle,fore(WHITE));
					sprintf(outbuf,"%schanged the Auto-Message to read:",fore(CYN));
					news(outbuf);
					for(i=0; i<numline; i++) {
						fprintf(fp,"%s\n",line[i]);
						news(line[i]);
					}
					fclose(fp);
				}
			}
		}
		NL;
	}
	else {
		pause();
		NL;
	}

	if(cd())
		return(RPGBBSmode==BBS);
	for(i=1; i<999 && !cd(); i++) {
		sprintf(outbuf,"%s.%03d",PLAYER.ID,i);
		strmfp(filename,EMAILpath,outbuf);
		if(ReadMail(filename)) {
			remove(filename);
			sprintf(prompt,"%sReply (Y/N)? ",fore(CYN));
			do {
				OUT(prompt);
				c=inkey('N','N');
				NL;
			} while(c!='Y' && c!='N');
			NL;
			if(c=='Y') {
				Editor(RPC[1][0],99);
				for(retry=1; retry<999; retry++) {
					sprintf(outbuf,"%s.%03d",ENEMY.ID,retry);
					strmfp(filename,EMAILpath,outbuf);
					if(!(fp=fopen(filename,"r")))
						break;
					fclose(fp);
				}
				WriteMail(filename);
			}
		}
		else
			break;
	}

	if(ACCESS(acclvl)->RolePlay=='Y')
		TaxMan();

	MainMenu();

	title("logoff");
	CurrentDir(root);

	GetSystemDate(Today);
	GetSystemTime(Time);
	julian(&Julian,Today);
	fdate(dated,Julian);
	ftime(timed,Time);
	if(PLAYER.LastDate!=Julian)
		PLAYER.CallsToday=0;
	PLAYER.LastDate=Julian;
	PLAYER.LastTime=atol(Time)/100;
	RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);

	memset(outbuf,0,OUTBUF_SIZE);
	if(ACCESS(acclvl)->RolePlay=='Y') {
		sprintf(outbuf,"%slogged off as a level %s%u %s",fore(CYN),fore(WHITE),PLAYER.Level,CLASS(ONLINE)->Name);
		news(outbuf);
		sprintf(outbuf,"%s(%s%s%s)\n",fore(CYN),fore(WHITE),reason,fore(CYN));
	}
	news(outbuf);
	strmfp(filename,"Notices","Today.IBM");
	if((newfp=fopen(filename,"a"))) {
		strmfp(filename,"T:",PLAYER.ID);
		if((fp=fopen(filename,"r"))) {
			while(fgets(inbuf,INBUF_SIZE,fp))
				fputs(inbuf,newfp);
			fclose(fp);
		}
		fclose(newfp);
	}
	strmfp(filename,"Notices","Today");
	if((newfp=fopen(filename,"a"))) {
		strmfp(filename,"T:",PLAYER.ID);
		if((fp=fopen(filename,"r"))) {
			while(fgets(inbuf,INBUF_SIZE,fp)) {
				while((p=strchr(inbuf,'\33'))) {
					m=strchr(p,'m');
					strcpy(p,++m);
				}
				fputs(inbuf,newfp);
			}
			fclose(fp);
			remove(filename);
		}
		fclose(newfp);
	}
	if(Julian>=SYS->BirthDate)
		for(i=0; i<3; i++)
			if(stricmp(CALLERS->Last[i],PLAYER.Handle)==0) {
				strcpy(CALLERS->Reason[i],reason);
				break;
			}
	if(logoff) {
		sound("logoff",48);
		type("Notices/Logoff",FALSE);
		Delay(baud ? 14400/baud*10 : 50);
		sprintf(outbuf,"%sRPGBBS Deluxe %s %s written by Robert Hurst",fore(BLACK),VERSION,__AMIGADATE__);
		NL;OUT(outbuf);NL;NORMAL;
		Delay(50);
	}
	broadcast(reason);
	return(RPGBBSmode==BBS);
}
