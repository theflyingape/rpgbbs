/* Prototypes for functions defined in
rpgbbs.c
 */

void __asm GetSystemDate(register __a0 UBYTE * );

void __asm GetSystemTime(register __a0 UBYTE * );

void __asm jdate(register __a0 UBYTE * , register __d0 JDATE );

void __asm fdate(register __a0 UBYTE * , register __d0 JDATE );

void __asm ftime(register __a0 UBYTE * , register __a1 UBYTE * );

void __asm julian(register __a0 JDATE * , register __a1 UBYTE * );

void __asm gold(register __a0 DOUBLE * , register __a1 UBYTE * );

void __asm phone(register __a0 UBYTE * );

void __asm proper(register __a0 UBYTE * );

int __asm cuss(register __a0 UBYTE * );

extern UBYTE __far alternate[32];

extern UWORD __far OLD[2][8];

extern UWORD __far AGA[2][32];

extern UBYTE __far keyboard[5][104];

void __asm current(register __a0 struct SERCON * );

void __asm scroll(register __a0 struct SERCON * , register __d0 int );

void __asm DecodeANSI(register __a0 struct SERCON * , register __a1 UBYTE * );

void __asm DrawCursor(register __a0 struct SERCON * , register __d0 UBYTE );

void __asm input(register __a0 struct SERCON * , register __a1 UBYTE * , register __a2 UBYTE * , register __d0 ULONG , register __d1 ULONG );

void __asm output(register __a0 struct SERCON * , register __a1 UBYTE * );

void __asm plot(register __a0 struct SERCON * , register __d0 UBYTE , register __d1 UBYTE );

void __asm OpenConsole(register __a0 struct SERCON * );

void __asm CloseConsole(register __a0 struct SERCON * );

void __asm WriteConsole(register __a0 struct SERCON * , register __a1 UBYTE * );

void __asm WriteSerial(register __a0 struct SERCON * , register __a1 UBYTE * );

void __asm SendPXL(register __a0 struct SERCON * , register __a1 UBYTE * );

void __stdargs _XCEXIT(long );

