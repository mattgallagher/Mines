// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LHeapAction.h				PowerPlant 2.2.2	©1997-2005 Metrowerks Inc.
// ===========================================================================
//	Original Author: John C. Daub

#ifndef _H_LHeapAction
#define _H_LHeapAction
#pragma once

#include <PP_Debug.h>
#include <LPeriodical.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

class LHeapAction : public LPeriodical {
public:
				enum EHeapAction {
					heapAction_None					=	0,
					heapAction_Compact				=	1,
					heapAction_Purge				=	2,
					heapAction_CompactAndPurge		=	3
			#if PP_QC_Support
					,heapAction_QCTests				=	4
			#endif
				};


								LHeapAction(
										EHeapAction				inAction,
										const THz				inHeapZone = nil,
										UInt32					inTicks = 0,
										bool					inIsRepeater = true );
								LHeapAction(
										const LHeapAction&		inOriginal);
				LHeapAction&	operator=(
										const LHeapAction&		inRhs);

		virtual					~LHeapAction();

		virtual	void			SpendTime(
										const EventRecord&		inMacEvent);

		virtual	void			Start(	SInt32					inTicks = -1);
		virtual	void			Stop();

				void			ChangeTime(
										UInt32					inWaitTicks)
									{
										mWaitTicks = inWaitTicks;
									}
				UInt32			GetTicks() const
									{
										return mWaitTicks;
									}
				UInt32			GetSeconds() const
									{
										return (mWaitTicks / 60);
									}

				void			MakeRepeater();
				bool			IsRepeater() const { return mRepeater; }

				void			MakeIdler();
				bool			IsIdler() const { return (mRepeater == false); }

				bool			IsRunning() const { return (IsRepeating() || IsIdling()); }

				void			ChangeAction(
										EHeapAction				inAction)
									{
										mAction = inAction;
									}
				EHeapAction		GetAction() const { return mAction; }

protected:
				EHeapAction		mAction;
				THz				mHeapZone;
				UInt32			mWaitTicks;
				UInt32			mLastTime;
				bool			mRepeater;

private:
								LHeapAction();
};

PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif // _H_LHeapAction
