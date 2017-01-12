// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharableModel.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for shared LModelObjects.

#ifndef _H_LSharableModel
#define _H_LSharableModel
#pragma once

#include <LModelObject.h>
#include <LSharable.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LSharableModel : public LModelObject,
						 virtual public LSharable {
public:
							LSharableModel();

							LSharableModel(
									LModelObject*	inSuperModel,
									DescType		inKind = typeNull);

	virtual					~LSharableModel();

	virtual void			AddUser(
									void*			inUser);

	virtual void			Finalize();

	virtual void			SuperDeleted();
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
