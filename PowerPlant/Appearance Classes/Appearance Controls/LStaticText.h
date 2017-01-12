// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LStaticText.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LStaticText
#define _H_LStaticText
#pragma once

#include <LControlPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LString;

// ---------------------------------------------------------------------------

class	LStaticText : public LControlPane {
public:
	enum { class_ID		= FOUR_CHAR_CODE('stxt'),
		   imp_class_ID	= FOUR_CHAR_CODE('istx') };

						LStaticText(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LStaticText(
								const SPaneInfo&	inPaneInfo,
								ConstStringPtr		inTitle,
								ResIDT				inTextTraitsID = 0,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LStaticText();

	virtual void		SetDescriptor( ConstStringPtr inDescriptor );

	virtual StringPtr	GetDescriptor( Str255 outDescriptor ) const;

	virtual void		SetValue( SInt32 inValue );

	virtual SInt32		GetValue() const;

	void				SetText(
								Ptr				inTextPtr,
								Size			inTextLength);

	void				SetText( const LString& inString );

	void				GetText(
								Ptr				outBuffer,
								Size			inBufferLength,
								Size*			outTextLength) const;

	void				GetText( LString& outString ) const;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
