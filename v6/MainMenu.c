/**********************\
 *  RPGBBS Main Menu  *
\**********************/
#include "RPGBBS.h"

void CharacterStats(int rec)
{
	int i,l1,l2;
	char vl1[20],vl2[20],vm1[20],vm2[20],comma[3];

	if(rec==user) {
		NMEhp=Thp;
		NMEsp=Tsp;
		NMEstr=Tstr;
		NMEint=Tint;
		NMEagl=Tagl;
		NMEchr=Tchr;
		NMEhit=Thit;
		NMEshield=Tshield;
		NMEhull=Thull;
	}
	else {
		nme=rec;
		NMEstat();
		nme=0;
	}
	cls();
	i=strlen(USER(rec)->Handle);
	if(i>sizeof(USER(rec)->Handle))
		i=sizeof(USER(rec)->Handle);
	l1=27-i/2-(i%2 ? 1 : 0);
	l2=27-i/2;
	sprintf(OUTbuf,"%s+%.*s=:))%s%s %.26s %s%s((:=%.*s+",Fcol(BLU),l1,"---------------------------",Bcol(BLU),Fcol(YELLOW),USER(rec)->Handle,Bcol(BLK),Fcol(BLU),l2,"---------------------------");
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"| %sAccess:%s %-9.9s        %sUser#:%s %-3d        %sBorn:%s %-12.12s  %s|",Fcol(CYN),Fcol(WHITE),ACCESS(USER(rec)->Access),Fcol(CYN),Fcol(WHITE),rec,Fcol(CYN),Fcol(WHITE),jdate(USER(rec)->BirthDate),Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"|  %sClass:%s %-9.9s       %sGender:%s %-6.6s     %sNeed:%s %-13.13s %s|",Fcol(CYN),Fcol(WHITE),CLASS(USER(rec)->Class),Fcol(CYN),Fcol(WHITE),(USER(rec)->Sex=='M') ? "Male" : (USER(rec)->Sex=='F') ? "Female" : "None",Fcol(CYN),Fcol(WHITE),Fnum(ExpNeed(USER(rec)->Level)),Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"|  %sLevel:%s%s           %sImmortal:%s %-5u       %sExp:%s %-13.13s %s|",Fcol(CYN),Fcol(WHITE),RankLevel(rec),Fcol(CYN),Fcol(WHITE),USER(rec)->Immortal,Fcol(CYN),Fcol(WHITE),Fnum(USER(rec)->Experience),Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(line[0],"%u of %u",USER(rec)->CallsToday,ACCESS(USER(rec)->Access)->Calls);
	i=ANSI;
	ANSI=0;
	l1=30-strlen(money(USER(rec)->Gold,user));
	ANSI=i;
	sprintf(OUTbuf,"|   %sCall:%s %-10.10s       %sMoney:%s %s%*s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(CYN),Fcol(WHITE),money(USER(rec)->Gold,user),l1,"",Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(line[0],"%u of %u",NMEhp,USER(rec)->HP);
	i=ANSI;
	ANSI=0;
	l1=30-strlen(money(USER(rec)->Bank,user));
	ANSI=i;
	sprintf(OUTbuf,"|     %sHP:%s %-12.12s      %sBank:%s %s%*s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(CYN),Fcol(WHITE),money(USER(rec)->Bank,user),l1,"",Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->SP)
		sprintf(line[0],"%u of %u",NMEsp,USER(rec)->SP);
	else
		strcpy(line[0],"None");
	i=ANSI;
	ANSI=0;
	l1=30-strlen(money(USER(rec)->Loan,user));
	ANSI=i;
	sprintf(OUTbuf,"|     %sSP:%s %-12.12s      %sLoan:%s %s%*s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(CYN),Fcol(WHITE),money(USER(rec)->Loan,user),l1,"",Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(line[0],"%u of %u",NMEstr,USER(rec)->Str);
	sprintf(line[1],"%u of %u",NMEint,USER(rec)->Int);
	sprintf(OUTbuf,"|    %sStr:%s %-10.10s         %sInt:%s %-10.10s                     %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(CYN),Fcol(WHITE),line[1],Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(line[0],"%u of %u",NMEagl,USER(rec)->Agl);
	sprintf(line[1],"%u of %u",NMEchr,USER(rec)->Chr);
	sprintf(OUTbuf,"|    %sAgl:%s %-10.10s         %sChr:%s %-10.10s                     %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(CYN),Fcol(WHITE),line[1],Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(NMEhit)
		sprintf(line[0],"%s (%+d)",WEAPON(USER(rec)->Weapon)->Name,NMEhit);
	else
		strcpy(line[0],WEAPON(USER(rec)->Weapon)->Name);
	sprintf(OUTbuf,"| %sWeapon:%s %-54.54s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(NMEshield)
		sprintf(line[0],"%s (%+d)",ARMOR(USER(rec)->Armor)->Name,NMEshield);
	else
		strcpy(line[0],ARMOR(USER(rec)->Armor)->Name);
	sprintf(OUTbuf,"|  %sArmor:%s %-54.54s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->Poison) {
		memset(line[0],0,sizeof(line[0]));
		strcpy(comma," ");
		for(i=0; i<MAXPOISON; i++)
			if((USER(rec)->Poison & (UWORD)pow2((double)i))) {
				sprintf(text,"%s%d",comma,i+1);
				strcat(line[0],text);
				strcpy(comma,", ");
			}
	}
	else
		strcpy(line[0]," None");
	sprintf(OUTbuf,"| %sPoison:%s%-55.55s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(BLU));
	outs(OUTbuf);
	newline();
	memset(line[0],0,sizeof(line[0]));
	strcpy(text,(CLASS(USER(rec)->Class)->Magic>1) ? "Magic:" : "Wands:");
	if(USER(rec)->Spell) {
		strcpy(comma," ");
		for(i=0; i<MAXMAGIC; i++)
			if((USER(rec)->Spell & (UWORD)pow2((double)i))) {
				strcat(line[0],comma);
				if(strlen(line[0])+strlen(MAGIC(i)->Name)>56) {
					sprintf(OUTbuf,"|%s%8s%s%-56.56s%s|",Fcol(CYN),text,Fcol(WHITE),line[0],Fcol(BLU));
					outs(OUTbuf);
					newline();
					strcpy(line[0]," ");
					memset(text,0,sizeof(text));
				}
				strcat(line[0],MAGIC(i)->Name);
				strcpy(comma,", ");
			}
	}
	else
		strcpy(line[0]," None");
	sprintf(OUTbuf,"|%s%8s%s%-56.56s%s|",Fcol(CYN),text,Fcol(WHITE),line[0],Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(line[0],"%s protected by a %s",ESTATE(USER(rec)->RealEstate),SECURITY(USER(rec)->Security));
	sprintf(OUTbuf,"| %sDwells:%s %-54.54s %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"|  %sParty:%s %-54.54s %s|",Fcol(CYN),Fcol(WHITE),(USER(rec)->Gang) ? GANG(USER(rec)->Gang)->Name : "None",Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->Hull) {
		sprintf(line[0],"%u of %u",NMEhull,USER(rec)->Hull);
		sprintf(OUTbuf,"|   %sShip:%s %-12.12s    %sCannon:%s %3u         %sRam:%s %c             %s|",Fcol(CYN),Fcol(WHITE),line[0],Fcol(CYN),Fcol(WHITE),USER(rec)->Cannon,Fcol(CYN),Fcol(WHITE),USER(rec)->Ram,Fcol(BLU));
		outs(OUTbuf);
		newline();
	}
	sprintf(line[0],"%.8g",(double)USER(rec)->TotalPosts/USER(rec)->TotalCalls);
	sprintf(OUTbuf,"|  %sPosts:%s %-5u            %sCalls:%s %-5u     %sRatio:%s %-10.10s    %s|",Fcol(CYN),Fcol(WHITE),USER(rec)->TotalPosts,Fcol(CYN),Fcol(WHITE),USER(rec)->TotalCalls,Fcol(CYN),Fcol(WHITE),line[0],Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->Blessed) {
		sprintf(OUTbuf,"|   %sNote:%s You were blessed by %-26.26s         %s|",Fcol(CYN),Fcol(WHITE),USER(USER(rec)->Blessed)->Handle,Fcol(BLU));
		outs(OUTbuf);
		newline();
	}
	if(USER(rec)->Cursed) {
		sprintf(OUTbuf,"|   %sNote:%s You were cursed by %-26.26s          %s|",Fcol(CYN),Fcol(WHITE),USER(USER(rec)->Cursed)->Handle,Fcol(BLU));
		outs(OUTbuf);
		newline();
	}
	outs("+----------------------------------------------------------------+");
	newline();
	normal();
	if(More()=='N')
		return;
	i=strlen(USER(rec)->Handle);
	if(i>sizeof(USER(rec)->Handle))
		i=sizeof(USER(rec)->Handle);
	l1=20-i/2;
	l2=20-i/2+(i%2 ? 0 : 1);
	sprintf(OUTbuf,"%s+%.*s=:))%s%s %.26s %s%s((:=%.*s+",Fcol(BLU),l1,"--------------------",Bcol(BLU),Fcol(YELLOW),USER(rec)->Handle,Bcol(BLK),Fcol(BLU),l2,"--------------------");
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"| %sHall of Fame           %9.9s       All-Time   %s|",Fcol(CYN),CLASS(USER(rec)->Class)->Name,Fcol(BLU));
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"| %s  Notables  Current   Least   Most   Least   Most %s|",Fcol(CYN),Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->Calls<=FAME(USER(rec)->Class,0)->ValueLeast)
		sprintf(vl1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl1," ");
	if(USER(rec)->Calls>=FAME(USER(rec)->Class,0)->ValueMost)
		sprintf(vm1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm1," ");
	if(USER(rec)->Calls<=FAME(0,0)->ValueLeast)
		sprintf(vl2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl2," ");
	if(USER(rec)->Calls>=FAME(0,0)->ValueMost)
		sprintf(vm2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm2," ");
	sprintf(OUTbuf,"| %sCalls made:%s %7u  %5u%s %5u%s  %5u%s %5u%s %s|",Fcol(CYN),Fcol(WHITE),USER(rec)->Calls,FAME(USER(rec)->Class,0)->ValueLeast,vl1,FAME(USER(rec)->Class,0)->ValueMost,vm1,FAME(0,0)->ValueLeast,vl2,FAME(0,0)->ValueMost,vm2,Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->YouKill<=FAME(USER(rec)->Class,1)->ValueLeast)
		sprintf(vl1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl1," ");
	if(USER(rec)->YouKill>=FAME(USER(rec)->Class,1)->ValueMost)
		sprintf(vm1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm1," ");
	if(USER(rec)->YouKill<=FAME(0,1)->ValueLeast)
		sprintf(vl2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl2," ");
	if(USER(rec)->YouKill>=FAME(0,1)->ValueMost)
		sprintf(vm2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm2," ");
	sprintf(OUTbuf,"| %sKills made:%s %7u  %5u%s %5u%s  %5u%s %5u%s %s|",Fcol(CYN),Fcol(WHITE),USER(rec)->YouKill,FAME(USER(rec)->Class,1)->ValueLeast,vl1,FAME(USER(rec)->Class,1)->ValueMost,vm1,FAME(0,1)->ValueLeast,vl2,FAME(0,1)->ValueMost,vm2,Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->WasKill<=FAME(USER(rec)->Class,2)->ValueLeast)
		sprintf(vl1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl1," ");
	if(USER(rec)->WasKill>=FAME(USER(rec)->Class,2)->ValueMost)
		sprintf(vm1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm1," ");
	if(USER(rec)->WasKill<=FAME(0,2)->ValueLeast)
		sprintf(vl2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl2," ");
	if(USER(rec)->WasKill>=FAME(0,2)->ValueMost)
		sprintf(vm2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm2," ");
	sprintf(OUTbuf,"| %sWas killed:%s %7u  %5u%s %5u%s  %5u%s %5u%s %s|",Fcol(CYN),Fcol(WHITE),USER(rec)->WasKill,FAME(USER(rec)->Class,2)->ValueLeast,vl1,FAME(USER(rec)->Class,2)->ValueMost,vm1,FAME(0,2)->ValueLeast,vl2,FAME(0,2)->ValueMost,vm2,Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->YouKO<=FAME(USER(rec)->Class,3)->ValueLeast)
		sprintf(vl1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl1," ");
	if(USER(rec)->YouKO>=FAME(USER(rec)->Class,3)->ValueMost)
		sprintf(vm1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm1," ");
	if(USER(rec)->YouKO<=FAME(0,3)->ValueLeast)
		sprintf(vl2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl2," ");
	if(USER(rec)->YouKO>=FAME(0,3)->ValueMost)
		sprintf(vm2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm2," ");
	sprintf(OUTbuf,"|  %sKnockouts:%s %7u  %5u%s %5u%s  %5u%s %5u%s %s|",Fcol(CYN),Fcol(WHITE),USER(rec)->YouKO,FAME(USER(rec)->Class,3)->ValueLeast,vl1,FAME(USER(rec)->Class,3)->ValueMost,vm1,FAME(0,3)->ValueLeast,vl2,FAME(0,3)->ValueMost,vm2,Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->WasKO<=FAME(USER(rec)->Class,4)->ValueLeast)
		sprintf(vl1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl1," ");
	if(USER(rec)->WasKO>=FAME(USER(rec)->Class,4)->ValueMost)
		sprintf(vm1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm1," ");
	if(USER(rec)->WasKO<=FAME(0,4)->ValueLeast)
		sprintf(vl2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl2," ");
	if(USER(rec)->WasKO>=FAME(0,4)->ValueMost)
		sprintf(vm2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm2," ");
	sprintf(OUTbuf,"|  %sWas KO-ed:%s %7u  %5u%s %5u%s  %5u%s %5u%s %s|",Fcol(CYN),Fcol(WHITE),USER(rec)->WasKO,FAME(USER(rec)->Class,4)->ValueLeast,vl1,FAME(USER(rec)->Class,4)->ValueMost,vm1,FAME(0,4)->ValueLeast,vl2,FAME(0,4)->ValueMost,vm2,Fcol(BLU));
	outs(OUTbuf);
	newline();
	if(USER(rec)->JoustWin>=FAME(USER(rec)->Class,5)->ValueLeast)
		sprintf(vl1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl1," ");
	if(USER(rec)->JoustLoss>=FAME(USER(rec)->Class,5)->ValueMost)
		sprintf(vm1,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm1," ");
	if(USER(rec)->JoustWin>=FAME(0,5)->ValueLeast)
		sprintf(vl2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vl2," ");
	if(USER(rec)->JoustLoss>=FAME(0,5)->ValueMost)
		sprintf(vm2,"%s*%s",Fcol(YELLOW),Fcol(WHITE));
	else
		strcpy(vm2," ");
	sprintf(line[0],"%3u-%-u",USER(rec)->JoustWin,USER(rec)->JoustLoss);
	sprintf(line[1],"%5u%s-%5u%s",FAME(USER(rec)->Class,5)->ValueLeast,vl1,FAME(USER(rec)->Class,5)->ValueMost,vm1);
	sprintf(line[2],"%5u%s-%5u%s",FAME(0,5)->ValueLeast,vl2,FAME(0,5)->ValueMost,vm2);
	sprintf(OUTbuf,"| %sJousts W-L:%s %7s  %s  %s %s|",Fcol(CYN),Fcol(WHITE),line[0],line[1],line[2],Fcol(BLU));
	outs(OUTbuf);
	newline();
	outs("+---------------------------------------------------+");
	newline();
	normal();
}

void RestartChar(int rec)
{
	int i;

	USER(rec)->Status=0;
	USER(rec)->Class=0;
	USER(rec)->Level=1;
	USER(rec)->ExpLevel=1;
	USER(rec)->Experience=0.;
	USER(rec)->Gold=1.;
	USER(rec)->Bank=5.;
	USER(rec)->Loan=0.;
	USER(rec)->HP=15;
	USER(rec)->SP=0;
	USER(rec)->Str=50;
	USER(rec)->Int=50;
	USER(rec)->Agl=50;
	USER(rec)->Chr=50;
	USER(rec)->Weapon=1;
	USER(rec)->Armor=1;
	USER(rec)->RealEstate=0;
	USER(rec)->Security=0;
	USER(rec)->Spell=0;
	USER(rec)->Poison=0;
	USER(rec)->JoustWin=0;
	USER(rec)->JoustLoss=0;
	USER(rec)->Hull=0;
	USER(rec)->Cannon=0;
	USER(rec)->Ram='N';
	USER(rec)->Calls=1;
	USER(rec)->YouKill=0;
	USER(rec)->WasKill=0;
	USER(rec)->YouKO=0;
	USER(rec)->WasKO=0;
	USER(rec)->BountyAmount=0.;
	USER(rec)->BountyWho=0;
	PutUser(rec);

	for(i=1; i<=MAXUSER; i++)
		if(USER(i)->BountyWho==rec) {
			USER(i)->BountyWho=0;
			USER(i)->BountyAmount=0.;
			PutUser(i);
		}
}

void ReRoll(int max,int a)
{
	int i,j,p;

	RestartChar(user);
	echo=TRUE;
	do {
		newline();
		newline();
		outs("You must pick a class.");
		newline();
		Delay(50);
		outs(Fcol(CYN));
		newline();
		outs("      Character         (Recommended abilities + bonus)");
		newline();
		outs("        Class    Users    Str     Int     Agl     Chr       Special Feature");
		newline();
		outs("      ---------   ---    -----   -----   -----   -----   ---------------------");
		newline();
		for(i=1; i<MAXCLASS; i++) {
			p=0;
			for(j=1; j<=MAXUSER; j++)
				if(USER(j)->Class==i)
					p++;
			if(i<=max)
				sprintf(text,"%4s  %s%-9.9s   %3d    ",bracket(i),Fcol(WHITE),CLASS(i)->Name,p);
			else {
				normal();
				sprintf(text,"%s<%2u>  %-9.9s   %3d    ",Fcol(CYN),i,CLASS(i)->Name,p);
			}
			outs(text);
			sprintf(text,"%2u +%u   %2u +%u   %2u +%u   %2u +%u   ",CLASS(i)->Recommend[0],CLASS(i)->Str,CLASS(i)->Recommend[1],CLASS(i)->Int,CLASS(i)->Recommend[2],CLASS(i)->Agl,CLASS(i)->Recommend[3],CLASS(i)->Chr);
			outs(text);
			outs(CLASS(i)->Note);
			newline();
		}
		newline();
		sprintf(text,"%sEnter class (1-%d): ",Fcol(CYAN),max);
		outs(text);
		if(input())
			return;
		i=atoi(ins);
	} while(i<1 || i>max);
	USER(user)->Class=i;
	sprintf(text," - %s",CLASS(USER(user)->Class)->Name);
	outs(text);
	newline();
	newline();
	if(CLASS(USER(user)->Class)->Magic>1)
		USER(user)->SP=15;
	do {
		sprintf(text,"You have %d ability points to distribute between 4 abilities: Stamina,",a);
		outs(text);
		newline();
		outs("Intellect, Agility, and Charisma.  Each ability must be between 20 and 80.");
		newline();
		p=a;
		do {
			newline();
			sprintf(text,"You have %d points left.",p);
			outs(text);
			newline();
			newline();
			sprintf(text,"%sEnter your Stamina %s: ",Fcol(CYN),bracket(CLASS(USER(user)->Class)->Recommend[0]));
			outs(text);
			if(input())
				return;
			Tstr=atoi(ins);
			if(!Tstr) {
				Tstr=CLASS(USER(user)->Class)->Recommend[0];
				sprintf(text,"%u",Tstr);
				outs(text);
			}
			newline();
		} while(Tstr<20 || Tstr>80);
		p-=Tstr;
		do {
			newline();
			sprintf(text,"You have %d points left.",p);
			outs(text);
			newline();
			newline();
			sprintf(text,"%sEnter your Intellect %s: ",Fcol(CYN),bracket(CLASS(USER(user)->Class)->Recommend[1]));
			outs(text);
			if(input())
				return;
			Tint=atoi(ins);
			if(!Tint) {
				Tint=CLASS(USER(user)->Class)->Recommend[1];
				sprintf(text,"%u",Tint);
				outs(text);
			}
			newline();
		} while(Tint<20 || Tint>80);
		p-=Tint;
		do {
			newline();
			sprintf(text,"You have %d points left.",p);
			outs(text);
			newline();
			newline();
			sprintf(text,"%sEnter your Agility %s: ",Fcol(CYN),bracket(CLASS(USER(user)->Class)->Recommend[2]));
			outs(text);
			if(input())
				return;
			Tagl=atoi(ins);
			if(!Tagl) {
				Tagl=CLASS(USER(user)->Class)->Recommend[2];
				sprintf(text,"%u",Tagl);
				outs(text);
			}
			newline();
		} while(Tagl<20 || Tagl>80);
		p-=Tagl;
		if(p<20 || p>80) {
			p=-1;
			continue;
		}
		do {
			newline();
			sprintf(text,"You have %d points left.",p);
			outs(text);
			newline();
			newline();
			sprintf(text,"%sEnter your Charisma %s: ",Fcol(CYN),bracket(p));
			outs(text);
			if(input())
				return;
			Tchr=atoi(ins);
			if(!Tchr) {
				Tchr=p;
				sprintf(text,"%u",p);
				outs(text);
			}
			newline();
		} while(Tchr<20 || Tchr>80);
		p-=Tchr;
	} while(p);
	USER(user)->Str=Tstr;
	USER(user)->Int=Tint;
	USER(user)->Agl=Tagl;
	USER(user)->Chr=Tchr;
	PutUser(user);
	RestoreStat();
	Thull=USER(user)->Hull;
}

void MainMenu(void)
{
	static double v[51];
	static int sub[51];

	FILE *fp;
	int chat,i,j,k,max,rob;
	char keys[3],c;

	chat=4;
	cuss=0;
	rob=0;
	while(!cd()) {
		RPGserver('U',0);
		if(USER(user)->Class==0) {
			newline();
			outs("You have been rerolled.");
			ReRoll(MAXCLASS-4,200);
		}
		while(USER(user)->Experience>=ExpNeed(USER(user)->Level) && acclvl) {
			USER(user)->Level++;
			USER(user)->ExpLevel=USER(user)->Level;
			sprintf(text,"%s-=%s>%s*%s<%s=-",Fcol(MAG),Fcol(BLU),Fcol(YELLOW),Fcol(BLU),Fcol(MAG));
			outs(text);
			newline();
			newline();
			sprintf(text,"%sLevel = %d",Fcol(WHITE),USER(user)->Level);
			outs(text);
			normal();
			newline();
			newline();
			RPGserver('R',0);
			if(ACCESS(USER(user)->Access)->Level==USER(user)->Level) {
				USER(user)->Access++;
				sprintf(text,"%sThe king is pleased by your accomplishments and makes you a %s!",Fcol(YELLOW),ACCESS(USER(user)->Access)->Name);
				outs(text);
				normal();
				newline();
				newline();
			}
			if(USER(user)->Level>MAXLEVEL) {
				USER(user)->Immortal++;
				if(USER(user)->KeySequence<1 || USER(user)->KeySequence>6)
					USER(user)->KeySequence=dice(6);
				switch(USER(user)->KeySequence) {
					case 1:
						correct[0]='G';
						correct[1]='S';
						correct[2]='C';
						break;
					case 2:
						correct[0]='G';
						correct[1]='C';
						correct[2]='S';
						break;
					case 3:
						correct[0]='S';
						correct[1]='G';
						correct[2]='C';
						break;
					case 4:
						correct[0]='S';
						correct[1]='C';
						correct[2]='G';
						break;
					case 5:
						correct[0]='C';
						correct[1]='G';
						correct[2]='S';
						break;
					case 6:
						correct[0]='C';
						correct[1]='S';
						correct[2]='G';
						break;
				}
				HallOfFame();
				max=USER(user)->Class<MAXCLASS-3 ? MAXCLASS-3 : USER(user)->Class==MAXCLASS-3 ? MAXCLASS-2 : MAXCLASS-1;
				RestartChar(user);
				outs("You have become so powerful that you are now immortal.");
				newline();
				outs(Fcol(CYN));
				outs("Press RETURN to start over in this world: ");
				if(input())
					break;
				newline();
				newline();
				outs(Fcol(GRY));
				outs("Ol' Mighty One!  Solve the Ancient Riddle of the Keys and you will become");
				newline();
				sprintf(text,"a %s.  You have a %sGold%s, %sSilver%s, and %sCopper%s key that must be",CLASS(max)->Name,Fcol(YELLOW),Fcol(GRY),Fcol(CYAN),Fcol(GRY),Fcol(BRED),Fcol(GRY));
				outs(text);
				newline();
				sprintf(text,"inserted in the proper order to unleash the Power of the %s!",CLASS(max)->Name);
				outs(text);
				newline();
				newline();
				keys[0]='G';
				keys[1]='S';
				keys[2]='C';
				for(i=0; i<3;) {
					sprintf(text,"%s[%c,%c,%c] Insert key #%d: ",Fcol(CYN),keys[0],keys[1],keys[2],i+1);
					outs(text);
					if(input())
						break;
					c=toupper(ins[0]);
					if((c!=keys[0] && c!=keys[1] && c!=keys[2]) || c=='+') {
						outs(" { Be careful, stupid!  You're playing with fire! }");
						newline();
						continue;
					}
					outs("...you insert and turn the key...");
					Delay(100);
					if(c==correct[i]) {
						outs("{Click!}");
						newline();
						keys[(correct[i]=='G') ? 0 : (correct[i]=='S') ? 1 : 2]='+';
						i++;
						if(i==3) {
							outs(Fcol(YELLOW));
							outs("A powerful, shining aura surrounds you...");
							normal();
							Delay(100);
							newline();
							USER(user)->KeySequence=dice(6);
						}
					}
					else {
						outs(Fcol(BLUE));
						outs("^> Boom! <^");
						normal();
						newline();
						if(max>MAXCLASS-3) {
							sprintf(text,"The correct sequence was: %c,%c,%c",correct[0],correct[1],correct[2]);
							USER(user)->KeySequence=dice(6);
						}
						else
							sprintf(text,"Remember to try a different key in this slot the next time you roll.");
						outs(text);
						newline();
						Delay(250);
						if(BBSprefs.Keep)
							max--;
						else
							max=MAXCLASS-4;
						break;
					}
				}
				i=200;
				i+=dice(USER(user)->Immortal);
				if(i>240)
					i=240;
				ReRoll(max,i);
				break;
			}
			i=USER(user)->Str/10+dice(USER(user)->Level)+USER(user)->Level;
			USER(user)->HP+=i;
			sprintf(text,"Hit points = Hit points + %d",i);
			outs(text);
			newline();
			newline();
			if(CLASS(USER(user)->Class)->Magic>1) {
				i=USER(user)->Int/10+dice(USER(user)->Level)+USER(user)->Level;
				USER(user)->SP+=i;
				sprintf(text,"Spell power = Spell power + %d",i);
				outs(text);
				newline();
				newline();
			}
			i=CLASS(USER(user)->Class)->Str;
			if(USER(user)->Str+i>99)
				i=99-USER(user)->Str;
			if(i) {
				USER(user)->Str+=i;
				sprintf(text,"Stamina = Stamina + %d",i);
				outs(text);
				newline();
				newline();
			}
			i=CLASS(USER(user)->Class)->Int;
			if(USER(user)->Int+i>99)
				i=99-USER(user)->Int;
			if(i) {
				USER(user)->Int+=i;
				sprintf(text,"Intellect = Intellect + %d",i);
				outs(text);
				newline();
				newline();
			}
			i=CLASS(USER(user)->Class)->Agl;
			if(USER(user)->Agl+i>99)
				i=99-USER(user)->Agl;
			if(i) {
				USER(user)->Agl+=i;
				sprintf(text,"Agility = Agility + %d",i);
				outs(text);
				newline();
				newline();
			}
			i=CLASS(USER(user)->Class)->Chr;
			if(USER(user)->Chr+i>99)
				i=99-USER(user)->Chr;
			if(i) {
				USER(user)->Chr+=i;
				sprintf(text,"Charisma = Charisma + %d",i);
				outs(text);
				newline();
				newline();
			}
			PutUser(user);
			RestoreStat();
			Delay(50);
		}
		Midnight();
		newline();
		outs(RPGinfo->Border);
		newline();
		newline();
		sprintf(text,"%sTime Left:%s %d min.",Fcol(MAG),Fcol(GRY),TimeLeft());
		outs(text);
		newline();
		sprintf(text,"%s[%sMain%s]%s Command (?=Menu): ",Fcol(BLU),Fcol(YELLOW),Fcol(BLU),Fcol(CYN));
		outs(text);
		c=inkey();
		newline();
		newline();
		from='M';

		switch(c) {
			case '?':
				cls();
				MenuFile("MainMenu");
				break;
			case '@':
				if(ACCESS(USER(user)->Access)->Sysop=='Y')
					Sysop();
				break;
			case 'A':
				if(!acclvl)
					break;
				Arena();
				break;
			case 'B':
				if(ACCESS(USER(user)->Access)->Bulletin=='Y')
					Bulletin();
				else {
					outs("Access denied.");
					newline();
				}
				break;
			case 'C':
				if(chat>2) {
					outs("Paging the sysop...");
					PlaySound(ChatKey);
					outs("Ok.  The sysop may come online later.");
				}
				if(chat==2) {
					outs("The sysop has already been notified of your chat request.");
					newline();
					outs("Repeated requests are unnecessary and annoying.");
				}
				if(chat==1) {
					outs("FINAL WARNING: Another chat request will become a quick logoff.");
				}
				if(!chat) {
					outs("You had been warned, but you chose to ignore it...");
					strcpy(reason,"for chatting too much");
					logoff=TRUE;
				}
				newline();
				chat--;
				break;
			case 'D':
				if(!acclvl)
					break;
				if(ACCESS(USER(user)->Access)->Dungeon=='Y')
					Dungeon(USER(user)->Level);
				else {
					outs("Access denied.");
					newline();
				}
				break;
			case 'E':
				outs("Send mail to whom (<S>ysop,?=list)? ");
				i=GetUser();
				if(stricmp(ins,"S")==0)
					i=1;
				if(i==0)
					break;
				Email(i);
				break;
			case 'F':
				if(!acclvl && ACCESS(USER(user)->Access)->Sysop!='Y')
					break;
				if(ACCESS(USER(user)->Access)->File=='Y')
					FileSection();
				else {
					outs("Access denied.");
					newline();
				}
				break;
			case 'G':
				if(!acclvl)
					break;
				Casino();
				break;
			case 'H':
				Hints();
				break;
			case 'I':
				outs("Top 50 All-Time Users.");
				newline();
				newline();
				k=0;
				for(i=1; i<=MAXUSER; i++) {
					if(USER(i)->Access) {
						sub[k]=i;
						j=100*USER(i)->Immortal+USER(i)->Level;
						v[k]=j;
						j=k;
						while(j>0 && v[j]>v[j-1]) {
							swmem(&v[j],&v[j-1],sizeof(double));
							swmem(&sub[j],&sub[j-1],sizeof(int));
							j--;
						}
						if(k<50)
							k++;
					}
				}
				sprintf(OUTbuf,"%s%sNum           Username           Immortal  Lvl  Calls  Avg   Access %s",Fcol(WHITE),Bcol(RED),Bcol(BLK));
				outs(OUTbuf);
				newline();
				sprintf(OUTbuf,"%s---  --------------------------  --------   --  -----  ---  --------%s",Bcol(RED),Bcol(BLK));
				outs(OUTbuf);
				newline();
				normal();
				for(i=0; i<k; i++) {
					d=(USER(sub[i])->TotalCalls) ? v[i]/USER(sub[i])->TotalCalls : 0;
					sprintf(OUTbuf,"%3d  %-26.26s  %7u   %s %5d %4.1f  %s",sub[i],USER(sub[i])->Handle,USER(sub[i])->Immortal,RankLevel(sub[i]),USER(sub[i])->TotalCalls,d,ACCESS(USER(sub[i])->Access)->Name);
					outs(OUTbuf);
					newline();
					if((i+1)%20==0)
						if(More()=='N')
							break;
				}
				break;
			case 'J':
				outs("50 top users, sorted by joust wins.");
				newline();
				newline();
				k=0;
				for(i=1; i<=MAXUSER; i++) {
					if(USER(i)->Access) {
						sub[k]=i;
						v[k]=USER(i)->JoustWin;
						j=k;
						while(j>0 && v[j]>v[j-1]) {
							swmem(&v[j],&v[j-1],sizeof(double));
							swmem(&sub[j],&sub[j-1],sizeof(int));
							j--;
						}
						if(k<50)
							k++;
					}
				}
				sprintf(OUTbuf,"%s%sNum           Username            Win-Loss  Ratio  Lvl   Access %s",Fcol(WHITE),Bcol(MAG),Bcol(BLK));
				outs(OUTbuf);
				newline();
				sprintf(OUTbuf,"%s---  --------------------------  ---------  -----   --  --------%s",Bcol(MAG),Bcol(BLK));
				outs(OUTbuf);
				newline();
				normal();
				for(i=0; i<k; i++) {
					j=(USER(sub[i])->JoustWin+USER(sub[i])->JoustLoss>0) ? 100*USER(sub[i])->JoustWin/(USER(sub[i])->JoustWin+USER(sub[i])->JoustLoss) : 0;
					sprintf(text,"%3d  %-26.26s  %4d-%-4d  %4d\%  %s %s",sub[i],USER(sub[i])->Handle,USER(sub[i])->JoustWin,USER(sub[i])->JoustLoss,j,RankLevel(sub[i]),ACCESS(USER(sub[i])->Access)->Name);
					outs(text);
					newline();
					if((i+1)%20==0)
						if(More()=='N')
							break;
				}
				break;
			case 'K':
				TypeFile("Last50",TRUE);
				break;
			case 'L':
				ListUsers();
				break;
			case 'M':
				outs("Current 50 top users, sorted by experience.");
				newline();
				newline();
				k=0;
				for(i=1; i<=MAXUSER; i++) {
					if(USER(i)->Access) {
						sub[k]=i;
						v[k]=USER(i)->Experience;
						j=k;
						while(j>0 && v[j]>v[j-1]) {
							swmem(&v[j],&v[j-1],sizeof(double));
							swmem(&sub[j],&sub[j-1],sizeof(int));
							j--;
						}
						if(k<50)
							k++;
					}
				}
				sprintf(OUTbuf,"%s%sNum           Username             Class   Lvl  Status         Party        %s",Fcol(WHITE),Bcol(GRN),Bcol(BLK));
				outs(OUTbuf);
				newline();
				sprintf(OUTbuf,"%s---  --------------------------  ---------  --  ------  --------------------%s",Bcol(GRN),Bcol(BLK));
				outs(OUTbuf);
				newline();
				normal();
				for(i=0; i<k; i++) {
					sprintf(text,"%3d  %-26.26s  %-9s %s %s  ",sub[i],USER(sub[i])->Handle,CLASS(USER(sub[i])->Class)->Name,RankLevel(sub[i]),(USER(sub[i])->Status ? " Dead " : "Alive!"));
					outs(text);
					if(USER(sub[i])->Gang && USER(sub[i])->Gang==USER(user)->Gang) {
						sprintf(text,"%s%s%.20s%s%s",Bcol(BLU),Fcol(WHITE),GANG(USER(sub[i])->Gang)->Name,Bcol(BLK),Fcol(GRY));
						outs(text);
						normal();
					}
					else {
						sprintf(text,"%.20s",USER(sub[i])->Gang ? GANG(USER(sub[i])->Gang)->Name : "None");
						outs(text);
					}
					newline();
					if((i+1)%20==0)
						if(More()=='N')
							break;
				}
				break;
			case 'N':
				if(!acclvl)
					break;
				if(ACCESS(USER(user)->Access)->Naval=='Y')
					Naval();
				else {
					outs("Access denied.");
					newline();
				}
				break;
			case 'P':
				if(!acclvl)
					break;
				if(ACCESS(USER(user)->Access)->Party=='Y')
					Party();
				else {
					outs("Access denied.");
					newline();
				}
				break;
			case 'R':
				if(!acclvl)
					break;
				if(ACCESS(USER(user)->Access)->User!='Y') {
					outs("Access denied.");
					newline();
					break;
				}
				outs("It is a hot, moonless night.");
				newline();
				outs("A city guard walks down another street.");
				newline();
				newline();
				modf((value(WEAPON(USER(user)->Weapon)->Cost+ARMOR(USER(user)->Armor)->Cost,Tchr)+USER(user)->Gold+USER(user)->Bank-USER(user)->Loan)/5.,&v[0]);
				outs(Fcol(CYN));
				outs("Who are you going to rob? ");
				nme=GetUser();
				if(nme<1 || nme>MAXUSER || nme==user)
					break;
				if((BYTE)(USER(user)->Level-USER(nme)->Level)>3) {
					outs("You can only rob someone higher or up to three levels below you.");
					newline();
					break;
				}
				modf(value(WEAPON(USER(nme)->Weapon*4/5)->Cost+ARMOR(USER(nme)->Armor*4/5)->Cost,Tchr)+USER(nme)->Gold,&v[1]);
				sprintf(text,"You case %.26s's joint out.",USER(nme)->Handle);
				outs(text);
				newline();
				if(dice(100)<=Tint && v[1]<v[0]) {
					outs("But you decide it is not worth the effort.");
					newline();
					break;
				}
				sprintf(text,"The goods are in a %s protected by a %s.",ESTATE(USER(nme)->RealEstate)->Name,SECURITY(USER(nme)->Security)->Name);
				outs(text);
				newline();
				newline();
				outs(Fcol(CYN));
				outs("Attempt to steal (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				if(c=='Y') {
					newline();
					outs("You slide into the shadows and make your attempt...");
					Delay(150);
					newline();
					max=5*(USER(nme)->Security+1)+USER(nme)->RealEstate;
					if(ACCESS(USER(user)->Access)->Sysop!='Y')
						max+=(USER(nme)->Level-USER(user)->Level)/3+(USER(user)->Level/10+1)*rob;
					j=0;
					for(i=0; i<CLASS(USER(user)->Class)->ToSteal; i++)
						j+=dice(USER(user)->Level+1);
					if(j>max) {
						USER(user)->Gold+=v[1];
						PutUser(user);
						USER(nme)->Gold=0.;
						USER(nme)->Weapon=USER(nme)->Weapon*4/5;
						USER(nme)->Armor=USER(nme)->Armor*4/5;
						PutUser(nme);
						numline=0;
						sprintf(line[numline++],"%.26s robbed you!",(dice(100)>USER(nme)->Int) ? "Somebody" : USER(user)->Handle);
						UserNote(nme);
						sprintf(text,"You make it out with %s worth of stuff!",money(v[1],user));
						outs(text);
						newline();
						sprintf(filename,"News.%lu",JulianDate);
						if((fp=fopen(filename,"a"))) {
							fprintf(fp,"          %.26s robbed %.26s.\n",USER(user)->Handle,USER(nme)->Handle);
							fclose(fp);
						}
						rob++;
					}
					else {
						beep();
						outs("A guard catches you and throws you into jail!");
						newline();
						outs("You might be released by your next call.");
						newline();
						numline=0;
						sprintf(line[numline++],"A city guard caught %.26s trying to rob you!",(dice(100)>USER(nme)->Int) ? "somebody" : USER(user)->Handle);
						UserNote(nme);
						sprintf(reason,"caught robbing %.26s",USER(nme)->Handle);
						logoff=TRUE;
					}
				}
				break;
			case 'S':
				if(!acclvl)
					break;
				Square();
				break;
			case 'T':
				if(ACCESS(USER(user)->Access)->Tavern=='Y') {
					if(cuss<3)
						Tavern();
					else {
						outs("Tiny's is closed for memorial services.");
						newline();
					}
				}
				else {
					outs("Access denied.");
					newline();
				}
				break;
			case 'U':
				sprintf(text,"%sANSI emulation is:%s %s",Fcol(CYN),Fcol(WHITE),USER(user)->ANSI ? "ON" : "OFF");
				outs(text);
				newline();
				sprintf(text,"%sExpert mode is:%s %s",Fcol(CYN),Fcol(WHITE),USER(user)->Expert ? "ON" : "OFF");
				outs(text);
				newline();
				sprintf(text,"%sFormat money is:%s %s",Fcol(CYN),Fcol(WHITE),USER(user)->Format ? "ON - platinum,gold,silver,copper" : "OFF - scientific");
				outs(text);
				newline();
				sprintf(text,"%sMaximum screen rows:%s %d",Fcol(CYN),Fcol(WHITE),USER(user)->Rows+1);
				outs(text);
				newline();
				outs("Autowrap at end of line should be ON.");
				newline();
				newline();
				outs(Fcol(CYN));
				outs("<A>NSI emulation, <E>xpert toggle, <F>ormat toggle, <P>assword change, <R>ows: ");
				if(!(c=inkey()))
					break;
				newline();
				if(c=='A') {
					if(USER(user)->ANSI>2)
						USER(user)->ANSI=1;
					sprintf(text,"%s ASCII text",bracket(0));
					outs(text);
					newline();
					sprintf(text,"%s VT100 series",bracket(1));
					outs(text);
					newline();
					sprintf(text,"%s ANSI color",bracket(2));
					outs(text);
					newline();
					newline();
					outs(Fcol(CYN));
					outs("Select (0-2)? ");
					if(input())
						break;
					newline();
					i=atoi(ins);
					if(i>=0 && i<=2) {
						USER(user)->ANSI=i;
						ANSI=USER(user)->ANSI;
					}
					sprintf(text,"ANSI emulation is now:%s %s",bracket(USER(user)->ANSI),USER(user)->ANSI ? "ON" : "OFF");
					outs(text);
					newline();
				}
				if(c=='E') {
					USER(user)->Expert=(USER(user)->Expert ? FALSE : TRUE);
					newline();
					sprintf(text,"Expert mode is now: %s",USER(user)->Expert ? "ON" : "OFF");
					outs(text);
					newline();
				}
				if(c=='F') {
					USER(user)->Format=(USER(user)->Format ? FALSE : TRUE);
					newline();
					sprintf(text,"Format money mode is now: %s",USER(user)->Format ? "ON" : "OFF");
					outs(text);
					newline();
				}
				if(c=='P') {
					USER(user)->Password[0]='A'+dice(26)-1;
					USER(user)->Password[1]='A'+dice(26)-1;
					USER(user)->Password[2]='0'+dice(10)-1;
					USER(user)->Password[3]='0'+dice(10)-1;
					USER(user)->Password[4]='0'+dice(10)-1;
					sprintf(text,"%sYour new password is:%s %.5s",Fcol(CYN),Fcol(WHITE),USER(user)->Password);
					outs(text);
					newline();
				}
				if(c=='R') {
					for(i=50; i>1; i--) {
						sprintf(text,"%s",bracket(i));
						outs(text);
						newline();
					}
					outs("Enter the number that appears at top of screen now: ");
					if(input())
						break;
					newline();
					i=atoi(ins);
					if(i>9 && i<51)
						USER(user)->Rows=i-1;
				}
				PutUser(user);
				break;
			case 'X':
				if(!acclvl)
					break;
				outs(Fcol(CYN));
				outs("Reroll your character (y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				if(c=='Y') {
					from=USER(user)->Class;
					ReRoll(MAXCLASS-4,200);
					sprintf(filename,"News.%lu",JulianDate);
					if((fp=fopen(filename,"a"))) {
						fprintf(fp,"          %.26s exterminated from %s to %s.\n",USER(user)->Handle,CLASS(from)->Name,CLASS(USER(user)->Class)->Name);
						fclose(fp);
					}
				}
				break;
			case 'Y':
				JOUST(USER(user)->Level);
				modf(d/2.,&d);
				if(USER(user)->Gold>=d && acclvl) {
					sprintf(text,"%sHire a scout for %s (Y/N)? ",Fcol(CYN),money(d,user));
					outs(text);
					if(!(c=inkey()))
						break;
					newline();
					if(c=='Y') {
						outs("Scout which user? ");
						i=GetUser();
						if(i<1)
							break;
						USER(user)->Gold-=d;
						PutUser(user);
						CharacterStats(i);
						break;
					}
				}
				CharacterStats(user);
				break;
			case 'Z':
				sprintf(text,"     %s///",Fcol(MAGENTA));
				outs(text);
				newline();
				sprintf(text,"    %s///%s  %s-=> RPGBBS <=-%s %scan run on any Commodore Amiga hardware configuration.",Fcol(BLUE),Fcol(WHITE),Bcol(RED),Bcol(BLK),Fcol(GRY));
				outs(text);
				newline();
				sprintf(text,"%s\\\\\\%s///%s   For more information:            Licensed to:",Fcol(BRED),Fcol(GRN),Fcol(GRY));
				outs(text);
				newline();
				sprintf(text," %s\\\\//%s",Fcol(YELLOW),Fcol(GRY));
				outs(text);
				newline();
				sprintf(text,"         Robert Hurst                     %s",USER(0)->Street[0]);
				outs(text);
				newline();
				sprintf(line[0],"%010f",USER(0)->VoicePhone);
				sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
				sprintf(text,"         BBS: %s                %s",line[1],USER(0)->Street[1]);
				outs(text);
				newline();
				outs("         Mark Montminy");
				newline();
				sprintf(line[0],"%010f",USER(0)->DataPhone);
				sprintf(line[1],"%.3s-%.3s-%.4s",&line[0][0],&line[0][3],&line[0][6]);
				sprintf(text,"         Door: %s",line[1]);
				outs(text);
				newline();
				if(ACCESS(USER(user)->Access)->User!='Y')
					break;
				newline();
				outs(Fcol(CYN));
				outs("<C> Change border message, <H> Hall of Fame: ");
				if(!(c=inkey()))
					break;
				newline();
				switch(c) {
					case 'C':
						if(!acclvl)
							break;
						newline();
						outs(Fcol(CYN));
						outs("Enter new border message:");
						newline();
						if(input())
							break;
						newline();
						if(strlen(ins))
							strncpy(RPGinfo->Border,ins,sizeof(RPGinfo->Border)-1);
						break;
					case 'H':
						DisplayHOF(0);
						FOREVER {
							newline();
							outs(Fcol(CYN));
							outs("View individual class Hall of Fame (Y/N)? ");
							if(!(c=inkey()))
								break;
							newline();
							if(c!='Y')
								break;
							newline();
							for(i=1; i<MAXCLASS; i++) {
								sprintf(text,"%4s %-10.10s",bracket(i),CLASS(i)->Name);
								outs(text);
								newline();
							}
							newline();
							outs(Fcol(CYN));
							outs("Enter class: ");
							if(input())
								break;
							newline();
							i=atoi(ins);
							if(i>=0 && i<MAXCLASS)
								DisplayHOF(i);
						}
						break;
				}
		}
	}
}
