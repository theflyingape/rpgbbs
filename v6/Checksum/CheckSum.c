#include <exec/types.h>
#include <stdio.h>
#include <string.h>

#define MAXLEVEL 		User.Level
#define MAXLINE 		User.ExpLevel

struct User {
	UBYTE Handle[26];
	UBYTE Name[30];
	UBYTE Street[2][30];
	DOUBLE VoicePhone,DataPhone;
	UBYTE Password[5];
	UBYTE Sex;
	ULONG BirthDate;
	UBYTE Access,ANSI,Expert,Format;
	ULONG ExpireDate,LastDate;
	UWORD LastTime,LastBaud;
	UWORD TotalCalls,TotalPosts;
	UBYTE CallsToday;
	UBYTE Status;
	DOUBLE Experience,Gold,Bank,Loan;
	UWORD Immortal,HP,SP;
	UBYTE Class,Level,ExpLevel,Str,Int,Agl,Chr,Blessed,Cursed;
	BYTE Weapon,Armor;
	UBYTE RealEstate,Security;
	UWORD Spell,Poison;
	UWORD Hull,Cannon;
	UBYTE Ram;
	UBYTE Gang;
	UWORD Calls,YouKill,WasKill,YouKO,WasKO,JoustWin,JoustLoss;
	UBYTE KeySequence;
	UWORD LastMessage[16];
} User;

UWORD Checksum(char *str,int s)
{
	UWORD result;
	int i;

	result=0;
	for(i=0; i<s; i++) {
		result++;
		result^=str[i];
	}
	return(result);
}

void main(void)
{
	UWORD total;
	char buffer[41];

	memset(&User,0,sizeof(User));
	MAXLINE=16;
	MAXLEVEL=99;

	printf("\nLicense to: ");
	gets(buffer);
	strcpy(User.Street[0],buffer);
	printf("\nLicense of: ");
	gets(buffer);
	strcpy(User.Street[1],buffer);

	total=255-MAXLINE;
	total+=Checksum(User.Street[0],sizeof(User.Street[0]));
	total+=Checksum(User.Street[1],sizeof(User.Street[1]));
	User.LastTime=total;
	printf("\nServer Lic#: %u   Online: %u\n",User.LastTime,MAXLINE);

	total=32768-MAXLEVEL;
	total+=Checksum(User.Street[0],sizeof(User.Street[0]));
	total+=Checksum(User.Street[1],sizeof(User.Street[1]));
	User.LastBaud=total;
	printf("\nClient Lic #: %u   Levels: %u\n\n",User.LastBaud,MAXLEVEL);
}
