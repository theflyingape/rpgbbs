/*********************\
 *  RPGBBS Bulletin  *
\*********************/
#include "RPGBBS.h"

void SendMessage(int who,int base)
{
	FILE *fp;
	int i;
	char file[13];

	newline();
	if(Editor(50)=='S') {
		outs("aving...");
		USER(user)->LastTime=100*atoi(&SystemTime[0])+atoi(&SystemTime[3]);
		if(SystemTime[9]=='P')
			USER(user)->LastTime+=1200;
		sprintf(file,"MSG%05u.B%02u",RPGinfo->NextMSG[base],base);
		strmfp(filename,MSGpath,file);
		if((fp=fopen(filename,"w"))) {
			fprintf(fp,"%.8s%04d%03d%03d\n",SystemDate,(int)USER(user)->LastTime,(int)user,who);
			for(i=0; i<numline; i++)
				fprintf(fp,"%s\n",line[i]);
			fputs("\n",fp);
			fclose(fp);
			RPGinfo->NextMSG[base]++;
			USER(user)->TotalPosts++;
			sprintf(text,"Ok.  Post/call ratio = %g",(double)USER(user)->TotalPosts/(double)USER(user)->TotalCalls);
			outs(text);
			if(who) {
				numline=0;
				sprintf(line[numline++],"%.26s left you a message in [#%d] %s.",USER(user)->Handle,base,MSGname[base]);
				UserNote(who);
			}
		}
		else
			outs("Error!");
		newline();
		newline();
	}
}

void Bulletin(void)
{
	static struct RPGmenu __far RPGmenu = { NULL,CYN,5,
							{	{ 'E',"Enter a message" },
								{ 'F',"First message" },
								{ 'L',"Last message" },
								{ 'N',"Next message" },
								{ 'P',"Previous message" }
							}
	};
	int who;
	char c,file[13];

	while(!cd()) {
		cls();
		sprintf(text,"%s--=:))%s%s BULLETINS %s%s((:=--",Fcol(BLU),Fcol(WHITE),Bcol(BLU),Fcol(BLU),Bcol(BLK));
		outs(text);
		normal();
		newline();
		newline();
		for(base=0; base<MaxMSG; base++) {
			if(USER(user)->LastMSG[base]<RPGinfo->FirstMSG[base])
				USER(user)->LastMSG[base]=RPGinfo->FirstMSG[base];
			if(USER(user)->LastMSG[base]>RPGinfo->NextMSG[base])
				USER(user)->LastMSG[base]=RPGinfo->NextMSG[base];
			outs(bracket(base+1));
			if(USER(user)->LastMSG[base]-RPGinfo->NextMSG[base]) {
				outs(Fcol(YELLOW));
				outs(" *");
				normal();
			}
			else
				outs("  ");
			outs(MSGname[base]);
			newline();
		}
		newline();
		outs(Fcol(CYN));
		outs("Enter Message Base # (Q=Quit): ");
		if(input())
			break;
		newline();
		if(toupper(ins[0])=='Q')
			break;
		base=atoi(ins);
		if(base<1 || base>MaxMSG)
			continue;
		base--;
		RPGmenu.option=MSGname[base];
		while(!cd()) {
			while(RPGinfo->NextMSG[base]-RPGinfo->FirstMSG[base]>KeepMSG[base]) {
				sprintf(file,"MSG%05u.B%02u",RPGinfo->FirstMSG[base]++,base);
				strmfp(filename,MSGpath,file);
				remove(filename);
			}
			if((c=Option(&RPGmenu))=='Q')
				break;
			switch(c) {
				case 'E':
					if(!acclvl)
						break;
					outs(Fcol(CYN));
					outs("To <All>: ");
					if((who=GetUser())<0)
						who=0;
					SendMessage(who,base);
					break;
				case 'F':
					USER(user)->LastMSG[base]=RPGinfo->FirstMSG[base];
					sprintf(file,"MSG%05u.B%02u",USER(user)->LastMSG[base]++,base);
					strmfp(filename,MSGpath,file);
					TypeMessage(filename);
					break;
				case 'L':
					if(RPGinfo->NextMSG[base]>1) {
						USER(user)->LastMSG[base]=RPGinfo->NextMSG[base]-1;
						sprintf(file,"MSG%05u.B%02u",USER(user)->LastMSG[base]++,base);
						strmfp(filename,MSGpath,file);
						TypeMessage(filename);
					}
					break;
				case 'N':
					while(USER(user)->LastMSG[base]<RPGinfo->NextMSG[base] && !cd()) {
						sprintf(text,"%s-=%s>%s*%s<%s=-",Fcol(GREEN),Fcol(YELLOW),Fcol(WHITE),Fcol(YELLOW),Fcol(GREEN));
						outs(text);
						normal();
						newline();
						newline();
						sprintf(file,"MSG%05u.B%02u",USER(user)->LastMSG[base]++,base);
						strmfp(filename,MSGpath,file);
						sprintf(text,"%sMsg#:%s %u",Fcol(CYN),Fcol(YELLOW),USER(user)->LastMSG[base]);
						outs(text);
						newline();
						TypeMessage(filename);
						sprintf(text,"%s<N>ext, <R>eply, <Q>uit: ",Fcol(CYN));
						outs(text);
						if(!(c=inkey()))
							break;
						newline();
						newline();
						if(c=='Q')
							break;
						if(c=='R' && acclvl) {
							outs(Fcol(CYN));
							outs("Private (Y/N)? ");
							if(!(c=inkey()))
								break;
							newline();
							c=toupper(ins[0]);
							if(c=='Y')
								Email(sender);
							else
								SendMessage(sender,base);
						}
					}
					if(USER(user)->LastMSG[base]==RPGinfo->NextMSG[base]) {
						outs("No more messages.");
						newline();
					}
					break;
				case 'P':
					while(USER(user)->LastMSG[base]>RPGinfo->FirstMSG[base] && !cd()) {
						sprintf(text,"%s-=%s>%s*%s<%s=-",Fcol(GREEN),Fcol(YELLOW),Fcol(WHITE),Fcol(YELLOW),Fcol(GREEN));
						outs(text);
						normal();
						newline();
						newline();
						sprintf(file,"MSG%05u.B%02u",--USER(user)->LastMSG[base],base);
						strmfp(filename,MSGpath,file);
						sprintf(text,"%sMsg#:%s %u",Fcol(CYN),Fcol(YELLOW),USER(user)->LastMSG[base]);
						outs(text);
						newline();
						TypeMessage(filename);
						sprintf(text,"%s<P>revious, <R>eply, <Q>uit: ",Fcol(CYN));
						outs(text);
						if(!(c=inkey()))
							break;
						newline();
						newline();
						if(c=='Q')
							break;
						if(c=='R' && acclvl) {
							outs(Fcol(CYN));
							outs("Private (Y/N)? ");
							if(!(c=inkey()))
								break;
							newline();
							if(c=='Y')
								Email(sender);
							else
								SendMessage(sender,base);
						}
					}
					if(USER(user)->LastMSG[base]==RPGinfo->FirstMSG[base]) {
						outs("No previous message.");
						newline();
					}
					break;
			}
		}
	}
}
