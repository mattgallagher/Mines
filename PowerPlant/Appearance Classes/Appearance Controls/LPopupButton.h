// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPopupButton.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LPopupButton
#define _H_LPopupButton
#pragma once

#include <LControlPane.h>
#include <LMenuController.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#ifndef PP_Uses_Carbon_Events
	#define	PP_Uses_Carbon_Events		0
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LPopupButton : public LControlPane,
					   public LMenuController {
public:
	enum { class_ID		= FOUR_CHAR_CODE('popb'),
		   imp_class_ID	= FOUR_CHAR_CODE('ipop') };

						LPopupButton(
								LStream*		inStream,
								ClassIDT		inImpID = imp_class_ID);

						LPopupButton(
								const SPaneInfo& inPaneInfo,
								MessageT		inValueMessage = 0,
								SInt16			inTitleOptions = 0,
								ResIDT			inMENUid = MENU_Unspecified,
								SInt16			inTitleWidth = -1,
								SInt16			inKind = kControlPopupButtonProc,
								ResIDT			inTextTraitsID = 0,
								ConstStringPtr	inTitle = Str_Empty,
								OSType			inResTypeMENU = 0,
								SInt16			inInitialMenuItem = 1,
								ClassIDT		inImpID = imp_class_ID);

	virtual				~LPopupButton();

#if PP_Uses_Carbon_Events
	virtual void		SetMacMenuH(
								MenuHandle		inMenuH,
								bool			inOwnsMenu = true);
#endif

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
	void				InitPopupButton(
								ResIDT			inMenuID,
								ResType			inMenuResType,
								SInt16			inInitialMenuItem);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
