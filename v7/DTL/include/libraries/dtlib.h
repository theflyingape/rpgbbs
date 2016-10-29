#ifndef	DTLIB_H
#define	DTLIB_H

/*==========================================================*/
/* DTLib.H -- External declarations for DES-Tracker Library */
/*															*/
/*				Release: 2.00								*/
/*				Version: 3.0								*/
/*															*/
/* Designed and written by Darren Schebek					*/
/* Copyright (c)1992,1993 by Darren Schebek					*/
/*															*/
/* rel 1.1         Begun: Sun, Aug 31, 1991					*/
/* rel 1.2  Last updated: Fri, Sep  4, 1992					*/
/* rel 1.5  Last updated: Fri, Jan  1, 1993					*/
/* rel 1.6  Last updated: Thu, Jan 14, 1993					*/
/* rel 1.61 Last updated: Sun, Jan 27, 1993					*/
/* rel 1.62 Last updated: Sat, Jan  6, 1993					*/
/* rel 1.63 Last updated: Wed, Jan 10, 1993					*/
/* rel 1.64 Last updated: Thu, May 22, 1993					*/
/* rel 2.00 Last updated: Sat, Dec 18, 1993					*/
/*==========================================================*/


/*===================================*/
/* Library Data Structures follow... */
/*===================================*/


/*------------------------------------------------------------------------------*/
/* CHANNELSTATUS structure follows. DTL_Base contains four instances of this	*/
/* structure, called dtl_Channel0, dtl_Channel1, etc.  This structure contains	*/
/* information concerning an individual sound channel (0 to 3).  The info		*/
/* contained here is only valid when a module is loaded and a play sequence		*/
/* is active...																	*/

struct CHANNELSTATUS
	{
	UBYTE	cs_Note;		 /* The # of last note played (0..11).					*/
	UBYTE	cs_Octave;		 /* The octave # of the last note played (0..2).		*/
	UBYTE	cs_NoteNum;		 /* cs_Note & cs_Octave combined (Note+(12*Octave)).	*/
	UBYTE	cs_Volume;		 /* Current volume of this channel (0..64).				*/
	UBYTE	cs_RealVolume;	 /* Cur. channel volume with Global Volume applied.		*/
	UBYTE	cs_Pad;			 /* Reserved.  Always zero.								*/
	UWORD	cs_Period;		 /* Current period of this channel.						*/
	APTR	cs_Instrument;	 /* Pointer to INSTSTATUS struct used by this channel.	*/
	ULONG	cs_SampleOffset; /* Offset from inst. start if SampleOffset was used.	*/
	};


/*------------------------------------------------------------------------------*/
/* MODULESTATUS structure follows.  DTL_Base contains an instance of this		*/
/* structure.  It contains information specific to the currently loaded module.	*/
/* This information is valid only when a module is loaded...					*/

struct MODULESTATUS
	{
	APTR	ms_Name;            /* Pointer to title of loaded song module		*/
	ULONG	ms_Length;			/* Length of entire module in memory.			*/
	APTR	ms_PlaySeqArray;	/* Ptr to play seq. array (dbl-long elements).	*/
	APTR	ms_PatternData;		/* Ptr to the module's pattern data.			*/
	APTR	ms_InstInfoBlocks;  /* Ptr to 31 internal instrument blocks (30 bytes each). */
	UBYTE	ms_Format;			/* Either MF_SOUNDTRACKER or MF_NOISETRACKER.	*/
	UBYTE	ms_Version;			/* Module format version.						*/
	UBYTE	ms_Revision;		/* Module format revision.						*/
	UBYTE	ms_Reserved;		/* Always 0.									*/
	APTR	ms_Instruments;		/* Ptr to 1st INSTSTATUS struct in linked list.	*/
	UWORD	ms_NumPatterns;		/* Number of unique patterns defined in module.	*/
	UWORD	ms_NumPositions;	/* Numberof 4-pattern groups to play in module.	*/
	APTR	ms_Pathname;		/* Ptr to pathspec from which module was loaded.*/
	APTR	ms_Filename;		/* Ptr to current filename of this module.		*/
	};


/* Values for ms_Format... */

enum MODFORMATS
	{
	MF_SOUNDTRACKER = 1,
	MF_NOISETRACKER,
	MF_DESTRACKER
	};


/*----------------------------------------------------------------------------------*/
/* Instrument Status structure follows.  The ms_Instruments field points to the		*/
/* first of a linked list of these structures.  There is one instrument structure	*/
/* in the linked list for each instrument used by the module...						*/

struct INSTSTATUS
	{
	APTR	is_Next;		/* Pointer to next instrument in linked list.	*/
	APTR	is_Name;		/* Pointer to name of this instrument.			*/
	APTR	is_Address;		/* Pointer to instrument in memory.				*/
	ULONG	is_Length;		/* Length of this instrument in bytes.			*/
	ULONG	is_Repeat;		/* Repeat loop offset in bytes.					*/
	ULONG	is_RepeatLen;	/* Length of repeat loop in bytes.				*/
	UBYTE	is_Number;		/* The number of this instrument within module.	*/
	BYTE	is_FineTune;	/* Fine tune amount for instrument (-8..7).		*/
	UBYTE	is_Volume;		/* The default volume of this instrument.		*/
	UBYTE	is_Reserved;	/* Reserved field. Should always be 0.			*/
	};

/*------------------------------------------------------------------------------*/
/* PATHENTRY structure, used for storing search paths for LoadModule.			*/
/* The path string itself appears immediately after the pe_Length field, and	*/
/* is always NULL-terminated.  Also, the value in pe_Length includes the		*/
/* terminating NULL.																				*/

struct PATHENTRY
	{
	APTR	pe_Next;		/* Ptr to next PATHENTRY struct in linked list.	*/
	UWORD	pe_Length;		/* Length of the NULL-terminated path string.	*/
	};

/*----------------------------------------------------------------------------*/
/* timescaninfo structure.  This structure is passed to the CalcElapsedTime() */
/* function.  See description of this function in the DTLibFuncs.DOC file     */
/* for more details.														  */

struct timescaninfo
	{
	UBYTE	tsi_StartPos;		/* Start position for the time scan.		*/
	UBYTE	tsi_StartNote;		/* Start note offset for the time scan.		*/
	UBYTE	tsi_EndPos;			/* End position for the time scan.			*/
	UBYTE	tsi_EndNote;		/* End note offset for the time scan.		*/
	UBYTE	tsi_Flags;			/* Flags used for the time scan (see below).*/
	UBYTE	tsi_DefaultTempo;	/* Initial default tempo for scan.			*/
	UBYTE	tsi_FineTempo;		/* Fine Tempo used for the time scan.		*/
	UBYTE	tsi_Pad;			/* Pad byte. Must always be 0.				*/

/*  The CalcElapsedTime() function fills these next fields out for you...	*/

	ULONG	tsi_FirstIterTime;	/* Time for first iteration of play seq.	*/
	ULONG	tsi_ConsecIterTime;	/* Time for consecutive iterations.			*/
	};

/*	Bit definitions for the tsi_Flags field...	*/

enum TSI_FLAGS
	{
	TSIB_MULTIPLEITERS,	/* Set to calc tsi_ConsecIterTime.				*/
	TSIB_TEMPOINTERP,	/* Tempo interpretation for scan (0=BPM enabled)*/
	TSIB_TIMINGMODE		/* Timing mode (0=60Hz, 1=50Hz).				*/
	};

/* Bit mask definitions for the tsi_Flags field...	*/

#define	TSIF_MULTIPLEITERS	(1 << TSIB_MULTIPLEITERS)
#define	TSIF_TEMPOINTERP	(1 << TSIB_TEMPOINTERP)
#define	TSIF_TIMINGMODE		(1 << TSIB_TIMINGMODE)


/*------------------------------------------------------------------------------*/
/* DTL_Base structure, returned by OpenLibrary.  This is the structure that		*/
/* you get a pointer to when you open this library. It contains lots of			*/
/* information about each sound channel, the currenlty loaded module (if one	*/
/* is loaded), and the play sequence parameters.								*/
/*																				*/
/* NOTE: EVERY ONE of these variables are READ ONLY!  NEVER attempt to write	*/
/*       to ANY of them. (Besides, you cannot affect the library by changing	*/
/*       the parameters in this structure, you will only be destroying valuable	*/
/*       status information for both you and anyone else using the library).	*/

struct	DTL_Base
	{
	struct	Library	dtl_lib;	/* DTBase (public data).					*/
	APTR	dtl_SysLib;			/* A pointer to ExecBase for you to use.	*/
	APTR	dtl_DosLib;			/* A pointer to DosBase for you to use.		*/
	APTR	dtl_RexxSysLib;		/* A pointer to RexxSysBase for me to use.	*/
	APTR	dtl_SegList;		/* Off limits. Ne touche pas!				*/
	APTR	dtl_System;			/* Off limits as well.						*/
	UBYTE	dtl_SysFlags;		/* Once again... off limits.				*/
	UBYTE	dtl_WhichTimer;		/* Holds CIA timer that DES-Tracker is using.*/

/* The following variables are always valid, whether a module is loaded or not... */

	APTR	dtl_ModPathList;	/* Pointer to 1st PATHENTRY in linked list.		*/
	UWORD	dtl_Flags;			/* DTL_Base flags (see below).					*/
	UWORD   dtl_Flags2;			/* Extra Flags for DES-Tracker r1.2.			*/ 
	UBYTE	dtl_GlobalVolume;	/* Global volume for all channels (0..63).		*/
	BYTE	dtl_GlobalTempo;	/* Global tempo for all modules (-16..15).		*/
	BYTE	dtl_FineTempo;		/* Fine tempo adjustment (-128..127).			*/
	UBYTE	dtl_DefaultTempo;	/* Default tempo (0..255) for start of play seq.*/
	UBYTE	dtl_VolFadeRate;	/* Speed at which volume fades transpire.		*/
	UBYTE	dtl_Iterations;		/* Number of iterations for play sequence.		*/
	UBYTE	dtl_TimingMode;		/* Holds 0 if NTSC timing is used, 1 if PAL.	*/
	UBYTE	dtl_SaveFormat;		/* Holds save file format for SaveModule func.	*/
	UBYTE	dtl_PlayStatus;		/* Holds play-status value. See below.			*/

/* The following variables are valid only when a module is loaded... */

	UBYTE	dtl_DefIterations;	/* Default # of iterations for play sequence.	*/
	UBYTE	dtl_IterationsToGo;	/* Number of iterations left including current.	*/
	UBYTE	dtl_CurPosition;	/* Current position in play sequence.			*/
	UBYTE	dtl_CurNoteIndex;	/* Current note index into current position.	*/
	UBYTE	dtl_StartPosition;	/* Play sequence start position.				*/
	UBYTE	dtl_StartNoteIndex;	/* Play sequence start note index into pos.		*/
	UBYTE	dtl_EndPosition;	/* Play sequence end position.					*/
	UBYTE	dtl_EndNoteIndex;	/* Play sequence end note index into pos.		*/
	UBYTE	dtl_CurrentTempo;	/* Current tempo (including global tempo).		*/

	struct	CHANNELSTATUS dtl_Channel0;	/* Status info for channel 0. */
	struct	CHANNELSTATUS dtl_Channel1;	/* Status info for channel 1. */
	struct	CHANNELSTATUS dtl_Channel2;	/* Status info for channel 2. */
	struct	CHANNELSTATUS dtl_Channel3;	/* Status info for channel 3. */

	struct	MODULESTATUS dtl_ModuleStatus;	/* Status info for currently loaded module. */

/* The following variables are valid only when a song is playing... */

	UBYTE	dtl_ElapsedHours;	/* # of hours that song has been playing for.	*/
	UBYTE	dtl_ElapsedMinutes;	/* # of minutes elapsed while playing.			*/
	UBYTE	dtl_ElapsedSeconds;	/* # of seconds elapsed.						*/
	UBYTE	dtl_ElapsedJiffies;	/* # of 60ths of a second elapsed.				*/

	ULONG	dtl_ElapsedTime;	/* Total current elapsed time in 60ths of a sec. */

	ULONG	dtl_CurrentHertz;	/* Current interrupt frequency (Fixed-point Int).*/
	};


/* Bit definitions for dtl_Flags field... */

enum DTLFLAGS
	{
	DF_CH0STATUS,		/* Set to 1 if channel 0 is enabled, 0 if disabled.	*/
	DF_CH1STATUS,		/* Same as above, but for channel 1.				*/
	DF_CH2STATUS,		/* Same for channel 2.								*/
	DF_CH3STATUS,		/* Same for channel 3.								*/
	DF_FILTERSTATUS,	/* Set to 1 when filter is on, 0 when off.			*/
	DF_MODULELOADED,	/* Set to 1 when a module is currently loaded.		*/
	DF_PLAYSEQDONE,		/* Set to 1 when a play sequence ends.				*/
	DF_PLAYING,			/* Set to 1 whenever a play sequence is active.		*/
	DF_CH0NOTEHIT,		/* Toggles between 1 and 0 when a note is played.	*/
	DF_CH1NOTEHIT,		/* Same as above,  but for channel 1.				*/
	DF_CH2NOTEHIT,		/* Same for channel 2.								*/
	DF_CH3NOTEHIT,		/* Same for channel 3.								*/
	DF_AUTOTIMING,		/* Set to 1 if auto timing detection is enabled.	*/
	DF_LOOPINTENT,		/* Set to 1 if this song is meant to loop.			*/
	DF_MODULECHANGED,	/* Toggles each time a change is made to a module.	*/
	DF_LIBLOCKED		/* Set when someone has OWNed the library.			*/
	};

/* Bit definitions for dtl_Flags2 field... */

#define DF_HASPSC0			14	/* 0=File has no PSC0 construct, 1=Has PSC0	const.	*/
#define	DF_TEMPOINT			15	/* 0=Normal/BPM tempos, 1=Normal tempos only.		*/

/* Bit Mask values for dtl_Flags... */

#define	DFM_CH0STATUS		(1 << DF_CH0STATUS)
#define	DFM_CH1STATUS		(1 << DF_CH1STATUS)
#define	DFM_CH2STATUS		(1 << DF_CH2STATUS)
#define	DFM_CH3STATUS		(1 << DF_CH3STATUS)
#define	DFM_FILTERSTATUS	(1 << DF_FILTERSTATUS)
#define	DFM_MODULELOADED	(1 << DF_MODULELOADED)
#define	DFM_PLAYSEQDONE		(1 << DF_PLAYSEQDONE)
#define	DFM_PLAYING			(1 << DF_PLAYING)
#define	DFM_CH0NOTEHIT		(1 << DF_CH0NOTEHIT)
#define	DFM_CH1NOTEHIT		(1 << DF_CH1NOTEHIT)
#define	DFM_CH2NOTEHIT		(1 << DF_CH2NOTEHIT)
#define	DFM_CH3NOTEHIT		(1 << DF_CH3NOTEHIT)
#define	DFM_AUTOTIMING		(1 << DF_AUTOTIMING)
#define	DFM_LOOPINTENT		(1 << DF_LOOPINTENT)
#define	DFM_MODULECHANGED	(1 << DF_MODULECHANGE)
#define	DFM_LIBLOCKED		(1 << DF_LIBLOCKED)

/* Bit Mask values for dtl_Flags2... */

#define DFM_HASPSC0			(1 << DFM_HASPSC0)
#define	DFM_TEMPOINT		(1 << DFM_TEMPOINT)

/* Values for dtl_PlayStatus... */

enum STATUSVALUES
	{
	PS_NOMODULE,	/* No module is loaded.							*/
	PS_IDLE,		/* Module loaded but hasn't started playing yet.*/
	PS_PLAYING,		/* Module is currently playing.					*/
	PS_PAUSED,		/* Module is paused.							*/
	PS_STOPPED,		/* Module was playing but has been stopped.		*/
	PS_PLAYSEQDONE	/* Module has finished playing.					*/
	};

/*------------------------------------------------------------------*/
/* Signal event bit definitions for AddStatusSignal() function...	*/

enum SIGEVENTBITS
	{
	SEB_START,			/* Signal when play sequence starts.			*/
	SEB_FINISH,			/* Signal when play sequence is completed.		*/
	SEB_PAUSE,			/* Signal whenever play is paused.				*/
	SEB_CONTINE,		/* Signal whenever play is un-paused.			*/
	SEB_STOP,			/* Signal when play sequence is stopped.		*/
	SEB_ITERATION,		/* Signal upon completion of an iteration.		*/
	SEB_PATTERN,		/* Signal when a pattern completes.				*/
	SEB_VOLFADE,		/* Signal upon completion of a volume fade.		*/
	SEB_MODLOADED,		/* Signal when a new module is loaded.			*/
	SEB_MODUNLOADED,	/* Signal when current module is unloaded.		*/
	SEB_LIBOWNED,		/* Signal when library becomes locked.			*/
	SEB_LIBDISOWNED,	/* Signal when library no longer locked.		*/
	SEB_MINUTE,			/* Signal on each minute of elapsed play-time.	*/
	SEB_SECOND			/* Signal on each second of elapsed play-time.	*/
	};

/* Signal event mask definitions...	*/

#define	SEF_START			(1 << SEB_START)
#define	SEF_FINISH			(1 << SEB_FINISH)
#define	SEF_PAUSE			(1 << SEB_PAUSE)
#define	SEF_CONTINUE		(1 << SEB_CONTINE)
#define	SEF_STOP			(1 << SEB_STOP)
#define	SEF_ITERATION		(1 << SEB_ITERATION)
#define	SEF_PATTERN			(1 << SEB_PATTERN)
#define	SEF_VOLFADE			(1 << SEB_VOLFADE)
#define	SEF_MODLOADED		(1 << SEB_MODLOADED)
#define	SEF_MODUNLOADED		(1 << SEB_MODUNLOADED)
#define	SEF_LIBOWNED		(1 << SEB_LIBOWNED)
#define	SEF_LIBDISOWNED		(1 << SEB_LIBDISOWNED)
#define SEF_MINUTE			(1 << SEB_MINUTE)
#define SEF_SECOND			(1 << SEB_SECOND)

/*--------------------------------------------------------------*/
/* Error codes returned by the library routines...				*/

#define	DE_SUCCESS		0

#define	DE_LIB_ALREADY_OWNED	-1	/* Someone else has owned the library.				*/
#define	DE_NO_MODULE_LOADED		-2	/* Command required a module to be in memory.		*/
#define	DE_NO_PLAY_SEQ_ACTIVE	-3	/* Command required a play sequence to be active.	*/
#define	DE_IVALUE_OUT_OF_RANGE	-4	/* Input value was out of range.					*/
#define	DE_VALUE_WAS_TRIMMED	-5	/* Input value was trimmed to legal range.			*/
#define	DE_FILE_LOAD_ERROR		-6	/* An error occurred while attempting to read file.	*/
#define	DE_INSUFFICIENT_MEMORY	-7	/* Insufficient free memory for operation.			*/
#define	DE_ILLEGAL_PATHNAME		-8	/* Pathname is not in proper pathname format.		*/
#define	DE_UNKNOWN_FORMAT		-9	/* Specified song file is not a recognized format.	*/
#define	DE_FILE_SAVE_ERROR		-10	/* An error occurred while attempting to write file.*/
#define	DE_NO_INSTRUMENT		-11	/* An instrument name was required (ARexx only).	*/
#define	DE_FILE_NOT_FOUND		-12	/* Specified file couldn't be found.				*/
#define	DE_MISSING_INST			-13	/* Instrument(s) missing from song being loaded.	*/
#define	DE_PLAY_SEQ_ACTIVE		-14	/* Returned by ContinuePlaySeq() if already playing.*/
#define DE_TOO_MANY_PATTERNS	-15 /* Can only happen when saving in ST2.6 format.		*/
#define DE_CH_ENABLE_FAILED		-16	/* User-specified audio channel could not be enabled.*/

/*--------------------------------------------------------------*/
/* A define for you to use (that you should use)...				*/

#define	DTLIBNAME	"destracker.library"

#endif
