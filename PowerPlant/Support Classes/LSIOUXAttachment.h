// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSIOUXAttachment.h			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================

#ifndef _H_LSIOUXAttachment
#define _H_LSIOUXAttachment
#pragma once

#include <LAttachment.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LSIOUXAttachment : public LAttachment {
public:
						LSIOUXAttachment();
protected:
	virtual void		ExecuteSelf(
								MessageT	inMessage,
								void*		ioParam);
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
