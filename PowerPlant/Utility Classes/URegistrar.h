// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	URegistrar.h				PowerPlant 2.2.2	©1993-2005 Metrowerks Inc.
// ===========================================================================
//
//	Associates class ID's with a function which returns an object of
//	that class. Used for creating classes from data in a Stream.

#ifndef _H_URegistrar
#define _H_URegistrar
#pragma once

#include <TArray.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

// ---------------------------------------------------------------------------

class	LStream;
											// Function returning a newly
											//   created object initialized
											//   from data in a stream
typedef void*	(*ClassCreatorFunc)(LStream *);

struct SClassTableEntry {					// Element in a Class Table
	ClassIDT			classID;
	ClassCreatorFunc	creatorFunc;
};


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

class	URegistrar {
public:
	static void		RegisterClass(
							ClassIDT			inClassID,
							ClassCreatorFunc	inCreatorFunc);

	static void		UnregisterClass(
							ClassIDT			inClassID);

	static void*	CreateObject(
							ClassIDT			inClassID,
							LStream*			inStream);

	static void		DisposeClassTable();

	static TArray<SClassTableEntry>*	GetClassTable()
						{
							return sClassTable;
						}

protected:
	static TArray<SClassTableEntry>*	sClassTable;
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

//	TRegistrar will automatically declare a "create from stream" function
//	for a class and register that function with the URegistrar. You do
//	NOT have to declare a "create from stream" function within the class
//	itself.
//
//	Examples:
//		Name of the class you want to register is "MyClass"
//
//	If you define a class_ID within MyClass:
//
//		TRegistrar<MyClass>::Register();
//
//	If you don't define a class_ID or want to use a different class_ID:
//
//		TRegistrar<MyClass>::RegisterID('ABCD');
//
//	where 'ABCD' is a four-character class ID. This form can be used
//	to associate multiple class IDs with the same class.

template <class T>
class TRegistrar {
public:

	static void*	CreateFromStream(LStream* inStream)
						{
	                      return static_cast<void *>(new T(inStream));
						}

	static void		Register()
						{
	                        URegistrar::RegisterClass( T::class_ID,
													   &CreateFromStream );
						}

	static void		RegisterID(ClassIDT inClassID)
						{
	                        URegistrar::RegisterClass( inClassID,
													   &CreateFromStream );
						}
};


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	Macros for those who don't like to type much.

#define	RegisterClass_(ClassName)	\
				PP_PowerPlant::TRegistrar<ClassName >::Register()

#define RegisterClassID_(ClassName, ClassID)	\
				PP_PowerPlant::TRegistrar<ClassName >::RegisterID(ClassID)


PP_End_Namespace_PowerPlant

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
