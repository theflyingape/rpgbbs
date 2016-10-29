;;==========================================================;;
;; DTLib.I -- External declarations for DES-Tracker Library ;;
;;                                                          ;;
;;            Release: 2.00                                 ;;
;;            Version: 3.0                                  ;;
;;                                                          ;;
;; Designed and written by Darren Schebek                   ;;
;; Copyright ©1992,1993 Darren Schebek                      ;;
;; All rights reserved                                      ;;
;;                                                          ;;
;; rel 1.1         Begun: Sun, Aug 31, 1991                 ;;
;; rel 1.2  Last updated: Fri, Sep  4, 1992                 ;;
;; rel 1.4  Last updated: Fri, Oct  2, 1992                 ;;
;; rel 1.5  Last updated: Fri, Jan  1, 1993                 ;;
;; rel 1.6  Last updated: Thu, Jan 14, 1993                 ;;
;; rel 1.61 Last updated: Wed, Jan 27, 1993                 ;;
;; rel 1.62 Last updated: Sat, Feb  6, 1993                 ;;
;; rel 1.63 Last updated: Wed, Feb 10, 1993                 ;;
;; rel 1.64 Last updated: Thu, May 22, 1993                 ;;
;; rel 2.00 Last updated: Sat, Dec 18, 1993                 ;;
;;==========================================================;;


;---------------------------------------------------------------
; Library Function Definitions...

	LIBINIT
	LIBDEF	_LVODispatch		;ARexx dispatch entry. For ARexx only.
	LIBDEF	_LVOOwnPlayer		;Lock out others from using library.
	LIBDEF	_LVODisownPlayer	;Allow others to use library again.
	LIBDEF	_LVOSetModulePath	;Set default path for disk operations.
	LIBDEF	_LVOSetFilter		;Turn low-pass filter on/off (power light).
	LIBDEF	_LVOSetChannelEnable	;Enable/disable specific channels.
	LIBDEF	_LVOSetStartPosition	;Set play sequence start position.
	LIBDEF	_LVOSetEndPosition	;Set play sequence end position.
	LIBDEF	_LVOSetIterations	;Set # of iterations for play sequence.
	LIBDEF	_LVOPausePlaySeq	;Pause a currently active play sequence.
	LIBDEF	_LVOContinuePlaySeq	;Resume a play seq from where it was paused.
	LIBDEF	_LVOSetGlobalVolume	;Set the global volume for all channels.
	LIBDEF	_LVOSetInstVolume	;Set default volume of a specific instrument.
	LIBDEF	_LVOSetDefaultTempo	;Set default tempo for start of play sequence.
	LIBDEF	_LVOSetGlobalTempo	;Set global tempo for entire play sequence.
	LIBDEF	_LVOSetFineTempo	;Set fine tempo control for playback.
	LIBDEF	_LVOFadeGlobalVolume	:Fade global volume to specific volume.
	LIBDEF	_LVOSetVolumeFadeRate	;Set rate (speed) at which volume fades.
	LIBDEF	_LVOLoadModule		;Load a specified module into memory.
	LIBDEF	_LVOUnloadModule	;Release currently loaded module from memory.
	LIBDEF	_LVOBeginPlaySeq	;Begin a play sequence.
	LIBDEF	_LVOApplyGlobalTempo	;Adds global tempo to all SETTEMPO commands.
	LIBDEF	_LVORetitleModule	;Change the name of a module (NOT filename).
	LIBDEF	_LVOSetCurPosition	;Set current position within a module.
	LIBDEF	_LVOSaveModule		;Save current module to disk.
	LIBDEF	_LVOSetSaveFormat	;Set save format used by SaveModule.
	LIBDEF	_LVOVerifyModule	;Verify disk file as being a module or not.
	LIBDEF	_LVOSetTimingMode	;Set song timing to NTSC or PAL timing.
	LIBDEF	_LVOSetAutoTiming	;Turn PAL/NTSC auto-detect on/off (Not in use yet.)
	LIBDEF	_LVOSetLoopIntent	;Set whether song is intended to loop.
	LIBDEF	_LVOResetDefaults	;Reset library parms to default values.
	LIBDEF	_LVOSetTempoInt		;Set tempo interpretation (BPM/Non-BPM).
	LIBDEF	_LVOAddStatusSignal	;Add user signal bit for play status changes. 
	LIBDEF	_LVORemStatusSignal	;Remove a signal bit from signal queue.
	LIBDEF	_LVOStopPlaySeq		;Stop playing song, reset position.
	LIBDEF	_LVOInstallModule	;"Load" a module that resides in memory.
	LIBDEF	_LVOCalcElapsedTime	;Calculate time to complete a play seq.
	LIBDEF	_LVOSetTemporalStart	;Set start position based on time.
	LIBDEF	_LVOSetTemporalEnd	;Set end position based on time.
	LIBDEF	_LVOSetTemporalPos	;Set current play position based on time.
	LIBDEF	_LVOCalcVolFadeTime	;Calculate time to complete a volume fade.

;;===================================;;
;; Library Data Structures follow... ;;
;;===================================;;


;---------------------------------------------------------------
; CHANNELSTATUS structure follows. DTL_Base contains four instances of this
; structure, called dtl_Channel0, dtl_Channel1, etc.  This structure contains
; information concerning an individual sound channel (0 to 3).  The info
; contained here is only valid when a module is loaded and a play sequence
; is active.  This information will be updated regardless of whether or not
; the channel in question is enabled...

	STRUCTURE CHANNELSTATUS,0
	UBYTE	cs_Note		;The # of last note played (0..11).
	UBYTE	cs_Octave	;The octave # of the last note played (0..2).
	UBYTE	cs_NoteNum	;cs_Note & cs_Octave combined (Note+(12*Octave)).
	UBYTE	cs_Volume	;Current volume of this channel (0..64).
	UBYTE	cs_RealVolume	;Cur. channel volume with Global Volume applied.
	UBYTE	cs_Pad		;Reserved.  Always zero.
	UWORD	cs_Period	;Current period of this channel.
	APTR	cs_Instrument	;Pointer to INSTSTATUS struct used by this channel.
	ULONG	cs_SampleOffset	;Offset from inst. start if SampleOffset was used.
	LABEL	cs_SIZE		;Size of the CHANNELSTATUS structure.


;---------------------------------------------------------------
; MODULESTATUS structure follows.  DTL_Base contains an instance of this
; structure.  It contains information specific to the currently loaded module.
; This information is valid only when a module is loaded...

	STRUCTURE MODULESTATUS,0
	APTR	ms_Name		;Pointer to title of loaded song module.
	ULONG	ms_Length	;Length of module in memory (in "DTL0" format).
	APTR	ms_PlaySeqArray	;Ptr to play sequence array (4-word elements).
	APTR	ms_PatternData	;Ptr to the module's pattern data.
	APTR	ms_InstInfoBlocks ;Ptr to 31 internal instrument blocks (30 bytes each).
	UBYTE	ms_Format	;MF_SOUNDTRACKER, MF_NOISETRACKER, or MF_DESTRACKER.
	UBYTE	ms_Version	;Module format version.
	UBYTE	ms_Revision	;Module format revision.
	UBYTE	ms_Reserved	;Always 0.
	APTR	ms_Instruments	;Ptr to 1st INSTSTATUS struct in linked list.
	UWORD	ms_NumPatterns	;Number of unique patterns defined in module.
	UWORD	ms_NumPositions	;Number of 4-pattern groups to play in module.
	APTR	ms_Pathname	;Ptr to pathspec from which module was loaded.
	APTR	ms_Filename	;Ptr to current filename of this module.
	LABEL	ms_SIZE		;Size of the MODULESTATUS structure

; Values for ms_Format...

MF_SOUNDTRACKER	= 1
MF_NOISETRACKER	= 2
MF_DESTRACKER	= 3

;---------------------------------------------------------------
; Instrument Status structure follows.  The ms_Instruments field points to the
; first of a linked list of these structures.  There is one instrument structure
; in the linked list for each instrument used by the module...

	STRUCTURE INSTSTATUS,0
	APTR	is_Next		;Pointer to next instrument in linked list.
	APTR	is_Name		;Pointer to name of this instrument.
	APTR	is_Address	;Pointer to instrument in memory.
	ULONG	is_Length	;Length of this instrument in bytes.
	ULONG	is_Repeat	;Repeat loop offset in bytes.
	ULONG	is_RepeatLen	;Length of repeat loop in bytes.
	UBYTE	is_Number	;The number of this instrument within module.
	BYTE	is_FineTune	;Fine tune amount for instrument (-8..7).
	UBYTE	is_Volume	;The default volume of this instrument.
	UBYTE	is_Reserved	;Reserved field. Should always be 0.
	LABEL	is_SIZE		;Size of the INSTSTATUS structure.


;---------------------------------------------------------------
; PATHENTRY structure, used for storing search paths for LoadModule.
; The path string itself appears immediately after the pe_Length field, and
; is always NULL-terminated.  Also, the value in pe_Length includes the
; terminating NULL.

	STRUCTURE PATHENTRY,0
	APTR	pe_Next		;Pointer to next PATHENTRY structure in linked list.
	UWORD	pe_Length	;Length of the NULL-terminated path string.
	LABEL	pe_SIZE		;Size of the PATHENTRY structure.


;---------------------------------------------------------------
; TIMESCANINFO structure.  This structure is passed to the CalcElapsedTime()
; function.  See description of this function in the DTLibFuncs.DOC file
; for more details.

	STRUCTURE TIMESCANINFO,0
	UBYTE	tsi_StartPos		;Start position for the time scan.
	UBYTE	tsi_StartNote		;Start note offset for the time scan.
	UBYTE	tsi_EndPos		;End position for the time scan.
	UBYTE	tsi_EndNote		;End note offset for the time scan.
	UBYTE	tsi_Flags		;Flags used for the time scan (see below).
	UBYTE	tsi_DefaultTempo	;Initial default tempo for scan.
	UBYTE	tsi_FineTempo		;Fine Tempo used for the time scan.
	UBYTE	tsi_Pad			;Pad byte. Must always be 0.

;       The CalcElapsedTime() function fills these next fields out for you...

	ULONG	tsi_FirstIterTime	;Time for first iteration of play seq.
	ULONG	tsi_ConsecIterTime	;Time for consecutive iterations.

	LABEL	tsi_SIZE		;Size of the TIMESCANINFO structure.

; Bit definitions for the tsi_Flags field...

TSIB_MULTIPLEITERS	= 0	;Set to calc tsi_ConsecIterTime.
TSIB_TEMPOINTERP	= 1	;Tempo interpretation for scan (0=BPM enabled)
TSIB_TIMINGMODE		= 2	;Timing mode (0=60Hz, 1=50Hz).

; Bit mask definitions for the tsi_Flags field...

TSIF_MULTIPLEITERS	= (1<<TSIB_MULTIPLEITERS)
TSIF_TEMPOINTERP	= (1<<TSIB_TEMPOINTERP)
TSIF_TIMINGMODE		= (1<<TSIB_TIMINGMODE)

;---------------------------------------------------------------
; DTL_Base structure, returned by OpenLibrary.  This is the structure that
; you get a pointer to when you open this library. It contains lots of
; information about each sound channel, the currently loaded module (if one
; is loaded), and the play sequence parameters.
;
; NOTE: EVERY ONE of these variables are READ ONLY!  NEVER attempt to write
;       to ANY of them. (Besides, you cannot affect the library by changing
;       the parameters in this structure, you will only be destroying valuable
;       status information for both you and anyone else using the library).

	STRUCTURE DTL_Base,LIB_SIZE	;DTBase (public data).
	APTR	dtl_SysLib		;A pointer to ExecBase for you to use.
	APTR	dtl_DosLib		;A pointer to DosBase for you to use.
	APTR	dtl_RexxSysLib		;A pointer to RexxSysBase for me to use.
	APTR	dtl_SegList		;Off limits. Ne touche pas!
	APTR	dtl_System		;Off limits as well.
	UBYTE	dtl_SysFlags		;Once again... off limits.
	UBYTE	dtl_WhichTimer		;Holds CIA timer that DES-Tracker is using.

; The following variables are always valid, whether a module is loaded or not...

	APTR	dtl_ModPathList		;Ptr to 1st PATHENTRY in linked list.
	UWORD	dtl_Flags		;DTL_Base flags (see below).
	UWORD	dtl_Flags2		;Extra flags for DES-Tracker v1.2.
	UBYTE	dtl_GlobalVolume	;Global volume for all channels (0..63).
	BYTE	dtl_GlobalTempo		;Global tempo for all modules (-16..15).
	BYTE	dtl_FineTempo		;Fine tempo adjustment (-128..127).
	UBYTE	dtl_DefaultTempo	;Default tempo (0..255) for start of play seq.
	UBYTE	dtl_VolFadeRate		;Speed at which volume fades transpire.
	UBYTE	dtl_Iterations		;Number of iterations for play sequence.
	UBYTE	dtl_TimingMode		;Holds 0 if NTSC timing is used, 1 if PAL.
	UBYTE	dtl_SaveFormat		;Holds save file format for SaveModule func.
	UBYTE	dtl_PlayStatus		;Holds play-status value. See below.

; The following variables are valid only when a module is loaded...

	UBYTE	dtl_DefIterations	;Default # of iterations for loaded song.
	UBYTE	dtl_IterationsToGo	;Number of iterations left including current.
	UBYTE	dtl_CurPosition		;Current position in play sequence.
	UBYTE	dtl_CurNoteIndex	;Current note index into current position.
	UBYTE	dtl_StartPosition	;Play sequence start position.
	UBYTE	dtl_StartNoteIndex	;Play sequence start note index into pos.
	UBYTE	dtl_EndPosition		;Play sequence end position.
	UBYTE	dtl_EndNoteIndex	;Play sequence end note index into pos.
	UBYTE	dtl_CurrentTempo	;Current tempo (including global tempo).

	STRUCT	dtl_Channel0,cs_SIZE	;Status info for channel 0.
	STRUCT	dtl_Channel1,cs_SIZE	;Status info for channel 1.
	STRUCT	dtl_Channel2,cs_SIZE	;Status info for channel 2.
	STRUCT	dtl_Channel3,cs_SIZE	;Status info for channel 3.

	STRUCT	dtl_ModuleStatus,ms_SIZE ;Status info for currently loaded module.

; The following variables are valid only when a song is playing...

	UBYTE	dtl_ElapsedHours	;# of hours that song has been playing for.
	UBYTE	dtl_ElapsedMinutes	;# of minutes elapsed while playing.
	UBYTE	dtl_ElapsedSeconds	;# of seconds elapsed.
	UBYTE	dtl_ElapsedJiffies	;# of 60ths of a second elapsed.

	ULONG	dtl_ElapsedTime		;Total current elapsed time in 60ths of a sec.

	ULONG	dtl_CurrentHertz	;Current interrupt frequency (Fixed-point Int).

	LABEL	dtl_SIZE

;-----------------------------------
; Bit Field values for dtl_Flags...

DF_CH0STATUS		= 0	;Set to 1 if channel 0 is enabled, 0 if disabled.
DF_CH1STATUS		= 1	;Same as above, but for channel 1.
DF_CH2STATUS		= 2	;Same for channel 2.
DF_CH3STATUS		= 3	;Same for channel 3.
DF_FILTERSTATUS		= 4	;Set to 1 when filter is on, 0 when off.
DF_MODULELOADED		= 5	;Set to 1 when a module is currently loaded.
DF_PLAYSEQDONE		= 6	;Set to 1 when a play sequence ends.
DF_PLAYING		= 7	;Set to 1 whenever a play sequence is active.
DF_CH0NOTEHIT		= 8	;Toggles between 1 and 0 when a note is played.
DF_CH1NOTEHIT		= 9	;Same as above,  but for channel 1.
DF_CH2NOTEHIT		= 10	;Same for channel 2.
DF_CH3NOTEHIT		= 11	;Same for channel 3.
DF_AUTOTIMING		= 12	;Set to 1 if auto timing detection is enabled.
DF_LOOPINTENT		= 13	;Set to 1 if this song is meant to loop.
DF_MODULECHANGED	= 14	;Toggles each time a change is made to a module.
DF_LIBLOCKED		= 15	;Set when someone has OWNed the library.

; Bit Field values for dtl_Flags2...

DF_TEMPOINT		= 15	;0=Normal/BPM tempo interpretation, 1=Normal tempo
DF_HASPSC0		= 14	;0=File had no PSC0 construct, 1=File has PSC0 const.

;-----------------------------------
; Bit Mask values for dtl_Flags...

DFM_CH0STATUS		= (1<<DF_CH0STATUS)
DFM_CH1STATUS		= (1<<DF_CH1STATUS)
DFM_CH2STATUS		= (1<<DF_CH2STATUS)
DFM_CH3STATUS		= (1<<DF_CH3STATUS)
DFM_FILTERSTATUS	= (1<<DF_FILTERSTATUS)
DFM_MODULELOADED	= (1<<DF_MODULELOADED)
DFM_PLAYSEQDONE		= (1<<DF_PLAYSEQDONE)
DFM_PLAYING		= (1<<DF_PLAYING)
DFM_CH0NOTEHIT		= (1<<DF_CH0NOTEHIT)
DFM_CH1NOTEHIT		= (1<<DF_CH1NOTEHIT)
DFM_CH2NOTEHIT		= (1<<DF_CH2NOTEHIT)
DFM_CH3NOTEHIT		= (1<<DF_CH3NOTEHIT)
DFM_AUTOTIMING		= (1<<DF_AUTOTIMING)
DFM_LOOPINTENT		= (1<<DF_LOOPINTENT)
DFM_MODULECHANGED	= (1<<DF_MODULECHANGED)
DFM_LIBLOCKED		= (1<<DF_LIBLOCKED)

; Bit mask values for dtl_Flags2...

DFM_TEMPOINT		= (1<<DF_TEMPOINT)
DFM_HASPSC0		= (1<<DF_HASPSC0)

;-----------------------------------
; Values for dtl_PlayStatus

PS_NOMODULE	= 0	;No module is loaded.
PS_IDLE		= 1	;Module loaded but hasn't started playing yet.
PS_PLAYING	= 2	;Module is currently playing.
PS_PAUSED	= 3	;Module is paused.
PS_STOPPED	= 4	;Module was playing but has been stopped.
PS_PLAYSEQDONE	= 5	;Module has finished playing.

;-----------------------------------
; Signal event bit numbers for AddStatusSignal...

SEB_START	= 0	 ;Signal when play sequence starts.
SEB_FINISH	= 1	 ;Signal when play sequence is completed.
SEB_PAUSE	= 2	 ;Signal whenever play is paused.
SEB_CONTINUE	= 3	 ;Signal whenever play is un-paused.
SEB_STOP	= 4	 ;Signal when play sequence is stopped.
SEB_ITERATION	= 5	 ;Signal upon completion of an iteration.
SEB_PATTERN	= 6	 ;Signal when a pattern completes.
SEB_VOLFADE	= 7	 ;Signal upon completion of a volume fade.
SEB_MODLOADED	= 8	 ;Signal when a new module is loaded.
SEB_MODUNLOADED	= 9	 ;Signal when current module is unloaded.
SEB_LIBOWNED	= 10	 ;Signal when library becomes locked.
SEB_LIBDISOWNED	= 11	 ;Signal when library no longer locked.
SEB_MINUTE	= 12	 ;Signal on each minute of elapsed time.
SEB_SECOND	= 13	 ;Signal on each second of elapsed time.

;---------------------------------------------------------------
; Signal event bit masks for AddStatusSignal() function. Desired events are
; OR'ed together & passed in D1...

SEF_START	= (1<<SEB_START)
SEF_FINISH	= (1<<SEB_FINISH)
SEF_PAUSE	= (1<<SEB_PAUSE)
SEF_CONTINUE	= (1<<SEB_CONTINUE)
SEF_STOP	= (1<<SEB_STOP)
SEF_ITERATION	= (1<<SEB_ITERATION)
SEF_PATTERN	= (1<<SEB_PATTERN)
SEF_VOLFADE	= (1<<SEB_VOLFADE)
SEF_MODLOADED	= (1<<SEB_MODLOADED)
SEF_MODUNLOADED	= (1<<SEB_MODUNLOADED)
SEF_LIBOWNED	= (1<<SEB_LIBOWNED)
SEF_LIBDISOWNED	= (1<<SEB_LIBDISOWNED)
SEF_MINUTE	= (1<<SEB_MINUTE)
SEF_SECOND	= (1<<SEB_SECOND)

;---------------------------------------------------------------
; Error codes returned by the library routines...

DE_SUCCESS		=  0

DE_LIB_ALREADY_OWNED	= -1	;Someone else has owned the library.
DE_NO_MODULE_LOADED	= -2	;Command required a module to be in memory.
DE_NO_PLAY_SEQ_ACTIVE	= -3	;Command required a play sequence to be active.
DE_IVALUE_OUT_OF_RANGE	= -4	;Input value was out of range.
DE_VALUE_WAS_TRIMMED	= -5	;Input value was trimmed to legal range.
DE_FILE_LOAD_ERROR	= -6	;An error occurred while attempting to read file.
DE_INSUFFICIENT_MEMORY	= -7	;Insufficient free memory for operation.
DE_ILLEGAL_PATHNAME	= -8	;Pathname is not in proper pathname format.
DE_UNKNOWN_FORMAT	= -9	;Specified song file is not a recognized format.
DE_FILE_SAVE_ERROR	= -10	;An error occurred while attempting to write file.
DE_NO_INSTRUMENT	= -11	;An instrument name was required (ARexx only).
DE_FILE_NOT_FOUND	= -12	;Specified file couldn't be found.
DE_MISSING_INST		= -13	;Instrument(s) missing from song being loaded.
DE_PLAY_SEQ_ACTIVE	= -14	;Returned by ContinuePlaySeq() if already playing.
DE_TOO_MANY_PATTERNS	= -15	;Can only happen when saving in ST2.6 format.
DE_CH_ENABLE_FAILED	= -16	;User-specified audio channel could not be enabled.

;---------------------------------------------------------------
; A macro for you to use (that you should use)...

DTLIBNAME	MACRO
		Dc.B	"destracker.library",0
		ENDM	

