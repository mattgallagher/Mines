// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LBroadcaster.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	A mix-in class that works with LListener class to implement
//	dependencies. A Broadcaster sends messages to its Listeners.

#ifndef _H_LBroadcaster
#define _H_LBroadcaster
#pragma once

#include <TArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class	LListener;

// ---------------------------------------------------------------------------

class	LBroadcaster {
public:
				LBroadcaster();

				LBroadcaster( const LBroadcaster& inOriginal );

	virtual		~LBroadcaster();

	void		AddListener( LListener* inListener );

	void		RemoveListener( LListener* inListener );

	bool		HasListener( LListener* inListener );

	void		StartBroadcasting()				{ mIsBroadcasting = true; }

	void		StopBroadcasting()				{ mIsBroadcasting = false; }

	bool		IsBroadcasting() const			{ return mIsBroadcasting; }

	void		BroadcastMessage(
						MessageT		inMessage,
						void*			ioParam = nil);

protected:
	TArray<LListener*>	mListeners;
	bool				mIsBroadcasting;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
