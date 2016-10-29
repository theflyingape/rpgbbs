#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/diskfont.h>
#include <proto/dtlib.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <ctype.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define	BBS		1
#define	DOOR	2
typedef	ULONG	JDATE;

#include "btree.h"
#include "rpgbbs.h"
#include "server.h"
#ifndef RPGBBS_DELUXE_SERVER
#include "client.h"
#endif
