#include "/include/all.h"
#include "global.h"

void ListGangMembers(struct gang *g1,struct gang *g2,int loaded)
{
	static UBYTE le[] = { '>','<','(',')','+','*' };
	static UBYTE re[] = { '<','>',')','(','+','*' };
	static UBYTE tb[] = { '-','=','~',':','+','*' };
	static UBYTE mp[] = { '@','#','$','&' };
	int i;
	UBYTE centered[21];

	sprintf(outbuf,"%s%c",fore(WHITE),mp[g1->BannerType/64]);
	OUT(outbuf);
	if(g2) {
		sprintf(outbuf,"                           %s%c",fore(WHITE),mp[g2->BannerType/64]);
		OUT(outbuf);
	}
	NL;
	sprintf(outbuf,"%s|%s%s%c",fore(GRY),fore(g1->BannerTrim),back(g1->BannerBack),le[g1->BannerType%6]);
	OUT(outbuf);
	memset(outbuf,tb[g1->BannerType/8%6],22);
	sprintf(&outbuf[22],"%c%s",re[g1->BannerType%6],back(BLK));
	OUT(outbuf);
	if(g2) {
		sprintf(outbuf,"   %s|%s%s%c",fore(GRY),fore(g2->BannerTrim),back(g2->BannerBack),le[g2->BannerType%6]);
		OUT(outbuf);
		memset(outbuf,tb[g2->BannerType/8%6],22);
		sprintf(&outbuf[22],"%c%s",re[g2->BannerType%6],back(BLK));
		OUT(outbuf);
	}
	NL;
	sprintf(centered,"%.20s",g1->Name);
	i=(20-strlen(centered))/2;
	sprintf(centered,"%*s%.20s",i,"",g1->Name);
	sprintf(outbuf,"%s|%s%s%c%s %-20s %s%c%s",fore(GRY),fore(g1->BannerTrim),back(g1->BannerBack),le[g1->BannerType%6],fore(g1->BannerFore),centered,fore(g1->BannerTrim),re[g1->BannerType%6],back(BLK));
	OUT(outbuf);
	if(g2) {
		sprintf(centered,"%.20s",g2->Name);
		i=(20-strlen(centered))/2;
		sprintf(centered,"%*s%.20s",i,"",g2->Name);
		sprintf(outbuf,"   %s|%s%s%c%s %-20s %s%c%s",fore(GRY),fore(g2->BannerTrim),back(g2->BannerBack),le[g2->BannerType%6],fore(g2->BannerFore),centered,fore(g2->BannerTrim),re[g2->BannerType%6],back(BLK));
		OUT(outbuf);
	}
	NL;
	sprintf(outbuf,"%s|%s%s%c",fore(GRY),fore(g1->BannerTrim),back(g1->BannerBack),le[g1->BannerType%6]);
	OUT(outbuf);
	memset(outbuf,tb[g1->BannerType/8%6],22);
	sprintf(&outbuf[22],"%c%s",re[g1->BannerType%6],back(BLK));
	OUT(outbuf);
	if(g2) {
		sprintf(outbuf,"   %s|%s%s%c",fore(GRY),fore(g2->BannerTrim),back(g2->BannerBack),le[g2->BannerType%6]);
		OUT(outbuf);
		memset(outbuf,tb[g2->BannerType/8%6],22);
		sprintf(&outbuf[22],"%c%s",re[g2->BannerType%6],back(BLK));
		OUT(outbuf);
	}
	NL;
	for(i=0; i<4; i++) {
		if(loaded) {
			sprintf(outbuf,"%s|%s %-22s",fore(GRY),fore(WHITE),strlen(GANG(0,i).Handle) ?  GANG(0,i).Handle : "");
			OUT(outbuf);
			sprintf(outbuf,"    %s|%s %-22s",fore(GRY),fore(WHITE),strlen(GANG(1,i).Handle) ? GANG(1,i).Handle : "");
			OUT(outbuf);
		}
		else {
			strcpy(ENEMY.ID,g1->MemberID[i]);
			if(g1->MemberID[i][0]=='_') {
				memset(&ENEMY,0,sizeof(struct user));
				sprintf(ENEMY.Handle,"Monster #%d",i+1);
				strncpy(ENEMY.Gang,g1->Name,20);
			}
			else
				RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
			sprintf(outbuf,"%s|%s %-22s",fore(GRY),fore(strlen(ENEMY.Handle) ? (strlen(ENEMY.Status) ? BLACK : WHITE) : GRY),strlen(ENEMY.Handle) ? (strncmp(g1->Name,ENEMY.Gang,20) ? "-reserved-" : ENEMY.Handle) : "-empty-");
			OUT(outbuf);
			if(g2) {
				strcpy(ENEMY.ID,g2->MemberID[i]);
				if(g2->MemberID[i][0]=='_') {
					memset(&ENEMY,0,sizeof(struct user));
					sprintf(ENEMY.Handle,"Monster #%d",i+1);
					strncpy(ENEMY.Gang,g2->Name,20);
				}
				else
					RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
				sprintf(outbuf,"    %s|%s %-22s",fore(GRY),fore(strlen(ENEMY.Handle) ? (strlen(ENEMY.Status) ? BLACK : WHITE) : GRY),strlen(ENEMY.Handle) ? (strncmp(g2->Name,ENEMY.Gang,20) ? "-reserved-" : ENEMY.Handle) : "-empty-");
				OUT(outbuf);
			}
		}
		NL;
	}
	NL;
}

void Party(void)
{
	static struct RPGmenu __far RPGmenu = { "Party",MAG,8,
							{	{ 'E',"Edit your gang" },
								{ 'F',"Fight another gang" },
								{ 'J',"Join a gang" },
								{ 'L',"List all gangs" },
								{ 'M',"Most Wanted list" },
								{ 'R',"Resign your membership" },
								{ 'S',"Start a new gang" },
								{ 'T',"Transfer leadership" }
							}
	};
	double divisor[2],specie,expaward,totgold[2];
	int i,j,l,totlvl[2],w;
	char c;

	from='P';
	while((c=option(&RPGmenu))!='Q') {
		for(i=1; i<5; i++)
			memset(RPC[0][i],0,sizeof(struct RPC));
		for(i=0; i<5; i++)
			memset(RPC[1][i],0,sizeof(struct RPC));
		switch(c) {
			case 'E':
				sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
				if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
					for(i=0; i<4 && strcmp(PLAYER.ID,gm[0].MemberID[i]); i++);
					if(i) {
						OUT("Only the gang leader may edit members.");NL;
						paws=!PLAYER.Expert;
						break;
					}
					ListGangMembers(&gm[0],NULL,FALSE);
					sprintf(prompt,"%sReplace member (2-4): ",fore(CYN));
					OUT(prompt);
					if(ins(4))
						break;
					if((i=atoi(inbuf)-1))
						if(i>0 && i<4)
							strcpy(inbuf,gm[0].MemberID[i]);
					if(strlen(inbuf))
						for(i=1; i<4 && stricmp(gm[0].MemberID[i],inbuf); i++);
					if(i>0 && i<4) {
						sprintf(prompt," %swith (*=anybody): ",fore(CYN));
						OUT(prompt);
						if(ins(4))
							break;
						NL;
						if(strlen(inbuf)) {
							strcpy(ENEMY.ID,gm[0].MemberID[i]);
							if(RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY)) {
								memset(ENEMY.Gang,0,20);
								RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
								memset(gm[0].MemberID[i],0,sizeof(ENEMY.ID));
							}
							strcpy(ENEMY.ID,inbuf);
							if(RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY)) {
								for(j=0; j<4 && strcmp(gm[0].MemberID[j],ENEMY.ID); j++);
								if(j==4) {
									strcpy(gm[0].MemberID[i],ENEMY.ID);
									numline=0;
									sprintf(line[numline++],"%s has invited you to join %s.",PLAYER.Handle,PLAYER.Gang);
									note(ENEMY.ID);
								}
								else {
									OUT("Who are you trying to fool?");NL;
									paws=!PLAYER.Expert;
								}
							}
							RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
						}
					}
				}
				else {
					memset(PLAYER.Gang,0,20);
					OUT("You are not a member of any gang.");NL;
					paws=!PLAYER.Expert;
				}
				break;
			case 'F':
				sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
				if(!RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
					memset(PLAYER.Gang,0,20);
					OUT("You are not a member of any gang.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(!party) {
					OUT("You have already used your gang fight this call.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				memset(&gm[1],0,sizeof(struct gang));
				i=1;
				while(RPGserver(SERVER_NEXTGANG,(UBYTE *)&gm[1])) {
					if(strncmp(gm[0].Name,gm[1].Name,20)) {
						if(i%(rows-1)==0) {
							if((c=more())=='N')
								break;
							i=1;
						}
						sprintf(outbuf,"%s%s %.20s",bracket(i),fore(GRY),gm[1].Name);
						OUT(outbuf);NL;
						sprintf(line[i++],"%.20s",gm[1].Name);
					}
				}
				NL;
				sprintf(prompt,"%sWhich gang do you want to fight: ",fore(CYN));
				OUT(prompt);
				if(ins(3))
					break;
				NL;
				j=atoi(inbuf);
				if(j<1 || j>=i)
					break;
				sprintf(gm[1].Name,"%.20s",line[j]);
				if(!RPGserver(SERVER_GETGANG,(UBYTE *)&gm[1]))
					break;
				OUT("Loading both gangs...");
				j=1;
				divisor[0]=pow(2.5,PLAYER.ExpLevel/3.);
				totgold[0]=PLAYER.Gold;
				totlvl[0]=PLAYER.ExpLevel;
				for(i=0; i<4; i++)
					if(strlen(gm[0].MemberID[i]) && strcmp(gm[0].MemberID[i],PLAYER.ID)) {
						strcpy(GANG(0,j).ID,gm[0].MemberID[i]);
						if(RPGserver(SERVER_GETUSER,(UBYTE *)&GANG(0,j))) {
							if(!strncmp(gm[0].Name,GANG(0,j).Gang,sizeof(GANG(0,j).Gang)) && !strlen(GANG(0,j).Status) && RPGserver(SERVER_LOCK,GANG(0,j).ID)) {
								LoadRPC(RPC[0][j]);
								divisor[0]+=pow(2.5,GANG(0,j).ExpLevel/3.);
								totgold[0]+=GANG(0,j).Gold;
								totlvl[0]+=GANG(0,j).ExpLevel;
								j++;
							}
							else
								memset(RPC[0][j],0,sizeof(struct RPC));
						}
					}
				NL;
				j=0;
				divisor[1]=0.;
				totgold[1]=0.;
				totlvl[1]=0;
				for(i=0; i<4; i++)
					if(strlen(gm[1].MemberID[i])) {
						strcpy(GANG(1,j).ID,gm[1].MemberID[i]);
						if(GANG(1,j).ID[0]=='_') {
							l=GANG(0,j).Level ? GANG(0,j).Level+dice(6)-3 : dice(99);
							l=(l<0) ? 0 : l;
							l=l*NUMDUNGEON/100+dice(3)-2;
							l=(l<0) ? 0 : (l>=NUMDUNGEON) ? NUMDUNGEON-1 : l;
							strcpy(GANG(1,j).Handle,DUNGEON(l)->Name);
							proper(GANG(1,j).Handle);
							strcpy(GANG(1,j).Class,DUNGEON(l)->Class);
							GANG(1,j).Spell=DUNGEON(l)->Spell;
							GANG(1,j).XSpell=DUNGEON(l)->XSpell;
							GANG(1,j).Gender='I';
							GANG(1,j).Level=l+1;
							modf(pow2(l/2.)*dice(10),&GANG(1,j).Gold);
							CreateRPC(RPC[1][j]);
							if(!RPC[1][j]->class_origin && !RPC[1][j]->class_type) {
								w=dice(NUMCLASS)-1;
								strmfe(GANG(1,j).Class,ServerTable->class[w]->Origin,ServerTable->class[w]->Character[dice(MAXCLASS(w))-1]->Name);
								CreateRPC(RPC[1][j]);
							}
							for(w=0; w<NUMMAGIC; w++)
								if(dice(ONLINE->CHA/2+5*w)==1) {
									if(w<16)
										GANG(1,j).Spell|=(UWORD)pow2((double)w);
									if(w>15 && w<24)
										GANG(1,j).XSpell|=(UBYTE)pow2((double)w-16);
								}
							for(w=0; w<NUMPOISON; w++)
								if(dice(ONLINE->CHA/(CLASS(RPC[1][j])->Poison+w+1))==1)
									GANG(1,j).Poison|=(UWORD)pow2((double)w);
							divisor[1]+=pow(2.5,GANG(1,j).ExpLevel/3.);
							totgold[1]+=GANG(1,j).Gold;
							totlvl[1]+=GANG(1,j).ExpLevel;
							j++;
						}
						else
						if(RPGserver(SERVER_GETUSER,(UBYTE *)&GANG(1,j))) {
							if(!strncmp(gm[1].Name,GANG(1,j).Gang,sizeof(GANG(1,j).Gang)) && !strlen(GANG(1,j).Status) && RPGserver(SERVER_LOCK,GANG(1,j).ID)) {
								LoadRPC(RPC[1][j]);
								divisor[1]+=pow(2.5,GANG(1,j).ExpLevel/3.);
								totgold[1]+=GANG(1,j).Gold;
								totlvl[1]+=GANG(1,j).ExpLevel;
								j++;
							}
							else
								memset(RPC[1][j],0,sizeof(struct RPC));
						}
					}
				displayview();
				NL;
				if(!j) {
					OUT("That gang is not active!");NL;
					paws=!PLAYER.Expert;
					break;
				}
				ListGangMembers(&gm[0],&gm[1],TRUE);
				sprintf(prompt,"%sContinue (Y/N)? ",fore(CYN));
				OUT(prompt);
				c=inkey('N','N');
				NL;NL;
				if(c!='Y')
					break;
				party--;
				sprintf(outbuf,"%s grins as %s pulls out %s %s.",ENEMY.Handle,RPC[1][0]->he,RPC[1][0]->his,WEAPON(RPC[1][0])->Name);
				OUT(outbuf);NL;NL;
				Delay(100);
				Battle();
				w=0;l=1;
				for(i=0; i<4; i++)
					if(GANG(1,i).HP>0 && RPC[1][i]->HP>0) {
						w=1;l=0;
						for(j=0; j<4; j++)
							if(GANG(0,j).HP>0 && RPC[0][j]->HP>0) {
								w=-1;l=-1;
								break;
							}
						break;
					}
				if(w<0 || l<0) {	//  forfeit
					gm[0].Loss++;
					RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
					gm[1].Win++;
					RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[1]);
					break;
				}
				if(w==1) {
					OUT("Your gang has been annihilated!");
					logoff=TRUE;
					sprintf(reason,"defeated by %.20s",gm[1].Name);
				}
				if(w==0) {
					sprintf(outbuf,"%.20s have been annihilated!",gm[l].Name);
					OUT(outbuf);
				}
				NL;NL;
				for(i=0; i<2; i++)
					for(j=0; j<4; j++)
						if(GANG(i,j).HP>0 && RPC[i][j]->HP==-1) {
							divisor[i]+=pow(2.5,GANG(i,j).ExpLevel/3.);
							totgold[i]-=GANG(i,j).Gold;
							totlvl[i]-=GANG(i,j).ExpLevel;
						}
				for(i=0; i<4; i++) {
					numline=0;
					sprintf(line[numline++],"%.20s defeated %.20s, initiated by %s.",gm[w].Name,gm[l].Name,PLAYER.Handle);
					if(strlen(GANG(w,i).ID) && GANG(w,i).ID[0]!='_' && RPC[w][i]->HP>-1) {
						modf(pow2((GANG(w,i).Level-2.))*1000.*totlvl[l]/totlvl[w],&expaward);
						modf(totgold[l]*pow(2.5,GANG(w,i).ExpLevel/3.)/divisor[w],&specie);
						GANG(w,i).Experience+=expaward;
						if(w || i) {
							if(GANG(w,i).Loan>0.) {
								GANG(w,i).Loan-=specie;
								if(GANG(w,i).Loan<0.) {
									GANG(w,i).Gold-=GANG(w,i).Loan;
									GANG(w,i).Loan=0.;
								}
							}
						}
						else
							GANG(w,i).Gold+=specie;
						RPGserver(SERVER_PUTUSER,(UBYTE *)&GANG(w,i));
						sprintf(outbuf,"You got %.8g experience and %s.",expaward,money(specie,GANG(w,i).Emulation));
						if(w || i) {
							strcpy(line[numline++],outbuf);
							note(GANG(w,i).ID);
						}
						else {
							OUT(outbuf);NL;
						}
					}
					if(strlen(GANG(l,i).ID) && GANG(l,i).ID[0]!='_') {
						numline=0;
						sprintf(line[numline++],"%.20s were defeated by %.20s, initiated by %s.",gm[l].Name,gm[w].Name,PLAYER.Handle);
						note(GANG(l,i).ID);
						if(GANG(l,i).HP>0 && RPC[l][i]->HP==0) {
							GANG(l,i).Gold=0.;
							RPGserver(SERVER_PUTUSER,(UBYTE *)&GANG(l,i));
						}
					}
				}
				gm[w].Win++;
				RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[w]);
				gm[l].Loss++;
				RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[l]);
				if(l) {
					sprintf(outbuf,"%sdefeated the gang, %s%.20s",fore(CYN),fore(WHITE),gm[l].Name);
					news(outbuf);
				}
				break;
			case 'J':
				if(ACCESS(acclvl)->RolePlay!='Y')
					break;
				if(strlen(PLAYER.Gang)) {
					sprintf(outbuf,"You are already a member of %s.",PLAYER.Gang);
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
					break;
				}
				memset(&gm[0],0,sizeof(struct gang));
				while(RPGserver(SERVER_NEXTGANG,(UBYTE *)&gm[0])) {
					j=0;
					for(i=1; i<4; i++) {
						if(!strlen(gm[0].MemberID[i]) && !j)
							j=i;
						if(!strcmp(PLAYER.ID,gm[0].MemberID[i])) {
							j=i;
							break;
						}
					}
					if(j) {
						ListGangMembers(&gm[0],NULL,FALSE);
						sprintf(prompt,"%sJoin (Y/N)? ",fore(CYN));
						OUT(prompt);
						c=inkey('N','N');
						NL;NL;
						if(c=='Y') {
							strncpy(PLAYER.Gang,gm[0].Name,20);
							RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
							strcpy(gm[0].MemberID[j],PLAYER.ID);
							RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
							sprintf(outbuf,"You are now a member of %s.",PLAYER.Gang);
							OUT(outbuf);NL;
							paws=!PLAYER.Expert;
							break;
						}
					}
				}
				break;
			case 'L':
				cls();
				c='Y';i=1;
				memset(&gm[0],0,sizeof(struct gang));
				while(RPGserver(SERVER_NEXTGANG,(UBYTE *)&gm[0]) && c=='Y') {
					memcpy(&gm[1],&gm[0],sizeof(struct gang));
					if(RPGserver(SERVER_NEXTGANG,(UBYTE *)&gm[1])) {
						ListGangMembers(&gm[0],&gm[1],FALSE);
						memcpy(&gm[0],&gm[1],sizeof(struct gang));
					}
					else {
						ListGangMembers(&gm[0],NULL,FALSE);
						break;
					}
					if((i^=1))
						c=more();
				}
				if(c=='Y' && !PLAYER.Expert)
					paws=!PLAYER.Expert;
				break;
			case 'M':
				cls();
				sprintf(outbuf,"%s%s       Party            Win-Loss   Ratio%s",back(BLU),fore(WHITE),back(BLK));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s%s----------------------------------------%s",back(BLU),fore(GRY),back(BLK));
				OUT(outbuf);NL;
				i=0;
				memset(&gm[0],0,sizeof(struct gang));
				while(RPGserver(SERVER_NEXTGANG,(UBYTE *)&gm[0]) && i<100) {
					memcpy(&gm[2],&gm[0],sizeof(struct gang));
					sprintf(line[i],"%.20s",gm[0].Name);
					j=i;
					while(j>0) {
						memcpy(&gm[1].Name,line[j-1],20);
						RPGserver(SERVER_GETGANG,(UBYTE *)&gm[1]);
						if(gm[0].Win<gm[1].Win)
							break;
						if(gm[0].Win==gm[1].Win && gm[0].Loss<=gm[1].Loss)
							break;
						swmem(line[j],line[j-1],20);
						j--;
					}
					memcpy(&gm[0],&gm[2],sizeof(struct gang));
					i++;
				}
				l=2;
				for(j=0; j<i; j++) {
					if(++l%rows==0)
						if((c=more())=='N')
							break;
					sprintf(gm[0].Name,"%.20s",line[j]);
					RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0]);
					d=1.;
					if(gm[0].Win || gm[0].Loss)
						d=(double)gm[0].Win/(gm[0].Win+gm[0].Loss);
					sprintf(inbuf,"%5.3f",d);
					sprintf(outbuf,"%-20.20s  %5u-%-5u  %5s",gm[0].Name,gm[0].Win,gm[0].Loss,(*inbuf=='0' ? &inbuf[1] : inbuf));
					OUT(outbuf);NL;
				}
				if(c!='N')
					paws=!PLAYER.Expert;
				break;
			case 'R':
				if(!strlen(PLAYER.Gang)) {
					OUT("You are not a member of any gang.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(!party) {
					OUT("You have already used your gang fight this call -- you cannot resign.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				sprintf(prompt,"%sReally resign from %s (Y/N)? ",fore(CYN),PLAYER.Gang);
				OUT(prompt);
				c=inkey('N','N');
				NL;
				if(c!='Y')
					break;
				sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
				if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
					for(i=0; i<4 && strcmp(PLAYER.ID,gm[0].MemberID[i]); i++);
					if(i) {
						memset(gm[0].MemberID[i],0,sizeof(PLAYER.ID));
						RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
						sprintf(outbuf,"You are no longer a member of %.20s.",gm[0].Name);
						OUT(outbuf);NL;
						paws=!PLAYER.Expert;
					}
					else {
						sprintf(outbuf,"%sDissolving %s...",fore(YELLOW),PLAYER.Gang);
						OUT(outbuf);
						paws=!PLAYER.Expert;
						for(i=1; i<4; i++)
							if(strlen(gm[0].MemberID[i])) {
								strcpy(ENEMY.ID,gm[0].MemberID[i]);
								if(RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY))
									if(!strncmp(ENEMY.Gang,gm[0].Name,20)) {
										memset(ENEMY.Gang,0,20);
										RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
									}
							}
						RPGserver(SERVER_DELGANG,(UBYTE *)&gm[0]);
						NL;
					}
				}
				memset(PLAYER.Gang,0,sizeof(PLAYER.Gang));
				RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
				break;
			case 'S':
				if(ACCESS(acclvl)->RolePlay!='Y')
					break;
				if(strlen(PLAYER.Gang)) {
					sprintf(outbuf,"You are already a member of %s.",PLAYER.Gang);
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
					break;
				}
				sprintf(prompt,"%sWhat is the name of your gang? ",fore(CYN));
				OUT(prompt);
				if(ins(20))
					break;
				NL;NL;
				proper(inbuf);
				if(!strlen(inbuf) || !stricmp(inbuf,"None"))
					break;
				strcpy(gm[0].Name,inbuf);
				if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
					OUT("That gang already exists!");NL;
					paws=!PLAYER.Expert;
					break;
				}
				sprintf(PLAYER.Gang,"%.20s",inbuf);
				RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
				sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
				strcpy(gm[0].MemberID[0],PLAYER.ID);
				do {
					gm[0].BannerType=dice(256)-1;
					gm[0].BannerFore=8+dice(8)-1;
					gm[0].BannerBack=dice(8)-1;
					gm[0].BannerTrim=dice(8)-1;
					cls();
					ListGangMembers(&gm[0],NULL,FALSE);
					sprintf(prompt,"%sAccept this banner (Y/N)? ",fore(CYN));
					OUT(prompt);
				} while((c=inkey('N','Y'))=='N');
				RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
				NL;NL;
				OUT("By default, anyone can now join your gang.  You can have up to 4 gang members in");NL;
				OUT("a party.  You can edit your gang to invite specific players.  In turn, those");NL;
				OUT("players you invite must then join your gang to become active members.");NL;
				if(!PLAYER.Expert)
					pause();
				break;
			case 'T':
				sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
				if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
					for(i=0; i<4 && strcmp(PLAYER.ID,gm[0].MemberID[i]); i++);
					if(i) {
						OUT("Only the gang leader may transfer leadership.");NL;
						paws=!PLAYER.Expert;
						break;
					}
					ListGangMembers(&gm[0],NULL,FALSE);
					sprintf(prompt,"%sTransfer to: ",fore(CYN));
					OUT(prompt);
					if(ins(4))
						break;
					if((i=atoi(inbuf)-1))
						if(i>0 && i<4)
							strcpy(inbuf,gm[0].MemberID[i]);
					if(strlen(inbuf))
						for(i=1; i<4 && stricmp(gm[0].MemberID[i],inbuf); i++);
					if(i>0 && i<4) {
						strcpy(ENEMY.ID,gm[0].MemberID[i]);
						if(RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY)) {
							if(!strncmp(ENEMY.Gang,gm[0].Name,20)) {
								swmem(gm[0].MemberID[0],gm[0].MemberID[i],sizeof(PLAYER.ID));
								RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
								sprintf(outbuf,"%s is now the leader of %.20s",ENEMY.Handle,gm[0].Name);
								OUT(outbuf);NL;
								paws=!PLAYER.Expert;
							}
						}
					}
				}
				else {
					OUT("You are not a member of any gang.");NL;
					paws=!PLAYER.Expert;
					memset(PLAYER.Gang,0,sizeof(PLAYER.Gang));
				}
				break;
		}
	}
}
