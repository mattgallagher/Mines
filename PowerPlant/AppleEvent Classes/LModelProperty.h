// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LModelProperty.h			PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for a Property defined by the Apple Event Object Model.

#ifndef _H_LModelProperty
#define _H_LModelProperty
#pragma once

#include <LModelObject.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LModelProperty : public LModelObject {
public:
					LModelProperty(
							DescType			inPropertyID,
							LModelObject*		inSuperModel,
							Boolean				inBeLazy = true);

	virtual			~LModelProperty();

	virtual void	HandleAppleEvent(
							const AppleEvent&	inAppleEvent,
							AppleEvent&			outAEReply,
							AEDesc&				outResult,
							SInt32				inAENumber);

	void			SendSetDataAE(
							DescType			inDataType,
							Ptr					inDataPtr,
							Size				inDataSize,
							Boolean				inExecute = true);

	void			SendSetDataAEDesc(
							const AEDesc&		inDesc,
							Boolean				inExecute = true);

	virtual Boolean	CompareToDescriptor(
							DescType			inComparisonOperator,
							const AEDesc&		inCompareDesc) const;

protected:
	DescType		mPropertyID;

	virtual void	MakeSelfSpecifier(
							AEDesc&				inSuperSpecifier,
							AEDesc&				outSelfSpecifier) const;

	virtual void	HandleGetData(
							const AppleEvent&	inAppleEvent,
							AEDesc&				outResult,
							SInt32				inAENumber);

	virtual void	HandleSetData(
							const AppleEvent&	inAppleEvent,
							AppleEvent&			outAEReply);
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
