#include "/include/all.h"
#include "global.h"

void Arena(void)
{
	static struct RPGmenu __far RPGmenu = { "Arena",RED,7,
							{	{ 'U',"User fights" },
								{ 'M',"Monster fights" },
								{ 'J',"Joust users" },
								{ 'C',"Cast a spell" },
								{ 'P',"Poison your weapon" },
								{ 'G',"Goto the square" },
								{ 'Y',"Your status" }
							}
	};

	int ability[2],i,jl,jw,n,result;
	char c;

	if(ACCESS(acclvl)->RolePlay=='Y' && PLAYER.Novice=='Y' && dice(PLAYER.Level/2)==1) {
		if(ONLINE->HP<PLAYER.HP) {
			OUT("> Buy Hit Points!");NL;NL;
			paws=!PLAYER.Expert;
		}
		if(joust) {
			OUT("> Try jousting another player to win money.");NL;NL;
			paws=!PLAYER.Expert;
		}
		if(PLAYER.Poison && !ONLINE->ToWC) {
			OUT("> Don't forget to poison your weapon.");NL;NL;
			paws=!PLAYER.Expert;
		}
		if(PLAYER.Gold>0.) {
			OUT("> Carrying money around here is not a good idea.  Spend it in the Square");NL;
			OUT("  or deposit it in the Bank for safer keeping.");NL;NL;
			paws=!PLAYER.Expert;
		}
	}

	while((c=option(&RPGmenu))!='Q') {
		for(i=1; i<5; i++)
			memset(RPC[0][i],0,sizeof(struct RPC));
		for(i=0; i<5; i++)
			memset(RPC[1][i],0,sizeof(struct RPC));
		switch(c) {
			case 'U':
				if(!arena) {
					OUT("You have no more arena fights.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				while(!strlen(ENEMY.ID)) {
					sprintf(prompt,"%sFight what user (?=list): ",fore(CYN));
					if(!GetRPC(RPC[1][0]))
						break;
					if(!strcmp(PLAYER.ID,ENEMY.ID)) {
						sprintf(outbuf,"You can't fight a wimp like %s.",ONLINE->him);
						OUT(outbuf);NL;NL;
						memset(RPC[1][0],0,sizeof(struct RPC));
						continue;
					}
					if((BYTE)(PLAYER.Level-ENEMY.Level)>3) {
						OUT("You can only attack someone higher or up to three levels below you.");NL;NL;
						memset(RPC[1][0],0,sizeof(struct RPC));
						continue;
					}
					if(strlen(ENEMY.Status)) {
						sprintf(outbuf,"%s ",ENEMY.Handle);
						if(strcmp(ENEMY.Status,"jail")) {
							i=ENEMY.ExpLevel;
							strcpy(ENEMY.ID,ENEMY.Status);
							if((RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY))) {
								sprintf(inbuf,"was killed by %s %s(%s%u%s)%s",ENEMY.Handle,fore(CYN),fore(WHITE),i,fore(CYN),fore(GRY));
								strcat(outbuf,inbuf);
							}
							else
								strcat(outbuf,"is not available to fight you");
						}
						else
							strcat(outbuf,"is locked-up in jail");
						OUT(outbuf);NL;NL;
						memset(RPC[1][0],0,sizeof(struct RPC));
						continue;
					}
					strmfp(filename,"PIX/Player",ENEMY.ID);
					if(!type_pix(filename,ENEMY.Gender)) {
						strmfp(filename,"PIX/Player",CLASS(RPC[1][0])->Name);
						type_pix(filename,ENEMY.Gender);
					}
					sprintf(outbuf,"%s is a level %u %s.",RPC[1][0]->He,ENEMY.ExpLevel,CLASS(RPC[1][0])->Name);
					OUT(outbuf);NL;NL;
					if(!RPC[1][0]->weapon_origin) {
						sprintf(outbuf,"%s is carrying %s%s.",RPC[1][0]->He,AN(WEAPON(RPC[1][0])->Name),WEAPON(RPC[1][0])->Name);
						OUT(outbuf);NL;NL;
					}
					if(!RPC[1][0]->armor_origin) {
						sprintf(outbuf,"%s is wearing %s%s.",RPC[1][0]->He,AN(ARMOR(RPC[1][0])->Name),ARMOR(RPC[1][0])->Name);
						OUT(outbuf);NL;NL;
					}
					displayview();
					sprintf(prompt,"%sWill you fight %s (Y/N)? ",fore(CYN),RPC[1][0]->him);
					OUT(prompt);
					c=inkey('N','N');
					NL;
					if(c=='Y') {
						NL;
						arena--;
						Battle();
						if(!RPC[1][0]->HP) {
							sprintf(line[numline++],"%s killed you!",PLAYER.Handle);
							strcpy(ENEMY.Status,PLAYER.ID);
							PLAYER.Coward=FALSE;
							modf(EXP(ENEMY.ExpLevel-1)/3.,&d);
							PLAYER.Experience+=d;
							if(PLAYER.Level<ENEMY.ExpLevel)
								ENEMY.ExpLevel=PLAYER.Level;
							sprintf(outbuf,"You get %.8g experience.",d);
							OUT(outbuf);NL;NL;
							if(ENEMY.Gold>0.) {
								sprintf(line[numline++],"%s took %s you were carrying.",HE(ONLINE),money(ENEMY.Gold,ENEMY.Emulation));
								PLAYER.Gold+=ENEMY.Gold;
								sprintf(outbuf,"You get %s %s was carrying.",money(ENEMY.Gold,ANSI),RPC[1][0]->he);
								OUT(outbuf);NL;
								ENEMY.Gold=0.;
							}
							if(ENEMY.Bounty>0.) {
								sprintf(line[numline++],"%s collected a %s bounty from %s.",HE(ONLINE),money(ENEMY.Bounty,ENEMY.Emulation),ENEMY.BountyWho);
								PLAYER.Gold+=ENEMY.Bounty;
								sprintf(outbuf,"You collect a %s bounty from %s.",money(ENEMY.Bounty,ANSI),ENEMY.BountyWho);
								OUT(outbuf);NL;
							}
							ENEMY.Bounty=0.;
							memset(ENEMY.BountyWho,0,sizeof(ENEMY.BountyWho));
							ExchangeWeapon(ONLINE,RPC[1][0]);
							ExchangeArmor(ONLINE,RPC[1][0]);
							if(strlen(PLAYER.Cursed)) {
								memset(PLAYER.Cursed,0,sizeof(PLAYER.Cursed));
								if((ONLINE->STR+=10)>PLAYER.STR)
									ONLINE->STR=PLAYER.STR;
								if((ONLINE->INT+=10)>PLAYER.INT)
									ONLINE->INT=PLAYER.INT;
								if((ONLINE->DEX+=10)>PLAYER.DEX)
									ONLINE->DEX=PLAYER.DEX;
								if((ONLINE->CHA+=10)>PLAYER.CHA)
									ONLINE->CHA=PLAYER.CHA;
								sprintf(outbuf,"%sYou give %s your %sblack%s cloud.",fore(GRY),RPC[1][0]->him,fore(BLACK),fore(GRY));
								OUT(outbuf);NL;
								strcpy(ENEMY.Cursed,PLAYER.ID);
								strcpy(line[numline++],"A black cloud hovers over you.");
							}
							if(strlen(ENEMY.Blessed)) {
								strcpy(PLAYER.Blessed,ENEMY.ID);
								if((ONLINE->STR+=10)>100)
									ONLINE->STR=100;
								if((ONLINE->INT+=10)>100)
									ONLINE->INT=100;
								if((ONLINE->DEX+=10)>100)
									ONLINE->DEX=100;
								if((ONLINE->CHA+=10)>100)
									ONLINE->CHA=100;
								sprintf(outbuf,"%sYou feel a shining aura surround you.%s",fore(YELLOW),fore(GRY));
								OUT(outbuf);NL;
								memset(ENEMY.Blessed,0,sizeof(ENEMY.Blessed));
								strcpy(line[numline++],"You lost your shining aura.");
							}
							if(strlen(PLAYER.Gang) && !strnicmp(PLAYER.Gang,ENEMY.Gang,20)) {
								sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
								if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
									if(!strcmp(ENEMY.ID,gm[0].MemberID[0])) {
										for(i=0; i<4 && strcmp(PLAYER.ID,gm[0].MemberID[i]); i++);
										if(i<4) {
											sprintf(outbuf,"You take over as leader of %.20s.",PLAYER.Gang);
											OUT(outbuf);NL;NL;
											swmem(gm[0].MemberID[0],gm[0].MemberID[i],sizeof(gm[0].MemberID[0]));
											RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
											sprintf(line[numline++],"You lost your leadership to %.20s.",ENEMY.Gang);
										}
									}
								}
							}
							sprintf(prompt,"%sDo you want to leave %s a comment (Y/N)? ",fore(CYN),RPC[1][0]->him);
							OUT(prompt);
							if((c=inkey('N','N'))=='Y') {
								NL;
								sprintf(prompt,"%s>",fore(CYN));
								OUT(prompt);
								ins(78);
								jazz(inbuf);
								sprintf(line[numline++],"%s says,\"%s\"",HE(ONLINE),inbuf);
							}
							NORMAL;NL;
							sprintf(outbuf,"%sdefeated %s, a level %s%u %s",fore(CYN),ENEMY.Handle,fore(WHITE),ENEMY.Level,CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						if(!ONLINE->HP) {
							sprintf(line[numline++],"You killed %s!",PLAYER.Handle);
							modf(EXP(PLAYER.ExpLevel-1)/3.,&d);
							ENEMY.Experience+=d;
							if(PLAYER.Gold>0.) {
								sprintf(line[numline++],"You took %s %s was carrying.",money(ENEMY.Gold,ENEMY.Emulation),HE(ONLINE));
								if(ENEMY.Loan>0.) {
									ENEMY.Loan-=PLAYER.Gold;
									if(ENEMY.Loan<0.) {
										ENEMY.Gold-=ENEMY.Loan;
										ENEMY.Loan=0.;
									}
								}
								else
									ENEMY.Gold+=PLAYER.Gold;
								sprintf(outbuf,"%s takes %s you were carrying.",RPC[1][0]->He,money(PLAYER.Gold,ANSI));
								OUT(outbuf);NL;
								PLAYER.Gold=0.;
							}
							ExchangeWeapon(RPC[1][0],ONLINE);
							ExchangeArmor(RPC[1][0],ONLINE);
							if(strlen(PLAYER.Blessed)) {
								sprintf(outbuf,"%sYour shining aura fades away%s.%s.%s.",fore(YELLOW),fore(WHITE),fore(GRY),fore(BLACK));
								OUT(outbuf);NL;
								memset(PLAYER.Blessed,0,sizeof(PLAYER.Blessed));
								sprintf(line[numline++],"A shining aura surrounds you.",money(ENEMY.Gold,ENEMY.Emulation),HE(ONLINE));
								strcpy(ENEMY.Blessed,PLAYER.ID);
							}
							if(strlen(ENEMY.Cursed)) {
								strcpy(PLAYER.Cursed,ENEMY.ID);
								sprintf(outbuf,"%sA %sblack%s cloud appears and hovers over you.",fore(GRY),fore(BLACK),fore(GRY));
								OUT(outbuf);NL;
								memset(ENEMY.Cursed,0,sizeof(ENEMY.Cursed));
								strcpy(line[numline++],"Your black cloud left you.");
							}
							if(strlen(PLAYER.Gang) && !strnicmp(PLAYER.Gang,ENEMY.Gang,20)) {
								sprintf(gm[0].Name,"%.20s",PLAYER.Gang);
								if(RPGserver(SERVER_GETGANG,(UBYTE *)&gm[0])) {
									if(!strcmp(ENEMY.ID,gm[0].MemberID[0])) {
										sprintf(outbuf,"%s%s says, \"%sLet that be a lesson to you, punk!%s\"",fore(CYN),RPC[1][0]->He,fore(WHITE),fore(CYN));
										OUT(outbuf);NL;
									}
									if(!strcmp(PLAYER.ID,gm[0].MemberID[0])) {
										for(i=0; i<4 && strcmp(ENEMY.ID,gm[0].MemberID[i]); i++);
										if(i<4) {
											sprintf(outbuf,"%s takes over as leader of %.20s.",RPC[1][0]->He,PLAYER.Gang);
											OUT(outbuf);NL;
											swmem(gm[0].MemberID[0],gm[0].MemberID[i],sizeof(gm[0].MemberID[0]));
											RPGserver(SERVER_PUTGANG,(UBYTE *)&gm[0]);
											sprintf(line[numline++],"You took over as leader of %.20s.",ENEMY.Gang);
										}
									}
								}
							}
							NL;
						}
						RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
						note(ENEMY.ID);
						break;
					}
				}
				break;
			case 'M':
				if(!arena) {
					OUT("You have no more arena fights.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				sprintf(prompt,"%sFight what monster (1-%u,<D>emon)? ",fore(CYN),NUMARENA);
				OUT(prompt);
				if(ins(2))
					break;
				NL;NL;
				i=atoi(inbuf);
				if((i<1 || i>NUMARENA) && toupper(*inbuf)!='D')
					break;
				if(toupper(*inbuf)=='D') {
					NL;
					if(PLAYER.Level<50) {
						OUT("You are not powerful enough to fight demons yet.  Go fight some monsters.");NL;
						paws=!PLAYER.Expert;
						break;
					}
					JOUST(PLAYER.Level);
					sprintf(outbuf,"The ancient necromancer will summon you a demon for %s.",money(d,ANSI));
					OUT(outbuf);NL;NL;
					if(PLAYER.Gold<d) {
						OUT("You don't have enough!");NL;
						paws=!PLAYER.Expert;
						break;
					}
					sprintf(prompt,"%sWill you pay (Y/N)? ",fore(CYN));
					OUT(prompt);
					c=inkey('N','N');
					NL;NL;
					if(c!='Y') {
						OUT("His eyes glow red and he says, \"I don't make deals!\"");NL;
						paws=!PLAYER.Expert;
						break;
					}
					PLAYER.Gold-=d;
					OUT("As you hand him the money, it disappears into thin air.");NL;NL;
					Delay(100);
					OUT("The old necromancer summons you a demon.");NL;NL;
					strcpy(ENEMY.Handle,"summoned demon");
					if(dice((ONLINE->INT+ONLINE->CHA)/50)>1)
						strcpy(ENEMY.Class,PLAYER.Class);
					else {
						i=dice(NUMCLASS)-1;
						strmfe(ENEMY.Class,ServerTable->class[i]->Origin,ServerTable->class[i]->Character[dice(MAXCLASS(i))-1]->Name);
					}
					ENEMY.Level=PLAYER.Level+dice(7)-4;
					if(ENEMY.Level>99)
						ENEMY.Level=99;
					ENEMY.Gender='I';
					i=MAXWEAPON(0)*ENEMY.Level/100+dice(3)-2;
					i=(i<1) ? 1 : (i>=MAXWEAPON(0)) ? MAXWEAPON(0)-1 : i;
					sprintf(ENEMY.Weapon,"NATURAL.%u",i);
					ENEMY.Gold+=value(ServerTable->weapon[0]->Item[i]->Value,ONLINE->CHA);
					i=MAXARMOR(0)*ENEMY.Level/100+dice(3)-2;
					i=(i<1) ? 1 : (i>=MAXARMOR(0)) ? MAXARMOR(0)-1 : i;
					sprintf(ENEMY.Armor,"NATURAL.%u",i);
					ENEMY.Gold+=value(ServerTable->armor[0]->Item[i]->Value,ONLINE->CHA);
					CreateRPC(RPC[1][0]);
					ENEMY.Poison=(UWORD)~0;
					ENEMY.Spell|=HEAL_SPELL | TELEPORT_SPELL | BLAST_SPELL;
					for(i=0; i<NUMMAGIC; i++)
						if(dice(ONLINE->CHA/2+5*i)==1) {
							if(i<16)
								ENEMY.Spell|=(UWORD)pow2((double)i);
							if(i>15 && i<24)
								ENEMY.XSpell|=(UWORD)pow2((double)i-16);
						}
				}
				else
				if(i) {
					i--;
					strcpy(ENEMY.Handle,ARENA(i)->Name);
					strcpy(ENEMY.Class,ARENA(i)->Class);
					ENEMY.Gender='I';
					ENEMY.Level=ARENA(i)->Level;
					strcpy(ENEMY.Weapon,ARENA(i)->Weapon);
					strcpy(ENEMY.Armor,ARENA(i)->Armor);
					ENEMY.Gold=ARENA(i)->Gold;
					ENEMY.Spell=ARENA(i)->Spell;
					CreateRPC(RPC[1][0]);
				}
				strmfp(filename,"PIX/Arena",ENEMY.Handle);
				type(filename,FALSE);
				sprintf(outbuf,"The %s is a level %u %s.",ENEMY.Handle,ENEMY.ExpLevel,CLASS(RPC[1][0])->Name);
				OUT(outbuf);NL;NL;
				if(RPC[1][0]->weapon_origin<NUMWEAPON) {
					sprintf(outbuf,"%s is carrying %s%s.",RPC[1][0]->He,AN(WEAPON(RPC[1][0])->Name),WEAPON(RPC[1][0])->Name);
					OUT(outbuf);NL;NL;
				}
				if(RPC[1][0]->armor_origin<NUMWEAPON) {
					sprintf(outbuf,"%s is wearing %s%s.",RPC[1][0]->He,AN(ARMOR(RPC[1][0])->Name),ARMOR(RPC[1][0])->Name);
					OUT(outbuf);NL;NL;
				}
				displayview();
				sprintf(prompt,"%sWill you fight the %s (Y/N)? ",fore(CYN),ENEMY.Handle);
				OUT(prompt);
				c=inkey('N','N');
				NL;
				if(c=='Y') {
					NL;
					arena--;
					Battle();
					if(!RPC[1][0]->HP) {
						modf(EXP(ENEMY.ExpLevel-1)/3.,&d);
						PLAYER.Experience+=d;
						sprintf(outbuf,"You get %.8g experience.",d);
						OUT(outbuf);NL;
						if(ENEMY.Gold>0.) {
							sprintf(line[numline++],"%s got %s you were carrying.",ONLINE->He,money(ENEMY.Gold,ENEMY.Emulation));
							PLAYER.Gold+=ENEMY.Gold;
							sprintf(outbuf,"You get %s %s was carrying.",money(ENEMY.Gold,ANSI),RPC[1][0]->he);
							OUT(outbuf);NL;
						}
						ExchangeWeapon(ONLINE,RPC[1][0]);
						ExchangeArmor(ONLINE,RPC[1][0]);
						note(ENEMY.ID);
						sprintf(outbuf,"%sdefeated a level %s%u %s",fore(CYN),fore(WHITE),ENEMY.Level,ENEMY.Handle);
						news(outbuf);
						paws=!PLAYER.Expert;
					}
					if(!ONLINE->HP) {
						if(PLAYER.Gold>0.) {
							ENEMY.Gold+=PLAYER.Gold;
							sprintf(outbuf,"%s takes %s you were carrying.",RPC[1][0]->He,money(PLAYER.Gold,ANSI));
							OUT(outbuf);NL;NL;
							PLAYER.Gold=0.;
						}
					}
					break;
				}
				break;
			case 'J':
				FOREVER {
					if(!joust) {
						OUT("You have run out of jousts.");NL;
						paws=!PLAYER.Expert;
						break;
					}
					OUT("You grab a horse and prepare yourself to joust.");NL;NL;
					FOREVER {
						ability[0]=(int)ONLINE->DEX*PLAYER.Level/10+2*PLAYER.Current.JoustWin-PLAYER.Current.JoustLoss+10;
						sprintf(prompt,"%sJoust what user (?=list): ",fore(CYN));
						if(!GetRPC(RPC[1][0]))
							break;
						if(!strcmp(PLAYER.ID,ENEMY.ID)) {
							sprintf(outbuf,"You can't joust a wimp like %s.",ONLINE->him);
							OUT(outbuf);NL;NL;
							continue;
						}
						if((BYTE)(PLAYER.Level-ENEMY.Level)>3) {
							OUT("You can only joust someone higher or up to three levels below you.");NL;NL;
							continue;
						}
						ability[1]=(int)RPC[1][0]->DEX*ENEMY.Level/10+2*ENEMY.Current.JoustWin-ENEMY.Current.JoustLoss+10;
						if(ability[1]<1 || (ENEMY.Level>1 && (ENEMY.Current.JoustWin+3*ENEMY.Level)<ENEMY.Current.JoustLoss)) {
							OUT("That knight is out practicing right now.");NL;NL;
							continue;
						}
						OUT("Jousting ability:");NL;NL;
						sprintf(outbuf,"%s%-25s%s%4d",fore(GRN),ENEMY.Handle,fore(WHITE),ability[1]);
						OUT(outbuf);NL;
						sprintf(outbuf,"%s%-25s%s%4d",fore(GRN),PLAYER.Handle,fore(WHITE),ability[0]);
						OUT(outbuf);NL;NL;
						n=(100-(PLAYER.Level>ENEMY.Level) ? PLAYER.Level : ENEMY.Level)/10+3;
						if(((ACCESS(ENEMY.Access)->RolePlay!='Y' || ACCESS(ENEMY.Access)->RolePlay!='Y') && ability[0]<ability[1]) || (ability[0]+n*PLAYER.Level)<(ability[1]+1L)) {
							sprintf(outbuf,"%s laughs rudely in your face!",ENEMY.Handle);
							OUT(outbuf);NL;NL;
							continue;
						}
						OUT(fore(CYN));
						OUT("Are you sure (Y/N)? ");
						if(!(c=inkey('N',NULL)))
							break;
						NL;NL;
						if(c=='Y')
							break;
					}
					if(!strlen(ENEMY.ID))
						break;
					joust--;
					jw=0;
					jl=0;
					OUT("The trumpets blare! You and your opponent ride into the arena. The crowd roars!");NL;NL;
					for(i=1; jw<3 && jl<3; i++) {
						sprintf(outbuf,"%s--=:)) Round %d/5: Won:%s%d %s^%s Lost:%s%d %s((:=--",fore(GRN),i,fore(WHITE),jw,fore(MAG),fore(GRN),fore(WHITE),jl,fore(GRN));
						OUT(outbuf);NL;
						do {
							sprintf(outbuf,"       %s<J>%s Joust %s* %s<F>%s Forfeit: ",fore(CYN),fore(YELLOW),fore(MAG),fore(CYN),fore(YELLOW));
							OUT(outbuf);
							if(!(c=inkey('J','F'))) {
								c='J';
								break;
							}
							NL;
						} while(c!='J' && c!='F');
						NL;
						if(c=='J') {
							OUT("You spur the horse.  The tension mounts.");NL;
							Delay(25);
							do {
								result=(ability[0]+dice(n*PLAYER.Level))-(ability[1]+dice(n*ENEMY.Level));
							} while(!result);
							if(result>0) {
								sprintf(outbuf,"%s-*>%s Thud! %s<*-%s A hit!  You win this pass!",fore(GRN),fore(WHITE),fore(GRN),fore(GRY));
								jw++;
							}
							else {
								sound("oof",48);
								sprintf(outbuf,"%s^>%s Oof! %s<^%s %s hits!  You lose this pass!",fore(MAG),fore(WHITE),fore(MAG),fore(GRY),RPC[1][0]->He);
								jl++;
							}
							OUT(outbuf);NL;NL;
						}
						if(c=='F') {
							OUT("The crowd throws rocks at you as you ride out of the arena.");
							PLAYER.Current.JoustLoss++;
							ENEMY.Current.JoustWin++;
							ENEMY.History.Joust++;
							sprintf(outbuf,"%slost to %s in a joust forfeit",fore(CYN),ENEMY.Handle);
							news(outbuf);
							break;
						}
					}
					if(jw==3) {
						OUT("You have won the joust!");NL;
						OUT("The crowd cheers!");NL;
						sound("cheer",64);
						JOUST(ENEMY.Level);
						sprintf(outbuf,"You win %s!",money(d,ANSI));
						OUT(outbuf);
						PLAYER.Gold+=d;
						PLAYER.Current.JoustWin++;
						PLAYER.History.Joust++;
						ENEMY.Current.JoustLoss++;
						numline=0;
						sprintf(line[numline++],"%s defeated you in a joust.",PLAYER.Handle);
						note(ENEMY.ID);
					}
					if(jl==3) {
						OUT("You have lost the joust.");NL;
						OUT("The crowd boo's you.");NL;NL;
						sound("boo",64);
						sprintf(outbuf,"%s spits in your face.",ENEMY.Handle);
						OUT(outbuf);
						JOUST(PLAYER.Level);
						if(ENEMY.Loan>0.) {
							ENEMY.Loan-=d;
							if(ENEMY.Loan<0.) {
								ENEMY.Gold-=ENEMY.Loan;
								ENEMY.Loan=0.;
							}
						}
						else
							ENEMY.Gold+=d;
						ENEMY.Current.JoustWin++;
						ENEMY.History.Joust++;
						PLAYER.Current.JoustLoss++;
						numline=0;
						sprintf(line[numline++],"You defeated %s in a joust.",PLAYER.Handle);
						note(ENEMY.ID);
						sprintf(outbuf,"%slost to %s in a joust contest",fore(CYN),ENEMY.Handle);
						news(outbuf);
					}
					RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
					NL;NL;
				}
				break;
			case 'C':
				if(ACCESS(acclvl)->RolePlay=='Y') {
					Cast(ONLINE,NULL);
					paws=!PLAYER.Expert;
				}
				break;
			case 'P':
				if(ACCESS(acclvl)->RolePlay=='Y') {
					PoisonWeapon(ONLINE);
					paws=!PLAYER.Expert;
				}
				break;
			case 'G':
				if(from=='S')
					return;
				from='A';
				Square();
				if(from=='Q')
					return;
				break;
			case 'Y':
				yourstats();
				paws=!PLAYER.Expert;
				break;
		}
	}
	from='Q';
}
