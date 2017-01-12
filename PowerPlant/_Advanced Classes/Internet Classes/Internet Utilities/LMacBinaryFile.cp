// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LMacBinaryFile.cp			PowerPlant 2.2.2	©1998-2005 Metrowerks Inc.
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

// To do: Interface to Open should be updated to handle FSSpec.
//		  Alternate constructor that accetps FSSpec should be created.
//		  GetFileSize presently only returns DF size... need to add RF size

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LMacBinaryFile.h>
#include <UInternet.h>
#include <cstring>

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

#define BLOCKS(x)	((x+127)/128)

#define Length(c)	c[0]


LMacBinaryFile::LMacBinaryFile()
{
	InitMembers();
}

LMacBinaryFile::LMacBinaryFile(char *file,
			short vrefnum,
			long dirID,
			short mode)
{
	InitMembers();
	Open(file, vrefnum, dirID, mode);
}

LMacBinaryFile::~LMacBinaryFile()
{
	Close();
}

void
LMacBinaryFile::InitMembers()
{
	mMBFile = nil;
}

void
LMacBinaryFile::Open(char *file,
			short vrefnum,
			long dirID,
			short mode)
{
	OSErr err;
	int i;

	//Close previously opened file if necessary
	if (mMBFile) {
		Close();
	}

	mMBFile = (MBFile*) ::NewPtrClear(sizeof(MBFile));
	ThrowIfMemFail_(mMBFile);

	::BlockMoveData(file, mMBFile->name, 32);
	if (PP_CSTD::strlen(file) > 31)
		mMBFile->name[31] = 0;

	LString::CToPStr((char*) mMBFile->name);

	if (mMBFile->name[1] == '.') mMBFile->name[1] = '_'; //replace leading period with slash

	for (i = 1; i <= mMBFile->name[0]; i++)		// replace embedded slashes, colons, and CRs
		if (mMBFile->name[i] == '/'
				|| mMBFile->name[i] == ':'
				|| mMBFile->name[i] == CR)
			mMBFile->name[i] = ' ';

	mMBFile->vrefnum = vrefnum;
	mMBFile->dirID = dirID;
	mMBFile->mode = mode;

	if ((err = ::HOpenDF( vrefnum, dirID, mMBFile->name, fsRdWrPerm, &mMBFile->fd)) != noErr) {
		if ((err==-43) && (mode & MB_WRITE)) {
			err = ::HCreate( vrefnum, dirID, mMBFile->name, FOUR_CHAR_CODE('\?\?\?\?'),
						FOUR_CHAR_CODE('BINA'));
			if (mode & MB_ISASCII)	{
				HFileParam blah;
				MakeTextFile(vrefnum, dirID, mMBFile->name, &blah);
				}

			ThrowIfOSErr_ (::HOpenDF( vrefnum, dirID, mMBFile->name, fsRdWrPerm, &mMBFile->fd));
			}
		else
			ThrowIfOSErr_ (err);
		}

	if (mode & MB_APPEND)
		SetFPos(mMBFile->fd,fsFromLEOF,0);

	mMBFile->binary=0;
	mMBFile->sector1=1;
	mMBFile->fork=0;
}


void
LMacBinaryFile::Close()
{
	HFileParam finfo;
	long fpos;
	
	if (mMBFile == NULL) {
		return;
	}
	
	if (mMBFile->fd) {

		if (!(mMBFile->mode & MB_DISABLE) && (mMBFile->mode & MB_WRITE)) {
		
			if (mMBFile->fork) {
				::SetEOF( mMBFile->fd, (long) mMBFile->rlen);
			} else {
				::SetEOF( mMBFile->fd, (long) mMBFile->dlen);
			}

			::FSClose( mMBFile->fd);

			GetFileInfo( mMBFile->vrefnum, mMBFile->dirID, mMBFile->name, &finfo);
			::BlockMoveData(&mMBFile->header.type[0], &finfo.ioFlFndrInfo, sizeof(FInfo));
			::BlockMoveData(&mMBFile->header.cdate[0], &finfo.ioFlCrDat, 4);
			::BlockMoveData(&mMBFile->header.mdate[0], &finfo.ioFlMdDat, 4);
			finfo.ioFlFndrInfo.fdFlags &= 0xfeff;
			finfo.ioFlFndrInfo.fdFlags |= (UInt16) (mMBFile->header.flags2 & 0x00FF);
			finfo.ioFlRLgLen=mMBFile->rlen;
			finfo.ioFlLgLen =mMBFile->dlen;

			SetFileInfo( mMBFile->vrefnum, mMBFile->dirID, mMBFile->name, &finfo);
			
		} else if (mMBFile->mode & MB_WRITE) {
			GetFPos( mMBFile->fd, &fpos);
			::SetEOF(  mMBFile->fd,  fpos);
			::FSClose( mMBFile->fd);
			
		} else {
			::FSClose( mMBFile->fd);
		}
	}

	::DisposePtr((Ptr)mMBFile);
	mMBFile = NULL;
}


long
LMacBinaryFile::Read(
	void *buffer,
	long size)
{
	char			*p;
	long			rsize=size;
	unsigned short	crc;

	ThrowIfNULL_(mMBFile);

	if (mMBFile->fork<0) {
		return(-1);
		}

	p = (char*)buffer;

	if (mMBFile->sector1) {
		HFileParam finfo;

//		setmem( &mMBFile->header, sizeof(MBHead), 0);
		ZeroPointer((Ptr)&mMBFile->header, sizeof(MBHead));
		::BlockMoveData(mMBFile->name, &mMBFile->header.nlen,
				(Length(mMBFile->name) > 31) ? 32 : (Length(mMBFile->name)+1));
		GetFileInfo( mMBFile->vrefnum, mMBFile->dirID, mMBFile->name, &finfo);
		::BlockMoveData(&finfo.ioFlFndrInfo, &mMBFile->header.type[0], sizeof(FInfo) );
		mMBFile->header.flags2 = (char) (finfo.ioFlFndrInfo.fdFlags & 0x00FF);
		mMBFile->header.isProtected = (char) ((mMBFile->header.zero2 & 0x40)?1:0);
		mMBFile->header.zero2 = 0;
		::BlockMoveData(&finfo.ioFlLgLen, &mMBFile->header.dflen[0], 4);
		::BlockMoveData(&finfo.ioFlRLgLen, &mMBFile->header.rflen[0], 4);
		::BlockMoveData(&finfo.ioFlCrDat, &mMBFile->header.cdate[0], 4);
		::BlockMoveData(&finfo.ioFlMdDat, &mMBFile->header.mdate[0], 4);
		mMBFile->header.mb2versnum = (char) 129;
		mMBFile->header.mb2minvers = (char) 129;
		crc = CalculateCRC((unsigned char *) &(mMBFile->header), 124, 0);
		::BlockMoveData(&crc, &(mMBFile->header.crc), 2);

		mMBFile->dlen=finfo.ioFlLgLen;
		mMBFile->rlen=finfo.ioFlRLgLen;

		if (! (mMBFile->mode & MB_DISABLE) ) {
			if (size<128) return(-1);

			::BlockMoveData(&mMBFile->header, p, 128);
			p +=128;
			size -= 128;
			mMBFile->bytes= BLOCKS(mMBFile->dlen)*128;
			mMBFile->binary=1;
			}
		else {
			mMBFile->bytes = mMBFile->dlen;
			mMBFile->rlen=0;
			mMBFile->binary=0;
			}
		mMBFile->sector1=0;
	}

	if (size >0) {
		long length = size;
		OSErr err;

		err = ::FSRead( mMBFile->fd, &length, p);

		size -= length;
		mMBFile->bytes -=length;
		p += length;

		if (err == -39 || (mMBFile->bytes<=0) ) {
			::FSClose( mMBFile->fd );
			if (mMBFile->bytes<0L) mMBFile->bytes=0L;
//			setmem(p, mMBFile->bytes, 0);			//	Make filler bytes zero
			ZeroPointer(p, size);
			size -= mMBFile->bytes;
			p    +=      mMBFile->bytes;				/* Make adjustments for necessary 128 byte term */
			if (!mMBFile->fork ) {
				mMBFile->fork=1;
				mMBFile->bytes= BLOCKS(mMBFile->rlen)*128;
				if (mMBFile->bytes) {
					::HOpenRF( mMBFile->vrefnum,
							mMBFile->dirID,
							mMBFile->name,
							fsRdWrPerm,
							&mMBFile->fd);
#ifdef READ	//¥ÊPFV ?????
					length=(long)size;
					if (length >0L) {
						err = ::FSRead( mMBFile->fd, &length, p);
						size -= length;
						mMBFile->bytes -=length;
						}
#endif // READ
					}
				else {
					mMBFile->fd=0;
					mMBFile->fork=-1;					/* Time to close up shop */
					}
				}
			else {
				mMBFile->fd=0;
				mMBFile->fork=-1;					/* Time to close up shop */
				}
			}
		}
	return( rsize-size);
}


long
LMacBinaryFile::Write(
	void *buffer,	/* BYU LSC */
	long size
  )
{
	long	rsize;

	ThrowIfNULL_(mMBFile);

	if (size < 1)
		return(0);

	rsize=size;

	if (mMBFile->sector1 && (size >= sizeof(struct MBHead)) && (!(mMBFile->mode & MB_DISABLE)))  {
		if (isMacBinary((MBHead *) buffer)) {			// WARNING: isMacBinary modifies data
			ProcessMBHead((MBHead *) buffer);
			buffer = (void*)((char*)(buffer) + 128);
			if ((size-=128) <1)
				return(rsize);
			}
		}

	if (bwrite((char*)buffer, size))
		return(-1);
	else
		return( rsize);
}

void
LMacBinaryFile::ZeroPointer(Ptr pointer,long size)
{
	while (size--) *pointer++ = 0;
}

unsigned short
LMacBinaryFile::CalculateCRC(unsigned char *ptr, short count, unsigned short crc)
{
	unsigned short	i;

	crc = 0;
	while (count -- > 0) {
		crc = (unsigned short) (crc ^ (unsigned short)*ptr++ << 8);
		for (i = 0; i < 8; i++)
			if (crc & 0x8000)
				crc = (unsigned short) (crc << 1 ^ 0x1021);
			else
				crc = (unsigned short) (crc << 1);
	}

	return crc;
}

void
LMacBinaryFile::GetFileInfo(short volume, long dirID, StringPtr name, HFileParam *iop)
{
	OSErr err;
	iop->ioNamePtr = name;
	iop->ioVRefNum = volume;
	iop->ioDirID = dirID;
	iop->ioFVersNum = 0;
	iop->ioFDirIndex = 0;
	err = PBHGetFInfo((HParmBlkPtr)iop, false);
}

void
LMacBinaryFile::SetFileInfo(short volume, long dirID, StringPtr name, HFileParam *iop)
{
	OSErr err;
	iop->ioNamePtr = name;
	iop->ioVRefNum = volume;
	iop->ioDirID = dirID;
	iop->ioFVersNum = 0;
	iop->ioFDirIndex = 0;
	err = PBHSetFInfo((HParmBlkPtr)iop, false);
}

void
LMacBinaryFile::MakeTextFile(short volume, long dirID, StringPtr name, HFileParam *iop)
{
	GetFileInfo(volume, dirID, name, iop);
	iop->ioFlFndrInfo.fdType = FOUR_CHAR_CODE('TEXT');
	iop->ioFlFndrInfo.fdCreator = FOUR_CHAR_CODE('ttxt');
	SetFileInfo(volume, dirID, name, iop);
}

short
LMacBinaryFile::isMacBinary(MBHead *p)
{
	unsigned short	crc;

	if ((p->nlen > 0)   &&	(p->nlen < 65)  &&
		(p->zero1 == 0) &&	(p->zero2 == 0) && (p->zero3 == 0))	{	// Assume MB I
			crc = CalculateCRC((unsigned char *)p, 124, 0);
			if (((p->crc[0] << 8) + p->crc[1] == crc) && (p->mb2versnum > 128))	{		// Check for MB II
				if (p->mb2minvers > 129)
					return(0);			// If vers is greater than 129, leave it alone

				return (1);				// Valid MB II file.
			}
			else {
				p->flags2 = 0;			// So we can use same routines for MB I & II
				return (1);				// Valid MB I file (we make it a II file on the fly.)
				}
		}

	return(0);							// Not a Macbinary file
}

long
LMacBinaryFile::GetFileSize()
{
	long size = 0;
	OSErr ret;

	ret = ::GetEOF( mMBFile->fd, &size );			/* length of file data fork */
	if (ret != noErr)
		size = 0;

	return(size);
}

OSErr
LMacBinaryFile::bwrite(char *buffer, long size)
{
	long len = size;
	OSErr error = noErr;

	if (mMBFile->binary) {
		if (mMBFile->bytes > 0) {
			if (mMBFile->bytes < len) len = mMBFile->bytes;
			error= ::FSWrite( mMBFile->fd, &len, buffer);
			mMBFile->bytes -= len;
			buffer +=len;
			size -= len;
			}
		if (mMBFile->bytes <= 0) {
			if (!mMBFile->fork) {
				mMBFile->fork = 1;
				mMBFile->bytes = BLOCKS(mMBFile->rlen)*128;
				::SetEOF( mMBFile->fd, (long)mMBFile->dlen);
				::FSClose( mMBFile->fd);
				if (mMBFile->bytes) {
					::HOpenRF( mMBFile->vrefnum,
								mMBFile->dirID,
								mMBFile->name,
								fsRdWrPerm,
								&mMBFile->fd);
					if (size) {
						len = (long)size;
						error= ::FSWrite( mMBFile->fd, &len, buffer);
						}
					}
				else
					mMBFile->fd = 0;
				}
			else ::SetEOF( mMBFile->fd, (long)mMBFile->rlen);
			}
		}
	else {
		error = ::FSWrite( mMBFile->fd, &len, buffer);
		}
	return (error);
}

void
LMacBinaryFile::ProcessMBHead (MBHead *header)
{
	OSErr	err;

	::BlockMoveData(header, &mMBFile->header, sizeof(MBHead));
	mMBFile->binary = 1;
	::BlockMoveData(&header->dflen[0], &mMBFile->dlen, 4);
	::BlockMoveData(&header->rflen[0], &mMBFile->rlen, 4);
	mMBFile->bytes = BLOCKS(mMBFile->dlen)*128;
	mMBFile->fork = 0;
	mMBFile->sector1 = 0;

	::FSClose(mMBFile->fd);
	ThrowIfOSErr_ (HDelete( mMBFile->vrefnum, mMBFile->dirID, mMBFile->name));			/* Error deleting Old File */

	::BlockMoveData(&mMBFile->header.nlen, mMBFile->name, 32);

	if (mMBFile->bytes) {
		if ((err = ::HOpenDF( mMBFile->vrefnum,
						mMBFile->dirID,
						mMBFile->name,
						fsRdWrPerm,
						&mMBFile->fd)) != noErr) {
			if (err == -43) {
				unsigned long	cre,typ;

				::BlockMoveData(mMBFile->header.type, &typ,  4);
				::BlockMoveData(mMBFile->header.creator, &cre, 4);

				HCreate( mMBFile->vrefnum, mMBFile->dirID, mMBFile->name, cre, typ);
				if (::HOpenDF(mMBFile->vrefnum,
								mMBFile->dirID,
								mMBFile->name,
								fsRdWrPerm,
								&mMBFile->fd))
					return;
				}
			else {
				return;
				}
			}
		}
	else {
		if ((err=::HOpenRF( mMBFile->vrefnum,
								mMBFile->dirID,
								mMBFile->name,
								fsRdWrPerm,
								&mMBFile->fd)) != noErr) {
			if (err == -43) {
				unsigned long	cre,typ;

				::BlockMoveData(mMBFile->header.type, &typ, 4);
				::BlockMoveData(mMBFile->header.creator, &cre, 4);

				HCreate( mMBFile->vrefnum, mMBFile->dirID, mMBFile->name, cre, typ);
				if (::HOpenRF( mMBFile->vrefnum,
								mMBFile->dirID,
								mMBFile->name,
								fsRdWrPerm,
								&mMBFile->fd))
					return;
				}
			else {
				return;
				}
			}
		mMBFile->fork = 1;
		mMBFile->bytes=BLOCKS(mMBFile->rlen)*128;
		}
}

PP_End_Namespace_PowerPlant
