#include "/include/all.h"
#include "global.h"

#define LEVEL(l)		(deep_dank_dungeon+l)
#define ROOM(l,x,y)		(((deep_dank_dungeon+l)->room)+(y)*(deep_dank_dungeon+l)->MaxCol+x)
#define	MAP				1
#define	MAGIC_MAP		2
#define	GIFT_VIAL		1
#define	GIFT_POISON		2
#define	GIFT_MAGIC		3
#define	GIFT_XMAGIC		4
#define	GIFT_CHEST		5
#define	TRAP			1
#define	WELL			2
#define WHEEL			3
#define	THIEF			4
#define	CLERIC			5
#define	WIZARD			6

struct room {
	UBYTE type:2;			//	0=Emp, 1=N-S, 2=W-E, 3=Cav
	UBYTE occupant:3;		//	0=none, 1=trap, 2=well, 3=wheel, 4=thief, 5=cleric, 6=wizard
	UBYTE gift_type:3;		//	0=none, 1=vial, 2=poison, 3=wand/scroll, 4=extra spell, 5=chest
	UBYTE gift_value:4;		//	1-16
	UBYTE map:1;			//	1=explored
	UBYTE level[3];			//	0=none, 1-99=monster level
	UBYTE monster[3];		//	monster name pointer (0-99)
};
struct deep_dank_dungeon {
	UBYTE Map;
	UBYTE MaxRow,MaxCol;
	struct room *room;
} *deep_dank_dungeon;

int dl,escape,hx,hy,fx,fy,move,moves;
UBYTE dmap[21][80];

void chkroom(int x,int y)
{
	ROOM(dl,x,y)->map=FALSE;
	if(ROOM(dl,x+1,y)->map && x+1<LEVEL(dl)->MaxCol && ROOM(dl,x,y)->type!=1 && ROOM(dl,x+1,y)->type!=1)
		chkroom(x+1,y);
	if(ROOM(dl,x,y+1)->map && y+1<LEVEL(dl)->MaxRow && ROOM(dl,x,y)->type!=2 && ROOM(dl,x,y+1)->type!=2)
		chkroom(x,y+1);
	if(ROOM(dl,x-1,y)->map && x-1>=0 && ROOM(dl,x,y)->type!=1 && ROOM(dl,x-1,y)->type!=1)
		chkroom(x-1,y);
	if(ROOM(dl,x,y-1)->map && y-1>=0 && ROOM(dl,x,y)->type!=2 && ROOM(dl,x,y-1)->type!=2)
		chkroom(x,y-1);
}

int chklevel(void)
{
	int x,y;
	int flag=TRUE;

	chkroom(0,0);
	for(y=0; y<LEVEL(dl)->MaxRow; y++)
		for(x=0; x<LEVEL(dl)->MaxCol; x++)
			if(ROOM(dl,x,y)->map)
				flag=FALSE;
	return(flag);
}

void clrmap(void)
{
	sprintf(outbuf,"\33[1;%ur\33[J",rows);
	OUT(outbuf);
	move=TRUE;
}

void drawroom(int x,int y)
{
	UBYTE dot,*mon;

	if(ANSI==1) {
		OUT("\16");
		dot='~';
	}
	else
		dot='ú';

	bold=FALSE;
	if(!LEVEL(dl)->Map) {
		plot(&SERCON,y*2+1,x*6+1);
		sprintf(outbuf,"%s%.7s",fore(BLACK),&dmap[y*2][6*x]);
		OUT(outbuf);
	}

	plot(&SERCON,y*2+2,x*6+1);
	sprintf(outbuf,"%s%c",fore(BLACK),dmap[y*2+1][6*x]);
	OUT(outbuf);
	if(ROOM(dl,x,y)->map)
		sprintf(outbuf,"%s  %c  ",fore(WHITE),dot);
	else
		strcpy(outbuf,"     ");
	if(ROOM(dl,x,y)->map || LEVEL(dl)->Map==MAGIC_MAP) {
		bold=FALSE;
		mon=NULL;
		if(ROOM(dl,x,y)->level[0]) {
			mon=ROOM(dl,x,y)->level[1] ? "Mob" : "Mon";
			sprintf(outbuf,"%s%s %s %s",(ANSI==1 ? "\17" : ""),fore(ROOM(dl,x,y)->occupant ? GREEN : RED),mon,(ANSI==1 ? "\16" : ""));
		}
		bold=TRUE;
		switch(ROOM(dl,x,y)->occupant) {
			case 0:
				break;
			case CLERIC:
				if(!mon)
					mon="Clr";
				sprintf(outbuf,"%s%s(%s%s%s)%s",(ANSI==1 ? "\17" : ""),fore(BRN),fore(YELLOW),mon,fore(BRN),(ANSI==1 ? "\16" : ""));
				break;
			case WIZARD:
				if(!mon)
					mon="Wiz";
				sprintf(outbuf,"%s%s(%s%s%s)%s",(ANSI==1 ? "\17" : ""),fore(MAG),fore(MAGENTA),mon,fore(MAG),(ANSI==1 ? "\16" : ""));
				break;
			default:
				if(LEVEL(dl)->Map==MAGIC_MAP)
					sprintf(outbuf,"%s  \33[5m?\33[m  ",fore(CYAN));	// term doesn't support "\33[25m"
				break;
		}
		bold=FALSE;
	}
	OUT(outbuf);
	sprintf(outbuf,"%s%c",fore(BLACK),dmap[y*2+1][6*(x+1)]);
	OUT(outbuf);

	if(!LEVEL(dl)->Map) {
		plot(&SERCON,y*2+3,x*6+1);
		sprintf(outbuf,"%s%.7s",fore(BLACK),&dmap[y*2+2][6*x]);
		OUT(outbuf);
	}
	if(ANSI==1)
		OUT("\17");
}

void drawyou(void)
{
	plot(&SERCON,hy*2+2,hx*6+2);
	REVERSE;
	sprintf(outbuf,"%s-YOU-",fore(WHITE));
	OUT(outbuf);
	NORMAL;
	plot(&SERCON,rows,1);
}

void drawmap(void)
{
	int r,c;
	UBYTE dot,*mon;

	clrmap();
	if(ANSI==1) {
		OUT("\16");
		dot='~';
	}
	else
		dot='ú';

	if(LEVEL(dl)->Map) {
		for(r=0; r<LEVEL(dl)->MaxRow*2+1; r++) {
			if(r%2) {
				for(c=0; c<LEVEL(dl)->MaxCol; c++) {
					sprintf(outbuf,"%s%c",fore(BLACK),dmap[r][6*c]);
					OUT(outbuf);
					if(ROOM(dl,c,r/2)->map)
						sprintf(outbuf,"%s  %c  ",fore(WHITE),dot);
					else
						strcpy(outbuf,"     ");
					if(ROOM(dl,c,r/2)->map || LEVEL(dl)->Map==MAGIC_MAP) {
						bold=FALSE;
						mon=NULL;
						if(ROOM(dl,c,r/2)->level[0]) {
							mon=ROOM(dl,c,r/2)->level[1] ? "Mob" : "Mon";
							sprintf(outbuf,"%s%s %s %s",(ANSI==1 ? "\17" : ""),fore(ROOM(dl,c,r/2)->occupant ? GREEN : RED),mon,(ANSI==1 ? "\16" : ""));
						}
						bold=TRUE;
						switch(ROOM(dl,c,r/2)->occupant) {
							case 0:
								break;
							case CLERIC:
								if(!mon)
									mon="Clr";
								sprintf(outbuf,"%s%s(%s%s%s)%s",(ANSI==1 ? "\17" : ""),fore(BRN),fore(YELLOW),mon,fore(BRN),(ANSI==1 ? "\16" : ""));
								break;
							case WIZARD:
								if(!mon)
									mon="Wiz";
								sprintf(outbuf,"%s%s(%s%s%s)%s",(ANSI==1 ? "\17" : ""),fore(MAG),fore(MAGENTA),mon,fore(MAG),(ANSI==1 ? "\16" : ""));
								break;
							default:
								if(LEVEL(dl)->Map==MAGIC_MAP)
									sprintf(outbuf,"%s  \33[5m?\33[m  ",fore(CYAN));	// term doesn't support "\33[25m"
								break;
						}
						bold=FALSE;
					}
					OUT(outbuf);
				}
				bold=FALSE;
				sprintf(outbuf,"%s%c",fore(BLACK),dmap[r][6*LEVEL(dl)->MaxCol]);
				OUT(outbuf);
			}
			else {
				OUT(fore(BLACK));
				OUT(&dmap[r][0]);
			}
			CLL;NL;
		}
	}
	else
		for(r=0; r<LEVEL(dl)->MaxRow; r++)
			for(c=0; c<LEVEL(dl)->MaxCol; c++)
				if(ROOM(dl,c,r)->map)
					drawroom(c,r);

	if(ANSI==1)
		OUT("\17");
	sprintf(outbuf,"\33[%u;%ur",2*(LEVEL(dl)->MaxRow+1),rows);
	OUT(outbuf);
}

void renderdmap(void)
{
	int b,c,r,x,y;

	b=(ANSI==1) ? 0 : 1;
	memset(dmap,0,sizeof(dmap));
	memset(&dmap[0][1],box[b][0],6*LEVEL(dl)->MaxCol-1);
	for(y=1; y<2*LEVEL(dl)->MaxRow; y++)
		sprintf(&dmap[y][0],"%c%*s%c",box[b][10],6*LEVEL(dl)->MaxCol-1,"",box[b][10]);
	memset(&dmap[2*LEVEL(dl)->MaxRow][1],box[b][0],6*LEVEL(dl)->MaxCol-1);

	for(y=0; y<LEVEL(dl)->MaxRow; y++) {
		for(x=0; x<LEVEL(dl)->MaxCol; x++) {
			if(ROOM(dl,x,y)->type==1) {
				r=2*y;c=6*x;
				if(dmap[r][c]==' ')
					dmap[r][c]=box[b][10];
				else
				if(dmap[r][c]==box[b][3])
					dmap[r][c]=box[b][6];
				else
				if(dmap[r][c]==box[b][2])
					dmap[r][c]=box[b][5];
				else
				if(dmap[r][c]==box[b][1])
					dmap[r][c]=box[b][4];
				else
				if(dmap[r][c]==box[b][0])
					dmap[r][c]=box[b][c>0 && dmap[r][c-1]==' ' ? 7 : dmap[r][c+1]==' ' ? 9 : 8];

				dmap[r+1][c]=box[b][10];

				if(dmap[r+2][c]==' ')
					dmap[r+2][c]=box[b][10];
				else
				if(dmap[r+2][c]==box[b][0])
					dmap[r+2][c]=box[b][c>0 && dmap[r+2][c-1]==' ' ? 1 : dmap[r+2][c+1]==' ' ? 3 : 2];

				c=6*(x+1);
				if(dmap[r][c]==' ')
					dmap[r][c]=box[b][10];
				else
				if(dmap[r][c]==box[b][0])
					dmap[r][c]=box[b][dmap[r][c-1]==' ' ? 7 : dmap[r][c+1]==' ' ? 9 : 8];
				else
				if(dmap[r][c]==box[b][1])
					dmap[r][c]=box[b][4];
				else
				if(dmap[r][c]==box[b][2])
					dmap[r][c]=box[b][5];
				else
				if(dmap[r][c]==box[b][3])
					dmap[r][c]=box[b][6];

				dmap[r+1][c]=box[b][10];
				dmap[r+2][c]=box[b][r+2<2*LEVEL(dl)->MaxRow ? 10 : 2];
			}
			if(ROOM(dl,x,y)->type==2) {
				r=2*y;c=6*x;
				if(dmap[r][c]==' ')
					dmap[r][c]=box[b][0];
				else
				if(dmap[r][c]==box[b][3])
					dmap[r][c]=box[b][2];
				else
				if(dmap[r][c]==box[b][6])
					dmap[r][c]=box[b][5];
				else
				if(dmap[r][c]==box[b][9])
					dmap[r][c]=box[b][8];
				else
				if(dmap[r][c]==box[b][10])
					dmap[r][c]=box[b][r>0 && dmap[r-1][c]==' ' ? 7 : dmap[r+1][c]==' ' ? 1 : 4];

				memset(&dmap[r][c+1],box[b][0],5);

				if(dmap[r][c+6]==' ')
					dmap[r][c+6]=box[b][0];
				else
				if(dmap[r][c+6]==box[b][1])
					dmap[r][c+6]=box[b][2];
				else
				if(dmap[r][c+6]==box[b][10])
					dmap[r][c+6]=box[b][dmap[r+1][c+6]==box[b][10] ? 6 : 3];

				if(dmap[r+2][c]==box[b][10])
					dmap[r+2][c]=box[b][c>0 && dmap[r+2][c-1]==' ' ? 1 : 4];
				else
				if(dmap[r+2][c]==' ')
					dmap[r+2][c]=box[b][0];

				memset(&dmap[r+2][c+1],box[b][0],5);

				if(dmap[r+2][c+6]==' ')
					dmap[r+2][c+6]=box[b][0];
				else
				if(dmap[r+2][c+6]==box[b][10])
					dmap[r+2][c+6]=box[b][dmap[r+3][c+6]==box[b][10] ? 6 : 3];
			}
		}
	}
	dmap[0][0]=box[b][7];
	sprintf(&dmap[0][6*LEVEL(dl)->MaxCol],"%c",box[b][9]);
	dmap[2*LEVEL(dl)->MaxRow][0]=box[b][1];
	sprintf(&dmap[2*LEVEL(dl)->MaxRow][6*LEVEL(dl)->MaxCol],"%c",box[b][3]);
	moves=0;
}

int putmonster(int x,int y)
{
	int i,j,l;

	for(i=0; i<3 && ROOM(dl,x,y)->level[i]; i++);
	if(i==3)
		return(FALSE);
	for(j=0,l=0; j<4; j++,l+=dice(7));
	l/=j;
	switch(l) {
		case 1:
			l=dice(dl);
			break;
		case 2:
			l=dl-3-dice(3);
			break;
		case 3:
			l=dl-dice(3);
			break;
		case 4:
			l=dl;
			break;
		case 5:
			l=dl+dice(3);
			break;
		case 6:
			l=dl+3+dice(3);
			break;
		case 7:
			l=dl+dice(100-dl);
			break;
	}
	l=(l<1) ? 1 : (l>99) ? 99 : l;
	l=(i==1) ? l/2+dice(l/2+1) : (i==2) ? dice(l+1) : l;
	l=(l<1) ? 1 : (l>99) ? 99 : l;
	ROOM(dl,x,y)->level[i]=l;
	j=l*NUMDUNGEON/100+dice(3)-2;
	j=(j<0) ? 0 : (j>=NUMDUNGEON) ? NUMDUNGEON-1 : j;
	ROOM(dl,x,y)->monster[i]=j;
	return(TRUE);
}

void teleport(void)
{
	char c;

	OUT(fore(YELLOW));
	OUT("What do you wish to do?");NL;NL;
	NORMAL;
	OUT("<U> Teleport up 1 level");NL;
	if(dl<99) {
		OUT("<D> Teleport down 1 level");NL;
	}
	OUT("<O> Teleport out of the dungeon");NL;
	OUT("<R> Random teleport");NL;NL;
	sprintf(outbuf,"%sTime Left:%s %d min.",fore(MAG),fore(GRY),timeleft());
	OUT(outbuf);NL;
	do {
		NL;
		sprintf(prompt,"%sTeleport: ",fore(CYN));
		OUT(prompt);
		c=inkey(NULL,'O');
	} while(!strchr("UDOR",c));
	NL;NL;
	PausePlaySeq();
	sound("teleport",64);
	if(c=='U') {
		if(dl>0)
			dl--;
		if(LEVEL(dl)->room) {
			renderdmap();
			c='R';
		}
	}
	if(c=='D') {
		if(dl<99)
			dl++;
		if(LEVEL(dl)->room) {
			renderdmap();
			c='R';
		}
	}
	if(c=='O')
		escape=TRUE;
	if(c=='R') {
		hx=dice(LEVEL(dl)->MaxCol)-1;hy=dice(LEVEL(dl)->MaxRow)-1;
		fx=hx;fy=hy;
		move=TRUE;
	}
}

void GenLevel(void)
{
	int i,j,m,n,x,y;

	LEVEL(dl)->MaxRow=6+dice(dl/32+1);	//	7-10 high
	LEVEL(dl)->MaxCol=6+dice(dl/16+1);	//	7-13 wide
	LEVEL(dl)->room=calloc(LEVEL(dl)->MaxRow*LEVEL(dl)->MaxCol,sizeof(struct room));
	do {
		for(i=0; i<LEVEL(dl)->MaxRow; i++)
			for(j=0; j<LEVEL(dl)->MaxCol; j++) {
				while((n=(dice(4)+dice(4))/2-1)==3);
				n=(n==0) ? 3 : (n==1) ? 0 : dice(2);
				ROOM(dl,j,i)->type=n;
				ROOM(dl,j,i)->map=TRUE;
			}
	} while(!chklevel());
	m=LEVEL(dl)->MaxRow*LEVEL(dl)->MaxCol/4+dice(dl/10+1);
	for(n=0; n<m; n++) {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		putmonster(x,y);
	}
	m=LEVEL(dl)->MaxRow*LEVEL(dl)->MaxCol/10+(dl<99 ? dice(dl/16+2) : 0);
	for(n=0; n<m; n++) {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		ROOM(dl,x,y)->occupant=TRAP;
	}
	if(dice((100-dl)/3+3)==1 && PLAYER.Novice!='Y') {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		ROOM(dl,x,y)->occupant=WELL;
	}
	if(dice((100-dl)/3+3)==1) {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		ROOM(dl,x,y)->occupant=WHEEL;
	}
	x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
	ROOM(dl,x,y)->occupant=THIEF;
	do {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
	} while(ROOM(dl,x,y)->occupant);
	ROOM(dl,x,y)->occupant=CLERIC;
	do {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
	} while(ROOM(dl,x,y)->occupant);
	ROOM(dl,x,y)->occupant=WIZARD;
	m=dice(dl/33+1);
	for(n=0; n<m; n++) {
		x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		ROOM(dl,x,y)->gift_type=GIFT_VIAL;
		j=dice(136);
		for(i=0; i<16 && j>0; i++)
			j-=i+1;
		ROOM(dl,x,y)->gift_value=16-i;
	}
	while(CLASS(ONLINE)->Poison && dice(10-CLASS(ONLINE)->Poison)==1) {
		do {
			x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		} while(!ROOM(dl,x,y)->gift_type);
		ROOM(dl,x,y)->gift_type=GIFT_POISON;
		i=dice(dl)-1;j=dice(136-i);
		for(i=0; i<16 && j>0; i++)
			j-=i+1;
		ROOM(dl,x,y)->gift_value=16-i;
	}
	while(CLASS(ONLINE)->Magic==1 && dice(13-dl/10)==1) {
		do {
			x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		} while(!ROOM(dl,x,y)->gift_type);
		ROOM(dl,x,y)->gift_type=GIFT_MAGIC;
		i=dice(dl)-1;j=dice(136-i);
		for(i=0; i<16 && j>0; i++)
			j-=i+1;
		ROOM(dl,x,y)->gift_value=16-i;
	}
	while(CLASS(ONLINE)->Magic==2 && dice(12-dl/10)==1) {
		do {
			x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		} while(!ROOM(dl,x,y)->gift_type);
		ROOM(dl,x,y)->gift_type=GIFT_MAGIC;
		i=dice(dl)-1;j=dice(136-i);
		for(i=0; i<16 && j>0; i++)
			j-=i+1;
		ROOM(dl,x,y)->gift_value=16-i;
	}
	while((CLASS(ONLINE)->Magic==1 || CLASS(ONLINE)->Magic==2) && dice(dl/3+3)==1) {
		do {
			x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		} while(!ROOM(dl,x,y)->gift_type);
		ROOM(dl,x,y)->gift_type=GIFT_XMAGIC;
		j=dice(15);
		for(i=0; i<5 && j>0; i++)
			j-=i+1;
		ROOM(dl,x,y)->gift_value=5-i;
	}
	if(dice(dl/3+3)==1) {
		do {
			x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
		} while(!ROOM(dl,x,y)->gift_type);
		ROOM(dl,x,y)->gift_type=GIFT_CHEST;
		ROOM(dl,x,y)->gift_value=dice(16)-1;
	}
	renderdmap();
	drawmap();
	hx=dice(LEVEL(dl)->MaxCol)-1;hy=dice(LEVEL(dl)->MaxRow)-1;
	fx=hx;fy=hy;
	move=TRUE;
	sprintf(outbuf,"is entering dungeon level %u",dl);
	broadcast(outbuf);
}

void ExamineRoom(void)
{
	static char *wol[] = { " Grace "," Doom! ","Fortune"," Taxes "," Power "," Death " };
	double g;
	UWORD t;
	int i,j,m,n;
	char c,*s,*v;

	if(ROOM(dl,hx,hy)->level[0]) {
		if(local) {
			FadeGlobalVolume(8);
			SetChannelEnable(2+8);
		}
		clrmap();
		OUT("There is something lurking in this chamber...");
		Delay(25);
		for(i=1; i<5; i++)
			if(RPC[0][i]->HP<1)
				memset(RPC[0][i],0,sizeof(struct RPC));
		for(i=0; i<5; i++)
			memset(RPC[1][i],0,sizeof(struct RPC));
		for(i=0,m=0; i<3 && ROOM(dl,hx,hy)->level[i]; i++) {
			strcpy(RPC[1][m]->user.Handle,DUNGEON(ROOM(dl,hx,hy)->monster[m])->Name);
			strcpy(RPC[1][m]->user.Class,DUNGEON(ROOM(dl,hx,hy)->monster[m])->Class);
			RPC[1][m]->user.Spell=DUNGEON(ROOM(dl,hx,hy)->monster[m])->Spell;
			RPC[1][m]->user.XSpell=DUNGEON(ROOM(dl,hx,hy)->monster[m])->XSpell;
			RPC[1][m]->user.Level=ROOM(dl,hx,hy)->level[m];
			RPC[1][m]->user.Gender='I';

			g=1.;

			n=MAXWEAPON(0)*dl/100+dice(3)-2;
			if(ONLINE->weapon_origin==0)
				n=(n+ONLINE->weapon_type)/2;
			n=(n<1) ? 1 : (n>=MAXWEAPON(0)) ? MAXWEAPON(0)-1 : n;
			if(dice(PLAYER.Level/4+12-ONLINE->CHA/10)==1) {
				j=0;
				if(ONLINE->weapon_origin==0) {
					n=ONLINE->weapon_type+dice(3)-2;
					n=(n<1) ? 1 : (n>=MAXWEAPON(0)) ? MAXWEAPON(0)-1 : n;
				}
				if(dice((103-dl)/2)==1) {
					for(; j<NUMWEAPON && stricmp(ServerTable->weapon[j]->Origin,"DUNGEON"); j++);
					if(j<NUMWEAPON)
						n=dice(MAXWEAPON(j))-1;
				}
				strmfe(RPC[1][m]->user.Weapon,ServerTable->weapon[j]->Origin,ServerTable->weapon[j]->Item[n]->Name);
			}
			else
				sprintf(RPC[1][m]->user.Weapon,"NATURAL.%u",n);
			modf(value(ServerTable->weapon[0]->Item[n]->Value,dice(ONLINE->CHA)/5+5),&d);
			g+=d;

			n=MAXARMOR(0)*dl/100+dice(3)-2;
			if(ONLINE->armor_origin==0)
				n=(n+ONLINE->armor_type)/2;
			n=(n<1) ? 1 : (n>=MAXARMOR(0)) ? MAXARMOR(0)-1 : n;
			if(dice(PLAYER.Level/3+12-ONLINE->CHA/10)==1) {
				j=0;
				if(ONLINE->armor_origin==0) {
					n=ONLINE->armor_type+dice(3)-2;
					n=(n<1) ? 1 : (n>=MAXARMOR(0)) ? MAXARMOR(0)-1 : n;
				}
				if(dice((103-dl)/2)==1) {
					for(; j<NUMARMOR && stricmp(ServerTable->armor[j]->Origin,"DUNGEON"); j++);
					if(j<NUMARMOR)
						n=dice(MAXARMOR(j))-1;
				}
				strmfe(RPC[1][m]->user.Armor,ServerTable->armor[j]->Origin,ServerTable->armor[j]->Item[n]->Name);
			}
			else
				sprintf(RPC[1][m]->user.Armor,"NATURAL.%u",n);
			modf(value(ServerTable->armor[0]->Item[n]->Value,dice(ONLINE->CHA)/5+5),&d);
			g+=d;
			RPC[1][m]->user.Gold=g;
			CreateRPC(RPC[1][m]);
			if(!RPC[1][m]->class_origin && !RPC[1][m]->class_type) {
				strcpy(RPC[1][m]->user.Class,PLAYER.Class);
				CreateRPC(RPC[1][m]);
			}
			RPC[1][m]->user.HP/=4;
			RPC[1][m]->HP/=4;
			RPC[1][m]->user.SP/=4;
			RPC[1][m]->SP/=4;
			if(dice(ONLINE->CHA)==1)
				RPC[1][m]->user.WCmod=(RPC[1][m]->weapon_origin<NUMWEAPON) ? 1-dice(WEAPON(RPC[1][m])->Class) : dice(RPC[1][m]->weapon_type);
			if(dice(ONLINE->CHA)==1)
				RPC[1][m]->user.ACmod=(RPC[1][m]->armor_origin<NUMARMOR) ? 1-dice(ARMOR(RPC[1][m])->Class) : dice(RPC[1][m]->armor_type);
			for(n=0; n<NUMMAGIC; n++)
				if(dice(ONLINE->CHA/2+5*n)==1) {
					if(n<16)
						RPC[1][m]->user.Spell|=(UWORD)pow2((double)n);
					if(n>15 && n<24)
						RPC[1][m]->user.XSpell|=(UBYTE)pow2((double)n-16);
				}
			for(n=0; n<NUMPOISON; n++)
				if(dice(ONLINE->CHA/(CLASS(RPC[1][m])->Poison+n+1))==1)
					RPC[1][m]->user.Poison|=(UWORD)pow2((double)n);
			NL;NL;
			strmfp(filename,"PIX/Dungeon",RPC[1][m]->user.Handle);
			type(filename,FALSE);
			sprintf(outbuf,"It's %s%s... ",AN(RPC[1][m]->user.Handle),RPC[1][m]->user.Handle);
			OUT(outbuf);
			Delay(25);
			for(j=1; j<4 && RPC[0][j]->HP; j++);
			if(j<4 && dice((dl/5+5)*(100-ONLINE->CHA+1))==1) {
				OUT("and it is charmed by your presence!");
				Delay(100);
				memcpy(RPC[0][j],RPC[1][m],sizeof(struct RPC));
				memset(RPC[1][m],0,sizeof(struct RPC));
				NL;NL;
				strupr(RPC[0][j]->user.Handle);
				sprintf(prompt,"%sBy what name will you call it? ",fore(CYN));
				OUT(prompt);
				ins(22);
				proper(inbuf);
				if(strlen(inbuf))
					strcpy(RPC[0][j]->user.Handle,inbuf);
				continue;
			}
			OUT("and it doesn't look friendly!");
			Delay(25);
			if(RPC[1][m]->weapon_origin<NUMWEAPON) {
				NL;NL;
				sprintf(outbuf,"%s is carrying %s%s.",RPC[1][m]->He,AN(WEAPON(RPC[1][m])->Name),WEAPON(RPC[1][m])->Name);
				OUT(outbuf);
			}
			if(RPC[1][m]->armor_origin<NUMARMOR) {
				NL;NL;
				sprintf(outbuf,"%s is wearing %s%s.",RPC[1][m]->He,AN(ARMOR(RPC[1][m])->Name),ARMOR(RPC[1][m])->Name);
				OUT(outbuf);
			}
			m++;
		}
		NL;NL;
		Battle();
		if(ONLINE->HP)
			for(i=0,j=0; i<3 && ROOM(dl,hx,hy)->level[i]; i++) {
				if(RPC[1][i]->HP<0) {	//  teleported away
					m=dice(LEVEL(dl)->MaxCol)-1;n=dice(LEVEL(dl)->MaxRow)-1;
					putmonster(m,n);
					ROOM(dl,hx,hy)->level[i]=0;
					ROOM(dl,hx,hy)->monster[i]=0;
				}
				else
				if(RPC[1][i]->HP) {		//  left standing
					ROOM(dl,hx,hy)->level[j]=ROOM(dl,hx,hy)->level[i];
					ROOM(dl,hx,hy)->monster[j]=ROOM(dl,hx,hy)->monster[i];
					j++;
				}
				else {		//  defeated
					modf(EXP(RPC[1][i]->user.ExpLevel-1)/15.,&d);
					PLAYER.Experience+=d;
					sprintf(outbuf,"You get %.8g experience.",d);
					OUT(outbuf);NL;NL;
					if(ONLINE->HP) {
						if(RPC[1][i]->user.Gold>0.) {
							PLAYER.Gold+=RPC[1][0]->user.Gold;
							sprintf(outbuf,"You get %s the %s was carrying.",money(RPC[1][i]->user.Gold,ANSI),RPC[1][i]->user.Handle);
							OUT(outbuf);NL;NL;
							RPC[1][i]->user.Gold=0.;
							Delay(25);
						}
						ExchangeWeapon(ONLINE,RPC[1][i]);
						ExchangeArmor(ONLINE,RPC[1][i]);
						if(LEVEL(dl)->Map!=MAGIC_MAP && dice((15-ONLINE->CHA/10)/2)==1) {
							n=(dice(dl/33+2)>1 ? MAP : MAGIC_MAP);
							if(LEVEL(dl)->Map<n) {
								LEVEL(dl)->Map=n;
								sprintf(outbuf,"%sYou find a %smap!",fore(YELLOW),(LEVEL(dl)->Map==MAGIC_MAP ? "magic " : ""));
								OUT(outbuf);NL;NL;
								NORMAL;
								Delay(25);
							}
						}
						ROOM(dl,hx,hy)->level[i]=0;
						ROOM(dl,hx,hy)->monster[i]=0;
					}
					Delay(25);
				}
			}
		else
			if(PLAYER.Gold>0.) {
				SYS->Gold+=PLAYER.Gold;
				RPGserver(SERVER_PUTUSER,(UBYTE *)SYS);
				sprintf(outbuf,"The %s takes %s you were carrying.",RPC[1][0]->user.Handle,money(PLAYER.Gold,ANSI));
				OUT(outbuf);NL;NL;
				PLAYER.Gold=0.;
			}
		if(ROOM(dl,hx,hy)->level[0]) {
			if(hx==fx && hy==fy) {
				if(hy>0 && ROOM(dl,hx,hy)->type!=2 && ROOM(dl,hx,hy-1)->type!=2)
					fy--;
				else
				if(hy+1<LEVEL(dl)->MaxRow && ROOM(dl,hx,hy)->type!=2 && ROOM(dl,hx,hy+1)->type!=2)
					fy++;
				else
				if(hx+1<LEVEL(dl)->MaxCol && ROOM(dl,hx,hy)->type!=1 && ROOM(dl,hx+1,hy)->type!=1)
					fx++;
				else
				if(hx>0 && ROOM(dl,hx,hy)->type!=1 && ROOM(dl,hx-1,hy)->type!=1)
					fx--;
			}
			swmem(&hx,&fx,sizeof(hx));
			swmem(&hy,&fy,sizeof(hy));
		}
	}

	if(!cd())
		switch(ROOM(dl,hx,hy)->occupant) {
			case CLERIC:
				if(ONLINE->HP<PLAYER.HP)
					type("PIX/Dungeon/Cleric",FALSE);
				OUT("There is an old cleric in this room.");NL;
				if(ONLINE->HP>=PLAYER.HP) {
					OUT("He says, \"I will pray for you.\"");NL;NL;
					break;
				}
				JOUST(dl);
				modf(d/6./PLAYER.HP*(PLAYER.HP-ONLINE->HP),&d);
				if(d<1.)
					d=1.;
				if(stricmp("Cleric",CLASS(ONLINE)->Name)==0)
					d=0.;
				sprintf(outbuf,"He says, \"I can heal all your wounds for %s.\"",(d ? money(d,ANSI) : "you brother"));
				OUT(outbuf);NL;
				if(d) {
					do {
						sprintf(prompt,"%sWill you pay (Y/N)? ",fore(CYN));
						OUT(prompt);
						c=inkey('Y','N');
						NL;
					} while(c!='Y' && c!='N');
				}
				else
					c='Y';
				NL;
				if(PLAYER.Gold<d) {
					OUT("He says, \"Not!\"");NL;NL;
					break;
				}
				if(c=='Y') {
					OUT("He casts a Cure spell on you.");NL;NL;
					PLAYER.Gold-=d;
					ONLINE->HP=PLAYER.HP;
				}
				break;
			case WIZARD:
				if(local) {
					FadeGlobalVolume(24);
					SetChannelEnable(2+8);
				}
				clrmap();
				OUT(fore(MAGENTA));
				OUT("You encounter a wizard in this room.");NL;NL;
				teleport();
				break;
			case TRAP:
				if(dice(100-dl)>1) {
					OUT("You have stepped onto a trapdoor!");NL;NL;
					Delay(25);
					n=(dice(120)<ONLINE->DEX);
					for(m=1; m<5; m++)
						if(RPC[0][m]->HP) {
							if(dice(120)<RPC[0][m]->DEX) {
								sprintf(outbuf,"%s manages to catch the edge and stop %sself from falling.",RPC[0][m]->user.Handle,RPC[0][m]->him);
								OUT(outbuf);NL;
								if(!n)
									memset(RPC[0][m],0,sizeof(struct RPC));
							}
							else {
								sprintf(outbuf,"%s%s falls down a level!",fore(YELLOW),RPC[0][m]->user.Handle);
								OUT(outbuf);NL;
								if(n)
									memset(RPC[0][m],0,sizeof(struct RPC));
							}
							Delay(25);
						}
					if(n) {
						OUT("You manage to catch the edge and stop yourself from falling.");NL;
						Delay(25);NL;
						ROOM(dl,hx,hy)->occupant=FALSE;
						break;
					}
					OUT(fore(YELLOW));
					OUT("You've fallen down a level!");NL;
					NORMAL;NL;
					Delay(25);
					move=TRUE;
					dl++;
					if(LEVEL(dl)->room) {
						renderdmap();
						hx=dice(LEVEL(dl)->MaxCol)-1;hy=dice(LEVEL(dl)->MaxRow)-1;
						fx=hx;fy=hy;
					}
				}
				else {
					OUT(fore(YELLOW));
					OUT("There is a ");
					switch(dice(3)) {
						case 1:
							OUT("fairie");
							break;
						case 2:
							OUT("pixie");
							break;
						case 3:
							OUT("sprite");
							break;
					}
					OUT(" in this room!");
					NORMAL;NL;
					Delay(50);
					if(dice(dl+50)<ONLINE->CHA) {
						OUT("You've enchanted her by your demeanor.");NL;
						Delay(50);
						JOUST(dl);
						if(ONLINE->HP<PLAYER.HP/2) {
							ONLINE->HP+=PLAYER.HP/2;
							OUT("She helps you convalesce by showering you with invigorating beams.");
						}
						else
						if(ONLINE->SP<PLAYER.SP/2) {
							ONLINE->SP+=PLAYER.SP/2;
							OUT("She endows you with restored spell power.");
						}
						else
						if(dice(100-dl+1)>1 && (PLAYER.Gold+PLAYER.Bank)<d) {
							PLAYER.Gold+=d*dice(dl);
							OUT("She deposits a gemstone in your coin pouch.");
						}
						else
						if(dice(WEAPON(ONLINE)->Class)==1 || PLAYER.ACmod>ARMOR(ONLINE)->Class/2) {
							PLAYER.WCmod++;
							OUT("She graces your weapon by a light touch from a tiny finger.");
						}
						else {
							PLAYER.ACmod++;
							OUT("She coats your armor with a twinkling dust.");
						}
					}
					else
						OUT("She is repulsed by your scars and flees in disgust.");
					NL;NL;
					Delay(50);
					ROOM(dl,hx,hy)->occupant=FALSE;
					break;
				}
				if(dice(120)>ONLINE->DEX) {
					if(ONLINE->weapon_origin || ONLINE->weapon_type) {
						if(dice(ONLINE->CHA/10+2)==1)
							PLAYER.WCmod-=dice(WEAPON(ONLINE)->Class/10);
						ONLINE->ToWC-=dice(WEAPON(ONLINE)->Class/10+1);
						sprintf(outbuf,"Your %s is damaged from the fall!",WEAPON(ONLINE)->Name);
						OUT(outbuf);NL;
					}
				}
				if(dice(120)>ONLINE->DEX) {
					if(ONLINE->armor_origin || ONLINE->armor_type) {
						if(dice(ONLINE->CHA/10+2)==1)
							PLAYER.ACmod-=dice(ARMOR(ONLINE)->Class/10);
						ONLINE->ToAC-=dice(ARMOR(ONLINE)->Class/10+1);
						sprintf(outbuf,"Your %s is damaged from the fall!",ARMOR(ONLINE)->Name);
						OUT(outbuf);NL;
					}
				}
				NL;
				break;
			case THIEF:
				sprintf(outbuf,"%sThere is a thief in this chamber!",fore(YELLOW));
				OUT(outbuf);NL;
				Delay(25);
				NORMAL;
				ROOM(dl,hx,hy)->occupant=FALSE;
				do {
					i=dice(LEVEL(dl)->MaxCol)-1;j=dice(LEVEL(dl)->MaxRow)-1;
				} while(ROOM(dl,i,j)->occupant);
				ROOM(dl,i,j)->occupant=THIEF;
				if(ROOM(dl,hx,hy)->occupant!=THIEF) {
					OUT("He surprises you!  As he passes by, he steals ");
					Delay(25);
					if((ONLINE->weapon_origin || ONLINE->weapon_type) && dice(ONLINE->CHA)==1) {
						sprintf(outbuf,"your %s %s(%s%+d%s,%s%+d%s)%s!",WEAPON(ONLINE)->Name,fore(MAG),fore(PLAYER.WCmod>0 ? YELLOW : PLAYER.WCmod<0 ? BRED : GRY),PLAYER.WCmod,fore(GRY),fore(ONLINE->ToWC>0 ? YELLOW : ONLINE->ToWC<0 ? BRED : GRY),ONLINE->ToWC,fore(MAG),fore(GRY));
						strmfe(PLAYER.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[0]->Name);
						ONLINE->weapon_origin=0;ONLINE->weapon_type=0;
						PLAYER.WCmod=0;ONLINE->ToWC=0;
					}
					else
					if(LEVEL(dl)->Map && dice(ONLINE->CHA/10)==1) {
						LEVEL(dl)->Map=FALSE;
						strcpy(outbuf,"your map!");
						move=TRUE;
					}
					else
					if(PLAYER.Spell && (CLASS(ONLINE)->Magic==1 || CLASS(ONLINE)->Magic==2) && dice(ONLINE->CHA/10)==1) {
						do {
							i=dice(NUMMAGIC<16 ? NUMMAGIC : 16)-1;
							t=pow2((double)i);
						} while(!(PLAYER.Spell & t));
						PLAYER.Spell-=t;
						sprintf(outbuf,"your %s of %s!",(CLASS(ONLINE)->Magic==1 ? "wand" : "scroll"),MAGIC(i)->Name);
					}
					else
					if(PLAYER.Poison && dice(ONLINE->CHA/10)==1) {
						do {
							i=dice(NUMPOISON)-1;
							t=pow2((double)i);
						} while(!(PLAYER.Poison & t));
						PLAYER.Poison-=t;
						sprintf(outbuf,"your vial of %s!",POISON(i)->Name);
					}
					else {
						strcpy(line[0],money(PLAYER.Gold,0));
						if((s=strchr(line[0],','))) {
							*s=NULL;
							if(dice(100)<ONLINE->CHA/2)
								strcpy(line[0],s+1);
						}
						gold(&d,line[0]);
						PLAYER.Gold-=d;
						strcpy(line[0],money(d,0));
						if(d)
							c=*(line[0]+strlen(line[0])-1);
						else
							c=NULL;
						sprintf(outbuf,"your pouch of %s pieces!",(c=='c' ? "copper" : c=='s' ? "silver" : c=='g' ? "gold" : c=='p' ? "platinum" : "Reese's"));
					}
					OUT(outbuf);NL;NL;
				}
				else {
					OUT("You surprise him!");NL;NL;
				}
				break;
			case WELL:
				if(local)
					FadeGlobalVolume(24);
				clrmap();
				OUT(fore(MAGENTA));
				OUT("You have found a legendary Wishing Well.");NL;NL;
				OUT(fore(YELLOW));
				OUT("What do you wish to do?");NL;NL;
				NORMAL;
				sprintf(outbuf,"%s<%sB%s>%s Bless yourself",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sC%s>%s Curse another",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sF%s>%s Fix all your damage",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sG%s>%s Grant another call",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sK%s>%s Key hint",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sL%s>%s Loot another player",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sM%s>%s Magical spell or device",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sR%s>%s Resurrect all the dead players",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				sprintf(outbuf,"%s<%sT%s>%s Teleport to another level",fore(BLACK),fore(WHITE),fore(BLACK),fore(CYN));
				OUT(outbuf);NL;
				modem_clr();
				do {
					NL;
					sprintf(prompt,"%sWish: ",fore(CYN));
					OUT(prompt);
					c=inkey(NULL,'C');
				} while(!strchr("BCFGKLMRT",c));
				NL;
				switch(c) {
					case 'B':
						if(strlen(PLAYER.Cursed)) {
							memset(PLAYER.Cursed,0,sizeof(PLAYER.Cursed));
							sprintf(outbuf,"%sYou feel a %sblack%s cloud leave you.",fore(GRY),fore(BLACK),fore(GRY));
							sprintf(outbuf,"%sremoved a cursed",fore(MAGENTA));
							news(outbuf);
						}
						else {
							strcpy(PLAYER.Blessed,"well");
							sprintf(outbuf,"%sYou feel a shining aura surround you.%s",fore(YELLOW),fore(GRY));
							sprintf(outbuf,"%swished for a blessing",fore(MAGENTA));
							news(outbuf);
						}
						ONLINE->STR+=10;
						if(ONLINE->STR>100)
							ONLINE->STR=100;
						ONLINE->INT+=10;
						if(ONLINE->INT>100)
							ONLINE->INT=100;
						ONLINE->DEX+=10;
						if(ONLINE->DEX>100)
							ONLINE->DEX=100;
						ONLINE->CHA+=10;
						if(ONLINE->CHA>100)
							ONLINE->CHA=100;
						OUT(outbuf);NL;
						break;
					case 'C':
						sprintf(prompt,"%sCurse user? ",fore(CYN));
						if(GetRPC(RPC[1][0])) {
							sprintf(outbuf,"%sYou send a %sblack%s cloud to %s.",fore(GRY),fore(BLACK),fore(GRY),ENEMY.Handle);
							OUT(outbuf);NL;
							memset(ENEMY.Blessed,0,sizeof(ENEMY.Blessed));
							strcpy(ENEMY.Cursed,PLAYER.ID);
							RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
							numline=0;
							sprintf(line[numline++],"%s cursed you!",PLAYER.Handle);
							note(ENEMY.ID);
							sprintf(outbuf,"%scursed %s, a level %u %s",fore(MAGENTA),ENEMY.Handle,ENEMY.Level,CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						else {
							sprintf(outbuf,"%sA %sblack%s cloud appears and hovers over you.",fore(GRY),fore(BLACK),fore(GRY));
							OUT(outbuf);NL;
							memset(PLAYER.Blessed,0,sizeof(PLAYER.Blessed));
							strcpy(PLAYER.Cursed,"well");
							if((ONLINE->STR-=10)<1)
								ONLINE->STR=0;
							if((ONLINE->INT-=10)<1)
								ONLINE->INT=0;
							if((ONLINE->DEX-=10)<1)
								ONLINE->DEX=0;
							if((ONLINE->CHA-=10)<1)
								ONLINE->CHA=0;
						}
						break;
					case 'F':
						if(ONLINE->STR<PLAYER.STR)
							ONLINE->STR=PLAYER.STR;
						if(ONLINE->INT<PLAYER.INT)
							ONLINE->INT=PLAYER.INT;
						if(ONLINE->DEX<PLAYER.DEX)
							ONLINE->DEX=PLAYER.DEX;
						if(ONLINE->CHA<PLAYER.CHA)
							ONLINE->CHA=PLAYER.CHA;
						if(PLAYER.ACmod<0)
							PLAYER.ACmod=0;
						if(PLAYER.WCmod<0)
							PLAYER.WCmod=0;
						if(ONLINE->ToAC<0)
							ONLINE->ToAC=0;
						if(ONLINE->ToWC<0)
							ONLINE->ToWC=0;
						if(ONLINE->HP<PLAYER.HP)
							ONLINE->HP=PLAYER.HP;
						if(ONLINE->SP<PLAYER.SP)
							ONLINE->SP=PLAYER.SP;
						if(ONLINE->HULL<PLAYER.Hull)
							ONLINE->HULL=PLAYER.Hull;
						OUT("You are completely healed and all damage has been repaired.");NL;
						break;
					case 'G':
						if(PLAYER.CallsToday) {
							PLAYER.CallsToday--;
							OUT("You are granted another call for the day.");
							sprintf(outbuf,"%swished for an extra call",fore(MAGENTA));
							news(outbuf);
						}
						else
							OUT("A deep laughter bellows.");
						NL;
						break;
					case 'K':
						keyhint();
						break;
					case 'L':
						sprintf(prompt,"%sLoot user? ",fore(CYN));
						if(GetRPC(RPC[1][0])) {
							d=ENEMY.Gold+ENEMY.Bank;
							sprintf(outbuf,"You steal %s from %s.",money(d,ANSI),ENEMY.Handle);
							OUT(outbuf);NL;
							PLAYER.Gold+=d;
							ENEMY.Gold=0.;
							ENEMY.Bank=0.;
							RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
							numline=0;
							sprintf(line[numline++],"%s wished for your %s.",PLAYER.Handle,money(d,ANSI));
							note(ENEMY.ID);
							sprintf(outbuf,"%slooted %s, a level %u %s",fore(MAGENTA),ENEMY.Handle,ENEMY.Level,CLASS(RPC[1][0])->Name);
							news(outbuf);
						}
						else
							OUT("A deep laughter bellows.");
						break;
					case 'M':
						j=0;
						for(i=0; i<(NUMMAGIC<16 ? NUMMAGIC : 16); i++)
							if(!(PLAYER.Spell & 1<<i)) {
								j=i+1;
								PLAYER.Spell|=1<<i;
								break;
							}
						if(!j)
							for(i=16; i<NUMMAGIC; i++)
								if(!(PLAYER.XSpell & 1<<(i-16))) {
									j=i+1;
									PLAYER.XSpell|=1<<(i-16);
								}
						switch(j ? CLASS(ONLINE)->Magic : 0) {
							case 1:
								sprintf(outbuf,"A Wand of %s appears in your hand.",MAGIC(j-1)->Name);
								break;
							case 2:
								sprintf(outbuf,"A Scroll of %s appears in your hand.",MAGIC(j-1)->Name);
								break;
							case 3:
								sprintf(outbuf,"The Spell of %s appears in your head.",MAGIC(j-1)->Name);
								break;
							default:
								strcpy(outbuf,"A deep laughter bellows.");
								break;
						}
						OUT(outbuf);NL;
						break;
					case 'R':
						numline=0;
						sprintf(line[numline++],"%s wished you back from the dead.",PLAYER.Handle);
						strcpy(userlevel.Current,"999");
						while(RPGserver(SERVER_GETUSERLEVEL,(UBYTE *)&userlevel)) {
							strcpy(ENEMY.ID,userlevel.ID);
							RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
							if(strlen(ENEMY.Status) && strcmp(ENEMY.Status,"jail")) {
								sprintf(outbuf,"Resurrecting %s...",ENEMY.Handle);
								OUT(outbuf);
								memset(ENEMY.Status,0,sizeof(ENEMY.Status));
								RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
								note(ENEMY.ID);
								NL;
							}
						}
						sprintf(outbuf,"%swished all the dead resurrected",fore(MAGENTA));
						news(outbuf);
						break;
					case 'T':
						do {
							sprintf(prompt,"%sLevel (0-99): ",fore(CYN));
							OUT(prompt);
							if(ins(2))
								break;
							NL;
							dl=atoi(inbuf);
						} while(dl<0 || dl>99);
						if(LEVEL(dl)->room) {
							renderdmap();
							hx=dice(LEVEL(dl)->MaxCol)-1;hy=dice(LEVEL(dl)->MaxRow)-1;
							fx=hx;fy=hy;
						}
						move=TRUE;
						break;
				}
				NL;NL;
				ROOM(dl,hx,hy)->occupant=FALSE;
				break;
			case WHEEL:
				if(local)
					FadeGlobalVolume(8);
				clrmap();
				Delay(50);
				OUT(fore(MAGENTA));
				OUT("You have found a Mystical Wheel of Life.");NL;NL;
				Delay(50);
				sprintf(outbuf,"%sThe runes are foreign to you.",fore(YELLOW));
				OUT(outbuf);NL;NL;
				Delay(50);
				PausePlaySeq();
				do {
					sprintf(prompt,"%sWill you spin it (Y/N)? ",fore(CYN));
					OUT(prompt);
					c=inkey('N','N');
					NL;
				} while(c!='Y' && c!='N');
				NL;
				if(c=='Y') {
					t=dice(6);
					for(i=0,j=10; i<5; i++,j+=5) {
						n=ONLINE->STR/5-5*i+dice(5)+1;
						for(m=0; m<n; m++) {
							t++;
							beep();
							sprintf(outbuf,"%c\10",spin[m%4]);
							OUT(outbuf);
							Delay(j);
						}
					}
					for(i=1; i<=3; i++) {
						t++;
						beep();
						sprintf(outbuf,"[%s]\15",wol[t%6]);
						OUT(outbuf);
						Delay(50*i);
					}
					switch(t%6) {
						case 0:
							PLAYER.STR=100;ONLINE->STR=100;
							PLAYER.INT=100;ONLINE->INT=100;
							PLAYER.DEX=100;ONLINE->DEX=100;
							PLAYER.CHA=100;ONLINE->CHA=100;
							memset(PLAYER.Cursed,0,sizeof(PLAYER.Cursed));
							break;
						case 1:
							PLAYER.STR-=20;ONLINE->STR-=20;
							PLAYER.INT-=20;ONLINE->INT-=20;
							PLAYER.DEX-=20;ONLINE->DEX-=20;
							PLAYER.CHA-=20;ONLINE->CHA-=20;
							memset(PLAYER.Blessed,0,sizeof(PLAYER.Blessed));
							break;
						case 2:
							JOUST(dl);
							d+=value(WEAPON(ONLINE)->Value,ONLINE->CHA);
							d+=value(ARMOR(ONLINE)->Value,ONLINE->CHA);
							d*=dl+1;
							PLAYER.Gold+=d;
							break;
						case 3:
							PLAYER.Gold=0.;
							PLAYER.Bank=0.;
							JOUST(dl);
							d+=value(WEAPON(ONLINE)->Value,ONLINE->CHA);
							d+=value(ARMOR(ONLINE)->Value,ONLINE->CHA);
							d*=dl+1;
							PLAYER.Loan+=d;
							break;
						case 4:
							ONLINE->HP+=PLAYER.HP;
							ONLINE->SP+=PLAYER.SP;
							PLAYER.WCmod+=dice(WEAPON(ONLINE)->Class);
							ONLINE->ToWC+=WEAPON(ONLINE)->Class;
							PLAYER.ACmod+=dice(ARMOR(ONLINE)->Class);
							ONLINE->ToAC+=ARMOR(ONLINE)->Class;
							sound("hone",64);
							break;
						case 5:
							ONLINE->HP=0;
							ONLINE->SP=0;
							sound("killed",64);
							logoff=TRUE;
							strcpy(reason,"Wheel of Death");
							break;
					}
				}
				NL;
				Delay(50);
				ROOM(dl,hx,hy)->occupant=FALSE;
				break;
		}
	if(!cd() && !escape && LEVEL(dl)->room) {
		if(move) {
			pause();
			drawmap();
		}
		else {
			switch(ROOM(dl,hx,hy)->gift_type) {
				case GIFT_VIAL:
					if(local)
						PausePlaySeq();
					if(CLASS(ONLINE)->Magic<2)
						switch(ROOM(dl,hx,hy)->gift_value) {
							case 2:
								ROOM(dl,hx,hy)->gift_value=0;
								break;
							case 3:
								ROOM(dl,hx,hy)->gift_value=1;
								break;
						}
					switch(ROOM(dl,hx,hy)->gift_value) {
						case 0:
							v="Vial of Slaad Secretions";
							break;
						case 1:
							v="Potion of Cure Light Wounds";
							break;
						case 2:
							v="Flask of Fire Water";
							break;
						case 3:
							v="Potion of Mana";
							break;
						case 4:
							v="Vial of Weakness";
							break;
						case 5:
							v="Potion of Stamina";
							break;
						case 6:
							v="Vial of Stupidity";
							break;
						case 7:
							v="Potion of Wisdom";
							break;
						case 8:
							v="Vial of Clumsiness";
							break;
						case 9:
							v="Potion of Agility";
							break;
						case 10:
							v="Vile Vial";
							break;
						case 11:
							v="Potion of Charm";
							break;
						case 12:
							v="Vial of Crack";
							break;
						case 13:
							v="Potion of Augment";
							break;
						case 14:
							v="Beaker of Death";
							break;
						case 15:
							v="Elixir of Restoration";
							break;
					}
					sprintf(outbuf,"%sOn the ground, you find ",fore(CYAN));
					OUT(outbuf);
					if(dice(100)>ONLINE->INT/10*CLASS(ONLINE)->Poison+1) {
						switch(dice(3)) {
							case 1:
								s="a bottle containing ";
								break;
							case 2:
								s="a flask of some ";
								break;
							case 3:
								s="a vial holding ";
								break;
						}
						OUT(s);
						switch(dice(5)) {
							case 1:
								s="bubbling";
								break;
							case 2:
								s="clear";
								break;
							case 3:
								s="dark";
								break;
							case 4:
								s="sparkling";
								break;
							case 5:
								s="tainted";
								break;
						}
						OUT(s);
						OUT(", ");
						switch(dice(5)) {
							case 1:
								s="amber";
								break;
							case 2:
								s="blue";
								break;
							case 3:
								s="crimson";
								break;
							case 4:
								s="green";
								break;
							case 5:
								s="purple";
								break;
						}
						OUT(s);
						OUT(" potion");
					}
					else {
						s=v;v=NULL;
						OUT(s);
					}
					OUT(".");NL;
					if(dice(100)<(50+ONLINE->INT/2)) {
						sprintf(prompt,"%sWill you drink it (Y/N)? ",fore(CYN));
						do {
							OUT(prompt);
							c=inkey('Y','N');
							NL;
						} while(c!='Y' && c!='N');
					}
					else {
						Delay(25);
						OUT("You quaff it without hesitation.");NL;
						c='Y';
					}
					if(v) {
						s=v;
						sprintf(outbuf,"It was %s%s.",AN(s),s);
						OUT(outbuf);NL;NL;
					}
					if(c=='Y') {
						sound("quaff",64);
						Delay(25);
						switch(ROOM(dl,hx,hy)->gift_value) {
							case 0:
								ONLINE->HP-=dice(ONLINE->HP);
								if(ONLINE->HP<1) {
									logoff=TRUE;
									sprintf(reason,"quaffed %s%s",AN(s),s);
								}
								break;
							case 1:
								ONLINE->HP+=dice(PLAYER.HP-ONLINE->HP);
								break;
							case 2:
								ONLINE->SP-=dice(ONLINE->SP);
								break;
							case 3:
								ONLINE->SP+=dice(PLAYER.SP-ONLINE->SP);
								break;
							case 4:
								if(ONLINE->STR<20)
									PLAYER.STR--;
								else
									ONLINE->STR-=dice(10);
								break;
							case 5:
								ONLINE->STR+=dice(10);
								if(ONLINE->STR>100) {
									ONLINE->STR=100;
									PLAYER.STR++;
									if(PLAYER.STR>99)
										PLAYER.STR=99;
								}
								break;
							case 6:
								if(ONLINE->INT<20)
									PLAYER.INT--;
								else
									ONLINE->INT-=dice(10);
								break;
							case 7:
								ONLINE->INT+=dice(10);
								if(ONLINE->INT>100) {
									ONLINE->INT=100;
									PLAYER.INT++;
									if(PLAYER.INT>99)
										PLAYER.INT=99;
								}
								break;
							case 8:
								if(ONLINE->DEX<20)
									PLAYER.DEX--;
								else
									ONLINE->DEX-=dice(10);
								break;
							case 9:
								ONLINE->DEX+=dice(10);
								if(ONLINE->DEX>100) {
									ONLINE->DEX=100;
									PLAYER.DEX++;
									if(PLAYER.DEX>99)
										PLAYER.DEX=99;
								}
								break;
							case 10:
								if(ONLINE->CHA<20)
									PLAYER.CHA--;
								else
									ONLINE->CHA-=dice(10);
								break;
							case 11:
								ONLINE->CHA+=dice(10);
								if(ONLINE->CHA>100) {
									ONLINE->CHA=100;
									PLAYER.CHA++;
									if(PLAYER.CHA>99)
										PLAYER.CHA=99;
								}
								break;
							case 12:
								ONLINE->STR-=dice(ONLINE->STR);
								ONLINE->INT-=dice(ONLINE->INT);
								ONLINE->DEX-=dice(ONLINE->DEX);
								ONLINE->CHA-=dice(ONLINE->CHA);
								PLAYER.STR-=(PLAYER.STR<10) ? PLAYER.STR : dice(10);
								PLAYER.INT-=(PLAYER.INT<10) ? PLAYER.INT : dice(10);
								PLAYER.DEX-=(PLAYER.DEX<10) ? PLAYER.DEX : dice(10);
								PLAYER.CHA-=(PLAYER.CHA<10) ? PLAYER.CHA : dice(10);
								break;
							case 13:
								ONLINE->STR+=dice(100-ONLINE->STR+1)-1;
								ONLINE->INT+=dice(100-ONLINE->INT+1)-1;
								ONLINE->DEX+=dice(100-ONLINE->DEX+1)-1;
								ONLINE->CHA+=dice(100-ONLINE->CHA+1)-1;
								PLAYER.STR+=(PLAYER.STR>90) ? 100-PLAYER.STR : dice(10);
								PLAYER.INT+=(PLAYER.INT>90) ? 100-PLAYER.INT : dice(10);
								PLAYER.DEX+=(PLAYER.DEX>90) ? 100-PLAYER.DEX : dice(10);
								PLAYER.CHA+=(PLAYER.CHA>90) ? 100-PLAYER.CHA : dice(10);
								break;
							case 14:
								ONLINE->HP=0;
								ONLINE->SP=0;
								logoff=TRUE;
								sprintf(reason,"quaffed a %s",v);
								break;
							case 15:
								ONLINE->HP=PLAYER.HP;
								ONLINE->SP=PLAYER.SP;
								break;
						}
					}
					break;
				case GIFT_POISON:
					PLAYER.Poison|=1<<ROOM(dl,hx,hy)->gift_value;
					sprintf(outbuf,"%sYou find a vial of %s!",fore(YELLOW),POISON(ROOM(dl,hx,hy)->gift_value)->Name);
					OUT(outbuf);NL;NL;
					break;
				case GIFT_MAGIC:
					PLAYER.Spell|=1<<ROOM(dl,hx,hy)->gift_value;
					if(CLASS(ONLINE)->Magic==1)
						sprintf(outbuf,"%sYou find a Wand of %s!",fore(YELLOW),MAGIC(ROOM(dl,hx,hy)->gift_value)->Name);
					else
						sprintf(outbuf,"%sYou find a %s scroll!",fore(YELLOW),MAGIC(ROOM(dl,hx,hy)->gift_value)->Name);
					OUT(outbuf);NL;NL;
					break;
				case GIFT_XMAGIC:
					PLAYER.XSpell|=1<<ROOM(dl,hx,hy)->gift_value;
					if(CLASS(ONLINE)->Magic==1)
						sprintf(outbuf,"%sYou find a Wand of %s!",fore(YELLOW),MAGIC(16+ROOM(dl,hx,hy)->gift_value)->Name);
					else
						sprintf(outbuf,"%sYou find a %s scroll!",fore(YELLOW),MAGIC(16+ROOM(dl,hx,hy)->gift_value)->Name);
					OUT(outbuf);NL;NL;
					break;
				case GIFT_CHEST:
					JOUST(dl);
					d+=value(WEAPON(ONLINE)->Value,ONLINE->CHA);
					d+=value(ARMOR(ONLINE)->Value,ONLINE->CHA);
					d*=ROOM(dl,hx,hy)->gift_value;
					PLAYER.Gold+=d;
					if(d>0.)
						sprintf(outbuf,"%sYou find a treasure chest holding %s!",fore(YELLOW),money(d,ANSI));
					else
						sprintf(outbuf,"%sYou find an empty, treasure chest.",fore(YELLOW));
					OUT(outbuf);NL;NL;
					break;
			}
			ROOM(dl,hx,hy)->gift_type=0;
		}
	}
}

void Dungeon(int level)
{
	int help=TRUE,x,y;
	char *oof,*s;
	char c;

	dl=(level<0) ? 0 : (level>99) ? 99 : level;
	escape=FALSE;
	if(!(deep_dank_dungeon=calloc(100,sizeof(struct deep_dank_dungeon))))
		return;
	title("Dungeon");

	if(local) {
		if(sample1->data) {
			gs_stop_sound(CHANNEL1);
			gs_stop_sound(CHANNEL3);
			gs_close_sound();
			gs_free_sound(sample1);
			sample1->data=NULL;
			if(sample2->data) {
				gs_free_sound(sample2);
				sample2->data=NULL;
			}
		}
		sprintf(filename,"Sounds/Dungeon%u.mod",dice(4));
		if(!(LoadModule(filename))) {
			SetChannelEnable(1+2+4+8+128);
			SetVolumeFadeRate(6);
			FadeGlobalVolume(56);
			BeginPlaySeq();
		}
	}

	if(ANSI==1)
		OUT("\33\51\60");

	while(!cd() && !escape) {
		if(!LEVEL(dl)->room)
			GenLevel();

		if((ONLINE->weapon_origin || ONLINE->weapon_type) && (int)WEAPON(ONLINE)->Class+PLAYER.WCmod+ONLINE->ToWC<0) {
			sprintf(outbuf,"Your %s is damaged beyond repair; you toss it aside.",WEAPON(ONLINE)->Name);
			OUT(outbuf);NL;NL;
			Delay(25);
			strmfe(PLAYER.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[0]->Name);
			ONLINE->weapon_origin=0;ONLINE->weapon_type=0;
			PLAYER.WCmod=0;ONLINE->ToWC=0;
		}
		if((ONLINE->armor_origin || ONLINE->armor_type) && (int)ARMOR(ONLINE)->Class+PLAYER.ACmod+ONLINE->ToAC<0) {
			sprintf(outbuf,"Your %s is damaged beyond repair; you toss it aside.",ARMOR(ONLINE)->Name);
			OUT(outbuf);NL;NL;
			Delay(25);
			strmfe(PLAYER.Armor,ServerTable->armor[0]->Origin,ServerTable->armor[0]->Item[0]->Name);
			ONLINE->armor_origin=0;ONLINE->armor_type=0;
			PLAYER.ACmod=0;ONLINE->ToAC=0;
		}

		if(move) {
			move=FALSE;
			x=dice(LEVEL(dl)->MaxCol)-1;y=dice(LEVEL(dl)->MaxRow)-1;
			if(dice((int)(ROOM(dl,x,y)->type==0 ? 2 : ROOM(dl,x,y)->type==3 ? 1 : 4)*ONLINE->CHA/5-moves/10)==1) {
				switch(dice(5)) {
					case 1:
						s="Your skin crawls";
						break;
					case 2:
						s="Your pulse quickens";
						break;
					case 3:
						s="You feel paranoid";
						break;
					case 4:
						s="Your grip tightens";
						break;
					case 5:
						s="You stand ready";
						break;
				}
				strcpy(outbuf,s);
				strcat(outbuf," when you hear a ");
				switch(dice(5)) {
					case 1:
						s="loud, scraping noise";
						break;
					case 2:
						s="clap of thunder";
						break;
					case 3:
						s="soft, hissing noise";
						break;
					case 4:
						s="growl from a hungry beast";
						break;
					case 5:
						s="hideous laugh";
						break;
				}
				strcat(outbuf,s);
				if(abs(x-hx)<3 && abs(y-hy)<3)
					s=" nearby!";
				else
				if(abs(x-hx)<6 && abs(y-hy)<6)
					s=" in the distance.";
				else
					s=" echoing through the corridors.";
				strcat(outbuf,s);
				if(putmonster(x,y)) {
					plot(&SERCON,rows,1);
					OUT(outbuf);NL;NL;
					if(LEVEL(dl)->Map==MAGIC_MAP)
						drawroom(x,y);
					if(ROOM(dl,x,y)->occupant==CLERIC) {
						plot(&SERCON,rows-1,1);
						NORMAL;
						sound("agony",32);
						Delay(50);
						OUT("You hear a dying cry of agony!!");NL;
						Delay(100);
						ROOM(dl,x,y)->occupant=FALSE;
						if(LEVEL(dl)->Map==MAGIC_MAP)
							drawroom(x,y);
					}
				}
			}
			drawroom(hx,hy);
			drawyou();
			if(ONLINE->INT>49)
				ROOM(dl,hx,hy)->map=TRUE;
			ExamineRoom();
			continue;
		}

		plot(&SERCON,rows,1);
		if(PLAYER.Level<SYS->Level)
			chkexp();
		RPGserver(SERVER_UNLOCK,NULL);
		if(local) {
			SetChannelEnable(1+2+4+8+128);
			FadeGlobalVolume(48);
			ContinuePlaySeq();
		}
		if(help) {
			strcpy(prompt,fore(CYN));
			if(hy>0 && ROOM(dl,hx,hy)->type!=2 && ROOM(dl,hx,hy-1)->type!=2)
				strcat(prompt,"<N> North, ");
			if(hy+1<LEVEL(dl)->MaxRow && ROOM(dl,hx,hy)->type!=2 && ROOM(dl,hx,hy+1)->type!=2)
				strcat(prompt,"<S> South, ");
			if(hx+1<LEVEL(dl)->MaxCol && ROOM(dl,hx,hy)->type!=1 && ROOM(dl,hx+1,hy)->type!=1)
				strcat(prompt,"<E> East, ");
			if(hx>0 && ROOM(dl,hx,hy)->type!=1 && ROOM(dl,hx-1,hy)->type!=1)
				strcat(prompt,"<W> West, ");
			if(PLAYER.Spell)
				strcat(prompt,"<C> Cast, ");
			if(PLAYER.Poison)
				strcat(prompt,"<P> Poison, ");
			strcat(prompt,"<Y> Status: ");
			help=(PLAYER.Novice=='Y');
		}
		else
			sprintf(prompt,"%s:",fore(CYN));
		OUT(prompt);

		move=FALSE;
		oof=NULL;
		c=inkey(NULL,NULL);
		NL;NL;
		switch(c) {
			case 'N':
				if(hy>0 && ROOM(dl,hx,hy)->type!=2 && ROOM(dl,hx,hy-1)->type!=2) {
					drawroom(hx,hy);
					fx=hx;fy=hy;
					hy--;
					move=TRUE;
				}
				else
					oof="north";
				break;
			case 'S':
				if(hy+1<LEVEL(dl)->MaxRow && ROOM(dl,hx,hy)->type!=2 && ROOM(dl,hx,hy+1)->type!=2) {
					drawroom(hx,hy);
					fx=hx;fy=hy;
					hy++;
					move=TRUE;
				}
				else
					oof="south";
				break;
			case 'E':
				if(hx+1<LEVEL(dl)->MaxCol && ROOM(dl,hx,hy)->type!=1 && ROOM(dl,hx+1,hy)->type!=1) {
					drawroom(hx,hy);
					fx=hx;fy=hy;
					hx++;
					move=TRUE;
				}
				else
					oof="east";
				break;
			case 'W':
				if(hx>0 && ROOM(dl,hx,hy)->type!=1 && ROOM(dl,hx-1,hy)->type!=1) {
					drawroom(hx,hy);
					fx=hx;fy=hy;
					hx--;
					move=TRUE;
				}
				else
					oof="west";
				break;
			case 'C':
				PausePlaySeq();
				x=Cast(ONLINE,NULL);
				if(x==TELEPORT_SPELL) {
					clrmap();
					teleport();
				}
				if(x==-TELEPORT_SPELL) {
					hx=dice(LEVEL(dl)->MaxCol)-1;hy=dice(LEVEL(dl)->MaxRow)-1;
					fx=hx;fy=hy;
					move=TRUE;
				}
				break;
			case 'P':
				PausePlaySeq();
				PoisonWeapon(ONLINE);
				break;
			case 'Y':
				yourstats();
				break;
			default:
				help=TRUE;
				break;
		}
		if(move) {
			moves++;
			if(dice((int)ONLINE->CHA*ONLINE->INT/10+2)==1) {
				plot(&SERCON,rows,1);
				switch(dice(5)) {
					case 1:
						sprintf(outbuf,"%sA bat flies by and soils your %s.",fore(BLU),ARMOR(ONLINE)->Name);
						PLAYER.ACmod--;
						break;
					case 2:
						sprintf(outbuf,"%sA drop of acid water lands on your %s.",fore(BLUE),WEAPON(ONLINE)->Name);
						PLAYER.WCmod--;
						break;
					case 3:
						sprintf(outbuf,"%sYou trip on the rocky surface and hurt yourself.",fore(BRN));
						ONLINE->HP-=dice(dl);
						if(ONLINE->HP<1) {
							logoff=TRUE;
							strcpy(reason,"fell down");
						}
						break;
					case 4:
						sprintf(outbuf,"%sYou are attacked by a swarm of bees.",fore(BRED));
						for(x=0,y=dice(dl)+1; x<y; x++)
							ONLINE->HP-=dice(dl+1);
						if(ONLINE->HP<1) {
							logoff=TRUE;
							strcpy(reason,"killer bees");
						}
						break;
					case 5:
						sprintf(outbuf,"%sA bolt of lightning strikes you.",fore(WHITE));
						PLAYER.ACmod-=dice(ARMOR(ONLINE)->Class/2);
						ONLINE->ToAC-=dice(ARMOR(ONLINE)->Class/2);
						PLAYER.WCmod-=dice(WEAPON(ONLINE)->Class/2);
						ONLINE->ToWC-=dice(WEAPON(ONLINE)->Class/2);
						ONLINE->HP-=dice(PLAYER.HP/2);
						if(ONLINE->HP<1) {
							logoff=TRUE;
							strcpy(reason,"struck by lightning");
						}
						break;
				}
				OUT(outbuf);
				NORMAL;NL;
				Delay(100);
			}
		}

		if(oof) {
			sound("wall",64);
			sprintf(outbuf,"%sOof!  There is a wall to the %s.",fore(YELLOW),oof);
			OUT(outbuf);NL;NL;
			ONLINE->HP-=dice(dl);
			modem_clr();
			if(ONLINE->HP<1) {
				sprintf(outbuf,"%sYou took too many hits and die!",fore(WHITE));
				OUT(outbuf);NL;NL;
				Delay(50);
				logoff=TRUE;
				strcpy(reason,"banged head against a wall");
			}
		}

		timeleft();
	}

	if(ANSI==1)
		OUT("\33\51\102");
	if(!escape) {
		Delay(100);
		y=SERCON.Row+1;
		sprintf(outbuf,"\33[1;%ur",rows);
		OUT(outbuf);
		plot(&SERCON,y,1);
	}

	if(local)
		UnloadModule();

	for(x=1; x<5; x++)
		memset(RPC[0][x],0,sizeof(struct RPC));
	for(x=0; x<5; x++)
		memset(RPC[1][x],0,sizeof(struct RPC));
	for(dl=0; dl<100; dl++)
		if(LEVEL(dl)->room)
			free(LEVEL(dl)->room);
	free(deep_dank_dungeon);
}
