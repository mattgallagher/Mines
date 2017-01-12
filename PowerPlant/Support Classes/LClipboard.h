// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LClipboard.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LClipboard
#define _H_LClipboard
#pragma once

#include <LAttachment.h>

#include <UScrap.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LClipboard : public LAttachment {
public:
						LClipboard();
	virtual				~LClipboard();

	virtual void		SetData(
								ResType		inDataType,
								Handle		inDataH,
								Boolean		inReset = true);

	virtual void		SetData(
								ResType		inDataType,
								Ptr			inDataPtr,
								SInt32		inDataLength,
								Boolean		inReset = true);

	virtual SInt32		GetData(
								ResType		inDataType,
								Handle		ioData);

protected:
	virtual void		ClearData();

	virtual void		SetDataSelf(
								ResType		inDataType,
								Ptr			inDataPtr,
								SInt32		inDataLength,
								Boolean		inReset);

	virtual SInt32		GetDataSelf(
								ResType		inData,
								Handle		ioDataH);

	virtual void		ImportSelf();
	virtual void		ExportSelf();

	virtual void		ExecuteSelf(
								MessageT	inMessage,
								void*		ioParam);
								
protected:
	UScrap::RefType	mScrapRef;
	bool			mImportPending;
	bool			mExportPending;	

	// -----------------------------------------------------------------------
	//	Static Class Interface
	
public:
	static LClipboard*	GetClipboard()		{ return sClipboard; }

protected:
	static LClipboard*	sClipboard;
	
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
