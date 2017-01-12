// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	UResourceMgr.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Stack-based classes for manipulating resources

#ifndef _H_UResourceMgr
#define _H_UResourceMgr
#pragma once

#include <PP_Prefix.h>
#include <UMemoryMgr.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	StNewResource : public StResource {
public:
				StNewResource(
						ResType			inResType,
						ResIDT			inResID,
						Size			inDefaultSize = 0,
						Boolean			inGet1Resource = true);
						
				~StNewResource();

	void		SetResName( ConstStringPtr inNewName );
	
	void		SetResAttrs( SInt16 inResAttrs );

	void		Write( Boolean inUpdateResFile = true );
	
	void		DontWrite();

	Boolean		ResourceExisted()		{ return mResourceExisted; }

protected:
	Boolean		mNeedToWrite;			// true if resource should be written

	ResType		mResType;				// resource type
	ResIDT		mResID;					// resource ID
	SInt16		mResAttrs;				// resource attributes
	Str255		mResName;				// resource name
	Boolean		mResAttrsSet;			// true if caller set resource attributes
	Boolean		mResNameSet;			// true if caller set resource name
	Boolean		mResourceExisted;		// true if resource existed previously
};


// ---------------------------------------------------------------------------

class	StDeleteResource : public StResource {
public:
				StDeleteResource(
						ResType			inResType,
						ResIDT			inResID,
						Boolean			inThrowFail = true,
						Boolean			inGet1Resource = true);
						
				~StDeleteResource();
};


// ---------------------------------------------------------------------------

class	StResLoad {
public:
				StResLoad( Boolean inResLoad = false );
				
				~StResLoad();

	Boolean		mOriginalResLoad;			// original value of ResLoad
};


// ---------------------------------------------------------------------------

class	StCurResFile {
public:
				StCurResFile( SInt16 inNewResFile = -1 );

				~StCurResFile();

protected:
	SInt16	mSaveResFile;
};

// ---------------------------------------------------------------------------
const SInt16		ResourceContext_NoFile		= -1;

class StResourceContext {
public:
							StResourceContext( SInt16 inContext = ResourceContext_NoFile );
							
	virtual					~StResourceContext();

							// CurResFile changers
			void			Enter();
			void			Exit();
			void			Save();

							// Status accessors
			bool			IsValid() const
								{
									return mIsValid;
								}
								
			bool			InContext() const
								{
									return mInContext;
								}

	static	SInt16			GetApplicationRefNum()
								{
									return sAppContext;
								}

	static 	bool			CurrentContextIsValid();
	static	SInt16			GetCurrentRefNum();


private:
			bool			mIsValid;		// true if we have a valid context
			bool			mInContext;		// true if this resource file is active
			bool			mSavedContext;	// true if Save() was called
			SInt16			mPrevContext;	// previous resource fork to restore
			SInt16			mThisContext;	// current resource fork
	StResourceContext*		mSaveContext;	// previous resource context object

	static	SInt16			sAppContext;	// application's resource fork
	static StResourceContext* sCurrentContext;	// current resource context

							StResourceContext(const StResourceContext &inOriginal);
		StResourceContext&	operator=(const StResourceContext &inRhs);
};


// ---------------------------------------------------------------------------
//	Stack-based class to enter & exit the main application's resource context.

class StApplicationContext : public StResourceContext {
public:
								StApplicationContext()
									: StResourceContext(GetApplicationRefNum())
									{
										// nothing
									}
private:
								StApplicationContext(const StApplicationContext& inOriginal);
		StApplicationContext&	operator=(const StApplicationContext& inRhs);
};


PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
