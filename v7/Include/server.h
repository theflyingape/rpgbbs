/*******************************\
 *  RPGBBS Deluxe File Server  *
 *                             *
 *   Written by Robert Hurst   *
\*******************************/
#define	SERVER_SHUTDOWN		-1
#define SERVER_CONNECT		 1
#define	SERVER_DISCONNECT	 2
#define	SERVER_BROADCAST	 3
#define	SERVER_DELUSER		 4
#define	SERVER_GETUSER		 5
#define	SERVER_NEXTUSER		 6
#define	SERVER_PREVUSER		 7
#define	SERVER_PUTUSER		 8
#define	SERVER_RNDUSER		 9
#define	SERVER_GETUSERLEVEL	10
#define	SERVER_GETUSERNAME	11
#define	SERVER_LOCK			12
#define	SERVER_UNLOCK		13
#define	SERVER_DELGANG		14
#define	SERVER_GETGANG		15
#define	SERVER_NEXTGANG		16
#define	SERVER_PREVGANG		17
#define	SERVER_PUTGANG		18
#define	SERVER_PUTHOF		19
#define	SERVER_PUTHOL		20
#define	SERVER_WAKEUP		21

#define STRENGTH_SPELL		1L<<0
#define INTELLECT_SPELL		1L<<1
#define DEXTERITY_SPELL		1L<<2
#define CHARISMA_SPELL		1L<<3
#define SHIELD_SPELL		1L<<4
#define HONE_SPELL			1L<<5
#define HEAL_SPELL			1L<<6
#define TELEPORT_SPELL		1L<<7
#define BLAST_SPELL			1L<<8
#define RESURRECT_SPELL		1L<<9
#define CONFUSION_SPELL		1L<<10
#define TRANSMUTE_SPELL		1L<<11
#define CURE_SPELL			1L<<12
#define ILLUSION_SPELL		1L<<13
#define DISINTEGRATE_SPELL	1L<<14
#define MORPH_SPELL			1L<<15

#define	ARMOR_RUSTING_XSPELL	1<<0
#define	WEAPON_DECAY_XSPELL		1<<1
#define	BIG_BLAST_XSPELL		1<<2
#define	MANA_STEALING_XSPELL	1<<3
#define	LIFE_STEALING_XSPELL	1<<4

#define USER_RECORD_SIZE	640L

struct charstats {
	UWORD Calls,Kills,Killed,Retreats,KOs,KOed,JoustWin,JoustLoss;
};

struct playerstats {
	ULONG HP,Hull,Kills;
	UWORD Calls,Killed,Retreats,KO,Joust,Stole,Gamble,Fish;
};

struct notable {
	UBYTE Who[23];
	UWORD What;
	JDATE When;
};

struct class_character {
	UBYTE Name[10];
	UBYTE Melee;		/*	fighting ability: 0x-4x							*/
	UBYTE Backstab;		/*	0=none, 1x-4x									*/
	UBYTE Poison;		/*	0=none, 1x-4x									*/
	UBYTE Magic;		/*	0=none, 1=wand, 2=scroll, 3=spell, 4=enchanted	*/
	UBYTE Steal;		/*	1x-3x											*/
	UBYTE BaseSTR,BaseINT,BaseDEX,BaseCHA;
	UBYTE BonusSTR,BonusINT,BonusDEX,BonusCHA;
	UBYTE MaxSTR,MaxINT,MaxDEX,MaxCHA;
	struct notable Calls[2],Kills[2],Killed[2],Retreats[2],Brawls[2],Jousts[2],melee,blast;
};
struct class {
	UBYTE Origin[9];
	UBYTE NumCharacter;
	struct class_character **Character;
};

struct user {
	UBYTE ID[5];
	UBYTE Handle[23];
	UBYTE Name[31];
	UBYTE Street[31];
	UBYTE City[21];
	UBYTE State[10];
	UBYTE Zip[11];
	UBYTE VoicePhone[16],DataPhone[16];
	UBYTE Password[10];
	JDATE BirthDate,CreateDate,ExpireDate,LastDate;
	UBYTE CallsToday,Emulation,Expert,Rows;
	UWORD LastTime,Immortal;
	UBYTE KeySequence[4];
	UBYTE Access,Class[19];
	UBYTE Gang[20];
	UBYTE Status[5],Blessed[5],Cursed[5],Coward;
	UBYTE KeyHints[4][3];
	DOUBLE Experience;
	UBYTE Sex,Gender,Level,ExpLevel;
	UWORD HP,SP,Hull;
	UBYTE STR,INT,DEX,CHA;
	UBYTE Cannon,Ram;
	DOUBLE Gold,Bank,Loan;
	UBYTE Weapon[34];
	UBYTE Armor[34];
	BYTE WCmod,ACmod;
	UBYTE RealEstate,Security;
	UWORD Spell,Poison;
	struct charstats Current;
	struct playerstats History;
	UWORD Posts;
	UWORD LastMSG[20];
	UBYTE FileRatio,ByteRatio;
	UWORD Uploads,Downloads;
	ULONG UpByte,DownByte;
	LONG LastFileScan;
	DOUBLE Bounty;
	UBYTE BountyWho[5];
	UBYTE Wins;
	DOUBLE Rating;
	UBYTE Novice;
	UBYTE XSpell;
};

struct userid {
	UBYTE ID[5];
	UWORD record;
};

struct userlevel {
	UBYTE Current[3];
	UBYTE ID[5];
};

struct username {
	UBYTE Handle[23];
	UBYTE ID[5];
};

struct gang {
	UBYTE Name[21];
	UBYTE MemberID[4][5];
	UWORD Win,Loss;
	UBYTE BannerType,BannerFore,BannerBack,BannerTrim;
};

struct armor_item {
	UBYTE Name[24];
	DOUBLE Value;
	UBYTE Class;
	UBYTE EgoClass[10];
	BYTE EgoMelee;
	BYTE EgoMagic;
	UBYTE EgoBearer[10];
	UBYTE EgoCurse[10];
};
struct armor {
	UBYTE Origin[9];
	UBYTE NumArmor;
	struct armor_item **Item;
};

struct magic {
	UBYTE Name[20];
	UWORD Power[2];
	DOUBLE Cost[2];
};

struct poison {
	UBYTE Name[20];
	UWORD Power[2];
	DOUBLE Cost[2];
};

struct realestate {
	UBYTE Name[20];
	DOUBLE Value;
};

struct security {
	UBYTE Name[20];
	DOUBLE Value;
};

struct weapon_item {
	UBYTE Name[24];
	DOUBLE Value;
	UBYTE Class;
	UBYTE *Action;
	UBYTE EgoClass[10];
	BYTE EgoMelee;
	BYTE EgoMagic;
	UBYTE EgoBearer[10];
	UBYTE EgoCurse[10];
};
struct weapon {
	UBYTE Origin[9];
	UBYTE NumWeapon;
	struct weapon_item **Item;
};

struct access {
	UBYTE Name[16];
	UBYTE Calls,Minutes,Promote;
	UBYTE Verify,Bulletin,File,RolePlay,Weekend,Sysop;
};

struct arena {
	UBYTE Name[20];
	UBYTE Class[19];
	UBYTE Level;
	UBYTE Weapon[34];
	UBYTE Armor[34];
	DOUBLE Gold;
	UWORD Spell;
};

struct dungeon {
	UBYTE Name[20];
	UBYTE Class[19];
	UWORD Spell;
	UBYTE XSpell;
};

struct naval {
	UBYTE Name[20];
	UBYTE INT;
	UBYTE Ram;
	UWORD Hull,Shot,Power;
	DOUBLE Value;
};

struct callers {
	UBYTE Last[3][23];
	UBYTE Reason[3][64];
};

struct HiScore {
	DOUBLE HP,Hull,Kills;
	DOUBLE Calls,Killed,Retreats,KO,Joust,Stole,Gamble,Fish;
	DOUBLE Rating;
	UBYTE Who[12][23];
};

struct ServerTable {
	UWORD NumAccess;
	struct access **access;
	UWORD NumArena;
	struct arena **arena;
	UWORD NumArmor;
	struct armor **armor;
	UWORD NumClass;
	struct class **class;
	UWORD NumDungeon;
	struct dungeon **dungeon;
	UWORD NumMagic;
	struct magic **magic;
	UWORD NumNaval;
	struct naval **naval;
	UWORD NumPoison;
	struct poison **poison;
	UWORD NumRealEstate;
	struct realestate **realestate;
	UWORD NumSecurity;
	struct security **security;
	UWORD NumWeapon;
	struct weapon **weapon;
	struct callers *callers;
	struct HiScore *HiScore;
	struct user *IRS,*NEW,*SYS;
};

struct RPGserver {
	struct Message message;
	UBYTE client[5];
	BYTE command;
	UBYTE *parameter;
	UBYTE *result;
};
