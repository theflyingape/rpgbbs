#include "/include/all.h"
#include "global.h"

void charstats(struct RPC *rpc)
{
	static char *magic[] = {"Wand","Scroll","Spell","Spell"};
	int i;
	char c='N',*p;

	if(ACCESS(acclvl)->RolePlay=='Y') {
		JOUST(PLAYER.Level);
		modf(d/10.,&d);
		sprintf(prompt,"%sScout another user for %s %s(Y/N)? ",fore(CYN),money(d,ANSI),fore(CYN));
		OUT(prompt);
		c=inkey('N','N');
	}
	if(c=='Y') {
		NL;NL;
		sprintf(prompt,"%sScout user? ",fore(CYN));
		if(!GetRPC(rpc))
			return;
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
	else
		memcpy(rpc,ONLINE,sizeof(struct RPC));
	cls();
	memset(line[52],'-',52);
	sprintf(line[1],"%s",rpc->user.Handle);
	i=20-strlen(line[1]);
	OUT(fore(BLU));
	sprintf(outbuf,"+%.*s=:))",11+i/2,line[52]);
	OUT(outbuf);
	if(ANSI>1) {
		OUT(back(BLU));
		OUT(fore(YELLOW));
	}
	else
		REVERSE;
	sprintf(outbuf," %s ",line[1]);
	OUT(outbuf);
	if(ANSI>1) {
		OUT(back(BLK));
		OUT(fore(BLU));
	}
	else
		NORMAL;
	sprintf(outbuf,"((:=%.*s+",11+i/2+i%2,line[52]);
	OUT(outbuf);NL;
	fdate(line[0],rpc->user.BirthDate);
	sprintf(outbuf,"%s|%s    %sTitle:%s %-18s %sBorn:%s %s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),ACCESS(rpc->user.Access)->Name,fore(GRY),fore(WHITE),line[0],back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%s (%c)",CLASS(rpc)->Name,rpc->user.Gender);
	sprintf(line[1],"%.8g",rpc->user.Experience);
	sprintf(outbuf,"%s|%s    %sClass:%s %-19s %sExp:%s %-15s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),line[1],back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%u.%02u",rpc->user.Immortal,rpc->user.Level);
	sprintf(line[1],"%.8g",EXP(rpc->user.Level));
	sprintf(outbuf,"%s|%s %sImmortal:%s %-18s %sNeed:%s %-15s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),line[1],back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%3u (%u,%u)",rpc->STR,rpc->user.STR,CLASS(rpc)->MaxSTR);
	sprintf(line[1],"%s",money(rpc->user.Gold,FALSE));
	sprintf(outbuf,"%s|%s      %sStr:%s%-19s %sHand:%s %s%*s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),money(rpc->user.Gold,ANSI),15-strlen(line[1]),"",back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%3u (%u,%u)",rpc->INT,rpc->user.INT,CLASS(rpc)->MaxINT);
	sprintf(line[1],"%s",money(rpc->user.Bank,FALSE));
	sprintf(outbuf,"%s|%s      %sInt:%s%-19s %sBank:%s %s%*s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),money(rpc->user.Bank,ANSI),15-strlen(line[1]),"",back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%3u (%u,%u)",rpc->DEX,rpc->user.DEX,CLASS(rpc)->MaxDEX);
	sprintf(line[1],"%s",money(rpc->user.Loan,FALSE));
	sprintf(outbuf,"%s|%s      %sDex:%s%-19s %sLoan:%s %s%*s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),money(rpc->user.Loan,ANSI),15-strlen(line[1]),"",back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%3u (%u,%u)",rpc->CHA,rpc->user.CHA,CLASS(rpc)->MaxCHA);
	sprintf(line[1],"%u:%u  %.3f",rpc->user.Posts,rpc->user.History.Calls,(double)rpc->user.Posts/rpc->user.History.Calls);
	sprintf(outbuf,"%s|%s      %sCha:%s%-18s %sPost%:%s %s%*s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),line[1],15-strlen(line[1]),"",back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%u/%u",rpc->HP,rpc->user.HP);
	d=(double)rpc->user.Uploads/(rpc->user.Downloads ? rpc->user.Downloads : 1.);
	sprintf(line[1],"%u:%u  %.3f",rpc->user.Uploads,rpc->user.Downloads,d);
	sprintf(outbuf,"%s|%s       %sHP:%s %-17s %sFile%:%s %s%*s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),line[1],15-strlen(line[1]),"",back(BLK),fore(BLU));
	OUT(outbuf);NL;
	if(rpc->user.SP) {
		sprintf(line[0],"%u/%u",rpc->SP,rpc->user.SP);
		sprintf(outbuf,"%s|%s       %sSP:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],back(BLK),fore(BLU));
		OUT(outbuf);NL;
	}
	sprintf(line[0],"%s (%+d,%+d)",WEAPON(rpc)->Name,rpc->user.WCmod,rpc->ToWC);
	sprintf(outbuf,"%s|%s   %sWeapon:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(line[0],"%s (%+d,%+d)",ARMOR(rpc)->Name,rpc->user.ACmod,rpc->ToAC);
	sprintf(outbuf,"%s|%s    %sArmor:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],back(BLK),fore(BLU));
	OUT(outbuf);NL;
	if(rpc->user.Poison) {
		strcpy(line[0],"");
		for(i=0; i<NUMPOISON; i++)
			if(rpc->user.Poison & (UWORD)pow2((double)i)) {
				sprintf(line[1],"%s%d",strlen(line[0]) ? "," : "",i+1);
				strcat(line[0],line[1]);
			}
		sprintf(outbuf,"%s|%s  %sPoisons:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],back(BLK),fore(BLU));
		OUT(outbuf);NL;
	}
	if(rpc->user.Spell) {
		strcpy(line[0],"");
		for(i=0; i<(NUMMAGIC<16 ? NUMMAGIC : 16); i++)
			if(rpc->user.Spell & (UWORD)pow2((double)i)) {
				sprintf(line[1],"%s%.*s",strlen(line[0]) ? "," : "",strlen(MAGIC(i)->Name)<7 ? strlen(MAGIC(i)->Name) : 3,MAGIC(i)->Name);
				strcat(line[0],line[1]);
			}
		strcpy(line[1],"");
		if(strlen(line[0])>40) {
			for(p=line[0],i=40; i>0 && p[i]!=','; i--);
			p[i]=NULL;
			strcpy(line[1],&p[i+1]);
		}
		sprintf(outbuf,"%s|%s %s%7ss:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),magic[CLASS(rpc)->Magic-1],fore(WHITE),line[0],back(BLK),fore(BLU));
		OUT(outbuf);NL;
		if(strlen(line[1])) {
			sprintf(outbuf,"%s|%s           %s%-40s %s%s|",fore(BLU),back(BLU),fore(WHITE),line[1],back(BLK),fore(BLU));
			OUT(outbuf);NL;
		}
	}
	if(rpc->user.XSpell) {
		strcpy(line[0],"");
		for(i=0; i<NUMMAGIC-16; i++)
			if(rpc->user.XSpell & (UWORD)pow2((double)i)) {
				sprintf(line[1],"%s%s",strlen(line[0]) ? "," : "",MAGIC(i+16)->Name);
				strcat(line[0],line[1]);
			}
		strcpy(line[1],"");
		if(strlen(line[0])>40) {
			for(p=line[0],i=40; i>0 && p[i]!=','; i--);
			p[i]=NULL;
			strcpy(line[1],&p[i+1]);
		}
		sprintf(outbuf,"%s|%s %s%7ss:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),magic[CLASS(rpc)->Magic-1],fore(WHITE),line[0],back(BLK),fore(BLU));
		OUT(outbuf);NL;
		if(strlen(line[1])) {
			sprintf(outbuf,"%s|%s           %s%-40s %s%s|",fore(BLU),back(BLU),fore(WHITE),line[1],back(BLK),fore(BLU));
			OUT(outbuf);NL;
		}
	}
	sprintf(outbuf,"%s|%s %sDwelling:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),REALESTATE(rpc->user.RealEstate)->Name,back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s %sSecurity:%s %-40s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),SECURITY(rpc->user.Security)->Name,back(BLK),fore(BLU));
	OUT(outbuf);NL;
	if(strlen(rpc->user.Gang)) {
		strcpy(gm[0].Name,rpc->user.Gang);
		if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
			sprintf(line[0],"%.20s",gm[0].Name);
			memset(line[1],0,16);
			for(i=0; i<4; i++)
				if(strcmp(gm[0].MemberID[i],rpc->user.ID)) {
					sprintf(outbuf,"%c%s",(strlen(line[1]) ? ',' : ' '),(strlen(gm[0].MemberID[i]) ? gm[0].MemberID[i] : "*"));
					strcat(line[1],outbuf);
				}
			sprintf(outbuf,"%s|%s    %sParty:%s %-20s %swith%s%-15s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),line[1],back(BLK),fore(BLU));
			OUT(outbuf);NL;
		}
	}
	if(rpc->user.Hull) {
		sprintf(line[0],"%d:%d",rpc->HULL,rpc->user.Hull);
		sprintf(line[1],"%d:%d  %s",rpc->user.Cannon,rpc->user.Hull/50,rpc->user.Ram=='Y' ? "(RAM)" : "");
		sprintf(outbuf,"%s|%s  %sWarship:%s %-9s        %sCannon:%s %-15s %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),line[0],fore(GRY),fore(WHITE),line[1],back(BLK),fore(BLU));
		OUT(outbuf);NL;
	}
	if(strlen(rpc->user.Blessed)) {
		sprintf(outbuf,"%s|%s     %sNote:%s You were blessed by %-4s                 %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),rpc->user.Blessed,back(BLK),fore(BLU));
		OUT(outbuf);NL;
	}
	if(strlen(rpc->user.Cursed)) {
		sprintf(outbuf,"%s|%s     %sNote:%s You were cursed by %-4s                  %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),rpc->user.Cursed,back(BLK),fore(BLU));
		OUT(outbuf);NL;
	}
	sprintf(outbuf,"%s+%.52s+",fore(BLU),line[52]);
	OUT(outbuf);NL;
	if((c=more())!='Y')
		return;
	sprintf(line[0],"%*s%s",(9-strlen(ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Name))/2,"",ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Name);
	sprintf(outbuf,"%s|%s %sCurrent Stats  %-9s  All-Time %s%s|",fore(BLU),back(BLU),fore(YELLOW),line[0],back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s    %sCalls:%s %3u   %3u/%-4u  %3u/%-4u %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),
			rpc->user.Current.Calls,
			ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Calls[0].What,ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Calls[1].What,
			ServerTable->class[0]->Character[0]->Calls[0].What,ServerTable->class[0]->Character[0]->Calls[1].What,
			back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s    %sKills:%s %3u   %3u/%-4u  %3u/%-4u %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),
			rpc->user.Current.Kills,
			ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Kills[0].What,ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Kills[1].What,
			ServerTable->class[0]->Character[0]->Kills[0].What,ServerTable->class[0]->Character[0]->Kills[1].What,
			back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s   %sKilled:%s %3u   %3u/%-4u  %3u/%-4u %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),
			rpc->user.Current.Killed,
			ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Killed[0].What,ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Killed[1].What,
			ServerTable->class[0]->Character[0]->Killed[0].What,ServerTable->class[0]->Character[0]->Killed[1].What,
			back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s %sRetreats:%s %3u   %3u/%-4u  %3u/%-4u %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),
			rpc->user.Current.Retreats,
			ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Retreats[0].What,ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Retreats[1].What,
			ServerTable->class[0]->Character[0]->Retreats[0].What,ServerTable->class[0]->Character[0]->Retreats[1].What,
			back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s      %sKOs:%s %3u   %3u/%-4u  %3u/%-4u %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),
			rpc->user.Current.KOs,
			ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Brawls[0].What,ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Brawls[1].What,
			ServerTable->class[0]->Character[0]->Brawls[0].What,ServerTable->class[0]->Character[0]->Brawls[1].What,
			back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s|%s   %sJousts:%s %3u   %3u/%-4u  %3u/%-4u %s%s|",fore(BLU),back(BLU),fore(GRY),fore(WHITE),
			rpc->user.Current.JoustWin,
			ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Jousts[0].What,ServerTable->class[rpc->class_origin]->Character[rpc->class_type]->Jousts[1].What,
			ServerTable->class[0]->Character[0]->Jousts[0].What,ServerTable->class[0]->Character[0]->Jousts[1].What,
			back(BLK),fore(BLU));
	OUT(outbuf);NL;
	sprintf(outbuf,"%s+%.36s+",fore(BLU),line[52]);
	OUT(outbuf);NL;
	paws=!PLAYER.Expert;
}

void MainMenu(void)
{
	static struct RPGmenu __far RPGmenu = { "Main",BLU,19,
							{	{ 'A',"Arena: Fight and Joust" },
								{ 'B',"Bulletin Boards" },
								{ 'C',"Chat with the Sysop" },
								{ 'D',"Deep Dank Dungeon" },
								{ 'E',"Electronic Mail and Feedback" },
								{ 'F',"Feature Files Section" },
								{ 'G',"Gambling Casino" },
								{ 'L',"List of Top Users: Fame & Lame" },
								{ 'M',"Most Wanted List" },
								{ 'N',"Naval Adventures" },
								{ 'P',"Party/Gang Wars" },
								{ 'R',"Rob/Burglarize other users" },
								{ 'S',"Public Square (Shops, etc.)" },
								{ 'T',"Tiny's Tavern" },
								{ 'U',"User Configuration" },
								{ 'X',"X-terminate: Reroll character" },
								{ 'Y',"Your Statistics" },
								{ 'Z',"System Status/Border Change" }
							}
	};
	FILE *fp;
	DOUBLE v,y;
	int i,immortal,max,rob=0,t;
	char c;

	while(!cd()) {
		for(i=1; i<5; i++)
			memset(RPC[0][i],0,sizeof(struct RPC));
		for(i=0; i<5; i++)
			memset(RPC[1][i],0,sizeof(struct RPC));
		from=NULL;
		switch((c=option(&RPGmenu))) {
			case 'A':
				from='M';
				Arena();
				break;
			case 'B':
				Bulletins();
				break;
			case 'C':
				chat++;
				if(chat<3) {
					sprintf(outbuf,"%sPaging the SYSOP...",fore(YELLOW));
					OUT(outbuf);
					sound("chat",0);
				}
				else
				if(chat==3) {
					OUT("Your CHAT request has already been noted.");NL;
					OUT("Repeated requests are unnecessary and annoying.");
				}
				else
				if(chat==4) {
					OUT("FINAL WARNING: Another CHAT request will become a quick logoff.");
				}
				else {
					OUT("You had been warned, but you chose to ignore it...");
					strcpy(reason,"for chatting too much");
					logoff=TRUE;
				}
				NL;
				paws=!PLAYER.Expert;
				break;
			case 'D':
				if(dungeon) {
					if(ACCESS(acclvl)->RolePlay=='Y' && PLAYER.Novice=='Y' && dice(PLAYER.Level/2)==1) {
						if(ONLINE->HP<PLAYER.HP) {
							OUT("> You forgot to buy all your Hit Points.");NL;NL;
						}
						if(ONLINE->INT<50) {
							OUT("> Since your character has low Intellect, the auto-mapping feature is not on.");NL;NL;
						}
						if(PLAYER.Gold==0. && PLAYER.Bank>0.) {
							OUT("> Bring a small amount of money with you the next time.");NL;NL;
						}
						if(PLAYER.Gold>1. && PLAYER.Bank==0.) {
							OUT("> Taking all your money with you?  Good luck!");NL;NL;
						}
						OUT("> Try using your Cursor Arrow keys to move instead of N,S,E,W.");NL;NL;
						pause();
					}
					dungeon--;
					Dungeon(PLAYER.Level-1);
				}
				else {
					OUT("You have run out of dungeon turns.");NL;
					paws=!PLAYER.Expert;
				}
				break;
			case 'E':
				sprintf(prompt,"%sSend mail to (%s=Sysop): ",fore(CYN),SYSID);
				if(GetRPC(RPC[1][0])) {
					Editor(RPC[1][0],99);
					for(i=1; i<999; i++) {
						sprintf(outbuf,"%s.%03d",ENEMY.ID,i);
						strmfp(filename,EMAILpath,outbuf);
						if(!(fp=fopen(filename,"r")))
							break;
						fclose(fp);
					}
					WriteMail(filename);
				}
				break;
			case 'F':
				Files();
				break;
			case 'G':
				Casino();
				break;
			case 'L':
				Lists();
				break;
			case 'M':
				cls();
				for(immortal=ServerTable->NumClass-1; immortal>0 && stricmp("IMMORTAL",ServerTable->class[immortal]->Origin); immortal--);
				sprintf(outbuf,"%s%s ID   Player's Handle           Class    Lvl  Status  Party               %s",back(BLU),fore(WHITE),back(BLK));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s%s--------------------------------------------------------------------------%s",back(BLU),fore(GRY),back(BLK));
				OUT(outbuf);NL;
				strcpy(userlevel.Current,"999");
				i=2;
				while(RPGserver(SERVER_GETUSERLEVEL,(UBYTE *)&userlevel)) {
					i++;
					if(atoi(userlevel.Current)==1 && i==rows)
						break;
					if(i%rows==0)
						if((c=more())=='N')
							break;
					strcpy(ENEMY.ID,userlevel.ID);
					RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
					LoadRPC(RPC[1][0]);
					strsfn(RPC[1][0]->user.Class,NULL,NULL,line[0],line[1]);
					max=(stricmp(ServerTable->class[RPC[1][0]->class_origin]->Origin,"IMMORTAL")) ? FALSE : RPC[1][0]->class_type+1;
					sprintf(outbuf,"%s%-4.4s  %-22.22s  %-9.9s  %3u  %-6.6s  %s%.20s",
						strncmp(ENEMY.ID,PLAYER.ID,sizeof(PLAYER.ID)) ? fore(max==ServerTable->class[RPC[1][0]->class_origin]->NumCharacter ? YELLOW : GRY) : fore(WHITE),
						ENEMY.ID,ENEMY.Handle,
						line[1],ENEMY.Level,
						strcmp(ENEMY.Status,"jail") ? (strlen(ENEMY.Status) ? " Dead " : "Alive!") : " Jail ",
						strncmp(ENEMY.Gang,PLAYER.Gang,sizeof(PLAYER.Gang)) ? back(BLK) : ANSI>1 ? back(RED) : "\33[7m",
						strlen(ENEMY.Gang) ? ENEMY.Gang : "None");
					OUT(outbuf);
					NORMAL;NL;
				}
				if(c!='N')
					pause();
				break;
			case 'N':
				Naval();
				break;
			case 'P':
				Party();
				break;
			case 'Q':
				if(!cd()) {
					modem_clr();
					beep();
					sprintf(outbuf,"%sAre you sure (Y/N)? ",fore(YELLOW));
					OUT(outbuf);
					c=inkey('N','Y');
					NL;NL;
					if(c=='Y') {
						logoff=TRUE;
						strcpy(reason,"had something better to do");
					}
				}
				break;
			case 'R':
				if(ACCESS(acclvl)->RolePlay=='Y') {
					OUT("It is a hot, moonless night.");NL;
					OUT("A city guard walks down another street.");NL;NL;
					modf((value(WEAPON(ONLINE)->Value+ARMOR(ONLINE)->Value,ONLINE->CHA)+PLAYER.Gold+PLAYER.Bank-PLAYER.Loan)/5.,&y);
					sprintf(prompt,"%sWho are you going to rob? ",fore(CYN));
					if(GetRPC(RPC[1][0])) {
						if(!strcmp(PLAYER.ID,ENEMY.ID))
							break;
						if((BYTE)(PLAYER.Level-ENEMY.Level)>3) {
							OUT("You can only rob someone higher or up to three levels below you.");NL;
							paws=!PLAYER.Expert;
							break;
						}
						sprintf(outbuf,"You case %s's joint out.",ENEMY.Handle);
						OUT(outbuf);NL;
						modf((value(WEAPON(RPC[1][0])->Value+ARMOR(RPC[1][0])->Value+ENEMY.Gold,ONLINE->CHA))/5.,&v);
						v++;
						if(dice(100)<ONLINE->INT && v<y) {
							OUT("But you decide it is not worth the effort.");NL;
							paws=!PLAYER.Expert;
							break;
						}
						sprintf(outbuf,"The goods are in a %s protected by %s%s.",REALESTATE(ENEMY.RealEstate)->Name,AN(SECURITY(ENEMY.Security)->Name),SECURITY(ENEMY.Security)->Name);
						OUT(outbuf);NL;NL;
						sprintf(prompt,"%sAttempt to steal (Y/N)? ",fore(CYN));
						OUT(prompt);
						c=inkey('N','N');
						NL;NL;
						if(c=='Y') {
							OUT("You slide into the shadows and make your attempt...");
							Delay(150);
							NL;
							max=5*(ENEMY.Security+1)+ENEMY.RealEstate;
							rob++;
							if(ACCESS(acclvl)->Sysop!='Y')
								max+=abs(ENEMY.Level-PLAYER.Level)*rob;
							t=CLASS(ONLINE)->Steal>3 ? PLAYER.Level/10+1 : 0;
							for(i=0; i<(CLASS(ONLINE)->Steal<3 ? CLASS(ONLINE)->Steal : 3); i++)
								t+=dice(PLAYER.Level+1);
							if(t>max) {
								PLAYER.Gold+=v;
								PLAYER.History.Stole++;
								sprintf(outbuf,"You make it out with %s worth of stuff!",money(v,ANSI));
								OUT(outbuf);NL;
								sprintf(outbuf,"%srobbed %s",fore(CYN),ENEMY.Handle);
								news(outbuf);
								ENEMY.Gold=0.;
								if(RPC[1][0]->weapon_origin) {
									RPC[1][0]->weapon_origin=0;
									RPC[1][0]->weapon_type=dice(MAXWEAPON(0)/2)+dice(MAXWEAPON(0)/2)-1;
								}
								else
									if(RPC[1][0]->weapon_type)
										RPC[1][0]->weapon_type--;
								strmfe(ENEMY.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[RPC[1][0]->weapon_type]->Name);
								ENEMY.WCmod=0;
								if(RPC[1][0]->armor_origin) {
									RPC[1][0]->armor_origin=0;
									RPC[1][0]->armor_type=dice(MAXARMOR(0)/2)+dice(MAXARMOR(0)/2)-1;
								}
								else
									if(RPC[1][0]->armor_type)
										RPC[1][0]->armor_type--;
								strmfe(ENEMY.Armor,ServerTable->armor[0]->Origin,ServerTable->armor[0]->Item[RPC[1][0]->armor_type]->Name);
								ENEMY.ACmod=0;
								RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
								numline=0;
								sprintf(line[numline++],"%s robbed you!",PLAYER.Handle);
								note(ENEMY.ID);
							}
							else {
								sound("arrested",64);
								OUT("A guard catches you and throws you into jail!");NL;
								OUT("You might be released by your next call.");NL;NL;
								Delay(100);
								numline=0;
								sprintf(line[numline++],"%s was caught robbing you!",PLAYER.Handle);
								note(ENEMY.ID);
								sprintf(reason,"caught robbing %s",ENEMY.Handle);
								strcpy(PLAYER.Status,"jail");
								logoff=TRUE;
							}
						}
					}
				}
				break;
			case 'S':
				from='M';
				Square();
				break;
			case 'T':
				if(ACCESS(acclvl)->RolePlay=='Y' && tiny==3) {
					OUT("Tiny went to Butler Hospital.  The Tavern will re-open later");NL;
					paws=!PLAYER.Expert;
					break;
				}
				Tavern();
				break;
			case 'U':
				do {
					cls();
					sprintf(outbuf,"%s%s  Suppress Menus: %s%s",bracket(1),fore(CYN),fore(WHITE),PLAYER.Expert ? "ON" : "OFF");
					OUT(outbuf);NL;
					sprintf(outbuf,"%s%s  Emulation/Rows: %s%s/%u",bracket(2),fore(CYN),fore(WHITE),PLAYER.Emulation==1 ? "DEC-VT220" : PLAYER.Emulation==2 ? "IBM-ANSI" : PLAYER.Emulation==3 ? "Amiga-RPGterm" : "???",PLAYER.Rows);
					OUT(outbuf);NL;
					sprintf(outbuf,"%s%s  Logon Password: %s%s",bracket(3),fore(CYN),fore(WHITE),"???");
					OUT(outbuf);NL;NL;
					sprintf(prompt,"%sSelect (1-3): ",fore(CYN));
					OUT(prompt);
					if(ins(1))
						break;
					NL;NL;
					i=atoi(inbuf);
					switch(i) {
						case 1:
							PLAYER.Expert=PLAYER.Expert ? FALSE : TRUE;
							break;
						case 2:
							emulation();
							break;
						case 3:
							sprintf(prompt,"%sNew password: ",fore(CYN));
							OUT(prompt);
							echo=FALSE;
							ins(sizeof(PLAYER.Password));NL;
							if(strlen(inbuf)>2) {
								strupr(inbuf);
								strcpy(line[0],inbuf);
								sprintf(prompt,"%sRe-enter to verify: ",fore(CYN));
								OUT(prompt);
								echo=FALSE;
								ins(sizeof(PLAYER.Password));NL;
								if(!stricmp(line[0],inbuf)) {
									strncpy(PLAYER.Password,inbuf,sizeof(PLAYER.Password));
									OUT("Ok.");NL;
									Delay(50);
								}
								else
									beep();
							}
							echo=TRUE;
							break;
					}
				} while(i);
				break;
			case 'X':
				if(ACCESS(acclvl)->RolePlay=='Y') {
					modem_clr();
					beep();
					sprintf(outbuf,"%sAre you sure (Y/N)? ",fore(YELLOW));
					OUT(outbuf);
					c=inkey('N','Y');
					NL;NL;
					if(c=='Y') {
						reroll(ONLINE);
						assign(200);
						PLAYER.Coward=TRUE;
					}
				}
				break;
			case 'Y':
				charstats(RPC[1][0]);
				break;
			case 'Z':
				type("Notices/System",TRUE);
				paws=!PLAYER.Expert;
				break;
			default:
				beep();
				break;
		}
	}
}
