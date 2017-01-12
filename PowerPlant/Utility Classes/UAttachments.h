// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UAttachments.h				PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_UAttachments
#define _H_UAttachments
#pragma once

#include <LAttachment.h>
#include <LCommander.h>
#include <LPane.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LStream;
class	LView;


// ===========================================================================
// ¥ LCommanderPane
// ===========================================================================

class	LCommanderPane : public LPane,
						 public LCommander {
public:
	enum { class_ID = FOUR_CHAR_CODE('cmpn') };

			LCommanderPane(SPaneInfo& inPaneInfo, LCommander* inSuper);
			LCommanderPane(LStream* inStream);

	virtual ~LCommanderPane();
};


// ===========================================================================
// ¥ LBeepAttachment
// ===========================================================================

class	LBeepAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('beep') };

					LBeepAttachment(
							MessageT	inMessage = msg_AnyMessage,
							Boolean		inExecuteHost = true);

					LBeepAttachment( LStream* inStream );

protected:
	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


// ===========================================================================
// ¥ LEraseAttachment
// ===========================================================================

class	LEraseAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('eras') };

					LEraseAttachment( Boolean inExecuteHost = true );
					LEraseAttachment( LStream* inStream );
protected:
	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


// ===========================================================================
// ¥ LColorEraseAttachment
// ===========================================================================

class LColorEraseAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('cers') };

					LColorEraseAttachment(
							const RGBColor*		inBackColor = nil,
							Boolean				inExecuteHost = true);

					LColorEraseAttachment(
							const PenState*		inPenState,
							const RGBColor*		inForeColor = nil,
							const RGBColor*		inBackColor = nil,
							Boolean				inExecuteHost = true);

					LColorEraseAttachment(LStream*	inStream);
protected:
	RGBColor		mForeColor;
	RGBColor		mBackColor;

	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


// ===========================================================================
// ¥ LBorderAttachment
// ===========================================================================

class	LBorderAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('brda') };

					LBorderAttachment(
							const PenState*	inPenState = nil,
							const RGBColor*	inForeColor = nil,
							const RGBColor*	inBackColor = nil,
							Boolean			inExecuteHost = true);

					LBorderAttachment(LStream*	inStream);
protected:
	PenState		mPenState;
	RGBColor		mForeColor;
	RGBColor		mBackColor;

	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


// ===========================================================================
// ¥ LPaintAttachment
// ===========================================================================

class	LPaintAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('pnta') };

					LPaintAttachment(
							const PenState*	inPenState = nil,
							const RGBColor*	inForeColor = nil,
							const RGBColor*	inBackColor = nil,
							Boolean			inExecuteHost = true);

					LPaintAttachment(LStream*	inStream);
protected:
	PenState		mPenState;
	RGBColor		mForeColor;
	RGBColor		mBackColor;

	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


// ===========================================================================
// ¥ LCmdEnablerAttachment
// ===========================================================================

class	LCmdEnablerAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('cena') };

					LCmdEnablerAttachment(CommandT	inCmdToEnable);
					LCmdEnablerAttachment(LStream*	inStream);
protected:
	CommandT		mCmdToEnable;

	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


// ===========================================================================
// ¥ LKeyScrollAttachment
// ===========================================================================

class	LKeyScrollAttachment : public LAttachment {
public:
	enum { class_ID = FOUR_CHAR_CODE('ksca') };

					LKeyScrollAttachment(LView*		inViewToScroll);
					LKeyScrollAttachment(LStream*	inStream);
protected:
	LView*			mViewToScroll;

	virtual void	ExecuteSelf(MessageT inMessage, void* ioParam);
};


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
