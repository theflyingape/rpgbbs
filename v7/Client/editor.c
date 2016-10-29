#include "/include/all.h"
#include "global.h"

void Editor(struct RPC *rpc,int maxline)
{
	int i,l,l1,l2,loop;
	char c[2]={ '\0','\0' };

	sprintf(outbuf,"%sDate:%s ",fore(CYN),fore(YELLOW));
	OUT(outbuf);
	fdate(outbuf,Julian);
	OUT(outbuf);
	OUT(" ");
	ftime(outbuf,Time);
	OUT(outbuf);NL;
	if(rpc) {
		sprintf(outbuf,"%s  To:%s %s",fore(CYN),fore(YELLOW),(strlen(rpc->user.Handle) ? rpc->user.Handle : "All"));
		OUT(outbuf);NL;
	}
	sprintf(outbuf,"%sFrom:%s %s",fore(CYN),fore(YELLOW),PLAYER.Handle);
	OUT(outbuf);NL;NL;
	sprintf(outbuf,"%sEnter your message and type a %s%s /? %s%s on a blank line for options.",fore(CYN),back(BLU),fore(WHITE),back(BLK),fore(CYN));
	OUT(outbuf);

	for(l=0; l<maxline; l++)
		memset(line[l],0,128);
	memset(inbuf,0,INBUF_SIZE);
	l=0;numline=0;

	while(l<maxline) {
		NL;
		sprintf(prompt,"%s%2u%s]%s %s",fore(YELLOW),l+1,fore(BLU),fore(WHITE),line[l]);
		OUT(prompt);
		strcpy(inbuf,line[l]);
		if(l>numline)
			numline=l;
		loop=TRUE;
		while(loop) {
			if(!(c[0]=getkey())) {
				l=maxline;numline=0;
				loop=FALSE;
			}
			if(c[0]=='/' && !strlen(inbuf)) {
				do {
					SAMELINE;
					sprintf(prompt,"%sEd%s>%s ",fore(YELLOW),fore(BLU),fore(WHITE));
					OUT(prompt);CLL;
					switch((c[0]=inkey(NULL,'Q'))) {
						case 'D':
							OUT("elete from: ");
							ins(2);
							l1=atoi(inbuf);
							if(l1>0 && l1<=l) {
								OUT(" to: ");
								ins(2);
								NL;
								l2=atoi(inbuf);
								if(l2>=l1 && l2<=l) {
									for(i=0; l2+i<numline; i++) {
										strcpy(line[l1-1+i],line[l2+i]);
										memset(line[l2+i],0,128);
									}
									numline-=l2-l1+1;l=numline;
								}
							}
							break;
						case 'E':
							OUT("dit: ");
							ins(2);
							i=atoi(inbuf);
							if(i>0 && i<=l)
								l=i-1;
							break;
						case 'L':
							OUT("ist from [1]: ");
							ins(2);
							if(!strlen(inbuf))
								strcpy(inbuf,"1");
							l1=atoi(inbuf);
							if(l1>0 && l1<=l) {
								sprintf(prompt," to [%d]: ",numline);
								OUT(prompt);
								ins(2);
								NL;
								if(!strlen(inbuf))
									sprintf(inbuf,"%d",numline);
								l2=atoi(inbuf);
								if(l2>=l1 && l2<=l) {
									for(i=l1-1; i<l2; i++) {
										NL;
										strcpy(inbuf,line[i]);
										jazz(inbuf);
										sprintf(prompt,"%s%2u%s]%s %s",fore(YELLOW),i+1,fore(BLU),fore(WHITE),inbuf);
										OUT(prompt);
									}
									NL;
								}
							}
							break;
						case 'Q':
							OUT("uitting...");NL;
							l=maxline;numline=0;
							break;
						case 'S':
							OUT("aving...");NL;
							l=maxline;
							for(i=0; i<numline; i++) {
								strcpy(inbuf,line[i]);
								while(strlen(inbuf)>0 && inbuf[strlen(inbuf)-1]==' ')
									inbuf[strlen(inbuf)-1]=NULL;
								strcpy(line[i],inbuf);
								if(cuss(line[i])) {
									logoff=TRUE;
									numline=0;
								}
								jazz(line[i]);
							}
							while(numline>0 && !strlen(line[0])) {
								for(i=1; i<numline; i++)
									memcpy(line[i-1],line[i],128);
								memset(line[--numline],0,128);
							}
							while(numline>0 && !strlen(line[numline-1]))
								numline--;
							for(l1=0; l1<numline-1; l1++) {
								while(l1<numline-1 && !strlen(line[l1]) && !strlen(line[l1+1])) {
									for(l2=l1+1; l2<numline; l2++)
										memcpy(line[l2-1],line[l2],128);
									memset(line[--numline],0,128);
								}
							}
							break;
						default:
							RUBOUT;
							sprintf(outbuf,"%sD%s:elete  %sE%s:dit  %sL%s:ist  %sQ%s:uit  %sS%s:ave",fore(WHITE),fore(CYN),fore(WHITE),fore(CYN),fore(WHITE),fore(CYN),fore(WHITE),fore(CYN),fore(WHITE),fore(CYN));
							OUT(outbuf);NL;
							break;
					}
				} while(c[0] && !strchr("EQS",c[0]));
				break;
			}
			if(c[0]==8)
				c[0]=127;
			switch(c[0]) {
				case 13:
					while((i=strlen(inbuf))>0 && inbuf[i-1]==' ')
						inbuf[i-1]=NULL;
					if(strlen(inbuf) || (l>0 && strlen(line[l-1]))) {
						strcpy(line[l++],inbuf);
						memset(inbuf,0,INBUF_SIZE);
	 					loop=FALSE;
					}
					break;
				case 24:
					for(i=strlen(inbuf); i>0; i--)
						RUBOUT;
					inbuf[0]=NULL;
					break;
				case 127:
					if((i=strlen(inbuf))) {
						inbuf[i-1]=NULL;
						RUBOUT;
					}
					else
					if(l>0) {
						SAMELINE;CLL;
						OUT("\33[2A");
						strcpy(line[l--],inbuf);
						memset(inbuf,0,INBUF_SIZE);
						loop=FALSE;
					}
					break;
				default:
					if(c[0]>=' ') {
						strcat(inbuf,c);
						OUT(c);
					}
					if((i=strlen(inbuf))>70) {
						if(l<maxline) {
							for(; i>=0 && inbuf[i]!=' '; i--) {
								strcpy(line[l+1],&inbuf[i]);
								RUBOUT;
							}
						}
						else {
							beep();
							RUBOUT;
							i--;
						}
						inbuf[i]=NULL;
						if(l<maxline)
							strcpy(line[l++],inbuf);
						loop=FALSE;
					}
					break;
			}
		}
	}
}

int ReadMail(char *msg)
{
	FILE *fp;
	int i;
	char c;

	if((fp=fopen(msg,"r"))) {
		cls();
		finputs(inbuf,INBUF_SIZE,fp);
		sprintf(outbuf,"%sDate:%s %s",fore(CYN),fore(YELLOW),inbuf);
		OUT(outbuf);NL;
		finputs(ENEMY.ID,sizeof(ENEMY.ID),fp);
		RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
		sprintf(outbuf,"%s  To:%s %s",fore(CYN),fore(YELLOW),strlen(ENEMY.Handle) ? ENEMY.Handle : "All");
		OUT(outbuf);NL;
		finputs(ENEMY.ID,sizeof(ENEMY.ID),fp);
		RPGserver(SERVER_GETUSER,(UBYTE *)&ENEMY);
		sprintf(outbuf,"%sFrom:%s %s",fore(CYN),fore(YELLOW),ENEMY.Handle);
		OUT(outbuf);NL;
		NORMAL;NL;
		i=4;
		while(!feof(fp)) {
			finputs(inbuf,INBUF_SIZE,fp);
			OUT(inbuf);NL;
			if(!(++i%rows))
				if((c=more())=='N')
					break;
		}
		fclose(fp);
	}
	return(fp ? TRUE : FALSE);
}

void WriteMail(char *msg)
{
	FILE *fp;
	int i;

	if(numline) {
		fdate(dated,Julian);
		ftime(timed,Time);
		if((fp=fopen(msg,"w"))) {
			fprintf(fp,"%s %s\n",dated,timed);
			fprintf(fp,"%s\n",ENEMY.ID);
			fprintf(fp,"%s\n",PLAYER.ID);
			for(i=0; i<numline; i++)
				fprintf(fp,"%s\n",line[i]);
			fclose(fp);
		}
	}
}
