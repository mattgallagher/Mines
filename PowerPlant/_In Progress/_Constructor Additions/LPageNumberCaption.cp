// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LPageNumberCaption.cp		PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	A caption that changes its contents based on the panel number.
//	Used to build strings of the form "Page #" for use in headers
//	of printed documents.

#ifdef PowerPlant_PCH
	#include PowerPlant_PCH
#endif

#include <LPageNumberCaption.h>

PP_Begin_Namespace_PowerPlant


// ---------------------------------------------------------------------------
//	¥ LPageNumberCaption					Stream Constructor		  [public]
// ---------------------------------------------------------------------------

LPageNumberCaption::LPageNumberCaption(
	LStream*	inStream)
	
	: LCaption(inStream)
{
	mOriginalCaption = mText;
}


// ---------------------------------------------------------------------------
//	¥ ~LPageNumberCaption					Destructor				  [public]
// ---------------------------------------------------------------------------

LPageNumberCaption::~LPageNumberCaption()
{
}


// ---------------------------------------------------------------------------
//	¥ PrintPanel													  [public]
// ---------------------------------------------------------------------------
//	Here we adapt the caption so it contains the page number.

void
LPageNumberCaption::PrintPanel(
	const PanelSpec&	inPanel,
	RgnHandle			inSuperPrintRgnH)
{
	LStr255 tempStr = mOriginalCaption;
	tempStr += (SInt32) inPanel.pageNumber;
	SetDescriptor(tempStr);

	LCaption::PrintPanel(inPanel, inSuperPrintRgnH);
}


PP_End_Namespace_PowerPlant
