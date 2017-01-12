// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPrintout.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPrintout
#define _H_LPrintout
#pragma once

#include <LView.h>
#include <UPrinting.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

typedef	UInt32		EPrintAttr;

const EPrintAttr	printAttr_NumberDown	= 0x80000000;

#pragma options align=mac68k

struct SPrintoutInfo {
	SInt16			width;
	SInt16			height;
	Boolean			active;
	Boolean			enabled;
	SInt32			userCon;
	UInt32			attributes;
};

#pragma options align=reset

// ---------------------------------------------------------------------------

class	LPrintout : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('prnt') };
	
	static LPrintout*	CreatePrintout( ResIDT inPrintoutID );

						LPrintout();

						LPrintout( LPrintSpec& inPrintSpec );

						LPrintout( LStream* inStream );

	virtual				~LPrintout();

	LPrintSpec*			GetPrintSpec() const;

	void				SetPrintSpec( LPrintSpec& inPrintSpec );

	virtual void		DoPrintJob();

	virtual void		PrintPanelRange(
								const PanelSpec&	inFirstPanel,
								const PanelSpec&	inLastPanel,
								UInt16				inCopyCount);

	virtual void		CountPanels(
								UInt32&		outHorizPanels,
								UInt32&		outVertPanels);

	virtual void		PrintPanel(
								const PanelSpec&	inPanel,
								RgnHandle			inSuperPrintRgnH);

	virtual GrafPtr		GetMacPort() const;

	virtual Boolean		EstablishPort();

	virtual void		SetForeAndBackColors(
								const RGBColor*		inForeColor,
								const RGBColor*		inBackColor);

	virtual void		GetForeAndBackColors(
								RGBColor*		outForeColor,
								RGBColor*		outBackColor) const;

	virtual void		ApplyForeAndBackColors() const;

	Boolean				HasAttribute( EPrintAttr inAttribute ) const;

	void				SetAttribute( EPrintAttr inAttribute );
	
	void				ClearAttribute( EPrintAttr inAttribute );

protected:
	UInt32			mAttributes;
	LPrintSpec*		mPrintSpec;
	GrafPtr			mPrinterPort;
	WindowPtr		mWindowPort;
	UInt32			mHorizPanelCount;
	UInt32			mVertPanelCount;
	RGBColor		mForeColor;
	RGBColor		mBackColor;

	virtual void		PageToPanel(
								UInt32		inPageNumber,
								PanelSpec&	outPanel) const;

	virtual void		AdjustToPaperSize();

	virtual void		GetPrintJobSpecs(
								PanelSpec&	outFirstPanel,
								PanelSpec&	outLastPanel,
								UInt16&		outCopyCount);

	virtual void		PrintCopiesOfPages(
								const PanelSpec&	inFirstPanel,
								const PanelSpec&	inLastPanel,
								UInt16				inCopyCount);

private:
	void				InitPrintout();
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
