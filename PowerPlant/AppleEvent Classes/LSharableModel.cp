// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LSharableModel.cp			PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	Class for shared LModelObjects.
//
//	"Shared" model objects are automatically made non-lazy.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LSharableModel.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LSharableModel						Default Constructor		  [public]
// ---------------------------------------------------------------------------

LSharableModel::LSharableModel()
{
}


// ---------------------------------------------------------------------------
//	¥ LSharableModel						Constructor				  [public]
// ---------------------------------------------------------------------------

LSharableModel::LSharableModel(
	LModelObject*	inSuperModel,
	DescType		inKind)

	: LModelObject(inSuperModel, inKind)
{
}


// ---------------------------------------------------------------------------
//	¥ LSharableModel						Destructor				  [public]
// ---------------------------------------------------------------------------

LSharableModel::~LSharableModel()
{
}


// ---------------------------------------------------------------------------
//	¥ AddUser														  [public]
// ---------------------------------------------------------------------------

void
LSharableModel::AddUser(
	void*	inUser)
{
	SetLaziness(false);
	LSharable::AddUser(inUser);
}


// ---------------------------------------------------------------------------
//	¥ Finalize														  [public]
// ---------------------------------------------------------------------------

void
LSharableModel::Finalize()
{
	if (GetUseCount() <= 0) {
		LModelObject::Finalize();
	}
}


// ---------------------------------------------------------------------------
//	¥ SuperDeleted													  [public]
// ---------------------------------------------------------------------------

void
LSharableModel::SuperDeleted()
{
	if (GetUseCount() <= 0) {
		LModelObject::SuperDeleted();
	}
}


PP_End_Namespace_PowerPlant
