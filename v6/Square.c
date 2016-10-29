/*******************\
 *  RPGBBS Square  *
\*******************/
#include "RPGBBS.h"

void BuySell(struct Store *Item,UBYTE MaxItem,UBYTE *UserItem,BYTE damage)
{
	int afford,i,lo,hi;
	char yn;

	if(*UserItem) {
		outs(Fcol(CYN));
		outs("<B>uy or <S>ell? ");
		if(!(yn=inkey()))
			return;
		newline();
	}
	else
		yn='B';
	if(yn!='B' && yn!='S')
		return;
	newline();
	sprintf(text,"You currently own %s %s",bracket(*UserItem),(Item+*UserItem)->Name);
	outs(text);
	if(damage) {
		sprintf(text," %s",points(damage));
		outs(text);
		if(damage>0)
			damage=0;
	}
	newline();
	if(yn=='S') {
		d=value((Item+*UserItem+damage)->Cost,Tchr);
		sprintf(text,"I'll give you %s for it.",money(d,user));
		outs(text);
		newline();
		outs(Fcol(CYN));
		outs("Do you accept (Y/N)? ");
		if(!(yn=inkey()))
			return;
		newline();
		if(yn!='Y')
			return;
		newline();
		*UserItem=0;
		USER(user)->Gold+=d;
		PutUser(user);
		outs("Done.");
		newline();
		return;
	}
	for(afford=0; afford<=MaxItem && (USER(user)->Gold+value((Item+*UserItem+damage)->Cost,Tchr))>=(Item+afford)->Cost; afford++);
	sprintf(text,"You can afford the first %u of %u items for sale.",--afford,MaxItem);
	outs(text);
	newline();
	newline();
	lo=*UserItem<afford ? *UserItem : afford;
	lo=(lo<1) ? 1 : (lo>MaxItem) ? MaxItem : lo;
	hi=*UserItem<afford ? afford : *UserItem;
	hi=(hi<1) ? 1 : (hi>MaxItem) ? MaxItem : hi;
	sprintf(text,"%sStart list at %s: ",Fcol(CYN),bracket(lo));
	outs(text);
	if(input())
		return;
	if(!strlen(ins)) {
		sprintf(text,"%d",lo);
		outs(text);
	}
	newline();
	i=atoi(ins);
	if(i>0)
		lo=i;
	sprintf(text,"%s  End list at %s: ",Fcol(CYN),bracket(hi));
	outs(text);
	if(input())
		return;
	if(!strlen(ins)) {
		sprintf(text,"%d",hi);
		outs(text);
	}
	newline();
	newline();
	i=atoi(ins);
	if(i>0)
		hi=i;
	if(lo<1 || lo>MaxItem || hi<1 || hi>MaxItem)
		return;
	for(i=lo; i<=hi; i++) {
		sprintf(text,"%s %s%-30s %s",bracket(i),Fcol(GRY),(Item+i)->Name,money((Item+i)->Cost,user));
		outs(text);
		newline();
	}
	newline();
	outs(Fcol(CYN));
	outs("Buy which? ");
	if(input())
		return;
	newline();
	i=atoi(ins);
	if(i<1 || i>afford)
		return;
	USER(user)->Gold+=value((Item+*UserItem+damage)->Cost,Tchr);
	*UserItem=i;
	USER(user)->Gold-=(Item+*UserItem)->Cost;
	PutUser(user);
	newline();
	outs("Done.");
	newline();
}

void Square(void)
{
	static struct RPGmenu __far RPGmenu = { "Square",BRN,10,
							{	{ 'A',"Armoury" },
								{ 'W',"Weapons Shoppe" },
								{ 'R',"Real Estate" },
								{ 'S',"Security" },
								{ 'M',"Mages Guild" },
								{ 'V',"Visit the Apothecary" },
								{ 'B',"Ye Olde Stone Bank" },
								{ 'H',"Butler Hospital" },
								{ 'P',"Pick pockets" },
								{ 'G',"Goto the arena" }
							}
	};
	static struct RPGmenu __far BANKmenu = { "Bank",GRN,5,
							{	{ 'D',"Deposit" },
								{ 'L',"Loan" },
								{ 'R',"Rob the bank" },
								{ 'S',"Status" },
								{ 'W',"Withdrawal" }
							}
	};
	double loan;
	int afford,i,mu;
	char c,item;

	if(USER(user)->Gold>0.) {
		while((nme=dice(MAXUSER))==user);
		NMEstat();	/*  if he is smarter than your luck...  */
		if(dice(USER(nme)->Level)*CLASS(USER(nme)->Class)->ToSteal/2>USER(user)->Level && dice(NMEint)>dice(Tchr)) {
			sprintf(OUTbuf,"You bump into %.26s on the outskirts of town.",USER(nme)->Handle);
			outs(OUTbuf);
			newline();
			Delay(50);
			if(dice(NMEagl)>dice(Tint)) {	/*  if he is quicker than your attention...  */
				USER(nme)->Gold+=USER(user)->Gold;
				PutUser(nme);
				numline=0;
				sprintf(line[numline++],"You picked %.26s's pocket and got %s!",USER(user)->Handle,money(USER(user)->Gold,nme));
				UserNote(nme);
				USER(user)->Gold=0.;
				PutUser(user);
			}
			sprintf(OUTbuf,"You exchange greetings with %s and go on your way.",NMEhim);
			outs(OUTbuf);
			newline();
			Delay(50);
		}
	}

	while(!cd()) {
		if((c=Option(&RPGmenu))=='Q')
			break;

		switch(c) {
			case 'A':
				item=USER(user)->Armor;
				BuySell(Armor,CLASS(USER(user)->Class)->AC,&USER(user)->Armor,Tshield);
				if(USER(user)->Armor!=item)
					Tshield=(USER(user)->Blessed) ? USER(user)->Armor/10 : (USER(user)->Cursed) ? -USER(user)->Armor/10 : 0;
				break;
			case 'B':
				while(!cd()) {
					if((c=Option(&BANKmenu))=='Q')
						break;
					switch(c) {
						case 'D':
							sprintf(text,"%sDeposit%s [MAX=%s]? ",Fcol(CYN),Fcol(GRY),money(USER(user)->Gold,user));
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
								d=USER(user)->Gold;
							if(d>0. && d<=USER(user)->Gold) {
								USER(user)->Gold-=d;
								if(USER(user)->Loan) {
									loan=(d<USER(user)->Loan) ? d : USER(user)->Loan;
									USER(user)->Loan-=loan;
									d-=loan;
								}
								USER(user)->Bank+=d;
								PutUser(user);
								newline();
								outs("Done.");
								newline();
							}
							break;
						case 'L':
							if(USER(user)->Level<50) {
								JOUST(USER(user)->Level);
								d*=3.;
							}
							else
								d=ESTATE(USER(user)->RealEstate)->Cost+SECURITY(USER(user)->Security)->Cost;
							d-=USER(user)->Loan;
							if(d<0.) {
								outs("Your credit is over its limit -- go kill some monsters.");
								newline();
								break;
							}
							sprintf(text,"%sBorrow how much%s [MAX=%s]? ",Fcol(CYN),Fcol(GRY),money(d,user));
							outs(text);
							if(input())
								break;
							newline();
							loan=atof(Fnum(atof(ins)));
							c=ins[strlen(ins)-1];
							switch(toupper(c)) {
								case 'C':
									loan*=1.;
									break;
								case 'S':
									loan*=1e+05;
									break;
								case 'G':
									loan*=1e+09;
									break;
								case 'P':
									loan*=1e+13;
									break;
							}
							if(stricmp(ins,"MAX")==0)
								loan=d;
							if(loan<1.)
								break;
							if(loan>d) {
								outs("Your credit is not THAT good!");
								newline();
								break;
							}
							USER(user)->Gold+=loan;
							USER(user)->Loan+=loan;
							PutUser(user);
							outs("Your loan has been approved.");
							newline();
							break;
						case 'R':
							d=USER(user)->Level+1;
							d=pow(d,8.);
							loan=d*dice(90)+d*dice(10);
							for(i=0; i<=MAXUSER; i++)
								loan+=USER(i)->Bank;
							outs("You attempt to sneak into the vault...");
							Delay(150);
							newline();
							newline();
							c=(USER(user)->Level/5+1)*CLASS(USER(user)->Class)->ToSteal+1;
							if(dice(100)>c) {
								beep();
								outs("A guard catches you and throws you into jail!");
								newline();
								outs("You might be released by your next call.");
								newline();
								Delay(100);
								strcpy(reason,"caught getting into the vault");
								logoff=TRUE;
								break;
							}
							sprintf(text,"You steal %s!",money(loan,user));
							outs(text);
							newline();
							Delay(150);
							newline();
							outs("You try to make your way out of the vault...");
							Delay(150);
							c/=2;
							if(dice(100)>c) {
								outs("something jingles!");
								newline();
								Delay(50);
								newline();
								beep();
								outs("A guard closes the vault door and you slowly suffocate!");
								newline();
								Delay(100);
								strcpy(reason,"caught getting out of the vault");
								logoff=TRUE;
								break;
							}
							newline();
							outs("You make good with your escape!");
							newline();
							numline=0;
							sprintf(line[numline++],"%.26s robbed the bank!",USER(user)->Handle);
							for(i=0; i<=MAXUSER; i++) {
								if(USER(i)->Access>0 && USER(i)->Bank>0.)
									UserNote(i);
								USER(i)->Bank=0.;
								PutUser(i);
							}
							USER(user)->Gold+=loan;
							PutUser(user);
							break;
						case 'S':
							sprintf(text,"%sMoney in hand:%s %s",Fcol(GRN),Fcol(WHITE),money(USER(user)->Gold,user));
							outs(text);
							newline();
							sprintf(text,"%sMoney in bank:%s %s",Fcol(GRN),Fcol(WHITE),money(USER(user)->Bank,user));
							outs(text);
							newline();
							sprintf(text,"%sMoney on loan:%s %s",Fcol(GRN),Fcol(WHITE),money(USER(user)->Loan,user));
							outs(text);
							newline();
							break;
						case 'W':
							sprintf(text,"%sWithdrawal%s [MAX=%s]? ",Fcol(CYN),Fcol(GRY),money(USER(user)->Bank,user));
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
								d=USER(user)->Bank;
							if(d>0. && d<=USER(user)->Bank) {
								USER(user)->Bank-=d;
								USER(user)->Gold+=d;
								PutUser(user);
								newline();
								outs("Done.");
								newline();
							}
							break;
					}
				}
				break;
			case 'G':
				if(from=='A')
					return;
				from='S';
				Arena();
				if(from=='M')
					return;
				break;
			case 'H':
				outs("Welcome to Butler Hospital.");
				newline();
				newline();
				sprintf(text,"Hit points cost %u each.",USER(user)->Level);
				outs(text);
				newline();
				sprintf(text,"You need %d hit points.",USER(user)->HP-Thp);
				outs(text);
				newline();
				afford=USER(user)->Level;
				loan=USER(user)->Gold/afford;
				sprintf(text,"You can afford %s hit points.",(loan<(USER(user)->HP-Thp)) ? Fnum(loan) : "all your");
				outs(text);
				newline();
				newline();
				loan=atof(Fnum(loan));
				afford=(loan>(USER(user)->HP-Thp)) ? USER(user)->HP-Thp : loan;
				if(afford<1)
					afford=0;
				sprintf(text,"%sHow many do you want%s [MAX=%u]? ",Fcol(CYN),Fcol(GRY),afford);
				outs(text);
				if(input())
					break;
				newline();
				i=atoi(ins);
				if(stricmp(ins,"MAX")==0)
					i=afford;
				if(i<1 || i>afford)
					break;
				Thp+=i;
				USER(user)->Gold-=(USER(user)->Level*i);
				PutUser(user);
				newline();
				sprintf(text,"Hit points = %d",Thp);
				outs(text);
				newline();
				break;
			case 'M':
				outs("The old mage ");
				switch(CLASS(USER(user)->Class)->Magic) {
					case 0:
						outs("says, \"Get outta here!\"");
						break;
					case 1:
						outs("offers to sell you a magic wand.");
						break;
					case 2:
						outs("offers to make you a scroll, for a price.");
						break;
					case 3:
						outs("offers to teach you a spell, for a price.");
						break;
					case 4:
						outs("wants to teach you a spell, for a price.");
						break;
				}
				newline();
				if(CLASS(USER(user)->Class)->Magic==0)
					break;
				newline();
				mu=(CLASS(USER(user)->Class)->Magic==1) ? 1 : 0;
				for(i=0; i<MAXMAGIC; i++) {
					if((USER(user)->Spell & (UWORD)pow2((double)i))==0) {
						sprintf(text,"%s %s%-24s %s",bracket(i+1),Fcol(GRY),MAGIC(i)->Name,money(MAGIC(i)->Cost[mu],user));
						outs(text);
						newline();
						if(USER(user)->Gold<MAGIC(i)->Cost[mu])
							break;
					}
				}
				newline();
				outs(Fcol(CYN));
				outs("Buy which? ");
				if(input())
					break;
				newline();
				i=atoi(ins);
				if(i<1 || i>MAXMAGIC)
					break;
				newline();
				i--;
				if(USER(user)->Gold<MAGIC(i)->Cost[mu]) {
					outs("You don't have enough money!");
					newline();
					break;
				}
				if((USER(user)->Spell & (UWORD)pow2((double)i))==0) {
					USER(user)->Spell|=(UWORD)pow2((double)i);
					USER(user)->Gold-=MAGIC(i)->Cost[mu];
					outs("The deed is done.");
					newline();
					PutUser(user);
				}
				break;
			case 'P':
				outs("You attempt to pick a passerby's pocket...");
				Delay(100);
				newline();
				newline();
				JOUST(USER(user)->Level);
				modf(5.*d/dice(10),&d);
				nme=dice(MAXUSER);
				if(nme!=user && USER(nme)->Access && USER(nme)->Gold>0.) {
					d=USER(nme)->Gold;
					sprintf(text,"You pick %.26s's pocket and steal %s!",USER(nme)->Handle,money(USER(nme)->Gold,user));
				}
				else {
					nme=0;
					sprintf(text,"You pick somebody's pocket and steal %s!",money(d,user));
				}
				outs(text);
				newline();
				Delay(100);
				if(dice(100)>15*CLASS(USER(user)->Class)->ToSteal+USER(user)->Level/10+Tagl/10) {
					beep();
					newline();
					outs("A guard catches you and throws you into jail!");
					newline();
					outs("You might be released by your next call.");
					newline();
					Delay(50);
					if(nme) {
						numline=0;
						sprintf(line[numline++],"%.26s was caught picking your pocket!",USER(user)->Handle);
						UserNote(nme);
					}
					strcpy(reason,"caught picking a pocket");
					logoff=TRUE;
					break;
				}
				USER(user)->Gold+=d;
				PutUser(user);
				if(nme) {
					numline=0;
					sprintf(line[numline++],"Somebody picked your pocket and stole %s!",money(USER(i)->Gold,i));
					UserNote(nme);
					USER(nme)->Gold=0.;
					PutUser(nme);
				}
				break;
			case 'R':
				BuySell(RealEstate,MAXREALESTATE-1,&USER(user)->RealEstate,0);
				break;
			case 'S':
				BuySell(Security,MAXSECURITY-1,&USER(user)->Security,0);
				break;
			case 'V':
				if(CLASS(USER(user)->Class)->Poison==0) {
					outs("The Apothecary says, \"Get outta here!\"");
					newline();
					break;
				}
				outs("You enter the back door of the shop.");
				newline();
				newline();
				mu=(CLASS(USER(user)->Class)->Poison<0) ? 1 : 0;
				for(i=0; i<MAXPOISON; i++) {
					if((USER(user)->Poison & (UWORD)pow2((double)i))==0) {
						sprintf(text,"%s %s%-24s %s",bracket(i+1),Fcol(GRY),POISON(i)->Name,money(POISON(i)->Cost[mu],user));
						outs(text);
						newline();
						if(USER(user)->Gold<POISON(i)->Cost[mu])
							break;
					}
				}
				newline();
				outs(Fcol(CYN));
				outs("Buy which? ");
				if(input())
					break;
				newline();
				i=atoi(ins);
				if(i<1 || i>MAXPOISON)
					break;
				newline();
				i--;
				if(USER(user)->Gold<POISON(i)->Cost[mu]) {
					outs("You don't have enough money!");
					newline();
					break;
				}
				if((USER(user)->Poison & (UWORD)pow2((double)i))==0) {
					USER(user)->Poison|=(UWORD)pow2((double)i);
					USER(user)->Gold-=POISON(i)->Cost[mu];
					outs("The Apothecary slips you the vial.");
					newline();
					PutUser(user);
				}
				break;
			case 'W':
				item=USER(user)->Weapon;
				BuySell(Weapon,CLASS(USER(user)->Class)->WC,&USER(user)->Weapon,Thit);
				if(USER(user)->Weapon!=item)
					Thit=(USER(user)->Blessed) ? USER(user)->Weapon/10 : (USER(user)->Cursed) ? -USER(user)->Weapon/10 : 0;
				break;
		}
	}
	from='M';
}
