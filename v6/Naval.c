/******************\
 *  RPGBBS Naval  *
\******************/
#include "RPGBBS.h"

int Fire(int p1,int p2)
{
	int i,n,hit,damage,cannon,ram;

	hit=0;
	damage=0;
	cannon=0;
	ram=FALSE;
	outs(Fcol(CYN));
	outs((p1==user) ? "Attacker: " : "Defender: ");
	for(i=0; i<USER(p1)->Cannon; i++) {
		n=dice(100);
		n=(n<66) ? 0 : (n<95) ? 1 : (n<100 || USER(p2)->Ram!='Y' || nme==0) ? 2 : 3;
		if(n==3) {
			ram=TRUE;
			USER(p2)->Ram='N';
			outs(Fcol(MAGENTA));
			outs("^");
		}
		if(n==2) {
			if(USER(p2)->Cannon>0 && nme>0) {
				cannon++;
				USER(p2)->Cannon--;
				outs(Fcol(GREEN));
				outs("@");
			}
			else
				n=1;
		}
		if(n==1) {
			hit++;
			damage+=dice(50);
			outs(Fcol(RED));
			outs("*");
		}
		if(n==0) {
			outs(Fcol(GRY));
			outs("-");
		}
		Delay(1);
	}
	outs(Fcol(CYN));
	newline();
	newline();
	sprintf(text,"%s hit %s %d times for %d hull points of damage.",(p1==user) ? "You" : "They",(p1==user && nme>0) ? "them" : (p1==user) ? "it" : "you",hit,damage);
	outs(text);
	newline();
	if(cannon) {
		sprintf(text,"%s also hit %d of %s cannons.",(p1==user) ? "You" : "They",cannon,(p1==user) ? "their" : "your");
		outs(text);
		newline();
	}
	if(ram) {
		sprintf(text,"%s also hit %s ram.",(p1==user) ? "You" : "They",(p1==user) ? "their" : "your");
		outs(text);
		newline();
	}
	PutUser(p2);
	return(damage);
}

void Naval(void)
{
	static struct RPGmenu RPGmenu = { "Naval",BLU,6,
							{	{ 'S',"Shipyard" },
								{ 'B',"Battle other users" },
								{ 'H',"Hunt sea monsters" },
								{ 'G',"Go fishing" },
								{ 'Y',"Your ship's status" },
								{ 'L',"List user ships" }
							}
	};
	static struct RPGmenu SHIPmenu = { "Shipyard",BRN,4,
							{	{ 'B',"Buy a new ship" },
								{ 'F',"Fix battle damage" },
								{ 'C',"Mount cannons" },
								{ 'R',"Mount a ram" }
							}
	};
	double cost;
	int average,i,lp,mh;
	char c,more;

	while(!cd()) {
		if((c=Option(&RPGmenu))=='Q')
			break;
		switch(c) {
			case 'S':
				while(!cd()) {
					if((c=Option(&SHIPmenu))=='Q')
						break;
					switch(c) {
						case 'B':
							if(Thull+50>9999) {
								outs("They don't make ships any bigger than the one you have now.");
								newline();
								break;
							}
							if(Thull==0) {
								modf(pow2(50./150.)*7937.+.5,&d);
								if(USER(user)->Gold<d) {
									sprintf(text,"You need at least %s to buy a ship.",money(d,user));
									outs(text);
									newline();
									break;
								}
							}
							outs("List of affordable ships:");
							newline();
							mh=USER(user)->Hull+50;
							modf(pow2(mh/150.)*7937.+.5,&d);
							while(mh<10000 && d<=USER(user)->Gold) {
								sprintf(text,"Hull size: %-4d     Cost: %s",mh,money(d,user));
								outs(text);
								newline();
								mh+=50;
								modf(pow2(mh/150.)*7937.+.5,&d);
							}
							newline();
							outs(Fcol(CYN));
							outs("Enter hull size to buy: ");
							if(input())
								break;
							newline();
							newline();
							i=atoi(ins);
							if(i==0)
								break;
							if(i%50) {
								outs("We don't make ships with that hull size, but only in multiples of 50.");
								newline();
								break;
							}
							if(i<=USER(user)->Hull) {
								sprintf(text,"You already have a %d hull size ship!",USER(user)->Hull);
								outs(text);
								newline();
								break;
							}
							if(i>9950) {
								outs("We don't make ships that big!");
								newline();
								break;
							}
							if(i>=mh) {
								outs("You don't have enough money!");
								newline();
								break;
							}
							modf(pow2(i/150.)*7937.+.5,&d);
							USER(user)->Gold-=d;
							USER(user)->Hull=i;
							USER(user)->Ram='N';
							PutUser(user);
							Thull=USER(user)->Hull;
							sprintf(text,"You now have a brand new %d hull point ship, with no ram.",Thull);
							outs(text);
							newline();
							break;
						case 'F':
							if(!Thull) {
								outs("You don't have a ship!");
								newline();
								break;
							}
							mh=USER(user)->Hull-Thull;
							sprintf(text,"You need %d hull points of repair.",mh);
							outs(text);
							newline();
							modf(pow2(USER(user)->Hull/150.)*7937.+.5,&cost);
							modf(cost/USER(user)->Hull/10.,&cost);
							sprintf(text,"Each hull point costs %s.",money(cost,user));
							outs(text);
							newline();
							newline();
							modf(USER(user)->Gold/cost,&d);
							if(d<mh)
								mh=d;
							sprintf(text,"%sHow many points [MAX=%d]? ",Fcol(CYN),mh);
							outs(text);
							if(input())
								break;
							newline();
							i=atoi(ins);
							if(stricmp(ins,"MAX")==0)
								i=mh;
							if(i<1 || i>mh)
								break;
							Thull+=i;
							USER(user)->Gold-=cost*i;
							PutUser(user);
							newline();
							sprintf(text,"Hull points = %d",Thull);
							outs(text);
							newline();
							break;
						case 'C':
							if(!Thull) {
								outs("You don't have a ship!");
								newline();
								break;
							}
							mh=USER(user)->Hull/50-USER(user)->Cannon;
							sprintf(text,"You can mount up to %d more cannons.",mh);
							outs(text);
							newline();
							modf(pow2(USER(user)->Hull/150.)*7937.+.5,&cost);
							modf(cost/250.,&cost);
							sprintf(text,"Each cannon costs %s.",money(cost,user));
							outs(text);
							newline();
							newline();
							modf(USER(user)->Gold/cost,&d);
							if(d<mh)
								mh=d;
							sprintf(text,"%sHow many cannons [MAX=%d]? ",Fcol(CYN),mh);
							outs(text);
							if(input())
								break;
							newline();
							i=atoi(ins);
							if(stricmp(ins,"MAX")==0)
								i=mh;
							if(i<1 || i>mh)
								break;
							USER(user)->Cannon+=i;
							USER(user)->Gold-=cost*i;
							PutUser(user);
							newline();
							sprintf(text,"Cannons = %d",USER(user)->Cannon);
							outs(text);
							newline();
							break;
						case 'R':
							if(!Thull) {
								outs("You don't have a ship!");
								newline();
								break;
							}
							if(USER(user)->Ram=='Y') {
								outs("But your ship already has a ram!");
								newline();
								break;
							}
							modf(pow2(USER(user)->Hull/150.)*7937.+.5,&cost);
							modf(cost/10.,&cost);
							sprintf(text,"We can equip your ship with a ram for %s.",money(cost,user));
							outs(text);
							newline();
							newline();
							modf(USER(user)->Gold/cost,&d);
							if(d<1.) {
								outs("You don't have enough money!");
								newline();
								break;
							}
							outs(Fcol(CYN));
							outs("Ok (Y/N)? ");
							if(!(c=inkey()))
								break;
							newline();
							if(c!='Y')
								break;
							USER(user)->Ram='Y';
							USER(user)->Gold-=cost;
							PutUser(user);
							newline();
							outs("You now have a ram.");
							newline();
							break;
					}
				}
				break;
			case 'B':
				if(!Thull) {
					outs("You don't have a ship!");
					newline();
					break;
				}
				if(naval>2) {
					outs("You have run out of battles.");
					newline();
					break;
				}
				outs(Fcol(CYN));
				outs("Battle which user? ");
				i=GetUser();
				if(i<1)
					i=0;
				nme=i;
				if(!nme)
					break;
				NMEstat();
				newline();
				if(!NMEhull) {
					sprintf(text,"%s does not have a ship.",USER(nme)->Sex=='M' ? "He" : "She");
					outs(text);
					newline();
					break;
				}
				if(nme==user) {
					sprintf(text,"What's the matter with you, %s!",(USER(user)->Sex=='M') ? "boy" : "girl");
					outs(text);
					newline();
					break;
				}
				sprintf(text,"You sail out until you spot %.26s's ship on the horizon.",USER(nme)->Handle);
				outs(text);
				newline();
				newline();
				sprintf(text,"It has %d hull points and is armed with %d cannons.",NMEhull,USER(nme)->Cannon);
				outs(text);
				newline();
				newline();
				if(USER(nme)->Ram=='Y') {
					outs("It also has a ram.");
					newline();
					newline();
				}
				outs(Fcol(CYN));
				outs("Continue (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				newline();
				if(c!='Y')
					break;
				naval++;
				more='Y';
				if((Tint+dice(100))>=(NMEint+dice(100))) {
					outs("You approach them and quickly open fire.");
					newline();
					newline();
					NMEhull-=Fire(user,nme);
					newline();
				}
				else {
					outs("They spot you coming and open fire.");
					newline();
					newline();
				}
				if(NMEhull>0 && USER(nme)->Cannon>0)
					Thull-=Fire(nme,user);
				if(NMEhull>0 && USER(nme)->Cannon==0) {
					outs("They are defenseless and attempt to flee...");
					newline();
					Delay(100);
					if(dice(50+Tint/2)>(50L*Thull/(Thull+NMEhull)+50L)) {
						outs("You outmaneuver them and stop their retreat!");
						newline();
						break;
					}
					outs("They sail away over the horizon.");
					more='N';
				}
				while(Thull>0 && NMEhull>0 && more=='Y') {
					DisplayUser();
					DisplayNME();
					newline();
					outs(Fcol(CYN));
					outs("<F> Fire cannons, <R> Ram their ship, <S> Sail off, <Y> Your status: ");
					if(!(c=inkey()))
						c='F';
					newline();
					newline();
					switch(c) {
						case 'F':
							average=USER(user)->Cannon/3*25;
							i=Fire(user,nme);
							if(i<(average-average/3)) {
								newline();
								outs(Fcol(CYN));
								outs("You scream at the gunner's mate, \"");
								normal();
								switch(dice(3)) {
									case 1:
										outs("Aim higher, you uneducated bloke!");
										break;
									case 2:
										outs("Fire on crest to hit the best!");
										break;
									case 3:
										outs("We're all gonna bloody die with your aim!");
										break;
								}
								outs(Fcol(CYN));
								outs("\"");
								newline();
								Delay(100);
							}
							if(i>(average+average/3)) {
								newline();
								outs(Fcol(CYN));
								outs("You exclaim to the gunner's mate, \"");
								normal();
								switch(dice(3)) {
									case 1:
										outs("YES!  She's all ours now!");
										break;
									case 2:
										outs("That's the way to shoot them dogs!");
										break;
									case 3:
										outs("She's listing badly now!");
										break;
								}
								outs(Fcol(CYN));
								outs("\"");
								newline();
								Delay(100);
							}
							NMEhull-=i;
							break;
						case 'R':
							if(USER(user)->Ram!='Y') {
								outs("You don't have a ram!");
								newline();
								newline();
								continue;
							}
							if(dice(50+NMEint/2)>(100L*NMEhull/(NMEhull+Thull))) {
								outs("They quickly outmaneuver your ship.");
								newline();
								newline();
								outs(Fcol(CYN));
								outs("You yell at your helmsman, \"");
								normal();
								switch(dice(3)) {
									case 1:
										outs("@#$%!");
										break;
									case 2:
										outs("I said port, dolt, not starboard!");
										break;
									case 3:
										outs("No more mead before duty for you!");
										break;
								}
								outs(Fcol(CYN));
								outs("\"");
								newline();
								Delay(100);
								break;
							}
							i=dice(USER(user)->Hull/2)+dice(Thull/2);
							NMEhull-=i;
							sprintf(text,"You ram them for %d hull points of damage!",i);
							outs(text);
							newline();
							break;
						case 'S':
							if(dice(50+NMEint/2)>(50L*NMEhull/(NMEhull+Thull)+50L)) {
								outs("They outmaneuver you and stop your retreat!");
								newline();
								break;
							}
							outs("You sail away safely out of range.");
							more='N';
							numline=0;
							sprintf(line[numline++],"%.26s, the coward, sailed away from you.",USER(user)->Handle);
							UserNote(nme);
							continue;
							break;
						case 'Y':
							sprintf(text,"%sHull points:%s %d",Fcol(GRN),Fcol(WHITE),Thull);
							outs(text);
							newline();
							sprintf(text,"%sCannons:%s %d",Fcol(GRN),Fcol(WHITE),USER(user)->Cannon);
							outs(text);
							newline();
							sprintf(text,"%sRam:%s %s",Fcol(GRN),Fcol(WHITE),(USER(user)->Ram=='Y') ? "Yes" : "No");
							outs(text);
							newline();
							newline();
							continue;
							break;
						default:
							continue;
							break;
					}
					newline();
					if(NMEhull<1)
						continue;
					c='F';
					if(!USER(nme)->Cannon && USER(nme)->Ram!='Y') {
						outs("They are defenseless and attempt to flee...");
						newline();
						Delay(100);
						c='S';
					}
					if(USER(nme)->Ram=='Y' && (dice(5)==1 || USER(nme)->Cannon==0))
						c='R';
					if(c=='F' && NMEhull<Thull && USER(nme)->Cannon<USER(user)->Cannon && dice(29-NMEint/10)==1) {
						sprintf(text,"%.26s salutes your superior naval tactics and tries to retreat...",USER(nme)->Handle);
						outs(text);
						newline();
						Delay(100);
						c='S';
					}
					switch(c) {
						case 'F':
							Thull-=Fire(nme,user);
							break;
						case 'R':
							outs("They attempt to ram you!");
							newline();
							newline();
							Delay(100);
							if(dice(50+Tint/2)>(100L*NMEhull/(NMEhull+Thull))) {
								outs("You quickly outmaneuver their ship.");
								newline();
								break;
							}
							i=dice(USER(nme)->Hull/2)+dice(NMEhull/2);
							Thull-=i;
							sprintf(text,"They ram you for %d hull points of damage!",i);
							outs(text);
							newline();
							break;
						case 'S':
							if(dice(50+Tint/2)>(50L*Thull/(Thull+NMEhull)+50L)) {
								outs("You outmaneuver them and stop their retreat!");
								newline();
								break;
							}
							outs("They sail away over the horizon.");
							more='N';
							break;
					}
					newline();
				}
				if(Thull<1) {
					Thull=0;
					sprintf(text,"%.26s smiles as a shark approaches you...",USER(nme)->Handle);
					outs(text);
					newline();
					modf(pow2(USER(nme)->Hull/150.)*7937.+.5,&cost);
					modf(cost/250.,&cost);
					cost*=USER(user)->Cannon;
					cost=value(cost,NMEchr)+USER(user)->Gold;
					USER(nme)->Gold+=cost;
					PutUser(nme);
					numline=0;
					sprintf(line[numline++],"You sank %.26s's ship!",USER(user)->Handle);
					if(USER(user)->Gold)
						sprintf(line[numline++],"You got %s.",money(cost,nme));
					UserNote(nme);
					USER(user)->Gold=0.;
					USER(user)->WasKill++;
					PutUser(user);
					Delay(100);
					sprintf(reason,"sunk by %.26s",USER(nme)->Handle);
					logoff=TRUE;
					sprintf(text,"[#%u] %.26s was %s.",user,USER(user)->Handle,reason);
					RPGmessage(text);
				}
				if(NMEhull<1) {
					NMEhull=0;
					switch(dice(5)) {
						case 1:
							sprintf(text,"You've sunk %.26s's ship!",USER(nme)->Handle);
							break;
						case 2:
							sprintf(text,"You've sunk %.26s's leaky, old tub!",USER(nme)->Handle);
							break;
						case 3:
							sprintf(text,"You've made splinters out of %.26s's ship!",USER(nme)->Handle);
							break;
						case 4:
							sprintf(text,"%.26s is now sleeping with the fishes!",USER(nme)->Handle);
							break;
						case 5:
							sprintf(text,"%.26s is now chum for the sharks!",USER(nme)->Handle);
							break;
					}
					outs(text);
					newline();
					newline();
					modf(pow2(USER(user)->Hull/150.)*7937.+.5,&cost);
					modf(cost/250.,&cost);
					cost*=USER(nme)->Cannon;
					cost=value(cost,Tchr)+USER(nme)->Gold;
					sprintf(text,"You get %s of booty!",money(cost,user));
					outs(text);
					newline();
					newline();
					USER(user)->Gold+=cost;
					PutUser(user);
					numline=0;
					sprintf(line[numline++],"%.26s sank your ship!",USER(user)->Handle);
					if(USER(nme)->Gold)
						sprintf(line[numline++],"They got %s.",money(cost,nme));
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
					USER(nme)->Gold=0.;
					PutUser(nme);
				}
				newline();
				break;
			case 'H':
				if(!Thull) {
					outs("You don't have a ship!");
					newline();
					break;
				}
				if(naval>2) {
					outs("You have run out of battles.");
					newline();
					break;
				}
				for(i=0; i<MAXSM; i++) {
					sprintf(text,"%s %s",bracket(i+1),SEA(i)->Name);
					outs(text);
					newline();
				}
				newline();
				outs(Fcol(CYN));
				outs("Hunt which monster? ");
				if(input())
					break;
				newline();
				mh=atoi(ins);
				if(mh<1 || mh>MAXSM)
					break;
				mh--;
				nme=0;
				memset(&NME,0,sizeof(struct User));
				strcpy(NME.Handle,SEA(mh)->Name);
				NME.Int=SEA(mh)->Int;
				NME.Hull=SEA(mh)->Hull;
				NME.HP=SEA(mh)->Power;
				NME.Ram=SEA(mh)->Ram;
				NME.Gold=SEA(mh)->Gold;
				NMEstat();
				NMEhit=SEA(mh)->Shot;
				newline();
				sprintf(text,"You sail out until you spot a %.26s on the horizon.",NME.Handle);
				outs(text);
				newline();
				newline();
				sprintf(text,"It has %d hull points.",NMEhull);
				outs(text);
				newline();
				newline();
				outs(Fcol(CYN));
				outs("Continue (Y/N)? ");
				if(!(c=inkey()))
					break;
				newline();
				newline();
				if(c!='Y')
					break;
				naval++;
				more='Y';
				if((Tint+dice(100))>=(NMEint+dice(100))) {
					outs("You approach it and quickly open fire.");
					newline();
					newline();
					NMEhull-=Fire(user,nme);
					newline();
					if(NMEhull>0) {
						mh=0;
						for(i=0; i<NMEhit; i++)
							mh+=dice(NMEhp)+dice(NMEhp);
						sprintf(text,"The %s attacks your ship, causing %d hull points of damage.",NME.Handle,mh);
						outs(text);
						newline();
						Thull-=mh;
					}
				}
				else {
					outs("It spots you coming and attacks.");
					newline();
					newline();
					mh=0;
					for(i=0; i<NMEhit; i++)
						mh+=dice(NMEhp)+dice(NMEhp);
					sprintf(text,"The %s attacks your ship, causing %d hull points of damage.",NME.Handle,mh);
					outs(text);
					newline();
					Thull-=mh;
				}
				while(Thull>0 && NMEhull>0 && more=='Y') {
					DisplayUser();
					DisplayNME();
					newline();
					outs(Fcol(CYN));
					outs("<F> Fire cannons, <R> Ram it, <S> Sail off, <Y> Your status: ");
					if(!(c=inkey()))
						c='F';
					newline();
					newline();
					switch(c) {
						case 'F':
							average=USER(user)->Cannon/3*25;
							i=Fire(user,nme);
							if(i<(average-average/3)) {
								newline();
								outs(Fcol(CYN));
								outs("You scream at the gunner's mate, \"");
								normal();
								switch(dice(3)) {
									case 1:
										outs("Cannonballs, not beer cans!");
										break;
									case 2:
										outs("Hit it, or I'll hit you!");
										break;
									case 3:
										outs("We're gonna be bloody supper to that beast!");
										break;
								}
								outs(Fcol(CYN));
								outs("\"");
								newline();
								Delay(100);
							}
							if(i>(average+average/3)) {
								newline();
								outs(Fcol(CYN));
								outs("You exclaim to the gunner's mate, \"");
								normal();
								switch(dice(3)) {
									case 1:
										outs("Good show, mate!");
										break;
									case 2:
										outs("It's bleeding badly!");
										break;
									case 3:
										outs("One more volley like that!");
										break;
								}
								outs(Fcol(CYN));
								outs("\"");
								newline();
								Delay(100);
							}
							NMEhull-=i;
							break;
						case 'R':
							if(USER(user)->Ram!='Y') {
								outs("You don't have a ram!");
								newline();
								newline();
								continue;
							}
							if(dice(50+NMEint/2)>(100L*NMEhull/(NMEhull+Thull))) {
								outs("It quickly outmaneuvers your ship.");
								newline();
								newline();
								outs(Fcol(CYN));
								outs("You yell at your helmsman, \"");
								normal();
								switch(dice(3)) {
									case 1:
										outs("Aim for the head, not the tail!");
										break;
									case 2:
										outs("I said starboard, asshole, not port!");
										break;
									case 3:
										outs("Whose side are you on anyways?!");
										break;
								}
								outs(Fcol(CYN));
								outs("\"");
								newline();
								Delay(100);
								break;
							}
							i=dice(USER(user)->Hull/2)+dice(Thull/2);
							NMEhull-=i;
							sprintf(text,"You ram it for %d hull points of damage!",i);
							outs(text);
							newline();
							break;
						case 'S':
							if(dice(50+NMEint/2)>(50L*NMEhull/(NMEhull+Thull)+50L)) {
								outs("It outmaneuvers you and stops your retreat!");
								newline();
								break;
							}
							outs("You sail away safely out of range.");
							more='N';
							continue;
							break;
						case 'Y':
							sprintf(text,"Hull points: %d",Thull);
							outs(text);
							newline();
							sprintf(text,"Cannons: %d",USER(user)->Cannon);
							outs(text);
							newline();
							sprintf(text,"Ram: %s",(USER(user)->Ram=='Y') ? "Yes" : "No");
							outs(text);
							newline();
							newline();
							continue;
							break;
						default:
							continue;
							break;
					}
					newline();
					if(NMEhull<1)
						continue;
					c='F';
					if(NME.Ram=='Y' && dice(4)==1)
						c='R';
					switch(c) {
						case 'F':
							mh=0;
							for(i=0; i<NMEhit; i++)
								mh+=dice(NMEhp)+dice(NMEhp);
							sprintf(text,"The %s attacks your ship, causing %d hull points of damage.",NME.Handle,mh);
							outs(text);
							newline();
							Thull-=mh;
							break;
						case 'R':
							sprintf(text,"The %s attempts to ram you!",NME.Handle);
							outs(text);
							newline();
							newline();
							Delay(100);
							if(dice(50+Tint/2)>(100L*NMEhull/(NMEhull+Thull))) {
								outs("You quickly outmaneuver it.");
								newline();
								break;
							}
							i=dice(NME.Hull/2)+dice(NMEhull/2);
							Thull-=i;
							sprintf(text,"It rams you for %d hull points of damage!",i);
							outs(text);
							newline();
							break;
					}
					newline();
				}
				if(Thull<1) {
					Thull=0;
					sprintf(text,"The %.26s sank your ship!",NME.Handle);
					outs(text);
					newline();
					Delay(50);
					USER(user)->WasKill++;
					if(USER(user)->Gold) {
						USER(0)->Gold+=USER(user)->Gold;
						PutUser(0);
						newline();
						outs("It gets all your money!");
						newline();
						Delay(50);
					}
					USER(user)->Gold=0.;
					PutUser(user);
					sprintf(reason,"sunk by the %.26s",NME.Handle);
					logoff=TRUE;
					sprintf(text,"[#%u] %.26s was %s.",user,USER(user)->Handle,reason);
					RPGmessage(text);
				}
				if(NMEhull<1) {
					NMEhull=0;
					newline();
					cost=value(NME.Gold,Tchr);
					sprintf(text,"%sYou get %s%s for bringing home the carcass.",Fcol(YELLOW),money(cost,user),Fcol(YELLOW));
					outs(text);
					newline();
					USER(user)->Gold+=cost;
					PutUser(user);
				}
				newline();
				break;
			case 'G':
				if(!Thull) {
					outs("You don't have a ship!");
					newline();
					break;
				}
				outs("It is a fine day for sailing.  You cast your reel into the ocean and feel");
				newline();
				outs("a gentle tug...");
				Delay(100);
				outs("you caught a");
				Delay(100);
				i=dice(Tchr);
				if(i<15) {
					nme=dice(MAXUSER-1);
					if(USER(nme)->Status) {
						outs(" floating carcass!");
						newline();
						Delay(100);
						sprintf(text,"It is %.26s's body in the ocean just left there",USER(nme)->Handle);
						outs(text);
						newline();
						sprintf(text,"by %.26s and you're able to bring %s back to life!",USER(USER(nme)->Status)->Handle,(USER(nme)->Sex=='M') ? "him" : "her");
						outs(text);
						newline();
						numline=0;
						sprintf(line[numline++],"%.26s recovered your dead body from the ocean!",USER(user)->Handle);
						UserNote(nme);
						USER(nme)->Status=0;
						PutUser(nme);
						break;
					}
					if(dice(10)==1) {
						outs("n old sea hag!");
						newline();
						Delay(100);
						outs("She cackles as she sends you spinning elsewhere...");
						newline();
						Delay(100);
						newline();
						Dungeon(USER(user)->Level+dice(35)+dice(35)+dice(35));
						if(cd())
							break;
						newline();
						sprintf(text,"%s\"%sSo you have escaped my magic, mortal.  Now try me!%s\"%s",Fcol(MAG),Fcol(YELLOW),Fcol(MAG),Fcol(GRY));
						outs(text);
						newline();
						nme=0;
						CreateMonster(&NME,MAXCLASS-1,MAXLEVEL);
						strcpy(NME.Handle,"an old sea hag");
						NME.Sex='F';	/*  Bitch  */
						NME.Str=20;		/*  Weak bitch  */
						NME.Int=99;		/*  Smart-ass bitch  */
						NME.Weapon=-1;	/*  Bitch with long fingernails  */
						NME.Armor=-1;	/*  Dress the ugly bitch  */
						NME.SP/=2;		/*  Old bitch  */
						NME.HP/=6;		/*  Real old bitch  */
						NME.Spell^=64;	/*  No running away bitch  */
						DungeonPixFile();
						DisplayNME();
						Delay(100);
						Battle();
						if(Thp>0 && NMEhp>0) {
							outs("The old sea hag disintegrates your boat in frustration, so you drown...");
							newline();
							Delay(100);
							USER(user)->Hull=0;
							USER(user)->Cannon=0;
							USER(user)->Ram='N';
							USER(user)->WasKill++;
							PutUser(user);
							strcpy(reason,"sunk by the old sea hag");
							logoff=TRUE;
						}
						break;
					}
					if(dice(10)==1) {
						outs(" titan named Neptune!");
						newline();
						nme=0;
						CreateMonster(&NME,MAXCLASS-2,USER(user)->Level);
						strcpy(NME.Handle,"Neptune of the Sea");
						NME.Sex='M';
						NME.Str=99;
						NME.Int=99;
						NME.Agl=99;
						NME.Weapon=MAXWEAPON-2;
						NME.Armor=MAXARMOR-2;
						NME.Spell^=64;	/*  No teleport  */
						DungeonPixFile();
						DisplayNME();
						Delay(100);
						outs("He looks at you angrily as he removes a hook from his shorts!");
						newline();
						Delay(100);
						newline();
						sprintf(text,"He's carrying a %s!",WEAPON(NME.Weapon)->Name);
						outs(text);
						newline();
						newline();
						sprintf(text,"He also has %s!",ARMOR(NME.Armor)->Name);
						outs(text);
						newline();
						newline();
						sprintf(text,"He has %d hit points.",NME.HP);
						outs(text);
						newline();
						newline();
						Battle();
						if(Thp>0 && NMEhp>0) {
							outs("Neptune smashes your boat in frustration, so you drown...");
							newline();
							Delay(100);
							USER(user)->Hull=0;
							USER(user)->Cannon=0;
							USER(user)->Ram='N';
							USER(user)->WasKill++;
							PutUser(user);
							strcpy(reason,"sunk by Neptune of the Sea");
							logoff=TRUE;
						}
						break;
					}
					outs(" fish and you eat it.");
					newline();
					Delay(100);
					outs("Ugh!  You feel sick and die!");
					newline();
					strcpy(reason,"ugly fish");
					logoff=TRUE;
					break;
				}
				if(i<50) {
					outs(" fish and you eat it.");
					newline();
					Delay(100);
					outs("Yum!  You feel stronger and healthier.");
					newline();
					newline();
					Tstr+=dice(10);
					if(Tstr>100)
						Tstr=100;
					sprintf(text,"Stamina = %d     ",Tstr);
					outs(text);
					Thp+=USER(user)->HP/(dice(11)+9);
					sprintf(text,"Hit points = %d     ",Thp);
					outs(text);
					if(USER(user)->SP) {
						Tsp+=USER(user)->SP/(dice(11)+9);
						sprintf(text,"Spell points = %d",Tsp);
						outs(text);
					}
					newline();
					break;
				}
				if(i<75) {
					outs("n oyster and you eat it.");
					newline();
					Delay(100);
					modf(pow2(USER(user)->Hull/150.)*7937.+.5,&d);
					modf(d/USER(user)->Hull/10.*dice(Thull),&d);
					modf(d*(USER(user)->Cannon+1.)/(USER(user)->Hull/50),&d);
					sprintf(text,"Ouch!  You bit into a pearl and sell it for %s.",money(d,user));
					outs(text);
					newline();
					USER(user)->Gold+=d;
					PutUser(user);
					break;
				}
				if(i<90) {
					outs("n oyster and you eat it.");
					newline();
					Delay(100);
					modf(pow2(USER(user)->Hull/150.)*7937.+.5,&d);
					modf(d/USER(user)->Hull*dice(Thull),&d);
					modf(d*(USER(user)->Cannon+1.)/(USER(user)->Hull/50),&d);
					sprintf(text,"Ouch!  You bit into a diamond and sell it for %s.",money(d,user));
					outs(text);
					newline();
					USER(user)->Gold+=d;
					PutUser(user);
					break;
				}
				if(i<95) {
					outs(" turtle and you let it go.");
					newline();
					Delay(100);
					Tshield+=dice(USER(user)->Armor/5+1);
					sprintf(text,"The turtle turns and smiles and enhances your %s %s.",ARMOR(USER(user)->Armor)->Name,points(Tshield));
					outs(text);
					newline();
					break;
				}
				if(i<100) {
					outs(" turtle and you let it go.");
					newline();
					Delay(100);
					Thit+=dice(USER(user)->Weapon/5+1);
					sprintf(text,"The turtle turns and smiles and enhances your %s %s.",WEAPON(USER(user)->Weapon)->Name,points(Thit));
					outs(text);
					newline();
					break;
				}
				outs(" mermaid!  ");
				Delay(50);
				newline();
				strcpy(NME.Handle,"Mermaid");
				DungeonPixFile();
				if(USER(user)->CallsToday) {
					outs("She grants you an extra call for today!");
					USER(user)->CallsToday--;
					PutUser(user);
				}
				else {
					sprintf(text,"She says, \"%s\"",KeyHint());
					outs(text);
				}
				newline();
				DisplayUser();
				Delay(100);
				break;
			case 'Y':
				if(!Thull) {
					outs("You don't have a ship!");
					newline();
					break;
				}
				outs("Ship's Status:");
				newline();
				newline();
				sprintf(text,"Hull points: %d out of %d",Thull,USER(user)->Hull);
				outs(text);
				newline();
				sprintf(text,"Cannons: %d",USER(user)->Cannon);
				outs(text);
				newline();
				sprintf(text,"Ram: %s",(USER(user)->Ram=='Y') ? "Yes" : "No");
				outs(text);
				newline();
				break;
			case 'L':
				sprintf(OUTbuf,"%s%sNum               Username               Hull      Cannons      Ram%s",Fcol(WHITE),Bcol(BLU),Bcol(BLK));
				outs(OUTbuf);
				newline();
				sprintf(OUTbuf,"%s---      --------------------------      ----      -------      ---%s",Bcol(BLU),Bcol(BLK));
				outs(OUTbuf);
				newline();
				normal();
				lp=2;
				more='Y';
				for(i=1; i<MAXUSER && more!='N' && !cd(); i++) {
					if(USER(i)->Hull) {
						sprintf(text,"%3d      %-26.26s      %4d      %5d        %s",i,USER(i)->Handle,USER(i)->Hull,USER(i)->Cannon,(USER(i)->Ram=='Y') ? "Yes" : "No");
						outs(text);
						newline();
						lp++;
						if(lp%23==0)
							more=More();
					}
				}
				if(more!='N' && lp>7 && lp%22)
					Pause();
				break;
		}
	}
}
