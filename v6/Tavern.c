/*******************\
 *  RPGBBS Tavern  *
\*******************/
#include "RPGBBS.h"

int Punch(int lvl,int tohit,int str)
{
	int power;

	power=lvl+tohit*lvl/20+str/10;
	return(power/2+dice(power/2));
}

void KnockOut(int winner,int loser)
{
	double e;
	int count;

	if(user==winner)
		sprintf(text,"You knock %.26s out!",USER(loser)->Handle);
	else
		sprintf(text,"%.26s knocks %.26s out!",USER(winner)->Handle,(user==loser) ? "you" : USER(loser)->Handle);
	outs(text);
	newline();
	newline();
	modf(pow2((double)(USER(loser)->Level-2))*1000./9.,&e);
	USER(winner)->Experience+=e;
	USER(winner)->Gold+=USER(loser)->Gold;
	USER(winner)->YouKO++;
	PutUser(winner);
	if(user==winner) {
		sprintf(text,"You got %s experience",Fnum(e));
		outs(text);
	}
	else {
		sprintf(text,"%s got %s experience",(USER(winner)->Sex=='M') ? "He" : "She",Fnum(e));
		outs(text);
	}
	if(USER(loser)->Gold) {
		sprintf(text," and %s",money(USER(loser)->Gold,user));
		outs(text);
	}
	outs(".");
	newline();
	newline();
	numline=0;
	sprintf(line[numline++],"%.26s knocked you out.",USER(winner)->Handle);
	sprintf(line[numline++],"%s got %s experience.",(USER(winner)->Sex=='M') ? "He" : "She",Fnum(e));
	if(USER(loser)->Gold)
		sprintf(line[numline++],"%s got %s.",(USER(winner)->Sex=='M') ? "He" : "She",money(USER(loser)->Gold,loser));
	UserNote(loser);
	USER(loser)->Gold=0.;
	USER(loser)->WasKO++;
	PutUser(loser);
	Delay(50);
	if(user==loser) {
		count=dice(5);
		sprintf(text,"You are unconscious for %d minute%s...",count,(count>1) ? "s" : "");
		outs(text);
		AdjustTime(-count);
		Delay(50*count);
		newline();
		newline();
	}
}

void Brawl(int player1,int *bp1,int str1,int agl1,int player2,int *bp2)
{
	int count,damage,standby,sbp;

	count=dice(20+((int)USER(player1)->Level-USER(player2)->Level)/5);
	if((count<20 && dice(100)>agl1) || count==1) {
		if(user==player2)
			sprintf(text,"You duck %s's punch.",USER(player1)->Handle);
		else
			sprintf(text,"%.26s ducks %s punch.",USER(player2)->Handle,(user==player1) ? "your" : (USER(player1)->Sex=='M') ? "his" : "her");
		outs(text);
		newline();
		newline();
		Delay(50);
		count=MAXUSER;
		do {
			standby=dice(MAXUSER-1);
			count--;
		} while((ACCESS(USER(standby)->Access)->User!='Y' || standby==player1 || standby==player2) && count);
		if(!count)
			return;
		if(user==player1)
			sprintf(text,"You hit %.26s!",USER(standby)->Handle);
		else
			sprintf(text,"%.26s hits %.26s!",USER(player1)->Handle,USER(standby)->Handle);
		outs(text);
		newline();
		newline();
		Delay(50);
		sbp=USER(standby)->HP/10-Punch(USER(player1)->Level,CLASS(USER(player1)->Class)->ToHit,str1);
		if(sbp>0) {
			sprintf(text,"Uh oh!  Here comes %.26s!",USER(standby)->Handle);
			outs(text);
			newline();
			newline();
			Delay(50);
			Brawl(standby,&sbp,USER(standby)->Str,USER(standby)->Agl,player1,bp1);
		}
		else {
			KnockOut(player1,standby);
			newline();
		}
	}
	else {
		damage=Punch(USER(player1)->Level,CLASS(USER(player1)->Class)->ToHit,str1);
		if(user==player1)
			sprintf(text,"You punch %.26s for %d points.",USER(player2)->Handle,damage);
		else
			sprintf(text,"%.26s punches %.26s for %d points.",USER(player1)->Handle,(user==player2) ? "you" : USER(player2)->Handle,damage);
		outs(text);
		newline();
		newline();
		*bp2-=damage;
		if(*bp2<1)
			KnockOut(player1,player2);
	}
}

void Tavern(void)
{
	static struct RPGmenu __far RPGmenu = { "Tavern",BRN,9,
							{	{ 'B',"Brawl another user" },
								{ 'E',"Eavesdrop on the arguments" },
								{ 'J',"Jump into the arguments" },
								{ 'G',"Guzzle beer" },
								{ 'L',"List user bounties" },
								{ 'P',"Post your own bounty" },
								{ 'S',"Swear at Tiny" },
								{ 'T',"Today's news" },
								{ 'Y',"Yesterday's news" }
							}
	};
	static bp1,bp2;
	FILE *fp,*fp2;
	double min,max;
	int i,lp,m,take;
	char c,more;

	sprintf(OUTbuf,"    %sWelcome to Tiny's Tavern",Fcol(BRN));
	outs(OUTbuf);
	newline();
	newline();
	while(!cd()) {
		if(USER(user)->Experience>=ExpNeed(USER(user)->Level))
			break;
		if((c=Option(&RPGmenu))=='Q')
			break;

		switch(c) {
			case 'B':
				if(ACCESS(USER(user)->Access)->User!='Y') {
					outs("You are not allowed to brawl.");
					newline();
					break;
				}
				if(brawl>2) {
					outs("You are out of brawls.");
					newline();
					break;
				}
				FOREVER {
					outs(Fcol(CYN));
					outs("Pick a fight with what user (?=list)? ");
					i=GetUser();
					if(i<1)
						i=0;
					nme=i;
					if(!nme)
						break;
					if(nme==user) {
						outs("You want to hit yourself?");
						newline();
						newline();
						continue;
					}
					if(ACCESS(USER(nme)->Access)->User!='Y') {
						outs("That user does not brawl.");
						newline();
						newline();
						continue;
					}
					if((BYTE)(USER(user)->Level-USER(nme)->Level)>3) {
						outs("You can only brawl someone higher or up to three levels below you.");
						newline();
						newline();
						continue;
					}
					NMEstat();
					bp1=Thp/10;
					if(!bp1)
						bp1=1;
					bp2=NMEhp/10;
					sprintf(text,"%sName:%s %-26.26s      You:",Fcol(GRN),Fcol(WHITE),USER(nme)->Handle);
					outs(text);
					newline();
					sprintf(text,"%sLevel:%s %-26d     %-2d",Fcol(GRN),Fcol(WHITE),USER(nme)->Level,USER(user)->Level);
					outs(text);
					newline();
					sprintf(text,"%sKnock out points:%s %-19d %-3d",Fcol(GRN),Fcol(WHITE),bp2,bp1);
					outs(text);
					normal();
					newline();
					newline();
					outs(Fcol(CYN));
					outs("Are you sure (Y/N)? ");
					if(!(c=inkey())) {
						nme=0;
						break;
					}
					newline();
					newline();
					if(c=='Y')
						break;
				}
				if(!nme)
					break;
				brawl++;
				if((dice(100)+Tagl)>(dice(100)+NMEagl)) {
					outs("You get the first punch.");
					newline();
					newline();
					Brawl(user,&bp1,Tstr,Tagl+CLASS(USER(nme)->Class)->DefAgl,nme,&bp2);
					if(bp1>0 && bp2>0)
						Brawl(nme,&bp2,NMEstr,NMEagl+CLASS(USER(user)->Class)->DefAgl,user,&bp1);
				}
				else {
					sprintf(text,"%s gets the first punch.",USER(nme)->Sex=='M' ? "He" : "She");
					outs(text);
					newline();
					newline();
					Brawl(nme,&bp2,NMEstr,NMEagl+CLASS(USER(user)->Class)->DefAgl,user,&bp1);
				}
				while(bp1>0 && bp2>0) {
					sprintf(text,"%s<P> Punch %s, <G> Give it up, <Y> Your status: ",Fcol(CYN),(USER(nme)->Sex=='M') ? "him" : "her");
					outs(text);
					if(!(c=inkey()))
						break;
					newline();
					newline();
					if(c=='G') {
						outs("We can't all be Rocky, eh?");
						newline();
						break;
					}
					if(c=='Y') {
						sprintf(text,"Your knock out points: %d",bp1);
						outs(text);
						newline();
						newline();
						continue;
					}
					if(c!='P')
						continue;
					Brawl(user,&bp1,Tstr,Tagl+CLASS(USER(nme)->Class)->DefAgl,nme,&bp2);
					if(bp1>0 && bp2>0)
						Brawl(nme,&bp2,NMEstr,NMEagl+CLASS(USER(user)->Class)->DefAgl,user,&bp1);
				}
				break;
			case 'E':
				TypeFile("Tavern",TRUE);
				break;
			case 'J':
				if(ACCESS(USER(user)->Access)->User!='Y') {
					outs("The mob yells, \"Get outta here, wimp!\"");
					newline();
					break;
				}
				if(Editor(5)=='S') {
					outs("aving...");
					if((fp=fopen("Tavern.NEW","w"))) {
						if((fp2=fopen("Tavern","r"))) {
							m=0;
							while(!feof(fp2)) {
								inputs(text,sizeof(text),fp2);
								if(strcmp(text,"-=>*<=-")==0)
									m++;
							}
							fclose(fp2);
							fp2=fopen("Tavern","r");
							for(i=9; i<=m;) {
								inputs(text,sizeof(text),fp2);
								if(strcmp(text,"-=>*<=-")==0)
									i++;
							}
							while(!feof(fp2)) {
								fprintf(fp,"%s\n",text);
								inputs(text,sizeof(text),fp2);
							}
							fclose(fp2);
						}
						fputs("-=>*<=-\n\n",fp);
						fprintf(fp,"%.26s says,\n",USER(user)->Handle);
						for(i=0; i<numline; i++)
							fprintf(fp,"  %s\n",line[i]);
						fputs("\n",fp);
						fclose(fp);
						remove("Tavern");
						rename("Tavern.NEW","Tavern");
						outs("Ok.");
					}
					else
						outs("Error!");
					newline();
				}
				break;
			case 'G':
				outs("Tiny, the 7'2\" barkeeper, pours you a beer.");
				newline();
				newline();
				outs(Fcol(CYN));
				outs("How much will you pay? ");
				if(input())
					break;
				newline();
				newline();
				modf(atof(ins),&d);
				if(d<1. || d>USER(user)->Gold) {
					outs("Tiny pours the beer on you and kicks you out of his bar.");
					newline();
					Delay(100);
					return;
				}
				outs("He grunts and hands you your beer.");
				newline();
				USER(user)->Gold-=d;
				PutUser(user);
				outs(Fcol(CYN));
				outs("He says, \"");
				normal();
				i=d;
				if(i>16)
					i=dice(16);
				switch(i) {
					case 1:
						outs("More stamina will yield more hit points");
						break;
					case 2:
						outs("More intellect will yield more spell power");
						break;
					case 3:
						outs("You can sell items for better prices with higher charisma");
						break;
					case 4:
						outs("You don't miss as often with higher agility");
						break;
					case 5:
						outs("You can do more damage in battle with higher stamina");
						break;
					case 6:
						outs("Spells don't fail as often with higher intellect");
						break;
					case 7:
						outs("Higher agility yields higher jousting ability");
						break;
					case 8:
						outs("Fishing can get better results from a lot of luck");
						break;
					case 9:
						outs("Real Estate and Security help protect your investments");
						break;
					case 10:
						outs("Higher baud rates yield faster screen displays");
						break;
					case 11:
						outs("Crying will not change the world");
						break;
					case 12:
						outs("Backstabs swish more than you wish");
						break;
					case 13:
						outs("Dungeon maps fall more into the hands of the lucky");
						break;
					case 14:
						outs("Higher intellect calculates opponent's hit points more accurately");
						break;
					case 15:
						outs("You need at least 50 Intellect points for Magic Mapping");
						break;
					default:
						outs("I'll have more hints tomorrow");
						break;
				}
				normal();
				outs(".");
				outs(Fcol(CYN));
				outs("\"");
				newline();
				break;
			case 'L':
				sprintf(text,"                    %s--=:))%s Tiny's Bounty List %s((:=--%s",Fcol(GRN),Fcol(WHITE),Fcol(GRN),Fcol(GRY));
				outs(text);
				normal();
				newline();
				newline();
				lp=2;
				more='Y';
				for(i=1; i<=MAXUSER && more!='N' && !cd(); i++) {
					c=TRUE;
					if(USER(i)->Access)
						for(m=1; m<=MAXUSER && more!='N'; m++)
							if(USER(m)->BountyWho==i) {
								if(c) {
									if(i==user)
										outs(Fcol(WHITE));
									else
										normal();
									sprintf(text,"%26.26s is worth ",USER(i)->Handle);
									outs(text);
									c=FALSE;
								}
								else
									outs("                                and ");
								sprintf(text,"%s to %.26s.",money(USER(m)->BountyAmount,user),USER(m)->Handle);
								outs(text);
								newline();
								lp++;
								if(lp%22==0)
									more=More();
							}
				}
				if(more!='N' && lp>7 && lp%22)
					Pause();
				normal();
				break;
			case 'P':
				min=1.;
				outs(Fcol(CYN));
				nme=USER(user)->BountyWho;
				if(nme) {
					sprintf(text,"You already have a bounty on %.26s for %s.",USER(nme)->Handle,money(USER(user)->BountyAmount,user));
					outs(text);
					newline();
					newline();
					min=USER(user)->BountyAmount;
				}
				else {
					outs("You clamor for the crowd's attention.");
					newline();
					outs("Post a bounty on what user (?=list)? ");
					i=GetUser();
					newline();
					if(i<1)
						i=0;
					nme=i;
					if(!nme)
						break;
				}
				JOUST(USER(nme)->Level);
				max=WEAPON(USER(nme)->Weapon)->Cost+d;
				sprintf(text,"%sHow much on %.26s's head %s(MAX=%s)? ",Fcol(CYN),USER(nme)->Handle,Fcol(GRY),money(max,user));
				outs(text);
				if(input())
					break;
				newline();
				d=atof(Fnum(atof(ins)));
				c=ins[strlen(ins)-1];
				switch(toupper(c)) {
					case 'C':
						d*=1.;
						break;
					case 'S':
						d*=1e+05;
						break;
					case 'G':
						d*=1e+09;
						break;
					case 'P':
						d*=1e+13;
						break;
				}
				if(stricmp(ins,"MAX")==0)
					d=max;
				if(d>=min && d<=max) {
					outs("Tiny posts your bounty for all to see.");
					newline();
					USER(user)->BountyWho=nme;
					USER(user)->BountyAmount=d;
					PutUser(user);
					Delay(100);
					for(m=0; m<=MAXUSER; m++)
						if(m!=user && USER(m)->Access && ((BYTE)USER(m)->Level-3<=USER(nme)->Level && USER(m)->Level>(BYTE)USER(nme)->Level-10))
							break;
					if(USER(m)->Access) {
						sprintf(text,"%.26s nods %s head and ",USER(m)->Handle,(USER(m)->Sex=='M') ? "his" : (USER(m)->Sex=='F') ? "her" : "its");
						outs(text);
						switch(dice(5)) {
							case 1:
								sprintf(text,"grins at you.");
								break;
							case 2:
								sprintf(text,"raises %s %s.",(USER(m)->Sex=='M') ? "his" : (USER(m)->Sex=='F') ? "her" : "its",WEAPON(USER(m)->Weapon)->Name);
								break;
							case 3:
								sprintf(text,"falls asleep.");
								break;
							case 4:
								sprintf(text,"strokes %s chin.",(USER(m)->Sex=='M') ? "his" : (USER(m)->Sex=='F') ? "her" : "its");
								break;
							case 5:
								sprintf(text,"cries, \"Death to %.26s!\"",USER(nme)->Handle);
								break;
						}
						outs(text);
						newline();
						Delay(100);
					}
				}
				else {
					outs("The crowd laughs, then they toss you out on the street.");
					newline();
					Delay(100);
					return;
				}
				break;
			case 'S':
				if(!acclvl)
					break;
				cuss++;
				outs("You call to Tiny, then ");
				if(cuss==1) {
					outs("yell, \"Freak!\"");
					newline();
					newline();
					Delay(100);
					switch(dice(2)) {
						case 1:
							outs("Tiny stares off into empty space, ignoring your wimpy comment.");
							break;
						case 2:
							outs("Tiny points at his massive, flexed bicep and laughs at your jest.");
							break;
					}
					newline();
					break;
				}
				if(cuss==2) {
					outs("thumb your nose.");
					newline();
					newline();
					Delay(100);
					switch(dice(2)) {
						case 1:
							outs("Annoyed, Tiny looks down at his furry feet and counts, \"100, 99, 98,...\"");
							break;
						case 2:
							outs("The once Champion Ogre glances you over and murmurs to himself, \"Not good for");
							newline();
							outs("business.\"");
							break;
					}
					newline();
					break;
				}
				outs("drop your trousers.");
				newline();
				if(!cd())
					Delay(100);
				outs("Uh, oh!");
				if(!cd())
					Delay(50);
				outs("  Here comes Tiny!");
				if(!cd())
					Delay(50);
				outs("  And he doesn't look friendly...");
				newline();
				newline();
				if(!cd())
					Delay(50);
				switch(dice(2)) {
					case 1:
						outs("\"I have had enough of you and your bullshit, punk!\"");
						break;
					case 2:
						outs("He lets off a heavy sigh and says, \"I am getting too old for this.\"");
						break;
				}
				newline();
				newline();
				if(!cd())
					Delay(100);
				outs("As he towels his hands dry from washing the day's glasses, he warns, \"Another");
				newline();
				outs("fool did something like that to me and got all busted up... I think it was");
				newline();
				sprintf(text,"%.26s, and it took me a week to clean up the blood!\"",USER(USER(0)->Status)->Handle);
				outs(text);
				newline();
				newline();
				if(!cd())
					Delay(300);
				outs("He points to a weapon hanging over the mantlepiece and says, \"Lookee there, he");
				newline();
				sprintf(text,"tried to use that %s on me, but it wasn't good enough.\"",WEAPON(USER(0)->BountyWho)->Name);
				outs(text);
				newline();
				newline();
				if(!cd())
					Delay(300);
				for(i=1; i<=MAXUSER; i++) {
					while((nme=dice(MAXUSER))==user);
					if(USER(nme)->Class)
						break;
				}
				if(!USER(nme)->Class)
					for(nme=MAXUSER; nme; nme--)
						if(nme!=user && USER(nme)->Class)
							break;
				outs("The patrons move in closer to witness the forthcoming slaughter, except for");
				newline();
				sprintf(text,"%.26s who is busy raiding the bar of its beer and nuts.",USER(nme)->Handle);
				outs(text);
				newline();
				newline();
				if(!cd())
					Delay(225);
				sprintf(text,"You hear %.26s cry, \"I'll pay ten-to-one for the challenger!\"",USER(0)->Handle);
				outs(text);
				newline();
				newline();
				if(!cd())
					Delay(150);
				outs("The crowd roars with laughter...");
				if(!cd())
					Delay(75);
				outs(" you are not amused.");
				newline();
				newline();
				if(!cd())
					Delay(75);
				outs("When Tiny removes his tunic to reveal a massive, but heavily scarred chest,");
				newline();
				if((USER(user)->Spell & (UWORD)pow2(6.)) && (CLASS(USER(user)->Class)->Magic==1 || (CLASS(USER(user)->Class)->Magic>1 && Tsp>=MAGIC(6)->Power[(CLASS(USER(user)->Class)->Magic==4) ? 1 : 0]))) {
					switch(CLASS(USER(user)->Class)->Magic) {
						case 1:
							outs("you start fumbling through your pockets for that Teleport Wand...");
							break;
						case 2:
							outs("you unroll a parchment titled, \"Teleportation is the safest way to travel.\"");
							break;
						default:
							outs("you try to recall the words for Teleportation...");
							break;
					}
					newline();
					newline();
					if(!cd())
						Delay(300);
					outs("Attempt to Teleport (Y/N)? ");
					if(!(c=inkey()))
						break;
					newline();
					newline();
					if(c=='Y') {
						if(CLASS(USER(user)->Class)->Magic==1 && dice(100+2*(10))<100) {
							outs("Your wand smokes as you cast the spell.");
							newline();
							newline();
							USER(user)->Spell-=pow2(6.);
							PutUser(user);
							Delay(25);
						}
						if(CLASS(USER(user)->Class)->Magic==2 && dice(9-CLASS(USER(user)->Class)->ToCast)==1) {
							outs("Your scroll burns as you cast the spell.");
							newline();
							newline();
							USER(user)->Spell-=pow2(6.);
							PutUser(user);
							Delay(25);
						}
						if(dice(100)>(Tint+CLASS(USER(user)->Class)->ToInt)) {
							outs("Fssst!  Your spell fails!");
							newline();
							newline();
							Delay(25);
						}
						else
							break;
					}
				}
				else {
					outs("you look for an exit, but there is none to be found...");
					newline();
					newline();
					if(!cd())
						Delay(300);
				}
				outs("Tiny puts a finger in your face and says, ");
				switch(dice(3)) {
					case 1:
						outs("\"Want a shot at the title?\"");
						break;
					case 2:
						outs("\"Better drop me on the first shot.\"");
						break;
					case 3:
						outs("\"Let's see what you got!\"");
						break;
				}
				newline();
				newline();
				if(!cd())
					Delay(200);
				nme=0;
				memset(&NME,0,sizeof(struct User));
				strcpy(NME.Handle,"Tiny, the 7'2\" barkeeper");
				NME.Sex='M';
				NME.Class=MAXCLASS-3;
				NME.Level=MAXLEVEL;
				NME.ExpLevel=MAXLEVEL*4/5;
				NME.HP=3583;
				NME.SP=5000;
				NME.Str=100;
				NME.Int=100;
				NME.Weapon=-MAXWEAPON;
				NME.Spell=1024 | 128;
				NME.Gold=1.;
				from='T';
				Battle();
				if(NMEhp) {
					USER(0)->Status=user;
					USER(0)->BountyWho=USER(user)->Weapon;
					USER(user)->Weapon=0;
				}
				else {
					take=(USER(user)->Weapon<USER(0)->BountyWho);
					if(USER(user)->Weapon>MAXWEAPON && USER(0)->BountyWho>=MAXWEAPON)
						take=(USER(0)->BountyWho==MAXWEAPON || (take && USER(0)->BountyWho>MAXWEAPON+MAXCLASS-3));
					if(take) {
						sprintf(text,"You take his %s from the mantlepiece.",WEAPON(USER(0)->BountyWho)->Name);
						outs(text);
						newline();
						m=USER(0)->BountyWho;
						USER(0)->Status=0;
						USER(0)->BountyWho=USER(user)->Weapon;
						USER(user)->Weapon=m;
						Thit=(USER(user)->Blessed) ? USER(user)->Weapon/10 : (USER(user)->Cursed) ? -USER(user)->Weapon/10 : 0;
					}
				}
				PutUser(0);
				PutUser(user);
				return;
				break;
			case 'T':
				sprintf(filename,"News.%lu",JulianDate);
				TypeFile(filename,TRUE);
				break;
			case 'Y':
				sprintf(filename,"News.%lu",JulianDate-1);
				TypeFile(filename,TRUE);
				break;
		}
	}
}
