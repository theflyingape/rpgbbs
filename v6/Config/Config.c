#include <exec/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FOREVER for(;;)

char __stdiowin[]="CON:0/0/640/200/RPGBBS Config v1.2/";
char __stdiov37[]="/CLOSE";

struct Access {
	UBYTE Name[10];
	UBYTE Calls,Minutes,Level;
	UBYTE User,Bulletin,Dungeon,File,Naval,Party,Tavern,Sysop;
} *Access[20];

struct Class {
	UBYTE Name[10];
	BYTE Poison,Vial;						/*  -1=service,0=none,1=1x,2=2x  */
	BYTE Magic,ToCast,ToInt,ToHeal,ToBlast;	/*  0=none,1=wand,2=scroll,3=spell,4=super  */
	BYTE Backstab;							/*  0=none,2=2x,3=3x,etc.  */
	BYTE ToHit,DefInt,DefAgl;
	UBYTE Recommend[4];
	UBYTE Str,Int,Agl,Chr;
	UBYTE WC,AC,ToSteal;
	UBYTE Note[22];
} *Class[19];

struct Store {
	char Name[26];
	double Cost;
} *Armor[100],*Weapon[100],*RealEstate[16],*Security[16];

struct Extra {
	char Name[20];
	double Cost[2];
	UWORD Power[2];
} *Magic[16],*Poison[16];

struct AM {
	char Name[17];
	UBYTE Class,Level;
	UBYTE Str,Int,Agl,Chr;
	UWORD HP,SP;
	BYTE Weapon,Armor;
	UWORD Spell;
	DOUBLE Gold;
} *AM[16];

struct DM {
	char Name[17];
	UBYTE Class;
} *DM[100];

struct SM {
	char Name[17];
	UBYTE Int;
	UWORD Hull,Shot,Power;
	char Ram;
	double Gold;
} *SM[16];

FILE *PrinterFP;
UBYTE MaxAccess,MaxFILE,MaxMSG,MaxClass;
UBYTE MaxWeapon,MaxArmor,MaxRealEstate,MaxSecurity;
UBYTE MaxMagic,MaxPoison;
UBYTE MaxAM,MaxDM,MaxSM;
UBYTE FILEname[16][31],MSGname[16][31];
UWORD MaxMessage[16];
UBYTE EMAILpath[41],FILEpath[16][41],HELPpath[41],MSGpath[41],NOTESpath[41],ArenaPIXpath[41],DungeonPIXpath[41],PlayerPIXpath[41],UPLOADpath[41];
DOUBLE d;
ULONG ul;
LONG l;
UWORD uw;
WORD w;
UBYTE ub;
BYTE b,choice;
UBYTE ModFlag;
UBYTE buffer[512],text[80];

int OpenPrinter(void)
{
	if((PrinterFP=fopen("PRT:","w")))
		return(TRUE);
	return(FALSE);
}

void inputs(char *str,int n,FILE *fp)
{
	if(!fgets(str,n,fp))
		memset(str,0,n);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
}

void Init(void)
{
	FILE *fp;
	int i;

	MaxMSG=0;
	if((fp=fopen("RPGBBS.config","r"))) {
		inputs(EMAILpath,sizeof(EMAILpath),fp);
		inputs(HELPpath,sizeof(HELPpath),fp);
		inputs(MSGpath,sizeof(MSGpath),fp);
		inputs(NOTESpath,sizeof(NOTESpath),fp);
		inputs(ArenaPIXpath,sizeof(ArenaPIXpath),fp);
		inputs(DungeonPIXpath,sizeof(DungeonPIXpath),fp);
		inputs(PlayerPIXpath,sizeof(PlayerPIXpath),fp);
		inputs(UPLOADpath,sizeof(UPLOADpath),fp);
		fgets(text,sizeof(text),fp);
		MaxFILE=atoi(text);
		for(i=0; i<MaxFILE; i++) {
			inputs(FILEname[i],sizeof(FILEname[i]),fp);
			inputs(FILEpath[i],sizeof(FILEpath[i]),fp);
		}
		fgets(text,sizeof(text),fp);
		MaxMSG=atoi(text);
		for(i=0; i<MaxMSG; i++) {
			inputs(MSGname[i],sizeof(MSGname[i]),fp);
			fgets(text,sizeof(text),fp);
			MaxMessage[i]=atoi(text);
		}
		fclose(fp);
	}

	MaxAccess=0;
	for(i=0; i<20; i++)
		Access[i]=malloc(sizeof(struct Access));
	if((fp=fopen("RPGBBS.access","rb"))) {
		fread((char *)&MaxAccess,sizeof(MaxAccess),1,fp);
		for(i=0; i<MaxAccess; i++)
			fread((char *)Access[i],sizeof(struct Access),1,fp);
		fclose(fp);
	}

	MaxClass=0;
	for(i=0; i<19; i++)
		Class[i]=malloc(sizeof(struct Class));
	if((fp=fopen("RPGBBS.class","rb"))) {
		fread((char *)&MaxClass,sizeof(MaxClass),1,fp);
		for(i=0; i<MaxClass; i++)
			fread((char *)Class[i],sizeof(struct Class),1,fp);
		fclose(fp);
	}

	MaxArmor=0;
	for(i=0; i<100; i++)
		Armor[i]=malloc(sizeof(struct Store));
	if((fp=fopen("RPGBBS.armor","rb"))) {
		fread((char *)&MaxArmor,sizeof(MaxArmor),1,fp);
		for(i=0; i<MaxArmor; i++)
			fread((char *)Armor[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	MaxWeapon=0;
	for(i=0; i<100; i++)
		Weapon[i]=malloc(sizeof(struct Store));
	if((fp=fopen("RPGBBS.weapon","rb"))) {
		fread((char *)&MaxWeapon,sizeof(MaxWeapon),1,fp);
		for(i=0; i<MaxWeapon; i++)
			fread((char *)Weapon[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	MaxRealEstate=0;
	for(i=0; i<16; i++)
		RealEstate[i]=malloc(sizeof(struct Store));
	if((fp=fopen("RPGBBS.realestate","rb"))) {
		fread((char *)&MaxRealEstate,sizeof(MaxRealEstate),1,fp);
		for(i=0; i<MaxRealEstate; i++)
			fread((char *)RealEstate[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	MaxSecurity=0;
	for(i=0; i<16; i++)
		Security[i]=malloc(sizeof(struct Store));
	if((fp=fopen("RPGBBS.security","rb"))) {
		fread((char *)&MaxSecurity,sizeof(MaxSecurity),1,fp);
		for(i=0; i<MaxSecurity; i++)
			fread((char *)Security[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	MaxMagic=0;
	for(i=0; i<16; i++)
		Magic[i]=malloc(sizeof(struct Extra));
	if((fp=fopen("RPGBBS.magic","rb"))) {
		fread((char *)&MaxMagic,sizeof(MaxMagic),1,fp);
		for(i=0; i<MaxMagic; i++)
			fread((char *)Magic[i],sizeof(struct Extra),1,fp);
		fclose(fp);
	}

	MaxPoison=0;
	for(i=0; i<16; i++)
		Poison[i]=malloc(sizeof(struct Extra));
	if((fp=fopen("RPGBBS.poison","rb"))) {
		fread((char *)&MaxPoison,sizeof(MaxPoison),1,fp);
		for(i=0; i<MaxPoison; i++)
			fread((char *)Poison[i],sizeof(struct Extra),1,fp);
		fclose(fp);
	}

	MaxAM=0;
	for(i=0; i<16; i++)
		AM[i]=malloc(sizeof(struct AM));
	if((fp=fopen("RPGBBS.monster","rb"))) {
		fread((char *)&MaxAM,sizeof(MaxAM),1,fp);
		for(i=0; i<MaxAM; i++)
			fread((char *)AM[i],sizeof(struct AM),1,fp);
		fclose(fp);
	}

	MaxDM=0;
	for(i=0; i<100; i++)
		DM[i]=malloc(sizeof(struct DM));
	if((fp=fopen("RPGBBS.dungeon","rb"))) {
		fread((char *)&MaxDM,sizeof(MaxDM),1,fp);
		for(i=0; i<MaxDM; i++)
			fread((char *)DM[i],sizeof(struct DM),1,fp);
		fclose(fp);
	}

	MaxSM=0;
	for(i=0; i<16; i++)
		SM[i]=malloc(sizeof(struct SM));
	if((fp=fopen("RPGBBS.seamonster","rb"))) {
		fread((char *)&MaxSM,sizeof(MaxSM),1,fp);
		for(i=0; i<MaxSM; i++)
			fread((char *)SM[i],sizeof(struct SM),1,fp);
		fclose(fp);
	}

	ModFlag=FALSE;
}

void InputString(int max,char *def)
{
	do {
		printf("\n");
		if(strlen(def))
			printf("[%.*s]",max,def);
		printf("> ");
		if(!gets(buffer))
			break;
		if(!strlen(buffer))
			sprintf(buffer,"%.*s",max,def);
	} while(strlen(buffer)>max);
	printf("\n");
}

void InputDOUBLE(DOUBLE n)
{
	sprintf(text,"%.8g",n);
	InputString(12,text);
	d=atof(buffer);
}

void InputULONG(ULONG n)
{
	sprintf(text,"%lu",n);
	InputString(10,text);
	ul=atol(buffer);
}

void InputLONG(LONG n)
{
	sprintf(text,"%ld",n);
	InputString(10,text);
	l=atol(buffer);
}

void InputUWORD(UWORD n)
{
	sprintf(text,"%hu",n);
	InputString(5,text);
	uw=atoi(buffer);
}

void InputWORD(WORD n)
{
	sprintf(text,"%hd",n);
	InputString(5,text);
	w=atoi(buffer);
}

void InputUBYTE(UBYTE n)
{
	UWORD i;

	i=n;
	sprintf(text,"%hu",i);
	InputString(3,text);
	ub=atoi(buffer);
}

void InputBYTE(BYTE n)
{
	WORD i;

	i=n;
	sprintf(text,"%hd",i);
	InputString(3,text);
	b=atoi(buffer);
}

void InputCHAR(UBYTE n)
{
	if(!n)
		n=' ';
	sprintf(text,"%c",n);
	InputString(1,text);
	buffer[0]=toupper(buffer[0]);
}

void BBSFM(void)
{
	int i;

	printf("Email path (Max %d chars)",sizeof(EMAILpath)-1);
	InputString(sizeof(EMAILpath)-1,EMAILpath);
	strcpy(EMAILpath,buffer);
	printf("Help path (Max %d chars)",sizeof(HELPpath)-1);
	InputString(sizeof(HELPpath)-1,HELPpath);
	strcpy(HELPpath,buffer);
	printf("Bulletin Message path (Max %d chars)",sizeof(MSGpath)-1);
	InputString(sizeof(MSGpath)-1,MSGpath);
	strcpy(MSGpath,buffer);
	printf("Notes path (Max %d chars)",sizeof(NOTESpath)-1);
	InputString(sizeof(NOTESpath)-1,NOTESpath);
	strcpy(NOTESpath,buffer);
	printf("Arena pix path (Max %d chars)",sizeof(ArenaPIXpath)-1);
	InputString(sizeof(ArenaPIXpath)-1,ArenaPIXpath);
	strcpy(ArenaPIXpath,buffer);
	printf("Dungeon pix path (Max %d chars)",sizeof(DungeonPIXpath)-1);
	InputString(sizeof(DungeonPIXpath)-1,DungeonPIXpath);
	strcpy(DungeonPIXpath,buffer);
	printf("Player pix path (Max %d chars)",sizeof(PlayerPIXpath)-1);
	InputString(sizeof(PlayerPIXpath)-1,PlayerPIXpath);
	strcpy(PlayerPIXpath,buffer);
	printf("Uploads path (Max %d chars)",sizeof(UPLOADpath)-1);
	InputString(sizeof(UPLOADpath)-1,UPLOADpath);
	strcpy(UPLOADpath,buffer);
	printf("Maximum File Sections (1-16)");
	InputUBYTE(MaxFILE);
	MaxFILE=ub;
	for(i=0; i<MaxFILE; i++) {
		printf("File Section Name #%d (Max %d chars)",i+1,sizeof(FILEname[i])-1);
		InputString(sizeof(FILEname[i])-1,FILEname[i]);
		strcpy(FILEname[i],buffer);
		printf("File Section path (Max %d chars)",sizeof(FILEpath[i])-1);
		InputString(sizeof(FILEpath[i])-1,FILEpath[i]);
		strcpy(FILEpath[i],buffer);
	}
	printf("Maximum Message Bases (1-16)");
	InputUBYTE(MaxMSG);
	MaxMSG=ub;
	for(i=0; i<MaxMSG; i++) {
		printf("Message Base Name #%d (Max %d chars)",i+1,sizeof(MSGname[i])-1);
		InputString(sizeof(MSGname[i])-1,MSGname[i]);
		strcpy(MSGname[i],buffer);
		printf("Maximum messages to hold (1-65535)");
		InputUWORD(MaxMessage[i]);
		MaxMessage[i]=uw;
	}
	ModFlag=TRUE;
}

void EditAccess(int sub)
{
	printf("Name (Max 9 chars)");
	InputString(9,Access[sub]->Name);
	strcpy(Access[sub]->Name,buffer);
	printf("Calls (0-255)");
	InputUBYTE(Access[sub]->Calls);
	Access[sub]->Calls=ub;
	printf("Minutes (0-255)");
	InputUBYTE(Access[sub]->Minutes);
	Access[sub]->Minutes=ub;
	printf("Promote to next Access upon reaching level (0-100)");
	InputUBYTE(Access[sub]->Level);
	Access[sub]->Level=ub;
	printf("Allow User brawl, fight, joust, naval battle, party, rob (Y/N)");
	InputCHAR(Access[sub]->User);
	Access[sub]->User=buffer[0];
	printf("Allow Bulletin access (Y/N)");
	InputCHAR(Access[sub]->Bulletin);
	Access[sub]->Bulletin=buffer[0];
	printf("Allow Dungeon access (Y/N)");
	InputCHAR(Access[sub]->Dungeon);
	Access[sub]->Dungeon=buffer[0];
	printf("Allow File Section access (Y/N)");
	InputCHAR(Access[sub]->File);
	Access[sub]->File=buffer[0];
	printf("Allow Naval access (Y/N)");
	InputCHAR(Access[sub]->Naval);
	Access[sub]->Naval=buffer[0];
	printf("Allow Party access (Y/N)");
	InputCHAR(Access[sub]->Party);
	Access[sub]->Party=buffer[0];
	printf("Allow Tavern access (Y/N)");
	InputCHAR(Access[sub]->Tavern);
	Access[sub]->Tavern=buffer[0];
	printf("Allow Sysop access (Y/N)");
	InputCHAR(Access[sub]->Sysop);
	Access[sub]->Sysop=buffer[0];
	ModFlag=TRUE;
}

void AccessFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.   Access    Call  Min  Lvl  U B D F N P T S\n");
		printf("---  ---------  ----  ---  ---  - - - - - - - -\n");
		for(sub=0; sub<MaxAccess; sub++)
			printf("%2d.  %-9.9s  %4u  %3u  %3u  %c %c %c %c %c %c %c %c\n",sub,Access[sub]->Name,Access[sub]->Calls,Access[sub]->Minutes,Access[sub]->Level,Access[sub]->User,Access[sub]->Bulletin,Access[sub]->Dungeon,Access[sub]->File,Access[sub]->Naval,Access[sub]->Party,Access[sub]->Tavern,Access[sub]->Sysop);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxAccess==20) {
					printf("Maximum of 20 reached.\n");
					break;
				}
				printf("Add Access # (0-%u)",MaxAccess);
				InputUBYTE(MaxAccess);
				sub=ub;
				if(sub<0 || sub>MaxAccess)
					break;
				MaxAccess++;
				for(i=MaxAccess; i>=sub; i--)
					memcpy(Access[i],Access[i-1],sizeof(struct Access));
				memset(Access[sub],0,sizeof(struct Access));
				EditAccess(sub);
				break;
			case 'C':
				if(!MaxAccess) {
					printf("None defined.");
					break;
				}
				printf("Change Access # (0-%u): ",MaxAccess-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxAccess-1)
					break;
				EditAccess(sub);
				break;
			case 'D':
				if(!MaxAccess) {
					printf("None defined.");
					break;
				}
				printf("Delete Access # (0-%u): ",MaxAccess-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxAccess-1)
					break;
				MaxAccess--;
				ModFlag=TRUE;
				for(i=sub; i<MaxAccess; i++)
					memcpy(Access[i],Access[i+1],sizeof(struct Access));
				memset(Access[MaxAccess],0,sizeof(struct Access));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.   Access    Call  Min  Lvl  U B D F N P T S\n");
					fprintf(PrinterFP,"---  ---------  ----  ---  ---  - - - - - - - -\n");
					for(sub=0; sub<MaxAccess; sub++)
						fprintf(PrinterFP,"%2d.  %-9.9s  %4u  %3u  %3u  %c %c %c %c %c %c %c %c\n",sub,Access[sub]->Name,Access[sub]->Calls,Access[sub]->Minutes,Access[sub]->Level,Access[sub]->User,Access[sub]->Bulletin,Access[sub]->Dungeon,Access[sub]->File,Access[sub]->Naval,Access[sub]->Party,Access[sub]->Tavern,Access[sub]->Sysop);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditClass(int sub)
{
	printf("Name (Max 9 chars)");
	InputString(9,Class[sub]->Name);
	strcpy(Class[sub]->Name,buffer);
	printf("Poison (-1=service, 0=none, 1=1x, 2=2x, etc.)");
	InputBYTE(Class[sub]->Poison);
	Class[sub]->Poison=b;
	printf("Empty Vial frequency (1-100)");
	InputBYTE(Class[sub]->Vial);
	Class[sub]->Vial=b;
	printf("Magic (0=none, 1=wand, 2=scroll, 3=spell, 4=super)");
	InputBYTE(Class[sub]->Magic);
	Class[sub]->Magic=b;
	printf("Cast spell frequency (2-5)");
	InputBYTE(Class[sub]->ToCast);
	Class[sub]->ToCast=b;
	printf("Intellect bonus (-10 to 10)");
	InputBYTE(Class[sub]->ToInt);
	Class[sub]->ToInt=b;
	printf("Healing ability (15-20)");
	InputBYTE(Class[sub]->ToHeal);
	Class[sub]->ToHeal=b;
	printf("Blasting ability (15-20)");
	InputBYTE(Class[sub]->ToBlast);
	Class[sub]->ToBlast=b;
	printf("Backstab (0=none, 2=2x, 3=3x, 4x)");
	InputBYTE(Class[sub]->Backstab);
	Class[sub]->Backstab=b;
	printf("Hit bonus multiplier (0=0x, 1=1x, 2=2x to 0x, 3=3x, 4=4x) ");
	InputBYTE(Class[sub]->ToHit);
	Class[sub]->ToHit=b;
	printf("Intellect saving throw (-10 to 10)");
	InputBYTE(Class[sub]->DefInt);
	Class[sub]->DefInt=b;
	printf("Agility saving throw (-10 to 10)");
	InputBYTE(Class[sub]->DefAgl);
	Class[sub]->DefAgl=b;
	printf("Strength bonus (1-5)");
	InputUBYTE(Class[sub]->Str);
	Class[sub]->Str=ub;
	printf("Intellect bonus (1-5)");
	InputUBYTE(Class[sub]->Int);
	Class[sub]->Int=ub;
	printf("Agility bonus (1-5)");
	InputUBYTE(Class[sub]->Agl);
	Class[sub]->Agl=ub;
	printf("Charisma bonus (1-5)");
	InputUBYTE(Class[sub]->Chr);
	Class[sub]->Chr=ub;
	printf("Stealing ability (1-3)");
	InputUBYTE(Class[sub]->ToSteal);
	Class[sub]->ToSteal=ub;
	printf("Recommended Str (20-80)");
	InputUBYTE(Class[sub]->Recommend[0]);
	Class[sub]->Recommend[0]=ub;
	printf("Recommended Int (20-80)");
	InputUBYTE(Class[sub]->Recommend[1]);
	Class[sub]->Recommend[1]=ub;
	printf("Recommended Agl (20-80)");
	InputUBYTE(Class[sub]->Recommend[2]);
	Class[sub]->Recommend[2]=ub;
	printf("Recommended Chr (20-80)");
	InputUBYTE(Class[sub]->Recommend[3]);
	Class[sub]->Recommend[3]=ub;
	printf("Maximum Weapon Class (1-%d)",MaxWeapon-1);
	InputUBYTE(Class[sub]->WC);
	Class[sub]->WC=ub;
	printf("Maximum Armor Class (1-%d)",MaxArmor-1);
	InputUBYTE(Class[sub]->AC);
	Class[sub]->AC=ub;
	printf("Special Feature Note (max 21 chars)");
	InputString(21,Class[sub]->Note);
	strcpy(Class[sub]->Note,buffer);

	ModFlag=TRUE;
}

void ClassFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.    Class    Poison  Magic Int  H  B  BS  Hit  Int  Agl  S I A C  Rob\n");
		printf("---  ---------  --- --   - -- --- -- --  --  ---  ---  ---  - - - -  ---\n");
		for(sub=0; sub<MaxClass; sub++)
			printf("%2d.  %-9.9s  %2hdx %hdx   %hd %hdx %3hd %2hd %2hd  %hdx  %2hdx  %3hd  %3hd  %1hu %1hu %1hu %1hu  %2hux\n",sub,Class[sub]->Name,Class[sub]->Poison,Class[sub]->Vial,Class[sub]->Magic,Class[sub]->ToCast,Class[sub]->ToInt,Class[sub]->ToHeal,Class[sub]->ToBlast,Class[sub]->Backstab,Class[sub]->ToHit,Class[sub]->DefInt,Class[sub]->DefAgl,Class[sub]->Str,Class[sub]->Int,Class[sub]->Agl,Class[sub]->Chr,Class[sub]->ToSteal);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxClass==19) {
					printf("Maximum of 19 reached.\n");
					break;
				}
				printf("Add Class # (0-%u)",MaxClass);
				InputUBYTE(MaxClass);
				sub=ub;
				if(sub<0 || sub>MaxClass)
					break;
				MaxClass++;
				for(i=MaxClass; i>=sub; i--)
					memcpy(Class[i],Class[i-1],sizeof(struct Class));
				memset(Class[sub],0,sizeof(struct Class));
				EditClass(sub);
				break;
			case 'C':
				if(!MaxClass) {
					printf("None defined.");
					break;
				}
				printf("Change Class # (0-%u): ",MaxClass-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxClass-1)
					break;
				EditClass(sub);
				break;
			case 'D':
				if(!MaxClass) {
					printf("None defined.");
					break;
				}
				printf("Delete Class # (0-%u): ",MaxClass-4);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxClass-4)
					break;
				MaxClass--;
				ModFlag=TRUE;
				for(i=sub; i<MaxClass; i++)
					memcpy(Class[i],Class[i+1],sizeof(struct Class));
				memset(Class[MaxClass],0,sizeof(struct Class));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.    Class    Poison  Magic Int  H  B  BS  Hit  Int  Agl  S I A C  Rob\n");
					fprintf(PrinterFP,"---  ---------  --- --   - -- --- -- --  --  ---  ---  ---  - - - -  ---\n");
					for(sub=0; sub<MaxClass; sub++)
						fprintf(PrinterFP,"%2d.  %-9.9s  %2hdx %hdx   %hd %hdx %3hd %2hd %2hd  %hdx  %2hdx  %3hd  %3hd  %1hu %1hu %1hu %1hu  %2hux\n",sub,Class[sub]->Name,Class[sub]->Poison,Class[sub]->Vial,Class[sub]->Magic,Class[sub]->ToCast,Class[sub]->ToInt,Class[sub]->ToHeal,Class[sub]->ToBlast,Class[sub]->Backstab,Class[sub]->ToHit,Class[sub]->DefInt,Class[sub]->DefAgl,Class[sub]->Str,Class[sub]->Int,Class[sub]->Agl,Class[sub]->Chr,Class[sub]->ToSteal);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditArmor(int sub)
{
	printf("Name (Max 25 chars)");
	InputString(25,Armor[sub]->Name);
	strcpy(Armor[sub]->Name,buffer);
	printf("Cost");
	InputDOUBLE(Armor[sub]->Cost);
	Armor[sub]->Cost=d;

	ModFlag=TRUE;
}

void ArmorFM(void)
{
	int i,sub,top;

	top=0;
	FOREVER {
		printf("\n");
		printf("No.  Armor Class Name             Cost\n");
		printf("---  -------------------------  --------\n");
		for(sub=top; sub<top+10 && sub<MaxArmor; sub++)
			printf("%2d.  %-25.25s  %8.8g\n",sub,Armor[sub]->Name,Armor[sub]->Cost);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <N>ext, <P>rev, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxArmor==100) {
					printf("Maximum of 100 reached.\n");
					break;
				}
				printf("Add Armor # (0-%u)",MaxArmor);
				InputUBYTE(MaxArmor);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxArmor)
					break;
				MaxArmor++;
				for(i=MaxArmor; i>=sub; i--)
					memcpy(Armor[i],Armor[i-1],sizeof(struct Store));
				memset(Armor[sub],0,sizeof(struct Store));
				EditArmor(sub);
				break;
			case 'C':
				if(!MaxArmor) {
					printf("None defined.");
					break;
				}
				printf("Change Armor # (0-%u): ",MaxArmor-1);
				InputUBYTE(0);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxArmor-1)
					break;
				EditArmor(sub);
				break;
			case 'D':
				if(!MaxArmor) {
					printf("None defined.");
					break;
				}
				printf("Delete Armor # (0-%u): ",MaxArmor-1);
				InputUBYTE(0);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxArmor-1)
					break;
				MaxArmor--;
				ModFlag=TRUE;
				for(i=sub; i<MaxArmor; i++)
					memcpy(Armor[i],Armor[i+1],sizeof(struct Store));
				memset(Armor[MaxArmor],0,sizeof(struct Store));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Armor Class Name             Cost\n");
					fprintf(PrinterFP,"---  -------------------------  --------\n");
					for(sub=0; sub<MaxArmor; sub++)
						fprintf(PrinterFP,"%2d.  %-25.25s  %8.8g\n",sub,Armor[sub]->Name,Armor[sub]->Cost);
					fclose(PrinterFP);
				}
				break;
			case 'N':
				if(top+10<MaxArmor)
					top+=10;
				break;
			case 'P':
				if(top)
					top-=10;
				break;
		}
	}
}

void EditWeapon(int sub)
{
	printf("Name (Max 25 chars)");
	InputString(25,Weapon[sub]->Name);
	strcpy(Weapon[sub]->Name,buffer);
	printf("Cost");
	InputDOUBLE(Weapon[sub]->Cost);
	Weapon[sub]->Cost=d;

	ModFlag=TRUE;
}

void WeaponFM(void)
{
	int i,sub,top;

	top=0;
	FOREVER {
		printf("\n");
		printf("No.  Weapon Class Name            Cost\n");
		printf("---  -------------------------  --------\n");
		for(sub=top; sub<top+10 && sub<MaxWeapon; sub++)
			printf("%2d.  %-25.25s  %8.8g\n",sub,Weapon[sub]->Name,Weapon[sub]->Cost);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <N>ext, <P>rev, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxWeapon==100) {
					printf("Maximum of 100 reached.\n");
					break;
				}
				printf("Add Weapon # (0-%u)",MaxWeapon);
				InputUBYTE(MaxWeapon);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxWeapon)
					break;
				MaxWeapon++;
				for(i=MaxWeapon; i>=sub; i--)
					memcpy(Weapon[i],Weapon[i-1],sizeof(struct Store));
				memset(Weapon[sub],0,sizeof(struct Store));
				EditWeapon(sub);
				break;
			case 'C':
				if(!MaxWeapon) {
					printf("None defined.");
					break;
				}
				printf("Change Weapon # (0-%u): ",MaxWeapon-1);
				InputUBYTE(0);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxWeapon-1)
					break;
				EditWeapon(sub);
				break;
			case 'D':
				if(!MaxWeapon) {
					printf("None defined.");
					break;
				}
				printf("Delete Weapon # (0-%u): ",MaxWeapon-1);
				InputUBYTE(0);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxWeapon-1)
					break;
				MaxWeapon--;
				ModFlag=TRUE;
				for(i=sub; i<MaxWeapon; i++)
					memcpy(Weapon[i],Weapon[i+1],sizeof(struct Store));
				memset(Weapon[MaxWeapon],0,sizeof(struct Store));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Weapon Class Name            Cost\n");
					fprintf(PrinterFP,"---  -------------------------  --------\n");
					for(sub=0; sub<MaxWeapon; sub++)
						fprintf(PrinterFP,"%2d.  %-25.25s  %8.8g\n",sub,Weapon[sub]->Name,Weapon[sub]->Cost);
					fclose(PrinterFP);
				}
				break;
			case 'N':
				if(top+10<MaxWeapon)
					top+=10;
				break;
			case 'P':
				if(top)
					top-=10;
				break;
		}
	}
}

void EditRealEstate(int sub)
{
	printf("Name (Max 25 chars)");
	InputString(25,RealEstate[sub]->Name);
	strcpy(RealEstate[sub]->Name,buffer);
	printf("Cost");
	InputDOUBLE(RealEstate[sub]->Cost);
	RealEstate[sub]->Cost=d;

	ModFlag=TRUE;
}

void RealEstateFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.  Real Estate Name             Cost\n");
		printf("---  -------------------------  --------\n");
		for(sub=0; sub<MaxRealEstate; sub++)
			printf("%2d.  %-25.25s  %8.8g\n",sub,RealEstate[sub]->Name,RealEstate[sub]->Cost);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxRealEstate==16) {
					printf("Maximum of 16 reached.\n");
					break;
				}
				printf("Add Real Estate # (0-%u)",MaxRealEstate);
				InputUBYTE(MaxRealEstate);
				sub=ub;
				if(sub<0 || sub>MaxRealEstate)
					break;
				MaxRealEstate++;
				for(i=MaxRealEstate; i>=sub; i--)
					memcpy(RealEstate[i],RealEstate[i-1],sizeof(struct Store));
				memset(RealEstate[sub],0,sizeof(struct Store));
				EditRealEstate(sub);
				break;
			case 'C':
				if(!MaxRealEstate) {
					printf("None defined.");
					break;
				}
				printf("Change RealEstate # (0-%u): ",MaxRealEstate-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxRealEstate-1)
					break;
				EditRealEstate(sub);
				break;
			case 'D':
				if(!MaxRealEstate) {
					printf("None defined.");
					break;
				}
				printf("Delete RealEstate # (0-%u): ",MaxRealEstate-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxRealEstate-1)
					break;
				MaxRealEstate--;
				ModFlag=TRUE;
				for(i=sub; i<MaxRealEstate; i++)
					memcpy(RealEstate[i],RealEstate[i+1],sizeof(struct Store));
				memset(RealEstate[MaxRealEstate],0,sizeof(struct Store));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Real Estate Name             Cost\n");
					fprintf(PrinterFP,"---  -------------------------  --------\n");
					for(sub=0; sub<MaxRealEstate; sub++)
						fprintf(PrinterFP,"%2d.  %-25.25s  %8.8g\n",sub,RealEstate[sub]->Name,RealEstate[sub]->Cost);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditSecurity(int sub)
{
	printf("Name (Max 25 chars)");
	InputString(25,Security[sub]->Name);
	strcpy(Security[sub]->Name,buffer);
	printf("Cost");
	InputDOUBLE(Security[sub]->Cost);
	Security[sub]->Cost=d;

	ModFlag=TRUE;
}

void SecurityFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.  Security Name                Cost\n");
		printf("---  -------------------------  --------\n");
		for(sub=0; sub<MaxSecurity; sub++)
			printf("%2d.  %-25.25s  %8.8g\n",sub,Security[sub]->Name,Security[sub]->Cost);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxSecurity==16) {
					printf("Maximum of 16 reached.\n");
					break;
				}
				printf("Add Security # (0-%u)",MaxSecurity);
				InputUBYTE(MaxSecurity);
				sub=ub;
				if(sub<0 || sub>MaxSecurity)
					break;
				MaxSecurity++;
				for(i=MaxSecurity; i>=sub; i--)
					memcpy(Security[i],Security[i-1],sizeof(struct Store));
				memset(Security[sub],0,sizeof(struct Store));
				EditSecurity(sub);
				break;
			case 'C':
				if(!MaxSecurity) {
					printf("None defined.");
					break;
				}
				printf("Change Security # (0-%u): ",MaxSecurity-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxSecurity-1)
					break;
				EditSecurity(sub);
				break;
			case 'D':
				if(!MaxSecurity) {
					printf("None defined.");
					break;
				}
				printf("Delete Security # (0-%u): ",MaxSecurity-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxSecurity-1)
					break;
				MaxSecurity--;
				ModFlag=TRUE;
				for(i=sub; i<MaxSecurity; i++)
					memcpy(Security[i],Security[i+1],sizeof(struct Store));
				memset(Security[MaxSecurity],0,sizeof(struct Store));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Security Name                Cost\n");
					fprintf(PrinterFP,"---  -------------------------  --------\n");
					for(sub=0; sub<MaxSecurity; sub++)
						fprintf(PrinterFP,"%2d.  %-25.25s  %8.8g\n",sub,Security[sub]->Name,Security[sub]->Cost);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditMagic(int sub)
{
	printf("Name (Max 15 chars)");
	InputString(15,Magic[sub]->Name);
	strcpy(Magic[sub]->Name,buffer);
	printf("Spell Cost");
	InputDOUBLE(Magic[sub]->Cost[0]);
	Magic[sub]->Cost[0]=d;
	printf("Wand Cost");
	InputDOUBLE(Magic[sub]->Cost[1]);
	Magic[sub]->Cost[1]=d;
	printf("Spell Power");
	InputUWORD(Magic[sub]->Power[0]);
	Magic[sub]->Power[0]=uw;
	printf("Super-User Spell Power");
	InputUWORD(Magic[sub]->Power[1]);
	Magic[sub]->Power[1]=uw;

	ModFlag=TRUE;
}

void MagicFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.  Magic Name         Cost      Cost    Power  Power\n");
		printf("---  ---------------  --------  --------  -----  -----\n");
		for(sub=0; sub<MaxMagic; sub++)
			printf("%2d.  %-15.15s  %8.8g  %8.8g  %5hu  %5hu\n",sub,Magic[sub]->Name,Magic[sub]->Cost[0],Magic[sub]->Cost[1],Magic[sub]->Power[0],Magic[sub]->Power[1]);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxMagic==16) {
					printf("Maximum of 16 reached.\n");
					break;
				}
				printf("Add Magic # (0-%u)",MaxMagic);
				InputUBYTE(MaxMagic);
				sub=ub;
				if(sub<0 || sub>MaxMagic)
					break;
				MaxMagic++;
				for(i=MaxMagic; i>=sub; i--)
					memcpy(Magic[i],Magic[i-1],sizeof(struct Extra));
				memset(Magic[sub],0,sizeof(struct Extra));
				EditMagic(sub);
				break;
			case 'C':
				if(!MaxMagic) {
					printf("None defined.");
					break;
				}
				printf("Change Magic # (0-%u): ",MaxMagic-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxMagic-1)
					break;
				EditMagic(sub);
				break;
			case 'D':
				if(!MaxMagic) {
					printf("None defined.");
					break;
				}
				printf("Delete Magic # (0-%u): ",MaxMagic-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxMagic-1)
					break;
				MaxMagic--;
				ModFlag=TRUE;
				for(i=sub; i<MaxMagic; i++)
					memcpy(Magic[i],Magic[i+1],sizeof(struct Extra));
				memset(Magic[MaxMagic],0,sizeof(struct Extra));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Magic Name         Cost      Cost    Power  Power\n");
					fprintf(PrinterFP,"---  ---------------  --------  --------  -----  -----\n");
					for(sub=0; sub<MaxMagic; sub++)
						fprintf(PrinterFP,"%2d.  %-15.15s  %8.8g  %8.8g  %5hu  %5hu\n",sub,Magic[sub]->Name,Magic[sub]->Cost[0],Magic[sub]->Cost[1],Magic[sub]->Power[0],Magic[sub]->Power[1]);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditPoison(int sub)
{
	printf("Name (Max 19 chars)");
	InputString(19,Poison[sub]->Name);
	strcpy(Poison[sub]->Name,buffer);
	printf("Poison User Cost");
	InputDOUBLE(Poison[sub]->Cost[0]);
	Poison[sub]->Cost[0]=d;
	printf("Poison Service Cost");
	InputDOUBLE(Poison[sub]->Cost[1]);
	Poison[sub]->Cost[1]=d;
	printf("Poison User Power");
	InputUWORD(Poison[sub]->Power[0]);
	Poison[sub]->Power[0]=uw;
	printf("Poison Service Power");
	InputUWORD(Poison[sub]->Power[1]);
	Poison[sub]->Power[1]=uw;

	ModFlag=TRUE;
}

void PoisonFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.      Poison Name        Cost      Cost    Power  Power\n");
		printf("---  -------------------  --------  --------  -----  -----\n");
		for(sub=0; sub<MaxPoison; sub++)
			printf("%2d.  %-19.19s  %8.8g  %8.8g  %5hu  %5hu\n",sub,Poison[sub]->Name,Poison[sub]->Cost[0],Poison[sub]->Cost[1],Poison[sub]->Power[0],Poison[sub]->Power[1]);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxPoison==16) {
					printf("Maximum of 16 reached.\n");
					break;
				}
				printf("Add Poison # (0-%u)",MaxPoison);
				InputUBYTE(MaxPoison);
				sub=ub;
				if(sub<0 || sub>MaxPoison)
					break;
				MaxPoison++;
				for(i=MaxPoison; i>=sub; i--)
					memcpy(Poison[i],Poison[i-1],sizeof(struct Extra));
				memset(Poison[sub],0,sizeof(struct Extra));
				EditPoison(sub);
				break;
			case 'C':
				if(!MaxPoison) {
					printf("None defined.");
					break;
				}
				printf("Change Poison # (0-%u): ",MaxPoison-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxPoison-1)
					break;
				EditPoison(sub);
				break;
			case 'D':
				if(!MaxPoison) {
					printf("None defined.");
					break;
				}
				printf("Delete Poison # (0-%u): ",MaxPoison-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxPoison-1)
					break;
				MaxPoison--;
				ModFlag=TRUE;
				for(i=sub; i<MaxPoison; i++)
					memcpy(Poison[i],Poison[i+1],sizeof(struct Extra));
				memset(Poison[MaxPoison],0,sizeof(struct Extra));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.      Poison Name        Cost      Cost    Power  Power\n");
					fprintf(PrinterFP,"---  -------------------  --------  --------  -----  -----\n");
					for(sub=0; sub<MaxPoison; sub++)
						fprintf(PrinterFP,"%2d.  %-19.19s  %8.8g  %8.8g  %5hu  %5hu\n",sub,Poison[sub]->Name,Poison[sub]->Cost[0],Poison[sub]->Cost[1],Poison[sub]->Power[0],Poison[sub]->Power[1]);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditAM(int sub)
{
	int i;

	printf("Name (Max 16 chars)");
	InputString(16,AM[sub]->Name);
	strcpy(AM[sub]->Name,buffer);
	for(i=0; i<MaxClass; i++)
		printf("%2d.  %s\n",i,Class[i]->Name);
	printf("Class (0-%hu)",MaxClass-1);
	InputUBYTE(AM[sub]->Class);
	AM[sub]->Class=ub;
	printf("Level (1-99)");
	InputUBYTE(AM[sub]->Level);
	AM[sub]->Level=ub;
	printf("Strength (20-99)");
	InputUBYTE(AM[sub]->Str);
	AM[sub]->Str=ub;
	printf("Intellect (20-99)");
	InputUBYTE(AM[sub]->Int);
	AM[sub]->Int=ub;
	printf("Agility (20-99)");
	InputUBYTE(AM[sub]->Agl);
	AM[sub]->Agl=ub;
	printf("Hit Points (1-9999)");
	InputUWORD(AM[sub]->HP);
	AM[sub]->HP=uw;
	printf("Spell Points (1-9999)");
	InputUWORD(AM[sub]->SP);
	AM[sub]->SP=uw;
	printf("Weapon Class (0-%hd, negative values yield Natural Weapon Class)",MaxWeapon-1);
	InputBYTE(AM[sub]->Weapon);
	AM[sub]->Weapon=b;
	printf("Armor Class (0-%hd, negative values yield Natural Armor Class)",MaxArmor-1);
	InputBYTE(AM[sub]->Armor);
	AM[sub]->Armor=b;
	printf("Spells (0-65535)");
	InputUWORD(AM[sub]->Spell);
	AM[sub]->Spell=uw;
	printf("Gold pieces");
	InputDOUBLE(AM[sub]->Gold);
	AM[sub]->Gold=d;

	ModFlag=TRUE;
}

void AMFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.  Monster Name       Class   Lv  S  I  A  HP   SP   WC  AC Spell   Gold  \n");
		printf("---  ---------------- --------- -- -- -- -- ---- ---- --- --- ----- --------\n");
		for(sub=0; sub<MaxAM; sub++)
			printf("%2d.  %-16.16s %-9.9s %2hu %2hu %2hu %2hu %4hu %4hu %3hd %3hd %5hu %8.8g\n",sub,AM[sub]->Name,Class[AM[sub]->Class]->Name,AM[sub]->Level,AM[sub]->Str,AM[sub]->Int,AM[sub]->Agl,AM[sub]->HP,AM[sub]->SP,AM[sub]->Weapon,AM[sub]->Armor,AM[sub]->Spell,AM[sub]->Gold);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxAM==16) {
					printf("Maximum of 16 reached.\n");
					break;
				}
				printf("Add Arena Monster # (0-%u)",MaxAM);
				InputUBYTE(MaxAM);
				sub=ub;
				if(sub<0 || sub>MaxAM)
					break;
				MaxAM++;
				for(i=MaxAM; i>=sub; i--)
					memcpy(AM[i],AM[i-1],sizeof(struct AM));
				memset(AM[sub],0,sizeof(struct AM));
				EditAM(sub);
				break;
			case 'C':
				if(!MaxAM) {
					printf("None defined.");
					break;
				}
				printf("Change Arena Monster # (0-%u): ",MaxAM-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxAM-1)
					break;
				EditAM(sub);
				break;
			case 'D':
				if(!MaxAM) {
					printf("None defined.");
					break;
				}
				printf("Delete Arena Monster # (0-%u): ",MaxAM-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxAM-1)
					break;
				MaxAM--;
				ModFlag=TRUE;
				for(i=sub; i<MaxAM; i++)
					memcpy(AM[i],AM[i+1],sizeof(struct AM));
				memset(AM[MaxAM],0,sizeof(struct AM));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Monster Name       Class   Lv  S  I  A  HP   SP   WC  AC Spell   Gold  \n");
					fprintf(PrinterFP,"---  ---------------- --------- -- -- -- -- ---- ---- --- --- ----- --------\n");
					for(sub=0; sub<MaxAM; sub++)
						fprintf(PrinterFP,"%2d.  %-16.16s %-9.9s %2hu %2hu %2hu %2hu %4hu %4hu %3hd %3hd %5hu %8.8g\n",sub,AM[sub]->Name,Class[AM[sub]->Class]->Name,AM[sub]->Level,AM[sub]->Str,AM[sub]->Int,AM[sub]->Agl,AM[sub]->HP,AM[sub]->SP,AM[sub]->Weapon,AM[sub]->Armor,AM[sub]->Spell,AM[sub]->Gold);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void EditDM(int sub)
{
	int i;

	printf("Name (Max 16 chars)");
	InputString(16,DM[sub]->Name);
	strcpy(DM[sub]->Name,buffer);
	for(i=0; i<MaxClass; i++)
		printf("%2d.  %s\n",i,Class[i]->Name);
	printf("Class (0-%hu)",MaxClass-1);
	InputUBYTE(DM[sub]->Class);
	DM[sub]->Class=ub;

	ModFlag=TRUE;
}

void DMFM(void)
{
	int i,sub,top;

	top=0;
	FOREVER {
		printf("\n");
		printf("No.  Monster Name        Class  \n");
		printf("---  ----------------  ---------\n");
		for(sub=top; sub<top+10 && sub<MaxDM; sub++)
			printf("%2d.  %-16.16s  %-9.9s\n",sub,DM[sub]->Name,Class[DM[sub]->Class]->Name);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <N>ext, <P>rev, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxDM==100) {
					printf("Maximum of 100 reached.\n");
					break;
				}
				printf("Add Dungeon Monster # (0-%u)",MaxDM);
				InputUBYTE(MaxDM);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxDM)
					break;
				MaxDM++;
				for(i=MaxDM; i>=sub; i--)
					memcpy(DM[i],DM[i-1],sizeof(struct DM));
				memset(DM[sub],0,sizeof(struct DM));
				EditDM(sub);
				break;
			case 'C':
				if(!MaxDM) {
					printf("None defined.");
					break;
				}
				printf("Change Dungeon Monster # (0-%u): ",MaxDM-1);
				InputUBYTE(0);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxDM-1)
					break;
				EditDM(sub);
				break;
			case 'D':
				if(!MaxDM) {
					printf("None defined.");
					break;
				}
				printf("Delete Dungeon Monster # (0-%u): ",MaxDM-1);
				InputUBYTE(0);
				sub=ub;
				top=sub/10*10;
				if(sub<0 || sub>MaxDM-1)
					break;
				MaxDM--;
				ModFlag=TRUE;
				for(i=sub; i<MaxDM; i++)
					memcpy(DM[i],DM[i+1],sizeof(struct DM));
				memset(DM[MaxDM],0,sizeof(struct DM));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Monster Name        Class  \n");
					fprintf(PrinterFP,"---  ----------------  ---------\n");
					for(sub=0; sub<MaxDM; sub++)
						fprintf(PrinterFP,"%2d.  %-16.16s  %-9.9s\n",sub,DM[sub]->Name,Class[DM[sub]->Class]->Name);
					fclose(PrinterFP);
				}
				break;
			case 'N':
				if(top+10<MaxDM)
					top+=10;
				break;
			case 'P':
				if(top)
					top-=10;
				break;
		}
	}
}

void EditSM(int sub)
{
	printf("Name (Max 16 chars)");
	InputString(16,SM[sub]->Name);
	strcpy(SM[sub]->Name,buffer);
	printf("Intellect (20-99)");
	InputUBYTE(SM[sub]->Int);
	SM[sub]->Int=ub;
	printf("Hull size (50-9950)");
	InputUWORD(SM[sub]->Hull);
	SM[sub]->Hull=uw;
	printf("Shot (1-3)");
	InputUWORD(SM[sub]->Shot);
	SM[sub]->Shot=uw;
	printf("Power (50-5000)");
	InputUWORD(SM[sub]->Power);
	SM[sub]->Power=uw;
	printf("Ram (Y/N)");
	InputCHAR(SM[sub]->Ram);
	SM[sub]->Ram=buffer[0];
	printf("Gold pieces");
	InputDOUBLE(SM[sub]->Gold);
	SM[sub]->Gold=d;

	ModFlag=TRUE;
}

void SMFM(void)
{
	int i,sub;

	FOREVER {
		printf("\n");
		printf("No.  Sea Monster Name  Int  Hull  Shot  Power  Ram    Gold  \n");
		printf("---  ----------------  ---  ----  ----  -----  ---  --------\n");
		for(sub=0; sub<MaxSM; sub++)
			printf("%2d.  %-16.16s  %3hu  %4hu  %4hu  %5hu  %3s  %8.8g\n",sub,SM[sub]->Name,SM[sub]->Int,SM[sub]->Hull,SM[sub]->Shot,SM[sub]->Power,(SM[sub]->Ram=='Y') ? "Yes" : "No",SM[sub]->Gold);
		printf("\n<A>dd, <C>hange, <D>elete, <H>ardcopy, <Q>uit: ");
		if(!gets(buffer))
			break;
		ub=toupper(buffer[0]);
		if(ub=='Q')
			break;
		printf("\n");
		switch(ub) {
			case 'A':
				if(MaxSM==16) {
					printf("Maximum of 16 reached.\n");
					break;
				}
				printf("Add Sea Monster # (0-%u)",MaxSM);
				InputUBYTE(MaxSM);
				sub=ub;
				if(sub<0 || sub>MaxSM)
					break;
				MaxSM++;
				for(i=MaxSM; i>=sub; i--)
					memcpy(SM[i],SM[i-1],sizeof(struct SM));
				memset(SM[sub],0,sizeof(struct SM));
				EditSM(sub);
				break;
			case 'C':
				if(!MaxSM) {
					printf("None defined.");
					break;
				}
				printf("Change Sea Monster # (0-%u): ",MaxSM-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxSM-1)
					break;
				EditSM(sub);
				break;
			case 'D':
				if(!MaxSM) {
					printf("None defined.");
					break;
				}
				printf("Delete Sea Monster # (0-%u): ",MaxSM-1);
				InputUBYTE(0);
				sub=ub;
				if(sub<0 || sub>MaxSM-1)
					break;
				MaxSM--;
				ModFlag=TRUE;
				for(i=sub; i<MaxSM; i++)
					memcpy(SM[i],SM[i+1],sizeof(struct SM));
				memset(SM[MaxSM],0,sizeof(struct SM));
				break;
			case 'H':
				if(OpenPrinter()) {
					fprintf(PrinterFP,"No.  Sea Monster Name  Int  Hull  Shot  Power  Ram    Gold  \n");
					fprintf(PrinterFP,"---  ----------------  ---  ----  ----  -----  ---  --------\n");
					for(sub=0; sub<MaxSM; sub++)
						fprintf(PrinterFP,"%2d.  %-16.16s  %3hu  %4hu  %4hu  %5hu  %3s  %8.8g\n",sub,SM[sub]->Name,SM[sub]->Int,SM[sub]->Hull,SM[sub]->Shot,SM[sub]->Power,(SM[sub]->Ram=='Y') ? "Yes" : "No",SM[sub]->Gold);
					fclose(PrinterFP);
				}
				break;
		}
	}
}

void SaveFiles(void)
{
	FILE *fp;
	int i;

	if((fp=fopen("RPGBBS.config","w"))) {
		fprintf(fp,"%s\n",EMAILpath);
		fprintf(fp,"%s\n",HELPpath);
		fprintf(fp,"%s\n",MSGpath);
		fprintf(fp,"%s\n",NOTESpath);
		fprintf(fp,"%s\n",ArenaPIXpath);
		fprintf(fp,"%s\n",DungeonPIXpath);
		fprintf(fp,"%s\n",PlayerPIXpath);
		fprintf(fp,"%s\n",UPLOADpath);
		fprintf(fp,"%u\n",MaxFILE);
		for(i=0; i<MaxFILE; i++) {
			fprintf(fp,"%s\n",FILEname[i]);
			fprintf(fp,"%s\n",FILEpath[i]);
		}
		fprintf(fp,"%u\n",MaxMSG);
		for(i=0; i<MaxMSG; i++) {
			fprintf(fp,"%s\n",MSGname[i]);
			fprintf(fp,"%u\n",MaxMessage[i]);
		}
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.access","wb"))) {
		fwrite((char *)&MaxAccess,sizeof(MaxAccess),1,fp);
		for(i=0; i<MaxAccess; i++)
			fwrite((char *)Access[i],sizeof(struct Access),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.class","wb"))) {
		fwrite((char *)&MaxClass,sizeof(MaxClass),1,fp);
		for(i=0; i<MaxClass; i++)
			fwrite((char *)Class[i],sizeof(struct Class),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.armor","wb"))) {
		fwrite((char *)&MaxArmor,sizeof(MaxArmor),1,fp);
		for(i=0; i<MaxArmor; i++)
			fwrite((char *)Armor[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.weapon","wb"))) {
		fwrite((char *)&MaxWeapon,sizeof(MaxWeapon),1,fp);
		for(i=0; i<MaxWeapon; i++)
			fwrite((char *)Weapon[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.realestate","wb"))) {
		fwrite((char *)&MaxRealEstate,sizeof(MaxRealEstate),1,fp);
		for(i=0; i<MaxRealEstate; i++)
			fwrite((char *)RealEstate[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.security","wb"))) {
		fwrite((char *)&MaxSecurity,sizeof(MaxSecurity),1,fp);
		for(i=0; i<MaxSecurity; i++)
			fwrite((char *)Security[i],sizeof(struct Store),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.magic","wb"))) {
		fwrite((char *)&MaxMagic,sizeof(MaxMagic),1,fp);
		for(i=0; i<MaxMagic; i++)
			fwrite((char *)Magic[i],sizeof(struct Extra),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.poison","wb"))) {
		fwrite((char *)&MaxPoison,sizeof(MaxPoison),1,fp);
		for(i=0; i<MaxPoison; i++)
			fwrite((char *)Poison[i],sizeof(struct Extra),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.monster","wb"))) {
		fwrite((char *)&MaxAM,sizeof(MaxAM),1,fp);
		for(i=0; i<MaxAM; i++)
			fwrite((char *)AM[i],sizeof(struct AM),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.dungeon","wb"))) {
		fwrite((char *)&MaxDM,sizeof(MaxDM),1,fp);
		for(i=0; i<MaxDM; i++)
			fwrite((char *)DM[i],sizeof(struct DM),1,fp);
		fclose(fp);
	}

	if((fp=fopen("RPGBBS.seamonster","wb"))) {
		fwrite((char *)&MaxSM,sizeof(MaxSM),1,fp);
		for(i=0; i<MaxSM; i++)
			fwrite((char *)SM[i],sizeof(struct SM),1,fp);
		fclose(fp);
	}

	ModFlag=FALSE;
}

void Shutdown(void)
{
	int i;

	for(i=0; i<MaxAccess; i++)
		if(Access[i])
			free(Access[i]);
	for(i=0; i<MaxClass; i++)
		if(Class[i])
			free(Class[i]);
	for(i=0; i<MaxArmor; i++)
		if(Armor[i])
			free(Armor[i]);
	for(i=0; i<MaxWeapon; i++)
		if(Weapon[i])
			free(Weapon[i]);
	for(i=0; i<MaxRealEstate; i++)
		if(RealEstate[i])
			free(RealEstate[i]);
	for(i=0; i<MaxSecurity; i++)
		if(Security[i])
			free(Security[i]);
	for(i=0; i<MaxMagic; i++)
		if(Magic[i])
			free(Magic[i]);
	for(i=0; i<MaxPoison; i++)
		if(Poison[i])
			free(Poison[i]);
	for(i=0; i<MaxAM; i++)
		if(AM[i])
			free(AM[i]);
	for(i=0; i<MaxDM; i++)
		if(DM[i])
			free(DM[i]);
	for(i=0; i<MaxSM; i++)
		if(SM[i])
			free(SM[i]);
}

void main()
{
	Init();

	do {
		printf("\14--=:)) RPGBBS Config ((:=--\n\n");
		printf("   1.  BBS Setup\n");
		printf("   2.  User Access Level\n");
		printf("   3.  Class Definition\n");
		printf("   4.  Armor Class\n");
		printf("   5.  Weapon Class\n");
		printf("   6.  Real Estate\n");
		printf("   7.  Security\n");
		printf("   8.  Magic Spells\n");
		printf("   9.  Poisons\n");
		printf("  10.  Arena Monsters\n");
		printf("  11.  Dungeon Monsters\n");
		printf("  12.  Sea Monsters\n");
		if(ModFlag)
			printf("\n*** MODIFICATIONS HAVE BEEN MADE AND ARE NOT SAVED ***\n");
		printf("\nChoice (1-12, <S>ave, <Q>uit): ");
		if(!gets(buffer))
			break;
		choice=atoi(buffer);
		if(!choice) {
			choice=-1;
			ub=toupper(buffer[0]);
			if(ub!='S' && ub!='Q')
				continue;
			if(ub=='S') {
				SaveFiles();
				ModFlag=FALSE;
				continue;
			}
			choice=0;
		}
		switch(choice) {
			case 1:
				BBSFM();
				break;
			case 2:
				AccessFM();
				break;
			case 3:
				ClassFM();
				break;
			case 4:
				ArmorFM();
				break;
			case 5:
				WeaponFM();
				break;
			case 6:
				RealEstateFM();
				break;
			case 7:
				SecurityFM();
				break;
			case 8:
				MagicFM();
				break;
			case 9:
				PoisonFM();
				break;
			case 10:
				AMFM();
				break;
			case 11:
				DMFM();
				break;
			case 12:
				SMFM();
				break;
		}
	} while(choice);

	Shutdown();
}
