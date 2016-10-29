/******************\
 *  RPGBBS Arena  *
\******************/
#include "RPGBBS.h"

void ArenaPixFile(void)
{
	FILE *NMEfp;

	strmfp(filename,ArenaPIXpath,NME.Handle);
	if(ANSI)
		strmfe(filename,filename,"ANS");
	else
		strmfe(filename,filename,"TXT");

	if(!(NMEfp=fopen(filename,"rb")))
		return;

	while(!feof(NMEfp)) {
		if(!feof(NMEfp))
			newline();
		inputs(line[0],sizeof(line[0]),NMEfp);
		outs(line[0]);
	}
	fclose(NMEfp);
	normal();
}

void PlayerPixFile(void)
{
	FILE *PICfp;
	int i,j,k;

	strmfp(filename,PlayerPIXpath,CLASS(NME.Class)->Name);
	if(ANSI)
		strmfe(filename,filename,"ANS");
	else
		strmfe(filename,filename,"TXT");
	i=0;
	if((PICfp=fopen(filename,"rb"))) {
		for(i=0; i<25 && !feof(PICfp); i++)
			inputs(line[i],sizeof(line[0]),PICfp);
		fclose(PICfp);
	}
	
	strmfp(filename,PlayerPIXpath,CLASS(USER(user)->Class)->Name);
	if(ANSI)
		strmfe(filename,filename,"ANS");
	else
		strmfe(filename,filename,"TXT");
	j=0;
	if((PICfp=fopen(filename,"rb"))) {
		for(j=0; j<25 && !feof(PICfp); j++)
			inputs(line[25+j],sizeof(line[0]),PICfp);
		fclose(PICfp);
	}

	for(k=0; k<i || k<j; k++) {
		outs((k<i) ? line[k] : "                           ");
		if(k<j) {
			outs("     ");
			outs(line[25+k]);
		}
		newline();
	}
	normal();
}

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
	int ability,NMEability,i,jw,jl,result;
	char c;

	while(!cd()) {
		nme=0;
		if(USER(user)->Experience>=ExpNeed(USER(user)->Level))
			break;
		if((c=Option(&RPGmenu))=='Q')
			break;

		switch(c) {
			case 'C':
				nme=0;
				memset(&NME,0,sizeof(NME));
				CastSpell();
				break;
			case 'G':
				if(from=='S')
					return;
				from='A';
				Square();
				if(from=='M')
					return;
				break;
			case 'J':
				FOREVER {
					if(joust>2) {
						outs("You have run out of jousts.");
						newline();
						break;
					}
					outs("You grab a horse and prepare yourself to joust.");
					newline();
					newline();
					FOREVER {
						ability=(int)Tagl*USER(user)->Level/10+2*USER(user)->JoustWin-USER(user)->JoustLoss+10;
						outs(Fcol(CYN));
						outs("Joust what user or <S>tatus? ");
						i=GetUser();
						if(stricmp(ins,"R")==0) {
							USER(user)->JoustWin=0;
							USER(user)->JoustLoss=0;
							PutUser(user);
							continue;
						}
						if(stricmp(ins,"S")==0) {
							sprintf(text,"%sJousts won:%s %d",Fcol(GRN),Fcol(WHITE),USER(user)->JoustWin);
							outs(text);
							newline();
							sprintf(text,"%sJousts loss:%s %d",Fcol(RED),Fcol(WHITE),USER(user)->JoustLoss);
							outs(text);
							newline();
							sprintf(text,"%sJousting ability:%s %d",Fcol(BRN),Fcol(WHITE),ability);
							outs(text);
							normal();
							newline();
							newline();
							outs("<R> Reset jousting stats");
							newline();
							newline();
							continue;
						}
						if(i<1)
							i=0;
						nme=i;
						if(!nme)
							break;
						NMEstat();
						if(nme==user) {
							sprintf(text,"You can't joust a wimp like %s.",NMEhim);
							outs(text);
							newline();
							newline();
							continue;
						}
						if((BYTE)(USER(user)->Level-USER(nme)->Level)>3) {
							outs("You can only attack someone higher or up to three levels below you.");
							newline();
							newline();
							continue;
						}
						NMEability=(int)NMEagl*USER(nme)->Level/10+2*USER(nme)->JoustWin-USER(nme)->JoustLoss+10;
						if(NMEability<1 || (USER(nme)->Level>1 && (USER(nme)->JoustWin+3*USER(nme)->Level)<USER(nme)->JoustLoss)) {
							outs("That knight is out practicing right now.");
							newline();
							newline();
							continue;
						}
						outs("Jousting ability:");
						newline();
						newline();
						sprintf(text,"%s%.26s:",Fcol(GRN),USER(nme)->Handle);
						outs(text);
						i=30-strlen(text);
						sprintf(text,"%*s%s%d",i,"",Fcol(WHITE),NMEability);
						outs(text);
						normal();
						newline();
						sprintf(text,"%s%.26s:",Fcol(GRN),USER(user)->Handle);
						outs(text);
						i=30-strlen(text);
						sprintf(text,"%*s%s%d",i,"",Fcol(WHITE),ability);
						outs(text);
						normal();
						newline();
						newline();
						if((ACCESS(USER(user)->Access)->User!='Y' || ACCESS(USER(nme)->Access)->User!='Y') && ability<NMEability) {
							sprintf(text,"%.26s laughs rudely in your face!",USER(nme)->Handle);
							outs(text);
							newline();
							newline();
							continue;
						}
						outs(Fcol(CYN));
						outs("Are you sure (Y/N)? ");
						if(!(c=inkey()))
							break;
						newline();
						newline();
						if(c=='Y')
							break;
					}
					if(!nme)
						break;
					joust++;
					jw=0;
					jl=0;
					outs("The trumpets blare! You and your opponent ride into the arena. The crowd roars!");
					newline();
					newline();
					for(i=1; jw<3 && jl<3; i++) {
						sprintf(text,"%s--=:)) Round %d/5: Won:%s%d %s^%s Lost:%s%d %s((:=--",Fcol(GRN),i,Fcol(WHITE),jw,Fcol(MAG),Fcol(GRN),Fcol(WHITE),jl,Fcol(GRN));
						outs(text);
						newline();
						do {
							sprintf(text,"       %s<J>%s Joust %s* %s<F>%s Forfeit: ",Fcol(CYN),Fcol(YELLOW),Fcol(MAG),Fcol(CYN),Fcol(YELLOW));
							outs(text);
							if(!(c=inkey())) {
								c='J';
								break;
							}
							newline();
						} while(c!='J' && c!='F');
						newline();
						if(c=='J') {
							outs("You spur the horse.  The tension mounts.");
							newline();
							Delay(30);
							do {
								result=(ability+dice(USER(user)->Level*10))-(NMEability+dice(USER(nme)->Level*10));
							} while(result==0);
							if(result>0) {
								sprintf(text,"%s-*>%s Thud! %s<*-%s A hit!  You win this pass!",Fcol(GRN),Fcol(WHITE),Fcol(GRN),Fcol(GRY));
								jw++;
							}
							else {
								sprintf(text,"%s^>%s Oof! %s<^%s %s hits!  You lose this pass!",Fcol(MAG),Fcol(WHITE),Fcol(MAG),Fcol(GRY),NMEHe);
								jl++;
							}
							outs(text);
							newline();
							newline();
						}
						if(c=='F') {
							outs("The crowd throws rocks at you as you ride out of the arena.");
							USER(user)->JoustLoss++;
							PutUser(user);
							USER(nme)->JoustWin++;
							PutUser(nme);
							LogLoser(USER(nme)->Handle,"joust forfeit");
							break;
						}
					}
					if(jw==3) {
						outs("You have won the joust!");
						newline();
						outs("The crowd cheers!");
						newline();
						JOUST(USER(nme)->Level);
						sprintf(text,"You win %s!",money(d,user));
						outs(text);
						USER(user)->Gold+=d;
						USER(user)->JoustWin++;
						PutUser(user);
						USER(nme)->JoustLoss++;
						PutUser(nme);
						numline=0;
						sprintf(line[numline++],"%-.26s defeated you in a joust.",USER(user)->Handle);
						UserNote(nme);
					}
					if(jl==3) {
						outs("You have lost the joust.");
						newline();
						outs("The crowd boo's you.");
						newline();
						newline();
						sprintf(text,"%-.26s spits in your face.",USER(nme)->Handle);
						outs(text);
						JOUST(USER(user)->Level);
						USER(nme)->Gold+=d;
						USER(nme)->JoustWin++;
						PutUser(nme);
						USER(user)->JoustLoss++;
						PutUser(user);
						numline=0;
						sprintf(line[numline++],"You defeated %-.26s in a joust.",USER(user)->Handle);
						UserNote(nme);
						if(ability<NMEability)
							LogLoser(USER(nme)->Handle,"joust");
					}
					newline();
					newline();
				}
				break;
			case 'M':
				if(fight>2) {
					outs("You have run out of arena fights.");
					newline();
					break;
				}
				FOREVER {
					sprintf(text,"%sFight what monster (1-%d,<D>emon)? ",Fcol(CYN),MAXAM);
					outs(text);
					if(input())
						break;
					newline();
					memset(&NME,0,sizeof(struct User));
					if(toupper(ins[0])=='D') {
						newline();
						if(USER(user)->Level<50) {
							outs("You are not powerful enough to fight demons yet.  Go fight some monsters.");
							newline();
							break;
						}
						JOUST(USER(user)->Level);
						sprintf(text,"The ancient necromancer will summon you a demon for %s.",money(d,user));
						outs(text);
						newline();
						newline();
						if(USER(user)->Gold<d) {
							outs("You don't have enough!");
							newline();
							break;
						}
						outs(Fcol(CYN));
						outs("Will you pay (Y/N)? ");
						if(!(c=inkey()))
							break;
						newline();
						newline();
						if(c!='Y') {
							outs("His eyes glow red and he says, \"I don't make deals!\"");
							newline();
							break;
						}
						USER(user)->Gold-=d;
						PutUser(user);
						outs("As you hand him the money, it disappears into thin air.");
						newline();
						Delay(100);
						newline();
						outs("The old necromancer summons you a demon.");
						newline();
						newline();
						nme=0;
						CreateMonster(&NME,dice((Tint+Tchr)/50)>1 ? USER(user)->Class : dice(MAXCLASS-1),USER(user)->Level+dice(7)-4);
						strcpy(NME.Handle,"summoned Demon");
						NME.Str=99;
						NME.Int=99;
						NME.Agl=99;
					}
					else {
						i=atoi(ins);
						if(i<1 || i>MAXAM)
							break;
						i--;
						strncpy(NME.Handle,ARENA(i)->Name,sizeof(NME.Handle));
						NME.Class=ARENA(i)->Class;
						NME.Level=ARENA(i)->Level;
						NME.ExpLevel=ARENA(i)->Level;
						NME.HP=ARENA(i)->HP;
						NME.SP=ARENA(i)->SP;
						NME.Str=ARENA(i)->Str;
						NME.Int=ARENA(i)->Int;
						NME.Agl=ARENA(i)->Agl;
						NME.Weapon=ARENA(i)->Weapon;
						NME.Armor=ARENA(i)->Armor;
						NME.Spell=ARENA(i)->Spell;
						NME.Gold=ARENA(i)->Gold;
					}
					ArenaPixFile();
					NME.Sex='I';
					sprintf(text,"  You encounter %s %s!",strchr("AEIOU",NME.Handle[0]) ? "an" : "a",NME.Handle);
					outs(text);
					newline();
					if(NME.Weapon>0) {
						sprintf(text,"It's carrying %s %s.",strchr("AEIOU",WEAPON(NME.Weapon)->Name[0]) ? "an" : "a",WEAPON(NME.Weapon)->Name);
						outs(text);
						newline();
						newline();
					}
					if(NME.Armor>0) {
						sprintf(text,"It has %s %s.",strchr("AEIOU",ARMOR(NME.Armor)->Name[0]) ? "an" : "a",ARMOR(NME.Armor)->Name);
						outs(text);
						newline();
						newline();
					}
					sprintf(text,"It has %d hit points.",NME.HP);
					outs(text);
					newline();
					newline();
					NMEstat();
					outs(Fcol(CYN));
					outs("Will you fight it (Y/N)? ");
					if(!(c=inkey()))
						break;
					newline();
					newline();
					if(c!='Y')
						continue;
					fight++;
					Battle();
					if(Tint<USER(user)->Int) {
						Tint=USER(user)->Int+((USER(user)->Blessed) ? 10 : (USER(user)->Cursed) ? -10 : 0);
						if(Tint>100)
							Tint=100;
					}
					if(Tagl<USER(user)->Agl) {
						Tagl=USER(user)->Agl+((USER(user)->Blessed) ? 10 : (USER(user)->Cursed) ? -10 : 0);
						if(Tagl>100)
							Tagl=100;
					}
					break;
				}
				break;
			case 'P':
				PoisonWeapon();
				break;
			case 'U':
				if(ACCESS(USER(user)->Access)->User!='Y') {
					outs("You cannot fight other users yet; go fight a monster.");
					newline();
					newline();
					break;
				}
				if(fight>2) {
					outs("You have run out of arena fights.");
					newline();
					break;
				}
				FOREVER {
					outs(Fcol(CYN));
					outs("Fight what user (?=list)? ");
					i=GetUser();
					if(i<1)
						i=0;
					nme=i;
					if(!nme)
						break;
					memcpy(&NME,USER(nme),sizeof(struct User));
					if(nme==user) {
						sprintf(text,"You can't fight a wimp like %s.",USER(user)->Sex=='M' ? "him" : "her");
						outs(text);
						newline();
						newline();
						continue;
					}
					if(ACCESS(NME.Access)->User!='Y') {
						outs("That user does not fight.");
						newline();
						newline();
						continue;
					}
					if((BYTE)(USER(user)->Level-NME.Level)>3) {
						outs("You can only attack someone higher or up to three levels below you.");
						newline();
						newline();
						continue;
					}
					if(NME.Status) {
						sprintf(text,"%.26s was killed by %.26s.",NME.Handle,USER(NME.Status)->Handle);
						outs(text);
						newline();
						newline();
						continue;
					}
					NMEstat();
					PlayerPixFile();
					sprintf(text,"%sName:%s %-32.32s You:",Fcol(GRN),Fcol(WHITE),NME.Handle);
					outs(text);
					newline();
					sprintf(text,"%sClass:%s %-31.31s %s",Fcol(GRN),Fcol(WHITE),CLASS(NME.Class)->Name,CLASS(USER(user)->Class)->Name);
					outs(text);
					newline();
					sprintf(text,"%sLevel:%s%3d ",Fcol(GRN),Fcol(WHITE),NME.Level);
					outs(text);
					if(NME.Level!=NME.ExpLevel)
						sprintf(text,"(%d)",NME.ExpLevel);
					else
						text[0]='\0';
					sprintf(OUTbuf,"%s%*s %d",text,28-strlen(text),"",USER(user)->Level);
					outs(OUTbuf);
					newline();
					sprintf(text,"%sWeapon:%s %.25s ",Fcol(GRN),Fcol(WHITE),WEAPON(NME.Weapon)->Name);
					outs(text);
					if(NME.Blessed || NME.Cursed)
						sprintf(text,"%s%s",points(NMEhit),Fcol(WHITE));
					else
						text[0]='\0';
					sprintf(OUTbuf,"%s%*s",text,30-strlen(WEAPON(NME.Weapon)->Name)-(strlen(text) ? 4 : 0),"");
					outs(OUTbuf);
					outs(WEAPON(USER(user)->Weapon)->Name);
					if(Thit) {
						sprintf(text," %s",points(Thit));
						outs(text);
					}
					newline();
					sprintf(text,"%sArmor:%s %.26s ",Fcol(GRN),Fcol(WHITE),ARMOR(NME.Armor)->Name);
					outs(text);
					if(NME.Blessed || NME.Cursed)
						sprintf(text,"%s%s",points(NMEshield),Fcol(WHITE));
					else
						text[0]='\0';
					sprintf(OUTbuf,"%s%*s",text,31-strlen(ARMOR(NME.Armor)->Name)-(strlen(text) ? 4 : 0),"");
					outs(OUTbuf);
					outs(ARMOR(USER(user)->Armor)->Name);
					if(Tshield) {
						sprintf(text," %s",points(Tshield));
						outs(text);
					}
					newline();
					sprintf(text,"%sHit points:%s %-26d %d",Fcol(GRN),Fcol(WHITE),NME.HP,Thp);
					outs(text);
					newline();
					if(NME.SP+Tsp) {
						sprintf(text,"%sSpell points:%s %-24d %d",Fcol(GRN),Fcol(WHITE),NME.SP,Tsp);
						outs(text);
						newline();
					}
					newline();
					outs(Fcol(CYN));
					outs("Are you sure (Y/N)? ");
					if(!(c=inkey()))
						break;
					newline();
					newline();
					if(c=='Y')
						break;
				}
				if(!nme)
					break;
				fight++;
				Battle();
				if(Tint<USER(user)->Int) {
					Tint=USER(user)->Int+((USER(user)->Blessed) ? 10 : (USER(user)->Cursed) ? -10 : 0);
					if(Tint>100)
						Tint=100;
				}
				if(Tagl<USER(user)->Agl) {
					Tagl=USER(user)->Agl+((USER(user)->Blessed) ? 10 : (USER(user)->Cursed) ? -10 : 0);
					if(Tagl>100)
						Tagl=100;
				}
				break;
			case 'Y':
				YourStats();
				break;
		}
	}
	from='M';
}
