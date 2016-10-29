/*****************************\
 *   RPGBBS Deluxe Client    *
 *                           *
 *  Written by Robert Hurst  *
\*****************************/
#include "GameSmith:include/sound/sound.h"
#include "GameSmith:include/proto/all.h"

#define NUMACCESS		ServerTable->NumAccess
#define ACCESS(n)		ServerTable->access[n]
#define NUMARENA		ServerTable->NumArena
#define	ARENA(n)		ServerTable->arena[n]
#define NUMARMOR		ServerTable->NumArmor
#define MAXARMOR(n)		ServerTable->armor[n]->NumArmor
#define ARMOR(n)		ServerTable->armor[n->armor_origin]->Item[n->armor_type]
#define NUMCLASS		ServerTable->NumClass
#define MAXCLASS(n)		ServerTable->class[n]->NumCharacter
#define CLASS(n)		ServerTable->class[n->class_origin]->Character[n->class_type]
#define NUMDUNGEON		ServerTable->NumDungeon
#define DUNGEON(n)		ServerTable->dungeon[n]
#define NUMMAGIC		ServerTable->NumMagic
#define MAGIC(n)		ServerTable->magic[n]
#define NUMNAVAL		ServerTable->NumNaval
#define NAVAL(n)		ServerTable->naval[n]
#define NUMPOISON		ServerTable->NumPoison
#define	POISON(n)		ServerTable->poison[n]
#define NUMREALESTATE	ServerTable->NumRealEstate
#define	REALESTATE(n)	ServerTable->realestate[n]
#define NUMSECURITY		ServerTable->NumSecurity
#define SECURITY(n)		ServerTable->security[n]
#define CALLERS			ServerTable->callers
#define	NUMWEAPON		ServerTable->NumWeapon
#define	MAXWEAPON(n)	ServerTable->weapon[n]->NumWeapon
#define	WEAPON(n)		ServerTable->weapon[n->weapon_origin]->Item[n->weapon_type]
#define	IRS				ServerTable->IRS
#define	NEW				ServerTable->NEW
#define	SYS				ServerTable->SYS

#define ONLINE			RPC[0][0]
#define PLAYER			RPC[0][0]->user
#define ENEMY			RPC[1][0]->user
#define	GANG(i,j)		RPC[i][j]->user

#define CLL			output(&SERCON,"\33[K")
#define INPUT(t)	input(&SERCON,Serbuf,&source,sizeof(Serbuf),t)
#define NL			output(&SERCON,"\15\12")
#define NORMAL		output(&SERCON,"\33[0;37m");bold=FALSE
#define OUT(s)		output(&SERCON,s)
#define OUTCON(s)	if(window) { WriteConsole(&SERCON,s); }
#define OUTSER(s)	WriteSerial(&SERCON,s)
#define REVERSE		output(&SERCON,"\33[7m")
#define RUBOUT		output(&SERCON,"\10 \10")
#define SAMELINE	output(&SERCON,"\15")

#define AN(s)		(strchr("AaEeIiOoUu",s[0]) ? "an " : "a ")
#define HE(n)		(n->user.Gender=='M' ? "he" : n->user.Gender=='F' ? "she" : "it")

#define SERBUF_SIZE		512
#define OUTBUF_SIZE		256
#define PROMPT_SIZE		128
#define INBUF_SIZE		256
#define FILE_SIZE		128

#define	GOLD	0
#define	BELL	1
#define	ORANGE	2
#define	LIME	3
#define	PLUM	4
#define	CHERRY	5

#define EXP(n) 1000.*pow(2.,n-1.)
#define JOUST(n) modf(pow2((double)(n-1)/2.)*10.,&d);

struct RPC {
	struct user user;
	UBYTE class_origin,class_type;
	UBYTE weapon_origin,weapon_type;
	UBYTE armor_origin,armor_type;
	WORD BP,HP,SP,HULL;
	BYTE ToAC,ToWC;
	UBYTE *He,*His;
	UBYTE *he,*his,*him;
	UBYTE STR,INT,DEX,CHA;
	UBYTE Confused;
};

struct RPGitem {
   char choice;
   char *name;
};
struct RPGmenu {
   char *option;
   char color;
   int items;
   struct RPGitem item[20];
};

extern	void assign(int);
extern	void beep(void);
extern	char *back(int);
extern	char *bracket(int);
extern	void broadcast(char *);
extern	void bump(char *);
extern	int cd(void);
extern	void charstats(struct RPC *);
extern	void chkexp(void);
extern	void cls(void);
extern	int dice(int);
extern	void displayview(void);
extern	void finputs(char *,int,FILE *);
extern	void emulation(void);
extern	void erase(void);
extern	char *fore(int);
extern	char getkey(void);
extern	void immortalize(void);
extern	char inkey(char,char);
extern	int ins(int);
extern	void jazz(char *);
extern	void keyhint(void);
extern	void modem_clr(void);
extern	void modem_cmd(char *);
extern	char *money(double,int);
extern	char more(void);
extern	void news(char *);
extern	void note(char *);
extern	char option(struct RPGmenu *);
extern	void pause(void);
extern	void ReKeySequence(struct RPC *);
extern	void reroll(struct RPC *);
extern	void sound(char *,int);
extern	int timeleft(void);
extern	void title(char *);
extern	int type(char *,int);
extern	int type_pix(char *,char);
extern	DOUBLE value(DOUBLE,UBYTE);
extern	int GetRPC(struct RPC *);
extern	void LoadRPC(struct RPC *);
extern	int RPGserver(char,UBYTE *);
extern	int OpenGUI(void);
extern	void CloseGUI(void);
extern	void yourstats(void);
extern	void CreateRPC(struct RPC *);
extern	void ExchangeWeapon(struct RPC *,struct RPC *);
extern	void ExchangeArmor(struct RPC *,struct RPC *);

extern	void Arena(void);
extern	int Cast(struct RPC *,struct RPC *);
extern	void PoisonWeapon(struct RPC *);
extern	void Battle(void);
extern	void Bulletins(void);
extern	void Casino(void);
extern	void Dungeon(int);
extern	void Editor(struct RPC *rpc,int);
extern	int ReadMail(char *);
extern	void WriteMail(char *);
extern	void Files(void);
extern	void Lists(void);
extern	int Logon(void);
extern	void MainMenu(void);
extern	void Naval(void);
extern	void NewUser(void);
extern	void Party(void);
extern	void Square(void);
extern	void Tavern(void);
