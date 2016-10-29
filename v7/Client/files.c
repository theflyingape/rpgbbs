#include "/include/all.h"
#include "global.h"

/********************************\
 *  External Protocol Handling  *
\********************************/
struct XPR_IO {
	STRPTR   xpr_filename;
	  LONG (* __asm xpr_fopen)(register __a0 STRPTR,register __a1 STRPTR);
	  LONG (* __asm xpr_fclose)(register __a0 FILE *);
	  LONG (* __asm xpr_fread)(register __a0 APTR,register __d0 LONG,register __d1 LONG,register __a1 FILE *);
	  LONG (* __asm xpr_fwrite)(register __a0 APTR,register __d0 LONG,register __d1 LONG,register __a1 FILE *);
	 ULONG (* __asm xpr_sread)(register __a0 APTR,register __d0 LONG,register __d1 ULONG);
	  LONG (* __asm xpr_swrite)(register __a0 APTR,register __d0 LONG);
	  LONG (* __asm xpr_sflush)(VOID);
	  LONG (* __asm xpr_update)(register __a0 struct XPR_UPDATE *);
	  LONG (* __asm xpr_chkabort)(VOID);
	  LONG (* __asm xpr_chkmisc)(VOID);
	  LONG (* __asm xpr_gets)(register __a0 STRPTR,register __a1 STRPTR);
	  LONG (* __asm xpr_setserial)(register __d0 LONG);
	  LONG (* __asm xpr_ffirst)(register __a0 STRPTR,register __a1 STRPTR);
	  LONG (* __asm xpr_fnext)(register __d0 LONG,register __a0 STRPTR,register __a1 STRPTR);
	  LONG (* __asm xpr_finfo)(register __a0 STRPTR,register __d0 LONG);
	  LONG (* __asm xpr_fseek)(register __a0 FILE *,register __d0 LONG,register __d1 LONG);
	  LONG   xpr_extension;
	  LONG  *xpr_data;
	 ULONG (* __asm xpr_options)(register __d0 LONG,register __a0 struct xpr_option **);
	  LONG (* __asm xpr_unlink)(register __a0 STRPTR);
	  LONG (* __asm xpr_squery)(VOID);
	  LONG (* __asm xpr_getptr)(register __d0 LONG);
};
#define XPR_EXTENSION 4L

LONG XProtocolCleanup(struct XPR_IO *);
LONG XProtocolSetup(struct XPR_IO *);
LONG XProtocolSend(struct XPR_IO *);
LONG XProtocolReceive(struct XPR_IO *);
LONG XProtocolHostMon(struct XPR_IO *,APTR,LONG,LONG);
LONG XProtocolUserMon(struct XPR_IO *,APTR,LONG,LONG);

#pragma libcall XProtocolBase XProtocolCleanup 1e 801
#pragma libcall XProtocolBase XProtocolSetup 24 801
#pragma libcall XProtocolBase XProtocolSend 2a 801
#pragma libcall XProtocolBase XProtocolReceive 30 801
#pragma libcall XProtocolBase XProtocolHostMon 36 109804
#pragma libcall XProtocolBase XProtocolUserMon 3c 109804

/*
*   Flags returned by XProtocolSetup()
*/
#define XPRS_FAILURE    0x00000000L
#define XPRS_SUCCESS    0x00000001L
#define XPRS_NORECREQ   0x00000002L
#define XPRS_NOSNDREQ   0x00000004L
#define XPRS_HOSTMON    0x00000008L
#define XPRS_USERMON    0x00000010L
#define XPRS_HOSTNOWAIT 0x00000020L
/*
*   The update structure
*/
struct XPR_UPDATE {     ULONG  xpru_updatemask;
                        STRPTR xpru_protocol;
                        STRPTR xpru_filename;
                        LONG   xpru_filesize;
                        STRPTR xpru_msg;
                        STRPTR xpru_errormsg;
                        LONG   xpru_blocks;
                        LONG   xpru_blocksize;
                        LONG   xpru_bytes;
                        LONG   xpru_errors;
                        LONG   xpru_timeouts;
                        LONG   xpru_packettype;
                        LONG   xpru_packetdelay;
                        LONG   xpru_chardelay;
                        STRPTR xpru_blockcheck;
                        STRPTR xpru_expecttime;
                        STRPTR xpru_elapsedtime;
                        LONG   xpru_datarate;
                        LONG   xpru_reserved1;
                        LONG   xpru_reserved2;
                        LONG   xpru_reserved3;
                        LONG   xpru_reserved4;
                        LONG   xpru_reserved5;
                   };
/*
*   The possible bit values for the xpru_updatemask are:
*/
#define XPRU_PROTOCOL           0x00000001L
#define XPRU_FILENAME           0x00000002L
#define XPRU_FILESIZE           0x00000004L
#define XPRU_MSG                0x00000008L
#define XPRU_ERRORMSG           0x00000010L
#define XPRU_BLOCKS             0x00000020L
#define XPRU_BLOCKSIZE          0x00000040L
#define XPRU_BYTES              0x00000080L
#define XPRU_ERRORS             0x00000100L
#define XPRU_TIMEOUTS           0x00000200L
#define XPRU_PACKETTYPE         0x00000400L
#define XPRU_PACKETDELAY        0x00000800L
#define XPRU_CHARDELAY          0x00001000L
#define XPRU_BLOCKCHECK         0x00002000L
#define XPRU_EXPECTTIME         0x00004000L
#define XPRU_ELAPSEDTIME        0x00008000L
#define XPRU_DATARATE           0x00010000L
/*
*   The xpro_option structure
*/
struct xpr_option {
   STRPTR xpro_description;      /* description of the option                  */
   LONG   xpro_type;             /* type of option                             */
   STRPTR xpro_value;            /* pointer to a buffer with the current value */
   LONG   xpro_length;           /* buffer size                                */
};
/*
*   Valid values for xpro_type are:
*/
#define XPRO_BOOLEAN 1L         /* xpro_value is "yes", "no", "on" or "off"   */
#define XPRO_LONG    2L         /* xpro_value is string representing a number */
#define XPRO_STRING  3L         /* xpro_value is a string                     */
#define XPRO_HEADER  4L         /* xpro_value is ignored                      */
#define XPRO_COMMAND 5L         /* xpro_value is ignored                      */
#define XPRO_COMMPAR 6L         /* xpro_value contains command parameters     */

#define ST_PARTYON	(1 << 0)
#define ST_PARTYODD	(1 << 1)
#define ST_7WIRE	(1 << 2)
#define ST_QBREAK	(1 << 3)
#define ST_RADBOOGIE	(1 << 4)
#define ST_SHARED	(1 << 5)
#define ST_EOFMODE	(1 << 6)
#define ST_XDISABLED	(1 << 7)
#define ST_PARTYMARKON	(1 << 8)
#define ST_PARTYMARK	(1 << 9)
#define ST_2BITS	(1 << 10)
#define ST_READ7	(1 << 11)
#define ST_WRITE7	(1 << 12)

struct	Library			*XProtocolBase;
struct	Window			*XPRwindow;
struct	XPR_IO			XPR_IO;

FILE *LOGfp;
UBYTE __far XPRlib[32];
UBYTE __far XPRopt[128];
UBYTE __far XPRcmd[3];
UBYTE __far BinaryTransfer;
LONG __far Bytes,Blocks,BlockSize,Errors,Timeouts;
UBYTE __far debug[61];
UBYTE __far TraceFlag=FALSE;

void trace(char *msg)
{
	if(TraceFlag) {
		sprintf(debug,"XPRtrace: %-50.50s",msg);
		Move(XPRwindow->RPort,4,8*(SERCON.TextFont->tf_YSize+2));
		Text(XPRwindow->RPort,debug,60);
	}
}

LONG display_value(long n)
{
	return((n>0) ? n : 0L);
}

LONG __saveds __asm
xpr_chkabort(void)
{
	struct IntuiMessage	*message;
	ULONG class;
	LONG result;

	trace("chkabort()");
	result=0L;
	while((message=(struct IntuiMessage *)GetMsg(XPRwindow->UserPort))) {
		class=message->Class;
		ReplyMsg((struct Message *)message);
		if(class & CLOSEWINDOW)
			result=-1;
	}
	return(result);
}

LONG __saveds __asm
xpr_fopen(register __a0 STRPTR FileName,register __a1 STRPTR AccessMode)
{
	FILE *File;

	strsfn(FileName,NULL,NULL,outbuf,extension);
	outbuf[12]=NULL;
	strupr(outbuf);
	extension[3]=NULL;
	strlwr(extension);
	strmfe(filename,outbuf,extension);
	sprintf(outbuf,"fopen(\"%s\",\"%s\")",FileName,AccessMode);
	trace(outbuf);
	Bytes=0L;
	Blocks=0L;
	BlockSize=0L;
	if((File=fopen(filename,AccessMode)))
		if(XPRcmd[0]=='R' && LOGfp!=NULL)
			fprintf(LOGfp,"%s\n",filename);
	return((LONG)File);
}

LONG __saveds __asm
xpr_fclose(register __a0 FILE *File)
{
	trace("fclose(fp)");
	fclose(File);
	return(1L);
}

LONG __saveds __asm
xpr_fread(register __a0 APTR Buffer,register __d0 LONG Size,register __d1 LONG Count,register __a1 FILE *File)
{
	sprintf(outbuf,"fread(buffer,%ld,fp)",Size);
	trace(outbuf);
	return((LONG)fread(Buffer,Size,Count,File));
}

LONG __saveds __asm
xpr_fwrite(register __a0 APTR Buffer,register __d0 LONG Size,register __d1 LONG Count,register __a1 FILE *File)
{
	sprintf(outbuf,"fwrite(buffer,%ld,fp)",Size);
	trace(outbuf);
	return((LONG)fwrite(Buffer,Size,Count,File));
}

LONG __saveds __asm
xpr_fseek(register __a0 FILE *File,register __d0 LONG Offset,register __d1 LONG Origin)
{
	trace("fseek()");
	return(fseek(File,Offset,Origin) ? 0L : -1L);
}

LONG __saveds __asm
xpr_sread(register __a0 APTR Buffer,register __d0 LONG Size,register __d1 ULONG Timeout)
{
	ULONG seconds,timer;
	LONG length;

	sprintf(outbuf,"sread(buffer,%ld,%ld)",Size,Timeout);
	trace(outbuf);
	if(Size>0) {
		if(Timeout<1000)
			seconds=Timeout*50;
		else
			seconds=Timeout/1000000L*50;
		for(timer=0; timer<=seconds; timer++) {
			if(xpr_chkabort())
				return(-1L);	/* user abort */
			SERCON.SerialIO->IOSer.io_Command=SDCMD_QUERY;
			DoIO((struct IORequest *)SERCON.SerialIO);
			if(SERCON.SerialIO->io_Status & 32)
				return(0L);		/* carrier lost */
			if(SERCON.SerialIO->IOSer.io_Actual>=Size) {
				SERCON.SerialIO->IOSer.io_Command=CMD_READ;
				SERCON.SerialIO->IOSer.io_Data=Buffer;
				SERCON.SerialIO->IOSer.io_Length=Size;
				DoIO((struct IORequest *)SERCON.SerialIO);
				return(Size);
			}
			Delay(1);
		}
		length=(SERCON.SerialIO->IOSer.io_Actual>Size) ? Size : SERCON.SerialIO->IOSer.io_Actual;
		if(length) {
			SERCON.SerialIO->IOSer.io_Command=CMD_READ;
			SERCON.SerialIO->IOSer.io_Data=Buffer;
			SERCON.SerialIO->IOSer.io_Length=length;
			DoIO((struct IORequest *)SERCON.SerialIO);
			return(length);
		}
	}
	return(0L);
}

LONG __saveds __asm
xpr_swrite(register __a0 APTR Buffer,register __d0 LONG Size)
{
	trace("swrite()");
	SERCON.SerialIO->IOSer.io_Command=SDCMD_QUERY;
	DoIO((struct IORequest *)SERCON.SerialIO);
	if(!(SERCON.SerialIO->io_Status & 32)) {
		SERCON.SerialIO->IOSer.io_Command=CMD_WRITE;
		SERCON.SerialIO->IOSer.io_Data=Buffer;
		SERCON.SerialIO->IOSer.io_Length=Size;
		if(DoIO((struct IORequest *)SERCON.SerialIO))
			return(-1L);
		return(0L);
	}
	return(-1L);
}

LONG __saveds __asm
xpr_sflush(void)
{
	trace("sflush()");
	SERCON.SerialIO->IOSer.io_Command=CMD_CLEAR;
	if(!DoIO((struct IORequest *)SERCON.SerialIO))
		return(0L);
	return(-1L);
}

LONG __saveds __asm
xpr_update(register __a0 struct XPR_UPDATE *UpdateInfo)
{
	trace("update()");
	if(UpdateInfo) {
		if((UpdateInfo->xpru_updatemask & (XPRU_PROTOCOL | XPRU_BLOCKCHECK))) {
			if((UpdateInfo->xpru_updatemask & XPRU_PROTOCOL) && UpdateInfo->xpru_protocol)
				strncpy(XPRlib,UpdateInfo->xpru_protocol,sizeof(XPRlib));
			sprintf(outbuf,"%.30s (%.16s)",XPRlib,(UpdateInfo->xpru_blockcheck) ? UpdateInfo->xpru_blockcheck : "n/a");
			sprintf(debug,"Protocol: %-50.50s",outbuf);
			Move(XPRwindow->RPort,4,1*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_FILENAME) && UpdateInfo->xpru_filename) {
			sprintf(debug,"Filename: %-50.50s",UpdateInfo->xpru_filename);
			Move(XPRwindow->RPort,4,2*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_FILESIZE)) {
			sprintf(outbuf,"%ld bytes",display_value(UpdateInfo->xpru_filesize));
			sprintf(debug,"Filesize: %-50.50s",outbuf);
			Move(XPRwindow->RPort,4,3*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
		if((UpdateInfo->xpru_updatemask & (XPRU_BYTES | XPRU_BLOCKS | XPRU_BLOCKSIZE))) {
			if((UpdateInfo->xpru_updatemask & XPRU_BYTES))
				Bytes=display_value(UpdateInfo->xpru_bytes);
			if((UpdateInfo->xpru_updatemask & XPRU_BLOCKS))
				Blocks=display_value(UpdateInfo->xpru_blocks);
			if((UpdateInfo->xpru_updatemask & XPRU_BLOCKSIZE))
				BlockSize=display_value(UpdateInfo->xpru_blocksize);
			sprintf(outbuf,"%ld bytes, %ld blocks, %ld blocksize",Bytes,Blocks,BlockSize);
			sprintf(debug,"Transfer: %-50.50s",outbuf);
			Move(XPRwindow->RPort,4,4*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
		if((UpdateInfo->xpru_updatemask & (XPRU_ERRORS | XPRU_TIMEOUTS))) {
			if((UpdateInfo->xpru_updatemask & XPRU_ERRORS))
				Errors=display_value(UpdateInfo->xpru_errors);
			if((UpdateInfo->xpru_updatemask & XPRU_TIMEOUTS))
				Timeouts=display_value(UpdateInfo->xpru_timeouts);
			sprintf(outbuf,"%ld     Timeouts: %ld",Errors,Timeouts);
			sprintf(debug,"# Errors: %-50.50s",outbuf);
			Move(XPRwindow->RPort,4,5*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_ERRORMSG) && UpdateInfo->xpru_errormsg) {
			sprintf(debug,"Last err: %-50.50s",UpdateInfo->xpru_errormsg);
			Move(XPRwindow->RPort,4,6*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_MSG) && UpdateInfo->xpru_msg) {
			sprintf(debug," Message: %-50.50s",UpdateInfo->xpru_msg);
			Move(XPRwindow->RPort,4,7*(SERCON.TextFont->tf_YSize+2));
			Text(XPRwindow->RPort,debug,60);
		}
	}
	return(1L);
}

LONG __saveds __asm
xpr_gets(register __a0 STRPTR Prompt,register __a1 STRPTR Buffer)
{
	if(TraceFlag) {
		sprintf(outbuf,"gets(\"%s\",buffer)",Prompt);
		trace(outbuf);
	}
	strcpy(Buffer,XPRopt);
	return(1L);
}

LONG __saveds __asm
xpr_setserial(register __d0 LONG Status)
{
	static LONG XprBauds[12]={
		   110,
		   300,
		  1200,
		  2400,
		  4800,
		  9600,
		 19200,
		 31250,
		 38400,
		 57600,
		 76800,
		115200
	};
	LONG i,result;

	if(TraceFlag) {
		sprintf(outbuf,"setserial(%lx)",Status);
		trace(outbuf);
	}
	SERCON.SerialIO->IOSer.io_Command=SDCMD_QUERY;
	DoIO((struct IORequest *)SERCON.SerialIO);
	result=SERCON.SerialIO->io_SerFlags & 0xFF;
	if(SERCON.SerialIO->io_ExtFlags & SEXTF_MSPON)
		result|=ST_PARTYMARKON;
	if(SERCON.SerialIO->io_ExtFlags & SEXTF_MARK)
		result|=ST_PARTYMARK;
	if(SERCON.SerialIO->io_StopBits==2)
		result|=ST_2BITS;
	if(SERCON.SerialIO->io_ReadLen==7)
		result|=ST_READ7;
	if(SERCON.SerialIO->io_WriteLen==7)
		result|=ST_WRITE7;
	for(i=0 ; i<12 ; i++)
		if(XprBauds[i]>=SERCON.SerialIO->io_Baud) {
			result|=(i << 16);
			break;
		}

	if(Status != -1) {
		SERCON.SerialIO->io_SerFlags=Status & 0xFF;
		SERCON.SerialIO->io_ExtFlags=0;
		if(Status & ST_PARTYMARKON)
			SERCON.SerialIO->io_ExtFlags|=SEXTF_MSPON;
		if(Status & ST_PARTYMARK)
			SERCON.SerialIO->io_ExtFlags|=SEXTF_MARK;
		if(Status & ST_2BITS)
			SERCON.SerialIO->io_StopBits=2;
		else
			SERCON.SerialIO->io_StopBits=1;
		if(Status & ST_READ7)
			SERCON.SerialIO->io_ReadLen=7;
		else
			SERCON.SerialIO->io_ReadLen=8;
		if(Status & ST_WRITE7)
			SERCON.SerialIO->io_WriteLen=7;
		else
			SERCON.SerialIO->io_WriteLen=8;

		SERCON.SerialIO->io_Baud		= SERCON.SerialIO->io_Baud;
		SERCON.SerialIO->io_SerFlags	= SERCON.SerialIO->io_SerFlags;
		SERCON.SerialIO->io_ExtFlags	= SERCON.SerialIO->io_ExtFlags;
		SERCON.SerialIO->io_StopBits	= SERCON.SerialIO->io_StopBits;
		SERCON.SerialIO->io_ReadLen		= SERCON.SerialIO->io_ReadLen;
		SERCON.SerialIO->io_WriteLen	= SERCON.SerialIO->io_WriteLen;
		SERCON.SerialIO->IOSer.io_Command = SDCMD_SETPARAMS;
		DoIO((struct IORequest *)SERCON.SerialIO);
	}
	return(result);
}

LONG __saveds __asm
xpr_ffirst(register __a0 STRPTR Buffer,register __a1 STRPTR Pattern)
{
	if(TraceFlag) {
		sprintf(outbuf,"ffirst(buffer,\"%s\")",Pattern);
		trace(outbuf);
	}
	if(strlen(filename)) {
		strcpy(Buffer,filename);
		return(1L);
	}
	return(0L);
}

LONG __saveds __asm
xpr_fnext(register __d0 LONG OldState,register __a0 STRPTR Buffer,register __a1 STRPTR Pattern)
{
	if(TraceFlag) {
		sprintf(outbuf,"fnext(state,buffer,\"%s\")",Pattern);
		trace(outbuf);
	}
	strcpy(Buffer,"");
	return(0L);
}

LONG __saveds __asm
xpr_finfo(register __a0 STRPTR FileName,register __d0 LONG InfoType)
{
	BPTR filelock;
	LONG size;

	if(TraceFlag) {
		sprintf(outbuf,"finfo(\"%s\",%ld)",FileName,InfoType);
		trace(outbuf);
	}
	switch(InfoType) {
		case 1:			/* Return the file size. */
			size=0;
			if((filelock=Lock(FileName,ACCESS_READ))) {
				if((Examine(filelock,&infoBlock)))
					size=infoBlock.fib_Size;
				UnLock(filelock);
			}
			return(size);
			break;
		case 2:			/* Return the file transfer mode. */
			return(BinaryTransfer ? 1L : 2L);
			break;
	}
	return(0L);
}

LONG __saveds __asm
xpr_squery(void)
{
	trace("squery()");
	SERCON.SerialIO->IOSer.io_Command=SDCMD_QUERY;
	DoIO((struct IORequest *)SERCON.SerialIO);
	if(!(SERCON.SerialIO->io_Status & 32))
		return((LONG)SERCON.SerialIO->IOSer.io_Actual);
	return(-1L);		/* Return error if carrier is lost. */
}

ULONG __saveds __asm
xpr_options(register __d0 LONG NumOpts,register __a0 struct xpr_option **Opts)
{
	trace("options()");
	return(0L);
}

LONG __saveds __asm
xpr_unlink(register __a0 STRPTR FileName)
{
	if(TraceFlag) {
		sprintf(outbuf,"unlink(\"%s\")",FileName);
		trace(outbuf);
	}
	return((LONG)remove(FileName));
}

LONG __saveds __asm
xpr_getptr(register __d0 LONG InfoType)
{
	if(TraceFlag) {
		sprintf(outbuf,"getptr(%ld)",InfoType);
		trace(outbuf);
	}
	if(InfoType == 1)
		return(0L);
	else
		return(-1L);
}

int XPRtalk(char *logfile)
{
	int i,result=-1;

	if(logfile)
		LOGfp=fopen(logfile,"w");
	BinaryTransfer=(XPRcmd[1]=='B');

	if((XPRwindow=(struct Window *)OpenWindowTags(NULL,
					WA_PubScreen, pubscreen,
					WA_Top, window->TopEdge+11, WA_Left, window->LeftEdge+4,
					WA_Width, 60*SERCON.TextFont->tf_XSize, WA_Height, 10*(SERCON.TextFont->tf_YSize+2),
					WA_Activate, FALSE,
					WA_Borderless, FALSE,
					WA_CloseGadget, TRUE,
					WA_DepthGadget, TRUE,
					WA_DragBar, TRUE,
					WA_SizeGadget, FALSE,
					WA_SizeBBottom, FALSE,
					WA_Title, "RPGBBS Deluxe interface to XPR",
					WA_GimmeZeroZero, TRUE,
					WA_IDCMP, IDCMP_CLOSEWINDOW,
					WA_NoCareRefresh, TRUE,
					WA_SmartRefresh, TRUE,
					TAG_END))) {
		SetFont(XPRwindow->RPort,SERCON.TextFont);
		SetRast(XPRwindow->RPort,SERCON.Window->WScreen->BitMap.Depth<4 ? BLK : WHITE);
		SetDrMd(XPRwindow->RPort,JAM2);
		SetAPen(XPRwindow->RPort,RED);
		SetBPen(XPRwindow->RPort,SERCON.Window->WScreen->BitMap.Depth<4 ? BLK : WHITE);
		sprintf(outbuf,"Protocol: %-50.50s",XPRlib);
		Move(XPRwindow->RPort,4,1*(SERCON.TextFont->tf_YSize+2));
		Text(XPRwindow->RPort,outbuf,strlen(outbuf));
		if((XProtocolBase=OpenLibrary(XPRlib,0))) {
			memset(&XPR_IO,0,sizeof(XPR_IO));
			XPR_IO.xpr_filename=XPRopt;
			XPR_IO.xpr_fopen=xpr_fopen;
			XPR_IO.xpr_fclose=xpr_fclose;
			XPR_IO.xpr_fread=xpr_fread;
			XPR_IO.xpr_fwrite=xpr_fwrite;
			XPR_IO.xpr_sread=xpr_sread;
			XPR_IO.xpr_swrite=xpr_swrite;
			XPR_IO.xpr_sflush=xpr_sflush;
			XPR_IO.xpr_update=xpr_update;
			XPR_IO.xpr_chkabort=xpr_chkabort;
			XPR_IO.xpr_gets=xpr_gets;
			XPR_IO.xpr_setserial=xpr_setserial;
			XPR_IO.xpr_ffirst=xpr_ffirst;
			XPR_IO.xpr_fnext=xpr_fnext;
			XPR_IO.xpr_finfo=xpr_finfo;
			XPR_IO.xpr_fseek=xpr_fseek;
			XPR_IO.xpr_extension=4;
			XPR_IO.xpr_options=xpr_options;
			XPR_IO.xpr_unlink=xpr_unlink;
			XPR_IO.xpr_squery=xpr_squery;
			XPR_IO.xpr_getptr=xpr_getptr;
			i=XProtocolSetup(&XPR_IO);
			if((i & XPRS_SUCCESS)) {
				switch(XPRcmd[0]) {
					case 'R':
						XPR_IO.xpr_filename=filename;
						sprintf(outbuf,"Filename: %-50.50s",XPR_IO.xpr_filename);
						Move(XPRwindow->RPort,4,2*(SERCON.TextFont->tf_YSize+2));
						Text(XPRwindow->RPort,outbuf,strlen(outbuf));
						result=XProtocolReceive(&XPR_IO);
						break;
					case 'S':
						XPR_IO.xpr_filename=filename;
						sprintf(outbuf,"Filename: %-50.50s",XPR_IO.xpr_filename);
						Move(XPRwindow->RPort,4,2*(SERCON.TextFont->tf_YSize+2));
						Text(XPRwindow->RPort,outbuf,strlen(outbuf));
						result=XProtocolSend(&XPR_IO);
						break;
				}
				XProtocolCleanup(&XPR_IO);
			}
			else {
				sprintf(outbuf,"XPRerror: %X",i);
				Move(XPRwindow->RPort,4,2*(SERCON.TextFont->tf_YSize+2));
				Text(XPRwindow->RPort,outbuf,strlen(outbuf));
			}
			CloseLibrary(XProtocolBase);
		}
		Delay(50);
		CloseWindow(XPRwindow);
	}
	if(LOGfp)
		fclose(LOGfp);
	return(result);
}

void GetFilename(void)
{
	OUT("Filename: ");
	if(ins(16))
		return;
	NL;NL;
	memset(filename,0,sizeof(filename));
	strsfn(inbuf,NULL,NULL,outbuf,extension);
	outbuf[12]=NULL;
	strupr(outbuf);
	extension[3]=NULL;
	strlwr(extension);
	if(strlen(outbuf))
		strmfe(filename,outbuf,extension);
}

void DownLoadFile(void)
{
	BPTR filelock;
	FILE *fp;
	double RPGratio,USERratio;
	long size;
	char timestamp[20];
	char c;

	sprintf(outbuf,"%s  Uploads:%s %u  (%lu bytes)",fore(CYN),fore(WHITE),PLAYER.Uploads,PLAYER.UpByte);
	OUT(outbuf);NL;
	sprintf(outbuf,"%sDownloads:%s %u  (%lu bytes)",fore(CYN),fore(WHITE),PLAYER.Downloads,PLAYER.DownByte);
	OUT(outbuf);NL;
	RPGratio=PLAYER.FileRatio ? 1./PLAYER.FileRatio : 0.;
	USERratio=PLAYER.Downloads ? (double)PLAYER.Uploads/(double)PLAYER.Downloads : PLAYER.Uploads;
	sprintf(outbuf,"%sU/D File Ratio:%s %.3f  ",fore(CYN),fore(WHITE),USERratio);
	OUT(outbuf);
	if(PLAYER.FileRatio) {
		if(USERratio<=RPGratio) {
			beep();
			sprintf(outbuf,"%s(%sUpload/Download ratio exceeded%s)",fore(BRN),fore(YELLOW),fore(BRN));
			OUT(outbuf);NL;
			return;
		}
	}
	else
		OUT("(disabled)");
	NL;
	RPGratio=PLAYER.ByteRatio ? 1./PLAYER.ByteRatio : 0.;
	USERratio=PLAYER.DownByte ? (double)PLAYER.UpByte/(double)PLAYER.DownByte : PLAYER.UpByte;
	sprintf(outbuf,"%sU/D Byte Ratio:%s %.3f  ",fore(CYN),fore(WHITE),USERratio);
	OUT(outbuf);
	if(PLAYER.ByteRatio) {
		if(USERratio<=RPGratio) {
			beep();
			sprintf(outbuf,"%s(%sUpload/Download ratio exceeded%s)",fore(BRN),fore(YELLOW),fore(BRN));
			OUT(outbuf);NL;
			return;
		}
	}
	else
		OUT("(disabled)");
	NL;NL;
	GetFilename();
	if(strlen(filename)) {
		if((filelock=Lock(filename,ACCESS_READ))) {
			size=0;
			if((Examine(filelock,&infoBlock)))
				size=infoBlock.fib_Size;
			UnLock(filelock);
			if((c=option(&XPRmenu))=='Q' || local || !size)
				return;
			switch(c) {
				case 'B':
					strcpy(XPRlib,"xprkermit.library");
					strcpy(XPRopt,"OCY,GN,KN,TN");
					strcpy(XPRcmd,"SB");
					break;
				case 'C':
					strcpy(XPRlib,"xprxmodem.library");
					strcpy(XPRopt,"T1,C0,K0");
					strcpy(XPRcmd,"S");
					break;
				case 'K':
					strcpy(XPRlib,"xprkermit.library");
					strcpy(XPRopt,"OCY,GN,KN,TY");
					strcpy(XPRcmd,"S");
					break;
				case 'X':
					strcpy(XPRlib,"xprxmodem.library");
					strcpy(XPRopt,"T0,C1,K1");
					strcpy(XPRcmd,"SB");
					break;
				case 'Z':
					strcpy(XPRlib,"xprzmodem.library");
					strcpy(XPRopt,"T?,B16,AN,DN,SN");
					strcpy(XPRcmd,"S");
					break;
			}
			sprintf(outbuf,"Ready to send you %s",filename);
			OUT(outbuf);NL;
			if(!XPRtalk(NULL)) {
				beep();
				NL;
				sprintf(outbuf,"%s*** %sDOWNLOAD ERROR%s ***",fore(BLU),fore(BRED),fore(BLU));
				OUT(outbuf);NL;
				NORMAL;
			}
			else {
				PLAYER.Downloads++;
				PLAYER.DownByte+=size;
				strmfp(outbuf,directory,"file.log");
				if((fp=fopen(outbuf,"a"))) {
					GetSystemDate(Today);
					GetSystemTime(Time);
					julian(&Julian,Today);
					fdate(timestamp,Julian);
					timestamp[10]=' ';
					ftime(&timestamp[11],Time);
					fprintf(fp,"%.18s  %s downloaded %s from %s-%s.\n",timestamp,PLAYER.Handle,filename,FILEname[base],(MaxDir) ? ta[section] : "");
					fclose(fp);
				}
			}
		}
	}
}

void UpLoadFile(void)
{
	struct InfoData info;
	BPTR uplock;
	FILE *fp;
	long t;
	char c,tempfile[40];
	char timestamp[20];

	CurrentDir(root);
	if(!(uplock=Lock(UPLOADpath,ACCESS_READ))) {
		sprintf(outbuf,"%sSorry, could not access Upload directory \"%s\"",fore(CYAN),UPLOADpath);
		OUT(outbuf);NL;
		pause();
		return;
	}
	CurrentDir(uplock);
	strsfn(FILEpath[base],filename,NULL,NULL,NULL);
	getdfs(filename,&info);
	sprintf(outbuf,"There are %ld disk blocks (%ld bytes) available.",info.id_NumBlocks-info.id_NumBlocksUsed,(info.id_NumBlocks-info.id_NumBlocksUsed)*info.id_BytesPerBlock);
	OUT(outbuf);NL;
	if((c=option(&XPRmenu))=='Q' || local)
		return;
	memset(filename,0,sizeof(filename));
	switch(c) {
		case 'B':
			strcpy(XPRlib,"xprkermit.library");
			strcpy(XPRopt,"OCY,GN,KN,TN");
			strcpy(XPRcmd,"RB");
			break;
		case 'C':
			GetFilename();
			if(strlen(filename)) {
				strcpy(XPRlib,"xprxmodem.library");
				strcpy(XPRopt,"T1,C0,K0");
				strcpy(XPRcmd,"R");
			}
			else
				return;
			break;
		case 'K':
			strcpy(XPRlib,"xprkermit.library");
			strcpy(XPRopt,"OCY,GN,KN,TY");
			strcpy(XPRcmd,"R");
			break;
		case 'X':
			GetFilename();
			if(strlen(filename)) {
				strcpy(XPRlib,"xprxmodem.library");
				strcpy(XPRopt,"T0,C1,K1");
				strcpy(XPRcmd,"RB");
			}
			else
				return;
			break;
		case 'Z':
			strcpy(XPRlib,"xprzmodem.library");
			strcpy(XPRopt,"T?,OR,B16,AN,KY,RN,P ");
			strcpy(XPRcmd,"R");
			break;
	}
	OUT("Ready to receive your file.");NL;
	t=timeleft();
	sprintf(tempfile,"T:FILE.%s",PLAYER.ID);
	if(!XPRtalk(tempfile)) {
		beep();
		NL;
		sprintf(outbuf,"%s*** %sUPLOAD ERROR%s ***",fore(BLU),fore(BRED),fore(BLU));
		OUT(outbuf);NL;
		NORMAL;
	}
	else {
		PLAYER.LastTime+=(t-timeleft()+1);
		if(PLAYER.LastTime%100>59)
			PLAYER.LastTime+=40;
		if((LOGfp=fopen(tempfile,"r"))) {
			while(!feof(LOGfp)) {
				finputs(filename,FILE_SIZE,LOGfp);
				strmfp(outbuf,directory,"file.log");
				if((fp=fopen(outbuf,"a"))) {
					GetSystemDate(Today);
					GetSystemTime(Time);
					julian(&Julian,Today);
					fdate(timestamp,Julian);
					timestamp[10]=' ';
					ftime(&timestamp[11],Time);
					fprintf(fp,"%.18s  %s uploaded %s.\n",timestamp,PLAYER.Handle,filename);
					fclose(fp);
				}
				if((lock=Lock(filename,ACCESS_READ))) {
					if((Examine(lock,&infoBlock)))
						if(infoBlock.fib_Size) {
							NL;
							sprintf(outbuf,"Received %s (%ld bytes)",filename,infoBlock.fib_Size);
							OUT(outbuf);NL;
							sprintf(prompt,"%sEnter file description: ",fore(CYN));
							OUT(prompt);NL;
							ins(80);
							NL;NL;
							SetComment(filename,inbuf);
							JOUST(PLAYER.Level);
							modf(d/5.,&d);
							sprintf(outbuf,"Thank you for the upload... you are rewarded %s!",money(d,ANSI));
							OUT(outbuf);NL;
							PLAYER.Gold+=d;
							PLAYER.Uploads++;
							PLAYER.UpByte+=infoBlock.fib_Size;
						}
					UnLock(lock);
				}
			}
			fclose(LOGfp);
		}
	}
	remove(tempfile);
	CurrentDir(root);
}

char NFScmd(void)
{
	char c;

	do {
		NL;
		sprintf(outbuf,"%s[%s%s%s]%s <D>ownload, <N>ext, <Q>uit: ",fore(MAG),fore(YELLOW),(MaxDir) ? ta[section] : FILEname[base],fore(MAG),fore(CYN));
		OUT(outbuf);
		c=inkey('N','Q');
	} while(c!='D' && c!='N' && c!='Q');
	NL;NL;
	if(c=='D')
		DownLoadFile();
	return(c);
}

char NewFileScan(void)
{
	struct DateStamp *now;
	long t;
	int i,lp;
	char c,filedate[12],*p;

	if((lock=Lock("",ACCESS_READ))) {
		if((Examine(lock,&infoBlock))) {
			now=__timecvt(time(NULL));
			LastFileScan=now->ds_Days-1;
			lp=0;
			c='N';
			while(ExNext(lock,&infoBlock) && c!='Q') {
				if(infoBlock.fib_DirEntryType<0 && infoBlock.fib_Date.ds_Days>=PLAYER.LastFileScan) {
					if(!lp) {
						NL;
						sprintf(outbuf,"%s    Filename       Size   Upload Date      Description",fore(MAG));
						OUT(outbuf);NL;
						sprintf(outbuf,"%s----------------  ------  -----------  -------------------",fore(GRY));
						OUT(outbuf);NL;
						NORMAL;
						lp=2;
					}
					t=getft(infoBlock.fib_FileName);
					strftime(filedate,sizeof(filedate),"%d-%b-%Y",localtime(&t));
					sprintf(outbuf,"%s%-16.16s %6luk  %-11.11s  %s",fore(WHITE),infoBlock.fib_FileName,(infoBlock.fib_Size-1)/1024+1,filedate,fore(CYN));
					OUT(outbuf);
					strcpy(outbuf,infoBlock.fib_Comment);
					p=outbuf;
					while(strlen(p)>40) {
						for(i=40; i>0 && p[i]!=' '; i--);
						p[i]='\0';
						sprintf(outbuf,"%s",p);
						OUT(p);
						p+=i+1;
						NL;
						lp++;
						OUT("                                        ");
					}
					OUT(p);NL;
					lp++;
					if(lp+2>=PLAYER.Rows) {
						lp=0;
						c=NFScmd();
					}
				}
			}
			if(!lp) {
				OUT("None.");NL;
			}
			else if(lp%5)
				c=NFScmd();
		}
		UnLock(lock);
	}
	return(c);
}

void FileAction(void)
{
	static struct RPGmenu __far RPGmenu = { NULL,MAG,3,
							{	{ 'D',"Download" },
								{ 'L',"List all files" },
								{ 'N',"New files scan" },
							}
	};
	static struct RPGmenu __far SYSOPmenu = { NULL,MAG,5,
							{	{ 'A',"Attach a comment" },
								{ 'D',"Download" },
								{ 'L',"List all files" },
								{ 'N',"New files scan" },
								{ 'R',"Remove file" },
							}
	};

	long t;
	int i,lp;
	char c,filedate[12],*p;

	while(!cd()) {
		RPGmenu.option=(MaxDir) ? ta[section] : FILEname[base];
		SYSOPmenu.option=(MaxDir) ? ta[section] : FILEname[base];
		if((c=option(ACCESS(PLAYER.Access)->Sysop=='Y' ? &SYSOPmenu : &RPGmenu))=='Q')
			break;
		switch(c) {
			case 'A':
				GetFilename();
				if(strlen(filename)) {
					sprintf(prompt,"%sEnter a comment: ",fore(CYN));
					OUT(prompt);
					ins(80);
					SetComment(filename,inbuf);
				}
				break;
			case 'D':
				DownLoadFile();
				break;
			case 'L':
				if((lock=Lock("",ACCESS_READ))) {
					if((Examine(lock,&infoBlock))) {
						sprintf(outbuf,"%s    Filename       Size   Upload Date      Description",fore(MAG));
						OUT(outbuf);NL;
						sprintf(outbuf,"%s----------------  ------  -----------  -------------------",fore(GRY));
						OUT(outbuf);NL;
						NORMAL;
						lp=2;
						c='Y';
						while(ExNext(lock,&infoBlock) && c!='N') {
							if(infoBlock.fib_DirEntryType<0) {
								t=getft(infoBlock.fib_FileName);
								strftime(filedate,sizeof(filedate),"%d-%b-%Y",localtime(&t));
								sprintf(outbuf,"%s%-16.16s %6luk  %-11.11s  %s",fore(WHITE),infoBlock.fib_FileName,(infoBlock.fib_Size-1)/1024+1,filedate,fore(CYN));
								OUT(outbuf);
								strcpy(outbuf,infoBlock.fib_Comment);
								p=outbuf;
								while(strlen(p)>40) {
									for(i=40; i>0 && p[i]!=' '; i--);
									p[i]='\0';
									sprintf(outbuf,"%s",p);
									OUT(p);
									p+=i+1;
									NL;
									lp++;
									OUT("                                        ");
								}
								OUT(p);
								NL;
								lp++;
								if(lp%PLAYER.Rows==0)
									c=more();
							}
						}
					}
					UnLock(lock);
					paws=!PLAYER.Expert;
				}
				break;
			case 'N':
				NewFileScan();
				paws=!PLAYER.Expert;
				break;
			case 'R':
				GetFilename();
				if(strlen(filename))
					remove(filename);
				break;
		}
	}
}

void Files(void)
{
	struct DiskObject *diskobj;
	BPTR filelock,sublock;
	int MaxRow,MaxCol;
	char c;
	char *s;
	char **tool;
	int i;

	while(!cd()) {
		title("Files");
		cls();
		if(RPGBBSmode==DOOR) {
			OUT("This section can only be used when RPGBBS Deluxe is in BBS mode.");NL;
			paws=!PLAYER.Expert;
			break;
		}
		CurrentDir(root);
		sprintf(outbuf,"%s--=:))%s%s FILE SECTION %s%s((:=--",fore(BLU),fore(WHITE),back(BLU),fore(BLU),back(BLK));
		OUT(outbuf);NL;NL;
		NORMAL;
		sprintf(outbuf," %s<%sU%s>%s Upload a file",fore(BLACK),fore(WHITE),fore(BLACK),fore(GRY));
		OUT(outbuf);NL;
		for(base=0; base<MaxFILE; base++) {
			sprintf(outbuf,"%s%s %s",bracket(base+1),fore(GRY),FILEname[base]);
			OUT(outbuf);NL;
		}
		NL;
		timeleft();
		if(!cd()) {
			sprintf(prompt,"%sEnter File Base # (Q=Quit): ",fore(CYN));
			OUT(prompt);
		}
		if(ins(2))
			break;
		NL;NL;
		c=toupper(inbuf[0]);
		if(c=='Q')
			break;
		if(c=='U') {
			UpLoadFile();
			continue;
		}
		base=atoi(inbuf);
		if(base<1 || base>MaxFILE)
			continue;
		base--;

		if(!(filelock=Lock(FILEpath[base],ACCESS_READ))) {
			sprintf(outbuf,"%sSorry, could not access that File Base directory \"%s\"",fore(CYAN),FILEpath);
			OUT(outbuf);NL;
			pause();
			continue;
		}

		while(!cd()) {
			CurrentDir(filelock);
			if((lock=Lock("",ACCESS_READ))) {
				if((Examine(lock,&infoBlock))) {
					MaxDir=0;
					while(ExNext(lock,&infoBlock)) {
						if(infoBlock.fib_DirEntryType>0) {
							i=TRUE;
							if(ACCESS(PLAYER.Access)->Sysop!='Y')
								if((diskobj=GetDiskObject(infoBlock.fib_FileName))) {
									tool=(char **)diskobj->do_ToolTypes;
									for(i=NUMACCESS-1; i>0 && !(s=(char *)FindToolType(tool,ACCESS(i)->Name)); i--);
									FreeDiskObject(diskobj);
								}
							if(i) {
								ta[MaxDir++]=line[MaxDir];
								strcpy(line[MaxDir],infoBlock.fib_FileName);
								if(MaxDir==48)
									break;
							}
						}
					}
				}
				UnLock(lock);
				if(MaxDir)
					tqsort(ta,MaxDir);
			}
			MaxCol=(MaxDir<10) ? 1 : (MaxDir<20) ? 2 : 3;
			MaxRow=MaxDir/MaxCol+MaxDir%MaxCol;
			for(section=0; section<MaxRow; section++) {
				sprintf(outbuf,"%s %s%-20.20s  ",bracket(section+1),fore(WHITE),ta[section]);
				OUT(outbuf);
				if(section+MaxRow<MaxDir) {
					sprintf(outbuf,"%s %s%-20.20s  ",bracket(section+MaxRow+1),fore(WHITE),ta[section+MaxRow]);
					OUT(outbuf);
				}
				if(section+2*MaxRow<MaxDir) {
					sprintf(outbuf,"%s %s%-20.20s",bracket(section+2*MaxRow+1),fore(WHITE),ta[section+2*MaxRow]);
					OUT(outbuf);
				}
				NL;
			}
			if(MaxDir) {
				NL;
				timeleft();
				if(!cd()) {
					OUT(fore(CYN));
					OUT("Enter Section # or <N>ew files (Q=Quit): ");
				}
				if(ins(2))
					break;
				NL;
				c=toupper(inbuf[0]);
				if(c=='Q')
					break;
				if(c=='N') {
					for(section=0; section<MaxDir; section++) {
						CurrentDir(filelock);
						if((sublock=Lock(ta[section],ACCESS_READ))) {
							CurrentDir(sublock);
							sprintf(outbuf,"Scanning %s...",ta[section]);
							OUT(outbuf);
							if(NewFileScan()=='Q')
								break;
							NL;
							UnLock(sublock);
						}
					}
				}
				else {
					section=atoi(inbuf);
					if(section>0 && section<=MaxDir) {
						if((sublock=Lock(ta[--section],ACCESS_READ))) {
							CurrentDir(sublock);
							FileAction();
							UnLock(sublock);
						}
					}
				}
			}
			else {
				FileAction();
				break;
			}
		}
		UnLock(filelock);
	}
}
