// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	MacBinaryFile.h				PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
// ===========================================================================
//	This object based (heavily) on the binsub routines found in NCSA Telnet
//		for the Macintosh.

	/****************************************************************
	*	NCSA Telnet for the Macintosh								*
	*																*
	*	National Center for Supercomputing Applications				*
	*	Software Development Group									*
	*	152 Computing Applications Building							*
	*	605 E. Springfield Ave.										*
	*	Champaign, IL  61820										*
	*																*
	*	Copyright (c) 1986-1993,									*
	*	Board of Trustees of the University of Illinois				*
	*****************************************************************
	*	MacBinary Subroutines.
	*/

#ifndef _H_LMacBinaryFile
#define _H_LMacBinaryFile
#pragma once

#include <PP_Prefix.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

const short	MB_READ		= 0;
const short	MB_WRITE	= 1;
const short	MB_DISABLE	= 0x80;
const short	MB_ISASCII	= 0x40;
const short	MB_APPEND	= 0x20;

typedef struct MBHead {		/* MacBinary Header */
	char zero1;				// 000	Old Version Number - Must be zero
	char nlen;				// 001	Filename - Str63
	char name[63];			// 002
	char type[4];			// 065	Filetype
	char creator[4];		// 069	File Creator Type
	char flags;				// 073	Old Finder Flags
	char zero2;				// 074	Must be zero
	char location[6];		// 075	Location information from Finder info
	char isProtected;		// 081	Low order bit is "protected" bit
	char zero3;				// 082	Must be zero
	char dflen[4];			// 083	Data Fork Length (bytes)
	char rflen[4];			// 087	Resource Fork Length (bytes)
	char cdate[4];			// 091	Creation Date
	char mdate[4];			// 095	Modification Date
	char ginfolen[2];		// 099	MB 1, but unused by Telnet			JMB 2.6
	char flags2;			// 101	Finder Flags, Low Byte				JMB 2.6
	char undefined[14];		// 102	Currently Undefined (9/93)			JMB 2.6
	char unpckdlen[4];		// 116	Unused by Telnet					JMB 2.6
	char sechdlen[2];		// 120	Unused by Telnet					JMB 2.6
	unsigned char mb2versnum;		// 122	Uploading Program's version (=129)	JMB 2.6
	unsigned char mb2minvers;		// 123	Min version needed (=129)			JMB 2.6
	unsigned char crc[2];			// 124	CRC of first 124 bytes				JMB 2.6
	char undefined2[2];		// 126	Currently Undefined (9/93)			JMB 2.6
} MBHead;

typedef struct MBFile {
	Str255 name;			/* The filename */
	short
		fd,					/* Current File Descriptor */
		mode,				/* Are we reading or writing */
		vrefnum,			/* The volume reference number */
		binary,				/* MacBinary active ? */
		sector1,			/* Are we at the first sector of the file */
		fork;				/* 0 if we're writing the DF, 1 if were writing the RF */
	long
		bytes,				/* Length of the current fork */
		rlen,				/* Length of Resource Fork (bytes) */
		dlen,				/* Length of Data Fork (bytes) */
		dirID;				/* The file's dirID */
	MBHead
		header;				/* the first sector... */
} MBFile;

// ---------------------------------------------------------------------------

class LMacBinaryFile {
public:
							LMacBinaryFile();
							LMacBinaryFile(char *file, short vrefnum, long dirID, short mode);
							~LMacBinaryFile();

	void					Open(char *file, short vrefnum, long dirID, short mode);
	void					Close();

	long					Read(
								void *buffer,
								long size);

	long					Write(
								void *buffer,
								long size);

	long					GetFileSize();

protected:
	void					InitMembers();
	void					ZeroPointer(Ptr pointer,long size);
	unsigned short			CalculateCRC(unsigned char *ptr, short count, unsigned short crc);
	void					SetFileInfo(short volume, long dirID, StringPtr name, HFileParam *iop);
	void					MakeTextFile(short volume, long dirID, StringPtr name, HFileParam *iop);
	short					isMacBinary(MBHead *p);
	OSErr					bwrite(char *buffer, long size);
	void					ProcessMBHead (MBHead *header);
	void					GetFileInfo(short volume, long dirID, StringPtr name, HFileParam *iop);

	MBFile *				mMBFile;
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
