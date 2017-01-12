// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UPrinting.h					PowerPlant 2.2.2	©1999-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UPrinting
#define _H_UPrinting
#pragma once

#include <UMemoryMgr.h>

#include <PMApplication.h>

#if !TARGET_RT_MAC_MACHO
	#include <Printing.h>
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------
#pragma mark LPrintSpec

class	LPrintSpec {
public:
					LPrintSpec();
					
					~LPrintSpec();
	
	void			BeginSession( PMPrintSession inSession );

	void			EndSession();
	
	bool			IsInSession() const;
	
	OSStatus		GetError() const;

	Handle			GetPrintRecord() const;

	void			SetPrintRecord( Handle inPrintRecordH );

	void			GetPrintJobParams(
							UInt32&			outFirstPage,
							UInt32&			outLastPage,
							UInt32&			outNumCopies);

	void			SetPrintJobParams(
							UInt32			inFirstPage,
							UInt32			inLastPage,
							UInt32			inNumCopies);

	void			GetPaperRect( Rect& outPaperRect );

	void			GetPageRect( Rect& outPageRect );
	
#if PP_Target_Classic

	THPrint			GetTHPrint() const;

#endif

#if PP_Target_Carbon
	PMPrintSession	GetPrintSession() const;

	PMPageFormat	GetPageFormat() const;

	Handle			GetFlatPageFormat() const;

	void			SetFlatPageFormat( Handle inFlatPageFormat );

	PMPrintSettings	GetPrintSettings() const;

	Handle			GetFlatPrintSettings() const;

	void			SetFlatPrintSettings( Handle inFlatPrintSettings );
#endif

private:
	StHandleBlock		mPrintRecordH;			// Classic PrintRecord
	PMPrintSession		mPrintSession;

#if PP_Target_Carbon
	PMPageFormat		mPageFormat;
	PMPrintSettings		mPrintSettings;

	#if PM_USE_SESSION_APIS
		bool				mUsePrintRecord;
	#else
		StHandleBlock		mFlatPageFormat;
		StHandleBlock		mFlatPrintSettings;
	#endif
#endif
};


// ---------------------------------------------------------------------------
#pragma mark StPrintContext

class	StPrintContext {
public:
					StPrintContext( const LPrintSpec& inPrintSpec );

					~StPrintContext();

	GrafPtr			GetGrafPtr() const;

	void			BeginPage();

	void			EndPage();

private:
	#if PP_Target_Carbon
	
		#if PM_USE_SESSION_APIS
		
			const LPrintSpec&	mPrintSpec;
			
		#else
		
			PMPrintContext	mPrintContext;
			
		#endif
		
	#else
	
		TPPrPort		mPrinterPort;
	
	#endif
};


// ---------------------------------------------------------------------------
#pragma mark StPrintSession

class	StPrintSession {
public:
					StPrintSession( LPrintSpec& ioPrintSpec );

					~StPrintSession();

private:
	LPrintSpec&		mPrintSpec;
};


// ---------------------------------------------------------------------------
#pragma mark UPrinting

class	UPrinting {
public:
	static bool		AskPageSetup( LPrintSpec& ioPrintSpec );

	static bool		AskPrintJob( LPrintSpec& ioPrintSpec );

	static LPrintSpec&	GetAppPrintSpec()		{ return sAppPrintSpec; }
	
	static void		PMRectToQDRect(
							const PMRect&	inPMRect,
							Rect&			outQDRect);

	static const SInt16		max_Pages = 9999;

private:
	static LPrintSpec	sAppPrintSpec;
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
