/*********************************\
 *  ILBM to PXL File Conversion  *
 *                               *
 *    Written by Robert Hurst    *
\*********************************/
#include <proto/exec.h>
#include <exec/memory.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <ctype.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pragmas/iff_pragmas.h>

char __near *__procname="ILBM2PXL";
long __near __stack=4096;
long __near __priority=0;

struct BitMapHeader {
	UWORD w,h;
	UWORD x,y;
	UBYTE nPlanes;
	UBYTE masking;
	UBYTE compression;
	UBYTE pad1;
	UWORD transparentColor;
	UBYTE xAspect;
	UBYTE yAspect;
	UWORD pageWidth;
	UWORD pageHeight;
};

extern	FILE *IFFL_OpenIFF(char *,char);
extern	struct BitMapHeader *IFFL_GetBMHD(FILE *);
extern	int IFFL_DecodePic(FILE *file,struct BitMap *);
extern	void IFFL_CloseIFF(FILE *);

struct FileRequester *FileRequester;
struct Library *AslBase,*IFFBase;
struct BitMapHeader *bmhd;
struct BitMap BitMap;

FILE *file,*fp;

char IFFdirname[128],IFFfilename[128],IFFpathname[128];
char PXLdirname[128],PXLfilename[128],PXLpathname[128];

void main(int argc,char **argv)
{
	long planesize;
	int i,num,ok,row,col,bit,color,lastcolor;
	unsigned char byte;

	getcwd(IFFdirname,sizeof(IFFdirname));
	memset(IFFfilename,0,sizeof(IFFfilename));
	memset(IFFpathname,0,sizeof(IFFpathname));
	if((AslBase=OpenLibrary("asl.library",0))) {
		if((FileRequester=AllocAslRequestTags(ASL_FileRequest,
				ASL_Hail, (ULONG)"Select an ILBM file",
				ASL_Dir, (ULONG)IFFdirname,
				ASL_File, (ULONG)IFFfilename,
				ASL_FuncFlags, FILF_PATGAD,
				ASL_Pattern, (ULONG)"~(#?.info)",
				TAG_DONE))) {
			if(AslRequest(FileRequester,NULL)) {
				strcpy(IFFdirname,FileRequester->rf_Dir);
				strcpy(IFFfilename,FileRequester->rf_File);
				strmfp(IFFpathname,IFFdirname,IFFfilename);
			}
			FreeAslRequest(FileRequester);
		}
		if(strlen(IFFpathname))
			if((IFFBase=OpenLibrary("iff.library",0L))) {
				if((file=IFFL_OpenIFF(IFFpathname,0))) {
					if((bmhd=IFFL_GetBMHD(file))) {
						InitBitMap(&BitMap,bmhd->nPlanes,bmhd->w,bmhd->h);
						ok=TRUE;
						planesize=BitMap.BytesPerRow*BitMap.Rows;
						for(i=0; i<bmhd->nPlanes; i++)
							if(!(BitMap.Planes[i]=AllocMem(planesize,MEMF_CHIP|MEMF_CLEAR)))
								ok=FALSE;
						if(ok)
							if((IFFL_DecodePic(file,&BitMap))) {
								strsfn(IFFpathname,PXLdirname,PXLpathname,PXLfilename,NULL);
								strcat(PXLdirname,PXLpathname);
								strmfe(PXLfilename,PXLfilename,"PXL");
								if((FileRequester=AllocAslRequestTags(ASL_FileRequest,
										ASL_Hail, (ULONG)"Save image to .PXL file",
										ASL_Dir, (ULONG)PXLdirname,
										ASL_File, (ULONG)PXLfilename,
										ASL_FuncFlags, FILF_PATGAD | FILF_SAVE,
										ASL_Pattern, (ULONG)"~(#?.info)",
										TAG_DONE))) {
									if(AslRequest(FileRequester,NULL)) {
										strcpy(PXLdirname,FileRequester->rf_Dir);
										strcpy(PXLfilename,FileRequester->rf_File);
										strmfp(PXLpathname,PXLdirname,PXLfilename);
									}
									FreeAslRequest(FileRequester);
								}
								if((fp=fopen(PXLpathname,"wb"))) {
									for(row=0; row<BitMap.Rows; row++) {
										num=0;
										lastcolor=16;
										for(col=0; col<BitMap.BytesPerRow; col++)
											for(bit=128; bit; bit>>=1) {
												color=0;
												for(i=0; i<bmhd->nPlanes; i++)
													if(*(BitMap.Planes[i]+BitMap.BytesPerRow*row+col) & bit)
														color|=1<<i;
												if(lastcolor<16 && color!=lastcolor) {
													while(num>15) {
														byte=num/15;
														if(num%15==0)
															byte--;
														if(byte>7)
															byte=7;
														fputc(byte,fp);
														num-=byte*15;
													}
													byte=(num<<4) | (lastcolor & 15);
													fputc(byte,fp);
													num=0;
												}
												lastcolor=color;
												num++;
											}
										if(num>0 && (color & 15)) {
											while(num>15) {
												byte=num/15;
												if(byte>7)
													byte=7;
												fputc(byte,fp);
												num-=byte*15;
											}
											byte=(num<<4) | (color & 15);
											fputc(byte,fp);
										}
										fprintf(fp,"\12");
									}
									fprintf(fp,"\15");
									fclose(fp);
								}
							}
						for(i=0; i<bmhd->nPlanes; i++)
							if(BitMap.Planes[i])
								FreeMem(BitMap.Planes[i],planesize);
					}
					IFFL_CloseIFF(file);
				}
				CloseLibrary(IFFBase);
			}
		CloseLibrary(AslBase);
	}
}
