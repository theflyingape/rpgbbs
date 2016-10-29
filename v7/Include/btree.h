/*****************************\
 *   B Tree Header File V4   *
 *                           *
 *  Created by Robert Hurst  *
 *   on September 6, 1990    *
\*****************************/
#define B_VERSION			4
#define B_MAXLEVELS			12
#define B_MAXKEYS			4
#define B_NODE(sub1,sub2)	(*b->Bnp[sub1]->node[sub2])
#define B_NUMKEYS(sub1)		(*b->Bnp[sub1]->numkeys)
#define B_SUCCESSFUL		0
#define B_CREATE_ERROR		1
#define B_OPEN_ERROR		2
#define B_KEY_NOT_FOUND		3
#define B_STORE_ERROR		4
#define B_UPDATE_ERROR		5
#define B_DELETE_ERROR		6
#define B_STACK_TOO_DEEP	7
#define B_END_OF_FILE		8

struct B {
	FILE *Bfh;							/* File Handle */
	struct Bheader *Bhp;				/* Pointer to B-Tree Header structure */
	struct Bnode *Bnp[3];				/* Pointer to B-Tree Node structure */
	unsigned long filepos[3];			/* Last file cursor position accessed */
	int keypos[3];						/* Last node position accessed (0-4) */
	unsigned char Bsp;					/* Stack pointer */
	unsigned long Bnodes[B_MAXLEVELS+1];/* Stack storage for nodes */
	unsigned char Bkeys[B_MAXLEVELS+1];	/* Stack storage for keys */
	unsigned char Bstatus;				/* I/O status of last operation */
};

struct Bheader {
	char id[4];							/* File identification code ("MWBT") */
	unsigned char version;				/* B-Tree version number */
	unsigned long filesize;				/* Number of bytes in file */
	unsigned long nodesize;				/* Number of bytes in node */
	unsigned short keysize;				/* Number of bytes in key */
	unsigned long nodes;				/* Number of active nodes in file */
	unsigned long keys;					/* Number of active keys in file */
	unsigned long root;					/* Pointer to root node */
	unsigned long free;					/* Pointer to next free node */
};

struct Bnode {
	unsigned long *node[B_MAXKEYS+1];	/* Left node pointers (0-4) */
	unsigned char *numkeys;				/* Number of active keys in this node */
	char *key[B_MAXKEYS];				/* Pointer to key data */
};

/* Prototypes for functions defined in Btree.c */
void __asm Bputheader(register __a0 struct B * );
void __asm Bgetnode(register __a0 struct B * , register __d0 unsigned long , register __d1 unsigned char );
void __asm Bputnode(register __a0 struct B * , register __d0 unsigned long , register __d1 unsigned char );
unsigned long __asm Bfree(register __a0 struct B * );
char * __asm Berror(register __a0 struct B * );
void __asm Bcreate(register __a0 struct B * , register __a1 char * , register __d0 unsigned short );
void __asm Bclose(register __a0 struct B * );
void __asm Bopen(register __a0 struct B * , register __a1 char * );
void __asm Bnext(register __a0 struct B * , register __a1 char * );
void __asm Bget(register __a0 struct B * , register __a1 char * );
void __asm Bprev(register __a0 struct B * , register __a1 char * );
void __asm Bfirst(register __a0 struct B * , register __a1 char * );
void __asm Blast(register __a0 struct B * , register __a1 char * );
void __asm Bstore(register __a0 struct B * , register __a1 char * );
void __asm Bdelete(register __a0 struct B * , register __a1 char * );
void __asm Bupdate(register __a0 struct B * , register __a1 char * );

/* "btree.library"*/
#pragma libcall BtreeBase Bputheader 1e 801
#pragma libcall BtreeBase Bgetnode 24 10803
#pragma libcall BtreeBase Bputnode 2a 10803
#pragma libcall BtreeBase Bfree 30 801
#pragma libcall BtreeBase Berror 36 801
#pragma libcall BtreeBase Bcreate 3c 09803
#pragma libcall BtreeBase Bclose 42 801
#pragma libcall BtreeBase Bopen 48 9802
#pragma libcall BtreeBase Bnext 4e 9802
#pragma libcall BtreeBase Bget 54 9802
#pragma libcall BtreeBase Bprev 5a 9802
#pragma libcall BtreeBase Bfirst 60 9802
#pragma libcall BtreeBase Blast 66 9802
#pragma libcall BtreeBase Bstore 6c 9802
#pragma libcall BtreeBase Bdelete 72 9802
#pragma libcall BtreeBase Bupdate 78 9802
