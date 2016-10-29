/*************************\
 *  RPGBBS File Section  *
\*************************/
#include "RPGBBS.h"

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

UBYTE __far version[80];
struct NewWindow __far newXPRwindow = {
	80,50,480,100,1,0,
	CLOSEWINDOW,
	GIMMEZEROZERO | SMART_REFRESH | WINDOWCLOSE | WINDOWDEPTH | WINDOWDRAG,
	NULL,NULL,
	version,
	NULL,NULL,
	480,100,480,100,
	CUSTOMSCREEN
};

FILE *LOGfp;
UBYTE __far XPRlib[32];
UBYTE __far XPRopt[128];
UBYTE __far XPRcmd[3];
UBYTE __far BinaryTransfer;
LONG __far Bytes,Blocks,BlockSize,Errors,Timeouts;
UBYTE __far output[61];
UBYTE __far TraceFlag=FALSE;

void trace(char *msg)
{
	if(TraceFlag) {
		sprintf(output,"XPRtrace: %-50.50s",msg);
		Move(XPRwindow->RPort,4,80);
		Text(XPRwindow->RPort,output,60);
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

	strsfn(FileName,NULL,NULL,text,extension);
	text[12]=NULL;
	strupr(text);
	extension[3]=NULL;
	strlwr(extension);
	strmfe(filename,text,extension);
	sprintf(text,"fopen(\"%s\",\"%s\")",FileName,AccessMode);
	trace(text);
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
	sprintf(text,"fread(buffer,%ld,fp)",Size);
	trace(text);
	return((LONG)fread(Buffer,Size,Count,File));
}

LONG __saveds __asm
xpr_fwrite(register __a0 APTR Buffer,register __d0 LONG Size,register __d1 LONG Count,register __a1 FILE *File)
{
	sprintf(text,"fwrite(buffer,%ld,fp)",Size);
	trace(text);
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

	sprintf(text,"sread(buffer,%ld,%ld)",Size,Timeout);
	trace(text);
	if(Size>0) {
		if(Timeout<1000)
			seconds=Timeout*50;
		else
			seconds=Timeout/1000000L*50;
		for(timer=0; timer<=seconds; timer++) {
			if(xpr_chkabort())
				return(-1L);	/* user abort */
			SerialIO->IOSer.io_Command=SDCMD_QUERY;
			DoIO((struct IORequest *)SerialIO);
			if(SerialIO->io_Status & 32)
				return(0L);		/* carrier lost */
			if(SerialIO->IOSer.io_Actual>=Size) {
				SerialIO->IOSer.io_Command=CMD_READ;
				SerialIO->IOSer.io_Data=Buffer;
				SerialIO->IOSer.io_Length=Size;
				DoIO((struct IORequest *)SerialIO);
				return(Size);
			}
			Delay(1);
		}
		length=(SerialIO->IOSer.io_Actual>Size) ? Size : SerialIO->IOSer.io_Actual;
		if(length) {
			SerialIO->IOSer.io_Command=CMD_READ;
			SerialIO->IOSer.io_Data=Buffer;
			SerialIO->IOSer.io_Length=length;
			DoIO((struct IORequest *)SerialIO);
			return(length);
		}
	}
	return(0L);
}

LONG __saveds __asm
xpr_swrite(register __a0 APTR Buffer,register __d0 LONG Size)
{
	trace("swrite()");
	SerialIO->IOSer.io_Command=SDCMD_QUERY;
	DoIO((struct IORequest *)SerialIO);
	if(!(SerialIO->io_Status & 32)) {
		SerialIO->IOSer.io_Command=CMD_WRITE;
		SerialIO->IOSer.io_Data=Buffer;
		SerialIO->IOSer.io_Length=Size;
		if(DoIO((struct IORequest *)SerialIO))
			return(-1L);
		return(0L);
	}
	return(-1L);
}

LONG __saveds __asm
xpr_sflush(void)
{
	trace("sflush()");
	SerialIO->IOSer.io_Command=CMD_CLEAR;
	if(!DoIO((struct IORequest *)SerialIO))
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
			sprintf(text,"%.30s (%.16s)",XPRlib,(UpdateInfo->xpru_blockcheck) ? UpdateInfo->xpru_blockcheck : "n/a");
			sprintf(output,"Protocol: %-50.50s",text);
			Move(XPRwindow->RPort,4,10);
			Text(XPRwindow->RPort,output,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_FILENAME) && UpdateInfo->xpru_filename) {
			sprintf(output,"Filename: %-50.50s",UpdateInfo->xpru_filename);
			Move(XPRwindow->RPort,4,20);
			Text(XPRwindow->RPort,output,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_FILESIZE)) {
			sprintf(text,"%ld bytes",display_value(UpdateInfo->xpru_filesize));
			sprintf(output,"Filesize: %-50.50s",text);
			Move(XPRwindow->RPort,4,30);
			Text(XPRwindow->RPort,output,60);
		}
		if((UpdateInfo->xpru_updatemask & (XPRU_BYTES | XPRU_BLOCKS | XPRU_BLOCKSIZE))) {
			if((UpdateInfo->xpru_updatemask & XPRU_BYTES))
				Bytes=display_value(UpdateInfo->xpru_bytes);
			if((UpdateInfo->xpru_updatemask & XPRU_BLOCKS))
				Blocks=display_value(UpdateInfo->xpru_blocks);
			if((UpdateInfo->xpru_updatemask & XPRU_BLOCKSIZE))
				BlockSize=display_value(UpdateInfo->xpru_blocksize);
			sprintf(text,"%ld bytes, %ld blocks, %ld blocksize",Bytes,Blocks,BlockSize);
			sprintf(output,"Transfer: %-50.50s",text);
			Move(XPRwindow->RPort,4,40);
			Text(XPRwindow->RPort,output,60);
		}
		if((UpdateInfo->xpru_updatemask & (XPRU_ERRORS | XPRU_TIMEOUTS))) {
			if((UpdateInfo->xpru_updatemask & XPRU_ERRORS))
				Errors=display_value(UpdateInfo->xpru_errors);
			if((UpdateInfo->xpru_updatemask & XPRU_TIMEOUTS))
				Timeouts=display_value(UpdateInfo->xpru_timeouts);
			sprintf(text,"%ld     Timeouts: %ld",Errors,Timeouts);
			sprintf(output,"# Errors: %-50.50s",text);
			Move(XPRwindow->RPort,4,50);
			Text(XPRwindow->RPort,output,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_ERRORMSG) && UpdateInfo->xpru_errormsg) {
			sprintf(output,"Last err: %-50.50s",UpdateInfo->xpru_errormsg);
			Move(XPRwindow->RPort,4,60);
			Text(XPRwindow->RPort,output,60);
		}
		if((UpdateInfo->xpru_updatemask & XPRU_MSG) && UpdateInfo->xpru_msg) {
			sprintf(output," Message: %-50.50s",UpdateInfo->xpru_msg);
			Move(XPRwindow->RPort,4,70);
			Text(XPRwindow->RPort,output,60);
		}
	}
	return(1L);
}

LONG __saveds __asm
xpr_gets(register __a0 STRPTR Prompt,register __a1 STRPTR Buffer)
{
	if(TraceFlag) {
		sprintf(text,"gets(\"%s\",buffer)",Prompt);
		trace(text);
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
		sprintf(text,"setserial(%lx)",Status);
		trace(text);
	}
	SerialIO->IOSer.io_Command=SDCMD_QUERY;
	DoIO((struct IORequest *)SerialIO);
	result=SerialIO->io_SerFlags & 0xFF;
	if(SerialIO->io_ExtFlags & SEXTF_MSPON)
		result|=ST_PARTYMARKON;
	if(SerialIO->io_ExtFlags & SEXTF_MARK)
		result|=ST_PARTYMARK;
	if(SerialIO->io_StopBits==2)
		result|=ST_2BITS;
	if(SerialIO->io_ReadLen==7)
		result|=ST_READ7;
	if(SerialIO->io_WriteLen==7)
		result|=ST_WRITE7;
	for(i=0 ; i<12 ; i++)
		if(XprBauds[i]>=SerialIO->io_Baud) {
			result|=(i << 16);
			break;
		}

	if(Status != -1) {
		SerialIO->io_SerFlags=Status & 0xFF;
		SerialIO->io_ExtFlags=0;
		if(Status & ST_PARTYMARKON)
			SerialIO->io_ExtFlags|=SEXTF_MSPON;
		if(Status & ST_PARTYMARK)
			SerialIO->io_ExtFlags|=SEXTF_MARK;
		if(Status & ST_2BITS)
			SerialIO->io_StopBits=2;
		else
			SerialIO->io_StopBits=1;
		if(Status & ST_READ7)
			SerialIO->io_ReadLen=7;
		else
			SerialIO->io_ReadLen=8;
		if(Status & ST_WRITE7)
			SerialIO->io_WriteLen=7;
		else
			SerialIO->io_WriteLen=8;

		SerialIO->io_Baud		= SerialIO->io_Baud;
		SerialIO->io_SerFlags	= SerialIO->io_SerFlags;
		SerialIO->io_ExtFlags	= SerialIO->io_ExtFlags;
		SerialIO->io_StopBits	= SerialIO->io_StopBits;
		SerialIO->io_ReadLen	= SerialIO->io_ReadLen;
		SerialIO->io_WriteLen	= SerialIO->io_WriteLen;
		SerialIO->IOSer.io_Command = SDCMD_SETPARAMS;
		DoIO((struct IORequest *)SerialIO);
	}
	return(result);
}

LONG __saveds __asm
xpr_ffirst(register __a0 STRPTR Buffer,register __a1 STRPTR Pattern)
{
	if(TraceFlag) {
		sprintf(text,"ffirst(buffer,\"%s\")",Pattern);
		trace(text);
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
		sprintf(text,"fnext(state,buffer,\"%s\")",Pattern);
		trace(text);
	}
	strcpy(Buffer,"");
	return(0L);
}

LONG __saveds __asm
xpr_finfo(register __a0 STRPTR FileName,register __d0 LONG InfoType)
{
	BPTR lock;
	struct FileInfoBlock infoBlock;
	LONG size;

	if(TraceFlag) {
		sprintf(text,"finfo(\"%s\",%ld)",FileName,InfoType);
		trace(text);
	}
	switch(InfoType) {
		case 1:			/* Return the file size. */
			size=0;
			if((lock=Lock(FileName,ACCESS_READ))) {
				if((Examine(lock,&infoBlock)))
					size=infoBlock.fib_Size;
				UnLock(lock);
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
	SerialIO->IOSer.io_Command=SDCMD_QUERY;
	DoIO((struct IORequest *)SerialIO);
	if(!(SerialIO->io_Status & 32))
		return((LONG)SerialIO->IOSer.io_Actual);
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
		sprintf(text,"unlink(\"%s\")",FileName);
		trace(text);
	}
	return((LONG)remove(FileName));
}

LONG __saveds __asm
xpr_getptr(register __d0 LONG InfoType)
{
	if(TraceFlag) {
		sprintf(text,"getptr(%ld)",InfoType);
		trace(text);
	}
	if(InfoType == 1)
		return(0L);
	else
		return(-1L);
}

int XPRtalk(char *logfile)
{
	int i,result=-1;

	newXPRwindow.Screen=screen;
	if(logfile)
		LOGfp=fopen(logfile,"w");
	BinaryTransfer=(XPRcmd[1]=='B');

	sprintf(version,"XPR interface %s ",__AMIGADATE__);
	if((XPRwindow=OpenWindow(&newXPRwindow))) {
		SetRast(XPRwindow->RPort,BBSprefs.Depth<3 ? BLK : WHITE);
		SetDrMd(XPRwindow->RPort,JAM2);
		SetAPen(XPRwindow->RPort,RED);
		SetBPen(XPRwindow->RPort,BBSprefs.Depth<3 ? BLK : WHITE);
		sprintf(text,"Protocol: %-50.50s",XPRlib);
		Move(XPRwindow->RPort,4,10);
		Text(XPRwindow->RPort,text,strlen(text));
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
						sprintf(text,"Filename: %-50.50s",XPR_IO.xpr_filename);
						Move(XPRwindow->RPort,4,20);
						Text(XPRwindow->RPort,text,strlen(text));
						result=XProtocolReceive(&XPR_IO);
						break;
					case 'S':
						XPR_IO.xpr_filename=filename;
						sprintf(text,"Filename: %-50.50s",XPR_IO.xpr_filename);
						Move(XPRwindow->RPort,4,20);
						Text(XPRwindow->RPort,text,strlen(text));
						result=XProtocolSend(&XPR_IO);
						break;
				}
				XProtocolCleanup(&XPR_IO);
			}
			else {
				sprintf(text,"XPRerror: %X",i);
				Move(XPRwindow->RPort,4,20);
				Text(XPRwindow->RPort,text,strlen(text));
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
	outs("Filename: ");
	if(input())
		return;
	newline();
	newline();
	memset(filename,0,sizeof(filename));
	strsfn(ins,NULL,NULL,text,extension);
	text[12]=NULL;
	strupr(text);
	extension[3]=NULL;
	strlwr(extension);
	if(strlen(text))
		strmfe(filename,text,extension);
}

void DownLoadFile(void)
{
	BPTR lock;
	struct FileInfoBlock infoBlock;
	FILE *fp;
	double RPGratio,USERratio;
	long size;
	char c;

	sprintf(OUTbuf,"%s  Uploads:%s %u  (%lu bytes)",Fcol(CYN),Fcol(WHITE),USER(user)->ULfiles,USER(user)->ULbytes);
	outs(OUTbuf);
	newline();
	sprintf(OUTbuf,"%sDownloads:%s %u  (%lu bytes)",Fcol(CYN),Fcol(WHITE),USER(user)->DLfiles,USER(user)->DLbytes);
	outs(OUTbuf);
	newline();
	if(USER(0)->DLfiles)
		RPGratio=(double)USER(0)->ULfiles/(double)USER(0)->DLfiles;
	else
		RPGratio=0.;
	if(USER(user)->DLfiles)
		USERratio=(double)USER(user)->ULfiles/(double)USER(user)->DLfiles;
	else
		USERratio=1000.;
	sprintf(OUTbuf,"%sU/D Ratio:%s %g  ",Fcol(CYN),Fcol(WHITE),USERratio);
	outs(OUTbuf);
	if(USER(user)->Ratio=='Y') {
		if(USERratio<=RPGratio) {
			beep();
			sprintf(OUTbuf,"%s(Upload/Download ratio exceeded)",Fcol(YELLOW));
			outs(OUTbuf);
			newline();
			return;
		}
	}
	else
		outs("(disabled)");
	newline();
	newline();
	GetFilename();
	if(strlen(filename)) {
		if((lock=Lock(filename,ACCESS_READ))) {
			size=0;
			if((Examine(lock,&infoBlock)))
				size=infoBlock.fib_Size;
			UnLock(lock);
			if((c=Option(&XPRmenu))=='Q')
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
			sprintf(OUTbuf,"Ready to send you %s",filename);
			outs(OUTbuf);
			newline();
			if(!XPRtalk(NULL)) {
				beep();
				newline();
				sprintf(OUTbuf,"%s*** %sDOWNLOAD ERROR%s ***",Fcol(BLU),Fcol(BRED),Fcol(BLU));
				outs(OUTbuf);
				normal();
				newline();
			}
			else {
				USER(user)->DLfiles++;
				USER(user)->DLbytes+=size;
				PutUser(user);
				strmfp(OUTbuf,directory,"file.log");
				if((fp=fopen(OUTbuf,"a"))) {
					GetSystemDateTime();
					fprintf(fp,"%s  %s  %.26s downloaded %s from %s-%s.\n",Fdate(SystemDate),Ftime(Time),USER(user)->Handle,filename,FILEname[base],(MaxDir) ? ta[section] : "");
					fclose(fp);
				}
			}
		}
	}
}

void UpLoadFile(void)
{
	BPTR lock;
	struct FileInfoBlock infoBlock;
	struct InfoData info;
	FILE *fp;
	long t;
	char c,tempfile[40];

	chdir(directory);
	chdir(UPLOADpath);
	strsfn(FILEpath[base],filename,NULL,NULL,NULL);
	getdfs(filename,&info);
	sprintf(OUTbuf,"There are %ld disk blocks (%ld bytes) available.",info.id_NumBlocks-info.id_NumBlocksUsed,(info.id_NumBlocks-info.id_NumBlocksUsed)*info.id_BytesPerBlock);
	outs(OUTbuf);
	newline();
	if((c=Option(&XPRmenu))=='Q')
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
	outs("Ready to receive your file.");
	newline();
	t=TimeLeft();
	sprintf(tempfile,"T:RPGBBS.%d",user);
	if(!XPRtalk(tempfile)) {
		beep();
		newline();
		sprintf(OUTbuf,"%s*** %sUPLOAD ERROR%s ***",Fcol(BLU),Fcol(BRED),Fcol(BLU));
		outs(OUTbuf);
		normal();
		newline();
	}
	else {
		AdjustTime(t-TimeLeft()+1);		/* give the donator some time back */
		if((LOGfp=fopen(tempfile,"r"))) {
			while(!feof(LOGfp)) {
				inputs(filename,sizeof(filename),LOGfp);
				if((lock=Lock(filename,ACCESS_READ))) {
					if((Examine(lock,&infoBlock)))
						if(infoBlock.fib_Size) {
							sprintf(OUTbuf,"\nReceived %s (%ld bytes)\nEnter file description: ",filename,infoBlock.fib_Size);
							outs(OUTbuf);
							input();
							newline();
							newline();
							SetComment(filename,ins);
							JOUST(USER(user)->Level);
							modf(d/5.,&d);
							sprintf(text,"Thank you for the upload... you are rewarded %s!",money(d,user));
							outs(text);
							newline();
							USER(user)->Gold+=d;
							USER(user)->ULfiles++;
							USER(user)->ULbytes+=infoBlock.fib_Size;
							PutUser(user);
							strmfp(OUTbuf,directory,"file.log");
							if((fp=fopen(OUTbuf,"a"))) {
								GetSystemDateTime();
								fprintf(fp,"%s  %s  %.26s uploaded %s.\n",Fdate(SystemDate),Ftime(Time),USER(user)->Handle,filename);
								fclose(fp);
							}
						}
					UnLock(lock);
				}
			}
			fclose(LOGfp);
		}
	}
	remove(tempfile);
	chdir(directory);
}

char NFScmd(void)
{
	char c;

	do {
		newline();
		sprintf(OUTbuf,"%s[%s%s%s]%s <D>ownload, <N>ext, <Q>uit: ",Fcol(MAG),Fcol(YELLOW),(MaxDir) ? ta[section] : FILEname[base],Fcol(MAG),Fcol(CYN));
		outs(OUTbuf);
		if(!(c=inkey()))
			c='Q';
		if(c=='\15')
			c='N';
	} while(c!='D' && c!='N' && c!='Q');
	newline();
	newline();
	if(c=='D')
		DownLoadFile();
	return(c);
}

char NewFileScan(void)
{
	BPTR lock;
	struct FileInfoBlock infoBlock;
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
				if(infoBlock.fib_DirEntryType<0 && infoBlock.fib_Date.ds_Days>=USER(user)->LastFileScan) {
					if(!lp) {
						newline();
						sprintf(OUTbuf,"%s    Filename       Size   Upload Date      Description",Fcol(MAG));
						outs(OUTbuf);
						newline();
						sprintf(OUTbuf,"%s----------------  ------  -----------  -------------------",Fcol(GRY));
						outs(OUTbuf);
						newline();
						normal();
					}
					t=getft(infoBlock.fib_FileName);
					strftime(filedate,sizeof(filedate),"%d-%b-%Y",localtime(&t));
					sprintf(OUTbuf,"%s%-16.16s %6luk  %-11.11s  %s",Fcol(WHITE),infoBlock.fib_FileName,infoBlock.fib_Size/1024+1,filedate,Fcol(CYN));
					outs(OUTbuf);
					strcpy(OUTbuf,infoBlock.fib_Comment);
					p=OUTbuf;
					while(strlen(p)>40) {
						for(i=40; i>0 && p[i]!=' '; i--);
						p[i]='\0';
						sprintf(OUTbuf,"%s",p);
						outs(p);
						p+=i+1;
						newline();
						lp++;
						outs("                                        ");
					}
					outs(p);
					newline();
					lp++;
					if(lp>=5) {
						lp=0;
						c=NFScmd();
					}
				}
			}
			if(!lp) {
				outs("None.");
				newline();
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
	static struct RPGmenu __far RPGmenu = { NULL,MAG,4,
							{	{ 'D',"Download" },
								{ 'L',"List all files" },
								{ 'N',"New files scan" },
								{ 'U',"Upload" }
							}
	};
	static struct RPGmenu __far SYSOPmenu = { NULL,MAG,6,
							{	{ 'A',"Attach a comment" },
								{ 'D',"Download" },
								{ 'L',"List all files" },
								{ 'N',"New files scan" },
								{ 'R',"Remove file" },
								{ 'U',"Upload" },
							}
	};

	BPTR lock;
	struct FileInfoBlock infoBlock;
	long t;
	int i,lp;
	char c,filedate[12],*p;

	while(!cd()) {
		RPGmenu.option=(MaxDir) ? ta[section] : FILEname[base];
		SYSOPmenu.option=(MaxDir) ? ta[section] : FILEname[base];
		if((c=Option(ACCESS(USER(user)->Access)->Sysop=='Y' ? &SYSOPmenu : &RPGmenu))=='Q')
			break;
		switch(c) {
			case 'A':
				GetFilename();
				if(strlen(filename)) {
					outs("Enter comment: ");
					input();
					SetComment(filename,ins);
				}
				break;
			case 'D':
				DownLoadFile();
				break;
			case 'L':
				if((lock=Lock("",ACCESS_READ))) {
					if((Examine(lock,&infoBlock))) {
						sprintf(OUTbuf,"%s    Filename       Size   Upload Date      Description",Fcol(MAG));
						outs(OUTbuf);
						newline();
						sprintf(OUTbuf,"%s----------------  ------  -----------  -------------------",Fcol(GRY));
						outs(OUTbuf);
						newline();
						normal();
						lp=2;
						c='Y';
						while(ExNext(lock,&infoBlock) && c!='N') {
							if(infoBlock.fib_DirEntryType<0) {
								t=getft(infoBlock.fib_FileName);
								strftime(filedate,sizeof(filedate),"%d-%b-%Y",localtime(&t));
								sprintf(OUTbuf,"%s%-16.16s %6luk  %-11.11s  %s",Fcol(WHITE),infoBlock.fib_FileName,infoBlock.fib_Size/1024+1,filedate,Fcol(CYN));
								outs(OUTbuf);
								strcpy(OUTbuf,infoBlock.fib_Comment);
								p=OUTbuf;
								while(strlen(p)>40) {
									for(i=40; i>0 && p[i]!=' '; i--);
									p[i]='\0';
									sprintf(OUTbuf,"%s",p);
									outs(p);
									p+=i+1;
									newline();
									lp++;
									outs("                                        ");
								}
								outs(p);
								newline();
								lp++;
								if(lp%USER(user)->Rows==0)
									c=More();
							}
						}
					}
					UnLock(lock);
				}
				break;
			case 'N':
				NewFileScan();
				break;
			case 'U':
				UpLoadFile();
				chdir(FILEpath[base]);
				if(MaxDir)
					chdir(ta[section]);
				break;
			case 'R':
				GetFilename();
				if(strlen(filename))
					remove(filename);
				break;
		}
	}
}

void FileSection(void)
{
	BPTR lock;
	struct DiskObject *diskobj;
	struct FileInfoBlock infoBlock;
	int MaxRow,MaxCol;
	char c;
	char *s;
	char **tool;
	int i;

	getcwd(directory,sizeof(directory));
	while(!cd()) {
		cls();
		sprintf(text,"%s--=:))%s%s FILE SECTION %s%s((:=--",Fcol(BLU),Fcol(WHITE),Bcol(BLU),Fcol(BLU),Bcol(BLK));
		outs(text);
		normal();
		newline();
		newline();
		sprintf(OUTbuf," %s<%sU%s>%s Upload a file",Fcol(BLU),Fcol(WHITE),Fcol(BLU),Fcol(GRY));
		outs(OUTbuf);
		newline();
		for(base=0; base<MaxFILE; base++) {
			sprintf(OUTbuf,"%s %s",bracket(base+1),FILEname[base]);
			outs(OUTbuf);
			newline();
		}
		newline();
		Midnight();
		if(!cd()) {
			outs(Fcol(CYN));
			outs("Enter File Base # (Q=Quit): ");
		}
		if(input())
			break;
		newline();
		newline();
		c=toupper(ins[0]);
		if(c=='Q')
			break;
		if(c=='U')
			UpLoadFile();
		base=atoi(ins);
		if(base<1 || base>MaxFILE)
			continue;
		base--;

		while(!cd()) {
			chdir(FILEpath[base]);
			if((lock=Lock("",ACCESS_READ))) {
				if((Examine(lock,&infoBlock))) {
					MaxDir=0;
					while(ExNext(lock,&infoBlock)) {
						if(infoBlock.fib_DirEntryType>0) {
							i=TRUE;
							if(ACCESS(USER(user)->Access)->Sysop!='Y')
								if((diskobj=GetDiskObject(infoBlock.fib_FileName))) {
									tool=(char **)diskobj->do_ToolTypes;
									for(i=MAXACCESS-1; i>0 && !(s=(char *)FindToolType(tool,ACCESS(i)->Name)); i--);
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
				sprintf(OUTbuf,"%s %s%-20.20s  ",bracket(section+1),Fcol(WHITE),ta[section]);
				outs(OUTbuf);
				if(section+MaxRow<MaxDir) {
					sprintf(OUTbuf,"%s %s%-20.20s  ",bracket(section+MaxRow+1),Fcol(WHITE),ta[section+MaxRow]);
					outs(OUTbuf);
				}
				if(section+2*MaxRow<MaxDir) {
					sprintf(OUTbuf,"%s %s%-20.20s",bracket(section+2*MaxRow+1),Fcol(WHITE),ta[section+2*MaxRow]);
					outs(OUTbuf);
				}
				newline();
			}
			if(MaxDir) {
				newline();
				Midnight();
				if(!cd()) {
					outs(Fcol(CYN));
					outs("Enter Section # or <N>ew files (Q=Quit): ");
				}
				if(input())
					break;
				newline();
				c=toupper(ins[0]);
				if(c=='Q')
					break;
				if(c=='N') {
					for(section=0; section<MaxDir; section++) {
						chdir(ta[section]);
						sprintf(OUTbuf,"Scanning %s...",ta[section]);
						outs(OUTbuf);
						if(NewFileScan()=='Q')
							break;
						newline();
						chdir(directory);
						chdir(FILEpath[base]);
					}
				}
				else {
					section=atoi(ins);
					if(section>0 && section<=MaxDir) {
						chdir(ta[--section]);
						FileAction();
					}
				}
			}
			else {
				FileAction();
				break;
			}
			chdir(directory);
		}
		chdir(directory);
	}
	chdir(directory);
}
