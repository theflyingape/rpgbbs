#include "/include/all.h"
#include "global.h"

void yourstats(void)
{
	sprintf(outbuf,"%sStr:%s%3u%s (%u,%u)     %sInt:%s%3u%s (%u,%u)     %sDex:%s%3u%s (%u,%u)     %sCha:%s%3u%s (%u,%u)",
		fore(CYN),fore(ONLINE->STR>PLAYER.STR ? YELLOW : ONLINE->STR<PLAYER.STR ? BRED : WHITE),ONLINE->STR,fore(GRY),PLAYER.STR,CLASS(ONLINE)->MaxSTR,
		fore(CYN),fore(ONLINE->INT>PLAYER.INT ? YELLOW : ONLINE->INT<PLAYER.INT ? BRED : WHITE),ONLINE->INT,fore(GRY),PLAYER.INT,CLASS(ONLINE)->MaxINT,
		fore(CYN),fore(ONLINE->DEX>PLAYER.DEX ? YELLOW : ONLINE->DEX<PLAYER.DEX ? BRED : WHITE),ONLINE->DEX,fore(GRY),PLAYER.DEX,CLASS(ONLINE)->MaxDEX,
		fore(CYN),fore(ONLINE->CHA>PLAYER.CHA ? YELLOW : ONLINE->CHA<PLAYER.CHA ? BRED : WHITE),ONLINE->CHA,fore(GRY),PLAYER.CHA,CLASS(ONLINE)->MaxCHA
	);
	OUT(outbuf);NL;
	sprintf(outbuf,"%sHit points: %s%d%s/%d",fore(CYN),fore(ONLINE->HP>PLAYER.HP ? YELLOW : ONLINE->HP<PLAYER.HP ? BRED : WHITE),ONLINE->HP,fore(GRY),PLAYER.HP);
	OUT(outbuf);
	if(PLAYER.SP) {
		sprintf(outbuf,"     %sSpell points: %s%d%s/%d",fore(CYN),fore(ONLINE->SP>PLAYER.SP ? YELLOW : ONLINE->SP<PLAYER.SP ? BRED : WHITE),ONLINE->SP,fore(GRY),PLAYER.SP);
		OUT(outbuf);
	}
	if(PLAYER.Gold) {
		sprintf(outbuf,"     %sMoney: %s",fore(CYN),money(PLAYER.Gold,ANSI));
		OUT(outbuf);
	}
	NL;
	sprintf(outbuf,"%sWeapon:%s %s",fore(CYN),fore(WHITE),WEAPON(ONLINE)->Name);
	OUT(outbuf);
	if(PLAYER.WCmod || ONLINE->ToWC) {
		sprintf(outbuf," %s(%s%+d%s,%s%+d%s)",fore(MAG),fore(PLAYER.WCmod>0 ? YELLOW : PLAYER.WCmod<0 ? BRED : GRY),PLAYER.WCmod,fore(GRY),fore(ONLINE->ToWC>0 ? YELLOW : ONLINE->ToWC<0 ? BRED : GRY),ONLINE->ToWC,fore(MAG));
		OUT(outbuf);
	}
	sprintf(outbuf,"     %sArmor:%s %s",fore(CYN),fore(WHITE),ARMOR(ONLINE)->Name);
	OUT(outbuf);
	if(PLAYER.ACmod || ONLINE->ToAC) {
		sprintf(outbuf," %s(%s%+d%s,%s%+d%s)",fore(MAG),fore(PLAYER.ACmod>0 ? YELLOW : PLAYER.ACmod<0 ? BRED : GRY),PLAYER.ACmod,fore(GRY),fore(ONLINE->ToAC>0 ? YELLOW : ONLINE->ToAC<0 ? BRED : GRY),ONLINE->ToAC,fore(MAG));
		OUT(outbuf);NL;
	}
	NORMAL;NL;
}

int Melee(struct RPC *rpc,struct RPC *enemy,int attack)
{
	int WC,AC;
	int damage,smash,sc=0;
	char *action,*s,period='.';

	if(rpc->weapon_origin<NUMWEAPON) {
		if((WC=WEAPON(rpc)->Class+rpc->user.WCmod+rpc->ToWC)<1)
			WC=0;
	}
	else
		WC=-(rpc->weapon_type+rpc->user.WCmod+rpc->ToWC);
	if(enemy->armor_origin<NUMARMOR) {
 		if((AC=ARMOR(enemy)->Class+enemy->user.ACmod+enemy->ToAC)<1)
			AC=0;
	}
	else
		AC=-(enemy->armor_type+enemy->user.ACmod+enemy->ToAC);

	damage=rpc->DEX;
	if(attack==1)
		damage=50+damage/2;
	damage+=((int)rpc->DEX-(int)enemy->DEX)/2;
	if(damage<5)
		damage=5;
	if(damage>99)
		damage=99;
	if(dice(100)>damage) {
		sound("miss",48);
		if(attack==1) {
			if(rpc==ONLINE)
				sprintf(outbuf,"Your %s passes through thin air.",WEAPON(rpc)->Name);
			else {
				if(rpc->weapon_origin<NUMWEAPON)
					sprintf(outbuf,"%s%s's %s whistles by %s%s.",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,WEAPON(rpc)->Name,(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
				else
					sprintf(outbuf,"%s%s attacks %s%s, but misses.",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
			}
		}
		else
			strcpy(outbuf,"Attempt fails!");
		return(0);
	}

	damage=dice(rpc->user.Level)+rpc->STR/10;
	damage+=CLASS(rpc)->Melee*dice(rpc->user.Level);
	smash=dice(100);
	for(; smash>98; smash--,sc=1,period='!')
		damage+=dice(rpc->user.Level);
	damage*=50+rpc->STR/2;
	damage/=100;
	damage+=2*(abs(WC)+dice(abs(WC)+1)-1);
	smash=dice(100);
	if(rpc->weapon_origin<NUMWEAPON) {
		if(strcmp(WEAPON(rpc)->EgoClass,CLASS(enemy)->Name)==0 || strcmp(WEAPON(rpc)->EgoClass,"*")==0)
			smash=(smash<99) ? smash=99 : smash+1;
		if(WEAPON(rpc)->EgoMelee==CLASS(enemy)->Melee)
			smash=(smash<99) ? smash=99 : smash+1;
		if(WEAPON(rpc)->EgoMagic==CLASS(enemy)->Magic)
			smash=(smash<99) ? smash=99 : smash+1;
		if(strcmp(WEAPON(rpc)->EgoBearer,CLASS(rpc)->Name)==0 || strcmp(WEAPON(rpc)->EgoBearer,"*")==0)
			smash=(smash<99) ? smash=99 : smash+1;
	}
	for(; smash>98; smash--,sc=1,period='!')
		damage+=dice(abs(WC));
	if(rpc->weapon_origin<NUMWEAPON)
		if(strcmp(WEAPON(rpc)->EgoCurse,CLASS(rpc)->Name)==0 || strcmp(WEAPON(rpc)->EgoCurse,"*")==0)
			damage/=2;
	damage*=50+rpc->STR/2;
	damage/=100;
	damage-=abs(AC)+dice(abs(AC));
	if(enemy->armor_origin<NUMARMOR) {
		if(strcmp(ARMOR(enemy)->EgoClass,CLASS(rpc)->Name)==0 || strcmp(ARMOR(enemy)->EgoClass,"*")==0)
			damage-=dice(abs(AC));
		if(ARMOR(enemy)->EgoMelee==CLASS(rpc)->Melee)
			damage-=dice(abs(AC));
		if(ARMOR(enemy)->EgoMagic==CLASS(rpc)->Magic)
			damage-=dice(abs(AC));
		if(strcmp(ARMOR(enemy)->EgoBearer,CLASS(enemy)->Name)==0 || strcmp(ARMOR(enemy)->EgoBearer,"*")==0)
			damage-=dice(abs(AC));
	}
	if(damage<1)
		damage=dice(2);
	if(enemy->armor_origin<NUMARMOR)
		if(strcmp(ARMOR(enemy)->EgoCurse,CLASS(enemy)->Name)==0 || strcmp(ARMOR(enemy)->EgoCurse,"*")==0)
			damage+=abs(AC)/2+dice(abs(AC));
	damage*=attack;
	if(damage>=CLASS(rpc)->melee.What) {
		sound("outstanding",48);
		strcpy(CLASS(rpc)->melee.Who,rpc->user.Handle);
		CLASS(rpc)->melee.What=damage;
		CLASS(rpc)->melee.When=Julian;
		if(damage>=ServerTable->class[0]->Character[0]->melee.What) {
			strcpy(ServerTable->class[0]->Character[0]->melee.Who,rpc->user.Handle);
			ServerTable->class[0]->Character[0]->melee.What=damage;
			ServerTable->class[0]->Character[0]->melee.When=Julian;
		}
		RPGserver(SERVER_PUTHOF,NULL);
	}

	action=sc ? (attack==1 ? "smash" : "plunge") : (attack==1 ? "hit" : "stab");
	if(rpc->weapon_origin<NUMWEAPON && WEAPON(rpc)->Action) {
		strcpy(inbuf,WEAPON(rpc)->Action);
		if(attack>1)
			sc+=2;
		for(action=strtok(inbuf,","); action && sc; sc--)
			action=strtok(NULL,",");
	}

	if(period=='!')
		sound("smash",64);
	if(rpc==ONLINE)
		sprintf(outbuf,"You %s %s%s for %d hit points%c",action,(enemy->user.Gender=='I' ? "the " : ""),enemy->user.Handle,damage,period);
	else {
		sc=strlen(action)-1;
		s=!stricmp(&action[sc-1],"ch") ? "es" : !stricmp(&action[sc-1],"sh") ? "es" : action[sc]=='s' ? "es" : "s";
		if(enemy==ONLINE)
			sprintf(outbuf,"%s%s %s%s you for %d hit points%c",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,action,s,damage,period);
		else
			sprintf(outbuf,"%s%s %s%s %s%s for %d hit points%c",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,action,s,(enemy->user.Gender=='I' ? "the " : ""),enemy->user.Handle,damage,period);
	}
	return(damage);
}

int Cast(struct RPC *rpc,struct RPC *enemy)
{
	int ability,backfire,i,mu,p,s;
	int AC1,AC2,WC1,WC2;

	mu=(CLASS(rpc)->Magic<4) ? 0 : 1;
	AC1=(rpc->armor_origin<NUMARMOR ? ARMOR(rpc)->Class : rpc->armor_type)+rpc->user.ACmod+rpc->ToAC;
	WC1=(rpc->weapon_origin<NUMWEAPON ? WEAPON(rpc)->Class : rpc->weapon_type)+rpc->user.WCmod+rpc->ToWC;
	if(enemy) {
		AC2=(enemy->armor_origin<NUMARMOR ? ARMOR(enemy)->Class : enemy->armor_type)+enemy->user.ACmod+enemy->ToAC;
		WC2=(enemy->weapon_origin<NUMWEAPON ? WEAPON(enemy)->Class : enemy->weapon_type)+enemy->user.WCmod+enemy->ToWC;
	}
	else {
		AC2=0;
		WC2=0;
	}

	if(rpc==ONLINE) {
		if(!PLAYER.Spell && !PLAYER.XSpell) {
			OUT("You don't have any magic.");NL;
			return(0);
		}
		sprintf(prompt,"%sEnter spell (?=list): ",fore(CYN));
		do {
			s=0;p=0;
			OUT(prompt);
			if(ins(2))
				break;
			NL;NL;
			if(*inbuf=='?') {
				for(i=0; i<16; i++) {
					p=pow2((double)i);
					if(PLAYER.Spell & p) {
						sprintf(outbuf,"%s%s %-20s",bracket(i+1),fore(GRY),MAGIC(i)->Name);
						OUT(outbuf);
						if(CLASS(rpc)->Magic>1) {
							sprintf(outbuf,"  %4u",MAGIC(i)->Power[mu]);
							OUT(outbuf);
						}
						ability=rpc->INT+rpc->user.Level/10-(i+1);
						if(enemy)
							if(p==BLAST_SPELL || p==CONFUSION_SPELL || p==ILLUSION_SPELL || p==DISINTEGRATE_SPELL || p==MORPH_SPELL)
								ability+=(int)(rpc->INT-enemy->INT);
						ability=(ability<1) ? 1 : (ability>99) ? 99 : ability;
						sprintf(outbuf,"  %3u%",100-ability);
						OUT(outbuf);NL;
					}
				}
				for(i=16; i<NUMMAGIC; i++) {
					p=pow2((double)i-16);
					if(PLAYER.XSpell & p) {
						sprintf(outbuf,"%s%s %-20s",bracket(i+1),fore(GRY),MAGIC(i)->Name);
						OUT(outbuf);
						if(CLASS(rpc)->Magic>1) {
							sprintf(outbuf,"  %4u",MAGIC(i)->Power[mu]);
							OUT(outbuf);
						}
						ability=rpc->INT+rpc->user.Level/10-i+15;
						if(enemy)
							ability+=(int)(rpc->INT-enemy->INT);
						if(!rpc->class_origin)
							ability-=5*i/4;
						ability=(ability<1) ? 1 : (ability>99) ? 99 : ability;
						sprintf(outbuf,"  %3u%",100-ability);
						OUT(outbuf);NL;
					}
				}
				NL;
				continue;
			}
			else {
				s=atoi(inbuf);
				p=pow2((s<17 ? s : s-16)-1.);
				if(!s)
					return(s);
			}
		} while(s<1 || s>NUMMAGIC || (s<17 && !(PLAYER.Spell & p)) || (s>16 && !(PLAYER.XSpell & p)));
		if(enemy) {
			if(enemy->user.Novice=='Y' && p==MORPH_SPELL) {
				OUT("You cannot cast a Morph spell on a novice player.");NL;
				return(0);
			}
			if(s<17 && p!=TELEPORT_SPELL && p!=HEAL_SPELL && p!=BLAST_SPELL && p!=CONFUSION_SPELL && p!=TRANSMUTE_SPELL && p!=CURE_SPELL && p!=ILLUSION_SPELL && p!=DISINTEGRATE_SPELL && p!=MORPH_SPELL) {
				OUT("You cannot cast that spell while engaged in battle.");NL;
				return(0);
			}
		}
		else
			if(s<17 && (p==BLAST_SPELL || p==CONFUSION_SPELL || p==TRANSMUTE_SPELL || p==ILLUSION_SPELL || p==DISINTEGRATE_SPELL || p==MORPH_SPELL)) {
				OUT("You can only cast that spell while engaged in battle.");
				return(0);
			}
		if(CLASS(rpc)->Magic>1 && rpc->SP<MAGIC(s-1)->Power[mu]) {
			OUT("You don't have enough spell power to cast that spell.");NL;
			return(0);
		}
	}
	else {
		p=0;s=0;
		if(CLASS(rpc)->Magic==1) {
			if((rpc->user.Spell & TELEPORT_SPELL) && rpc->HP<rpc->user.HP/8 && dice(6-CLASS(enemy)->Melee)==1) {
				p=TELEPORT_SPELL;
				s=8;
			}
			else
			if((rpc->user.Spell & HEAL_SPELL) && rpc->HP<rpc->user.HP/3 && dice(CLASS(enemy)->Melee+2)>1) {
				p=HEAL_SPELL;
				s=7;
			}
			else
			if((rpc->user.Spell & BLAST_SPELL) && rpc->HP<rpc->user.HP*2/3 && dice(CLASS(enemy)->Melee+2)>1) {
				p=BLAST_SPELL;
				s=9;
			}
			else
			if((rpc->user.Spell & CONFUSION_SPELL) && rpc->HP>rpc->user.HP*2/3 && dice(CLASS(enemy)->Melee+2)==1) {
				p=CONFUSION_SPELL;
				s=11;
			}
			else
			if((rpc->user.Spell & TRANSMUTE_SPELL) && rpc->HP>rpc->user.HP*2/3 && dice(CLASS(enemy)->Melee+((BYTE)rpc->user.Level-enemy->user.Level)/3)==1) {
				p=TRANSMUTE_SPELL;
				s=12;
			}
			else
			if((rpc->user.Spell & CURE_SPELL) && rpc->HP<rpc->user.HP/6 && dice(6-CLASS(enemy)->Melee)==1) {
				p=CURE_SPELL;
				s=13;
			}
			else
			if((rpc->user.Spell & ILLUSION_SPELL) && rpc->HP>rpc->user.HP/2 && dice(6-CLASS(enemy)->Melee+((BYTE)rpc->user.Level-enemy->user.Level)/3)==1) {
				p=ILLUSION_SPELL;
				s=14;
			}
			else
			if((rpc->user.Spell & DISINTEGRATE_SPELL) && !rpc->Confused && dice((BYTE)rpc->user.Level-enemy->user.Level)==1) {
				p=DISINTEGRATE_SPELL;
				s=15;
			}
			else
			if((rpc->user.Spell & MORPH_SPELL) && rpc->HP==rpc->user.HP && dice((BYTE)rpc->user.Level-enemy->user.Level)==1) {
				p=MORPH_SPELL;
				s=16;
			}
		}
		if(CLASS(rpc)->Magic>1) {
			if((rpc->user.Spell & ILLUSION_SPELL) && rpc->SP>=MAGIC(13)->Power[mu] && rpc->HP>rpc->user.HP/2 && dice(6-CLASS(enemy)->Melee)==1) {
				p=ILLUSION_SPELL;
				s=14;
			}
			else
			if((rpc->user.Spell & CONFUSION_SPELL) && rpc->SP>=MAGIC(10)->Power[mu] && rpc->HP>rpc->user.HP/2 && dice(CLASS(enemy)->Melee+2)==1) {
				p=CONFUSION_SPELL;
				s=11;
			}
			else
			if((rpc->user.Spell & TRANSMUTE_SPELL) && rpc->SP>=MAGIC(11)->Power[mu] && rpc->HP>rpc->user.HP*2/3 && dice(CLASS(enemy)->Melee+((BYTE)rpc->user.Level-enemy->user.Level)/3)==1) {
				p=TRANSMUTE_SPELL;
				s=12;
			}
			else
			if((rpc->user.Spell & DISINTEGRATE_SPELL) && rpc->SP>=MAGIC(15)->Power[mu] && !rpc->Confused && dice((BYTE)rpc->user.Level-enemy->user.Level)==1) {
				p=DISINTEGRATE_SPELL;
				s=15;
			}
			else
			if((rpc->user.Spell & MORPH_SPELL) && rpc->SP>=MAGIC(14)->Power[mu] && rpc->HP==rpc->user.HP && !rpc->Confused && dice((BYTE)rpc->user.Level-enemy->user.Level)==1) {
				p=MORPH_SPELL;
				s=16;
			}
			else
			if((rpc->user.Spell & TELEPORT_SPELL) && rpc->SP>=MAGIC(7)->Power[mu] && rpc->HP<rpc->user.HP/8 && dice((BYTE)enemy->user.Level-rpc->user.Level)>1) {
				p=TELEPORT_SPELL;
				s=8;
			}
			else
			if((rpc->user.Spell & CURE_SPELL) && rpc->SP>=MAGIC(12)->Power[mu] && rpc->HP<rpc->user.HP/4) {
				p=CURE_SPELL;
				s=13;
			}
			else
			if((rpc->user.Spell & HEAL_SPELL) && rpc->SP>=MAGIC(6)->Power[mu] && rpc->HP<rpc->user.HP/2 && (dice(CLASS(enemy)->Melee+2)==1 || (!(rpc->user.Spell & BLAST_SPELL) && !rpc->user.XSpell) || rpc->SP<MAGIC(8)->Power[mu])) {
				p=HEAL_SPELL;
				s=7;
			}
			else
			if((rpc->user.Spell & BLAST_SPELL) && rpc->SP>=MAGIC(8)->Power[mu]) {
				p=BLAST_SPELL;
				s=9;
			}
		}
		if(!p) {
			if(rpc->user.XSpell && dice((enemy->INT/10+enemy->CHA/10)/4+2)==1) {
				i=dice(5);
				switch(i) {
					case 1:
						if((rpc->user.XSpell & ARMOR_RUSTING_XSPELL) && AC2>0 && (CLASS(rpc)->Magic==1 || rpc->SP>=MAGIC(16)->Power[mu])) {
							p=ARMOR_RUSTING_XSPELL;
							s=17;
						}
						break;
					case 2:
						if((rpc->user.XSpell & WEAPON_DECAY_XSPELL) && WC2>0 && (CLASS(rpc)->Magic==1 || rpc->SP>=MAGIC(17)->Power[mu])) {
							p=WEAPON_DECAY_XSPELL;
							s=18;
						}
						break;
					case 3:
						if((rpc->user.XSpell & BIG_BLAST_XSPELL) && (CLASS(rpc)->Magic==1 || rpc->SP>=MAGIC(18)->Power[mu])) {
							p=BIG_BLAST_XSPELL;
							s=19;
						}
						break;
					case 4:
						if((rpc->user.XSpell & MANA_STEALING_XSPELL) && enemy->SP && (CLASS(rpc)->Magic==1 || rpc->SP>=MAGIC(19)->Power[mu])) {
							p=MANA_STEALING_XSPELL;
							s=20;
						}
						break;
					case 5:
						if((rpc->user.XSpell & LIFE_STEALING_XSPELL) && (CLASS(rpc)->Magic==1 || rpc->SP>=MAGIC(20)->Power[mu])) {
							p=LIFE_STEALING_XSPELL;
							s=21;
						}
						break;
				}
			}
			if(!p)
				return(0);
		}
	}
	if(rpc->SP)
		rpc->SP-=MAGIC(s-1)->Power[mu];
	if(CLASS(rpc)->Magic==1 && dice(100)<100-2*(21-(s>16 ? 16 : s))) {
		if(s<17)
			rpc->user.Spell&=(UWORD)~p;
		else
			rpc->user.XSpell&=(UBYTE)~p;
		sprintf(outbuf,"%s wand smokes as %s cast%s the spell.",rpc->His,rpc->he,(rpc!=ONLINE ? "s" : ""));
		OUT(outbuf);NL;
		Delay(25);
		if(strlen(rpc->user.ID) && *rpc->user.ID!='_')
			RPGserver(SERVER_PUTUSER,(UBYTE *)&rpc->user);
	}
	else
	if(CLASS(rpc)->Magic==2 && dice(5)==1) {
		if(s<17)
			rpc->user.Spell&=(UWORD)~p;
		else
			rpc->user.XSpell&=(UBYTE)~p;
		sprintf(outbuf,"%s scroll burns as %s cast%s the spell.",rpc->His,rpc->he,(rpc!=ONLINE ? "s" : ""));
		OUT(outbuf);NL;
		Delay(25);
		if(strlen(rpc->user.ID) && *rpc->user.ID!='_')
			RPGserver(SERVER_PUTUSER,(UBYTE *)&rpc->user);
	}
	else
	if(rpc!=ONLINE)
		Delay(25);
	ability=rpc->INT+rpc->user.Level/10-(s<17 ? s : s-16);
	if(enemy)
		if(p==BLAST_SPELL || p==CONFUSION_SPELL || p==ILLUSION_SPELL || p==DISINTEGRATE_SPELL || p==MORPH_SPELL || s>16)
			ability+=(int)(rpc->INT-enemy->INT);
	if(s>16 && !rpc->class_origin)
		ability-=5*s/4;
	ability=(ability<1) ? 1 : (ability>99) ? 99 : ability;
	backfire=FALSE;
	if(dice(100)>ability) {
		ability=50+ability/2;
		if(dice(100)>ability) {
			sound("oops",64);
			sprintf(outbuf,"Oops!  %s%s%s spell backfires!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "Your" : rpc->user.Handle),(rpc==ONLINE ? "" : "'s"));
			OUT(outbuf);NL;
			Delay(25);
			backfire=TRUE;
		}
		else {
			sound("fssst",64);
			sprintf(outbuf,"Fssst!  %s%s%s spell fails!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "Your" : rpc->user.Handle),(rpc==ONLINE ? "" : "'s"));
			p=0;s='F';
		}
	}
	if(s<17) {
		switch(p) {
			case STRENGTH_SPELL:
				ability=CLASS(rpc)->MaxSTR-(strlen(rpc->user.Cursed) ? 10 : 0);
				i=dice(10);
				if(backfire) {
					rpc->STR-=(i>rpc->STR) ? rpc->STR : i;
					sprintf(outbuf,"You feel weaker (%d)",rpc->STR);
				}
				else {
					if(rpc->STR+i<ability) {
						rpc->STR+=i;
						sprintf(outbuf,"You feel much more stronger (%d)",rpc->STR);
					}
					else {
						rpc->STR=ability;
						strcpy(outbuf,"This game prohibits the use of steroids.");
					}
				}
				break;
			case INTELLECT_SPELL:
				ability=CLASS(rpc)->MaxINT-(strlen(rpc->user.Cursed) ? 10 : 0);
				i=dice(10);
				if(backfire) {
					rpc->INT-=(i>rpc->INT) ? rpc->INT : i;
					sprintf(outbuf,"You feel stupid (%d)",rpc->INT);
				}
				else {
					if(rpc->INT+i<ability) {
						rpc->INT+=i;
						sprintf(outbuf,"You feel much more intelligent (%d)",rpc->INT);
					}
					else {
						rpc->INT=ability;
						strcpy(outbuf,"Get on with it, professor!");
					}
				}
				break;
			case DEXTERITY_SPELL:
				ability=CLASS(rpc)->MaxDEX-(strlen(rpc->user.Cursed) ? 10 : 0);
				i=dice(10);
				if(backfire) {
					rpc->DEX-=(i>rpc->DEX) ? rpc->DEX : i;
					sprintf(outbuf,"You feel clumsy (%d)",rpc->DEX);
				}
				else {
					if(rpc->DEX+i<ability) {
						rpc->DEX+=i;
						sprintf(outbuf,"You feel much more agile (%d)",rpc->DEX);
					}
					else {
						rpc->DEX=ability;
						strcpy(outbuf,"Y'all shakin' and bakin'.");
					}
				}
				break;
			case CHARISMA_SPELL:
				ability=CLASS(rpc)->MaxCHA-(strlen(rpc->user.Cursed) ? 10 : 0);
				i=dice(10);
				if(backfire) {
					rpc->CHA-=(i>rpc->CHA) ? rpc->CHA : i;
					sprintf(outbuf,"You feel nasty (%d)",rpc->CHA);
				}
				else {
					if(rpc->CHA+i<ability) {
						rpc->CHA+=i;
						sprintf(outbuf,"You feel much more charismatic (%d)",rpc->CHA);
					}
					else {
						rpc->CHA=ability;
						strcpy(outbuf,"Stop being so vain.");
					}
				}
				break;
			case SHIELD_SPELL:
				if(backfire) {
					if(CLASS(rpc)->Magic>2 && rpc->user.ACmod>0)
						rpc->user.ACmod--;
					else
					if(rpc->ToAC>0)
						rpc->ToAC-=dice(rpc->ToAC);
					else
						rpc->ToAC--;
					sprintf(outbuf,"%s %s loses some of its effectiveness.",rpc->His,ARMOR(rpc)->Name);
				}
				else {
					if(CLASS(rpc)->Magic>2)
						rpc->user.ACmod++;
					rpc->ToAC++;
					sprintf(outbuf,"A magical field surrounds %s %s.",rpc->his,ARMOR(rpc)->Name);
					sound("shield",64);
				}
				if(-rpc->user.ACmod>=ARMOR(rpc)->Class || -(rpc->user.ACmod+rpc->ToAC)>=ARMOR(rpc)->Class) {
					rpc->armor_origin=0;rpc->armor_type=0;
					rpc->user.ACmod=0;rpc->ToAC=0;
					strmfe(rpc->user.Armor,ServerTable->armor[0]->Origin,ARMOR(rpc)->Name);
					sprintf(outbuf,"%s %s crumbles off of %s body!",rpc->His,ARMOR(rpc)->Name,rpc->his);
				}
				if(dice(3*(rpc->user.ACmod+rpc->ToAC+1)/CLASS(rpc)->Magic)>ARMOR(rpc)->Class) {
					sprintf(outbuf,"%s %s vaporizes!",rpc->His,ARMOR(rpc)->Name);
					rpc->armor_origin=0;rpc->armor_type=0;
					rpc->user.ACmod=0;rpc->ToAC=0;
					strmfe(rpc->user.Armor,ServerTable->armor[0]->Origin,ARMOR(rpc)->Name);
				}
				break;
			case HONE_SPELL:
				if(backfire) {
					if(CLASS(rpc)->Magic>2 && rpc->user.WCmod>0)
						rpc->user.WCmod--;
					else
					if(rpc->ToWC>0)
						rpc->ToWC-=dice(rpc->ToWC+1);
					else
						rpc->ToWC--;
					sprintf(outbuf,"%s %s loses some of its effectiveness.",rpc->His,WEAPON(rpc)->Name);
				}
				else {
					if(CLASS(rpc)->Magic>2)
						rpc->user.WCmod++;
					rpc->ToWC++;
					sprintf(outbuf,"%s %s glows with magical sharpness.",rpc->His,WEAPON(rpc)->Name);
					sound("hone",64);
				}
				if(-rpc->user.WCmod>=WEAPON(rpc)->Class || -(rpc->user.WCmod+rpc->ToWC)>=WEAPON(rpc)->Class) {
					rpc->weapon_origin=0;rpc->weapon_type=0;
					rpc->user.WCmod=0;rpc->ToWC=0;
					strmfe(rpc->user.Weapon,ServerTable->weapon[0]->Origin,WEAPON(rpc)->Name);
					sprintf(outbuf,"%s %s crumbles off of %s !",rpc->His,WEAPON(rpc)->Name,rpc->him);
				}
				if(dice(3*(rpc->user.WCmod+rpc->ToWC+1)/CLASS(rpc)->Magic)>WEAPON(rpc)->Class) {
					sprintf(outbuf,"%s %s vaporizes!",rpc->His,WEAPON(rpc)->Name);
					rpc->weapon_origin=0;rpc->weapon_type=0;
					rpc->user.WCmod=0;rpc->ToWC=0;
					strmfe(rpc->user.Weapon,ServerTable->weapon[0]->Origin,WEAPON(rpc)->Name);
				}
				break;
			case HEAL_SPELL:
				switch(CLASS(rpc)->Magic) {
					case 3:
						ability=rpc->user.Level/10+9;
						break;
					case 4:
						ability=rpc->user.Level/10+9;
						break;
					default:
						ability=16;
						break;
				}
				p=0;
				for(i=0; i<rpc->user.Level; i++)
					p+=dice(ability);
				if(backfire) {
					rpc->HP-=p;
					if(rpc->HP<1) {
						rpc->HP=0;
						if(rpc==ONLINE)
							strcpy(reason,"heal backfired");
					}
					sprintf(outbuf,"%s%s hurt%s %sself for %d hit points!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),rpc!=ONLINE ? "s" : "",rpc->him,p);
				}
				else {
					if(rpc->HP+p>rpc->user.HP)
						p=rpc->user.HP-rpc->HP+2;
					rpc->HP+=p;
					sprintf(outbuf,"%s%s heal%s %sself for %d hit points.",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),rpc!=ONLINE ? "s" : "",rpc->him,p);
					sound("heal",64);
				}
				break;
			case TELEPORT_SPELL:
				memset(outbuf,0,OUTBUF_SIZE);
				if(enemy) {
					sound("teleport",64);
					sprintf(outbuf,"%s%s teleports away from the battle!",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle);
					rpc->user.Current.Retreats++;
					rpc->user.History.Retreats++;
					paws=!PLAYER.Expert;
				}
				s=p;
				break;
			case BLAST_SPELL:
				sound("blast",64);
				switch(CLASS(rpc)->Magic) {
					case 2:
						ability=17;
						break;
					case 3:
						ability=17+rpc->user.Level/10-(backfire ? AC1 : AC2)/5;
						break;
					case 4:
						ability=17+rpc->user.Level/10-(backfire ? AC1 : AC2)/5;
						break;
					default:
						ability=17-(backfire ? AC1 : AC2)/5;
						break;
				}
				if(enemy->armor_origin<NUMARMOR) {
					if(ARMOR(enemy)->EgoMagic==CLASS(rpc)->Magic)
						ability-=dice(ability)/2+1;
					if(strcmp(ARMOR(enemy)->EgoCurse,CLASS(enemy)->Name)==0 || strcmp(ARMOR(enemy)->EgoCurse,"*")==0)
						ability+=dice(ability)/2+1;
				}
				if(CLASS(enemy)->Melee>3)
					ability*=CLASS(enemy)->Melee/2;
				p=0;
				for(i=0; i<rpc->user.Level; i++)
					p+=dice(ability);
				if(p>=CLASS(rpc)->blast.What) {
					Delay(100);
					strcpy(CLASS(rpc)->blast.Who,rpc->user.Handle);
					CLASS(rpc)->blast.What=p;
					CLASS(rpc)->blast.When=Julian;
					if(p>=ServerTable->class[0]->Character[0]->blast.What) {
						strcpy(ServerTable->class[0]->Character[0]->blast.Who,rpc->user.Handle);
						ServerTable->class[0]->Character[0]->blast.What=p;
						ServerTable->class[0]->Character[0]->blast.When=Julian;
					}
					RPGserver(SERVER_PUTHOF,NULL);
					sound("outstanding",48);
				}
				if(backfire) {
					rpc->HP-=p;
					if(rpc->HP<1) {
						rpc->HP=0;
						if(rpc==ONLINE)
							strcpy(reason,"blast backfired");
					}
					sprintf(outbuf,"%s%s blast%s %sself for %d hit points!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),rpc->him,p);
				}
				else {
					enemy->HP-=p;
					rpc->user.History.HP+=p;
					sprintf(outbuf,"%s%s blast%s %s for %d hit points!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),(enemy==ONLINE ? "you" : enemy->user.Handle),p);
				}
				break;
			case RESURRECT_SPELL:
				if(backfire) {
					strcpy(outbuf,"You die by your own doing.");
					rpc->HP=0;
					strcpy(rpc->user.Status,rpc->user.ID);
					if(rpc==ONLINE) {
						strcpy(reason,"resurrect backfired");
						logoff=TRUE;
					}
				}
				else {
					sprintf(prompt,"%sResurrect who? ",fore(CYN));
					if(GetRPC(RPC[1][0])) {
						sound("resurrect",64);
						sprintf(outbuf,"Now raising %s...",ENEMY.Handle);
						memset(&ENEMY.Status,0,sizeof(ENEMY.Status));
						RPGserver(SERVER_PUTUSER,(UBYTE *)&ENEMY);
						NL;
					}
				}
				break;
			case CONFUSION_SPELL:
				sound("confusion",64);
				if(backfire) {
					rpc->INT/=2;
					rpc->DEX/=2;
					rpc->Confused=TRUE;
					sprintf(outbuf,"%s%s blitz%s %sself with exploding %sc%so%sl%so%sr%ss%s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "es" : ""),rpc->him,fore(BRED),fore(YELLOW),fore(GREEN),fore(CYAN),fore(BLUE),fore(MAGENTA),fore(GRY));
				}
				else {
					enemy->INT/=2;
					enemy->DEX/=2;
					enemy->Confused=TRUE;
					sprintf(outbuf,"%s%s blitz%s %s with exploding %sc%so%sl%so%sr%ss%s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "es" : ""),(enemy==ONLINE ? "you" : enemy->user.Handle),fore(BRED),fore(YELLOW),fore(GREEN),fore(CYAN),fore(BLUE),fore(MAGENTA),fore(GRY));
				}
				break;
			case TRANSMUTE_SPELL:
				sound("transmute",64);
				if(backfire) {
					sprintf(outbuf,"%s%s transform%s %s %s into ",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),rpc->his,WEAPON(rpc)->Name);
					if(rpc==ONLINE)
						sprintf(line[numline],"%s transforms own %s into ",PLAYER.Handle,WEAPON(rpc)->Name);
					else
						sprintf(line[numline],"You transform your %s into ",WEAPON(rpc)->Name);
					rpc->weapon_origin=0;
					rpc->weapon_type=(dice(WEAPON(rpc)->Class)+dice(WEAPON(rpc)->Class)+dice(MAXWEAPON(0)))/3;
					strmfe(rpc->user.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[rpc->weapon_type]->Name);
					strcat(outbuf,AN(rpc->user.Weapon));
					strcat(outbuf,WEAPON(rpc)->Name);
					strcat(outbuf,"!");
					strcat(line[numline++],WEAPON(rpc)->Name);
					rpc->user.WCmod=0;rpc->ToWC=0;
				}
				else {
					sprintf(outbuf,"%s%s transform%s %s %s into ",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),enemy->his,WEAPON(enemy)->Name);
					if(rpc==ONLINE)
						sprintf(line[numline],"%s transforms your %s into ",PLAYER.Handle,WEAPON(enemy)->Name);
					else
						sprintf(line[numline],"You transform %s's %s into ",enemy->user.Handle,WEAPON(enemy)->Name);
					enemy->weapon_origin=0;
					enemy->weapon_type=(dice(WEAPON(enemy)->Class)+dice(WEAPON(enemy)->Class)+dice(MAXWEAPON(0)))/3;
					strmfe(enemy->user.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[enemy->weapon_type]->Name);
					strcat(outbuf,AN(rpc->user.Weapon));
					strcat(outbuf,WEAPON(enemy)->Name);
					strcat(outbuf,"!");
					strcat(line[numline++],WEAPON(enemy)->Name);
					enemy->user.WCmod=0;enemy->ToWC=0;
				}
				break;
			case CURE_SPELL:
				sound("cure",64);
				if(backfire) {
					enemy->HP=enemy->user.HP;
					sprintf(outbuf,"%s%s cure%s %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
				}
				else {
					if(rpc==ONLINE)
						strcpy(outbuf,"You feel your vitality completely restored.");
					else
						sprintf(outbuf,"%s%s cures %sself!",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,rpc->him);
					rpc->HP=rpc->user.HP;
				}
				break;
			case ILLUSION_SPELL:
				sound("illusion",64);
				if(backfire) {
					for(i=0; i<5; i++)
						if(rpc==RPC[0][i]) {
							i=1;
							break;
						}
					if(i==5)
						i=0;
					memcpy(RPC[i][4],enemy,sizeof(struct RPC));
					strcpy(RPC[i][4]->user.ID,"_IOU");
					sprintf(RPC[i][4]->user.Handle,"Image of %s",strlen(enemy->user.ID) ? enemy->user.ID : "IT");
					sprintf(outbuf,"%s%s render%s an image of %s%s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "yourself" : enemy->user.Handle));
				}
				else {
					for(i=0; i<5; i++)
						if(rpc==RPC[0][i]) {
							i=0;
							break;
						}
					if(i==5)
						i=1;
					memcpy(RPC[i][4],rpc,sizeof(struct RPC));
					strcpy(RPC[i][4]->user.ID,"_IOU");
					sprintf(RPC[i][4]->user.Handle,"Image of %s",strlen(rpc->user.ID) ? rpc->user.ID : "IT");
					sprintf(outbuf,"%s%s render%s an image of %sself!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),rpc->him);
				}
				break;
			case DISINTEGRATE_SPELL:
				sound("disintegrate",64);
				if(backfire) {
					sprintf(outbuf,"%s%s completely atomize%s %sself!",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,(rpc!=ONLINE ? "s" : ""),rpc->him);
					rpc->HP=0;
					if(rpc==ONLINE)
						strcpy(reason,"disintegrate backfired");
				}
				else {
					if(rpc==ONLINE)
						sprintf(outbuf,"You completely atomize %s%s!",(enemy->user.Gender=='I' ? "the " : ""),enemy->user.Handle);
					else
						sprintf(outbuf,"%s%s zaps %s%s's soul with one blow!",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,(enemy->user.Gender=='I' ? "the " : ""),enemy->user.Handle);
					rpc->user.History.HP+=enemy->HP;
					enemy->HP=0;
				}
				Delay(50);
				break;
			case MORPH_SPELL:
				sound("morph",64);
				ability=0;
				for(i=0; i<NUMCLASS; i++)
					ability+=MAXCLASS(i);
				ability=dice(ability)-1;
				for(i=0; i<NUMCLASS && ability>=MAXCLASS(i); i++)
					ability-=MAXCLASS(i);
				if(backfire) {
					rpc->class_origin=i;
					rpc->class_type=ability;
					strmfe(rpc->user.Class,ServerTable->class[rpc->class_origin]->Origin,CLASS(rpc)->Name);
					rpc->user.Level=0;
					rpc->user.Gender=dice(2)==1 ? 'M' : 'F';
					CreateRPC(rpc);
					if(strlen(rpc->user.ID) && *rpc->user.ID!='_') {
						RPGserver(SERVER_PUTUSER,(UBYTE *)&rpc->user);
						sprintf(line[numline++],"You morphed yourself into a level %u %s instead of %s!",rpc->user.Level,CLASS(rpc)->Name,enemy->user.Handle);
						note(rpc->user.ID);
						numline=0;
					}
					sprintf(outbuf,"%s%s%s morphed %sself into a level %u %s!",fore(YELLOW),(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,rpc->him,rpc->user.Level,CLASS(rpc)->Name);
					news(outbuf);
					sprintf(outbuf,"%s%s morph%s %sself into a level %u %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),rpc->him,rpc->user.Level,CLASS(rpc)->Name);
				}
				else {
					enemy->class_origin=i;
					enemy->class_type=ability;
					strmfe(enemy->user.Class,ServerTable->class[enemy->class_origin]->Origin,CLASS(enemy)->Name);
					enemy->user.Level=0;
					enemy->user.Gender=dice(2)==1 ? 'M' : 'F';
					CreateRPC(enemy);
					if(strlen(enemy->user.ID) && *enemy->user.ID!='_') {
						RPGserver(SERVER_PUTUSER,(UBYTE *)&enemy->user);
						sprintf(line[numline++],"%s%s morphed you into a level %u %s!",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,enemy->user.Level,CLASS(enemy)->Name);
						note(enemy->user.ID);
						numline=0;
					}
					if(rpc==ONLINE)
						sprintf(outbuf,"%smorphed %s into a level %u %s!",fore(YELLOW),enemy->user.Handle,enemy->user.Level,CLASS(enemy)->Name);
					else
					if(enemy==ONLINE)
						sprintf(outbuf,"%sgot morphed into a level %u %s by %s!",fore(YELLOW),enemy->user.Level,CLASS(enemy)->Name,rpc->user.Handle);
					else
						sprintf(outbuf,"%s%s%s morphed %s%s into a level %u %s!",fore(YELLOW),(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,(enemy->user.Gender=='I' ? AN(enemy->user.Handle) : ""),enemy->user.Handle,enemy->user.Level,CLASS(enemy)->Name);
					news(outbuf);
					sprintf(outbuf,"%s%s morph%s %s into a level %u %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),enemy->him,enemy->user.Level,CLASS(enemy)->Name);
				}
				break;
			}
	}
	if(s>16 && s<25) {
		switch(p) {
			case ARMOR_RUSTING_XSPELL:
				if(backfire) {
					sprintf(outbuf,"%s%s get%s swallowed by an acid mist...",fore(BRN),rpc->He,(rpc!=ONLINE ? "s" : ""));
					OUT(outbuf);
					Delay(75);NL;
					sprintf(outbuf,"%s%s damage%s %s own %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),rpc->his,(rpc->armor_origin<NUMARMOR ? ARMOR(rpc)->Name : "armor"));
					rpc->ToAC-=dice(AC1/5+1);
					rpc->user.ACmod-=dice(AC1/10+1);
				}
				else {
					sprintf(outbuf,"%sAn acid mist surrounds %s%s...",fore(BRN),(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
					OUT(outbuf);
					Delay(75);NL;
					sprintf(outbuf,"%s%s damage%s %s %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),enemy->his,(enemy->armor_origin<NUMARMOR ? ARMOR(enemy)->Name : "armor"));
					enemy->ToAC-=dice(AC2/5+1);
					enemy->user.ACmod-=dice(AC2/10+1);
				}
				break;
			case WEAPON_DECAY_XSPELL:
				sprintf(outbuf,"%sAn ultraviolet particle beam is emitted...",fore(MAG));
				OUT(outbuf);
				Delay(75);NL;
				if(backfire) {
					sprintf(outbuf,"%s%s damage%s %s own %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),rpc->his,(rpc->weapon_origin<NUMWEAPON ? WEAPON(rpc)->Name : "weapon"));
					rpc->ToWC-=dice(WC1/5+1);
					rpc->user.WCmod-=dice(WC1/10+1);
				}
				else {
					sprintf(outbuf,"%s%s damage%s %s %s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),enemy->his,(enemy->weapon_origin<NUMWEAPON ? WEAPON(enemy)->Name : "weapon"));
					enemy->ToWC-=dice(WC2/5+1);
					enemy->user.WCmod-=dice(WC2/10+1);
				}
				break;
			case BIG_BLAST_XSPELL:
				sprintf(outbuf,"%sA blinding flash erupts before %s...",fore(WHITE),(backfire ? rpc->him : enemy->him));
				OUT(outbuf);
				sound("blast",64);
				Delay(75);NL;
				switch(CLASS(rpc)->Magic) {
					case 2:
						ability=30;
						break;
					case 3:
						ability=30+rpc->user.Level/10-(backfire ? AC1 : AC2)/5;
						break;
					case 4:
						ability=30+rpc->user.Level/10-(backfire ? AC1 : AC2)/5;
						break;
					default:
						ability=30-(backfire ? AC1 : AC2)/5;
						break;
				}
				if(enemy->armor_origin<NUMARMOR) {
					if(ARMOR(enemy)->EgoMagic==CLASS(rpc)->Magic)
						ability-=dice(ability)/2+1;
					if(strcmp(ARMOR(enemy)->EgoCurse,CLASS(enemy)->Name)==0 || strcmp(ARMOR(enemy)->EgoCurse,"*")==0)
						ability+=dice(ability)/2+1;
				}
				if(CLASS(enemy)->Melee>3)
					ability*=CLASS(enemy)->Melee/2;
				p=0;
				for(i=0; i<rpc->user.Level; i++)
					p+=dice(ability);
				if(backfire) {
					rpc->HP-=p;
					if(rpc->HP<1) {
						rpc->HP=0;
						if(rpc==ONLINE)
							strcpy(reason,"BLAST backfired");
					}
					sprintf(outbuf,"%s%s BLAST%s %sself for %d hit points!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "S" : ""),rpc->him,p);
				}
				else {
					enemy->HP-=p;
					rpc->user.History.HP+=p;
					sprintf(outbuf,"%s%s BLAST%s %s for %d hit points!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "S" : ""),(enemy==ONLINE ? "you" : enemy->user.Handle),p);
				}
				break;
			case MANA_STEALING_XSPELL:
				sprintf(outbuf,"%sA glowing orb radiates above %s...",fore(CYAN),(backfire ? enemy->him : rpc->him));
				OUT(outbuf);
				Delay(75);NL;
				if(backfire) {
					modf((double)rpc->SP*1./(dice(3)+1.),&d);
					sprintf(outbuf,"%s%s absorb%s spell power (%g) from %s%s!",(enemy->user.Gender=='I' ? "The " : ""),(enemy==ONLINE ? "You" : enemy->user.Handle),(enemy!=ONLINE ? "s" : ""),d,(rpc->user.Gender=='I' ? "the " : ""),(rpc==ONLINE ? "you" : rpc->user.Handle));
					rpc->SP-=d;
					enemy->SP+=d;
				}
				else {
					modf((double)enemy->SP*1./(dice(3)+1.),&d);
					sprintf(outbuf,"%s%s absorb%s spell power (%g) from %s%s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),d,(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
					enemy->SP-=d;
					rpc->SP+=d;
				}
				break;
			case LIFE_STEALING_XSPELL:
				sprintf(outbuf,"%sA black finger extends and touches %s...",fore(BLACK),(backfire ? rpc->him : enemy->him));
				OUT(outbuf);
				Delay(75);NL;
				if(backfire) {
					modf(rpc->user.Experience*1./(dice(3)+1.),&d);
					rpc->user.Experience-=d;
					enemy->user.Experience+=(enemy->user.Level>rpc->user.Level) ? d : enemy->user.Experience;
					if(enemy==ONLINE)
						chkexp();
					sprintf(outbuf,"%s%s absorb%s life energy (%.8g) from %s%s!",(enemy->user.Gender=='I' ? "The " : ""),(enemy==ONLINE ? "You" : enemy->user.Handle),(enemy!=ONLINE ? "s" : ""),d,(rpc->user.Gender=='I' ? "the " : ""),(rpc==ONLINE ? "you" : rpc->user.Handle));
				}
				else {
					modf(enemy->user.Experience*1./(dice(3)+1.),&d);
					enemy->user.Experience-=d;
					rpc->user.Experience+=(rpc->user.Level>enemy->user.Level) ? d : rpc->user.Experience;
					if(rpc==ONLINE)
						chkexp();
					sprintf(outbuf,"%s%s absorb%s life energy (%.8g) from %s%s!",(rpc->user.Gender=='I' ? "The " : ""),(rpc==ONLINE ? "You" : rpc->user.Handle),(rpc!=ONLINE ? "s" : ""),d,(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
				}
				break;
		}
	}
	if(!enemy) {
		OUT(outbuf);NL;NL;
	}
	return(backfire ? -s : s);
}

void PoisonWeapon(struct RPC *rpc)
{
	int i,p=0;

	if(rpc==ONLINE) {
		if(!PLAYER.Poison) {
			OUT("You don't have any poison.");NL;
			return;
		}
		sprintf(prompt,"%sEnter type (?=list): ",fore(CYN));
		do {
			OUT(prompt);
			if(ins(2))
				break;
			NL;NL;
			if(!strlen(inbuf))
				return;
			if(*inbuf=='?') {
				for(i=0; i<NUMPOISON; i++)
					if(PLAYER.Poison & (UWORD)pow2((double)i)) {
						sprintf(outbuf,"%s%s %s",bracket(i+1),fore(GRY),POISON(i)->Name);
						OUT(outbuf);NL;
					}
				NL;
				continue;
			}
			p=atoi(inbuf);
		} while(p>NUMPOISON || !(PLAYER.Poison & (UWORD)pow2(p-1.)));
	}
	else
	if(rpc->user.Poison)
		while(!(rpc->user.Poison & (UWORD)pow2((p=dice(NUMPOISON))-1.)));
	if(p) {
		if(rpc->user.WCmod>0) {
			if(CLASS(rpc)->Poison>1)
				rpc->user.WCmod=CLASS(rpc)->Poison/2*p;
		}
		else
			rpc->user.WCmod+=CLASS(rpc)->Poison/2*p;
		if(rpc->ToWC>0)
			rpc->ToWC=(CLASS(rpc)->Poison+1)/2*p;
		else
			rpc->ToWC+=(CLASS(rpc)->Poison+1)/2*p;
		if(rpc->user.Gender=='I')
			sprintf(outbuf,"It secretes a caustic ooze %s(%s%+d%s,%s%+d%s)%s.",fore(MAG),fore(YELLOW),rpc->user.WCmod,fore(GRY),fore(YELLOW),rpc->ToWC,fore(MAG),fore(GRY));
		else
			sprintf(outbuf,"%s pour%s some poison on %s %s.",rpc->He,(rpc!=ONLINE ? "s" : ""),rpc->his,WEAPON(rpc)->Name);
		OUT(outbuf);NL;
		sound("hone",64);
		Delay(25);
		if(rpc->user.Gender!='I' && rpc->weapon_origin<NUMWEAPON && (rpc->ToWC+rpc->user.WCmod)>WEAPON(rpc)->Class) {
			if(rpc->user.WCmod>0) {
				rpc->user.WCmod=WEAPON(rpc)->Class-rpc->ToWC;
				if(rpc->user.WCmod<0) {
					rpc->ToWC+=rpc->user.WCmod;
					rpc->user.WCmod=0;
				}
			}
			else
				rpc->ToWC=WEAPON(rpc)->Class;
			sprintf(outbuf,"%s weapon has as much as it can hold; the rest just drips off.",rpc->His);
			OUT(outbuf);NL;
			Delay(25);
		}
		if(rpc!=ONLINE || dice(CLASS(rpc)->Poison)==1) {
			rpc->user.Poison-=pow2(p-1.);
			if(rpc==ONLINE) {
				OUT("You toss the empty vial aside.");NL;
				Delay(25);
			}
		}
		NL;
	}
}

char Attack(int volley,struct RPC *rpc,struct RPC *enemy)
{
	int i,n;
	char c;

	if(rpc->Confused) {
		if(rpc->INT<rpc->user.INT) {
			rpc->INT+=CLASS(rpc)->BonusINT;
			if(rpc->INT>rpc->user.INT)
				rpc->INT=rpc->user.INT;
		}
		if(rpc->DEX<rpc->user.DEX) {
			rpc->DEX+=CLASS(rpc)->BonusDEX;
			if(rpc->DEX>rpc->user.DEX)
				rpc->DEX=rpc->user.DEX;
		}
	}
	if(rpc==ONLINE) {
		displayview();
		if(!volley) {
			c=dice(20);
			if((CLASS(rpc)->Backstab>1 && c>1) || (CLASS(rpc)->Backstab>0 && c==20)) {
				do {
					sprintf(prompt,"%sAttempt to backstab (Y/N)? ",fore(CYN));
					OUT(prompt);
					c=inkey('Y','Y');
					NL;NL;
				} while(c!='Y' && c!='N');
				c=(c=='Y') ? (CLASS(rpc)->Backstab<2 ? 2 : CLASS(rpc)->Backstab) : 1;
				n=Melee(rpc,enemy,c);
				enemy->HP-=n;
			}
			else {
				n=Melee(rpc,enemy,1);
				enemy->HP-=n;
			}
		}
		else
			do {
				NL;
				sprintf(outbuf,"%s[%s%u%s,",fore(BLU),fore((rpc->HP>(PLAYER.HP*2/3)) ? GREEN : (rpc->HP>(PLAYER.HP/3)) ? YELLOW : BRED),rpc->HP,fore(CYAN));
				OUT(outbuf);
				OUT(fore((enemy->HP>(enemy->user.HP*2/3)) ? GREEN : (enemy->HP>(enemy->user.HP/3)) ? YELLOW : BRED));
				if(rpc->INT>=50 && rpc->INT<100) {
					i=2*(100-rpc->INT);
					n=(enemy->HP/i+.5);
					n*=i;
					sprintf(outbuf,"%d",n);
				}
				if(rpc->INT<50 || n==0)
					sprintf(outbuf,"%s",(enemy->HP>(enemy->user.HP*2/3)) ? "Healthy" : (enemy->HP>(enemy->user.HP/3)) ? "Hurting" : "Weak");
				if(rpc->INT>99)
					sprintf(outbuf,"%d",enemy->HP);
				OUT(outbuf);
				sprintf(outbuf,"%s] ",fore(BLU));
				OUT(outbuf);
				sprintf(prompt,"%s<A> Attack, ",fore(CYN));
				if(CLASS(rpc)->Magic && rpc->user.Spell && rpc->SP)
					strcat(prompt,"<C> Cast spell, ");
				strcat(prompt,"<R> Retreat, <Y> Status: ");
				OUT(prompt);
				c=inkey('A','A');
				NL;NL;
				switch(c) {
					case 'A':
						n=Melee(rpc,enemy,1);
						PLAYER.History.HP+=n;
						enemy->HP-=n;
						break;
					case 'C':
						if(!(n=Cast(rpc,enemy)))
							c='?';
						if(abs(n)==TELEPORT_SPELL) {
							if(from=='T') {
								OUT("Tiny pulls you back from the anti-matter stream!");NL;
								c='\0';
								break;
							}
							c='R';
							PLAYER.Current.Retreats++;
							PLAYER.History.Retreats++;
						}
						break;
					case 'R':
						if(from=='L') {
							OUT("\"You can never escape the taxman!\"");NL;
							c=NULL;
							break;
						}
						if(from=='T') {
							OUT("You try to escape, but the crowd throws you back for the slaughter!");NL;
							c=NULL;
							break;
						}
						n=50+rpc->DEX/2;
						n+=((int)rpc->DEX-(int)enemy->DEX)/2;
						if(n<5)
							n=5;
						if(n>99)
							n=99;
						if(dice(100)>n) {
							switch(dice(5)) {
								case 1:
									sprintf(outbuf,"You trip and fail in your attempt to retreat.");
									break;
								case 2:
									sprintf(outbuf,"%s%s pulls you back into the battle.",(enemy->user.Gender=='I' ? "The " : ""),enemy->user.Handle);
									break;
								case 3:
									sprintf(outbuf,"%s%s prevents your retreat and says, \"I'm not through with you yet!\"",(enemy->user.Gender=='I' ? "The " : ""),enemy->user.Handle);
									break;
								case 4:
									sprintf(outbuf,"%s%s outmaneuvers you and says, \"You started this, I'm finishing it.\"",(enemy->user.Gender=='I' ? "The " : ""),enemy->user.Handle);
									break;
								case 5:
									sprintf(outbuf,"%s%s blocks your path and says, \"Where do you want to go today?\"",(enemy->user.Gender=='I' ? "The " : ""),enemy->user.Handle);
									break;
							}
							c='A';
							break;
						}
						PLAYER.Current.Retreats++;
						PLAYER.History.Retreats++;
						sprintf(line[numline++],"%s, the coward, retreated from you.",PLAYER.Handle);
						paws=!PLAYER.Expert;
						switch(dice(5)) {
							case 1:
								sprintf(outbuf,"You are successful in your attempt to retreat.");
								break;
							case 2:
								sprintf(outbuf,"You limp away from the battle.");
								break;
							case 3:
								sprintf(outbuf,"You decide this isn't worth the effort.");
								break;
							case 4:
								sprintf(outbuf,"You listen to the voice in your head yelling, \"Run!\"");
								break;
							case 5:
								sprintf(outbuf,"You say, \"%s who fights and runs away lives to fight another day!\"",(PLAYER.Gender=='M' ? "He" : "She"));
								break;
						}
						break;
					case 'Y':
						yourstats();
						break;
				}
			} while(!strchr("ARC",c));
	}
	else {
		if(!volley && dice((100-rpc->user.Level)/5+5)<CLASS(rpc)->Poison)
			PoisonWeapon(rpc);
		c='A';
		if(rpc->INT>(70-5*CLASS(rpc)->Magic) || !rpc->Confused)
			switch(CLASS(rpc)->Magic) {
				case 1:
					if(dice(6)==1)
						c='C';
					break;
				case 2:
					if(dice(5)==1)
						c='C';
					break;
				case 3:
					if(dice(4*rpc->HP/rpc->user.HP+2)==1)
						c='C';
					break;
				case 4:
					if(dice(3*rpc->HP/rpc->user.HP+2)==1)
						c='C';
					break;
			}
		if(c=='C') {
			if(!(n=Cast(rpc,enemy)))
				c='A';
			if(abs(n)==TELEPORT_SPELL)
				c='r';
		}
		if(c=='A') {
			if(rpc->user.Coward && (BYTE)(rpc->user.ExpLevel-enemy->user.ExpLevel)>3 && rpc->HP<rpc->user.HP/4) {
				memset(rpc->user.Blessed,0,sizeof(rpc->user.Blessed));
				if(strlen(enemy->user.ID))
					strcpy(rpc->user.Cursed,enemy->user.ID);
				sprintf(outbuf,"%s%s runs away from the battle!",(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle);
				rpc->user.Current.Retreats++;
				rpc->user.History.Retreats++;
				c='r';
			}
			else {
				n=Melee(rpc,enemy,1);
				rpc->user.History.HP+=n;
				enemy->HP-=n;
			}
		}
	}

	if(enemy->HP<1) {
		enemy->HP=0;
		if(from=='P') {
			if(c=='A')
				switch(dice(5)) {
					case 1:
						sprintf(outbuf,"%s makes a fatal blow to %s.",rpc->user.Handle,enemy->user.Handle);
						break;
					case 2:
						sprintf(outbuf,"%s blows %s away.",rpc->user.Handle,enemy->user.Handle);
						break;
					case 3:
						sprintf(outbuf,"%s laughs, then kills %s.",rpc->user.Handle,enemy->user.Handle);
						break;
					case 4:
						sprintf(outbuf,"%s easily slays %s.",rpc->user.Handle,enemy->user.Handle);
						break;
					case 5:
						sprintf(outbuf,"%s makes minced-meat out of %s.",rpc->user.Handle,enemy->user.Handle);
						break;
				}
			if(c=='C')
				strcat(outbuf," {RIP}");
		}
		else {
			OUT(outbuf);NL;NL;
			if(rpc==ONLINE) {
				PLAYER.Current.Kills++;
				PLAYER.History.Kills++;
				enemy->user.Current.Killed++;
				enemy->user.History.Killed++;
				sprintf(outbuf,"%sYou killed %s%s!",fore(YELLOW),(enemy->user.Gender=='I' ? "the " : ""),enemy->user.Handle);
				if(enemy->user.Gender!='I') {
					sound("kill",64);
					Delay(50);
				}
			}
			else {
				rpc->user.Current.Kills++;
				rpc->user.History.Kills++;
				enemy->user.Current.Killed++;
				enemy->user.History.Killed++;
				sprintf(outbuf,"%s%s%s killed %s%s!",fore(YELLOW),(rpc->user.Gender=='I' ? "The " : ""),rpc->user.Handle,(enemy->user.Gender=='I' ? "the " : ""),(enemy==ONLINE ? "you" : enemy->user.Handle));
				if(enemy==ONLINE) {
					sound("killed",64);
					if(strlen(rpc->user.ID))
						sprintf(reason,"defeated by %s",rpc->user.Handle);
					else
						sprintf(reason,"defeated by a level %u %s",rpc->user.Level,rpc->user.Handle);
				}
			}
		}
	}
	return(c);
}

void Battle(void)
{
	struct mob {
		struct RPC *rpc;
		int reaction;
	} mob[2][5];
	int alive[2],i,j,k,l,o,member[2],volley;
	char c=NULL;

	numline=0;	//	start recording any incidents during this battle...

	for(volley=0; volley<10000 && c!='R';) {
		for(i=0; i<2; i++) {
			member[i]=0;
			for(j=0; j<5; j++) {
				mob[i][j].rpc=NULL;
				mob[i][j].reaction=0;
				if(RPC[i][j]->HP>0) {
					mob[i][member[i]].rpc=RPC[i][j];
					mob[i][member[i]].reaction=RPC[i][j]->user.Level+dice(RPC[i][j]->DEX);
					k=member[i];
					while(k>0 && mob[i][k].reaction>mob[i][k-1].reaction) {
						swmem(&mob[i][k],&mob[i][k-1],sizeof(struct mob));
						k--;
					}
					member[i]++;
				}
			}
		}
		if(!(alive[0]=member[0]))
			break;
		if(!(alive[1]=member[1]))
			break;

		for(k=0; alive[0] && alive[1] && (k<member[0] || k<member[1]); k++) {
			i=(mob[0][k].reaction>mob[1][k].reaction) ? 0 : 1;
			for(l=0; alive[0] && alive[1] && l<2; i^=1,l++) {
				if(!mob[i][k].rpc)
					continue;
				if(mob[i][k].rpc->HP<1)
					continue;
				o=i^1;
				do {
					j=dice(member[o])-1;
				} while(mob[o][j].rpc->HP<1);
				c=Attack(volley++,mob[i][k].rpc,mob[o][j].rpc);
				OUT(fore(i==0 ? WHITE : GRY));
				OUT(outbuf);
				NORMAL;NL;
				if(c=='R') {
					alive[i]=0;
					break;
				}
				if(c=='r')	//  left the battle under own power
					mob[i][k].rpc->HP=-1;
				if(mob[i][k].rpc->HP<1)
					alive[i]--;
				if(mob[o][j].rpc->HP<1)
					alive[o]--;
			}
		}
		NL;
		if(!ONLINE->HP) {
			displayview();
			Delay(100);
		}
	}
	if(!member[0])
		logoff=TRUE;
	NORMAL;
	if(ONLINE->Confused) {
		ONLINE->INT=PLAYER.INT+(strlen(PLAYER.Blessed) ? 10 : strlen(PLAYER.Cursed) ? -10 : 0);
		if(ONLINE->INT>100)
			ONLINE->INT=100;
		ONLINE->DEX=PLAYER.DEX+(strlen(PLAYER.Blessed) ? 10 : strlen(PLAYER.Cursed) ? -10 : 0);
		if(ONLINE->DEX>100)
			ONLINE->DEX=100;
		ONLINE->Confused=FALSE;
	}
	memset(RPC[0][4],0,sizeof(struct RPC));
	for(i=0; i<2; i++)
		for(j=i^1; j<4; j++)
			if(strlen(RPC[i][j]->user.ID) && *RPC[i][j]->user.ID!='_' && (RPC[i][j]->HP>0 || from=='P') && CLASS(RPC[i][j])->Magic) {
				l=(CLASS(RPC[i][j])->Magic>1) ? 0 : 1;
				if(!(RPC[i][j]->user.Spell & HEAL_SPELL) && RPC[i][j]->user.Bank>=MAGIC(6)->Cost[l]) {
					RPC[i][j]->user.Spell|=HEAL_SPELL;
					RPC[i][j]->user.Bank-=MAGIC(6)->Cost[l];
				}
				if(!(RPC[i][j]->user.Spell & TELEPORT_SPELL) && RPC[i][j]->user.Bank>=MAGIC(7)->Cost[l]) {
					RPC[i][j]->user.Spell|=TELEPORT_SPELL;
					RPC[i][j]->user.Bank-=MAGIC(7)->Cost[l];
				}
				if(!(RPC[i][j]->user.Spell & BLAST_SPELL) && RPC[i][j]->user.Bank>=MAGIC(8)->Cost[l]) {
					RPC[i][j]->user.Spell|=BLAST_SPELL;
					RPC[i][j]->user.Bank-=MAGIC(8)->Cost[l];
				}
				RPGserver(SERVER_PUTUSER,(UBYTE *)&RPC[i][j]->user);
			}
	displayview();
}

void CreateRPC(struct RPC *rpc)
{
	int i;

	if(!rpc->user.Level)
		rpc->user.Level=dice(99);
	if(!strlen(rpc->user.Weapon)) {
		i=MAXWEAPON(0)*rpc->user.Level/100+dice(3)-2;
		i=(i<1) ? 1 : (i>=MAXWEAPON(0)) ? MAXWEAPON(0)-1 : i;
		strmfe(rpc->user.Weapon,ServerTable->weapon[0]->Origin,ServerTable->weapon[0]->Item[i]->Name);
	}
	if(!strlen(rpc->user.Armor)) {
		i=MAXARMOR(0)*rpc->user.Level/100+dice(3)-2;
		i=(i<1) ? 1 : (i>=MAXARMOR(0)) ? MAXARMOR(0)-1 : i;
		strmfe(rpc->user.Armor,ServerTable->armor[0]->Origin,ServerTable->armor[0]->Item[i]->Name);
	}
	rpc->user.ExpLevel=rpc->user.Level;
	rpc->user.Experience=EXP(rpc->user.Level-1);
	LoadRPC(rpc);
	rpc->user.HP=15;
	rpc->user.SP=(CLASS(rpc)->Magic>1) ? 15 : 0;
	if(!CLASS(rpc)->Magic) {
		rpc->user.Spell=FALSE;
		rpc->user.XSpell=FALSE;
	}
	if(!CLASS(rpc)->Poison)
		rpc->user.Poison=FALSE;
	rpc->user.STR=CLASS(rpc)->BaseSTR;
	rpc->user.INT=CLASS(rpc)->BaseINT;
	rpc->user.DEX=CLASS(rpc)->BaseDEX;
	rpc->user.CHA=CLASS(rpc)->BaseCHA;
	for(i=2; i<=rpc->user.Level; i++) {
		rpc->user.STR+=CLASS(rpc)->BonusSTR;
		if(rpc->user.STR>CLASS(rpc)->MaxSTR)
			rpc->user.STR=CLASS(rpc)->MaxSTR;
		rpc->user.INT+=CLASS(rpc)->BonusINT;
		if(rpc->user.INT>CLASS(rpc)->MaxINT)
			rpc->user.INT=CLASS(rpc)->MaxINT;
		rpc->user.DEX+=CLASS(rpc)->BonusDEX;
		if(rpc->user.DEX>CLASS(rpc)->MaxDEX)
			rpc->user.DEX=CLASS(rpc)->MaxDEX;
		rpc->user.CHA+=CLASS(rpc)->BonusCHA;
		if(rpc->user.CHA>CLASS(rpc)->MaxCHA)
			rpc->user.CHA=CLASS(rpc)->MaxCHA;
		rpc->user.HP+=i+dice(i)+rpc->user.STR/10;
		if(rpc->user.SP)
			rpc->user.SP+=i+dice(i)+rpc->user.INT/10;
	}
	LoadRPC(rpc);
}

void ExchangeWeapon(struct RPC *rpc,struct RPC *enemy)
{
	int flag=FALSE;
	char c;

	if(!enemy->weapon_origin && !enemy->weapon_type)
		return;

	if(enemy->weapon_origin>=NUMWEAPON)
		return;

	if(WEAPON(rpc)->Class>WEAPON(enemy)->Class && rpc->user.WCmod>=0 && !rpc->weapon_origin && !enemy->weapon_origin)
		return;

	if(WEAPON(rpc)->Class==WEAPON(enemy)->Class || rpc->user.WCmod!=0 || rpc->weapon_origin || enemy->weapon_origin) {
		if(rpc==ONLINE) {
			sprintf(prompt,"%sDo you want %s %s (Y/N)? ",fore(CYN),enemy->his,WEAPON(enemy)->Name);
			OUT(prompt);
			while(!strchr("YN",(c=inkey('N','N'))))
				RUBOUT;
			NL;
			flag=(c=='Y');
		}
		else {
			d=value(WEAPON(rpc)->Value,rpc->CHA);
			if(rpc->user.WCmod)
				modf(d*(WEAPON(rpc)->Class+rpc->user.WCmod/(CLASS(rpc)->Poison+1))/WEAPON(rpc)->Class,&d);
			if(d<value(WEAPON(enemy)->Value,rpc->CHA))
				flag=TRUE;
		}
	}
	else
		flag=TRUE;
	if(flag) {
		sprintf(line[numline++],"%s also took %s %s.",(rpc==ONLINE ? "and" : "You"),(rpc==ONLINE ? "your" : PLAYER.Gender=='M' ? "his" : PLAYER.Gender=='F' ? "her" : "its"),WEAPON(enemy)->Name);
		sprintf(outbuf,"%s take%s %s %s%s.",rpc->He,(rpc!=ONLINE ? "s" : ""),enemy->his,(enemy->user.Gender=='I' && enemy->user.WCmod>0 ? "super " : ""),WEAPON(enemy)->Name);
		OUT(outbuf);NL;
		swmem(&rpc->weapon_origin,&enemy->weapon_origin,sizeof(enemy->weapon_origin));
		swmem(&rpc->weapon_type,&enemy->weapon_type,sizeof(enemy->weapon_type));
		swmem(rpc->user.Weapon,enemy->user.Weapon,sizeof(rpc->user.Weapon));
		if(enemy->user.Gender!='I') {	//	change of owner -- penalize magical sharpness
			if(rpc->user.WCmod>0)
				rpc->user.WCmod/=2;
			if(enemy->user.WCmod>0)
				enemy->user.WCmod/=2;
		}
		swmem(&rpc->user.WCmod,&enemy->user.WCmod,sizeof(rpc->user.WCmod));
		rpc->ToWC=(enemy->user.Gender=='I') ? enemy->ToWC : 0;
		enemy->ToWC=0;
	}
	if((from=='A' || from=='M' || from=='S') && rpc==ONLINE && !strlen(enemy->user.ID)) {
		d=value(WEAPON(enemy)->Value,rpc->CHA);
		if(enemy->user.WCmod!=0)
			modf(d*(WEAPON(enemy)->Class+enemy->user.WCmod)/WEAPON(enemy)->Class,&d);
		if(enemy->ToWC<0)
			modf(d*(WEAPON(enemy)->Class+enemy->ToWC)/WEAPON(enemy)->Class,&d);
		if(enemy->weapon_origin==0 && enemy->weapon_type==0)
			d=0.;
		if(d>0.) {
			PLAYER.Gold+=d;
			sprintf(outbuf,"You get %s for selling its %s",money(d,ANSI),WEAPON(enemy)->Name);
			OUT(outbuf);
			if(enemy->user.WCmod || enemy->ToWC) {
				sprintf(outbuf," %s(%s%+d%s,%s%+d%s)",fore(MAG),fore(enemy->user.WCmod>0 ? YELLOW : enemy->user.WCmod<0 ? BRED : GRY),enemy->user.WCmod,fore(GRY),fore(enemy->ToWC>0 ? YELLOW : enemy->ToWC<0 ? BRED : GRY),enemy->ToWC,fore(MAG));
				OUT(outbuf);
			}
			OUT(".");NL;
		}
	}
}

void ExchangeArmor(struct RPC *rpc,struct RPC *enemy)
{
	int flag=FALSE;
	char c;

	if(!enemy->armor_origin && !enemy->armor_type)
		return;

	if(enemy->armor_origin>=NUMARMOR)
		return;

	if(ARMOR(rpc)->Class>ARMOR(enemy)->Class && rpc->user.ACmod>=0 && !rpc->armor_origin && !enemy->armor_origin)
		return;

	if(ARMOR(rpc)->Class==ARMOR(enemy)->Class || rpc->user.ACmod!=0 || rpc->armor_origin || enemy->armor_origin) {
		if(rpc==ONLINE) {
			sprintf(prompt,"%sDo you want %s %s (Y/N)? ",fore(CYN),enemy->his,ARMOR(enemy)->Name);
			OUT(prompt);
			while(!strchr("YN",(c=inkey('N','N'))))
				RUBOUT;
			NL;
			flag=(c=='Y');
		}
		else {
			d=value(ARMOR(rpc)->Value,rpc->CHA);
			if(rpc->user.ACmod)
				modf(d*(ARMOR(rpc)->Class+rpc->user.ACmod)/ARMOR(rpc)->Class,&d);
			if(d<value(ARMOR(enemy)->Value,rpc->CHA))
				flag=TRUE;
		}
	}
	else
		flag=TRUE;
	if(flag) {
		sprintf(line[numline++],"%s also took %s %s.",(rpc==ONLINE ? "and" : "You"),(rpc==ONLINE ? "your" : PLAYER.Gender=='M' ? "his" : PLAYER.Gender=='F' ? "her" : "its"),ARMOR(enemy)->Name);
		sprintf(outbuf,"%s take%s %s %s.",rpc->He,(rpc!=ONLINE ? "s" : ""),enemy->his,ARMOR(enemy)->Name);
		OUT(outbuf);NL;
		swmem(&rpc->armor_origin,&enemy->armor_origin,sizeof(enemy->armor_origin));
		swmem(&rpc->armor_type,&enemy->armor_type,sizeof(enemy->armor_type));
		swmem(rpc->user.Armor,enemy->user.Armor,sizeof(rpc->user.Armor));
		if(rpc->user.ACmod>0)	//	change of owner -- penalize shimmering field
			rpc->user.ACmod/=2;
		if(enemy->user.ACmod>0)
			enemy->user.ACmod/=2;
		swmem(&rpc->user.ACmod,&enemy->user.ACmod,sizeof(rpc->user.ACmod));
		rpc->ToAC=0;
	}
	if((from=='A' || from=='M' || from=='S') && rpc==ONLINE && !strlen(enemy->user.ID)) {
		d=value(ARMOR(enemy)->Value,rpc->CHA);
		if(enemy->user.ACmod!=0)
			modf(d*(ARMOR(enemy)->Class+enemy->user.ACmod)/ARMOR(enemy)->Class,&d);
		if(enemy->ToAC<0)
			modf(d*(ARMOR(enemy)->Class+enemy->ToAC)/ARMOR(enemy)->Class,&d);
		if(enemy->armor_origin==0 && enemy->armor_type==0)
			d=0.;
		if(d>0.) {
			PLAYER.Gold+=d;
			sprintf(outbuf,"You get %s for selling its %s",money(d,ANSI),ARMOR(enemy)->Name);
			OUT(outbuf);
			if(enemy->user.ACmod || enemy->ToAC) {
				sprintf(outbuf," %s(%s%+d%s,%s%+d%s)",fore(MAG),fore(enemy->user.ACmod>0 ? YELLOW : enemy->user.ACmod<0 ? BRED : GRY),enemy->user.ACmod,fore(GRY),fore(enemy->ToAC>0 ? YELLOW : enemy->ToAC<0 ? BRED : GRY),enemy->ToAC,fore(MAG));
				OUT(outbuf);
			}
			OUT(".");NL;
		}
	}
}
