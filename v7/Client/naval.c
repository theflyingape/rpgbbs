#include "/include/all.h"
#include "global.h"

int fire(struct RPC *rpc,struct RPC *enemy)
{
	int i,n,hit,damage,cannon,hull,ram;

	hit=0;
	damage=0;
	cannon=0;
	hull=0;
	ram=FALSE;
	OUT(fore(CYN));
	OUT((rpc==ONLINE) ? "Attacker: " : "Defender: ");
	for(i=0; i<rpc->user.Cannon; i++) {
		n=dice(100);
		n=(n<66) ? 0 : (n<96) ? 1 : (n<100 || !strlen(enemy->user.ID)) ? 2 : 3;
		if(n==3) {
			if(enemy->user.Ram=='Y') {
				ram=TRUE;
				enemy->user.Ram='N';
				OUT(fore(MAGENTA));
				OUT("^");
			}
			else
				n=2;
		}
		if(n==2) {
			if(strlen(enemy->user.ID)>0) {
				if(enemy->user.Cannon>0) {
					cannon++;
					enemy->user.Cannon--;
					OUT(fore(GREEN));
					OUT("@");
				}
			}
			else
				n=1;
		}
		if(n==1) {
			n=dice(50);
			damage+=n;
			if(n<50) {
				OUT(fore(RED));
				OUT("*");
			}
			else
			if(enemy->user.Hull>0) {
				hull+=50;
				enemy->user.Hull-=50;
				OUT(fore(YELLOW));
				if(enemy->user.Hull)
					OUT("#");
				else
					OUT("&");
			}
		}
		if(n==0) {
			OUT(fore(GRY));
			OUT("-");
		}
		else
			hit++;
		Delay(1);
	}
	OUT(fore(CYN));NL;NL;
	sprintf(outbuf,"%s hit %s %d times for %d hull points of damage.",(rpc==ONLINE) ? "You" : "They",(rpc==ONLINE && strlen(enemy->user.ID)) ? "them" : (rpc==ONLINE) ? "it" : "you",hit,damage);
	OUT(outbuf);NL;
	if(cannon) {
		sprintf(outbuf,"%s also hit %d of %s cannons.",(rpc==ONLINE) ? "You" : "They",cannon,(rpc==ONLINE) ? "their" : "your");
		OUT(outbuf);NL;
	}
	if(hull) {
		sprintf(outbuf,"%s also reduced %d hull points of %s ship.",(rpc==ONLINE) ? "You" : "They",hull,(rpc==ONLINE) ? "their" : "your");
		OUT(outbuf);NL;
	}
	if(ram) {
		sprintf(outbuf,"%s also hit %s ram.",(rpc==ONLINE) ? "You" : "They",(rpc==ONLINE) ? "their" : "your");
		OUT(outbuf);NL;
	}
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
	char c,cont;

	while((c=option(&RPGmenu))!='Q') {
		switch(c) {
			case 'S':
				while((c=option(&SHIPmenu))!='Q')
					switch(c) {
						case 'B':
							if(PLAYER.Hull+50>8000) {
								OUT("They don't make ships any bigger than the one you have now.");NL;
								paws=!PLAYER.Expert;
								break;
							}
							if(PLAYER.Hull==0) {
								modf(pow2(50./150.)*7937.+.5,&d);
								if(PLAYER.Gold<d) {
									sprintf(outbuf,"You need at least %s to buy a ship.",money(d,ANSI));
									OUT(outbuf);NL;
									paws=!PLAYER.Expert;
									break;
								}
							}
							OUT("List of affordable ships:");NL;
							mh=PLAYER.Hull+50;
							modf(pow2(mh/150.)*7937.+.5,&d);
							while(mh<=8000 && d<=PLAYER.Gold) {
								sprintf(outbuf,"Hull size: %-4d     Cost: %s",mh,money(d,ANSI));
								OUT(outbuf);NL;
								mh+=50;
								modf(pow2(mh/150.)*7937.+.5,&d);
							}
							NL;
							sprintf(prompt,"%sEnter hull size to buy: ",fore(CYN));
							OUT(prompt);
							if(ins(4))
								break;
							NL;NL;
							i=atoi(inbuf);
							if(i==0)
								break;
							if(i%50) {
								OUT("We don't make ships with that hull size.  Only in multiples of 50.");NL;
								paws=!PLAYER.Expert;
								break;
							}
							if(i<=PLAYER.Hull) {
								sprintf(outbuf,"You already have a %d hull size ship!",PLAYER.Hull);
								OUT(outbuf);NL;
								paws=!PLAYER.Expert;
								break;
							}
							if(i>8000) {
								OUT("We don't make ships that big!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							if(i>=mh) {
								OUT("You don't have enough money!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							if(ACCESS(acclvl)->RolePlay=='Y') {
								modf(pow2(i/150.)*7937.+.5,&d);
								PLAYER.Gold-=d;
								PLAYER.Hull=i;
								PLAYER.Ram='N';
								ONLINE->HULL=PLAYER.Hull;
								RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
								sprintf(outbuf,"You now have a brand new %d hull point ship, with no ram.",ONLINE->HULL);
								OUT(outbuf);NL;
								paws=!PLAYER.Expert;
							}
							break;
						case 'F':
							if(!PLAYER.Hull) {
								OUT("You don't have a ship!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							mh=PLAYER.Hull-ONLINE->HULL;
							sprintf(outbuf,"You need %d hull points of repair.",mh);
							OUT(outbuf);NL;
							modf(pow2(PLAYER.Hull/150.)*7937.+.5,&cost);
							modf(cost/PLAYER.Hull/10.,&cost);
							sprintf(outbuf,"Each hull point costs %s.",money(cost,ANSI));
							OUT(outbuf);NL;NL;
							modf(PLAYER.Gold/cost,&d);
							if(d<mh)
								mh=d;
							sprintf(prompt,"%sHow many points [MAX=%d]? ",fore(CYN),mh);
							OUT(prompt);
							if(ins(4))
								break;
							NL;
							i=atoi(inbuf);
							if(!stricmp(inbuf,"max")) {
								sound("max",64);
								i=mh;
							}
							if(i<1 || i>mh)
								break;
							if(ACCESS(acclvl)->RolePlay=='Y') {
								ONLINE->HULL+=i;
								PLAYER.Gold-=cost*i;
								sprintf(outbuf,"Hull points = %d",ONLINE->HULL);
								OUT(outbuf);NL;
								paws=!PLAYER.Expert;
							}
							break;
						case 'C':
							if(!PLAYER.Hull) {
								OUT("You don't have a ship!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							mh=PLAYER.Hull/50-PLAYER.Cannon;
							sprintf(outbuf,"You can mount up to %d more cannons.",mh);
							OUT(outbuf);NL;
							modf(pow2(PLAYER.Hull/150.)*7937.+.5,&cost);
							modf(cost/250.,&cost);
							sprintf(outbuf,"Each cannon costs %s.",money(cost,ANSI));
							OUT(outbuf);NL;NL;
							modf(PLAYER.Gold/cost,&d);
							if(d<mh)
								mh=d;
							sprintf(prompt,"%sHow many cannons [MAX=%d]? ",fore(CYN),mh);
							OUT(prompt);
							if(ins(3))
								break;
							NL;
							i=atoi(inbuf);
							if(!stricmp(inbuf,"max")) {
								sound("max",64);
								i=mh;
							}
							if(i<1 || i>mh)
								break;
							if(ACCESS(acclvl)->RolePlay=='Y') {
								PLAYER.Cannon+=i;
								PLAYER.Gold-=cost*i;
								RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
								sprintf(outbuf,"Cannons = %d",PLAYER.Cannon);
								OUT(outbuf);NL;
								paws=!PLAYER.Expert;
							}
							break;
						case 'R':
							if(!PLAYER.Hull) {
								OUT("You don't have a ship!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							if(PLAYER.Ram=='Y') {
								OUT("But your ship already has a ram!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							modf(pow2(PLAYER.Hull/150.)*7937.+.5,&cost);
							modf(cost/10.,&cost);
							sprintf(outbuf,"We can equip your ship with a ram for %s.",money(cost,ANSI));
							OUT(outbuf);NL;NL;
							modf(PLAYER.Gold/cost,&d);
							if(d<1.) {
								OUT("You don't have enough money!");NL;
								paws=!PLAYER.Expert;
								break;
							}
							sprintf(prompt,"%sOk (Y/N)? ",fore(CYN));
							OUT(prompt);
							c=inkey('Y','Y');
							NL;
							if(c!='Y')
								break;
							if(ACCESS(acclvl)->RolePlay=='Y') {
								PLAYER.Ram='Y';
								PLAYER.Gold-=cost;
								RPGserver(SERVER_PUTUSER,(UBYTE *)&PLAYER);
								OUT("You now have a ram.");NL;
								paws=!PLAYER.Expert;
							}
							break;
					}
				break;
			case 'B':
				if(!PLAYER.Hull) {
					OUT("You don't have a ship!");NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(!naval) {
					OUT("You have run out of battles.");NL;
					paws=!PLAYER.Expert;
					break;
				}
				sprintf(prompt,"%sBattle which user? ",fore(CYN));
				if(!GetRPC(RPC[1][0]))
					break;
				if(!RPC[1][0]->HULL) {
					sprintf(outbuf,"%s does not have a ship.",RPC[1][0]->He);
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(!strcmp(PLAYER.ID,ENEMY.ID)) {
					sprintf(outbuf,"What's the matter with you, %s!",(PLAYER.Sex=='M') ? "boy" : "girl");
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
					break;
				}
				sprintf(outbuf,"You sail out until you spot %s's ship on the horizon.",ENEMY.Handle);
				OUT(outbuf);NL;NL;
				sprintf(outbuf,"It has %d hull points and is armed with %d cannons.",RPC[1][0]->HULL,ENEMY.Cannon);
				OUT(outbuf);NL;NL;
				if(ENEMY.Ram=='Y') {
					OUT("It also has a ram.");NL;NL;
				}
				sprintf(prompt,"%sContinue (Y/N)? ",fore(CYN));
				OUT(prompt);
				c=inkey('N','N');
				NL;	NL;
				if(c!='Y')
					break;
				naval--;
				cont='Y';
				if((ONLINE->INT+dice(100))>=(RPC[1][0]->INT+dice(100))) {
					OUT("You approach them and quickly open fire.");NL;NL;
					i=fire(ONLINE,RPC[1][0]);
					RPC[1][0]->HULL-=i;
					PLAYER.History.Hull+=i;
					NL;
				}
				else {
					OUT("They spot you coming and ");
				}
				if(RPC[1][0]->HULL>0 && ENEMY.Cannon>0) {
					OUT("open fire.");NL;NL;
					i=fire(RPC[1][0],ONLINE);
					ONLINE->HULL-=i;
					ENEMY.History.Hull+=i;
				}
				else
				if(ENEMY.Ram=='Y') {
					OUT("attempt to ram you!");NL;NL;
					Delay(50);
					if(dice(50+ONLINE->INT/2)>(100L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL))) {
						OUT("You quickly outmaneuver their ship.");NL;
					}
					else {
						i=dice(RPC[1][0]->HULL/2)+dice(RPC[1][0]->HULL/2);
						ONLINE->HULL-=i;
						ENEMY.History.Hull+=i;
						sprintf(outbuf,"They ram you for %d hull points of damage!",i);
						OUT(outbuf);NL;
					}
				}
				if(RPC[1][0]->HULL>0 && ENEMY.Cannon==0 && ENEMY.Ram!='Y') {
					OUT("attempt to flee...");
					Delay(50);
					NL;
					if(dice(50+ONLINE->INT/2)>(50L*ONLINE->HULL/(ONLINE->HULL+RPC[1][0]->HULL)+50L)) {
						OUT("You outmaneuver them and stop their retreat!");NL;
					}
					else {
						OUT("They sail away over the horizon.");
						cont='N';
					}
				}
				while(ONLINE->HULL>0 && RPC[1][0]->HULL>0 && cont=='Y') {
					NL;
					sprintf(prompt,"%s<F> Fire cannons, <R> Ram their ship, <S> Sail off, <Y> Your status: ",fore(CYN));
					OUT(prompt);
					c=inkey('F','F');
					NL;NL;
					switch(c) {
						case 'F':
							average=PLAYER.Cannon/3*25;
							i=fire(ONLINE,RPC[1][0]);
							if(i<(average-average/3)) {
								NL;
								OUT(fore(CYN));
								OUT("You scream at the gunner's mate, \"");
								NORMAL;
								switch(dice(3)) {
									case 1:
										OUT("Aim higher, you uneducated bloke!");
										break;
									case 2:
										OUT("Fire on crest to hit the best!");
										break;
									case 3:
										OUT("We're all gonna bloody die with your aim!");
										break;
								}
								OUT(fore(CYN));
								OUT("\"");NL;
								Delay(50);
							}
							if(i>(average+average/3)) {
								NL;
								OUT(fore(CYN));
								OUT("You exclaim to the gunner's mate, \"");
								NORMAL;
								switch(dice(3)) {
									case 1:
										OUT("YES!  She's all ours now!");
										break;
									case 2:
										OUT("That's the way to shoot them dogs!");
										break;
									case 3:
										OUT("She's listing badly now!");
										break;
								}
								OUT(fore(CYN));
								OUT("\"");NL;
								Delay(50);
							}
							RPC[1][0]->HULL-=i;
							PLAYER.History.Hull+=i;
							break;
						case 'R':
							if(PLAYER.Ram!='Y') {
								OUT("You don't have a ram!");NL;NL;
								continue;
							}
							if(dice(50+RPC[1][0]->INT/2)>(100L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL))) {
								OUT("They quickly outmaneuver your ship.");NL;NL;
								OUT(fore(CYN));
								OUT("You yell at your helmsman, \"");
								NORMAL;
								switch(dice(3)) {
									case 1:
										OUT("@#$%!");
										break;
									case 2:
										OUT("I said port, dolt, not starboard!");
										break;
									case 3:
										OUT("No more mead before duty for you!");
										break;
								}
								OUT(fore(CYN));
								OUT("\"");NL;
								Delay(50);
								break;
							}
							i=dice(PLAYER.Hull/2)+dice(ONLINE->HULL/2);
							RPC[1][0]->HULL-=i;
							PLAYER.History.Hull+=i;
							sprintf(outbuf,"You ram them for %d hull points of damage!",i);
							OUT(outbuf);NL;
							break;
						case 'S':
							if(dice(50+RPC[1][0]->INT/2)>(50L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL)+50L)) {
								OUT("They outmaneuver you and stop your retreat!");NL;
								break;
							}
							OUT("You sail away safely out of range.");
							paws=!PLAYER.Expert;
							cont='N';
							numline=0;
							sprintf(line[numline++],"%s, the coward, sailed away from you.",PLAYER.Handle);
							note(ENEMY.ID);
							continue;
							break;
						case 'Y':
							sprintf(outbuf,"%sHull points:%s %d",fore(GRN),fore(WHITE),ONLINE->HULL);
							OUT(outbuf);NL;
							sprintf(outbuf,"%sCannons:%s %d",fore(GRN),fore(WHITE),PLAYER.Cannon);
							OUT(outbuf);NL;
							sprintf(outbuf,"%sRam:%s %s",fore(GRN),fore(WHITE),(PLAYER.Ram=='Y') ? "Yes" : "No");
							OUT(outbuf);NL;NL;
							continue;
							break;
						default:
							continue;
							break;
					}
					NL;
					if(RPC[1][0]->HULL<1)
						continue;
					c='F';
					if(!ENEMY.Cannon && ENEMY.Ram!='Y') {
						OUT("They are defenseless and attempt to flee...");NL;
						Delay(50);
						c='S';
					}
					if(ENEMY.Ram=='Y' && (dice(5)==1 || ENEMY.Cannon==0))
						c='R';
					if(c=='F' && RPC[1][0]->HULL<ONLINE->HULL && ENEMY.Cannon<PLAYER.Cannon && dice(29-RPC[1][0]->INT/10)==1) {
						sprintf(outbuf,"%s salutes your superior naval tactics and tries to retreat...",ENEMY.Handle);
						OUT(outbuf);NL;
						Delay(50);
						c='S';
					}
					switch(c) {
						case 'F':
							i=fire(RPC[1][0],ONLINE);
							ONLINE->HULL-=i;
							ENEMY.History.Hull+=i;
							break;
						case 'R':
							OUT("They attempt to ram you!");NL;NL;
							Delay(50);
							if(dice(50+ONLINE->INT/2)>(100L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL))) {
								OUT("You quickly outmaneuver their ship.");
								NL;
								break;
							}
							i=dice(RPC[1][0]->HULL/2)+dice(RPC[1][0]->HULL/2);
							ONLINE->HULL-=i;
							ENEMY.History.Hull+=i;
							sprintf(outbuf,"They ram you for %d hull points of damage!",i);
							OUT(outbuf);NL;
							break;
						case 'S':
							if(dice(50+ONLINE->INT/2)>(50L*ONLINE->HULL/(ONLINE->HULL+RPC[1][0]->HULL)+50L)) {
								OUT("You outmaneuver them and stop their retreat!");NL;
								break;
							}
							OUT("They sail away over the horizon.");
							paws=!PLAYER.Expert;
							cont='N';
							break;
					}
					NL;
				}
				if(ONLINE->HULL<1) {
					ONLINE->HULL=0;
					sprintf(outbuf,"%s smiles as a shark approaches you...",ENEMY.Handle);
					OUT(outbuf);NL;
					modf(pow2(ENEMY.Hull/150.)*7937.+.5,&cost);
					modf(cost/250.,&cost);
					modf(cost*ENEMY.Hull/50.,&d);
					cost*=PLAYER.Cannon;
					cost=value(cost,RPC[1][0]->CHA)+(PLAYER.Gold>d ? d : PLAYER.Gold);
					ENEMY.Gold+=cost;
					numline=0;
					sprintf(line[numline++],"You sank %s's ship!",PLAYER.Handle);
					if(PLAYER.Gold)
						sprintf(line[numline++],"You got %s.",money(cost,ENEMY.Emulation));
					note(ENEMY.ID);
					PLAYER.Gold=0.;
					PLAYER.Current.Killed++;
					PLAYER.History.Killed++;
					Delay(50);
					logoff=TRUE;
					sprintf(reason,"sunk by %s",ENEMY.Handle);
				}
				if(RPC[1][0]->HULL<1) {
					RPC[1][0]->HULL=0;
					switch(dice(5)) {
						case 1:
							sprintf(outbuf,"You've sunk %s's ship!",ENEMY.Handle);
							break;
						case 2:
							sprintf(outbuf,"You've sunk %s's leaky, old tub!",ENEMY.Handle);
							break;
						case 3:
							sprintf(outbuf,"You've made splinters out of %s's ship!",ENEMY.Handle);
							break;
						case 4:
							sprintf(outbuf,"%s is now sleeping with the fishes!",ENEMY.Handle);
							break;
						case 5:
							sprintf(outbuf,"%s is now chum for the sharks!",ENEMY.Handle);
							break;
					}
					OUT(outbuf);NL;NL;
					modf(pow2(PLAYER.Hull/150.)*7937.+.5,&cost);
					modf(cost/250.,&cost);
					modf(cost*PLAYER.Hull/50.,&d);
					cost*=ENEMY.Cannon;
					cost=value(cost,ONLINE->CHA)+(ENEMY.Gold>d ? d : ENEMY.Gold);
					sound("booty",64);
					if(ENEMY.Gold>d) {
						sprintf(outbuf,"%s of the booty has settled on the ocean floor...",money(ENEMY.Gold-d,ANSI));
						OUT(outbuf);
						Delay(100);
						NL;
					}
					sprintf(outbuf,"You get %s of booty!",money(cost,ANSI));
					OUT(outbuf);NL;NL;
					PLAYER.Gold+=cost;
					numline=0;
					sprintf(line[numline++],"%s sank your ship!",PLAYER.Handle);
					if(ENEMY.Gold)
						sprintf(line[numline++],"They got %s.",money(cost,ENEMY.Emulation));
					sprintf(prompt,"%sWould you like to leave %s a comment (Y/N)? ",fore(CYN),RPC[1][0]->him);
					OUT(prompt);
					c=inkey('Y','Y');
					NL;
					if(c=='Y') {
						sprintf(prompt,"%sEnter a comment on the line below.",fore(CYN));
						OUT(prompt);NL;
						sprintf(prompt,"%s>",fore(CYN));
						OUT(prompt);
						ins(78);NL;
						jazz(inbuf);
						sprintf(line[numline++],"%s says, \"%s\"",HE(ONLINE),inbuf);
					}
					note(ENEMY.ID);
					ENEMY.Gold=0.;
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
				NL;
				break;
			case 'H':
				if(!PLAYER.Hull) {
					OUT("You don't have a ship!");NL;
					paws=!PLAYER.Expert;
					break;
				}
				if(!naval) {
					OUT("You have run out of battles.");NL;
					break;
				}
				for(i=0; i<NUMNAVAL; i++) {
					sprintf(outbuf,"%s%s %s",bracket(i+1),fore(CYN),NAVAL(i)->Name);
					OUT(outbuf);NL;
				}
				NL;
				sprintf(prompt,"%sHunt which monster? ",fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL;
				mh=atoi(inbuf);
				if(mh<1 || mh>NUMNAVAL)
					break;
				mh--;
				memset(RPC[1][0],0,sizeof(struct RPC));
				strcpy(ENEMY.Handle,NAVAL(mh)->Name);
				RPC[1][0]->INT=NAVAL(mh)->INT;
				RPC[1][0]->HULL=NAVAL(mh)->Hull;
				RPC[1][0]->user.Ram=NAVAL(mh)->Ram;
				ENEMY.Gold=NAVAL(mh)->Value;
				RPC[1][0]->ToWC=NAVAL(mh)->Shot;
				RPC[1][0]->HP=NAVAL(mh)->Power;
				NL;
				sprintf(outbuf,"You sail out until you spot a %s on the horizon.",ENEMY.Handle);
				OUT(outbuf);NL;NL;
				sprintf(outbuf,"It has %d hull points.",RPC[1][0]->HULL);
				OUT(outbuf);NL;NL;
				sprintf(prompt,"%sContinue (Y/N)? ",fore(CYN));
				OUT(prompt);
				c=inkey('N','N');
				NL;NL;
				if(c!='Y')
					break;
				naval--;
				cont='Y';
				if((ONLINE->INT+dice(100))>=(RPC[1][0]->INT+dice(100))) {
					OUT("You approach it and quickly open fire.");NL;NL;
					i=fire(ONLINE,RPC[1][0]);
					RPC[1][0]->HULL-=i;
					PLAYER.History.Hull+=i;
					if(RPC[1][0]->HULL>0) {
						mh=0;
						for(i=0; i<RPC[1][0]->ToWC; i++)
							mh+=dice(RPC[1][0]->HP)+dice(RPC[1][0]->HP);
						sprintf(outbuf,"The %s attacks your ship, causing %d hull points of damage.",ENEMY.Handle,mh);
						OUT(outbuf);NL;
						ONLINE->HULL-=mh;
						ENEMY.History.Hull+=mh;
					}
				}
				else {
					OUT("It spots you coming and attacks.");NL;NL;
					mh=0;
					for(i=0; i<RPC[1][0]->ToWC; i++)
						mh+=dice(RPC[1][0]->HP)+dice(RPC[1][0]->HP);
					sprintf(outbuf,"The %s attacks your ship, causing %d hull points of damage.",ENEMY.Handle,mh);
					OUT(outbuf);NL;
					ONLINE->HULL-=mh;
					ENEMY.History.Hull+=mh;
				}
				while(ONLINE->HULL>0 && RPC[1][0]->HULL>0 && cont=='Y') {
					NL;
					sprintf(prompt,"%s<F> Fire cannons, <R> Ram it, <S> Sail off, <Y> Your status: ",fore(CYN));
					OUT(prompt);
					c=inkey('F','F');
					NL;NL;
					switch(c) {
						case 'F':
							average=PLAYER.Cannon/3*25;
							i=fire(ONLINE,RPC[1][0]);
							if(i<(average-average/3)) {
								NL;
								OUT(fore(CYN));
								OUT("You scream at the gunner's mate, \"");
								NORMAL;
								switch(dice(3)) {
									case 1:
										OUT("Cannonballs, not beer cans!");
										break;
									case 2:
										OUT("Hit it, or I'll hit you!");
										break;
									case 3:
										OUT("We're gonna be bloody supper to that beast!");
										break;
								}
								OUT(fore(CYN));
								OUT("\"");NL;
								Delay(50);
							}
							if(i>(average+average/3)) {
								NL;
								OUT(fore(CYN));
								OUT("You exclaim to the gunner's mate, \"");
								NORMAL;
								switch(dice(3)) {
									case 1:
										OUT("Good show, mate!");
										break;
									case 2:
										OUT("It's bleeding badly!");
										break;
									case 3:
										OUT("One more volley like that!");
										break;
								}
								OUT(fore(CYN));
								OUT("\"");NL;
								Delay(50);
							}
							RPC[1][0]->HULL-=i;
							PLAYER.History.Hull+=i;
							break;
						case 'R':
							if(PLAYER.Ram!='Y') {
								OUT("You don't have a ram!");NL;NL;
								continue;
							}
							if(dice(50+RPC[1][0]->INT/2)>(100L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL))) {
								OUT("It quickly outmaneuvers your ship.");NL;NL;
								OUT(fore(CYN));
								OUT("You yell at your helmsman, \"");
								NORMAL;
								switch(dice(3)) {
									case 1:
										OUT("Aim for the head, not the tail!");
										break;
									case 2:
										OUT("I said starboard, asshole, not port!");
										break;
									case 3:
										OUT("Whose side are you on anyways?!");
										break;
								}
								OUT(fore(CYN));
								OUT("\"");NL;
								Delay(50);
								break;
							}
							i=dice(PLAYER.Hull/2)+dice(ONLINE->HULL/2);
							RPC[1][0]->HULL-=i;
							PLAYER.History.Hull+=i;
							sprintf(outbuf,"You ram it for %d hull points of damage!",i);
							OUT(outbuf);NL;
							break;
						case 'S':
							if(dice(50+RPC[1][0]->INT/2)>(50L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL)+50L)) {
								OUT("It outmaneuvers you and stops your retreat!");NL;
								break;
							}
							OUT("You sail away safely out of range.");
							cont='N';
							continue;
							break;
						case 'Y':
							sprintf(outbuf,"Hull points: %d",ONLINE->HULL);
							OUT(outbuf);NL;
							sprintf(outbuf,"Cannons: %d",PLAYER.Cannon);
							OUT(outbuf);NL;
							sprintf(outbuf,"Ram: %s",(PLAYER.Ram=='Y') ? "Yes" : "No");
							OUT(outbuf);NL;NL;
							continue;
							break;
						default:
							continue;
							break;
					}
					NL;
					if(RPC[1][0]->HULL<1)
						continue;
					c='F';
					if(ENEMY.Ram=='Y' && dice(4)==1)
						c='R';
					switch(c) {
						case 'F':
							mh=0;
							for(i=0; i<RPC[1][0]->ToWC; i++)
								mh+=dice(RPC[1][0]->HP)+dice(RPC[1][0]->HP);
							sprintf(outbuf,"The %s attacks your ship, causing %d hull points of damage.",ENEMY.Handle,mh);
							OUT(outbuf);NL;
							ONLINE->HULL-=mh;
							ENEMY.History.Hull+=mh;
							break;
						case 'R':
							sprintf(outbuf,"The %s attempts to ram you!",ENEMY.Handle);
							OUT(outbuf);NL;NL;
							Delay(50);
							if(dice(50+ONLINE->INT/2)>(100L*RPC[1][0]->HULL/(RPC[1][0]->HULL+ONLINE->HULL))) {
								OUT("You quickly outmaneuver it.");
								NL;
								break;
							}
							i=dice(RPC[1][0]->HULL/2)+dice(RPC[1][0]->HULL/2);
							ONLINE->HULL-=i;
							ENEMY.History.Hull+=i;
							sprintf(outbuf,"It rams you for %d hull points of damage!",i);
							OUT(outbuf);NL;
							break;
					}
					NL;
				}
				if(ONLINE->HULL<1) {
					ONLINE->HULL=0;
					sprintf(outbuf,"The %s sank your ship!",ENEMY.Handle);
					OUT(outbuf);NL;
					Delay(50);
					PLAYER.Current.Killed++;
					PLAYER.History.Killed++;
					if(PLAYER.Gold) {
						NL;OUT("It gets all your money!");NL;
						Delay(50);
					}
					PLAYER.Gold=0.;
					logoff=TRUE;
					sprintf(reason,"sunk by the %s",ENEMY.Handle);
				}
				if(RPC[1][0]->HULL<1) {
					RPC[1][0]->HULL=0;
					cost=value(ENEMY.Gold,ONLINE->CHA);
					PLAYER.Gold+=cost;
					sound("booty",64);
					sprintf(outbuf,"%sYou get %s%s for bringing home the carcass.",fore(YELLOW),money(cost,ANSI),fore(YELLOW));
					OUT(outbuf);NL;
					paws=!PLAYER.Expert;
				}
				NL;
				break;
			case 'G':
				paws=!PLAYER.Expert;
				if(!PLAYER.Hull) {
					OUT("You don't have a ship!");NL;
					break;
				}
				PLAYER.History.Fish++;
				OUT("It is a fine day for sailing.  You cast your reel into the ocean and feel");NL;
				OUT("a gentle tug...");
				Delay(50);
				OUT("you caught a");
				Delay(50);
				if(ACCESS(acclvl)->RolePlay!='Y') {
					OUT(" cold!  You feel sick and die!");NL;NL;
					Delay(50);
					logoff=TRUE;
					strcpy(reason,"caught a cold");
					break;
				}
				average=100L*ONLINE->CHA/CLASS(ONLINE)->MaxCHA;
				average=(average<15) ? 15 : (average>100) ? 100 : average;
				i=dice(average);
				if(i<15) {
					RPGserver(SERVER_RNDUSER,(UBYTE *)&RPC[1][0]->user);
					if(strlen(ENEMY.Status) && strcmp(ENEMY.Status,"jail")) {
						LoadRPC(RPC[1][0]);
						OUT(" floating carcass!");NL;
						Delay(50);
						sprintf(outbuf,"It is %s's body in the ocean just left there by %s and",ENEMY.Handle,ENEMY.Status);
						OUT(outbuf);NL;
						sprintf(outbuf,"you're able to bring %s back to life!",RPC[1][0]->him);
						OUT(outbuf);NL;NL;
						numline=0;
						sprintf(line[numline++],"%s recovered your dead body from the ocean!",PLAYER.Handle);
						note(ENEMY.ID);
						memset(ENEMY.Status,0,sizeof(ENEMY.Status));
						RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
						break;
					}
					memset(RPC[1][0],0,sizeof(struct RPC));
					if(dice(PLAYER.Level/3+2)==1) {
						OUT("n old sea hag!");NL;
						Delay(50);
						OUT("It cackles as you are sent spinning elsewhere...");NL;
						Delay(50);
						Dungeon(PLAYER.Level+3*dice(PLAYER.Level));
						if(cd())
							break;
						sound("god",64);
						NL;
						sprintf(outbuf,"%s\"%sSo you have escaped my magic, mortal.  Now try me!%s\"%s",fore(MAG),fore(YELLOW),fore(MAG),fore(GRY));
						OUT(outbuf);NL;
						strcpy(ENEMY.Handle,"old sea hag");
						ENEMY.Level=99;
						ENEMY.Gender='I';
						strcpy(ENEMY.Class,"IMMORTAL.God");
						sprintf(ENEMY.Weapon,"NATURAL.%d",dice(100));
						for(i=0; i<NUMWEAPON; i++)
							if(!stricmp(ServerTable->weapon[i]->Origin,"DUNGEON"))
								strmfe(RPC[1][0]->user.Weapon,ServerTable->weapon[i]->Origin,ServerTable->weapon[i]->Item[dice(MAXWEAPON(i))-1]->Name);
						sprintf(ENEMY.Armor,"NATURAL.%d",dice(50));
						for(i=0; i<NUMARMOR; i++)
							if(!stricmp(ServerTable->armor[i]->Origin,"DUNGEON"))
								strmfe(RPC[1][0]->user.Armor,ServerTable->armor[i]->Origin,ServerTable->armor[i]->Item[dice(MAXARMOR(i))-1]->Name);
						CreateRPC(RPC[1][0]);
						ENEMY.STR=80;RPC[1][0]->STR=70;
						ENEMY.INT=99;RPC[1][0]->INT=89;
						ENEMY.DEX=85;RPC[1][0]->DEX=75;
						ENEMY.CHA=50;RPC[1][0]->CHA=40;
						ENEMY.HP/=4;RPC[1][0]->HP/=4;
						ENEMY.Poison=(UWORD)~0;
						ENEMY.Spell=(UWORD)~0;
						ENEMY.Spell&=~TELEPORT_SPELL;
						ENEMY.XSpell=(UBYTE)~0;
						strmfp(filename,"PIX/Naval",ENEMY.Handle);
						type(filename,FALSE);
						Delay(50);
						Battle();
						if(!RPC[1][0]->HP) {
							modf(EXP(ENEMY.ExpLevel-1)/3.,&d);
							PLAYER.Experience+=d;
							sprintf(outbuf,"You get %.8g experience.",d);
							OUT(outbuf);NL;NL;
							ExchangeWeapon(ONLINE,RPC[1][0]);
							ExchangeArmor(ONLINE,RPC[1][0]);
							sprintf(outbuf,"%sdefeated the %s, a level %s%u %s",fore(CYN),ENEMY.Handle,fore(WHITE),ENEMY.Level,CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						else {
							strcpy(PLAYER.Cursed,"hag");
							sprintf(outbuf,"%sA %sblack%s cloud appears and hovers over you...",fore(GRY),fore(BLACK),fore(GRY));
							OUT(outbuf);
							Delay(50);NL;
						}
						if(ONLINE->HP>0 && RPC[1][0]->HP>0) {
							OUT("The old sea hag disintegrates your boat in frustration and you drown...");
							Delay(50);NL;
							PLAYER.Hull=0;
							PLAYER.Cannon=0;
							PLAYER.Ram='N';
							PLAYER.Current.Killed++;
							PLAYER.History.Killed++;
							strcpy(reason,"sunk by the old sea hag");
							logoff=TRUE;
						}
						break;
					}
					if(dice(PLAYER.Level/3+2)==1) {
						OUT(" titan named Neptune!");NL;
						strcpy(ENEMY.Handle,"Neptune of the Sea");
						ENEMY.Level=PLAYER.Level+dice(3)-2;
						strcpy(ENEMY.Class,"IMMORTAL.Titan");
						strcpy(ENEMY.Weapon,"NAVAL.Trident of Titans");
						strcpy(ENEMY.Armor,"NAVAL.Titanium Armor");
						CreateRPC(RPC[1][0]);
						ENEMY.HP*=3;
						ENEMY.HP/=ENEMY.Level/11+1;
						RPC[1][0]->HP=ENEMY.HP;
						ENEMY.Spell=(UWORD)~0;
						ENEMY.Spell&=~TELEPORT_SPELL;
						ENEMY.XSpell=(UBYTE)~0;
						strmfp(filename,"PIX/Naval",ENEMY.Handle);
						type(filename,FALSE);
						Delay(50);
						OUT("He looks at you angrily as he removes a hook from his shorts!");NL;
						Delay(50);
						NL;
						sprintf(outbuf,"He's carrying a %s!",WEAPON(RPC[1][0])->Name);
						OUT(outbuf);NL;NL;
						sprintf(outbuf,"He also has %s!",ARMOR(RPC[1][0])->Name);
						OUT(outbuf);NL;NL;
						Battle();
						if(!RPC[1][0]->HP) {
							modf(EXP(RPC[1][0]->user.ExpLevel-1)/15.,&d);
							PLAYER.Experience+=d;
							sprintf(outbuf,"You get %.8g experience.",d);
							OUT(outbuf);NL;NL;
							ExchangeWeapon(ONLINE,RPC[1][0]);
							ExchangeArmor(ONLINE,RPC[1][0]);
							sprintf(outbuf,"%sdefeated %s, a level %s%u %s",fore(CYN),ENEMY.Handle,fore(WHITE),ENEMY.Level,CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						if(ONLINE->HP>0 && RPC[1][0]->HP>0) {
							OUT("Neptune smashes your boat in frustration and you drown...");
							Delay(50);NL;
							PLAYER.Hull=0;
							PLAYER.Cannon=0;
							PLAYER.Ram='N';
							PLAYER.Current.Killed++;
							PLAYER.History.Killed++;
							strcpy(reason,"sunk by Neptune of the Sea");
							logoff=TRUE;
						}
						break;
					}
					OUT(" fish and you eat it.");NL;
					Delay(50);
					OUT("Ugh!  You feel sick and die!");NL;NL;
					strcpy(reason,"ate yesterday's catch of the day");
					logoff=TRUE;
					break;
				}
				if(i<50) {
					OUT(" fish and you eat it.");NL;
					Delay(50);
					sound("yum",64);
					OUT("Yum!  You feel stronger and healthier.");NL;NL;
					if(ONLINE->STR<CLASS(ONLINE)->MaxSTR) {
						ONLINE->STR+=dice(10);
						if(ONLINE->STR>CLASS(ONLINE)->MaxSTR)
							ONLINE->STR=CLASS(ONLINE)->MaxSTR;
					}
					else
					if(ONLINE->STR>=CLASS(ONLINE)->MaxSTR)
						ONLINE->STR++;
					if(ONLINE->STR>100)
						ONLINE->STR=100;
					sprintf(outbuf,"Stamina = %d     ",ONLINE->STR);
					OUT(outbuf);
					ONLINE->HP+=PLAYER.HP/(dice(11)+9);
					sprintf(outbuf,"Hit points = %d     ",ONLINE->HP);
					OUT(outbuf);
					if(PLAYER.SP) {
						ONLINE->SP+=PLAYER.SP/(dice(11)+9);
						sprintf(outbuf,"Spell points = %d",ONLINE->SP);
						OUT(outbuf);
					}
					NL;NL;
					break;
				}
				if(i<75) {
					OUT("n oyster and you eat it.");NL;
					Delay(50);
					modf(pow2(PLAYER.Hull/150.)*7937.+.5,&d);
					modf(d/PLAYER.Hull/10.*dice(ONLINE->HULL),&d);
					modf(d*(PLAYER.Cannon+1.)/(PLAYER.Hull/50),&d);
					d=value(d,ONLINE->CHA);
					sprintf(outbuf,"Ouch!  You bit into a pearl and sell it for %s.",money(d,ANSI));
					OUT(outbuf);NL;NL;
					PLAYER.Gold+=d;
					break;
				}
				if(i<90) {
					OUT("n oyster and you eat it.");NL;
					Delay(50);
					modf(pow2(PLAYER.Hull/150.)*7937.+.5,&d);
					modf(d/PLAYER.Hull*dice(ONLINE->HULL),&d);
					modf(d*(PLAYER.Cannon+1.)/(PLAYER.Hull/50.),&d);
					d=value(d,ONLINE->CHA);
					sprintf(outbuf,"Ouch!  You bit into a diamond and sell it for %s.",money(d,ANSI));
					OUT(outbuf);NL;NL;
					PLAYER.Gold+=d;
					break;
				}
				if(i<95) {
					OUT(" turtle and you let it go.");NL;
					Delay(50);
					PLAYER.ACmod++;
					ONLINE->ToAC+=dice(ARMOR(ONLINE)->Class/5+1);
					sprintf(outbuf,"The turtle turns and smiles and enhances your %s",ARMOR(ONLINE)->Name);
					OUT(outbuf);
					sprintf(outbuf," %s(%s%+d%s,%s%+d%s).",fore(MAG),fore(PLAYER.ACmod>0 ? YELLOW : PLAYER.ACmod<0 ? BRED : GRY),PLAYER.ACmod,fore(GRY),fore(ONLINE->ToAC>0 ? YELLOW : ONLINE->ToAC<0 ? BRED : GRY),ONLINE->ToAC,fore(MAG));
					OUT(outbuf);NL;NL;
					break;
				}
				if(i<100) {
					OUT(" turtle and you let it go.");NL;
					Delay(50);
					PLAYER.WCmod++;
					ONLINE->ToWC+=dice(WEAPON(ONLINE)->Class/10+1);
					sprintf(outbuf,"The turtle turns and smiles and enhances your %s",WEAPON(ONLINE)->Name);
					OUT(outbuf);
					sprintf(outbuf," %s(%s%+d%s,%s%+d%s).",fore(MAG),fore(PLAYER.WCmod>0 ? YELLOW : PLAYER.WCmod<0 ? BRED : GRY),PLAYER.WCmod,fore(GRY),fore(ONLINE->ToWC>0 ? YELLOW : ONLINE->ToWC<0 ? BRED : GRY),ONLINE->ToWC,fore(MAG));
					OUT(outbuf);NL;NL;
					break;
				}
				OUT(" mermaid!");
				Delay(50);
				NL;
				strcpy(ENEMY.Handle,"Mermaid");
				strmfp(filename,"PIX/Naval",ENEMY.Handle);
				type(filename,FALSE);
				if(PLAYER.CallsToday) {
					OUT("She grants you an extra call for today!");NL;
					PLAYER.CallsToday--;
					sprintf(outbuf,"%scaught an extra call",fore(BLUE));
					news(outbuf);
				}
				else {
					sprintf(outbuf,"She says, \"Here's a key hint:\"");
					OUT(outbuf);NL;
					keyhint();
				}
				NL;
				break;
			case 'Y':
				paws=!PLAYER.Expert;
				if(!PLAYER.Hull) {
					OUT("You don't have a ship!");NL;
					break;
				}
				OUT("Ship's Status:");NL;NL;
				sprintf(outbuf,"Hull points: %d out of %d",ONLINE->HULL,PLAYER.Hull);
				OUT(outbuf);NL;
				sprintf(outbuf,"Cannons: %d",PLAYER.Cannon);
				OUT(outbuf);NL;
				sprintf(outbuf,"Ram: %s",(PLAYER.Ram=='Y') ? "Yes" : "No");
				OUT(outbuf);NL;
				break;
			case 'L':
				cls();
				sprintf(outbuf,"%s%s ID              Username             Hull      Cannons      Ram%s",fore(WHITE),back(BLU),back(BLK));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s----      ----------------------      ----      -------      ---%s",back(BLU),back(BLK));
				OUT(outbuf);NL;
				NORMAL;
				lp=2;
				c='Y';
				strcpy(userlevel.Current,"999");
				while(RPGserver(SERVER_GETUSERLEVEL,(UBYTE *)&userlevel) && atoi(userlevel.Current)>1) {
					strcpy(ENEMY.ID,userlevel.ID);
					RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
					if(ENEMY.Hull) {
						if(++lp%rows==0)
							if((c=more())=='N')
								break;
						sprintf(outbuf,"%-4.4s      %-22.22s      %4u      %5u         %c",ENEMY.ID,ENEMY.Handle,ENEMY.Hull,ENEMY.Cannon,ENEMY.Ram);
						OUT(outbuf);NL;
					}
				}
				if(c!='N' && !PLAYER.Expert)
					paws=TRUE;
				break;
		}
	}
}
