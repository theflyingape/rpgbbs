#include "/include/all.h"
#include "global.h"



void MSGbase(int base,int first,int last,int keep)
{
	static char where[24];
	static struct RPGmenu __far RPGmenu = { where,BLU,6,
							{	{ 'E',"Enter new message" },
								{ 'F',"First message" },
								{ 'L',"Last message" },
								{ 'N',"Next message" },
								{ 'P',"Previous message" },
								{ 'R',"Reply" }
							}
	};
	static struct RPGmenu __far VISITmenu = { where,BLU,4,
							{	{ 'F',"First message" },
								{ 'N',"Next message" },
								{ 'P',"Previous message" },
								{ 'L',"Last message" },
							}
	};
	static struct RPGmenu __far SYSOPmenu = { where,BLU,7,
							{	{ 'E',"Enter new message" },
								{ 'F',"First message" },
								{ 'K',"Kill message" },
								{ 'L',"Last message" },
								{ 'N',"New messages" },
								{ 'P',"Previous message" },
								{ 'R',"Reply" }
							}
	};
	char c;

	strncpy(where,ta[base],24);
	while((c=option(ACCESS(PLAYER.Access)->Sysop=='Y' ? &SYSOPmenu : ACCESS(acclvl)->RolePlay=='Y' ? &RPGmenu : &VISITmenu))!='Q') {
		if(PLAYER.LastMSG[base]<NEW->LastMSG[base])
			PLAYER.LastMSG[base]=NEW->LastMSG[base];
		if(PLAYER.LastMSG[base]>SYS->LastMSG[base])
			PLAYER.LastMSG[base]=SYS->LastMSG[base];
		for(; (NEW->LastMSG[base]+99)<SYS->LastMSG[base]; NEW->LastMSG[base]++) {
			sprintf(filename,"%05u",NEW->LastMSG[base]);
			remove(filename);
			RPGserver(SERVER_PUTUSER,(UBYTE *)NEW);
		}
		switch(c) {
			case 'E':
				sprintf(prompt,"%sTo: ",fore(CYN));
				GetRPC(RPC[1][0]);
				Editor(RPC[1][0],99);
				if(numline) {
					sprintf(filename,"%05u",++SYS->LastMSG[base]);
					RPGserver(SERVER_PUTUSER,(UBYTE *)SYS);
					if(!NEW->LastMSG[base]) {
						NEW->LastMSG[base]++;
						RPGserver(SERVER_PUTUSER,(UBYTE *)NEW);
					}
					WriteMail(filename);
					if(strlen(ENEMY.ID)) {
						numline=0;
						sprintf(line[numline++],"%s left you a message in %s",PLAYER.Handle,where);
						note(ENEMY.ID);
					}
					PLAYER.Posts++;
				}
				break;
			case 'F':
				if(!(PLAYER.LastMSG[base]=NEW->LastMSG[base])) {
					OUT("No more messages.");NL;
					break;
				}
				sprintf(filename,"%05u",PLAYER.LastMSG[base]);
				if(!ReadMail(filename)) {
					sprintf(outbuf,"Message `%s' has been deleted.",filename);
					OUT(outbuf);NL;NL;
				}
				else
				if(!PLAYER.Expert)
					pause();
				break;
			case 'K':
				sprintf(filename,"%05u",PLAYER.LastMSG[base]);
				remove(filename);
				break;
			case 'L':
				if(!(PLAYER.LastMSG[base]=SYS->LastMSG[base])) {
					OUT("No more messages.");NL;
					break;
				}
				sprintf(filename,"%05u",PLAYER.LastMSG[base]);
				if(!ReadMail(filename)) {
					sprintf(outbuf,"Message `%s' has been deleted.",filename);
					OUT(outbuf);NL;
					break;
				}
				if(!PLAYER.Expert)
					pause();
				break;
			case 'N':
				if(PLAYER.LastMSG[base]==SYS->LastMSG[base]) {
					OUT("No more messages.");NL;
					break;
				}
				PLAYER.LastMSG[base]++;
				sprintf(filename,"%05u",PLAYER.LastMSG[base]);
				if(!ReadMail(filename)) {
					sprintf(outbuf,"Message `%s' has been deleted.",filename);
					OUT(outbuf);NL;NL;
				}
				else
				if(!PLAYER.Expert)
					pause();
				break;
			case 'P':
				if(PLAYER.LastMSG[base]<2 || PLAYER.LastMSG[base]==NEW->LastMSG[base]) {
					OUT("No more messages.");NL;
					break;
				}
				PLAYER.LastMSG[base]--;
				sprintf(filename,"%05u",PLAYER.LastMSG[base]);
				if(!ReadMail(filename)) {
					sprintf(outbuf,"Message `%s' has been deleted.",filename);
					OUT(outbuf);NL;NL;
				}
				else
				if(!PLAYER.Expert)
					pause();
				break;
			case 'R':
				Editor(RPC[1][0],99);
				if(numline) {
					sprintf(filename,"%05u",++SYS->LastMSG[base]);
					RPGserver(SERVER_PUTUSER,(UBYTE *)SYS);
					if(!NEW->LastMSG[base]) {
						NEW->LastMSG[base]++;
						RPGserver(SERVER_PUTUSER,(UBYTE *)NEW);
					}
					WriteMail(filename);
					if(strlen(ENEMY.ID)) {
						numline=0;
						sprintf(line[numline++],"%s left you a message in %s",PLAYER.Handle,where);
						note(ENEMY.ID);
					}
					PLAYER.Posts++;
				}
				break;
		}
	}
}

void Bulletins(void)
{
	BPTR msglock;
	long first,last,keep;
	int b=(ANSI==1) ? 0 : 1;
	int i,r,c;
	char *s,**tool;
	char new;

	if(!(msglock=Lock(MSGpath,ACCESS_READ))) {
		sprintf(outbuf,"%sSorry, could not access Message Base directory \"%s\"",fore(CYAN),MSGpath);
		OUT(outbuf);NL;
		pause();
		return;
	}

	if(!b)
		OUT("\33)0");

	while(!cd()) {
		title("Bulletins");
		cls();
		CurrentDir(msglock);
		if((lock=Lock("",ACCESS_READ))) {
			if((Examine(lock,&infoBlock))) {
				MaxDir=0;
				while(ExNext(lock,&infoBlock)) {
					if(infoBlock.fib_DirEntryType>0) {
						ta[MaxDir++]=line[MaxDir];
						strcpy(line[MaxDir],infoBlock.fib_FileName);
						if(MaxDir==20)
							break;
					}
				}
			}
			UnLock(lock);
			if(MaxDir)
				tqsort(ta,MaxDir);
		}

		outbuf[0]=box[b][7];
		memset(&outbuf[1],box[b][0],59);
		outbuf[60]=box[b][9];
		outbuf[61]=NULL;
		OUT(b ? fore(BLU) : "\16");
		OUT(outbuf);NL;

		OUT(b ? fore(BLU) : "\16");
		sprintf(outbuf,"%c",box[b][10]);
		OUT(outbuf);
		OUT(b ? fore(BLUE) : "\17");
		sprintf(line[0],"%s Bulletins",SYS->Name);
		i=(59-strlen(line[0]))/2;
		c=i+(strlen(line[0])+1)%2;
		sprintf(outbuf,"%*s%s%*s",i,"",line[0],c,"");
		OUT(outbuf);
		OUT(b ? fore(BLU) : "\16");
		sprintf(outbuf,"%c",box[b][10]);
		OUT(outbuf);
		OUT(b ? fore(BLU) : "\17");
		NL;

		outbuf[0]=box[b][4];
		memset(&outbuf[1],box[b][0],59);
		outbuf[30]=box[b][8];
		outbuf[60]=box[b][6];
		outbuf[61]=NULL;
		OUT(b ? fore(BLU) : "\16");
		OUT(outbuf);NL;
		OUT(b ? fore(BLU) : "\17");

		for(r=0; r<MaxDir/2+1; r++) {
			for(c=0; c<2; c++) {
				i=(MaxDir/2+1)*c+r;
				new=PLAYER.LastMSG[i]<SYS->LastMSG[i] ? '*' : ' ';
				sprintf(outbuf,"%s%c%s%s %s%c%s%-23.23s",
						(b ? fore(BLU) : "\16"),box[b][10],(b ? "" : "\17"),
						(i<MaxDir ? bracket(i+1) : "    "),
						fore(YELLOW),new,fore(GRY),
						(i<MaxDir ? ta[i] : "")
				);
				OUT(outbuf);
			}
			sprintf(outbuf,"%s%c",(b ? fore(BLU) : "\16"),box[b][10]);
			OUT(outbuf);NL;
		}

		outbuf[0]=box[b][4];
		memset(&outbuf[1],box[b][0],59);
		outbuf[30]=box[b][2];
		outbuf[60]=box[b][6];
		outbuf[61]=NULL;
		OUT(b ? fore(BLU) : "\16");
		OUT(outbuf);NL;
		OUT(b ? fore(BLU) : "\17");

		outbuf[0]=box[b][10];
		memset(&outbuf[1],' ',59);
		outbuf[60]=box[b][10];
		outbuf[61]=NULL;
		OUT(b ? fore(BLU) : "\16");
		OUT(outbuf);NL;
		OUT(b ? fore(BLU) : "\17");

		outbuf[0]=box[b][1];
		memset(&outbuf[1],box[b][0],59);
		outbuf[60]=box[b][3];
		outbuf[61]=NULL;
		OUT(b ? fore(BLU) : "\16");
		OUT(outbuf);
		OUT(b ? fore(BLU) : "\17");
		SAMELINE;
		sprintf(outbuf,"\33[A\33[55C%s*%snew",fore(YELLOW),fore(BLUE));
		OUT(outbuf);
		SAMELINE;OUT("\33[2C");
		sprintf(prompt,"%sSection # (Q=Quit): ",fore(CYN));
		OUT(prompt);
		if(ins(2))
			break;
		NL;
		if(toupper(*inbuf)=='Q')
			break;
		i=atoi(inbuf);
		if(i<1 || i>MaxDir)
			continue;
		if((diskobj=GetDiskObject(ta[--i]))) {
			tool=(char **)diskobj->do_ToolTypes;
			first=1;last=1;keep=100;
			if((s=(char *)FindToolType(tool,"FIRST")))
				first=atol(s);
			if((s=(char *)FindToolType(tool,"LAST")))
				last=atol(s);
			if((s=(char *)FindToolType(tool,"KEEP")))
				keep=atol(s);
			if((lock=Lock(ta[i],ACCESS_READ))) {
				CurrentDir(lock);
				MSGbase(i,first,last,keep);
				UnLock(lock);
			}
			FreeDiskObject(diskobj);
		}
	}
	UnLock(msglock);
	CurrentDir(root);
}
