// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LCaption.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Pane with a block of text

#ifndef _H_LCaption
#define _H_LCaption
#pragma once

#include <LPane.h>
#include <LString.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LCaption : public LPane {
public:
	enum { class_ID = FOUR_CHAR_CODE('capt') };

						LCaption();

						LCaption( const LCaption& inCaption );

						LCaption(
								const SPaneInfo&	inPaneInfo,
								ConstStringPtr		inString,
								ResIDT				inTextTraitsID);

						LCaption( LStream* inStream );

	virtual				~LCaption();

	virtual SInt32		GetValue() const;

	virtual void		SetValue( SInt32 inValue );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	ResIDT				GetTextTraitsID() const		{ return mTxtrID; }

	void				SetTextTraitsID( ResIDT inTxtrID )
							{
								mTxtrID = inTxtrID;
							}

protected:
	LStr255				mText;
	ResIDT				mTxtrID;

	virtual void		DrawSelf();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
