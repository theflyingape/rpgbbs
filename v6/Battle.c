/*****************************\
 *  RPGBBS Battle Simulator  *
\*****************************/
#include "RPGBBS.h"

void CreateMonster(struct User *thing,int class,int level)
{
	int ego,i;

	memset(thing,0,sizeof(struct User));
	thing->Class=class;
	thing->Level=level;
	thing->ExpLevel=level;
	if(thing->ExpLevel>MAXLEVEL)
		thing->Level=MAXLEVEL;
	thing->Experience=pow2((double)level-2)*1000.;
	thing->HP=15;
	for(i=2; i<level; i++)
		thing->HP+=9+(i<5)+(i<15)+(i<25)+dice(i)+i;
	if(CLASS(thing->Class)->Magic>1) {
		thing->SP=15;
		for(i=2; i<level; i++)
			thing->SP+=9+(i<5)+(i<15)+(i<25)+dice(i)+i;
		thing->Spell=1472;		/* Spells: 7,8,9,11 */
		for(i=0; i<MAXMAGIC; i++)
			if(dice(Tchr/4)==1)
				thing->Spell|=(UWORD)pow2((double)i);
	}
	else {
		thing->Spell=0;
		if(level>MAXLEVEL/3)
			for(i=0; i<MAXMAGIC; i++)
				if(dice(Tchr/3)==1)
					thing->Spell|=(UWORD)pow2((double)i);
	}
	if(CLASS(thing->Class)->Poison>0)
		thing->Poison=thing->Spell;
	thing->Str=thing->Level+20;
	thing->Int=thing->Level+20;
	thing->Agl=thing->Level+20;
	if(thing->Str<USER(user)->Str)
		thing->Str=USER(user)->Str;
	if(thing->Int<USER(user)->Int)
		thing->Int=USER(user)->Int;
	if(thing->Agl<USER(user)->Agl)
		thing->Agl=USER(user)->Agl;
	if(thing->Str>99)
		thing->Str=99;
	if(thing->Int>99)
		thing->Int=99;
	if(thing->Agl>99)
		thing->Agl=99;

	thing->Weapon=USER(user)->Weapon+dice(3)-2;
	if(thing->Weapon<level*CLASS(thing->Class)->WC/MAXLEVEL || thing->Weapon>CLASS(thing->Class)->WC)
		thing->Weapon=level*CLASS(thing->Class)->WC/MAXLEVEL+dice(3)-2;
	if(thing->Weapon<1)
		thing->Weapon=1;
	if(thing->Weapon>CLASS(thing->Class)->WC)
		thing->Weapon=CLASS(thing->Class)->WC;

	thing->Armor=USER(user)->Armor+dice(3)-2;
	if(thing->Armor<level*CLASS(thing->Class)->AC/MAXLEVEL || thing->Armor>CLASS(thing->Class)->AC)
		thing->Armor=level*CLASS(thing->Class)->AC/MAXLEVEL+dice(3)-2;
	if(thing->Armor<1)
		thing->Armor=1;
	if(thing->Armor>CLASS(thing->Class)->AC)
		thing->Armor=CLASS(thing->Class)->AC;

	ego=100-level;
	if(dice(level/3+2)>1)	/*  is it natural weapon class?  */
		thing->Weapon=-thing->Weapon;
	else if(dice(ego>2 ? ego : 2)==1)	/*  does it have an ego?  */
		thing->Weapon=MAXWEAPON+dice(MAXCLASS)-1;
	if(dice(level/3+2)>1)	/*  is it natural armor class?  */
		thing->Armor=-thing->Armor;
	else if(dice(ego>2 ? ego : 2)==1)	/*  does it have an ego?  */
		thing->Armor=MAXARMOR+dice(MAXCLASS)-1;

	modf(WEAPON(USER(user)->Weapon)->Cost/10.+ARMOR(USER(user)->Armor)->Cost/10.,&thing->Gold);
	thing->Gold+=thing->Gold/dice(100);
	thing->Gold*=Tchr/100.;
	modf(thing->Gold,&thing->Gold);
	if(thing->Gold<1.)
		thing->Gold=1.;
}

void Last50(char *str)
{
	FILE *fp;
	int i;

	if(NME.Sex!='d') {
		GetSystemDateTime();
		numline=0;
		sprintf(line[numline++],"%.6s  %s  %s",Fdate(SystemDate),Ftime(Time),str);
		if((fp=fopen("Last50","r"))) {
			while(!feof(fp) && numline<50)
				inputs(line[numline++],sizeof(line[0]),fp);
			fclose(fp);
			if(numline<50)
				numline--;
		}
		if((fp=fopen("Last50","w"))) {
			for(i=0; i<numline; i++) {
				fputs(line[i],fp);
				fputs("\n",fp);
			}
			fclose(fp);
		}
		sprintf(line[1],"[#%u] %s.",user,str);
		RPGmessage(line[1]);
	}
}

int Attack(BYTE class,BYTE level,BYTE weapon,WORD wm,BYTE strength,BYTE class2,BYTE armor,WORD am,double potency)
{
	int l,w,s,a,damage;

	l=level*(CLASS(class)->ToHit==2 ? potency : 1.);
	w=abs(weapon);
	if(w>MAXWEAPON) {	/* ego weapon bonus */
		if(w-MAXWEAPON==class2)
			w=MAXWEAPON;
		else
			w=MAXWEAPON-1;
	}
	s=strength;
	a=abs(armor);
	if(a>=MAXARMOR) {	/* ego armor bonus */
		if(a-MAXARMOR==class || a==MAXARMOR)
			a=2*MAXARMOR;
		else
			a=MAXARMOR;
	}
	modifier=FALSE;
	damage=(4*(w+wm)+level+s/10-2*(a+am))/2;
	a=(w<MAXWEAPON) ? dice(100) : 97+dice(3);
	if(a>97) {			/* smash! */
		modifier=TRUE;
		if(a==100)
			a+=dice(100);
		a+=dice(100);
	}
	damage+=a*damage/100;
	damage+=dice(l)*CLASS(class)->ToHit;
	if(CLASS(class)->Magic==4) {
		damage*=4;
		damage/=5;
	}
	if(damage<1)
		damage=0;
	return(damage);
}

int CastSpell(void)
{
	int i,mu,spell,result;

	if(USER(user)->Spell==0) {
		outs("You don't have any magic!");
		newline();
		newline();
		return(0);
	}
	mu=(CLASS(USER(user)->Class)->Magic==4) ? 1 : 0;
	result=0;

	do {
		sprintf(text,"%sEnter %s number (?=List): ",Fcol(CYN),CLASS(USER(user)->Class)->Magic>1 ? "spell" : "wand");
		outs(text);
		if(input())
			return(0);
		newline();
		newline();
		spell=atoi(ins);
		if(ins[0]=='?') {
			spell=-1;
			for(i=0; i<MAXMAGIC; i++) {
				if((USER(user)->Spell & (UWORD)pow2((double)i))) {
					sprintf(text,"%s %-24s",bracket(i+1),MAGIC(i)->Name);
					outs(text);
					if(CLASS(USER(user)->Class)->Magic>1) {
						sprintf(text," %4u",MAGIC(i)->Power[mu]);
						outs(text);
					}
					newline();
				}
			}
			newline();
		}
	} while(spell<0 || spell>MAXMAGIC);
	if(spell==0)
		return(0);
	if((USER(user)->Spell & (UWORD)pow2((double)spell-1.))==0) {
		switch(CLASS(USER(user)->Class)->Magic) {
			case 1:
				outs("You don't have that wand!");
				break;
			case 2:
				outs("You don't have that scroll!");
				break;
			default:
				outs("You don't know that spell!");
				break;
		}
		newline();
		newline();
		return(0);
	}
	if(CLASS(USER(user)->Class)->Magic>1 && MAGIC(spell-1)->Power[mu]>Tsp) {
		outs("You don't have enough spell power to cast that spell!");
		newline();
		newline();
		return(0);
	}
	if(Tsp)
		Tsp-=MAGIC(spell-1)->Power[mu];
	if(CLASS(USER(user)->Class)->Magic==1 && dice(100+2*(17-spell))<100) {
		outs("Your wand smokes as you cast the spell.");
		newline();
		newline();
		USER(user)->Spell-=pow2((double)spell-1);
		PutUser(user);
		Delay(25);
	}
	if(CLASS(USER(user)->Class)->Magic==2 && dice((9-CLASS(USER(user)->Class)->ToCast)*(CLASS(NME.Class)->ToHit>3 && NME.Class-2<MAXCLASS ? 2 : 1))==1) {
		outs("Your scroll burns as you cast the spell.");
		newline();
		newline();
		USER(user)->Spell-=pow2((double)spell-1);
		PutUser(user);
		Delay(25);
	}
	if(dice(100)>(Tint+CLASS(USER(user)->Class)->ToInt+((spell<9 || spell==11) ? 0 : 2*CLASS(NME.Class)->DefInt))) {
		if(dice(10)>1)
			outs("Fssst!  Your spell fails!");
		else
			outs("Pfssst!  You crapped your britches!");
		newline();
		newline();
		return(-1);
	}
	if(from=='T' && spell>11) {
		outs(Fcol(YELLOW));
		if(CLASS(USER(user)->Class)->Magic==1)
			outs("You're going to need more than that fancy stick to take me out!");
		else
			outs("Your mental powers are no match to me!");
		normal();
		newline();
		newline();
		return(-1);
	}

	switch(spell) {
		case 1:
			if(Tchr>99) {
				outs("Stop being so vain!");
				spell=0;
				break;
			}
			Tchr+=dice(10);
			if(Tchr>100)
				Tchr=100;
			if(dice(10)>1)
				outs("You feel much more charismatic");
			else
				outs("{Sigh} I really don't need more charm");
			sprintf(text," (%d).",Tchr);
			outs(text);
			break;
		case 2:
			if(Tint>99) {
				outs("Get on with it, professor!");
				spell=0;
				break;
			}
			Tint+=dice(10);
			if(Tint>100)
				Tint=100;
			if(dice(10)>1)
				outs("You feel much more intelligent");
			else
				outs("You feel less like a turd and more like a nerd");
			sprintf(text," (%d).",Tint);
			outs(text);
			break;
		case 3:
			if(Tstr>99) {
				outs("This game prohibits the use of steroids.");
				spell=0;
				break;
			}
			Tstr+=dice(10);
			if(Tstr>100)
				Tstr=100;
			if(dice(10)>1)
				outs("You feel much more stronger");
			else
				outs("My!  What bulging muscles");
			sprintf(text," (%d).",Tstr);
			outs(text);
			break;
		case 4:
			if(Tagl>99) {
				outs("Y'all shakin' & bakin'!");
				spell=0;
				break;
			}
			Tagl+=dice(10);
			if(Tagl>100)
				Tagl=100;
			if(dice(10)>1)
				outs("You feel much more agile");
			else
				outs("Can't touch this");
			sprintf(text," (%d).",Tagl);
			outs(text);
			break;
		case 5:
			for(i=4-CLASS(USER(user)->Class)->Magic+mu; i>0; i--)
				result+=dice(Tshield);
			if(Tshield>USER(user)->Armor/2 && result>USER(user)->Armor) {
				sprintf(text,"Your %s crumbles off you!",ARMOR(USER(user)->Armor)->Name);
				outs(text);
				USER(user)->Armor=0;
				PutUser(user);
				Tshield=0;
				break;
			}
			Tshield++;
			if(dice(10)>1)
				outs("A magic field shimmers around you");
			else
				outs("A magic field SMELLS around you");
			sprintf(text," %s.",points(Tshield));
			outs(text);
			break;
		case 6:
			for(i=4-CLASS(USER(user)->Class)->Magic+mu; i>0; i--)
				result+=dice(Thit);
			if(Thit>USER(user)->Weapon/2 && result>USER(user)->Weapon) {
				sprintf(text,"Your %s burns to a pile of ashes!",WEAPON(USER(user)->Weapon)->Name);
				outs(text);
				USER(user)->Weapon=0;
				PutUser(user);
				Thit=0;
				break;
			}
			Thit++;
			if(dice(10)>1)
				sprintf(text,"Your %s now glows with magical sharpness",WEAPON(USER(user)->Weapon)->Name);
			else
				sprintf(text,"Your %s chants, \"Bring on the turkey!\"",WEAPON(USER(user)->Weapon)->Name);
			outs(text);
			sprintf(text," %s.",points(Thit));
			outs(text);
			break;
		case 7:
			break;
		case 8:
			if(Thp>=USER(user)->HP) {
				outs("Stop acting like a hypochondriac!");
				spell=0;
				break;
			}
			for(i=1; i<USER(user)->Level; i++)
				result+=dice(CLASS(USER(user)->Class)->ToHeal);
			if(Thp+result>USER(user)->HP)
				result=USER(user)->HP-Thp;
			sprintf(text,"Hit points = hit points + %d",result);
			outs(text);
			Thp+=result;
			break;
		case 9:
			for(i=1; i<USER(user)->Level; i++)
				result+=dice(CLASS(USER(user)->Class)->ToBlast*(CLASS(NME.Class)->ToHit>3 ? 2 : 1));
			sprintf(text,"You blast %s for %d hit points!",NMEhim,result);
			outs(text);
			NMEhp-=result;
			break;
		case 10:
			do {
				outs(Fcol(CYN));
				outs("Resurrect who? ");
				i=GetUser();
				newline();
			} while(i<0 || i>MAXUSER);
			if(i>0 && i<=MAXUSER) {
				sprintf(text,"Now raising %.26s...",USER(i)->Handle);
				outs(text);
				USER(i)->Status=0;
				PutUser(i);
				numline=0;
				sprintf(line[numline++],"%.26s used magic to resurrect you!",USER(user)->Handle);
				UserNote(i);
				outs("Ok.");
			}
			break;
		case 11:
			outs("You feel your vitality completely restored.");
			Thp=USER(user)->HP;
			break;
		case 12:
			outs("Your opponent is completely atomized!");
			NMEhp=0;
			break;
		case 13:
			sprintf(text,"You blitz %s with exploding %sc%so%sl%so%sr%ss%s causing a dizzying effect!",NMEhim,Fcol(BRED),Fcol(YELLOW),Fcol(GREEN),Fcol(CYAN),Fcol(BLUE),Fcol(MAGENTA),Fcol(GRY));
			outs(text);
			NMEagl/=2;
			NMEint/=2;
			break;
		case 14:
			if(abs(NME.Weapon)>=abs(NME.Armor)) {
				sprintf(text,"You transform %s %s",NMEhis,NME.Weapon<0 ? "weapon" : WEAPON(NME.Weapon)->Name);
				outs(text);
				NME.Weapon=dice(abs(NME.Weapon))-1;
				sprintf(text," into %s %s!",strchr("AEIOU",WEAPON(NME.Weapon)->Name[0]) ? "an" : "a",WEAPON(NME.Weapon)->Name);
				outs(text);
				NMEhit=0;
			}
			else {
				sprintf(text,"You transform %s %s",NMEhis,NME.Armor<0 ? "armor" : ARMOR(NME.Armor)->Name);
				outs(text);
				USER(user)->Armor=dice(abs(USER(user)->Armor))-1;
				NMEshield=0;
				sprintf(text," into %s %s!",strchr("AEIOU",ARMOR(NME.Armor)->Name[0]) ? "an" : "a",ARMOR(NME.Armor)->Name);
				outs(text);
			}
			if(nme) {
				memcpy(USER(nme),&NME,sizeof(struct User));
				PutUser(nme);
			}
			break;
		case 15:
			do {
				outs(Fcol(CYN));
				outs("Cause mayhem to who? ");
				i=GetUser();
				newline();
			} while(i<0 || i>MAXUSER);
			if(i>0 && i<=MAXUSER && (BYTE)(USER(user)->Level-USER(i)->Level)<=30) {
				sprintf(text,"Now ruining %.26s...",USER(i)->Handle);
				outs(text);
				USER(user)->Gold+=USER(i)->Gold+USER(i)->Bank;
				USER(user)->Gold+=value(ARMOR(USER(i)->Armor)->Cost,Tchr);
				USER(user)->Gold+=value(ESTATE(USER(i)->RealEstate)->Cost,Tchr);
				USER(user)->Gold+=value(SECURITY(USER(i)->Security)->Cost,Tchr);
				USER(user)->Gold+=value(WEAPON(USER(i)->Weapon)->Cost,Tchr);
				PutUser(user);
				USER(i)->Gold=0.;
				USER(i)->Bank=0.;
				USER(i)->Armor/=2;
				USER(i)->Poison/=2;
				if(USER(i)->RealEstate)
					USER(i)->RealEstate--;
				if(USER(i)->Security)
					USER(i)->Security--;
				USER(i)->Spell/=2;
				USER(i)->Weapon/=2;
				if(USER(i)->Hull)
					USER(i)->Hull-=50;
				USER(i)->Cannon=0;
				USER(i)->Ram='N';
				PutUser(i);
				numline=0;
				sprintf(line[numline++],"%.26s ruined you!",(dice(100)>NMEint/2) ? "Somebody" : USER(user)->Handle);
				UserNote(i);
				outs("Ok.");
			}
			else
				outs("Tsk! Tsk!  Maybe next time you'll pay attention!");
			break;
		case 16:
			NME.Level=dice(100);
			if(NME.Level<NME.ExpLevel)
				NME.ExpLevel=NME.Level;
			NME.Experience=ExpNeed(NME.Level)-1.;
			NME.Class=dice(MAXCLASS)-1;
			NME.HP=15;
			for(i=2; i<NME.Level; i++)
				NME.HP+=9+(i<5)+(i<15)+(i<25)+dice(i)+i;
			NMEhp=NME.HP;
			if(CLASS(NME.Class)->Magic>1) {
				NME.SP=15;
				for(i=2; i<NME.Level; i++)
					NME.SP+=9+(i<5)+(i<15)+(i<25)+dice(i)+i;
				NMEsp=NME.SP;
			}
			else {
				NME.SP=0;
				NME.Spell=0;
				NMEsp=0;
			}
			switch(dice(3)) {
				case 1:
					NME.Sex='F';
					break;
				case 2:
					NME.Sex='M';
					break;
				case 3:
					NME.Sex='I';
					break;
			}
			sprintf(text,"You morph %s into a level %u %s (%c)!",NMEhim,NME.Level,CLASS(NME.Class)->Name,NME.Sex);
			outs(text);
			if(nme) {
				memcpy(USER(nme),&NME,sizeof(struct User));
				PutUser(nme);
			}
			break;
	}
	if(spell!=7) {
		newline();
		newline();
	}
	return(spell);
}

void PoisonWeapon(void)
{
	int i,mu,poison;

	if(USER(user)->Poison==0) {
		outs("You don't have any poison!");
		newline();
		newline();
		return;
	}

	mu=(CLASS(USER(user)->Class)->Poison==-1) ? 1 : 0;

	do {
		outs(Fcol(CYN));
		outs("Enter type (?=List): ");
		if(input())
			return;
		newline();
		newline();
		poison=atoi(ins);
		if(ins[0]=='?') {
			poison=-1;
			for(i=0; i<MAXPOISON; i++) {
				if((USER(user)->Poison & (UWORD)pow2((double)i))) {
					sprintf(text,"%s %s",bracket(i+1),POISON(i)->Name);
					outs(text);
					newline();
				}
			}
			newline();
		}
	} while(poison<0 || poison>MAXPOISON);
	if(!poison)
		return;
	poison--;
	if((USER(user)->Poison & (UWORD)pow2((double)poison))==0) {
		outs("You don't have that poison!");
		newline();
		newline();
		return;
	}
	sprintf(text,"You pour some poison on your %s.",WEAPON(USER(user)->Weapon)->Name);
	outs(text);
	newline();
	newline();
	if(Thit>0)
		Thit=(USER(user)->Blessed) ? USER(user)->Weapon/10 : (USER(user)->Cursed) ? -USER(user)->Weapon/10 : 0;
	Thit+=abs(CLASS(USER(user)->Class)->Poison)*POISON(poison)->Power[mu];
	Delay(50);
	if(Thit>(USER(user)->Weapon+(CLASS(USER(user)->Class)->Poison==4 ? USER(user)->Weapon/4 : 0))) {
		outs("Your weapon has as much as it can hold; the rest just drips off.");
		newline();
		newline();
		Thit=USER(user)->Weapon;
		Delay(50);
	}
	if(dice(CLASS(USER(user)->Class)->Vial)==1) {
		outs("You toss the empty vial aside.");
		newline();
		newline();
		USER(user)->Poison-=pow2((double)poison);
		PutUser(user);
		Delay(50);
	}
	DisplayUser();
}

void YourStats(void)
{
	sprintf(text,"Stamina:%s%3u%s/%-3u     ",(Tstr!=USER(user)->Str) ? Fcol(YELLOW) : "",Tstr,Fcol(GRY),USER(user)->Str);
	outs(text);
	sprintf(text,"Intellect:%s%3u%s/%-3u     ",(Tint!=USER(user)->Int) ? Fcol(YELLOW) : "",Tint,Fcol(GRY),USER(user)->Int);
	outs(text);
	sprintf(text,"Agility:%s%3u%s/%-3u     ",(Tagl!=USER(user)->Agl) ? Fcol(YELLOW) : "",Tagl,Fcol(GRY),USER(user)->Agl);
	outs(text);
	sprintf(text,"Charisma:%s%3u%s/%-3u",(Tchr!=USER(user)->Chr) ? Fcol(YELLOW) : "",Tchr,Fcol(GRY),USER(user)->Chr);
	outs(text);
	newline();
	sprintf(text,"Hit points: %s%d%s/%-u      ",(Thp!=USER(user)->HP) ? Fcol(YELLOW) : "",Thp,Fcol(GRY),USER(user)->HP);
	outs(text);
	if(USER(user)->SP) {
		sprintf(text,"Spell points: %s%d%s/%-u      ",(Tsp!=USER(user)->SP) ? Fcol(YELLOW) : "",Tsp,Fcol(GRY),USER(user)->SP);
		outs(text);
	}
	if(USER(user)->Gold) {
		sprintf(text,"Money: %s",money(USER(user)->Gold,user));
		outs(text);
	}
	newline();
	sprintf(text,"Weapon: %s",WEAPON(USER(user)->Weapon)->Name);
	outs(text);
	if(Thit) {
		sprintf(text," %s",points(Thit));
		outs(text);
	}
	sprintf(text,"     Armor: %s",ARMOR(USER(user)->Armor)->Name);
	outs(text);
	if(Tshield) {
		sprintf(text," %s",points(Tshield));
		outs(text);
	}
	newline();
}

int YourSwing(void)
{
	return(Attack(USER(user)->Class,USER(user)->Level,USER(user)->Weapon,Thit,Tstr,NME.Class,NME.Armor,NMEshield,(double)Thp/USER(user)->HP));
}

int NMESwing(void)
{
	int i,mu,spell,result,u;

	mu=(CLASS(NME.Class)->Magic==4) ? 1 : 0;
	if(CLASS(NME.Class)->Magic==1)
		NMEsp=65535;
	spell=0;
	if(!spell && (NME.Spell & 64) && NMEhp<NME.HP/8 && NMEsp>=MAGIC(6)->Power[mu])
		spell=(dice(5*CLASS(NME.Class)->ToCast)==1) ? 7 : 0;
	if(!spell && (NME.Spell & 128) && NMEhp<NME.HP/2 && NMEsp>=MAGIC(7)->Power[mu] && (!NMEcure || NMEsp>=(MAGIC(10)->Power[mu]+MAGIC(7)->Power[mu])))
		spell=(dice(CLASS(NME.Class)->ToCast)==1) ? 8 : 0;
	if(!spell && (NME.Spell & 256) && NMEsp>=MAGIC(8)->Power[mu] && (!NMEcure || NMEsp>=(MAGIC(10)->Power[mu]+MAGIC(8)->Power[mu])))
		spell=(dice(CLASS(NME.Class)->ToCast)==1) ? 9 : 0;
	if(!spell && (NME.Spell & 1024) && NMEhp<NME.HP/8 && NMEsp>=MAGIC(10)->Power[mu])
		spell=11;
	if(!spell && (NME.Spell & 2048) && NMEsp>=MAGIC(11)->Power[mu])
		spell=(dice(5*CLASS(NME.Class)->ToCast)==1) ? 12 : 0;
	if(!spell && (NME.Spell & 4096) && NMEsp>=MAGIC(12)->Power[mu] && (!NMEcure || NMEsp>=(MAGIC(10)->Power[mu]+MAGIC(12)->Power[mu])))
		spell=(dice(CLASS(NME.Class)->ToCast)==1) ? 13 : 0;
	if(!spell && (NME.Spell & 8192) && NMEsp>=MAGIC(13)->Power[mu] && (!NMEcure || NMEsp>=(MAGIC(10)->Power[mu]+MAGIC(13)->Power[mu])) && (USER(user)->Weapon>0 || USER(user)->Armor>0))
		spell=(dice(CLASS(NME.Class)->ToCast)==1) ? 14 : 0;
	if(!spell && (NME.Spell & 16384) && NMEsp>=MAGIC(6)->Power[mu] && (!NMEcure || NMEsp>=(MAGIC(10)->Power[mu]+MAGIC(6)->Power[mu])) && NME.Sex=='d')
		spell=(dice(CLASS(NME.Class)->ToCast)==1) ? 15 : 0;
	if(!spell && (NME.Spell & 32768) && NMEsp>=MAGIC(7)->Power[mu] && (!NMEcure || NMEsp>=(MAGIC(10)->Power[mu]+MAGIC(7)->Power[mu])) && NME.Sex=='d')
		spell=(dice(CLASS(NME.Class)->ToCast)==1) ? 16 : 0;
	if(spell) {
		if(MAGIC(spell-1)->Power[mu]>NMEsp)
			NMEsp=0;	/* allow for `extra' spells a dungeon monster may cast */
		else
			NMEsp-=MAGIC(spell-1)->Power[mu];
		if(CLASS(NME.Class)->Magic==1 && dice(100+2*(17-spell))<100) {
			sprintf(text,"%s wand smokes as %s casts the spell.",NMEHis,NMEhe);
			outs(text);
			newline();
			newline();
			NME.Spell-=pow2((double)spell-1.);
			if(nme) {
				memcpy(USER(nme),&NME,sizeof(struct User));
				PutUser(nme);
			}
		}
		if(CLASS(NME.Class)->Magic==2 && dice((9-CLASS(NME.Class)->ToCast)*(CLASS(USER(user)->Class)->ToHit>3 ? 2 : 1))==1) {
			sprintf(text,"%s scroll burns as %s casts the spell.",NMEHis,NMEhe);
			outs(text);
			newline();
			newline();
			NME.Spell-=pow2((double)spell-1.);
			if(nme) {
				memcpy(USER(nme),&NME,sizeof(struct User));
				PutUser(nme);
			}
		}
		Delay(25);
		if(dice(100)>(NMEint+CLASS(NME.Class)->ToInt+((spell<9 || spell==11) ? 0 : 2*CLASS(USER(user)->Class)->DefInt))) {
			if(dice(10)>1)
				sprintf(text,"Fssst!  %s spell fails!",NMEHis);
			else
				sprintf(text,"Pfssst!  %s crapped %s britches!",NMEHe,NMEhis);
			outs(text);
			newline();
			newline();
			return(0);
		}
		result=0;
		switch(spell) {
			case 7:
				sprintf(text,"%s%.26s teleports %sself away from the battle!",Fcol(YELLOW),NME.Handle,NMEhim);
				outs(text);
				normal();
				if(NME.Sex!='d') {
					newline();
					newline();
					Delay(25);
					sprintf(text,"As %s fades away, %s says,",NMEhe,NMEhe);
					outs(text);
					newline();
					newline();
					if(nme) {
						numline=0;
						sprintf(line[numline++],"%.26s cursed as you teleported away.",USER(user)->Handle);
						UserNote(nme);
					}
					sprintf(ins,"%.26s watched %s%.26s teleport away!",USER(user)->Handle,NME.Sex=='I' ? (strchr("AEIOU",NME.Handle[0]) ? "an " : "a ") : "",NME.Handle);
					Last50(ins);
					sprintf(text,"%s who fights and runs away, lives to fight another day.",NMEHe);
					outs(text);
				}
				break;
			case 8:
				for(i=1; i<NME.Level; i++)
					result+=dice(CLASS(NME.Class)->ToHeal);
				if(NMEhp+result>NME.HP)
					result=NME.HP-NMEhp;
				sprintf(text,"%s heals %sself!  Hit points = hit points + %d",NMEHe,NMEhim,result);
				outs(text);
				NMEhp+=result;
				break;
			case 9:
				for(i=1; i<NME.Level; i++)
					result+=dice(CLASS(NME.Class)->ToBlast*(CLASS(USER(user)->Class)->ToHit>3 ? 2 : 1)-(CLASS(USER(user)->Class)->ToHit>3 && NME.Sex=='d' ? 9 : 0));
				sprintf(text,"%s blasts you for %d hit points!",NMEHe,result);
				outs(text);
				Thp-=result;
				break;
			case 11:
				sprintf(text,"%s cures %sself!",NMEHe,NMEhim);
				outs(text);
				NMEhp=NME.HP;
				NMEcure=FALSE;
				break;
			case 12:
				sprintf(text,"%s zaps your soul with one blow!",NMEHe);
				outs(text);
				Thp=0;
				break;
			case 13:
				sprintf(text,"%s blitzes you with exploding %sc%so%sl%so%sr%ss%s causing a dizzying effect!",NMEHe,Fcol(BRED),Fcol(YELLOW),Fcol(GREEN),Fcol(CYAN),Fcol(BLUE),Fcol(MAGENTA),Fcol(GRY));
				outs(text);
				Tagl/=2;
				Tint/=2;
				break;
			case 14:
				if(USER(user)->Weapon>=USER(user)->Armor) {
					sprintf(text,"%s transforms your %s",NMEHe,WEAPON(USER(user)->Weapon)->Name);
					outs(text);
					USER(user)->Weapon=dice(USER(user)->Weapon)-1;
					Thit=0;
					sprintf(text," into %s %s!",strchr("AEIOU",WEAPON(USER(user)->Weapon)->Name[0]) ? "an": "a",WEAPON(USER(user)->Weapon)->Name);
					outs(text);
				}
				else {
					sprintf(text,"%s transforms your %s",NMEHe,ARMOR(USER(user)->Armor)->Name);
					outs(text);
					USER(user)->Armor=dice(USER(user)->Armor)-1;
					Tshield=0;
					sprintf(text," into %s %s!",strchr("AEIOU",ARMOR(USER(user)->Armor)->Name[0]) ? "an": "a",ARMOR(USER(user)->Armor)->Name);
					outs(text);
				}
				PutUser(user);
				break;
			case 15:
				if(USER(user)->Armor) {
					sprintf(text,"%s sends an acid rain, damaging your %s!",NMEHe,ARMOR(USER(user)->Armor)->Name);
					outs(text);
					Tshield-=dice(USER(user)->Armor);
				}
				else {
					sprintf(text,"%s sends an acid rain, damaging your %s!",NMEHe,WEAPON(USER(user)->Weapon)->Name);
					outs(text);
					Thit-=dice(USER(user)->Weapon);
				}
				break;
			case 16:
				if(USER(user)->Blessed) {
					for(i=1; i<=MAXUSER; i++) {
						u=dice(MAXUSER);
						if(!USER(u)->Blessed)
							break;
					}
					USER(u)->Blessed=user;
					PutUser(u);
					USER(user)->Blessed=0;
					PutUser(user);
				}
				if(!USER(user)->Cursed) {
					u=CurseRemove();
					sprintf(text,"%s sends a black cloud from %.26s!",NMEHe,USER(u)->Handle);
					outs(text);
					Tstr-=10;
					Tint-=10;
					Tagl-=10;
					Tchr-=10;
					Thit-=USER(user)->Weapon/10;
					Tshield-=USER(user)->Armor/10;
					USER(user)->Cursed=u;
					PutUser(user);
				}
				else {
					sprintf(text,"%s drains the abilities from your body!",NMEHe);
					outs(text);
					if(Tstr)
						Tstr-=dice(Tstr);
					if(Tint)
						Tint-=dice(Tint);
					if(Tagl)
						Tagl-=dice(Tagl);
					if(Tchr)
						Tchr-=dice(Tchr);
				}
				break;
		}
		newline();
		newline();
		Delay(25);
		return(spell);
	}
	else {
		if(dice(100)>(50+NMEagl/2+CLASS(USER(user)->Class)->DefAgl)) {
			result=0;
			if(NME.Sex=='F' || NME.Sex=='M') {
				if(from=='T' && dice(5)>1) {
					if(NMEhp>NME.HP/2)
						switch(dice(5)) {
							case 1:
								strcpy(text,"\"Swing harder!  Move faster!\"");
								break;
							case 2:
								strcpy(text,"\"At least you will go down swinging!\"");
								break;
							case 3:
								strcpy(text,"\"You hit like a little girl!\"");
								break;
							case 4:
								strcpy(text,"\"You should have stayed in bed today!\"");
								break;
							case 5:
								sprintf(text,"\"Your %s is going to look nice on my mantlepiece!\"",WEAPON(USER(user)->Weapon)->Name);
								break;
						}
					else
						switch(dice(5)) {
							case 1:
								strcpy(text,"\"Where did you learn to fight like that?\"");
								break;
							case 2:
								strcpy(text,"\"Ouch!  No hitting below the belt!\"");
								break;
							case 3:
								strcpy(text,"\"I am definitely too old for this!\"");
								break;
							case 4:
								strcpy(text,"\"Why couldn't you have just drink your drink?\"");
								break;
							case 5:
								strcpy(text,"\"I am going to feel this tomorrow!\"");
								break;
						}
				}
				else
					sprintf(text,"%s %s whistles by you.",NMEHis,(NME.Weapon>=0) ? WEAPON(NME.Weapon)->Name : "fists");
			}
			else
				sprintf(text,"It attacks you, but misses.");
		}
		else {
			result=Attack(NME.Class,NME.Level,NME.Weapon,NMEhit,NMEstr,USER(user)->Class,USER(user)->Armor,Tshield,(double)NMEhp/NME.HP);
			if(result) {
				if(modifier)
					sprintf(text,"%s smashes you for %d hit points!",NMEHe,result);
				else
					sprintf(text,"%s hits you for %d hit points.",NMEHe,result);
			}
			else
				sprintf(text,"%s %s does not even scratch you.",NMEHis,WEAPON(NME.Weapon)->Name);
		}
		outs(text);
		newline();
		newline();
		Thp-=result;
		return(0);
	}
}

void NMEstat(void)
{
	NMEhp=NME.HP;
	NMEsp=NME.SP;
	if(CLASS(NME.Class)->Magic==1)
		NMEsp=65535;
	NMEhull=NME.Hull;
	NMEstr=NME.Str+(NME.Blessed ? 10 : (NME.Cursed ? -10 : 0));
	NMEint=NME.Int+(NME.Blessed ? 10 : (NME.Cursed ? -10 : 0));
	NMEagl=NME.Agl+(NME.Blessed ? 10 : (NME.Cursed ? -10 : 0));
	NMEchr=NME.Chr+(NME.Blessed ? 10 : (NME.Cursed ? -10 : 0));
	if(NMEstr>100)
		NMEstr=100;
	if(NMEint>100)
		NMEint=100;
	if(NMEagl>100)
		NMEagl=100;
	if(NMEchr>100)
		NMEchr=100;
	NMEhit=(NME.Blessed) ? NME.Weapon/10 : (NME.Cursed) ? -NME.Weapon/10 : 0;
	NMEshield=(NME.Blessed) ? NME.Armor/10 :(NME.Cursed) ? -NME.Armor/10 : 0;
	NMEcure=((NME.Spell & 1024)==1024 && NMEsp>=MAGIC(10)->Power[(CLASS(NME.Class)->Magic==4) ? 1 : 0] && dice(5*CLASS(NME.Class)->ToCast)==1) ? TRUE : FALSE;
	switch(NME.Sex) {
		case 'M':
			NMEHe="He";
			NMEhe="he";
			NMEHim="Him";
			NMEhim="him";
			NMEHis="His";
			NMEhis="his";
			break;
		case 'F':
			NMEHe="She";
			NMEhe="she";
			NMEHim="Her";
			NMEhim="her";
			NMEHis="Her";
			NMEhis="her";
			break;
		default:
			NMEHe="It";
			NMEhe="it";
			NMEHim="It";
			NMEhim="it";
			NMEHis="Its";
			NMEhis="its";
			break;
	}
	DisplayNME();
}

void Battle(void)
{
	FILE *fp;
	int i,n,result,take;
	char c,sex[4];

	NMEstat();
	if((dice(100)+USER(user)->ExpLevel+Tagl+CLASS(NME.Class)->DefAgl)>=(dice(100)+NME.ExpLevel+NMEagl)) {
		outs("You get the first swing.");
		newline();
		newline();
		if(CLASS(USER(user)->Class)->Backstab) {
			do {
				outs(Fcol(CYN));
				outs("Attempt to backstab (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				newline();
			} while(c!='Y' && c!='N');
			if(c=='Y') {
				if((dice(100))>(Tagl+CLASS(NME.Class)->DefAgl)) {
					sprintf(text,"Attempt fails!  %s gets the first swing.",NMEHe);
					outs(text);
					newline();
				}
				else {
					result=YourSwing()*CLASS(USER(user)->Class)->Backstab;
					NMEhp-=result;
					if(modifier)
						sprintf(text,"You plunge into %s for %d hit points!",NMEhim,result);
					else
						sprintf(text,"You stab %s for %d hit points.",NMEhim,result);
					outs(text);
					newline();
				}
			}
			else {
				if(dice(100)>(50+Tagl/2+CLASS(NME.Class)->DefAgl)) {
					result=0;
					sprintf(text,"Your %s passes through thin air.",WEAPON(USER(user)->Weapon)->Name);
				}
				else {
					result=YourSwing();
					sprintf(text,"You hit %s for %d hit points.",NMEhim,result);
				}
				outs(text);
				newline();
				NMEhp-=result;
			}
		}
		else {
			if(dice(100)>(50+Tagl/2+CLASS(NME.Class)->DefAgl)) {
				result=0;
				sprintf(text,"Your %s passes through thin air.",WEAPON(USER(user)->Weapon)->Name);
			}
			else {
				result=YourSwing();
				if(modifier)
					sprintf(text,"You smash %s for %d hit points!",NMEhim,result);
				else
					sprintf(text,"You hit %s for %d hit points.",NMEhim,result);
			}
			outs(text);
			newline();
			NMEhp-=result;
		}
	}
	else {
		sprintf(text,"%s gets the first swing.",NMEHe);
		outs(text);
		newline();
		Delay(25);
		if(dice(100)<=NMEint/10*abs(CLASS(NME.Class)->Poison) && NME.Poison) {
			do {
				n=dice(MAXPOISON)-1;
			} while((NME.Poison & (UWORD)pow2((double)n))==0);
			NME.Poison-=pow2((double)n);
			if(nme) {
				memcpy(USER(nme),&NME,sizeof(struct User));
				PutUser(nme);
			}
			NMEhit+=abs(CLASS(NME.Class)->Poison)*POISON(n)->Power[(CLASS(NME.Class)->Poison==-1) ? 1 : 0];
			newline();
			if(NME.Weapon>0) {
				sprintf(text,"%s quickly empties a vial of poison on %s %s.",NMEHe,NMEhis,WEAPON(NME.Weapon)->Name);
				outs(text);
				newline();
				Delay(50);
				if(NMEhit>abs(NME.Weapon)) {
					NMEhit=abs(NME.Weapon);
					newline();
					outs("You watch in terror as excess poison drips off!");
					newline();
					Delay(50);
				}
			}
			else {
				sprintf(text,"It secretes a caustic ooze %s.",points(NMEhit));
				outs(text);
				newline();
				Delay(50);
			}
		}
	}
	newline();
	if(NMEhp>0)
		if(NMESwing()==7)
			return;
	DisplayUser();
	DisplayNME();

	while(Thp>0 && NMEhp>0) {
		sprintf(text,"%s[%s%u%s,",Fcol(BLU),Fcol((Thp>(USER(user)->HP*2/3)) ? GREEN : (Thp>(USER(user)->HP*1/3)) ? YELLOW : BRED),Thp,Fcol(CYAN));
		outs(text);
		outs(Fcol((NMEhp>(NME.HP*2/3)) ? GREEN : (NMEhp>(NME.HP/3)) ? YELLOW : BRED));
		if(Tint>=50 && Tint<100) {
			i=2*(100-Tint);
			n=(NMEhp/i+.5);
			n*=i;
			sprintf(text,"%d",n);
		}
		if(Tint<50 || n==0)
			sprintf(text,"%s",(NMEhp>(NME.HP*2/3)) ? "Healthy" : (NMEhp>(NME.HP/3)) ? "Hurting" : "Weak");
		if(Tint>99)
			sprintf(text,"%d",NMEhp);
		outs(text);
		sprintf(text,"%s]%s <A> Attack, <C> Cast spell, <R> Retreat, <S> Status: ",Fcol(BLU),Fcol(CYN));
		outs(text);
		if(!(c=inkey()))
			c='A';
		newline();
		newline();
		switch(c) {
			case 'A':
				if(dice(100)>(50+Tagl/2+CLASS(NME.Class)->DefAgl)) {
					result=0;
					sprintf(text,"Your %s passes through thin air.",WEAPON(USER(user)->Weapon)->Name);
				}
				else {
					result=YourSwing();
					if(result) {
						if(modifier)
							sprintf(text,"You smash %s for %d hit points!",NMEhim,result);
						else
							sprintf(text,"You hit %s for %d hit points.",NMEhim,result);
					}
					else
						sprintf(text,"Your %s does not even scratch %s.",WEAPON(USER(user)->Weapon)->Name,NMEhim);
				}
				outs(text);
				newline();
				newline();
				NMEhp-=result;
				break;
			case 'C':
				i=CastSpell();
				if(i==0)
					continue;
				if(i==7) {
					if(from=='L') {
						outs("The Taxman snatches you from the anti-matter stream!");
						newline();
						newline();
						break;
					}
					if(from=='T') {
						outs("Tiny snatches you from the anti-matter stream!");
						newline();
						newline();
						break;
					}
					outs("You teleport away from the battle!");
					newline();
					newline();
					return;
				}
				break;
			case 'R':
				if(from=='D') {
					outs("Argh!  You run like crazy out the nearest exit!");
					newline();
					newline();
					Delay(50);
					if(dice((Tint-CLASS(USER(user)->Class)->DefInt+CLASS(NME.Class)->ToInt)/20+1)==1) {
						sprintf(text,"%s%s%.26s says, \"%sI'm not through with you yet!%s\"",Fcol(GRN),NME.Sex=='d' ? "The " : "",NME.Handle,Fcol(WHITE),Fcol(GRN));
						outs(text);
						normal();
						newline();
						newline();
						Delay(50);
						if(NMESwing()==7)
							return;
						Delay(25);
						if(Thp<1)
							continue;
					}
					outs("You make good on your escape!");
					newline();
					newline();
				}
				else
				if(from=='L') {
					outs("You can never retreat from The Taxman!");
					newline();
					newline();
					continue;
				}
				else
				if(from=='T') {
					outs("The crowd stops your retreat and toss you back!");
					newline();
					newline();
					continue;
				}
				else {
					sprintf(text,"%s who fights and runs away, lives to fight another day.",USER(user)->Sex=='M' ? "He" : USER(user)->Sex=='F' ? "She" : "It");
					outs(text);
					newline();
					newline();
					numline=0;
					if(nme) {
						sprintf(line[numline++],"%.26s, the coward, retreated from you.",USER(user)->Handle);
						UserNote(nme);
						sprintf(OUTbuf,"%.26s retreated from %.26s",USER(user)->Handle,NME.Handle);
						Last50(OUTbuf);
					}
				}
				return;
				break;
			case 'S':
				YourStats();
				newline();
				continue;
				break;
			default:
				continue;
				break;
		}
		DisplayNME();
		if(NMEhp>0)
			if(NMESwing()==7)
				return;
		DisplayUser();
	}
	if(Thp<1) {
		Thp=0;
		numline=0;
		strcpy(sex,(USER(user)->Sex=='M') ? "his" : (USER(user)->Sex=='F') ? "her" : "its");
		if(NME.Sex=='M' || NME.Sex=='F' || nme>0) {
			sprintf(text,"%.26s killed you!",NME.Handle);
			sprintf(line[numline++],"You killed %.26s!",USER(user)->Handle);
		}
		else
			sprintf(text,"The %s killed you!",NME.Handle);
		outs(text);
		newline();
		newline();
		Delay(50);
		if(nme) {
			if(USER(user)->Gold>0.) {
				sprintf(text,"%s gets %s that you were carrying.",NMEHe,money(USER(user)->Gold,user));
				sprintf(line[numline++],"You got %s.",money(USER(user)->Gold,nme));
				outs(text);
				newline();
			}
			take=(NME.Weapon<USER(user)->Weapon);
			if(NME.Weapon>MAXWEAPON && USER(user)->Weapon>=MAXWEAPON)
				take=(USER(user)->Weapon==MAXWEAPON || (take && USER(user)->Weapon>=MAXWEAPON+MAXCLASS-3));
			if(take) {
				sprintf(text,"%s also gets your %s.",NMEHe,WEAPON(USER(user)->Weapon)->Name);
				sprintf(line[numline++],"You also got %s %s.",sex,WEAPON(USER(user)->Weapon)->Name);
				swmem(&NME.Weapon,&USER(user)->Weapon,sizeof(NME.Weapon));
			}
			else {
				d=value(WEAPON(USER(user)->Weapon)->Cost,NMEchr);
				sprintf(text,"%s gets %s for your %s.",NMEHe,money(d,user),WEAPON(USER(user)->Weapon)->Name);
				sprintf(line[numline++],"You got %s for %s %s.",money(d,nme),sex,WEAPON(USER(user)->Weapon)->Name);
				NME.Gold+=d;
			}
			outs(text);
			newline();
			take=(NME.Armor<USER(user)->Armor);
			if(NME.Armor>MAXARMOR && USER(user)->Armor>=MAXARMOR)
				take=(USER(user)->Armor==MAXARMOR || (take && USER(user)->Armor>=MAXARMOR+MAXCLASS-3));
			if(take) {
				sprintf(text,"%s also gets your %s.",NMEHe,ARMOR(USER(user)->Armor)->Name);
				sprintf(line[numline++],"You also got %s %s.",sex,ARMOR(USER(user)->Armor)->Name);
				swmem(&NME.Armor,&USER(user)->Armor,sizeof(NME.Armor));
			}
			else {
				d=value(ARMOR(USER(user)->Armor)->Cost,NMEchr);
				sprintf(text,"%s gets %s for your %s.",NMEHe,money(d,user),ARMOR(USER(user)->Armor)->Name);
				sprintf(line[numline++],"You got %s for %s %s.",money(d,nme),sex,ARMOR(USER(user)->Armor)->Name);
				NME.Gold+=d;
			}
			outs(text);
			newline();
			if(USER(user)->Gang && USER(user)->Gang==NME.Gang) {
				if(GANG(NME.Gang)->Member[0]==nme) {
					sprintf(text,"%s says, \"Let that be a lesson to you, punk!\"",NMEHe);
					outs(text);
					newline();
				}
				if(GANG(USER(user)->Gang)->Member[0]==user) {
					for(i=1; i<4; i++) {
						if(GANG(USER(user)->Gang)->Member[i]==nme) {
							GANG(NME.Gang)->Member[0]=nme;
							GANG(NME.Gang)->Member[i]=user;
							PutGang(NME.Gang);
							sprintf(text,"You also forfeit your %.20s leadership.",GANG(USER(user)->Gang)->Name);
							outs(text);
							newline();
							break;
						}
					}
				}
			}
			if(NME.Cursed) {
				NME.Cursed=0;
				if(nme) {
					memcpy(USER(nme),&NME,sizeof(struct User));
					PutUser(nme);
				}
				USER(user)->Cursed=nme;
				newline();
				Delay(50);
				outs(Fcol(BLUE));
				outs("A black cloud appears and hovers over you...");
				normal();
				Delay(50);
				newline();
				newline();
			}
		}
		if(USER(user)->Blessed>0 && NME.Sex!='d') {
			USER(user)->Blessed=0;
			if(nme)
				NME.Blessed=user;
			else
				BlessAssign();
			if(USER(user)->Blessed==0) {
				newline();
				Delay(50);
				outs(Fcol(YELLOW));
				outs("Your shining aura fades away...");
				normal();
				Delay(50);
				newline();
				newline();
			}
		}
		if(nme==0 && USER(user)->Gold>0.) {
			sprintf(text,"%s gets all your money!",NMEHe);
			outs(text);
			newline();
			USER(nme)->Gold+=USER(user)->Gold;	/* accumulate monster jackpot */
			PutUser(nme);
		}
		modf(pow2((double)(USER(user)->ExpLevel-2))*1000./3.,&d),
		NME.Experience+=d;
		NME.Gold+=USER(user)->Gold;
		NME.YouKill++;
		if(nme) {
			memcpy(USER(nme),&NME,sizeof(struct User));
			PutUser(nme);
			UserNote(nme);
		}
		USER(user)->Gold=0.;
		USER(user)->WasKill++;
		PutUser(user);
		Delay(50);
		newline();
		if(NME.Sex!='d') {
			sprintf(reason,"killed by %s%.26s",NME.Sex=='I' ? (strchr("AEIOU",NME.Handle[0]) ? "an " : "a ") : "",NME.Handle);
			sprintf(OUTbuf,"%.26s was %s",USER(user)->Handle,reason);
			Last50(OUTbuf);
		}
		else {
			sprintf(reason,"killed by a level %u %.26s",NME.Level,NME.Handle);
			sprintf(text,"[#%u] %.26s was %s",user,USER(user)->Handle,reason);
			RPGmessage(text);
		}
		logoff=TRUE;
	}
	if(NMEhp<1) {
		NMEhp=0;
		numline=0;
		strcpy(sex,(USER(user)->Sex=='M') ? "He" : (USER(user)->Sex=='F') ? "She" : "It");
		if(NME.Sex=='M' || NME.Sex=='F' || nme>0) {
			sprintf(text,"You killed %.26s!",NME.Handle);
			sprintf(line[numline++],"%.26s killed you!",USER(user)->Handle);
			if(USER(user)->Level+3<NME.ExpLevel) {
				sprintf(filename,"News.%lu",JulianDate);
				if((fp=fopen(filename,"a"))) {
					GetSystemDateTime();
					fprintf(fp,"          %.26s (%d) beat %.26s (%d).\n",USER(user)->Handle,USER(user)->Level,NME.Handle,NME.ExpLevel);
					fclose(fp);
				}
			}
		}
		else
			sprintf(text,"You killed the %s!",NME.Handle);
		outs(text);
		newline();
		newline();
		Delay(25);
		modf(pow2((double)(NME.ExpLevel-2))*1000./(NME.Sex=='d' ? 15. : 3.),&d),
		USER(user)->Experience+=d;
		sprintf(text,"You get %s experience",Fnum(d));
		outs(text);
		if(NME.Gold>0.) {
			sprintf(text," and %s that %s was carrying",money(NME.Gold,user),NMEhe);
			sprintf(line[numline++],"%s got %s.",sex,money(NME.Gold,nme));
			outs(text);
		}
		outs(".");
		newline();
		take=(USER(user)->Weapon<NME.Weapon);
		if(USER(user)->Weapon>MAXWEAPON && NME.Weapon>=MAXWEAPON)
			take=(NME.Weapon==MAXWEAPON || (take && NME.Weapon>=MAXWEAPON+MAXCLASS-3));
		if(take) {
			sprintf(text,"You also get %s %s.",NMEhis,WEAPON(NME.Weapon)->Name);
			sprintf(line[numline++],"%s also got your %s.",sex,WEAPON(NME.Weapon)->Name);
			outs(text);
			newline();
			swmem(&USER(user)->Weapon,&NME.Weapon,sizeof(USER(user)->Weapon));
			Thit=(USER(user)->Blessed) ? USER(user)->Weapon/10 : (USER(user)->Cursed) ? -USER(user)->Weapon/10 : 0;
		}
		else if(NME.Weapon>0) {
			d=value(WEAPON(NME.Weapon)->Cost,Tchr);
			sprintf(text,"You get %s for %s %s.",money(d,user),NMEhis,WEAPON(NME.Weapon)->Name);
			sprintf(line[numline++],"%s got %s for your %s.",sex,money(d,nme),WEAPON(NME.Weapon)->Name);
			outs(text);
			newline();
			USER(user)->Gold+=d;
		}
		take=(USER(user)->Armor<NME.Armor);
		if(USER(user)->Armor>MAXARMOR && NME.Armor>=MAXARMOR)
			take=(NME.Armor==MAXARMOR || (take && NME.Armor>=MAXARMOR+MAXCLASS-3));
		if(take) {
			sprintf(text,"You also get %s %s.",NMEhis,ARMOR(NME.Armor)->Name);
			sprintf(line[numline++],"%s also got your %s.",sex,ARMOR(NME.Armor)->Name);
			outs(text);
			newline();
			swmem(&USER(user)->Armor,&NME.Armor,sizeof(USER(user)->Armor));
			Tshield=(USER(user)->Blessed) ? USER(user)->Armor/10 : (USER(user)->Cursed) ? -USER(user)->Armor/10 : 0;
		}
		else if(NME.Armor>0) {
			d=value(ARMOR(NME.Armor)->Cost,Tchr);
			sprintf(text,"You get %s for %s %s.",money(d,user),NMEhis,ARMOR(NME.Armor)->Name);
			sprintf(line[numline++],"%s got %s for your %s.",sex,money(d,nme),ARMOR(NME.Armor)->Name);
			outs(text);
			newline();
			USER(user)->Gold+=d;
		}
		if(nme) {
			for(i=1; i<=MAXUSER; i++) {
				if(USER(i)->BountyWho==nme) {
					if(i!=user) {
						sprintf(text,"You get %s bounty from %.26s.",money(USER(i)->BountyAmount,user),USER(i)->Handle);
						sprintf(line[numline++],"%s got %s bounty from %.26s.",sex,money(USER(i)->BountyAmount,nme),USER(i)->Handle);
						USER(user)->Gold+=USER(i)->BountyAmount;
						USER(i)->Gold-=USER(i)->BountyAmount;
						if(USER(i)->Gold<0.) {
							USER(i)->Bank+=USER(i)->Gold;
							USER(i)->Gold=0.;
						}
						if(USER(i)->Bank<0.) {
							USER(i)->Loan-=USER(i)->Bank;
							USER(i)->Bank=0.;
						}
					}
					else
						sprintf(text,"You did your own dirty deed; the bounty is lifted.");
					outs(text);
					newline();
					if(i==nme) {	/*  for the idiots that put a bounty on themselves  */
						NME.BountyAmount=0.;
						NME.BountyWho=0;
					}
					else {
						USER(i)->BountyAmount=0.;
						USER(i)->BountyWho=0;
					}
					PutUser(i);
				}
			}
		}
		if(USER(user)->Gang && USER(user)->Gang==NME.Gang) {
			if(GANG(NME.Gang)->Member[0]==nme) {
				for(i=1; i<4; i++) {
					if(GANG(USER(user)->Gang)->Member[i]==user) {
						GANG(USER(user)->Gang)->Member[0]=user;
						GANG(USER(user)->Gang)->Member[i]=nme;
						PutGang(USER(user)->Gang);
						sprintf(text,"You take over as leader of %.20s.",GANG(USER(user)->Gang)->Name);
						outs(text);
						newline();
						break;
					}
				}
			}
		}
		if(NME.Blessed || (USER(user)->Cursed>0 && nme>0)) {
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
		if(NME.Blessed) {
			NME.Blessed=0;
			USER(user)->Blessed=nme;
			newline();
			Delay(50);
			outs(Fcol(YELLOW));
			outs("An aura shines about yourself and you feel augmented.");
			normal();
			newline();
			Delay(50);
			newline();
		}
		if(USER(user)->Cursed>0 && nme>0) {
			NME.Cursed=user;
			USER(user)->Cursed=0;
			newline();
			Delay(50);
			outs(Fcol(BLUE));
			outs("You feel as though a black cloud has just left you.");
			normal();
			newline();
			Delay(50);
			newline();
		}
		USER(user)->Gold+=NME.Gold;
		USER(user)->YouKill++;
		NME.Gold=0.;
		NME.Status=user;
		if(USER(user)->ExpLevel<NME.ExpLevel)
			NME.ExpLevel=USER(user)->ExpLevel;
		NME.WasKill++;
		if(nme) {
			memcpy(USER(nme),&NME,sizeof(struct User));
			PutUser(nme);
			newline();
			sprintf(text,"%sWould you like to leave %s a comment (Y/N)? ",Fcol(CYN),NMEhim);
			outs(text);
			c=inkey();
			newline();
			if(c=='Y') {
				outs(Fcol(CYN));
				outs("Enter a comment on the line below.");
				newline();
				outs("> ");
				input();
				newline();
				sprintf(line[numline++],"");
				sprintf(line[numline++],"%.26s says,",USER(user)->Handle);
				sprintf(line[numline++],"\"%s\"",ins);
			}
			UserNote(nme);
		}
		PutUser(user);
		Delay(25);
		newline();
		if(NME.Sex!='d') {
			sprintf(OUTbuf,"%.26s killed %s%.26s",USER(user)->Handle,NME.Sex=='I' ? (strchr("AEIOU",NME.Handle[0]) ? "an " : "a ") : "",NME.Handle);
			Last50(OUTbuf);
		}
	}
	DisplayUser();
	DisplayNME();
}
