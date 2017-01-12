// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPopupGroupBox.h			PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPopupGroupBox
#define _H_LPopupGroupBox
#pragma once

#include <LControlView.h>
#include <LMenuController.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#ifndef PP_Uses_Carbon_Events
	#define	PP_Uses_Carbon_Events		0
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPopupGroupBox : public LControlView,
						 public LMenuController {
public:
	enum { class_ID		= FOUR_CHAR_CODE('pgbx'),
		   imp_class_ID	= FOUR_CHAR_CODE('ipgb') };

						LPopupGroupBox(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LPopupGroupBox(
								const SPaneInfo&	inPaneInfo,
								const SViewInfo&	inViewInfo,
								MessageT			inValueMessage = 0,
								SInt16				inTitleOptions = 0,
								ResIDT				inMENUid = MENU_Unspecified,
								SInt16				inTitleWidth = -1,
								Boolean				inPrimary = true,
								ResIDT				inTextTraitsID = 0,
								ConstStringPtr		inTitle = Str_Empty,
								SInt16				inInitialMenuItem = 1,
								ClassIDT			inImpID = imp_class_ID);

	virtual				~LPopupGroupBox();

	virtual void		SetMacMenuH(
								MenuHandle		inMenuH,
								bool			inOwnsMenu = true);

	virtual void		SetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inDataSize,
								void*			inDataPtr);

	virtual void		GetDataTag(
								SInt16			inPartCode,
								FourCharCode	inTag,
								Size			inBufferSize,
								void*			inBuffer,
								Size*			outDataSize = nil) const;

private:
	void				InitPopupGroupBox(
								ResIDT			inMenuID,
								SInt16			inInitialMenuItem);

public:
									// This tag is for internal use
	static const FourCharCode	dataTag_MenuHandle	= FOUR_CHAR_CODE('MHAN');
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
