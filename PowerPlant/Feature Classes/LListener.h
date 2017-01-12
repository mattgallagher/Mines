// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LListener.h					PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	An abstract mix-in class that works with LBroadcaster class to implement
//	dependencies. A Listener receives messages from its Broadcasters.

#ifndef _H_LListener
#define _H_LListener
#pragma once

#include <TArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LListener {
	friend class LBroadcaster;
public:
					LListener();
					
					LListener( const LListener& inOriginal );
							
	virtual			~LListener();

	bool			HasBroadcaster( LBroadcaster* inBroadcaster );

	void			StartListening()		{ mIsListening = true; }

	void			StopListening()			{ mIsListening = false; }

	Boolean			IsListening() const		{ return mIsListening; }

						// Pure Virtual. Concrete subclasses must override
	virtual void	ListenToMessage(
							MessageT		inMessage,
							void*			ioParam) = 0;

protected:
	TArray<LBroadcaster*>	mBroadcasters;
	Boolean					mIsListening;

	void			AddBroadcaster( LBroadcaster* inBroadcaster );

	void			RemoveBroadcaster( LBroadcaster* inBroadcaster );
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
