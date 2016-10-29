/********************\
 *  RPGBBS Dungeon  *
\********************/
#include "RPGBBS.h"

void DungeonPixFile(void)
{
	FILE *NMEfp;

	strmfp(filename,DungeonPIXpath,NME.Handle);
	if(ANSI)
		strmfe(filename,filename,"ANS");
	else
		strmfe(filename,filename,"TXT");

	if(!(NMEfp=fopen(filename,"rb")))
		return;

	while(!feof(NMEfp)) {
		inputs(line[0],sizeof(line),NMEfp);
		outs(line[0]);
		newline();
	}
	fclose(NMEfp);
	BoldFlag=TRUE;
	normal();
	Delay(20);
}

void GenLvl(void)
{
	int i,j;

	sprintf(text,"[#%u] %.26s is entering dungeon level %d.",user,USER(user)->Handle,level);
	RPGmessage(text);
	MaxDY=6+dice(level/25+1);
	if(MaxDY>10)
		MaxDY=10;
	MaxDX=6+dice(level/10+1);
	if(MaxDX>15)
		MaxDX=15;
	for(i=0; i<MaxDY; i++) {
		for(j=0; j<MaxDX; j++)
			dungeon[i][j]=dice(8);
		Delay(5);
	}
	i=dice(MaxDY)-1;
	j=dice(MaxDX)-1;
	dungeon[i][j]=10;
	do {
		i=dice(MaxDY)-1;
		j=dice(MaxDX)-1;
	} while(dungeon[i][j]==10);
	dungeon[i][j]=11;
	if(dice(600/level)==1) {
		do {
			i=dice(MaxDY)-1;
			j=dice(MaxDX)-1;
		} while(dungeon[i][j]>8);
		dungeon[i][j]=9;	/* Wishing Well */
	}
	map=FALSE;
	gift=FALSE;
}

void Teleport(void)
{
	char c;

	outs(Fcol(YELLOW));
	outs("What do you wish to do?");
	normal();
	newline();
	newline();
	outs("<U> Teleport up 1 level");
	newline();
	outs("<D> Teleport down 1 level");
	newline();
	outs("<O> Teleport out of the dungeon");
	newline();
	outs("<R> Random teleport");
	newline();
	outs("<S> Stay on this level");
	newline();
	do {
		newline();
		outs(Fcol(CYN));
		outs("Teleport: ");
		if(input())
			break;
		c=toupper(ins[0]);
	} while(c!='U' && c!='D' && c!='O' && c!='R' && c!='S');
	newline();
	newline();
	if(c!='S' && dice(Tchr)==1) {
		sprintf(text,"%sYou hear a deep, scottish voice bellow, \"%sI canna even beam a fly out!%s\"",Fcol(CYN),Fcol(WHITE),Fcol(GRY));
		outs(text);
		newline();
		dungeon[dy][dx]=0x81;
		return;
	}
	switch(c) {
		case 'U':
			if(level>1)
				level--;
			sprintf(text,"%sNow teleporting up to level %d.%s",Fcol(GRN),level,Fcol(GRY));
			outs(text);
			newline();
			newline();
			outs("Everything spins around, and suddenly you're elsewhere...");
			newline();
			newline();
			GenLvl();
			ExamineRoom();
			break;
		case 'D':
			level++;
			sprintf(text,"%sNow teleporting down to level %d.%s",Fcol(GRN),level,Fcol(GRY));
			outs(text);
			newline();
			newline();
			outs("Everything spins around, and suddenly you're elsewhere...");
			newline();
			newline();
			GenLvl();
			ExamineRoom();
			break;
		case 'O':
			outs("Now teleporting out of the dungeon.");
			newline();
			newline();
			outs("As you leave, you hear a voice boom, \"Next time, you will not escape so easily!");
			newline();
			outs("Moohahahaha!...\"");
			newline();
			Delay(100);
			escape=TRUE;
			break;
		case 'R':
			outs("Everything spins around, and suddenly you're elsewhere...");
			newline();
			newline();
			dx=dice(MaxDX)-1;
			dy=dice(MaxDY)-1;
			break;
	}
}

void WishingWell(void)
{
	int i;

	dungeon[dy][dx]=0x81;
	outs(Fcol(MAGENTA));
	outs("You have found a legendary Wishing Well.");
	newline();
	newline();
	outs(Fcol(YELLOW));
	outs("What do you wish to do?");
	normal();
	newline();
	newline();
	outs(" <1> Resurrect all the dead players");
	newline();
	outs(" <2> Teleport to any level");
	newline();
	outs(" <3> Bless yourself");
	newline();
	outs(" <4> Curse another");
	newline();
	outs(" <5> Cure yourself");
	newline();
	outs(" <6> Rob the bank");
	newline();
	newline();
	do {
		outs(Fcol(CYN));
		outs("Select wish: ");
		if(input())
			return;
		newline();
		i=atoi(ins);
	} while(i<1 || i>6);
	newline();
	switch(i) {
		case 1:
			if(dice(Tint)>1) {
				numline=0;
				sprintf(line[numline++],"%.26s's wish brought you back to life!",USER(user)->Handle);
				for(i=1; i<=MAXUSER; i++) {
					if(USER(i)->Status>0) {
						sprintf(text,"Resurrecting %.26s.",USER(i)->Handle);
						outs(text);
						newline();
						USER(i)->Status=0;
						PutUser(i);
						UserNote(i);
					}
				}
			}
			else {
				beep();
				outs("You die instead!");
				strcpy(reason,"tried wishing to resurrect the dead");
				logoff=TRUE;
			}
			break;
		case 2:
			if(dice(Tint/2)>1) {
				do {
					sprintf(text,"%sTeleport to level (1-%d): ",Fcol(CYN),MAXLEVEL);
					outs(text);
					if(input())
						return;
					newline();
					level=atoi(ins);
				} while(level<1 || level>MAXLEVEL);
				newline();
			}
			else
				level=dice(MAXLEVEL);
			sprintf(text,"%sNow teleporting to level %d.%s",Fcol(GRN),level,Fcol(GRY));
			outs(text);
			newline();
			newline();
			outs("Everything spins around, and suddenly you're elsewhere...");
			newline();
			newline();
			GenLvl();
			ExamineRoom();
			break;
		case 3:
			if(dice(Tint/5)>1) {
				nme=BlessRemove();
				numline=0;
				sprintf(line[numline++],"%.26s wished for your Blessedness!",USER(user)->Handle);
				UserNote(nme);
				sprintf(text,"You take the Blessedness from %.26s!",USER(nme)->Handle);
				outs(text);
				USER(user)->Blessed=nme;
				if(USER(user)->Cursed) {
					CurseAssign();
					USER(user)->Cursed=0;
				}
				Tstr+=10;
				if(Tstr>100)
					Tstr=100;
				Tint+=10;
				if(Tint>100)
					Tint=100;
				Tagl+=10;
				if(Tagl>100)
					Tagl=100;
				Tchr+=10;
				if(Tchr>100)
					Tchr=100;
				Thit+=USER(user)->Weapon/10;
				Tshield+=USER(user)->Armor/10;
			}
			else {
				beep();
				nme=CurseRemove();
				numline=0;
				sprintf(line[numline++],"%.26s's wish got your Cursedness!",USER(user)->Handle);
				UserNote(nme);
				USER(user)->Cursed=nme;
				if(USER(user)->Blessed) {
					BlessAssign();
					USER(user)->Blessed=0;
				}
				sprintf(text,"You get %.26s's Cursedness instead!",USER(nme)->Handle);
				outs(text);
				newline();
				outs(Fcol(BLUE));
				outs("A black cloud appears and hovers over you...");
				normal();
				Tstr-=10;
				Tint-=10;
				Tagl-=10;
				Tchr-=10;
				Thit-=USER(user)->Weapon/10;
				Tshield-=USER(user)->Armor/10;
			}
			break;
		case 4:
			nme=CurseRemove();
			if(dice(Tint/10)>1) {
				sprintf(text,"You take the Cursedness from %.26s!",USER(nme)->Handle);
				outs(text);
				newline();
				do {
					outs(Fcol(CYN));
					outs("Who do you wish to curse now? ");
					i=GetUser();
					newline();
				} while(i<0 || i>MAXUSER);
				if(USER(i)->Blessed)
					BlessAssign();
				numline=0;
				sprintf(line[numline++],"%.26s wished your Cursedness onto %.26s!",USER(user)->Handle,USER(i)->Handle);
				UserNote(nme);
				USER(i)->Blessed=0;
				USER(i)->Cursed=user;
				PutUser(i);
				numline=0;
				sprintf(line[numline++],"%.26s wished you Cursed!",USER(user)->Handle);
				UserNote(i);
			}
			else {
				beep();
				numline=0;
				sprintf(line[numline++],"%.26s's wish got your Cursedness!",USER(user)->Handle);
				UserNote(nme);
				sprintf(text,"You get %.26s's Cursedness instead!",USER(nme)->Handle);
				outs(text);
				newline();
				if(USER(user)->Blessed)
					BlessAssign();
				USER(user)->Blessed=0;
				USER(user)->Cursed=nme;
				outs(Fcol(BLUE));
				outs("A black cloud appears and hovers over you...");
				normal();
				Tstr-=10;
				Tint-=10;
				Tagl-=10;
				Tchr-=10;
				Thit-=USER(user)->Weapon/10;
				Tshield-=USER(user)->Armor/10;
			}
			break;
		case 5:
			outs("Your vitality is completely restored.");
			newline();
			if(dice(Tint/15)>1) {
				Thp=USER(user)->HP;
				Tsp=USER(user)->SP;
			}
			else {
				USER(user)->Level=dice(100);
				USER(user)->ExpLevel=USER(user)->Level;
				USER(user)->Experience=ExpNeed(USER(user)->Level)-1.;
				USER(user)->Class=dice(MAXCLASS)-1;
				USER(user)->HP=15;
				for(i=2; i<USER(user)->Level; i++)
					USER(user)->HP+=9+(i<5)+(i<15)+(i<25)+dice(i)+i;
				Thp=USER(user)->HP;
				if(CLASS(USER(user)->Class)->Magic>1) {
					USER(user)->SP=15;
					for(i=2; i<USER(user)->Level; i++)
						USER(user)->SP+=9+(i<5)+(i<15)+(i<25)+dice(i)+i;
					Tsp=NME.SP;
				}
				else {
					USER(user)->SP=0;
					USER(user)->Spell=0;
					Tsp=0;
				}
				switch(dice(3)) {
					case 1:
						USER(user)->Sex='F';
						break;
					case 2:
						USER(user)->Sex='M';
						break;
					case 3:
						USER(user)->Sex='I';
						break;
				}
				PutUser(user);
				Delay(100);
				sprintf(OUTbuf,"...for a level %u %s (%c) that is!",USER(user)->Level,CLASS(USER(user)->Class)->Name,USER(user)->Sex);
				outs(OUTbuf);
			}
			break;
		case 6:
			if(dice(Tint/20)>1) {
				for(i=0; i<=MAXUSER; i++) {
					if(i>0 && USER(i)->Access>0 && USER(i)->Bank>0.) {
						d+=USER(i)->Bank;
						sprintf(OUTbuf,"You get %s from %.26s's account!",money(USER(i)->Bank,user),USER(i)->Handle);
						outs(OUTbuf);
						newline();
						numline=0;
						sprintf(line[numline++],"%.26s's wish robbed the bank!",USER(user)->Handle);
						sprintf(line[numline++],"%s stole %s from your account!",USER(user)->Sex=='M' ? "He" : USER(user)->Sex=='F' ? "She" : "It",money(USER(i)->Bank,i));
						UserNote(i);
					}
					USER(i)->Bank=0.;
					PutUser(i);
				}
				USER(user)->Gold+=d;
				PutUser(user);
			}
			else {
				beep();
				outs("A guard catches you and throws you into jail!");
				newline();
				outs("You might be released by your next call.");
				strcpy(reason,"tried a wish to rob the bank");
				logoff=TRUE;
			}
			break;
	}
	newline();
	Delay(100);
}

void DMattack(char *str)
{
	FILE *NMEfp;
	int mon;

	nme=dice(MAXUSER);
	if(nme!=user && USER(nme)->Level==level && USER(nme)->Class>0 && USER(nme)->Status==0) {
		strmfp(filename,PlayerPIXpath,CLASS(USER(nme)->Class)->Name);
		if(ANSI)
			strmfe(filename,filename,"ANS");
		else
			strmfe(filename,filename,"TXT");
		if((NMEfp=fopen(filename,"rb"))) {
			while(!feof(NMEfp)) {
				if(!feof(NMEfp))
					newline();
				inputs(line[0],sizeof(line),NMEfp);
				outs(line[0]);
			}
			fclose(NMEfp);
			normal();
		}
		memcpy(&NME,USER(nme),sizeof(struct User));
		sprintf(text,"It's %s!",USER(nme)->Handle);
		newline();
		outs(text);
	}
	else {
		outs(str);
		newline();
		Delay(20);
		nme=0;
		if(dice(Tchr)>1) {
			mon=level+dice(11)-6;
			mon=(mon<1) ? dice(5) : (mon>MAXDM) ? MAXDM-5+dice(5) : mon;
			mon--;
			CreateMonster(&NME,(DUNGEON(mon)->Class) ? DUNGEON(mon)->Class : USER(user)->Class,level);
		}
		else {
			mon=dice(MAXDM);
			mon--;
			CreateMonster(&NME,(DUNGEON(mon)->Class) ? DUNGEON(mon)->Class : USER(user)->Class,mon);
		}
		NME.Sex='d';
		NME.HP/=4;
		NME.SP/=4;
		strncpy(NME.Handle,DUNGEON(mon)->Name,sizeof(NME.Handle));
		if(NME.Weapon>0 || NME.Armor>0 || !USER(user)->Expert || local || carrier>2400)
			DungeonPixFile();
		sprintf(text,"It's %s %s!",strchr("aeiou",NME.Handle[0]) ? "an" : "a",NME.Handle);
		newline();
		outs(text);
		newline();
		newline();
		Delay(20);
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
		sprintf(text,"It has %u hit points, and it doesn't look friendly...",NME.HP);
		outs(text);
	}
	newline();
	newline();
	Delay(20);
	from='D';
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
	if(NMEhp==0) {
		if((dungeon[dy][dx] & 0x7f)==3 || (dungeon[dy][dx] & 0x7f)==4)
			dungeon[dy][dx]=0x81;
		mon=4*USER(user)->Level/(Tchr+1)+1;
		if(mon<2)
			mon=2;
		if(!map && dice(mon)==1) {
			map=TRUE;
			outs(Fcol(YELLOW));
			outs("You find a map!");
		}
	}
	if(Thp>0 && NMEhp>0) {
		dx=hx;
		dy=hy;
		ExamineRoom();
	}
}

void ExamineRoom(void)
{
	int find,i,n,x;
	char c;

	if(dx<0)
		dx=0;
	if(dx>=MaxDX)
		dx=MaxDX-1;
	if(dy<0)
		dy=0;
	if(dy>=MaxDY)
		dy=MaxDY-1;
	dungeon[dy][dx]|=0x80;
	switch(dungeon[dy][dx] & 0x7f) {
		case 1:
			outs("You are in a");
			switch(dice(3)) {
				case 1:
					outs("n empty chamber.");
					break;
				case 2:
					outs(" barren chamber.");
					break;
				case 3:
					outs(" quiet chamber.");
					break;
			}
			n=20;
			break;
		case 2:
			outs("You cautiously enter a hidden cave.");
			Delay(25);
			n=10;
			if(dice(10)==1) {
				n=0;
				newline();
				newline();
				outs("On the ground, you find a small ");
				switch(dice(3)) {
					case 1:
						outs("bottle");
						break;
					case 2:
						outs("flask");
						break;
					case 3:
						outs("vial");
						break;
				}
				outs(", containing a potion.");
				newline();
				newline();
				if(dice(100)<(50+Tint/2)) {
					do {
						outs(Fcol(CYN));
						outs("Will you drink it (Y/N)? ");
						if(!(c=inkey()))
							break;
						newline();
					} while(c!='Y' && c!='N');
				}
				else {
					Delay(25);
					outs("You quaff it without hesitation.");
					newline();
					Delay(25);
					c='Y';
				}
				if(dice(level/10+2)>1) {
					newline();
					find=dice(6);
					if(find==1) {
						if(CLASS(USER(user)->Class)->Magic>1) {
							outs("It was a Potion of Magic.");
							if(c=='Y') {
								outs("  Your spell power is temporarily increased.");
								Tsp+=dice(USER(user)->SP-Tsp);
							}
						}
						else
							find=2;
					}
					if(find==2 || find==3) {
						outs("It was a Potion of Healing.");
						if(c=='Y') {
							outs("  Some of your wounds disappear.");
							Thp+=dice(USER(user)->HP-Thp);
						}
					}
					if(find==4) {
						if(CLASS(USER(user)->Class)->Magic>1) {
							outs("It was an Anti-Magic Potion!");
							if(c=='Y') {
								outs("  Your spell power is temporarily decreased.");
								x=dice(Tsp);
								if(x<=Tsp)
									Tsp-=x;
							}
						}
						else
							find=5;
					}
					if(find==5 || find==6) {
						outs("It contained poison!");
						if(c=='Y') {
							outs("  Your hit points are temporarily decreased.");
							x=dice(Thp);
							if(x<=Thp)
								Thp-=x;
						}
					}
				}
				else {
					newline();
					find=dice(9);
					switch(find) {
						case 1:
							if(Tchr<100) {
								outs("It was a Potion of Charm.");
								if(c=='Y') {
									outs("  Your charisma is temporarily increased.");
									Tchr+=dice(100-Tchr);
								}
							}
							else
								find=0;
							break;
						case 2:
							if(Tint<100) {
								outs("It was a Potion of Wisdom.");
								if(c=='Y') {
									outs("  Your intelligence is temporarily increased.");
									Tint+=dice(100-Tint);
								}
							}
							else
								find=0;
							break;
						case 3:
							if(Tstr<100) {
								outs("It was a Potion of Strength.");
								if(c=='Y') {
									outs("  Your stamina is temporarily increased.");
									Tstr+=dice(100-Tstr);
								}
							}
							else
								find=0;
							break;
						case 4:
							if(Tagl<100) {
								outs("It was a Potion of Dexterity.");
								if(c=='Y') {
									outs("  Your agility is temporarily increased.");
									Tagl+=dice(100-Tagl);
								}
							}
							else
								find=0;
							break;
						case 5:
							outs("It was a Potion of Cure.");
							if(c=='Y') {
								outs("  Your vitality is completely restored.");
								Thp=USER(user)->HP;
								Tsp=USER(user)->SP;
							}
							break;
						case 6:
							outs("It was a Beaker of Death.");
							if(c=='Y') {
								outs("  You slowly turn shades of blue and die.");
								strcpy(reason,"drank a Beaker of Death");
								logoff=TRUE;
							}
							break;
						case 7:
							if(USER(user)->Sex=='M') {
								outs("It was a Bottle of Estrogen.");
								if(c=='Y') {
									outs("  You lose your manlihood.");
									USER(user)->Sex='F';
								}
							}
							else {
								outs("It was a Bottle of Testosterone.");
								if(c=='Y') {
									outs("  You feel a buldge emerge in your britches.");
									USER(user)->Sex='M';
								}
							}
							break;
						case 8:
							outs("It was a Potion of Augment.");
							if(c=='Y') {
								outs("  You feel invincible.");
								if(Tstr<100)
									Tstr+=dice(100-Tstr);
								if(Tint<100)
									Tint+=dice(100-Tint);
								if(Tagl<100)
									Tagl+=dice(100-Tagl);
								if(Tchr<100)
									Tchr+=dice(100-Tchr);
								Thp+=2*dice(USER(user)->HP);
								Tsp+=2*dice(USER(user)->SP);
								Thit+=dice(USER(user)->Weapon)+dice(abs(Thit));
								Tshield+=dice(USER(user)->Armor)+dice(abs(Tshield));
							}
							break;
						case 9:
							outs("It was a vial of crack.");
							if(c=='Y') {
								outs("  The hit felt like shit.");
								Tstr-=dice(Tstr);
								Tint-=dice(Tint);
								Tagl-=dice(Tagl);
								Tchr-=dice(Tchr);
								Thp-=2*dice(Thp);
								if(Tsp)
									Tsp-=dice(Tsp);
								Thit-=dice(USER(user)->Weapon)+dice(abs(Thit));
								Tshield-=dice(USER(user)->Armor)+dice(abs(Tshield));
							}
							break;
					}
					if(!find) {
						outs("It was Narragansett Beer.");
						if(c=='Y') {
							outs("  You feel dizzy from its effects.");
							if(Tchr>5)
								Tchr-=dice(5);
							if(Tint>5)
								Tint-=dice(5);
							if(Tstr>5)
								Tstr-=dice(5);
							if(Tagl>5)
								Tagl-=dice(5);
						}
					}
				}
			}
			else
			if((CLASS(USER(user)->Class)->Magic==1 || CLASS(USER(user)->Class)->Magic==2) && USER(user)->Spell!=65535 && !gift) {
				x=(CLASS(USER(user)->Class)->Magic==1) ? 2 : 1;
				if(dice((140-Tchr)/2*x)>1)
					break;
				find=0;
				for(i=0; i<MAXMAGIC; i++) {
					if(!(USER(user)->Spell & (UWORD)pow2((double)i)))
						if(dice(2*x++)==1) {
							find=i+1;
							break;
						}
				}
				if(find) {
					n=0;
					gift=TRUE;
					find--;
					USER(user)->Spell|=(UWORD)pow2((double)find);
					PutUser(user);
					newline();
					newline();
					if(CLASS(USER(user)->Class)->Magic==1)
						sprintf(text,"You find a Wand of %s!",MAGIC(find)->Name);
					if(CLASS(USER(user)->Class)->Magic==2)
						sprintf(text,"You find a %s scroll!",MAGIC(find)->Name);
					outs(Fcol(YELLOW));
					outs(text);
				}
			}
			else
			if(CLASS(USER(user)->Class)->Poison  && USER(user)->Poison!=65535 && !gift) {
				x=(CLASS(USER(user)->Class)->Poison<0) ? 2 : 1;
				if(dice((140-Tchr)/2*x)>1)
					break;
				find=0;
				for(i=0; i<MAXPOISON; i++) {
					if(!(USER(user)->Poison & (UWORD)pow2((double)i)))
						if(dice(2*x++)==1) {
							find=i+1;
							break;
						}
				}
				if(find) {
					n=0;
					gift=TRUE;
					find--;
					USER(user)->Poison|=(UWORD)pow2((double)find);
					PutUser(user);
					newline();
					newline();
					sprintf(text,"You find a vial of %s poison!",POISON(find)->Name);
					outs(Fcol(YELLOW));
					outs(text);
				}
			}
			break;
		case 3:
			DMattack("There's something lurking in here...");
			n=0;
			break;
		case 4:
			DMattack("Something foul is in this chamber.");
			n=0;
			break;
		case 5:
			outs("There is a thief in this chamber!");
			newline();
			newline();
			modf(USER(user)->Gold*dice(25)/100.,&d);
			if(dice(3)>1) {
				outs("He suprises you!  As he passes by, he steals ");
				if(dice(Tchr/10)>1) {
					sprintf(text,"%s.",money(d,user));
					USER(user)->Gold-=d;
				}
				else {
					outs("your ");
					if(dice(2)==1) {
						if(map) {
							sprintf(text,"map.");
							map=FALSE;
						}
						else
						if(USER(user)->Spell && CLASS(USER(user)->Class)->Magic<3) {
							do {
								x=dice(MAXMAGIC)-1;
							} while((USER(user)->Spell & (UWORD)pow2((double)x))==0);
							if(CLASS(USER(user)->Class)->Magic==1)
								sprintf(text,"Wand of %s.",MAGIC(x)->Name);
							if(CLASS(USER(user)->Class)->Magic==2)
								sprintf(text,"%s scroll.",MAGIC(x)->Name);
							USER(user)->Spell-=pow2((double)x);
						}
						else
						if(USER(user)->Poison) {
							do {
								x=dice(MAXPOISON)-1;
							} while((USER(user)->Poison & (UWORD)pow2((double)x))==0);
							sprintf(text,"vial of %s poison.",POISON(x)->Name);
							USER(user)->Poison-=pow2((double)x);
						}
						else
							sprintf(text,"lint.");
					}
					else
					if(dice(Tchr/10)==1 && USER(user)->Weapon) {
						sprintf(text,"%s",WEAPON(USER(user)->Weapon)->Name);
						outs(text);
						if(Thit) {
							sprintf(text," %s",points(Thit));
							outs(text);
						}
						outs("...");
						newline();
						Delay(100);
						if(dice(100)>Tagl/2) {
							sprintf(text,"And he scampers away leaving you swearing after him!");
							USER(user)->Weapon=0;
							Thit=0;
						}
						else
							sprintf(text,"But you manage to hold on!");
					}
					else
						sprintf(text,"virtue.");
				}
			}
			else {
				sprintf(text,"You surprise the thief!  As he runs out, he drops %s.",money(d,user));
				USER(user)->Gold+=d;
			}
			PutUser(user);
			outs(text);
			n=0;
			break;
		case 6:
			outs("You have entered a north-south passage");
			if(dx!=hx) {
				outs(" through a secret door.");
				newline();
				outs("You hear the door lock behind of you");
			}
			outs(".");
			n=Tchr/2;
			break;
		case 7:
			outs("You have entered an east-west passage");
			if(dy!=hy) {
				outs(" through a secret door.");
				newline();
				outs("You hear the door lock behind of you");
			}
			outs(".");
			n=Tchr/2;
			break;
		case 8:
			outs("You have stepped onto a trapdoor!");
			newline();
			newline();
			n=0;
			if(dice(100)+20<Tagl) {
				outs("You manage to catch the edge and stop yourself from falling.");
				break;
			}
			outs(Fcol(YELLOW));
			outs("You've fallen down a level!");
			normal();
			newline();
			newline();
			if(dice(100)>(Tagl/2)) {
				Thit-=dice(level/20+1);
				sprintf(text,"Your %s is damaged from the fall!",WEAPON(USER(user)->Weapon)->Name);
				outs(text);
				newline();
				newline();
			}
			else if(dice(100)>(Tagl/2)) {
				Tshield-=dice(level/20+1);
				sprintf(text,"Your %s is damaged from the fall!",ARMOR(USER(user)->Armor)->Name);
				outs(text);
				newline();
				newline();
			}
			level++;
			GenLvl();
			ExamineRoom();
			break;
		case 9:
			WishingWell();
			n=0;
			break;
		case 10:
			outs(Fcol(MAGENTA));
			outs("You have entered a magical teleport chamber.");
			newline();
			newline();
			Teleport();
			n=0;
			break;
		case 11:
			outs("There is an old cleric in this room.");
			newline();
			newline();
			n=0;
			if(Thp>=USER(user)->HP) {
				outs("He says, \"Get outta here, snapperhead!\"");
				break;
			}
			i=level/10+1;
			x=level*CLASS(USER(user)->Class)->WC/MAXLEVEL-i+2;
			d=WEAPON(x)->Cost/i*(USER(user)->HP-Thp)/level;
			modf(d,&d);
			if(d<1.)
				d=1.;
			sprintf(text,"He says, \"I can heal all your wounds for %s.\"",money(d,user));
			outs(text);
			newline();
			newline();
			do {
				outs(Fcol(CYN));
				outs("Will you pay (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
			} while(c!='Y' && c!='N');
			newline();
			if(USER(user)->Gold<d) {
				outs("He says, \"Not!\"");
				break;
			}
			if(c=='Y') {
				if(dice(level+1)>1) {
					outs("He casts a Cure spell on you.");
					Thp=USER(user)->HP;
					USER(user)->Gold-=d;
					PutUser(user);
				}
				else {
					Delay(50);
					switch(dice(3)) {
						case 1:
							outs("Fssst!  His spell fails!");
							newline();
							newline();
							Delay(100);
							outs("He teleports himself and your gold out of the dungeon...");
							Delay(100);
							dungeon[dy][dx]=0x81;
							break;
						case 2:
							dungeon[dy][dx]=0x84;
							DMattack("The old cleric laughs as he transforms into a monster!");
							break;
						case 3:
							outs("As the room fades away, you hear the old cleric say, \"Oops!\"");
							newline();
							newline();
							Delay(100);
							if(dice(100)<(Tchr/2)) {
								outs("Now teleporting out of the dungeon.");
								newline();
								newline();
								escape=TRUE;
							}
							else {
								level+=dice(11)+dice(11)+3;
								sprintf(text,"Now teleporting down to level %d.",level);
								outs(text);
								newline();
								newline();
								outs("Everything spins around, and suddenly you're elsewhere...");
								newline();
								newline();
								GenLvl();
								ExamineRoom();
							}
							break;
					}
				}
			}
			else
				outs("He says, \"I will pray for you.\"");
			break;
		default:
			outs("How did I get here?");
			n=0;
			break;
	}
	if(n>0 && dice(n)==1) {
		newline();
		newline();
		Delay(25);
		outs("You hear a noise up ahead...");
		Delay(25);
		newline();
		newline();
		outs("Cautiously, you approach to investigate...");
		Delay(25);
		newline();
		newline();
		outs("Suddenly, from out of the darkness, ");
		Delay(25);
		if(dice(10)==1) {
			outs("a bat flies by and");
			newline();
			sprintf(text,"soils your %s!",ARMOR(USER(user)->Armor)->Name);
			outs(text);
			Tshield--;
		}
		else
			DMattack("a monster attacks!");
	}
}

void Dungeon(int StartLevel)
{
	static char *legend = { "EmpCavMonMon???N-SW-E??????TelClr" };

	int i,j,room;
	char c;

	outs("Generating dungeon, please wait...");
	newline();
	newline();
	escape=FALSE;
	level=StartLevel-1;
	if(level<1)
		level=1;
	GenLvl();
	dy=dice(MaxDY)-1;
	dx=dice(MaxDX)-1;
	sprintf(text,"%sNow entering dungeon level %d.",Fcol(GRN),level);
	outs(text);
	normal();
	newline();
	newline();
	ExamineRoom();
	newline();
	newline();

	while(!cd() && !escape) {
		nme=0;
		if(!cd() && Thp<1) {
			outs("You've taken too many hits and die.");
			newline();
			strcpy(reason,"fell apart in the dungeon");
			logoff=TRUE;
		}
		Midnight();
		RPGserver('U',0);
		room=dungeon[dy][dx] & 0x7f;
		if(!cd()) {
			outs(Fcol(CYN));
			if(dy>0 && room!=7)
				outs("<N>orth, ");
			if(dy<MaxDY-1 && room!=7)
				outs("<S>outh, ");
			if(dx<MaxDX-1 && room!=6)
				outs("<E>ast, ");
			if(dx>0 && room!=6)
				outs("<W>est, ");
			if(USER(user)->Spell)
				outs("<C>ast, ");
			if(USER(user)->Poison)
				outs("<P>oison, ");
			if(Tint>49 || map)
				outs("<M>ap, ");
			outs("<Y>our stats: ");
		}
		if(!(c=inkey()))
			break;
		newline();
		newline();
		hy=dy;
		hx=dx;
		switch(c) {
			case 'N':
				if(dy==0) {
					outs("Oof!  There is a wall to the north.");
					Thp--;
					break;
				}
				if(room==7) {
					outs("Oof!  This is an east-west corridor.  You may only go east or west.");
					Thp--;
					break;
				}
				dy--;
				ExamineRoom();
				break;
			case 'S':
				if(dy==MaxDY-1) {
					outs("Oof!  There is a wall to the south.");
					Thp--;
					break;
				}
				if(room==7) {
					outs("Oof!  This is an east-west corridor.  You may only go east or west.");
					Thp--;
					break;
				}
				dy++;
				ExamineRoom();
				break;
			case 'E':
				if(dx==MaxDX-1) {
					outs("Oof!  There is a wall to the east.");
					Thp--;
					break;
				}
				if(room==6) {
					outs("Oof!  This is a north-south corridor.  You may only go north or south.");
					Thp--;
					break;
				}
				dx++;
				ExamineRoom();
				break;
			case 'W':
				if(dx==0) {
					outs("Oof!  There is a wall to the west.");
					Thp--;
					break;
				}
				if(room==6) {
					outs("Oof!  This is a north-south corridor.  You may only go north or south.");
					Thp--;
					break;
				}
				dx--;
				ExamineRoom();
				break;
			case 'C':
				nme=0;
				memset(&NME,0,sizeof(NME));
				i=CastSpell();
				if(i==7)
					Teleport();
				break;
			case 'P':
				PoisonWeapon();
				break;
			case 'M':
				if(!map && Tint<50) {
					outs("You don't have a map of this level, stupid!");
					break;
				}
				cls();
				normal();
				for(i=0; i<MaxDY; i++) {
					if(!map) {
						room=FALSE;
						for(j=0; j<MaxDX; j++)
							if(dungeon[i][j] & 0x80)
								room=TRUE;
						if(!room)
							continue;
					}
					if(i)
						newline();
					for(j=0; j<MaxDX; j++) {
						if(!map && dungeon[i][j]<0x80) {
							outs("     ");
							continue;
						}
						room=dungeon[i][j] & 0x7f;
						if(i==dy && j==dx) {
							reverse();
							outs("-YOU-");
							normal();
							continue;
						}
						if(room>9) {
							outs(Fcol(YELLOW));
							outs("(");
						}
						else
							outs(" ");
						sprintf(text,"%.3s",&legend[3*(room-1)]);
						outs(text);
						if(room>9) {
							outs(")");
							normal();
						}
						else
							outs(" ");
					}
					newline();
				}
				break;
			case 'Y':
				YourStats();
				break;
		}
		newline();
		if(USER(user)->Weapon>0 && (USER(user)->Weapon<=MAXWEAPON ? USER(user)->Weapon : MAXWEAPON/2)+Thit<1) {
			sprintf(text,"Your %s is damaged beyond repair; you toss it aside...",WEAPON(USER(user)->Weapon)->Name);
			outs(text);
			newline();
			USER(user)->Weapon=0;
			PutUser(user);
			Thit=0;
		}
		if(USER(user)->Armor>0 && (USER(user)->Armor<=MAXARMOR ? USER(user)->Armor : MAXARMOR/2)+Tshield<1) {
			sprintf(text,"Your %s is damaged beyond repair; you toss it aside...",ARMOR(USER(user)->Armor)->Name);
			outs(text);
			newline();
			USER(user)->Armor=0;
			PutUser(user);
			Tshield=0;
		}
		newline();
	}
}
