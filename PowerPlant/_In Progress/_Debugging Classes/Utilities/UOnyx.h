// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UOnyx.h						PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_UOnyx
#define _H_UOnyx
#pragma once

#include <PP_Debug.h>

#if PP_Spotlight_Support
	#include "SpotlightAPI.h"
#endif
#if PP_QC_Support
	#include "QCAPI.h"
#endif

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


PP_Begin_Namespace_PowerPlant


#pragma mark ** Spotlight Supports
// ===========================================================================
//	¥ Spotlight
// ===========================================================================
//	These are various supports for interacting with Spotlight, from
//	Onyx Technology.

// ---------------------------------------------------------------------------
//	API macros
// ---------------------------------------------------------------------------
//	These macros allow you to directly access the Spotlight API.
//	When PP_Spotlight_Support is disabled, the calls to the Spotlight API
//	will be preprocessed out; this provides an easy way to remove the
//	supports when you don't want them (e.g. release builds).

#if PP_Spotlight_Support
	#define SLDisable_()					SLDisable()
	#define SLEnable_()						SLEnable()
	#define SLEnterInterrupt_()				SLEnterInterrupt()
	#define SLLeaveInterrupt_()				SLLeaveInterrupt()
	#define SLResetLeaks_()					SLResetLeaks()
	#define SLInit_()						SLInit()
#else
	#define SLDisable_()					(void)0
	#define SLEnable_()						(void)0
	#define SLEnterInterrupt_()				(void)0
	#define SLLeaveInterrupt_()				(void)0
	#define SLResetLeaks_()					(void)0
	#define SLInit_()						(void)0
#endif


// ---------------------------------------------------------------------------
//	Utility classes
// ---------------------------------------------------------------------------
//	These are some useful utility classes for working with Spotlight. They
//	are only available if PP_Spotlight_Support is true.

#if PP_Spotlight_Support

// ---------------------------------------------------------------------------
//	¥ StSpotlightState
// ---------------------------------------------------------------------------
//	A stack-based class to save/change/restore the Spotlight enable/disable
//	state. Calls to StSpotlightState can be nested (and receive proper
//	behavior). The nesting is done by Spotlight itself.
//
//	Instead of calling this class directly, you may wish to use the macro
//	versions of the class (located below). The macro version will allow you to
//	have the supports in your debug builds and have them preprocessed out
//	in your release builds.

class StSpotlightState {
public:
	enum ESpotlightState {
			spotlightState_Enable	= 0,
			spotlightState_Disable	= 1
	};

			StSpotlightState(ESpotlightState inState = spotlightState_Disable)
				{
					mState = inState;

					if (mState) {
						SLDisable();
					} else {
						SLEnable();
					}
				}

			~StSpotlightState()
				{
					if (mState) {
						SLEnable();
					} else {
						SLDisable();
					}
				}

private:
	ESpotlightState		mState;
};


// ---------------------------------------------------------------------------
//	¥ StSpotlightInterrupt
// ---------------------------------------------------------------------------
//	A stack-based class to enable/disable Spotlight with interrupt code
//
//	Instead of calling this class directly, you may wish to use the macro
//	versions of the class above. The macro version will allow you to
//	have the supports in your debug builds and have them preprocessed out
//	in your release builds.

class StSpotlightInterrupt {
public:
	enum ESpotlightInterrupt {
		spotlightInterrupt_Enter	= 0,
		spotlightInterrupt_Leave	= 1
	};

			StSpotlightInterrupt(ESpotlightInterrupt inState = spotlightInterrupt_Enter)
				{
					mState = inState;

					if (mState) {
						SLEnterInterrupt();
					} else {
						SLLeaveInterrupt();
					}
				}

			~StSpotlightInterrupt()
				{
					if (mState) {
						SLLeaveInterrupt();
					} else {
						SLEnterInterrupt();
					}
				}

private:
	ESpotlightInterrupt		mState;
};

#endif // PP_Spotlight_Support

// ---------------------------------------------------------------------------
// Macros for ease of using the StSpotlightState and StSpotlightInterrupt
// classes. By using these macros you can leave the support in your code
// and they will be automagically preprocessed out in release builds.

#if PP_Spotlight_Support
	#define		StSpotlightState_(state)				\
					PP_PowerPlant::StSpotlightState		__sstate(state)
	#define		StSpotlightInterrupt_(state)			\
					PP_PowerPlant::StSpotlightInterrupt	__sstate(state)
	#define		StSpotlightEnable_()					\
					PP_PowerPlant::StSpotlightState		__sstate(PP_PowerPlant::StSpotlightState::spotlightState_Enable)
	#define		StSpotlightDisable_()					\
					PP_PowerPlant::StSpotlightState		__sstate(PP_PowerPlant::StSpotlightState::spotlightState_Disable)
	#define		StSpotlightInterruptEnable_()			\
					PP_PowerPlant::StSpotlightInterrupt	__sstate(PP_PowerPlant::StSpotlightInterrupt::spotlightInterrupt_Enter)
	#define		StSpotlightInterruptDisable_()		\
					PP_PowerPlant::StSpotlightInterrupt	__sstate(PP_PowerPlant::StSpotlightInterrupt::spotlightInterrupt_Leave)
#else
	#define		StSpotlightState_(state)
	#define		StSpotlightInterrupt_(state)
	#define		StSpotlightEnable_()
	#define		StSpotlightDisable_()
	#define		StSpotlightInterruptEnable_()
	#define		StSpotlightInterruptDisable_()
#endif


#pragma mark -
#pragma mark ** QC Supports
// ===========================================================================
//	¥ QC
// ===========================================================================
//	Various supports for working with QC from Onyx Technology

// ---------------------------------------------------------------------------
//	API macros
// ---------------------------------------------------------------------------
//	These macros allow you to directly access the Spotlight API.
//	When PP_Spotlight_Support is disabled, the calls to the Spotlight API
//	will be preprocessed out; this provides an easy way to remove the
//	supports when you don't want them (e.g. release builds).

#if PP_QC_Support
	#define	QCVerifyPtr_(p)					QCVerifyPtr((Ptr)(p))
	#define QCVerifyHandle_(h)				QCVerifyHandle((Handle)(h))
	#define QCHeapCheckNow_()				UQC::CheckHeap()
	#define QCScrambleHeapNow_()			UQC::ScrambleHeap()
	#define QCBlockBoundsCheckNow_()		UQC::BoundsCheck()
#else
	#define	QCVerifyPtr_(p)
	#define QCVerifyHandle_(h)
	#define QCHeapCheckNow_()
	#define QCScrambleHeapNow_()
	#define QCBlockBoundsCheckNow_()
#endif


// ---------------------------------------------------------------------------
//	QC Error Handling
// ---------------------------------------------------------------------------
//	An extension to the macros found in UException.h for handling QC errors.
//	Only available if PP_QC_Support and PP Debug_Throw support is enabled.

#if PP_QC_Support && defined(Debug_Throw)

	#define ThrowIfQCErr_(err)																	\
		do {																					\
			QCErr	__theErr = err;																\
			if (__theErr != kQCNoErr) {															\
				PP_PowerPlant::UQC::AlertThrowAt(__theErr, StringLiteral_(__FILE__), __LINE__);	\
				Throw_Err(__theErr);															\
			}																					\
		} while (false)

#else

	#define ThrowIfQCErr_(err)		Throw_Err(err)

#endif


// ---------------------------------------------------------------------------
//	¥ UQC
// ---------------------------------------------------------------------------
//	Only available if PP_QC_Support enabled

#if PP_QC_Support

namespace UQC {

	bool				IsInstalled();

	bool				IsActive();
	bool				Activate(
							const THz				inHeapZone = nil);
	bool				Deactivate();

	THz					GetTestingHeap();

	QCStateHandle		GetState();
	void				SetState(
							const QCStateHandle		inStateH);

	bool				GetTestState(
							QCType					inTestSelector);
	bool				SetTestState(
							QCType					inTestSelector,
							bool					inState,
							bool					inActivateIfOff = true);

	void				CheckHeap(
							bool					inAlsoCheckSysHeap = false);
	void				ScrambleHeap(
							bool					inAlsoScrambleSysHeap = false);
	void				BoundsCheck(
							bool					inAlsoCheckSysHeap = false);

	Boolean				ErrorHandler(
							const QCPBPtr			inPB);

	QCCallBackUPP		InstallErrorHandler(
							QCCallBackUPP			inHandler = nil);
	void				RemoveErrorHandler();

	extern QCCallBackUPP	gQCErrorHandlerUPP;

	void				AlertThrowAt(
							QCErr					inErr,
							ConstStr255Param		inFile,
							long					inLine);
}


// ---------------------------------------------------------------------------

class StQCActivate {
public:
			StQCActivate(const THz inHeapZone = nil);
			~StQCActivate();
private:
	THz		mSaveHeap;
};

// ---------------------------------------------------------------------------

class StQCState {
public:
			StQCState(const QCStateHandle inStateH = nil);
			~StQCState();
private:
	QCStateHandle	mStateH;
	StQCActivate	*mActivateState;
};


// ---------------------------------------------------------------------------

class StQCTestState {
public:
			StQCTestState(	QCType	inTestSelector,
							bool	inState);
			~StQCTestState();
private:
	QCType			mSelector;
	bool			mState;
	StQCActivate*	mActivateState;
};

#endif // PP_QC_Support

// ---------------------------------------------------------------------------
//	Macros for ease of using the StQCActivate, StQCState, and StQCTestState
//	macros.

#if PP_QC_Support

	#define StQCActivate_()									\
				PP_PowerPlant::StQCActivate		__qcActivate
	#define StQCState_(stateH)								\
				PP_PowerPlant::StQCState		__qcState(stateH)
	#define StQCTestState_(selector, state)					\
				PP_PowerPlant::StQCTestState	__qcTestState(selector, state)
#else

	#define StQCActivate_()
	#define StQCState_(stateH)
	#define StQCTestState_(selector, state)

#endif


// ---------------------------------------------------------------------------

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_UOnyx
