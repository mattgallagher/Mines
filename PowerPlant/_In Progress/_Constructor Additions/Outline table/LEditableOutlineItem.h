// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LEditableOutlineItem.h		PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LEditableOutlineItem
#define _H_LEditableOutlineItem
#pragma once

#include <LOutlineItem.h>
#include <LPeriodical.h>
#include <UKeyFilters.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LInPlaceEditField;

// ---------------------------------------------------------------------------

class LEditableOutlineItem : public LOutlineItem,
							 public LPeriodical {
public:
							LEditableOutlineItem();
	virtual					~LEditableOutlineItem();

	// in-place editing accessors

	virtual Boolean			CanDoInPlaceEdit( const STableCell& inCell );
	
	virtual void			StartInPlaceEdit( const STableCell& inCell );

	LInPlaceEditField*		GetEditField() const
									{ return mEditField; }

	// in-place editing implementation

protected:
	virtual void			SingleClick(
									const STableCell&		inCell,
									const SMouseDownEvent&	inMouseDown,
									const SOutlineDrawContents&	inDrawContents,
									Boolean					inHitText);
									
	virtual void			SpendTime( const EventRecord& inMacEvent );

	virtual void			ConfigureInPlaceEdit(
									const STableCell&		inCell,
									ResIDT&					outTextTraitsID,
									SInt16&					outMaxChars,
									UInt8&					outAttributes,
									TEKeyFilterFunc&		outKeyFilter);
									
	virtual void			StopInPlaceEdit();

	virtual void			PrepareDrawContents(
									const STableCell&		inCell,
									SOutlineDrawContents&	ioDrawContents);


	// data members

protected:
	LInPlaceEditField*		mEditField;						// the edit field we've created (if any)


	// class variables

protected:
	static UInt32			sInPlaceEditTickCount;			// time to start editing (if single-clicked)
	static Rect				sInPlaceEditTextFrame;			// where editing will take place
	static STableCell		sInPlaceEditCell;				// cell which is being edited

	friend class LOutlineEditFieldWatcher;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
