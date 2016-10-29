#include "/include/all.h"
#include "global.h"

void Lists(void)
{
	static struct RPGmenu __far RPGmenu = { "Lists",GRN,5,
							{	{ 'F',"Hall of Fame" },
								{ 'H',"Hi-Scores" },
								{ 'L',"Hall of Lame" },
								{ 'M',"Most Memorable Hits" },
								{ 'W',"Winners List" }
							}
	};

	double topv[21];
	int i,j,lp,o,t;
	char topid[21][5];
	char c;

	while((c=option(&RPGmenu))!='Q') {
		switch(c) {
			case 'F':
				while(timeleft() && !cd()) {
					for(o=0; o<NUMCLASS; o++) {
						sprintf(outbuf,"%s%s %s",bracket(o+1),fore(GRY),ServerTable->class[o]->Origin);
						OUT(outbuf);NL;
					}
					sprintf(prompt,"%sSelect origin: ",fore(CYN));
					NL;OUT(prompt);
					if(ins(2))
						break;
					NL;NL;
					o=atoi(inbuf);
					if(o<1 || o>NUMCLASS)
						break;
					o--;
					for(t=0; t<ServerTable->class[o]->NumCharacter; t++) {
						if(o==0 && t==0)
							sprintf(outbuf,"%s%s All-Time",bracket(t+1),fore(GRY));
						else
							sprintf(outbuf,"%s%s %s",bracket(t+1),fore(GRY),ServerTable->class[o]->Character[t]->Name);
						OUT(outbuf);NL;
					}
					sprintf(prompt,"%sSelect class: ",fore(CYN));
					NL;OUT(prompt);
					if(ins(2))
						break;
					NL;NL;
					t=atoi(inbuf);
					if(t<1 || t>ServerTable->class[o]->NumCharacter)
						break;
					t--;
					cls();
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m     Hall of Fame     \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					if(o==0 && t==0)
						strcpy(line[0],"All-Time");
					else
						sprintf(line[0],"%s - %s",ServerTable->class[o]->Origin,ServerTable->class[o]->Character[t]->Name);
					sprintf(line[1],"%*s%s",(22-strlen(line[0]))/2,"",line[0]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(RED),line[1],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  Score  \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(RED),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m     Least Calls      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Calls[0].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Calls[0].Who))/2,"",ServerTable->class[o]->Character[t]->Calls[0].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Calls[0].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m     Least Kills      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Kills[0].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Kills[0].Who))/2,"",ServerTable->class[o]->Character[t]->Kills[0].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Kills[0].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m     Least Killed     \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Killed[0].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Killed[0].Who))/2,"",ServerTable->class[o]->Character[t]->Killed[0].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Killed[0].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m    Least Retreats    \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Retreats[0].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Retreats[0].Who))/2,"",ServerTable->class[o]->Character[t]->Retreats[0].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Retreats[0].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m   Most Knock-Outs    \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Brawls[0].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Brawls[0].Who))/2,"",ServerTable->class[o]->Character[t]->Brawls[0].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Brawls[0].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m   Most Joust Wins    \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Jousts[0].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Jousts[0].Who))/2,"",ServerTable->class[o]->Character[t]->Jousts[0].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Jousts[0].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					pause();
				}
				break;
			case 'H':
				sprintf(outbuf,"         %s-------------",fore(BLU));
				OUT(outbuf);NL;
				sprintf(outbuf,"  --=:))%s Hi-Score List %s((:=--",fore(YELLOW),fore(BLU));
				OUT(outbuf);NL;
				sprintf(outbuf,"         -------------%s                                      Your Score",fore(YELLOW));
				OUT(outbuf);NORMAL;NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[0]))/2,"",ServerTable->HiScore->Who[0]);
				sprintf(outbuf,"%s%s    The One to Beat%s    %-22s %9.2f %9.2f %srating",bracket(1),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Rating,PLAYER.Rating,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[1]))/2,"",ServerTable->HiScore->Who[1]);
				sprintf(outbuf,"%s%s  The Sultan of Slash%s  %-22s %9.9g %9u %shits",bracket(2),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->HP,PLAYER.History.HP,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[2]))/2,"",ServerTable->HiScore->Who[2]);
				sprintf(outbuf,"%s%s The Admirable Admiral%s %-22s %9.9g %9u %shulls",bracket(3),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Hull,PLAYER.History.Hull,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[3]))/2,"",ServerTable->HiScore->Who[3]);
				sprintf(outbuf,"%s%s   The Serial Killer%s   %-22s %9.2f %9.2f %skills",bracket(4),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Kills,(double)PLAYER.History.Kills/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[4]))/2,"",ServerTable->HiScore->Who[4]);
				sprintf(outbuf,"%s%s  Most Popular Player%s  %-22s %9.2f %9.2f %scalls/day",bracket(5),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Calls,(double)PLAYER.History.Calls/(Julian-PLAYER.CreateDate+1),fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[5]))/2,"",ServerTable->HiScore->Who[5]);
				sprintf(outbuf,"%s%s  The Easiest Target%s   %-22s %9.2f %9.2f %skilled",bracket(6),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Killed,(double)PLAYER.History.Killed/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[6]))/2,"",ServerTable->HiScore->Who[6]);
				sprintf(outbuf,"%s%s   The Runaway Train%s   %-22s %9.2f %9.2f %sretreats",bracket(7),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Retreats,(double)PLAYER.History.Retreats/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[7]))/2,"",ServerTable->HiScore->Who[7]);
				sprintf(outbuf,"%s%s  The Knock-out King%s   %-22s %9.2f %9.2f %sbrawls",bracket(8),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->KO,(double)PLAYER.History.KO/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[8]))/2,"",ServerTable->HiScore->Who[8]);
				sprintf(outbuf,"%s%s  The Joust Champion%s   %-22s %9.2f %9.2f %sjousts",bracket(9),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Joust,(double)PLAYER.History.Joust/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[9]))/2,"",ServerTable->HiScore->Who[9]);
				sprintf(outbuf,"%s%s The Prince of Thieves%s %-22s %9.2f %9.2f %ssteals",bracket(10),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Stole,(double)PLAYER.History.Stole/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[10]))/2,"",ServerTable->HiScore->Who[10]);
				sprintf(outbuf,"%s%s  Ace of High Rollers%s  %-22s %9.2f %9.2f %sgambles",bracket(11),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Gamble,(double)PLAYER.History.Gamble/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;
				sprintf(line[0],"%*s%s",(22-strlen(ServerTable->HiScore->Who[11]))/2,"",ServerTable->HiScore->Who[11]);
				sprintf(outbuf,"%s%s I'd Rather Be Fishing%s %-22s %9.2f %9.2f %sfishes",bracket(12),fore(CYN),fore(WHITE),line[0],ServerTable->HiScore->Fish,(double)PLAYER.History.Fish/PLAYER.History.Calls,fore(CYN));
				OUT(outbuf);NL;NL;
				sprintf(prompt,"%sPick a Top 20 list: ",fore(CYN));
				OUT(prompt);
				if(ins(2))
					break;
				NL;
				o=atoi(inbuf);
				if(o>0 && o<13) {
					t=0;
					memset(&RPC[1][1]->user,0,sizeof(struct user));
					for(i=0; RPGserver(SERVER_NEXTUSER,(UBYTE *)&RPC[1][1]->user); i++) {
						sprintf(outbuf,"%c\10",spin[i%4]);
						OUT(outbuf);
						if(*RPC[1][1]->user.ID=='_' || RPC[1][1]->user.History.Calls<10)
							continue;
						switch(o) {
							case 1:
								topv[t]=RPC[1][1]->user.Rating;
								break;
							case 2:
								topv[t]=RPC[1][1]->user.History.HP;
								break;
							case 3:
								topv[t]=RPC[1][1]->user.History.Hull;
								break;
							case 4:
								topv[t]=(double)RPC[1][1]->user.History.Kills/RPC[1][1]->user.History.Calls;
								break;
							case 5:
								topv[t]=(double)RPC[1][1]->user.History.Calls/(Julian-RPC[1][1]->user.CreateDate+1);
								break;
							case 6:
								topv[t]=(double)RPC[1][1]->user.History.Killed/RPC[1][1]->user.History.Calls;
								break;
							case 7:
								topv[t]=(double)RPC[1][1]->user.History.Retreats/RPC[1][1]->user.History.Calls;
								break;
							case 8:
								topv[t]=(double)RPC[1][1]->user.History.KO/RPC[1][1]->user.History.Calls;
								break;
							case 9:
								topv[t]=(double)RPC[1][1]->user.History.Joust/RPC[1][1]->user.History.Calls;
								break;
							case 10:
								topv[t]=(double)RPC[1][1]->user.History.Stole/RPC[1][1]->user.History.Calls;
								break;
							case 11:
								topv[t]=(double)RPC[1][1]->user.History.Gamble/RPC[1][1]->user.History.Calls;
								break;
							case 12:
								topv[t]=(double)RPC[1][1]->user.History.Fish/RPC[1][1]->user.History.Calls;
								break;
						}
						if(topv[t]<.005)
							continue;
						strcpy(topid[t],RPC[1][1]->user.ID);
						j=t;
						if(t<20)
							t++;
						while(j>0 && topv[j]>topv[j-1]) {
							strcpy(outbuf,topid[j-1]);
							strcpy(topid[j-1],topid[j]);
							strcpy(topid[j],outbuf);
							d=topv[j-1];
							topv[j-1]=topv[j];
							topv[j]=d;
							j--;
						}
					}
					cls();
					sprintf(outbuf,"    %s%s   Top 20 %s   ",back(BLU),fore(WHITE),
							o==1 ? "Rated" :
							o==2 ? "Hitters" :
							o==3 ? "Gunners" :
							o==4 ? "Killers" :
							o==5 ? "Callers" :
							o==6 ? "Targets" :
							o==7 ? "Cowards" :
							o==8 ? "Fists" :
							o==9 ? "Jousters" :
							o==10 ? "Thieves" :
							o==11 ? "Gamblers" :
							o==12 ? "Fishers" :
							"?"
							);
					OUT(outbuf);NORMAL;NL;NL;
					for(i=0; i<t; i++) {
						strcpy(RPC[1][1]->user.ID,topid[i]);
						RPGserver(SERVER_GETUSER,(UBYTE *)&RPC[1][1]->user);
						sprintf(outbuf,"%s%2d.%s  %-22s  ",fore(CYN),i+1,fore(WHITE),RPC[1][1]->user.Handle);
						OUT(outbuf);
						sprintf(outbuf,"%s",fore(i==0 ? YELLOW : i==1 ? CYAN : i==2 ? BRED : GRY));
						OUT(outbuf);
						if(o==2 || o==3)
							sprintf(outbuf,"%9.9g",topv[i]);
						else
							sprintf(outbuf,"%9.2f",topv[i]);
						OUT(outbuf);NL;
					}
					paws=!PLAYER.Expert;
				}
				break;
			case 'L':
				while(timeleft() && !cd()) {
					for(o=0; o<NUMCLASS; o++) {
						sprintf(outbuf,"%s%s %s",bracket(o+1),fore(GRY),ServerTable->class[o]->Origin);
						OUT(outbuf);NL;
					}
					sprintf(prompt,"%sSelect origin: ",fore(CYN));
					NL;OUT(prompt);
					if(ins(2))
						break;
					NL;NL;
					o=atoi(inbuf);
					if(o<1 || o>NUMCLASS)
						break;
					o--;
					for(t=0; t<ServerTable->class[o]->NumCharacter; t++) {
						if(o==0 && t==0)
							sprintf(outbuf,"%s%s All-Time",bracket(t+1),fore(GRY));
						else
							sprintf(outbuf,"%s%s %s",bracket(t+1),fore(GRY),ServerTable->class[o]->Character[t]->Name);
						OUT(outbuf);NL;
					}
					sprintf(prompt,"%sSelect class: ",fore(CYN));
					NL;OUT(prompt);
					if(ins(2))
						break;
					NL;NL;
					t=atoi(inbuf);
					if(t<1 || t>ServerTable->class[o]->NumCharacter)
						break;
					t--;
					cls();
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m     Hall of Lame     \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					if(o==0 && t==0)
						strcpy(line[0],"All-Time");
					else
						sprintf(line[0],"%s - %s",ServerTable->class[o]->Origin,ServerTable->class[o]->Character[t]->Name);
					sprintf(line[1],"%*s%s",(22-strlen(line[0]))/2,"",line[0]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(RED),line[1],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  Score  \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(RED),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m      Most Calls      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Calls[1].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Calls[1].Who))/2,"",ServerTable->class[o]->Character[t]->Calls[1].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Calls[1].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m      Most Kills      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Kills[1].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Kills[1].Who))/2,"",ServerTable->class[o]->Character[t]->Kills[1].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Kills[1].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m     Most Killed      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Killed[1].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Killed[1].Who))/2,"",ServerTable->class[o]->Character[t]->Killed[1].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Killed[1].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m    Most Retreats     \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Retreats[1].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Retreats[1].Who))/2,"",ServerTable->class[o]->Character[t]->Retreats[1].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Retreats[1].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m   Most Knocked-Out   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Brawls[1].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Brawls[1].Who))/2,"",ServerTable->class[o]->Character[t]->Brawls[1].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Brawls[1].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m                      \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m         \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  Most Joust Losses   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					fdate(dated,ServerTable->class[o]->Character[t]->Jousts[1].When);
					sprintf(line[0],"%.7s%s",&dated[4],&dated[13]);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLK),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					sprintf(line[0],"%*s%s",(22-strlen(ServerTable->class[o]->Character[t]->Jousts[1].Who))/2,"",ServerTable->class[o]->Character[t]->Jousts[1].Who);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m%-22s\33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);
					sprintf(line[0],"%4u",ServerTable->class[o]->Character[t]->Jousts[1].What);
					sprintf(outbuf," %s∞±≤€%s≤±∞ %s∞±≤%s\33[7m  %s   \33[27m%s≤±∞%s ∞±≤%s€≤±∞",fore(BLACK),back(GRY),fore(WHITE),back(BLU),line[0],back(GRY),fore(BLACK),back(BLK));
					OUT(outbuf);NL;
					pause();
				}
				break;
			case 'M':
				sprintf(outbuf,"%s  Class    Player                     Date     Melee/Blast",fore(CYN));
				OUT(outbuf);NL;
				OUT("----------------------------------------------------------");NL;
				c='Y';lp=2;
				for(o=0; o<NUMCLASS && c!='N'; o++) {
					for(t=0; t<MAXCLASS(o); t++) {
						lp+=3;
						if(lp>=rows) {
							lp=3;
							if((c=more())=='N')
								break;
						}
						if(o || t)
							sprintf(outbuf,"%s%-9s",fore(GRY),ServerTable->class[o]->Character[t]->Name);
						else
							sprintf(outbuf,"%sAll-Time ",fore(GRY));
						OUT(outbuf);
						fdate(dated,ServerTable->class[o]->Character[t]->melee.When);
						sprintf(outbuf,"  %s%-22s  %s   %5u",fore(WHITE),ServerTable->class[o]->Character[t]->melee.Who,&dated[4],ServerTable->class[o]->Character[t]->melee.What);
						OUT(outbuf);NL;
						if(ServerTable->class[o]->Character[t]->blast.What) {
							fdate(dated,ServerTable->class[o]->Character[t]->blast.When);
							sprintf(outbuf,"           %-22s  %s   %5u",ServerTable->class[o]->Character[t]->blast.Who,&dated[4],ServerTable->class[o]->Character[t]->blast.What);
							OUT(outbuf);NL;
						}
						NL;
					}
				}
				if(c=='Y')
					paws=!PLAYER.Expert;
				break;
			case 'W':
				sprintf(outbuf,"       %s-------------",fore(BLU));
				OUT(outbuf);NL;
				sprintf(outbuf,"--=:))%s Winner's List %s((:=--",fore(YELLOW),fore(BLU));
				OUT(outbuf);NL;
				sprintf(outbuf,"       -------------");
				OUT(outbuf);NORMAL;NL;
				type("Notices/Winners",TRUE);
				paws=!PLAYER.Expert;
				break;
		}
	}
}
