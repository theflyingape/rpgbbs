#ifndef	DTLIB_PROTOS_H
#define	DTLIB_PROTOS_H

void Dispatch(void);
short OwnPlayer(void);
short DisownPlayer(void);
short SetModulePath(char *pathname);
short UnloadModule(void);
short LoadModule(char *filename);
short SetSaveFormat(unsigned short format);
short SaveModule(char *filename);
short SetFilter(unsigned short flag);
short SetChannelEnable(unsigned short enablebits);
short SetStartPosition(unsigned short startpos, unsigned short startnote);
short SetEndPosition(unsigned short endpos, unsigned short endnote);
short SetIterations(unsigned short numiters);
short PausePlaySeq(void);
short ContinuePlaySeq(void);
short SetGlobalVolume(unsigned short volume);
short SetInstVolume(unsigned short instnum, unsigned short volume);
short SetDefaultTempo(unsigned short tempo);
short SetGlobalTempo(short tempo);
short SetFineTempo(short finetempo);
short FadeGlobalVolume(unsigned short destvolume);
short SetVolumeFadeRate(unsigned short faderate);
short BeginPlaySeq(void);
short ApplyGlobalTempo(void);
short RetitleModule(char *newtitle);
short SetCurPosition(unsigned short curpos, unsigned short curnote);
short VerifyModule(char *filename);
short SetTimingMode(unsigned short mode);
short SetAutoTiming(unsigned short flag);
short SetLoopIntent(unsigned short flag);
short ResetDefaults(void);
short SetTempoInt(unsigned short flag);
short AddStatusSignal( unsigned short bitnum, unsigned short eventmask );
short RemStatusSignal( unsigned short bitnum );
short StopPlaySeq(void);
short InstallModule( void *moduleaddr );
short CalcElapsedTime( struct timescaninfo *tsi );
short SetTemporalStart( unsigned long time );
short SetTemporalEnd( unsigned long time );
short SetTemporalPos( unsigned long time );
long  CalcVolFadeTime( unsigned short startvol, unsigned short endvol, unsigned short rate );
#endif
