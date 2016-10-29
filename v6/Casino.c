/*******************\
 *  RPGBBS Casino  *
\*******************/
#include "RPGBBS.h"

double Bet(void)
{
	char c;

	sprintf(text,"Bet [MAX=%s]? ",money(USER(user)->Gold,user));
	outs(text);
	if(input())
		return(0.);
	newline();
	newline();
	modf(atof(ins),&d);
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
	if(d<1. || d>USER(user)->Gold)
		d=0.;
	USER(user)->Gold-=d;
	return(d);
}

int ShowHand(struct card *hand[7],int cards)
{
	int ace,i,value;

	outs(" hand: ");
	ace=0;
	value=0;
	for(i=0; i<cards; i++) {
		sprintf(text,"%s[%s%s%s] ",Fcol(RED),Fcol(WHITE),hand[i]->Name,Fcol(RED));
		outs(text);
		value+=(hand[i]->Value<11) ? hand[i]->Value : (hand[i]->Value!=14) ? 10 : 1;
		if(hand[i]->Value==14)
			ace++;
	}
	for(i=0; i<ace && value+10<22; i++)
		value+=10;
	sprintf(text,"%s= %d",Fcol(GRY),value);
	outs(text);
	newline();
	Delay(50);
	return(value);
}

void Casino(void)
{
	static struct RPGmenu __far RPGmenu = { "Casino",BLU,8,
							{	{ 'B',"Blackjack" },
								{ 'C',"Craps" },
								{ 'G',"Greyhound race" },
								{ 'H',"High card" },
								{ 'I',"Instant Cash machine" },
								{ 'K',"Keno" },
								{ 'R',"Roulette" },
								{ 'S',"One-armed Bandit" }
							}
	};
	static struct RPGmenu __far ATMmenu = { "ATM",GRN,2,
							{	{ 'D',"Deposit" },
								{ 'W',"Withdrawal" }
							}
	};
	static char *dog[9] = {
		"Armored Tank","Knight Rider","Wham's Wings","Go For Grunt",
		"Let It Ride","Stinky","Lucky Lois","Rhody Runner","Beetle Bomb"
	};
	struct card *Dealer[7],*Player[7];
	double bet,loan;
	int balls[81],post[10],d1,d2,i,pile,point,value;
	char c;

	sprintf(OUTbuf,"    %sWelcome to the Gambling Casino!",Fcol(GREEN));
	outs(OUTbuf);
	newline();
	newline();
	while(!cd()) {
		if((c=Option(&RPGmenu))=='Q')
			break;

		switch(c) {
			case 'B':
				Shuffle();
				if((bet=Bet())==0.)
					break;
				pile=0;
				d1=0;
				d2=0;
				Player[d1++]=deck[pile++];
				Dealer[d2++]=deck[pile++];
				Player[d1++]=deck[pile++];
				Dealer[d2++]=deck[pile++];
				sprintf(text,"%sDealer's hand: %s[%sDOWN%s] [%s%s%s]",Fcol(GRN),Fcol(RED),Fcol(WHITE),Fcol(RED),Fcol(WHITE),Dealer[1]->Name,Fcol(RED));
				outs(text);
				newline();
				outs(Fcol(GRN));
				outs("Player's");
				point=ShowHand(Player,d1);
				if(point==21) {
					sprintf(text,"Blackjack!  You win %s!",money(2.*bet,user));
					outs(text);
					newline();
					Delay(50);
					USER(user)->Gold+=3.*bet;
					newline();
					outs(Fcol(GRN));
					outs("Dealer's");
					value=ShowHand(Dealer,d2);
					if(value==21) {
						outs("BlackJack!");
						newline();
						outs("Your bet was a loser!");
						newline();
						Delay(50);
						USER(user)->Gold-=3.*bet;
						break;
					}
					break;
				}
				loan=bet;
				FOREVER {
					newline();
					if(d1==2 && USER(user)->Gold>=bet)
						outs("<D>ouble down or ");
					outs("<H>it or <S>tay? ");
					if(!(c=inkey()))
						break;
					newline();
					if(d1==2 && USER(user)->Gold>=bet && c=='D') {
						USER(user)->Gold-=bet;
						bet*=2.;
						c='H';
					}
					if(c=='H') {
						Player[d1++]=deck[pile++];
						newline();
						outs(Fcol(GRN));
						outs("Player's");
						point=ShowHand(Player,d1);
						if(point==21)
							break;
						if(point>21) {
							outs("You bust!");
							newline();
							break;
						}
						if(loan<bet)
							break;
						if(d1==5) {
							sprintf(text,"Five card charley!  You win %s!",money(2.*bet,user));
							outs(text);
							newline();
							USER(user)->Gold+=3.*bet;
							break;
						}
					}
					if(c=='S')
						break;
				}
				newline();
				outs(Fcol(GRN));
				outs("Dealer's");
				value=ShowHand(Dealer,d2);
				if(d1<5 && value==21) {
					outs("BlackJack!");
					newline();
					outs("Your bet was a loser!");
					newline();
					Delay(50);
					break;
				}
				do {
					if(value<17) {
						Dealer[d2++]=deck[pile++];
						outs(Fcol(GRN));
						outs("Dealer's");
						value=ShowHand(Dealer,d2);
					}
					Delay(30);
				} while(value<17 && value<22);
				newline();
				if(point>21 || d1==5)
					break;
				if(value>21) {
					sprintf(text,"Dealer bust!  You win %s!",money(bet,user));
					outs(text);
					newline();
					USER(user)->Gold+=2.*bet;
					break;
				}
				if(value<point) {
					sprintf(text,"You win %s!",money(bet,user));
					outs(text);
					newline();
					USER(user)->Gold+=2.*bet;
					break;
				}
				if(value==point) {
					outs("It's a push.");
					newline();
					USER(user)->Gold+=bet;
					break;
				}
				if(value>point) {
					sprintf(text,"Dealer wins.");
					outs(text);
					newline();
					break;
				}
				break;
			case 'C':
				if((bet=Bet())==0.)
					break;
				outs("Rolling dice for your point");
				d1=dice(6);
				d2=dice(6);
				point=d1+d2;
				for(i=0; i<5; i++) {
					Delay(5);
					outs(".");
				}
				Delay(25);
				sprintf(text,"%s[%s%d%s] [%s%d%s] %s= %d",Fcol(BLU),Fcol(CYAN),d1,Fcol(BLU),Fcol(CYAN),d2,Fcol(BLU),Fcol(WHITE),d1+d2);
				outs(text);
				newline();
				Delay(50);
				newline();
				if(point==7 || point==11) {
					sprintf(text,"A natural!  You win %s!",money(2.*bet,user));
					outs(text);
					newline();
					Delay(50);
					USER(user)->Gold+=3.*bet;
					break;
				}
				if(point==2 || point==3 || point==12) {
					outs("Crapped out!  You lose.");
					newline();
					Delay(50);
					break;
				}
				cls();
				sprintf(text,"Your point to make is: %d",point);
				outs(text);
				newline();
				newline();
				outs("Press RETURN to roll dice and try to make your point");
				newline();
				outs("or bet on another number for additional payoffs:");
				newline();
				newline();
				outs("  [2] or [12] pays 35:1");
				newline();
				outs("  [3] or [11] pays 17:1");
				newline();
				outs("  [4] or [10] pays 11:1");
				newline();
				outs("  [5] or  [9] pays  8:1");
				newline();
				outs("  [6] or  [8] pays  6:1");
				newline();
				outs("  [7] to break pays 5:1");
				newline();
				do {
					plot(13,1);
					outs(Fcol(CYN));
					outs("Roll dice: ");
					cll();
					if(input())
						break;
					newline();
					cll();
					i=atoi(ins);
					if(i>1 && i<13)
						loan=Bet();
					else
						i=0;
					cll();
					d1=dice(6);
					d2=dice(6);
					sprintf(text,"%s[%s%d%s] [%s%d%s] %s= %d   ",Fcol(BLU),Fcol(CYAN),d1,Fcol(BLU),Fcol(CYAN),d2,Fcol(BLU),Fcol(WHITE),d1+d2);
					outs(text);
					newline();
					cll();
					if(d1+d2==7)
						outs("Crapped out!  You lose.");
					if(d1+d2==point) {
						sprintf(text,"You've made your point!  You win %s!",money(bet,user));
						outs(text);
						USER(user)->Gold+=2.*bet;
					}
					newline();
					cll();
					if(i!=0 && d1+d2!=i)
						outs("You lose on the side bet.");
					if(d1+d2==i) {
						i=(i==2 || i==12) ? 35 : (i==3 || i==11) ? 17 : (i==4 || i==10) ? 11 : (i==5 || i==9) ? 8 : (i==6 || i==8) ? 6 : 5;
						sprintf(text,"You've made your side bet!  You win %s!",money(loan*i,user));
						outs(text);
						USER(user)->Gold+=loan*(i+1);
					}
					newline();
					cll();
				} while(d1+d2!=point && d1+d2!=7);
				break;
			case 'G':
				if((bet=Bet())==0.)
					break;
				cls();
				outs(Fcol(BLU));
				outs("[==============*=========+=========+=========+=========+=========F");
				newline();
				for(i=0; i<9; i++) {
					post[i]=0;
					sprintf(text,"%s[%s%s %-12.12s :%s%d%49.49s",Fcol(BLU),Bcol(BLU),Fcol(WHITE),dog[i],Bcol(BLK),i+1,"|");
					outs(text);
					newline();
				}
				outs(Fcol(BLU));
				outs("[==============*=========+=========+=========+=========+=========F");
				newline();
				outs(Fcol(CYN));
				outs("Which dog (1-9)? ");
				if(input())
					break;
				point=atoi(ins)-1;
				if(point<0 || point>8) {
					USER(user)->Gold+=bet;
					break;
				}
				outs(Fcol(CYN));
				outs("   <W>in, <P>lace, or <S>how? ");
				if(!(c=inkey()))
					break;
				if(c!='W' && c!='P' && c!='S') {
					USER(user)->Gold+=bet;
					break;
				}
				outs(Fcol(YELLOW));
				outs("   They're off!");
				outs(Fcol(GRY));
				loan=0.;
				i=0;
				do {
					d1=dice(9)-1;
					if(post[d1]<50) {
						if(local)
							Delay(1);
						plot(2+d1,17+post[d1]);
						sprintf(text,"%c%d",post[d1]%2 ? '`' : ',',d1+1);
						outs(text);
						post[d1]++;
						if(post[d1]==50) {
							i++;
							outs(Fcol(i==1 ? CYAN : i==2 ? YELLOW : MAGENTA));
							outs(i==1 ? " #1st#" : i==2 ? " =2nd=" : " -3rd-");
							outs(Fcol(GRY));
							if(d1==point)
								if(i==1 || (i==2 && c!='W') || (i==3 && c=='S'))
									loan=bet*((c=='W') ? 8. : (c=='P') ? 4. : 2.);
							Delay(30);
						}
					}
				} while(i<3);
				plot(14,1);
				if(loan) {
					sprintf(text,"You win %s!",money(loan,user));
					outs(text);
					USER(user)->Gold+=loan+bet;
				}
				else
					outs("You lose.");
				cll();
				newline();
				break;
			case 'H':
				Shuffle();
				if((bet=Bet())==0.)
					break;
				outs(Fcol(CYN));
				outs("Pick any card (1-52)? ");
				if(input())
					break;
				d1=atoi(ins);
				if(d1<1 || d1>52) {
					USER(user)->Gold+=bet;
					newline();
					break;
				}
				d1--;
				sprintf(text," - %s[%s%s%s]",Fcol(RED),Fcol(WHITE),deck[d1]->Name,Fcol(RED));
				outs(text);
				newline();
				normal();
				Delay(50);
				do {
					d2=dice(52)-1;
				} while(d2==d1);
				sprintf(text,"Dealer picks card #%d",d2+1);
				outs(text);
				Delay(50);
				sprintf(text," - %s[%s%s%s]",Fcol(RED),Fcol(WHITE),deck[d2]->Name,Fcol(RED));
				outs(text);
				newline();
				normal();
				Delay(50);
				newline();
				if(deck[d1]->Value>deck[d2]->Value) {
					sprintf(text,"You win %s!",money(bet,user));
					outs(text);
					USER(user)->Gold+=2.*bet;
				}
				if(deck[d1]->Value==deck[d2]->Value) {
					outs("We tie.");
					USER(user)->Gold+=bet;
				}
				if(deck[d1]->Value<deck[d2]->Value)
					outs("You lose.");
				newline();
				break;
			case 'I':
				sprintf(OUTbuf,"%sEnter PIN:%s",Fcol(CYN),Fcol(WHITE));
				outs(OUTbuf);
				for(i=0; i<6; i++) {
					Delay(dice(37)+3);
					outs("#");
				}
				newline();
				while(!cd()) {
					newline();
					sprintf(OUTbuf,"%sAutomatic Teller Machine",Fcol(CYAN));
					outs(OUTbuf);
					newline();
					newline();
					sprintf(text,"%sMoney in hand: %s",Fcol(GRN),money(USER(user)->Gold,user));
					outs(text);
					newline();
					sprintf(text,"%sMoney in bank: %s",Fcol(GRN),money(USER(user)->Bank,user));
					outs(text);
					newline();
					sprintf(text,"%sMoney on loan: %s",Fcol(GRN),money(USER(user)->Loan,user));
					outs(text);
					newline();
					newline();
					if((c=Option(&ATMmenu))=='Q')
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
								outs("Cha-ching!");
								newline();
							}
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
								outs("Cha-ching!");
								newline();
							}
							break;
					}
				}
				break;
			case 'K':
				outs(Fcol(CYN));
				outs("How many numbers (1-10)? ");
				if(input())
					break;
				newline();
				newline();
				point=atoi(ins);
				if(point<1 || point>10)
					break;
				sprintf(OUTbuf,"%sKENO PAYOUTS for a %d spot game:",Fcol(GRN),point);
				outs(OUTbuf);
				newline();
				newline();
				sprintf(OUTbuf,"%sMATCH     PRIZE%s",Fcol(GREEN),Fcol(CYN));
				outs(OUTbuf);
				newline();
				switch(point) {
					case 1:
						outs("   1         $1");
						newline();
						strcpy(text,"4");
						break;
					case 2:
						outs("   2         $9");
						newline();
						strcpy(text,"16.6");
						break;
					case 3:
						outs("   3        $20");
						newline();
						outs("   2          2");
						newline();
						strcpy(text,"6.55");
						break;
					case 4:
						outs("   4        $50");
						newline();
						outs("   3          5");
						newline();
						outs("   2          1");
						newline();
						strcpy(text,"3.86");
						break;
					case 5:
						outs("   5       $400");
						newline();
						outs("   4         10");
						newline();
						outs("   3          2");
						newline();
						strcpy(text,"10.33");
						break;
					case 6:
						outs("   6      $1000");
						newline();
						outs("   5         50");
						newline();
						outs("   4          5");
						newline();
						outs("   3          1");
						newline();
						strcpy(text,"6.19");
						break;
					case 7:
						outs("   7      $4000");
						newline();
						outs("   6         75");
						newline();
						outs("   5         15");
						newline();
						outs("   4          2");
						newline();
						outs("   3          1");
						newline();
						strcpy(text,"4.22");
						break;
					case 8:
						outs("   8     $10000");
						newline();
						outs("   7        500");
						newline();
						outs("   6         40");
						newline();
						outs("   5         10");
						newline();
						outs("   4          2");
						newline();
						strcpy(text,"9.79");
						break;
					case 9:
						outs("   9     $25000");
						newline();
						outs("   8       2500");
						newline();
						outs("   7        100");
						newline();
						outs("   6         20");
						newline();
						outs("   5          5");
						newline();
						outs("   4          1");
						newline();
						strcpy(text,"6.55");
						break;
					case 10:
						outs("  10    $100000");
						newline();
						outs("   9       4000");
						newline();
						outs("   8        400");
						newline();
						outs("   7         25");
						newline();
						outs("   6         10");
						newline();
						outs("   5          2");
						newline();
						outs(" none         5");
						newline();
						strcpy(text,"9.04");
						break;
				}
				newline();
				sprintf(OUTbuf,"%sodds of winning a prize in this game are 1:%s.",Fcol(GRY),text);
				outs(OUTbuf);
				newline();
				newline();
				if((bet=Bet())==0.)
					break;
				i=0;
				do {
					FOREVER {
						d1=dice(80);
						for(d2=0; d2<i && post[d2]!=d1; d2++);
						if(d2==i)
							break;
					}
					sprintf(OUTbuf,"%sPick #%d [%02d]: ",Fcol(CYN),i+1,d1);
					outs(OUTbuf);
					if(input())
						break;
					d2=atoi(ins);
					if(!strlen(ins)) {
						d2=d1;
						sprintf(OUTbuf,"%s%02d",Fcol(WHITE),d2);
						outs(OUTbuf);
					}
					newline();
					if(d2>=1 && d2<=80) {
						for(d1=0; d1<i && post[d1]!=d2; d1++);
						if(d1==i)
							post[i++]=d2;
						else
							beep();
					}
					else
						beep();
				} while(i<point);
				newline();
				sprintf(OUTbuf,"%sHere comes those lucky numbers!",Fcol(YELLOW));
				outs(OUTbuf);
				newline();
				newline();
				Delay(50);
				for(i=1; i<=80; i++)
					balls[i]=i;
				value=0;
				for(i=0; i<20; i++) {
					while(!balls[(d1=dice(80))]);
					balls[d1]=0;
					for(pile=0; pile<point && post[pile]!=d1; pile++);
					if(pile==point)
						sprintf(OUTbuf," %s[%s %s%02d%s %s]  ",Fcol(BLU),Bcol(GRY),Fcol(RED),d1,Fcol(BLU),Bcol(BLK));
					else {
						sprintf(OUTbuf,"%s*%s[%s %s%02d%s %s]%s* ",Fcol(YELLOW),Fcol(BLU),Bcol(GRY),Fcol(RED),d1,Fcol(BLU),Bcol(BLK),Fcol(YELLOW));
						beep();
						value++;
					}
					outs(OUTbuf);
					Delay(25);
					if((i+1)%5==0) {
						newline();
						newline();
					}
				}
				loan=0.;
				switch(point) {
					case 1:
						if(value==1)
							loan=bet*2.;
						break;
					case 2:
						if(value==2)
							loan=bet*9.;
						break;
					case 3:
						if(value==3)
							loan=bet*20.;
						if(value==2)
							loan=bet*2.;
						break;
					case 4:
						if(value==4)
							loan=bet*50.;
						if(value==3)
							loan=bet*5.;
						if(value==2)
							loan=bet*1.;
						break;
					case 5:
						if(value==5)
							loan=bet*400.;
						if(value==4)
							loan=bet*10.;
						if(value==3)
							loan=bet*2.;
						break;
					case 6:
						if(value==6)
							loan=bet*1000.;
						if(value==5)
							loan=bet*50.;
						if(value==4)
							loan=bet*5.;
						if(value==3)
							loan=bet*1.;
						break;
					case 7:
						if(value==7)
							loan=bet*4000.;
						if(value==6)
							loan=bet*75.;
						if(value==5)
							loan=bet*15.;
						if(value==4)
							loan=bet*2.;
						if(value==3)
							loan=bet*1.;
						break;
					case 8:
						if(value==8)
							loan=bet*10000.;
						if(value==7)
							loan=bet*500.;
						if(value==6)
							loan=bet*40.;
						if(value==5)
							loan=bet*10.;
						if(value==4)
							loan=bet*2.;
						break;
					case 9:
						if(value==9)
							loan=bet*25000.;
						if(value==8)
							loan=bet*2500.;
						if(value==7)
							loan=bet*100.;
						if(value==6)
							loan=bet*20.;
						if(value==5)
							loan=bet*5.;
						if(value==4)
							loan=bet*1.;
						break;
					case 10:
						if(value==10)
							loan=bet*100000.;
						if(value==9)
							loan=bet*4000.;
						if(value==8)
							loan=bet*400.;
						if(value==7)
							loan=bet*25.;
						if(value==6)
							loan=bet*10.;
						if(value==5)
							loan=bet*2.;
						if(value==0)
							loan=bet*5.;
						break;
				}
				if(loan) {
					sprintf(OUTbuf,"%sYou win %s!",Fcol(WHITE),money(loan,user));
					outs(OUTbuf);
					USER(user)->Gold+=loan;
				}
				else
					outs("You lose.");
				newline();
				break;
			case 'R':
				if((bet=Bet())==0.)
					break;
				outs(Fcol(CYN));
				outs("Which number (1-36), <O>dd, or <E>ven? ");
				if(input())
					break;
				newline();
				newline();
				loan=0.;
				point=atoi(ins);
				c=toupper(ins[0]);
				if((point<1 || point>36) && c!='O' && c!='E') {
					USER(user)->Gold+=bet;
					break;
				}
				outs("The wheel spins");
				for(i=0; i<4; i++) {
					outs(Fcol(GRY));
					for(d1=0; d1<5; d1++) {
						value=dice(36);
						outs(".");
						Delay(5);
					}
					sprintf(text,"%s[%s%d%s]",Fcol(i<3 ? BLU : RED),Fcol(YELLOW),value,Fcol(i<3 ? BLU : RED));
					outs(text);
					Delay(5);
				}
				Delay(20);
				normal();
				newline();
				newline();
				if(value==point)
					loan=35.*bet;
				if(c=='O' && value%2==1)
					loan=bet;
				if(c=='E' && value%2==0)
					loan=bet;
				if(loan) {
					sprintf(text,"You win %s!",money(loan,user));
					outs(text);
					USER(user)->Gold+=loan+bet;
				}
				else
					outs("You lose.");
				newline();
				break;
			case 'S':
				outs("2 Cherry + any...$2        3 Lime..........$20");
				newline();
				outs("3 Cherry.........$5        3 Orange........$50");
				newline();
				outs("3 Plum..........$10        3 Bell.........$100");
				newline();
				sprintf(text,"3 =Gold= pays $1000 -plus- Monster Jackpot worth %s",money(USER(0)->Gold,user));
				outs(text);
				newline();
				newline();
				if((bet=Bet())==0.)
					break;
				outs("You insert the money, pull the arm, and the wheels spin...");
				Delay(50);
				newline();
				for(i=0; i<3; i++) {
					d2=14+dice(14);
					for(d1=0; d1<d2; d1++) {
						bandit[i]++;
						bandit[i]%=14;
						sprintf(text,"%c\10",spin[d1%4]);
						outs(text);
						Delay(2);
					}
					sprintf(text,"%s[%s%s%s] ",Fcol(BLU),Fcol(slotcol[wheel[i][bandit[i]]]),slotval[wheel[i][bandit[i]]],Fcol(BLU));
					outs(text);
					normal();
				}
				newline();
				if(wheel[0][bandit[0]]==GOLD && wheel[1][bandit[1]]==GOLD && wheel[2][bandit[2]]==GOLD) {
					for(i=0; i<50; i++) {
						beep();
						outs(Fcol(i%8));
						outs("YOU WIN! ");
					}
					normal();
					newline();
					bet*=1000.;
					bet+=USER(0)->Gold;
				}
				else
				if(wheel[0][bandit[0]]==BELL && wheel[1][bandit[1]]==BELL && wheel[2][bandit[2]]==BELL) {
					for(i=0; i<8; i++) {
						beep();
						outs(Fcol(i%8));
						outs("YOU WIN! ");
					}
					normal();
					newline();
					bet*=100.;
				}
				else
				if(wheel[0][bandit[0]]==ORANGE && wheel[1][bandit[1]]==ORANGE && wheel[2][bandit[2]]==ORANGE) {
					beep();
					bet*=50.;
				}
				else
				if(wheel[0][bandit[0]]==LIME && wheel[1][bandit[1]]==LIME && wheel[2][bandit[2]]==LIME) {
					beep();
					bet*=20.;
				}
				else
				if(wheel[0][bandit[0]]==PLUM && wheel[1][bandit[1]]==PLUM && wheel[2][bandit[2]]==PLUM)
					bet*=10.;
				else
				if(wheel[0][bandit[0]]==CHERRY && wheel[1][bandit[1]]==CHERRY && wheel[2][bandit[2]]==CHERRY)
					bet*=5.;
				else
				if(wheel[0][bandit[0]]==CHERRY && wheel[1][bandit[1]]==CHERRY)
					bet*=2.;
				else {
					outs("You lose.");
					newline();
					USER(0)->Gold+=bet;
					bet=0.;
				}
				PutUser(0);
				if(bet>0.) {
					sprintf(text,"You win %s!",money(bet,user));
					outs(text);
					newline();
					USER(user)->Gold+=bet;
					USER(0)->Gold-=bet;
					if(USER(0)->Gold<1.)
						USER(0)->Gold=0.;
					PutUser(0);
				}
				break;
		}
		PutUser(user);
	}
}
