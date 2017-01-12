// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UCallbackUtils.h			PowerPlant 2.2.2	©1996-2005 Metrowerks Inc.
// ===========================================================================
//	A collection of utilities for low-level Toolbox callbacks.

#ifndef _H_UCallbackUtils
#define _H_UCallbackUtils
#pragma once

#include <PP_Types.h>
#include <LowMem.h>

#if !TARGET_RT_MAC_CFM && !__A5__ && !TARGET_RT_MAC_MACHO
	#include <A4Stuff.h>
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class StSetupGlobals;


// ===========================================================================
//		¥ LGlobalsContext
// ===========================================================================
//	A mix-in for any class which needs to save the A5 or A4 (global
//	variables) context for use in a Toolbox callback. Defined as a
//	null class for PowerPC or 68K CFM builds. Designed to be used in
//	concert with StSetupGlobals.
//
//	Constructor saves global variables context. Assumes that the
//	global variables register (either A5 or A4) is correct at this time.
//	(On PowerPC or 68K CFM, it does nothing.)

class LGlobalsContext {

public:
					LGlobalsContext()
						{
							#if !TARGET_RT_MAC_CFM && !TARGET_RT_MAC_MACHO
								#if __A5__
									mSavedGlobals = reinterpret_cast<long>(LMGetCurrentA5());
								#else
									mSavedGlobals = GetCurrentA4();
								#endif
							#endif
						}

					~LGlobalsContext()
						{	// inlined to save code space
						}

	SInt32			GetGlobals()
						{
							#if !TARGET_RT_MAC_CFM && !TARGET_RT_MAC_MACHO
								return mSavedGlobals;
							#else
								return 0;
							#endif
						}

#if !TARGET_RT_MAC_CFM && !TARGET_RT_MAC_MACHO
private:
	SInt32			mSavedGlobals;				// saved copy of application's A5
												// (or A4 if a code resource)
#endif

	friend class StSetupGlobals;
};


// ===========================================================================
//		¥ StSetupGlobals
// ===========================================================================
//	Stack-based class for setting up and restoring A5 or A4 from
//	an LGlobalsContext object. This class is suitable for use in an
//	interrupt service routine where the Toolbox does not restore the
//	globals register for you.

class StSetupGlobals {

#if !TARGET_RT_MAC_CFM && !TARGET_RT_MAC_MACHO

	// -----------------------------------------------------------------------
	// 		¥ 68K (non-CFM)
	// -----------------------------------------------------------------------
	//	Constructor sets the A5 (or A4) to the value stored in the
	//	given LGlobalsContext object. Destructor restores A5/A4 to
	//	its previous value.

public:
				StSetupGlobals(
					LGlobalsContext& inGlobalsContext)
						{
							#if __A5__
								mOldGlobals = SetA5(inGlobalsContext.GetGlobals());
							#else
								mOldGlobals = SetA4(inGlobalsContext.GetGlobals());
							#endif
						}

				~StSetupGlobals()
						{
							#if __A5__
								SetA5(mOldGlobals);
							#else
								SetA4(mOldGlobals);
							#endif
						}
private:
	long		mOldGlobals;

#else

	// -----------------------------------------------------------------------
	// 		¥ PowerPC or 68K CFM
	// -----------------------------------------------------------------------
	//	There is no need to set up A5/A4/RTOC since the Mixed Mode
	//	Manager will do this automatically. The constructor and
	//	destructor do nothing.

public:
				StSetupGlobals(
					LGlobalsContext& /* inGlobalsContext */)
						{
						}

				~StSetupGlobals()
						{
						}
#endif

};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
