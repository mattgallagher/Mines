// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPicture.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPicture
#define _H_LPicture
#pragma once

#include <LView.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPicture : public LView {
public:
	enum { class_ID = FOUR_CHAR_CODE('pict') };

						LPicture();
						
						LPicture( const LPicture& inOriginal );
						
						LPicture(
								const SPaneInfo		&inPaneInfo,
								const SViewInfo		&inViewInfo,
								ResIDT				inPICTid);
								
						LPicture( LStream *inStream );
								
						LPicture( ResIDT inPictureID );

	virtual				~LPicture();

	ResIDT				GetPictureID() const	{ return mPICTid; }

	void				SetPictureID( ResIDT inPictureID );

protected:
	ResIDT				mPICTid;

	virtual void		DrawSelf();

private:
	void				InitPicture();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
