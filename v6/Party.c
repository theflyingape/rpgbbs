/******************\
 *  RPGBBS Party  *
\******************/
#include "RPGBBS.h"
#define ATTACKER g[gi[x]][gj[x]]
#define DEFENDER g[i][j]
#define LOSER GANG(loser)->Member[j]
#define WINNER GANG(winner)->Member[j]
#define TOCAST (4-CLASS(USER(Offender)->Class)->Magic)

void GangName(int n)
{
	static char bg[] = { RED,BRN,GRN,CYN,BLU,MAG };
	static char fg[] = { WHITE,BLK,YELLOW,BLU,WHITE,YELLOW };
	int b,f,i;

	if(n) {
		i=(n-1)%6;
		b=bg[i];
		f=fg[i];
	}
	else {		/*  Monster Mash Banner colors  */
		b=GRN;
		f=BLK;
	}
		
	sprintf(text,"%s-=[%s%s %.20s %s%s]=-",Fcol(b),Bcol(b),Fcol(f),GANG(n)->Name,Bcol(BLK),Fcol(b));
	outs(text);
	normal();
}

void GangSwing(int Offender,int *Ohp,int *Osp,int *Ostr,int *Oint,int *Oagl,int *Ohit,int Defender,int *Dhp,int *Dint,int *Dagl,int *Dhit,int *Dshield)
{
	int i,mu,spell,result;

	memset(OUTbuf,0,sizeof(OUTbuf));
	switch(USER(Offender)->Sex) {
		case 'M':
			NMEhe="he";
			break;
		case 'F':
			NMEhe="she";
			break;
		default:
			NMEhe="it";
			break;
	}
	mu=(CLASS(USER(Offender)->Class)->Magic==4) ? 1 : 0;
	if(CLASS(USER(Offender)->Class)->Magic==1)
		*Osp=65535;
	spell=0;
	if(!spell && (USER(Offender)->Spell & 128) && *Ohp<USER(Offender)->HP/2 && *Osp>=MAGIC(7)->Power[mu])
		spell=(dice(CLASS(USER(Offender)->Class)->ToCast+TOCAST)==1) ? 8 : 0;
	if(!spell && (USER(Offender)->Spell & 256) && *Osp>=MAGIC(8)->Power[mu])
		spell=(dice(CLASS(USER(Offender)->Class)->ToCast+TOCAST)==1) ? 9 : 0;
	if(!spell && (USER(Offender)->Spell & 1024) && *Ohp<USER(Offender)->HP/8 && *Osp>=MAGIC(10)->Power[mu])
		spell=11;
	if(!spell && (USER(Offender)->Spell & 2048) && *Osp>=MAGIC(11)->Power[mu])
		spell=(dice(5*CLASS(USER(Offender)->Class)->ToCast+TOCAST)==1) ? 12 : 0;
	if(!spell && (USER(Offender)->Spell & 4096) && *Osp>=MAGIC(12)->Power[mu])
		spell=(dice(CLASS(USER(Offender)->Class)->ToCast+TOCAST)==1) ? 13 : 0;
	if(!spell && (USER(Offender)->Spell & 8192) && *Osp>=MAGIC(13)->Power[mu] && (USER(Defender)->Weapon>0 || USER(Defender)->Armor>0))
		spell=(dice(5*CLASS(USER(Offender)->Class)->ToCast+TOCAST)==1) ? 14 : 0;
	if(spell) {
		*Osp-=MAGIC(spell-1)->Power[mu];
		if(CLASS(USER(Offender)->Class)->Magic==1 && dice(100+2*(17-spell))<100) {
			sprintf(OUTbuf,"'s wand smokes as %s",NMEhe);
			USER(Offender)->Spell-=pow2((double)spell-1.);
			PutUser(Offender);
		}
		if(CLASS(USER(Offender)->Class)->Magic==2 && dice((20-2*CLASS(USER(Offender)->Class)->ToCast)*(CLASS(USER(Defender)->Class)->ToHit>3 ? 2 : 1))==1) {
			sprintf(OUTbuf,"'s scroll burns as %s",NMEhe);
			USER(Offender)->Spell-=pow2((double)spell-1.);
			PutUser(Offender);
		}
		if(dice(100)>(*Oint+CLASS(USER(Offender)->Class)->ToInt+((spell<9 || spell==11) ? 0 : 2*CLASS(USER(Defender)->Class)->DefInt))) {
			strcat(OUTbuf," casts a spell.  Fssst!");
			return;
		}
		result=0;
		switch(spell) {
			case 8:
				for(i=1; i<USER(Offender)->Level; i++)
					result+=dice(CLASS(USER(Offender)->Class)->ToHeal);
				if(*Ohp+result>USER(Offender)->HP)
					result=USER(Offender)->HP-*Ohp;
				sprintf(text," heals %sself for %d hit points!",NMEhim,result);
				strcat(OUTbuf,text);
				*Ohp+=result;
				break;
			case 9:
				for(i=1; i<USER(Offender)->Level; i++)
					result+=dice(CLASS(USER(Offender)->Class)->ToBlast*(CLASS(USER(Defender)->Class)->ToHit>3 ? 2 : 1));
				sprintf(text," blasts %.26s for %d hit points!",USER(Defender)->Handle,result);
				strcat(OUTbuf,text);
				*Dhp-=result;
				if(*Dhp<1)
					strcat(OUTbuf," {R.I.P.}");
				break;
			case 11:
				sprintf(text," cures %sself!",NMEhim);
				strcat(OUTbuf,text);
				*Ohp=USER(Offender)->HP;
				break;
			case 12:
				sprintf(text," disintegrates %.26s!",USER(Defender)->Handle);
				strcat(OUTbuf,text);
				*Dhp=0;
				break;
			case 13:
				sprintf(text," blitzes %.26s with exploding %sc%so%sl%so%sr%ss%s!",USER(Defender)->Handle,Fcol(BRED),Fcol(YELLOW),Fcol(GREEN),Fcol(CYAN),Fcol(BLUE),Fcol(MAGENTA),Fcol(GRY));
				strcat(OUTbuf,text);
				*Dagl/=2;
				*Dint/=2;
				break;
			case 14:
				numline=0;
				sprintf(line[numline],"During a gang battle, %.26s ",USER(Offender)->Handle);
				if(USER(Defender)->Weapon>=USER(Defender)->Armor) {
					sprintf(text," transforms %.26s's %s",USER(Defender)->Handle,WEAPON(USER(Defender)->Weapon)->Name);
					strcat(OUTbuf,text);
					sprintf(text,"transformed your %s",WEAPON(USER(Defender)->Weapon)->Name);
					strcat(line[numline],text);
					USER(Defender)->Weapon=dice(USER(Defender)->Weapon)-1;
					*Dhit=0;
					sprintf(text," into %s %s!",strchr("AEIOU",WEAPON(USER(Defender)->Weapon)->Name[0]) ? "an": "a",WEAPON(USER(Defender)->Weapon)->Name);
					strcat(OUTbuf,text);
					strcat(line[numline],text);
				}
				else {
					sprintf(text," transforms %.26s's %s",USER(Defender)->Handle,ARMOR(USER(Defender)->Armor)->Name);
					strcat(OUTbuf,text);
					sprintf(text,"transformed your %s",ARMOR(USER(Defender)->Armor)->Name);
					strcat(line[numline],text);
					USER(Defender)->Armor=dice(USER(Defender)->Armor)-1;
					*Dshield=0;
					sprintf(text," into %s %s!",strchr("AEIOU",ARMOR(USER(Defender)->Armor)->Name[0]) ? "an": "a",ARMOR(USER(Defender)->Armor)->Name);
					strcat(OUTbuf,text);
					strcat(line[numline],text);
				}
				PutUser(Defender);
				numline++;
				UserNote(Defender);
				break;
		}
	}
	else {
		if(dice(100)>(50+*Oagl/2+CLASS(USER(Defender)->Class)->DefAgl))
			sprintf(OUTbuf," attacks %.26s, but misses.",USER(Defender)->Handle);
		else {
			result=Attack(USER(Offender)->Class,USER(Offender)->Level,USER(Offender)->Weapon,*Ohit,*Ostr,USER(Defender)->Class,USER(Defender)->Armor,*Dshield,(double)*Ohp/USER(Offender)->HP);
			if(result) {
				*Dhp-=result;
				if(modifier)
					sprintf(OUTbuf," smashes %.26s for %d hit points!",USER(Defender)->Handle,result);
				else
					sprintf(OUTbuf," hits %.26s for %d hit points.",USER(Defender)->Handle,result);
				if(*Dhp<1)
					switch(dice(5)) {
						case 1:
							sprintf(OUTbuf,"'s %s makes a fatal blow to %.26s.",WEAPON(USER(Offender)->Weapon)->Name,USER(Defender)->Handle);
							break;
						case 2:
							sprintf(OUTbuf," blows %.26s away.",USER(Defender)->Handle);
							break;
						case 3:
							sprintf(OUTbuf," laughs, then kills %.26s.",USER(Defender)->Handle);
							break;
						case 4:
							sprintf(OUTbuf," easily slays %.26s.",USER(Defender)->Handle);
							break;
						case 5:
							sprintf(OUTbuf,"'s %s makes minced-meat out of %.26s.",WEAPON(USER(Offender)->Weapon)->Name,USER(Defender)->Handle);
							break;
					}
			}
			else
				sprintf(OUTbuf,"'s %s does not even scratch %.26s.",WEAPON(USER(Offender)->Weapon)->Name,USER(Defender)->Handle);
		}
	}
	return;
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
	static UWORD v[MAXGANG+2];
	static int sub[MAXGANG+2];

	double gold,divisor,specie,experience,totg,tote,tll,tlw;
	ULONG age;
	int g[2][4],gi[8],gj[8],gl[2],ghp[2][4],gsp[2][4],ghit[2][4],gshield[2][4],gstr[2][4],gint[2][4],gagl[2][4];
	int YourGang,YourMember;
	int a,active,i,j,k,loser,lp,volley,winner,x;
	char c,more;

	YourGang=USER(user)->Gang;
	YourMember=-1;
	if(YourGang)
		for(i=0; i<4; i++)
			if(GANG(YourGang)->Member[i]==user)
				YourMember=i;

	while(!cd()) {
		if(USER(user)->Experience>=ExpNeed(USER(user)->Level)) {
			Delay(25);
			newline();
			newline();
			break;
		}
		if((c=Option(&RPGmenu))=='Q')
			break;
		switch(c) {
			case 'E':
				if(YourMember!=0) {
					outs("Only your gang leader may do this.");
					newline();
					break;
				}
				GangName(YourGang);
				newline();
				newline();
				for(i=0; i<4; i++) {
					sprintf(text,"%s %.26s",bracket(i+1),(GANG(YourGang)->Member[i]) ? USER(abs(GANG(YourGang)->Member[i]))->Handle : "(Any player may join)");
					outs(text);
					if(i==0)
						outs(" (leader)");
					if(GANG(YourGang)->Member[i]<0)
						outs(" (invited)");
					newline();
				}
				newline();
				outs(Fcol(CYN));
				outs("Replace which member (2-4)? ");
				if(input())
					break;
				i=atoi(ins);
				if(i<2 || i>4) {
					newline();
					break;
				}
				i--;
				sprintf(text," - %.26s",(GANG(YourGang)->Member[i]) ? USER(abs(GANG(YourGang)->Member[i]))->Handle : "(anybody)");
				outs(text);
				newline();
				outs("Invite who (*=anybody,?=List)? ");
				x=GetUser();
				if(x==0)
					break;
				if(x==-1) {
					if(GANG(YourGang)->Member[i]>0) {
						USER(GANG(YourGang)->Member[i])->Gang=0;
						PutUser(GANG(YourGang)->Member[i]);
					}
					GANG(YourGang)->Member[i]=0;
				}
				if(x>0) {
					c=FALSE;
					for(j=0; j<4; j++)
						if(x==abs(GANG(YourGang)->Member[j]))
							c=TRUE;
					if(c) {
						outs("I don't think so!");
						newline();
						break;
					}
					if(GANG(YourGang)->Member[i]>0) {
						USER(GANG(YourGang)->Member[i])->Gang=0;
						PutUser(GANG(YourGang)->Member[i]);
					}
					GANG(YourGang)->Member[i]=-x;
					numline=0;
					sprintf(line[numline++],"You have been invited to join the gang, %.20s.",GANG(YourGang)->Name);
					UserNote(x);
				}
				PutGang(YourGang);
				break;
			case 'F':
				if(!YourGang) {
					outs("You are not a member of any gang!");
					newline();
					break;
				}
				if(party) {
					outs("You have already used your gang fight this call.");
					newline();
					break;
				}
				active=TRUE;
				for(i=0; i<4; i++)
					if(GANG(YourGang)->Member[i]<1)
						active=FALSE;
				if(!active) {
					outs("Your gang is not active yet!");
					newline();
					break;
				}
				gl[0]=4;
				for(j=0; j<4; j++) {
					g[0][j]=GANG(YourGang)->Member[j];
					ghp[0][j]=USER(g[0][j])->HP;
					gsp[0][j]=USER(g[0][j])->SP;
					gstr[0][j]=USER(g[0][j])->Str;
					gint[0][j]=USER(g[0][j])->Int;
					gagl[0][j]=USER(g[0][j])->Agl;
					ghit[0][j]=0;
					gshield[0][j]=0;
					if(USER(g[0][j])->Blessed) {
						ghit[0][j]=USER(g[0][j])->Weapon/10;
						gshield[0][j]=USER(g[0][j])->Armor/10;
						gstr[0][j]+=10;
						if(gstr[0][j]>100)
							gstr[0][j]=100;
						gint[0][j]+=10;
						if(gint[0][j]>100)
							gint[0][j]=100;
						gagl[0][j]+=10;
						if(gagl[0][j]>100)
							gagl[0][j]=100;
					}
					if(USER(g[0][j])->Cursed) {
						ghit[0][j]=-USER(g[0][j])->Weapon/10;
						gshield[0][j]=-USER(g[0][j])->Armor/10;
						gstr[0][j]-=10;
						gint[0][j]-=10;
						gagl[0][j]-=10;
					}
				}
				sprintf(text," %s<%sM%s>%s %.20s",Fcol(BLU),Fcol(WHITE),Fcol(BLU),Fcol(GRY),GANG(0)->Name);
				outs(text);
				newline();
				for(i=1; i<=MAXGANG; i++)
					if(strlen(GANG(i)->Name) && i!=YourGang) {
						sprintf(text,"%s %.20s",bracket(i),GANG(i)->Name);
						outs(text);
						newline();
					}
				newline();
				outs(Fcol(CYN));
				outs("Fight which gang: ");
				if(input())
					break;
				newline();
				c=toupper(ins[0]);
				nme=atoi(ins);
				if((nme<1 || nme>MAXGANG) && c!='M')
					break;
				if(!strlen(GANG(nme)->Name))
					break;
				newline();
				if(nme==YourGang) {
					outs("They're just a bunch of wimps!");
					newline();
					break;
				}
				GangName(nme);
				newline();
				newline();
				if(nme)
					for(i=0; i<4; i++) {
						g[1][i]=GANG(nme)->Member[i];
						if(GANG(nme)->Member[i]<1) {
							active=FALSE;
							break;
						}
						if(i)
							sprintf(text,"         %.26s",USER(g[1][i])->Handle);
						else
							sprintf(text,"Members: %.26s (leader)",USER(g[1][i])->Handle);
						outs(text);
						newline();
						if(RPGserver('L',g[1][i])) {
							active=FALSE;
							break;
						}
					}
				if(c=='M')
					for(i=0; i<4; i++) {
						GANG(nme)->Member[i]=MAXUSER+i+1;
						g[1][i]=GANG(nme)->Member[i];
						if(RPGserver('L',g[1][i])) {
							active=FALSE;
							break;
						}
						memset(USER(g[1][i]),0,sizeof(struct User));
						j=USER(g[0][i])->Level+dice(6)-3;
						if(j<1)
							j=1;
						CreateMonster(USER(g[1][i]),USER(g[0][i])->Class,j);
						strcpy(USER(g[1][i])->Handle,DUNGEON(USER(g[1][i])->Level)->Name);
						ToUpLow(USER(g[1][i])->Handle);
						USER(g[1][i])->Weapon=abs(USER(g[1][i])->Weapon);
						USER(g[1][i])->Armor=abs(USER(g[1][i])->Armor);
						USER(g[1][i])->Sex='I';
						USER(g[1][i])->Str=90+USER(g[1][i])->Level/10;
						USER(g[1][i])->Int=90+USER(g[1][i])->Level/10;
						USER(g[1][i])->Agl=90+USER(g[1][i])->Level/10;
						modf(pow2((double)(j-1)/2.)*dice(10),&USER(g[1][i])->Gold);
						if(i)
							sprintf(text,"         %.26s",USER(g[1][i])->Handle);
						else
							sprintf(text,"Members: %.26s (leader)",USER(g[1][i])->Handle);
						outs(text);
						newline();
					}
				if(!active) {
					newline();
					outs("Their gang is not active yet!");
					newline();
					break;
				}
				gl[1]=4;
				for(j=0; j<4; j++) {
					ghp[1][j]=USER(g[1][j])->HP;
					gsp[1][j]=USER(g[1][j])->SP;
					gstr[1][j]=USER(g[1][j])->Str;
					gint[1][j]=USER(g[1][j])->Int;
					gagl[1][j]=USER(g[1][j])->Agl;
					ghit[1][j]=0;
					gshield[1][j]=0;
					if(USER(g[1][j])->Blessed) {
						ghit[1][j]=USER(g[1][j])->Weapon/10;
						gshield[1][j]=USER(g[1][j])->Armor/10;
						gstr[1][j]+=10;
						if(gstr[1][j]>100)
							gstr[1][j]=100;
						gint[1][j]+=10;
						if(gint[1][j]>100)
							gint[1][j]=100;
						gagl[1][j]+=10;
						if(gagl[1][j]>100)
							gagl[1][j]=100;
					}
					if(USER(g[1][j])->Cursed) {
						ghit[1][j]=-USER(g[1][j])->Weapon/10;
						gshield[1][j]=-USER(g[1][j])->Armor/10;
						gstr[1][j]-=10;
						gint[1][j]-=10;
						gagl[1][j]-=10;
					}
				}
				newline();
				outs(Fcol(CYN));
				outs("Are you sure (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				if(c!='Y')
					break;
				party=TRUE;
				newline();
				outs("The two parties advance cautiously towards each other.");
				newline();
				newline();
				Delay(50);
				if(USER(g[1][0])->Weapon)
					sprintf(text,"%.26s grins as %s pulls out %s %s.",USER(g[1][0])->Handle,(USER(g[1][0])->Sex=='M') ? "he" : (USER(g[1][0])->Sex=='F') ? "she" : "it",(USER(g[1][0])->Sex=='M') ? "his" : (USER(g[1][0])->Sex=='F') ? "her" : "its",WEAPON(USER(g[1][0])->Weapon)->Name);
				else
					sprintf(text,"%.26s quivers as %s stares at %s empty %s!",USER(g[1][0])->Handle,(USER(g[1][0])->Sex=='M') ? "he" : (USER(g[1][0])->Sex=='F') ? "she" : "it",(USER(g[1][0])->Sex=='M') ? "his" : (USER(g[1][0])->Sex=='F') ? "her" : "it",WEAPON(USER(g[1][0])->Weapon)->Name);
				outs(text);
				newline();
				Delay(50);
				for(volley=0; gl[0]>0 && gl[1]>0; volley++) {
					newline();
					x=0;
					for(i=0; i<2; i++)
						for(j=0; j<4; j++) {
							gi[x]=i;
							gj[x++]=j;
						}
					for(x=0; x<8; x++) {
						a=dice(8)-1;
						i=gi[x];
						gi[x]=gi[a];
						gi[a]=i;
						j=gj[x];
						gj[x]=gj[a];
						gj[a]=j;
					}
					for(x=0; x<8 && gl[0]>0 && gl[1]>0; x++) {
						if(ghp[gi[x]][gj[x]]<1)
							continue;
						if(gi[x]) {
							outs(Fcol(CYN));
							outs("  ");
						}
						else
							outs(Fcol(WHITE));
						sprintf(text,"%.26s",USER(ATTACKER)->Handle);
						outs(text);
						if(!volley && USER(ATTACKER)->Weapon && ghp[gi[x]][gj[x]]>USER(ATTACKER)->HP/2 && USER(ATTACKER)->Poison && dice(100)<=gint[gi[x]][gj[x]]/10*abs(CLASS(USER(ATTACKER)->Class)->Poison)) {
							do {
								a=dice(MAXPOISON)-1;
							} while((USER(ATTACKER)->Poison & (UWORD)pow2((double)a))==0);
							ghit[gi[x]][gj[x]]+=abs(CLASS(USER(ATTACKER)->Class)->Poison)*POISON(a)->Power[(CLASS(USER(ATTACKER)->Class)->Poison==-1) ? 1 : 0];
							if(ghit[gi[x]][gj[x]]>abs(USER(ATTACKER)->Weapon))
								ghit[gi[x]][gj[x]]=abs(USER(ATTACKER)->Weapon);
							sprintf(text," loads up %s %s instead.",(USER(ATTACKER)->Sex=='M') ? "his" : (USER(ATTACKER)->Sex=='F') ? "her" : "its",WEAPON(abs(USER(ATTACKER)->Weapon))->Name);
							outs(text);
							newline();
							continue;
						}
						i=gi[x]^1;
						do {
							j=dice(4)-1;
						} while(ghp[i][j]<1);
						GangSwing(ATTACKER,&ghp[gi[x]][gj[x]],&gsp[gi[x]][gj[x]],&gstr[gi[x]][gj[x]],&gint[gi[x]][gj[x]],&gagl[gi[x]][gj[x]],&ghit[gi[x]][gj[x]],DEFENDER,&ghp[i][j],&gint[i][j],&gagl[i][j],&ghit[i][j],&gshield[i][j]);
						outs(OUTbuf);
						newline();
						if(ghp[i][j]<1)
							gl[i]--;
					}
					newline();
					c='\0';
					while(gl[0]>0 && gl[1]>0) {
						outs(Fcol(CYN));
						outs("<C> Continue the slaughter, <R> Run like hell, <Y> Your gang's status: ");
						if(!(c=inkey()))
							break;
						newline();
						newline();
						if(c=='C' || c=='R')
							break;
						if(c=='Y') {
							for(j=0; j<4; j++) {
								if(ghp[0][j]>0)
									sprintf(text,"%.26s has %d hit points.",USER(g[0][j])->Handle,ghp[0][j]);
								else
									sprintf(text,"%.26s is dead.",USER(g[0][j])->Handle);
								outs(text);
								newline();
							}
							newline();
						}
					}
					if(c=='C')
						continue;
					if(c=='R') {
						sprintf(text,"%.20s has become known as a bunch of cowards.",GANG(YourGang)->Name);
						outs(text);
						newline();
						newline();
						numline=0;
						sprintf(line[numline++],"%.20s ran like hell from %.20s.",GANG(YourGang)->Name,GANG(nme)->Name);
						for(i=0; i<2; i++)
							for(j=0; j<4; j++)
								UserNote(g[i][j]);
						break;
					}
				}
				if(gl[0]>0 && gl[1]>0) {
					GANG(nme)->Win++;
					PutGang(nme);
					GANG(YourGang)->Loss++;
					PutGang(YourGang);
					for(j=0; j<4; j++) {
						if(ghp[1][j]>0) {
							if(ghp[1][j]<USER(g[1][j])->HP/4)
								strcpy(text,"limping");
							else
							if(ghp[1][j]<USER(g[1][j])->HP/2)
								strcpy(text,"relieved");
							else
								strcpy(text,"laughing");
							sprintf(OUTbuf,"%.26s walks away %s with %d hit points left.",USER(g[1][j])->Handle,text,ghp[1][j]);
							outs(OUTbuf);
							newline();
						}
					}
					break;
				}
				if(gl[0]==0) {
					sprintf(text,"Your gang has been annihilated!",GANG(YourGang)->Name);
					outs(text);
					newline();
					numline=0;
					sprintf(reason,"defeated by %.20s",GANG(nme)->Name);
					sprintf(line[numline++],"%.20s were %s, initiated by %.26s.",GANG(YourGang)->Name,reason,USER(user)->Handle);
					winner=nme;
					loser=YourGang;
					LogLoser(GANG(nme)->Name,"party");
					logoff=TRUE;
					RPGmessage(line[0]);
				}
				if(gl[1]==0) {
					sprintf(text,"%.20s are annihilated!",GANG(nme)->Name);
					outs(text);
					newline();
					numline=0;
					winner=YourGang;
					loser=nme;
					sprintf(line[numline++],"%.20s defeated %.20s, initiated by %.26s.",GANG(YourGang)->Name,GANG(nme)->Name,USER(user)->Handle);
					RPGmessage(line[0]);
				}
				GANG(winner)->Win++;
				PutGang(winner);
				GANG(loser)->Loss++;
				PutGang(loser);
				gold=0.;
				divisor=0.;
				tll=0.;
				tlw=0.;
				totg=0.;
				tote=0.;
				numline=2;
				for(j=0; j<4; j++) {
					i=USER(WINNER)->ExpLevel;
					divisor+=pow(2.5,i/3.);
					gold+=USER(LOSER)->Gold;
					USER(LOSER)->Gold=0.;
					PutUser(LOSER);
					i=(int)USER(LOSER)->ExpLevel;
					tll+=i;
					i=(int)USER(WINNER)->ExpLevel;
					tlw+=i;
				}
				for(j=0; j<4; j++) {
					i=USER(WINNER)->ExpLevel;
					modf(pow2((double)(i-2))*1000.*tll/tlw,&experience);
					i=USER(WINNER)->ExpLevel;
					modf(gold*pow(2.5,(double)i/3.)/divisor,&specie);
					totg+=specie;
					tote+=experience;
					USER(WINNER)->Gold+=specie;
					USER(WINNER)->Experience+=experience;
					PutUser(WINNER);
					sprintf(text,"You got %s experience and %s.",Fnum(experience),money(specie,WINNER));
					if(user==WINNER) {
						outs(text);
						newline();
						if(USER(user)->Experience>=ExpNeed(USER(user)->Level))
							outs("Hang on...");
						else
							newline();
					}
					else {
						strcpy(line[1],text);
						UserNote(WINNER);
					}
				}
				for(j=0; j<4; j++) {
					sprintf(text,"They got %s experience and %s.",Fnum(tote),money(totg,LOSER));
					if(user==LOSER) {
						outs(text);
						newline();
					}
					else {
						strcpy(line[1],text);
						UserNote(LOSER);
					}
				}
				break;
			case 'J':
				if(party) {
					outs("Now why would I let you join a gang when I let you fight before?");
					newline();
					break;
				}
				if(YourGang) {
					sprintf(text,"You are already a member of %.20s.",GANG(YourGang)->Name);
					outs(text);
					newline();
					break;
				}
				if(ACCESS(USER(user)->Access)->User!='Y') {
					sprintf(text,"Nobody wants to party with a puny %s.",ACCESS(USER(user)->Access)->Name);
					outs(text);
					newline();
					break;
				}
				outs("Gangs you can join:");
				newline();
				newline();
				for(i=1; i<=MAXGANG; i++) {
					if(strlen(GANG(i)->Name)) {
						active=FALSE;
						for(j=1; j<4; j++)
							if(user==abs(GANG(i)->Member[j])) {
								sprintf(text,"%s %.20s (invited)",bracket(i),GANG(i)->Name);
								outs(text);
								newline();
								active=TRUE;
								break;
							}
						if(!active)
							for(j=1; j<4; j++)
								if(!GANG(i)->Member[j]) {
									sprintf(text,"%s %.20s (anybody)",bracket(i),GANG(i)->Name);
									outs(text);
									newline();
									break;
								}
					}
				}
				newline();
				outs(Fcol(CYN));
				outs("Enter which number? ");
				if(input())
					break;
				newline();
				i=atoi(ins);
				if(i<1 || i>MAXGANG)
					break;
				if(!strlen(GANG(i)->Name))
					break;
				for(j=1; j<4; j++)
					if(user==abs(GANG(i)->Member[j])) {
						YourGang=i;
						YourMember=j;
						break;
					}
				if(!YourGang)
					for(j=1; j<4; j++)
						if(GANG(i)->Member[j]==0) {
							YourGang=i;
							YourMember=j;
							break;
						}
				if(!YourGang) {
					sprintf(text,"%.20s did not invite a wimp like you!",GANG(i)->Name);
					outs(text);
					newline();
					break;
				}
				GANG(YourGang)->Member[YourMember]=user;
				PutGang(YourGang);
				USER(user)->Gang=YourGang;
				PutUser(user);
				sprintf(text,"You are now a gang member of %.20s.",GANG(YourGang)->Name);
				outs(text);
				newline();
				break;
			case 'L':
				lp=0;
				more='Y';
				for(i=1; i<=MAXGANG && more=='Y'; i++)
					if(strlen(GANG(i)->Name)) {
						GangName(i);
						for(k=i+1; k<=MAXGANG && !strlen(GANG(k)->Name); k++);
						if(k<=MAXGANG && strlen(GANG(k)->Name)) {
							tab(40);
							GangName(k);
						}
						newline();
						for(j=0; j<4; j++) {
							sprintf(text,"    %.26s",(GANG(i)->Member[j]) ? USER(abs(GANG(i)->Member[j]))->Handle : "(Any player may join)");
							outs(text);
							if(j==0)
								outs(" (leader)");
							if(GANG(i)->Member[j]<0)
								outs(" (invited)");
							if(k<=MAXGANG && strlen(GANG(k)->Name)) {
								tab(40);
								sprintf(text,"    %.26s",(GANG(k)->Member[j]) ? USER(abs(GANG(k)->Member[j]))->Handle : "(Any player may join)");
								outs(text);
								if(j==0)
									outs(" (leader)");
								if(GANG(k)->Member[j]<0)
									outs(" (invited)");
							}
							newline();
						}
						newline();
						lp++;
						if(lp%3==0) {
							lp=0;
							more=More();
						}
						i=k;
					}
				if(lp)
					Pause();
				break;
			case 'M':
				sprintf(text,"Current %d top gangs, sorted by wins.",MAXGANG+1);
				outs(text);
				newline();
				newline();
				k=0;
				for(i=0; i<MAXGANG; i++) {
					if(strlen(GANG(i)->Name)) {
						sub[k]=i;
						v[k]=GANG(i)->Win;
						j=k;
						while(j>0 && v[j]>v[j-1]) {
							swmem(&v[j],&v[j-1],sizeof(UWORD));
							swmem(&sub[j],&sub[j-1],sizeof(int));
							j--;
						}
						if(k<MAXGANG)
							k++;
					}
				}
				sprintf(OUTbuf,"%s%s       Party            Win-Loss    Ratio   Age%s",Fcol(WHITE),Bcol(GRN),Bcol(BLK));
				outs(OUTbuf);
				newline();
				sprintf(OUTbuf,"%s--------------------   ----------   -----   ---%s",Bcol(GRN),Bcol(BLK));
				outs(OUTbuf);
				newline();
				normal();
				for(i=0; i<k; i++) {
					j=(GANG(sub[i])->Win+GANG(sub[i])->Loss>0) ? 100*GANG(sub[i])->Win/(GANG(sub[i])->Win+GANG(sub[i])->Loss) : 0;
					age=0;
					active=0;
					for(x=0; x<4; x++) {
						if(GANG(sub[i])->Member[x]>0 && USER(GANG(sub[i])->Member[x])->BirthDate) {
							active++;
							age+=(JulianDate-USER(GANG(sub[i])->Member[x])->BirthDate);
						}
					}
					if(active)
						age/=365.25*active;
					sprintf(text,"%-20.20s  %5u-%-5u   %4d\%   %3d",GANG(sub[i])->Name,GANG(sub[i])->Win,GANG(sub[i])->Loss,j,age);
					outs(text);
					newline();
					if((i+1)%20==0)
						if(More()=='N')
							break;
				}
				newline();
				break;
			case 'R':
				if(!YourGang) {
					outs("You are not a member of any gang.");
					newline();
					break;
				}
				if(party) {
					outs("You used your fight this call -- you cannot resign!");
					newline();
					break;
				}
				sprintf(text,"%sReally resign from %s (Y/N)? ",Fcol(CYN),GANG(YourGang)->Name);
				outs(text);
				if(!(c=inkey()))
					break;
				newline();
				if(c!='Y')
					break;
				USER(user)->Gang=0;
				PutUser(user);
				GANG(YourGang)->Member[YourMember]=0;
				PutGang(YourGang);
				if(YourMember==0) {
					sprintf(text,"Dissolving %s...",GANG(YourGang)->Name);
					outs(text);
					numline=0;
					sprintf(line[numline++],"%.26s dissolved your gang, %.20s!",USER(user)->Handle,GANG(YourGang)->Name);
					for(i=1; i<4; i++)
						if(GANG(YourGang)->Member[i]>0) {
							USER(GANG(YourGang)->Member[i])->Gang=0;
							PutUser(GANG(YourGang)->Member[i]);
							UserNote(GANG(YourGang)->Member[i]);
						}
					memset(GANG(YourGang),0,sizeof(struct Gang));
					PutGang(YourGang);
					outs("Ok.");
					newline();
				}
				YourGang=0;
				YourMember=-1;
				break;
			case 'S':
				if(YourGang) {
					sprintf(text,"You are already a member of %.20s.",GANG(YourGang)->Name);
					outs(text);
					newline();
					break;
				}
				for(i=1; i<=MAXGANG; i++)
					if(!strlen(GANG(i)->Name))
						break;
				if(i>MAXGANG) {
					outs("Sorry, no more new gangs can be created -- join one instead.");
					newline();
					break;
				}
				if(ACCESS(USER(user)->Access)->User!='Y') {
					sprintf(text,"Get real!  Who would follow the likes of a %s?",ACCESS(USER(user)->Access)->Name);
					outs(text);
					newline();
					break;
				}
				sprintf(text,"%sWhat is the name of gang #%d [max 20]? ",Fcol(CYN),i);
				outs(text);
				if(input())
					break;
				newline();
				ToUpLow(ins);
				if(strlen(ins)<1 || strlen(ins)>20) {
					outs("Invalid gang name!");
					newline();
					break;
				}
				c=FALSE;
				for(j=1; j<=MAXGANG; j++)
					if(strnicmp(ins,GANG(j)->Name,sizeof(GANG(j)->Name))==0)
						c=TRUE;
				if(c) {
					outs("That gang already exists!");
					newline();
					break;
				}
				YourGang=i;
				YourMember=0;
				strncpy(GANG(YourGang)->Name,ins,sizeof(GANG(YourGang)->Name));
				GANG(YourGang)->Member[YourMember]=user;
				for(j=1; j<4; j++)
					GANG(YourGang)->Member[j]=0;
				GANG(YourGang)->Win=0;
				GANG(YourGang)->Loss=0;
				sprintf(text,"Saving %.20s...",GANG(YourGang)->Name);
				outs(text);
				PutGang(YourGang);
				USER(user)->Gang=YourGang;
				PutUser(user);
				outs("Ok.");
				newline();
				newline();
				outs("By default, anyone can now join your gang.  You need 4 gang members to party.");
				newline();
				outs("You may edit your gang to invite specific players.  In turn, those players you");
				newline();
				outs("invite must then join your gang to become active members.");
				newline();
				break;
			case 'T':
				if(YourMember!=0) {
					outs("Only your gang leader may do this.");
					newline();
					break;
				}
				GangName(YourGang);
				newline();
				newline();
				for(i=0; i<4; i++) {
					sprintf(text,"%s %.26s",bracket(i+1),(GANG(YourGang)->Member[i]) ? USER(abs(GANG(YourGang)->Member[i]))->Handle : "(Any player may join)");
					outs(text);
					if(i==0)
						outs(" (leader)");
					if(GANG(YourGang)->Member[i]<0)
						outs(" (invited)");
					newline();
				}
				newline();
				outs(Fcol(CYN));
				outs("Transfer leadership to who (2-4)? ");
				if(input())
					break;
				i=atoi(ins);
				if(i<2 || i>4) {
					newline();
					break;
				}
				i--;
				sprintf(text," - %.26s",(GANG(YourGang)->Member[i]) ? USER(abs(GANG(YourGang)->Member[i]))->Handle : "(anybody)");
				outs(text);
				newline();
				if(GANG(YourGang)->Member[i]<1) {
					outs("I don't think so.");
					newline();
					break;
				}
				GANG(YourGang)->Member[0]=GANG(YourGang)->Member[i];
				GANG(YourGang)->Member[i]=user;
				YourMember=i;
				PutGang(YourGang);
				break;
		}
	}
}
