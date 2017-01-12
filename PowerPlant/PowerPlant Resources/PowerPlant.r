// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	PowerPlant.r			  	PowerPlant 2.2.2	©1994-2005 Metrowerks Inc.
// ===========================================================================
//
//	Rez definitions for custom resource types used by PowerPlant
//
//	Resources Types:
//		'PPob'		PowerPlant objects (mostly Panes)
//		'Mcmd'		Command numbers for menu items
//		'RidL'		Resource ID list (long integer IDs)
//		'RID#'		Resource ID list (short integer IDs)
//		'Txtr'		Text traits

#ifndef __POWERPLANT_R__
#define __POWERPLANT_R__

// ===========================================================================

#define	PP_WindowData														\
		integer;							/* WIND Resource ID			*/	\
		integer	modal, floating, regular;	/* Window Layer				*/	\
		bitstring[3] = 0;					/* Placeholder for layer	*/	\
		boolean	noCloseBox, hasCloseBox;	/* Close Box?				*/	\
		boolean	noTitleBar, hasTitleBar;	/* Title Bar?				*/	\
		boolean	noResize, hasResize;		/* Resizable?				*/	\
		boolean noSizeBox, hasSizeBox;		/* Draw Size Box?			*/	\
		boolean	noZoom, hasZoom;			/* Zoomable?				*/	\
		boolean	noShowNew, hasShowNew;		/* Visible after creating?	*/	\
		boolean	disabled, enabled;			/* Enabled?					*/	\
		boolean	noTarget, hasTarget;		/* Targetable?				*/	\
		boolean	noGetSelectClick,			/* Get Select Click?		*/	\
				hasGetSelectClick;											\
		boolean	noHideOnSuspend,			/* Hide when Suspended?		*/	\
				hasHideOnSuspend;											\
		boolean	noDelaySelect,				/* Delayed Selection?		*/	\
				hasDelaySelect;												\
		boolean	noEraseOnUpdate,			/* Erase when Updating?		*/	\
				hasEraseOnUpdate;											\
		bitstring[17] = 0;					/* Reserved bits			*/	\
		integer;							/* Minimum width			*/	\
		integer;							/* Minimum height			*/	\
		integer	screenSize = -1;			/* Maximum width			*/	\
		integer	screenSize = -1;			/* Maximum height			*/	\
		integer	screenSize = -1;			/* Standard width			*/	\
		integer	screenSize = -1;			/* Standard height			*/	\
		longint								/* UserCon					*/

#define	PP_DialogBoxData													\
		PP_WindowData;														\
		longint;							/* Default Button ID		*/	\
		longint								/* Cancel Button ID			*/

#define	PP_PrintoutData														\
		point;								/* Width, Height			*/	\
		byte	inactive, active;			/* Active					*/	\
		byte	disabled, enabled;			/* Enabled					*/	\
		longint;							/* UserCon					*/	\
		boolean	numberAcross, numberDown;	/* Page Numbering Order		*/	\
		bitstring[31] = 0					/* Reserved bits			*/

#define	PP_PaneData															\
		longint;							/* Pane ID					*/	\
		point;								/* Width, Height			*/	\
		byte	invisible, visible;			/* Visible					*/	\
		byte	disabled, enabled;			/* Enabled					*/	\
		byte	unbound, bound;				/* Left Binding				*/	\
		byte	unbound, bound;				/* Top Binding				*/	\
		byte	unbound, bound;				/* Right Binding			*/	\
		byte	unbound, bound;				/* Bottom Binding			*/	\
		longint;							/* Left Location			*/	\
		longint;							/* Top Location				*/	\
		longint;							/* UserCon					*/	\
		longint	noSuperView = 0,			/* SuperView				*/	\
				defaultSuperView = -1

#define	PP_CaptionData														\
		PP_PaneData;														\
		pstring;							/* Caption Text				*/	\
		integer	useSystemFont = 0			/* Text Traits ID			*/

#define	PP_EditFieldData													\
		PP_PaneData;														\
		pstring;							/* Initial Text				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		integer;							/* Maximum Characters		*/	\
		boolean	noBox, hasBox;				/* Box around field?		*/	\
		boolean noWordWrap, hasWordWrap;	/* Wrap text to frame?		*/	\
		boolean noAutoScroll, hasAutoScroll;/* AutoScroll text			*/	\
		boolean noTextBuffering,			/* Text Buffering			*/	\
				hasTextBuffering;			/*   of keyboard input		*/	\
		boolean noOutlineHilite,			/* Outline hiliting when	*/	\
				hasOutlineHilite;			/*   inactive				*/	\
		boolean noInlineInput,				/* Allow inline input		*/	\
				hasInlineInput;												\
		boolean noTextServices,				/* Use Text Services for	*/	\
				hasTextServices;			/*   inputting text			*/	\
		bitstring[1] = 0;					/* Reserved bits			*/	\
		byte	noFilter,					/* Keystroke Filter			*/	\
				integerFilter,												\
				alphaNumericFilter,											\
				printingCharFilter,											\
				negativeIntegerFilter,										\
				printingCharAndCRFilter

#define	PP_ListBoxData														\
		PP_PaneData;														\
		byte	noHorizScroll, hasHorizScroll;								\
		byte	noVertScroll, hasVertScroll;								\
		byte	noGrowBox, hasGrowBox;										\
		byte	noFocusBox, hasFocusBox;									\
		longint;							/* Double-Click Message		*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		integer	textList = 0;				/* LDEF ID					*/	\
		integer = $$CountOf(ListBoxItems);	/* Number of Items in List	*/	\
		array ListBoxItems {												\
			pstring;						/* Text of List Item		*/	\
		}

#define	PP_ControlData														\
		PP_PaneData;														\
		longint;							/* Value Changed Message	*/	\
		longint;							/* Initial Value			*/	\
		longint;							/* Minimum Value			*/	\
		longint								/* Maximum Value			*/

#define	PP_ButtonData														\
		PP_PaneData;														\
		longint;							/* Pushed Message			*/	\
		longint = 0;						/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 0;						/* Maximum Value			*/	\
		literal longint;					/* Graphics Resource Type	*/	\
		integer;							/* ID of Normal Graphic		*/	\
		integer								/* ID of Pushed Graphic		*/

#define	PP_ToggleButtonData													\
		PP_PaneData;														\
		longint;							/* Pushed Message			*/	\
		longint = 0;						/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 1;						/* Maximum Value			*/	\
		literal longint;					/* Graphics Resource Type	*/	\
		integer;							/* ID of On Graphic			*/	\
		integer;							/* ID of On Click Graphic	*/	\
		integer;							/* ID of Off Graphic		*/	\
		integer;							/* ID of Off Click Graphic	*/	\
		integer								/* ID of Transition graphic	*/

#define PP_CicnButtonData													\
		PP_PaneData;														\
		longint;							/* Pushed Message			*/	\
		longint = 0;						/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 0;						/* Maximum Value			*/	\
		integer;							/* ID of Normal 'cicn'		*/	\
		integer								/* ID of Pushed 'cicn'		*/

#define	PP_TextButtonData													\
		PP_PaneData;														\
		longint;							/* Pushed Message			*/	\
		longint	unchecked, checked;			/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 1;						/* Maximum Value			*/	\
		pstring;							/* Title					*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		integer								/* Selected Text Style		*/

#define	PP_StdControlData													\
		PP_ControlData;														\
		integer;							/* Control Kind				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		longint								/* RefCon					*/

#define PP_StdButtonData													\
		PP_PaneData;														\
		longint;							/* Pushed Message			*/	\
		longint = 0;						/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 0;						/* Maximum Value			*/	\
		integer	= 0;						/* pushButProc				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		longint								/* RefCon					*/	\

#define PP_StdCheckBoxData													\
		PP_PaneData;														\
		longint;							/* Value Changed Message	*/	\
		longint	unchecked, checked;			/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 1;						/* Maximum Value			*/	\
		integer	= 1;						/* checkBoxProc				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		longint								/* RefCon					*/

#define PP_StdRadioButtonData												\
		PP_PaneData;														\
		longint;							/* Value Changed Message	*/	\
		longint	unchecked, checked;			/* Initial Value			*/	\
		longint = 0;						/* Minimum Value			*/	\
		longint = 1;						/* Maximum Value			*/	\
		integer	= 2;						/* radioButProc				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		longint								/* RefCon					*/

#define	PP_StdPopupMenuData													\
		PP_PaneData;														\
		longint;							/* Value Changed Message	*/	\
		longint;							/* Title Position & Style	*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* MENU Resource ID			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* Pixel Width of Title		*/	\
		integer	defaultPopup = 1008,		/* Default Variation		*/	\
				fixedWidth = 1009,			/* Fixed Width Popup Box	*/	\
				addResource = 1012,			/* Add Items of Res Type	*/	\
				fixedAndResource = 1013;	/* Fixed Width & Resources	*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		literal longint;					/* ResType for Menu Items	*/	\
		integer								/* Initial Item Choice		*/

#define PP_IconPaneData														\
		PP_PaneData;														\
		integer								/* Icon resource ID			*/

#define PP_ViewData															\
		PP_PaneData;														\
		longint;							/* Image Width				*/	\
		longint;							/* Image Height				*/	\
		longint;							/* Horiz Scroll Position	*/	\
		longint;							/* Vert Scroll Position		*/	\
		longint;							/* Horiz Scroll Unit		*/	\
		longint;							/* Vert Scroll Unit			*/	\
		integer	noReconcileOverhang,		/* Reconcile after resize	*/	\
				hasReconcileOverhang

#define	PP_ScrollerData														\
		PP_ViewData;														\
		integer	noHorizScroll = -1;			/* Horiz SBar Left Indent	*/	\
		integer;							/* Horiz SBar Right Indent	*/	\
		integer	noVertScroll = -1;			/* Vert SBar Top Indent		*/	\
		integer;							/* Vert SBar Bottom Indent	*/	\
		longint								/* Scrolling View ID		*/

#define	PP_ScrollerViewData													\
		PP_PaneData;														\
		integer;							/* Filler					*/	\
		integer	normalThickness = 16,		/* Thickness of scroll bars	*/	\
				floaterThickness = 11;		/*   In floating window		*/	\
		longint;							/* Image Height				*/	\
		longint;							/* Horiz Scroll Position	*/	\
		longint;							/* Vert Scroll Position		*/	\
		longint;							/* Horiz Scroll Unit		*/	\
		longint;							/* Vert Scroll Unit			*/	\
		integer	noReconcileOverhang,		/* Reconcile after resize	*/	\
				hasReconcileOverhang;										\
		integer	noHorizScroll = -1;			/* Horiz SBar Left Indent	*/	\
		integer;							/* Horiz SBar Right Indent	*/	\
		integer	noVertScroll = -1;			/* Vert SBar Top Indent		*/	\
		integer;							/* Vert SBar Bottom Indent	*/	\
		longint;							/* Scrolling View ID		*/	\
		byte	noLiveScrolling,			/* Live Scrolling?			*/	\
				hasLiveScrolling

#define PP_PictureData														\
		PP_ViewData;														\
		integer								/* 'PICT' Resource ID		*/

#define	PP_PlaceHolderData													\
		PP_ViewData;														\
		integer								/* Alignment of Occupant	*/

#define	PP_TextEditData														\
		PP_ViewData;														\
		Boolean	noMultiStyle,				/* Single or Multi Style	*/	\
				hasMultiStyle;												\
		Boolean	noEdit, hasEdit;			/* Read only or Write		*/	\
		Boolean	noSelect, hasSelect;		/* Allow Selection			*/	\
		Boolean	noWordWrap, hasWordWrap;	/* Wrap text to frame?		*/	\
		bitstring[12] = 0;					/* Reserved bits			*/	\
		integer	useSystemFont = 0;			/* TextTraits ID			*/	\
		integer								/* TEXT Resource ID			*/

#define	PP_TextEditViewData													\
		PP_ViewData;														\
		Boolean	noMultiStyle,				/* Single or Multi Style	*/	\
				hasMultiStyle;												\
		Boolean	noEdit, hasEdit;			/* Read only or Write		*/	\
		Boolean	noSelect, hasSelect;		/* Allow Selection			*/	\
		Boolean	noWordWrap, hasWordWrap;	/* Wrap text to frame?		*/	\
		Boolean noAutoScroll, hasAutoScroll;/* Autoscroll text?			*/	\
		Boolean noOutlineHilite,			/* Outline hilite when		*/	\
				hasOutlineHilite;			/*   inactive?				*/	\
		bitstring[10] = 0;					/* Reserved bits			*/	\
		integer	useSystemFont = 0;			/* TextTraits ID			*/	\
		integer								/* TEXT Resource ID			*/

#define	PP_TableData														\
		PP_ViewData;														\
		longint;							/* Number of Rows			*/	\
		longint;							/* Number of Columns		*/	\
		longint;							/* Row Height				*/	\
		longint;							/* Column Width				*/	\
		longint								/* Cell Data Size			*/

#define PP_MultiPanelViewData												\
		PP_ViewData;														\
		integer = $$CountOf(PPobIDs);		/* No. of Panels			*/	\
		wide array PPobIDs {				/* List of:					*/	\
			integer;						/*   PPob ID				*/	\
		};																	\
		integer;							/* Initial Panel Choice		*/	\
		longint;							/* Switch Message			*/	\
		byte	noListenToSuperView,		/* Listen to SuperView?		*/	\
				hasListenToSuperView

#define	PP_RadioGroupData													\
		integer = $$CountOf(RadioIDs);		/* No. of Radios in Group	*/	\
		wide array RadioIDs {												\
			longint;						/* RadioButton Pane ID		*/	\
		}

#define	PP_AttachmentData													\
		longint	any = -2,					/* Any Message				*/	\
				event = 810,				/* Commander, all events	*/	\
				drawOrPrint = 811,			/* Pane, draw or print		*/	\
				click = 812,				/* Pane, click				*/	\
				adjustCursor = 813,			/* Pane, adjiust cursor		*/	\
				keyPress = 814,				/* Commander, key press		*/	\
				commandStatus = 815,		/* Commander, cmd status	*/	\
				postAction = 816;			/* Commander, post action	*/	\
		byte	dontExecute, execute;		/* Execute Host				*/	\
		byte	noOwner, hasOwner			/* Host is owner			*/

#define PP_BorderAttachmentData												\
		PP_AttachmentData;													\
		Point;								/* Pen Size					*/	\
		integer								/* Pen Mode					*/	\
				patCopy = 8, patOr, patXor, patBic,							\
				notPatCopy, notPatOr, notPatXor, notPatBic,					\
				blend = 32, addPin, addOver, subPin, transparent,			\
				adMax, subOver, adMin, ditherCopy = 64;						\
		integer	dkGray, ltGray,				/* Pen Pattern				*/	\
				gray, black, white;											\
		wide array[1] {						/* Foreground Color			*/	\
			unsigned integer;				/*		red					*/	\
			unsigned integer;				/*		green				*/	\
			unsigned integer;				/*		blue				*/	\
		};																	\
		wide array[1] {						/* Background Color			*/	\
			unsigned integer;				/*		red					*/	\
			unsigned integer;				/*		green				*/	\
			unsigned integer;				/*		blue				*/	\
		}

#define	PP_CmdEnablerAttachmentData											\
		PP_AttachmentData;													\
		longint								/* Command to Enable		*/

#define	PP_ColorEraseAttachmentData											\
		PP_AttachmentData;													\
		wide array[1] {						/* Foreground Color			*/	\
			unsigned integer;				/*		red					*/	\
			unsigned integer;				/*		green				*/	\
			unsigned integer;				/*		blue				*/	\
		};																	\
		wide array[1] {						/* Background Color			*/	\
			unsigned integer;				/*		red					*/	\
			unsigned integer;				/*		green				*/	\
			unsigned integer;				/*		blue				*/	\
		}

#define PP_ContextualMenuAttachment											\
		PP_AttachmentData;													\
		integer	defaultMenu = 0;			/* Menu ID					*/	\
		integer	defaultCursor = 450;		/* Cursor ID				*/	\
		longint	noHelp = 0,					/* Help Type				*/	\
				appleGuideHelp = 1,											\
				otherHelp = 2;												\
		pstring;							/* Help Item Text			*/	\
		integer = $$CountOf(CommandList);	/* Number of Commands		*/	\
		wide array CommandList {											\
			longint;						/* Command number			*/	\
		};																	\
		longint								/* CmdTarget Pane ID		*/

#define PP_PaintAttachmentData		PP_BorderAttachmentData

#define PP_WindowThemeAttachmentData										\
		PP_AttachmentData;													\
		integer default = 0,				/* Active Background Brush	*/	\
				activeDialog = 1,											\
				activeAlert = 3,											\
				activeModelessDialog = 5,									\
				activeUtilityWindow = 7,									\
				documentWindow = 15;										\
		integer default = 0,				/* Inactive Background		*/	\
				inactiveDialog = 2,											\
				inactiveAlert = 4,											\
				inactiveModelessDialog = 6,									\
				inactiveUtilityWindow = 8,									\
				documentWindow = 15;										\
		integer default = 0,				/* Active Text Color		*/	\
				activeDialog = 1,											\
				activeAlert = 3,											\
				activeModelessDialog = 5;									\
		integer	default = 0,				/* Inactive Text Color		*/	\
				inactiveDialog = 2,											\
				inactiveAlert = 4,											\
				inactiveModelessDialog = 6

#define	PP_UnusedControlData												\
		PP_PaneData;														\
		longint = 0;						/* Message					*/	\
		longint = 0;						/* Value					*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0							/* Maximum					*/

#define PP_BevelButtonData													\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* Menu ID					*/	\
		integer = 0;						/* Filler					*/	\
		boolean	noOffsetContents,			/* Offset when clicked?		*/	\
				hasOffsetContents;											\
		boolean	noMultiValueMenu,			/* Mutliple selection in	*/	\
				hasMultiValueMenu;			/*   menu?					*/	\
		bitstring[6]	pushButton,			/* Behavior					*/	\
						toggles,											\
						sticky;												\
		byte	textOnly = 0,				/* Content Type				*/	\
				iconSuiteResource = 1,										\
				cicnResource = 2,											\
				pictResource = 3,											\
				iconSuiteHandle = 129,										\
				cicnHandle = 130,											\
				pictHandle = 131,											\
				iconReference = 132;										\
		integer = 0;						/* Filler					*/	\
		integer;							/* Res ID for content		*/	\
		bitstring[12] = 2;					/* Proc ID					*/	\
		boolean = 0;						/* Window Font? (unused)	*/	\
		boolean	arrowDown, arrowRight;		/* Popup Arrow Direction	*/	\
		bitstring[2]	smallBevel,			/* Bevel Size				*/	\
						normalBevel,										\
						largeBevel;											\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		integer;							/* Initial Value			*/	\
		integer		systemDirection = -1,	/* Title Placement			*/	\
					normally = 0,											\
					rightOfGraphic,											\
					leftOfGraphic,											\
					belowGraphic,											\
					aboveGraphic;											\
		integer		flushDefault = 0,		/* Title Alignment			*/	\
					center = 1,												\
					flushRight = -1,										\
					flushLeft = -2;											\
		integer;							/* Title Offset in pixels	*/	\
		integer		systemDirection = -1,	/* Graphic Alignment		*/	\
					center = 0,												\
					left, right,											\
					top, bottom,											\
					topLeft, bottomLeft,									\
					topRight, bottomRight;									\
		point;								/* Graphic Offset (v,h)		*/	\
		byte		noCenterPopupArrow,		/* Center Popup Arrow?		*/	\
					hasCenterPopupArrow

#define	PP_ChasingArrowsData												\
		PP_UnusedControlData;												\
		integer = 112;						/* Control Kind				*/	\
		integer = 0;						/* Text Traits ID (unused)	*/	\
		byte = 0							/* Title (empty string)		*/

#define	PP_CheckBoxData														\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		longint	unchecked, checked, mixed;	/* Initial Value			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 2;						/* Maximum					*/	\
		integer = 369;						/* Control Kind				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring								/* Title					*/

#define	PP_CheckBoxGroupBoxData												\
		PP_ViewData;														\
		longint;							/* Value Message			*/	\
		longint	unchecked, checked, mixed;	/* Initial Value			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 2;						/* Maximum					*/	\
		integer	primaryGroup = 161,			/* Group Box Kind			*/	\
				secondaryGroup = 165;										\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring								/* Title					*/

#define PP_ClockData														\
		PP_PaneData;														\
		longint = 0;						/* Value Message			*/	\
		longint	noFlogs = 0,				/* Clock Flags				*/	\
				displayOnly = 1,											\
				liveDisplayOnly = 3;										\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer	time = 240,					/* Clock Kind				*/	\
				timeWithSeconds = 241,										\
				date = 242,													\
				monthAndYear = 243;											\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_CmdBevelButtonData												\
		PP_BevelButtonData;													\
		longint								/* Command to send			*/

#define PP_DisclosureTriangleData											\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		longint		up, down;				/* Initial Setting			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 1;						/* Maximum					*/	\
		integer		faceRight = 64,			/* Up Direction				*/	\
					faceLeft = 65;											\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_EditTextData														\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		longint = 0;						/* Initial Value			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer		regular = 272,			/* Kind of EditText			*/	\
					password = 274;											\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Initial Text				*/	\
		integer;							/* Maximum Characters		*/	\
		boolean	noBox, hasBox;				/* Box around field?		*/	\
		boolean noWordWrap, hasWordWrap;	/* Wrap text to frame?		*/	\
		boolean noAutoScroll, hasAutoScroll;/* AutoScroll text			*/	\
		boolean noTextBuffering,			/* Text Buffering			*/	\
				hasTextBuffering;			/*   of keyboard input		*/	\
		boolean noOutlineHilite,			/* Outline hiliting when	*/	\
				hasOutlineHilite;			/*   inactive				*/	\
		boolean noInlineInput,				/* Allow inline input		*/	\
				hasInlineInput;												\
		boolean noTextServices,				/* Use Text Services for	*/	\
				hasTextServices;			/*   inputting text			*/	\
		bitstring[1] = 0;					/* Reserved bits			*/	\
		byte	noFilter,					/* Keystroke Filter			*/	\
				integerFilter,												\
				alphaNumericFilter,											\
				printingCharFilter,											\
				negativeIntegerFilter,										\
				printingCharAndCRFilter

#define	PP_IconControlData													\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* Icon Resource ID			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		bitstring[14] = 80;					/* Control Kind				*/	\
		bitstring[1]	cicn, suite;		/* Icon Type				*/	\
		bitstring[1]	hasTracking,		/* Tracking					*/	\
						noTracking;			/*   has = 0, no = 1		*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0;							/* Title					*/	\
		integer		none = 0,				/* Icon Alignment			*/	\
					center = 5,												\
					centerTop = 6,											\
					centerBottom = 7,										\
					centerLeft = 9,											\
					topLeft = 10,											\
					bottomLeft = 11,										\
					centerRight = 13,										\
					topRight = 14,											\
					bottomRight = 15

#define	PP_ImageWellData													\
		PP_ViewData;														\
		longint;							/* Value Message			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* Content Resource ID		*/	\
		integer = 0;						/* Filler					*/	\
		integer	iconSuiteResource = 1,		/* Content Type				*/	\
				cicnResource = 2,											\
				pictResource = 3,											\
				iconSuiteHandle = 129,										\
				cicnHandle = 130,											\
				pictHandle = 131,											\
				iconReference = 132;										\
		longint = 0;						/* Maximum					*/	\
		integer = 176;						/* Control Kind				*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_LittleArrowsData													\
		PP_ControlData;														\
		integer = 96;						/* Control Kind				*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_PictureControlData												\
		PP_ViewData;														\
		longint;							/* Value Message			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* PICT Resource ID			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer		hasTracking = 304,		/* Control Kind				*/	\
					noTracking = 305;										\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_PlacardData														\
		PP_ViewData;														\
		longint = 0;						/* Message					*/	\
		longint = 0;						/* Value					*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer = 224;						/* Control Kind				*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_PopupButtonData													\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		bitstring[17] = 0;					/* Filler					*/	\
		boolean	noExtended,	hasExtended;	/* Title Style Options		*/	\
		boolean	noCondensed, hasCondensed;									\
		boolean	noShadow,	hasShadow;										\								\
		boolean	noOutline,	hasOutline;										\
		boolean	noUnderline, hasUnderline;									\
		boolean	noItalic,	hasItalic;										\
		boolean	noBold,		hasBold;										\
		byte	alignLeft = 0,				/* Title Alignment			*/	\
				alignCenter = 1,											\
				alignRight = 255;											\
		integer = 0;						/* Filler					*/	\
		integer	noMenuResource = -12345;	/* MENU Resource ID			*/	\
		integer = 0;						/* Filler					*/	\
		integer	calculateWidth = -1;		/* Title Width				*/	\
		bitstring[13] = 50;					/* Control Kind				*/	\
		boolean	noResMenu, hasResMenu;		/* Fill Menu from ResType?	*/	\
		boolean = 0;						/* Filler					*/	\
		boolean	noFixedWidith,				/* Fixed or Variable Width	*/	\
				hasFixedWidth;												\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		literal longint;					/* ResType for Menu Items	*/	\
		integer								/* Initial Item Choice		*/

#define	PP_PopupGroupBoxData												\
		PP_ViewData;														\
		longint;							/* Value Message			*/	\
		bitstring[17] = 0;					/* Filler					*/	\
		boolean	noExtended,	hasExtended;	/* Title Style Options		*/	\
		boolean	noCondensed, hasCondensed;									\
		boolean	noShadow,	hasShadow;										\								\
		boolean	noOutline,	hasOutline;										\
		boolean	noUnderline, hasUnderline;									\
		boolean	noItalic,	hasItalic;										\
		boolean	noBold,		hasBold;										\
		byte	alignLeft = 0,				/* Title Alignment			*/	\
				alignCenter = 1,											\
				alignRight = 255;											\
		integer = 0;						/* Filler					*/	\
		integer	noMenuResource = -12345;	/* MENU Resource ID			*/	\
		integer = 0;						/* Filler					*/	\
		integer	calculateWidth = -1;		/* Title Width				*/	\
		integer	primaryGroup = 162,			/* Group Kind				*/	\
				secondaryGroup = 166;										\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		integer								/* Initial Item Choice		*/

#define	PP_ProgressBarData													\
		PP_ControlData;														\
		integer = 80;						/* Control Kind				*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0;							/* Title					*/	\
		byte	determinate,				/* Bar fills up				*/	\
				indeterminate				/*   Barber Pole			*/

#define	PP_PushButtonData													\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		longint = 0;						/* Value					*/	\
		longint = 0;						/* Minimum					*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* cicn ID					*/	\
		integer		textOnly = 368,			/* Control Kind				*/	\
					iconOnLeft = 374,										\
					iconOnRight = 375;										\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		byte	notDefault, isDefault		/* Default Button?			*/

#define	PP_RadioButtonData													\
		PP_PaneData;														\
		longint;							/* Value Message			*/	\
		longint	off, on, mixed;				/* Initial Value			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 2;						/* Maximum					*/	\
		integer = 370;						/* Control Kind				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring								/* Title					*/

#define	PP_ScrollBarData													\
		PP_ControlData;														\
		integer	regular = 384,				/* Control Kind				*/	\
				liveScrolling = 386;										\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_SeparatorLineData												\
		PP_UnusedControlData;												\
		integer = 144;						/* Control Kind				*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/

#define	PP_SliderData														\
		PP_ControlData;														\
		bitstring[12] = 3;					/* Proc ID					*/	\
		boolean	hasDirection,				/* Directional Indicator?	*/	\
				noDirection;												\
		boolean	normalDirection,			/* Direction of Indicator	*/	\
				reverseDirection;											\
		boolean	noTicks, hasTicks;			/* Has tick marks?			*/	\
		boolean	noLiveFeedback,				/* Live Feedback?			*/	\
				hasLiveFeedback;											\
		integer								/* Number of tick marks		*/

#define	PP_StaticTextData													\
		PP_UnusedControlData;												\
		integer = 288;						/* Control Kind				*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring								/* Title					*/

#define	PP_TabsControlData													\
		PP_ViewData;														\
		longint;							/* Value Message			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* tab# Resource ID			*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer	largeTabs = 128,			/* Control Kind				*/	\
				smallTabs = 129;											\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		byte = 0;							/* Title					*/	\
		integer								/* Initial Tab Choice		*/

#define	PP_TextGroupBoxData													\
		PP_ViewData;														\
		longint = 0;						/* Value Message			*/	\
		longint = 0;						/* Value					*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer		primaryGroup = 160,		/* Group Kind				*/	\
					secondaryGroup = 164;									\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring								/* Title					*/

#define	PP_WindowHeaderData													\
		PP_ViewData;														\
		longint = 0;						/* Value Message			*/	\
		longint = 0;						/* Value					*/	\
		longint = 0;						/* Minimum					*/	\
		longint = 0;						/* Maximum					*/	\
		integer		regular = 336,			/* Control Kind				*/	\
					listView = 337;			/*   No bottom line			*/	\
		integer = 0;						/* Text Traits ID			*/	\
		byte = 0							/* Title					*/


#define	Field_IconSize				integer									\
		none = 0, mini = 12, small = 16, large = 32

#define	Field_IconPosition			integer									\
		none, topLeft, topCenter, topRight, rightCenter,					\
		bottomRight, bottomCenter, bottomLeft, leftCenter, center


#define	PP_GAIconButtonData													\
		PP_ControlData;														\
		integer	noMode, buttonMode,			/* Control Mode				*/	\
				radioMode, switchMode;										\
		integer;							/* Icon Suite ID			*/	\
		Field_IconSize;						/* Icon Size				*/	\
		Field_IconPosition;					/* Icon Position			*/	\
		byte	noHilite, hasHilite;		/* Hilite when pressed		*/	\
		byte	noOffset, hasOffset			/* Offset Icon on Hilite	*/

#define	PP_GACmdIconButtonData												\
		PP_GAIconButtonData;												\
		longint								/* Command Number			*/

#define PP_GACmdTextButtonData												\
		PP_StdControlData;													\
		longint								/* Command Number			*/

#define	PP_GADisclosureTriangleData											\
		PP_ControlData;														\
		byte	facesRight, facesLeft		/* Faces Right or Left?		 */

#define	PP_GAIconTextButtonData												\
		PP_GAIconButtonData;												\
		pstring;							/* Title					*/	\
		integer useSystemFont = 0			/* Text Traits ID			*/

#define	PP_GACmdIconTextButtonData											\
		PP_GAIconTextButtonData;											\
		longint								/* Command Number			*/

#define	PP_GAIconButtonPopupData											\
		PP_GAIconButtonData;												\
		integer;							/* Popup Menu Resource ID	*/	\
		integer;							/* Initial Item				*/	\
		integer useSystemFont = 0			/* Text Traits ID			*/

#define	PP_GAIconSuiteControlData											\
		PP_ControlData;														\
		integer	noMode, buttonMode,			/* Control Mode				*/	\
				radioMode, switchMode;										\
		integer;							/* Icon Suite ID			*/	\
		Field_IconSize;						/* Icon Size				*/	\
		Field_IconPosition;					/* Icon Position			*/	\
		byte	noHilite, hasHilite;		/* Hilite when pressed?		*/	\
		byte	noClickInIcon,				/* Click In Icon?			*/	\
				hasClickInIcon

#define PP_GACmdIconSuiteControlData										\
		PP_GAIconSuiteControlData;											\
		longint								/* Command Number			*/

#define	PP_GAPopupData														\
		PP_PaneData;														\
		longint;							/* Value Changed Message	*/	\
		longint;							/* Title Position & Style	*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* MENU Resource ID			*/	\
		integer = 0;						/* Filler					*/	\
		integer;							/* Pixel Width of Title		*/	\
		integer	defaultPopup = 1008,		/* Default Variation		*/	\
				fixedWidth = 1009,			/* Fixed Width Popup Box	*/	\
				addResource = 1012,			/* Add Items of Res Type	*/	\
				fixedAndResource = 1013;	/* Fixed Width & Resources	*/	\
		integer	useSystemFont = 0;			/* Text Traits ID			*/	\
		pstring;							/* Title					*/	\
		literal longint;					/* ResType for Menu Items	*/	\
		integer								/* Initial Item Choice		*/

#define	PP_GABoxData														\
		PP_ViewData;														\
		byte;								/* Filler					*/	\
		pstring;							/* Title					*/	\
		integer useSystemFont = 0;			/* Text Traits ID			*/	\
		byte noBorder, hasBorder;			/* Border?					*/	\
		integer	noBorder,					/* Border Style				*/	\
				plainBorder,												\
				bezelBorder,												\
				recessedOneBorder,											\
				recessedTwoBorder,											\
				embossedOneBorder,											\
				embossedTwoBorder;											\
		integer	none,						/* Title Position			*/	\
				default,													\
				topRight,													\
				topLeftOut,													\
				topRightOut,												\
				centerTopOut,												\
				centerTop,													\
				centerInside,												\
				centerTopInside,											\
				centerBottomInside,											\
				centerBottom,												\
				centerBottomOut

#define	PP_GAColorSwatchData												\
		PP_PaneData;														\
		integer;							/* RGB Color: Red			*/	\
		integer;							/* 			  Green			*/	\
		integer								/*			  Blue			*/

#define	PP_GAIconSuiteData													\
		PP_PaneData;														\
		integer;							/* Icon Suite ID			*/	\
		Field_IconSize;						/* Icon Size				*/	\
		Field_IconPosition					/* Icon Position			*/

#define	PP_GAPrimaryBoxData													\
		PP_ViewData;														\
		pstring;							/* Title					*/	\
		integer useSystemFont = 0			/* Text Traits ID			*/

#define	PP_GAFocusBorderData												\
		PP_ViewData;														\
		longint;							/* Inset SubPane ID			*/	\
		longint;							/* Commander SubPane ID		*/	\
		byte	noPaintFace, hasPaintFace;	/* Paint Border Face?		*/	\
		byte	noFrameInset, hasFrameInset;/* Frame Inset SubPane?		*/	\
		byte	noNotchInset, hasNotchInset;/* Notch Inset Border?		*/	\
		byte	noNotchFace, hasNotchFace;	/* Notch Border Face?		*/	\
		integer;							/* Notch Width				*/	\
		byte	noCanFocus, hasCanFocus;	/* Can Focus?				*/	\
		byte	noCurrentFocus,				/* Is Currently Focused?	*/	\
				hasCurrentFocus

#define PP_GALittleArrowsData												\
		PP_ControlData;														\
		integer singleClick = 1,			/* Click Action				*/	\
				delayContinuous = 2,										\
				continuous = 3;												\
		integer;							/* Delay for continuous		*/	\
		integer								/* Delay for value change	*/

#define PP_GATabPanelData													\
		PP_ViewData;														\
		integer useSystemFont = 0;			/* Text Traits ID			*/	\
		longint;							/* Panel Message			*/	\
		integer = $$CountOf(Titles);		/* Number of Titles	*/			\
		array Titles {														\
			pstring;						/* Text of Tab Title		*/	\
		};																	\
		integer;							/* Initial selection		*/	\
		byte	noPlaceHolder,				/* Want PlaceHolder?		*/	\
				hasPlaceHolder;												\
		Field_IconPosition					/* Pane Alignment			*/

#define PP_GAWindowHeaderData												\
		PP_ViewData;														\
		byte	noAdornment, hasAdornment;	/* Adornment?				*/	\
		byte	noDivider, hasDivider		/* Divider at Bottom?		*/

#define PP_PageControllerData												\
		PP_ViewData;														\
		integer;							/* Text Traits ID			*/	\
		longint;							/* Controller Message		*/	\
		integer;							/* STR# ID					*/	\
		integer;							/* Initial selection index	*/	\
		wide array [3] {					/* Colors: Background		*/	\
											/*		   Face				*/	\
											/*		   Pushed Text		*/	\
			unsigned integer;				/*		RGB: red			*/	\
			unsigned integer;				/*			 green			*/	\
			unsigned integer;				/*			 blue			*/	\
		};																	\
		byte	noGroupedArrows,			/* Arrows grouped at end	*/	\
				hasGroupedArrows

#define	PP_GAColorSwatchControlData											\
		PP_UnusedControlData;												\
		unsigned integer;					/* Color:	red				*/	\
		unsigned integer;					/* 			green			*/	\
		unsigned integer;					/* 			blue			*/	\
		pstring								/* Prompt string			*/

// ===========================================================================

type 'PPob' {								// PowerPlant Object
	integer = 2;								// version number
	wide array TagArray {

		switch {

		case ObjectData:
			key literal longint = 'objd';
			ObjectDataStart:
			longint = (ObjectDataEnd[$$ArrayIndex(TagArray)] - ObjectDataStart[$$ArrayIndex(TagArray)]) / 8 - 4;

			switch {
													// Pane Classes
			case AbstractPane:
				key literal longint = 'pane';
				PP_PaneData;

			case AbstractView:
				key literal longint = 'view';
				PP_ViewData;

			case ActiveScroller:
				key literal longint = 'ascr';
				PP_ScrollerData;

			case Button:
				key literal longint = 'butn';
				PP_ButtonData;

			case Caption:
				key literal longint = 'capt';
				PP_CaptionData;

			case CicnButton:
				key literal longint = 'cicn';
				PP_CicnButtonData;

			case Control:
				key literal longint = 'cntl';
				PP_ControlData;

			case DialogBox:
				key literal longint = 'dlog';
				PP_DialogBoxData;

			case EditField:
				key literal longint = 'edit';
				PP_EditFieldData;

			case GrafPortView:
				key literal longint = 'gpvw';
				PP_ViewData;

			case GroupBox:
				key literal longint = 'gbox';
				PP_CaptionData;

			case IconPane:
				key literal longint = 'icnp';
				PP_IconPaneData;

			case ListBox:
				key literal longint = 'lbox';
				PP_ListBoxData;

			case MultiPanelView:
				key literal longint = 'mpvw';
				PP_MultiPanelViewData;

			case OffscreenView:
				key literal longint = 'offv';
				PP_ViewData;

			case OverlappingView:
				key literal longint = 'ovlv';
				PP_ViewData;

			case Picture:
				key literal longint = 'pict';
				PP_PictureData;

			case PlaceHolder:
				key literal longint = 'plac';
				PP_PlaceHolderData;

			case Printout:
				key literal longint = 'prnt';
				PP_PrintoutData;

			case RadioGroupView:
				key literal longint = 'rgpv';
				PP_ViewData;

			case Scroller:
				key literal longint = 'scrl';
				PP_ScrollerData;

			case ScrollerView:
				key literal longint = 'sclv';
				PP_ScrollerViewData;

			case StdButton:
				key literal longint = 'pbut';
				PP_StdButtonData;

			case StdCheckBox:
				key literal longint = 'cbox';
				PP_StdCheckBoxData;

			case StdControl:
				key literal longint = 'sctl';
				PP_StdControlData;

			case StdPopupMenu:
				key literal longint = 'popm';
				PP_StdPopupMenuData;

			case StdRadioButton:
				key literal longint = 'rbut';
				PP_StdRadioButtonData;

			case SubOverlapView:
				key literal longint = 'solv';
				PP_ViewData;

			case TabGroupView:
				key literal longint = 'tbgv';
				PP_ViewData;

			case Table:
				key literal longint = 'tabl';
				PP_TableData;

			case TableView:
				key literal longint = 'tabv';
				PP_ViewData;

			case TextButton:
				key literal longint = 'txbt';
				PP_TextButtonData;

			case TextEdit:
				key literal longint = 'text';
				PP_TextEditData;

			case TextEditView:
				key literal longint = 'txtv';
				PP_TextEditViewData;

			case ToggleButton:
				key literal longint = 'tbut';
				PP_ToggleButtonData;

			case Window:
				key literal longint = 'wind';
				PP_WindowData;
													// Support Classes
			case TabGroup:
				key literal longint = 'tabg';

			case RadioGroup:
				key literal longint = 'radg';
				PP_RadioGroupData;
													// Attachments
			case Attachment:
				key literal longint = 'atch';
				PP_AttachmentData;

			case BeepAttachment:
				key literal longint = 'beep';
				PP_AttachmentData;

			case BorderAttachment:
				key literal longint = 'brda';
				PP_BorderAttachmentData;

			case CmdEnablerAttachment:
				key literal longint = 'cena';
				PP_CmdEnablerAttachmentData;

			case ColorEraseAttachment:
				key literal longint = 'cers';
				PP_ColorEraseAttachmentData;

			case ContextualMenuAttachment:
				key literal longint = 'cmat';
				PP_ContextualMenuAttachment;

			case EraseAttachment:
				key literal longint = 'eras';
				PP_AttachmentData;

			case KeyScrollAttachment:
				key literal longint = 'ksca';
				PP_AttachmentData;

			case PaintAttachment:
				key literal longint = 'pnta';
				PP_PaintAttachmentData;

			case WindowThemeAttachment:
				key literal longint = 'wtha';
				PP_WindowThemeAttachmentData;
													// Appearance Controls
			case BevelButton:
				key literal longint = 'bbut';
				PP_BevelButtonData;

			case ChasingArrows:
				key literal longint = 'carr';
				PP_ChasingArrowsData;

			case CheckBox:
				key literal longint = 'chbx';
				PP_CheckBoxData;

			case CheckBoxGroupBox:
				key literal longint = 'cbgb';
				PP_CheckBoxGroupBoxData;

			case Clock:
				key literal longint = 'clck';
				PP_ClockData;

			case CmdBevelButton:
				key literal longint = 'cbbt';
				PP_CmdBevelButtonData;

			case DisclosureTriangle:
				key literal longint = 'dtri';
				PP_DisclosureTriangleData;

			case EditText:
				key literal longint = 'etxt';
				PP_EditTextData;

			case IconControl:
				key literal longint = 'ictl';
				PP_IconControlData;

			case ImageWell:
				key literal longint = 'iwel';
				PP_ImageWellData;

			case LittleArrows:
				key literal longint = 'larr';
				PP_LittleArrowsData;

			case PictureControl:
				key literal longint = 'picd';
				PP_PictureControlData;

			case Placard:
				key literal longint = 'plcd';
				PP_PlacardData;

			case PopupButton:
				key literal longint = 'popb';
				PP_PopupButtonData;

			case PopupGroupBox:
				key literal longint = 'pgbx';
				PP_PopupGroupBoxData;

			case ProgressBar:
				key literal longint = 'pbar';
				PP_ProgressBarData;

			case PushButton:
				key literal longint = 'push';
				PP_PushButtonData;

			case RadioButton:
				key literal longint = 'rdbt';
				PP_RadioButtonData;

			case ScrollBar:
				key literal longint = 'sbar';
				PP_ScrollBarData;

			case SeparatorLine:
				key literal longint = 'sepl';
				PP_SeparatorLineData;

			case Slider:
				key literal longint = 'slid';
				PP_SliderData;

			case StaticText:
				key literal longint = 'stxt';
				PP_StaticTextData;

			case TabsControl:
				key literal longint = 'tabs';
				PP_TabsControlData;

			case TextGroupBox:
				key literal longint = 'tgbx';
				PP_TextGroupBoxData;

			case WindowHeader:
				key literal longint = 'winh';
				PP_WindowHeaderData;
													// Grayscale Classes
			case GACheckBox:
				key literal longint = 'gchk';
				PP_StdCheckBoxData;

			case GAPushButton:
				key literal longint = 'gpsh';
				PP_StdButtonData;

			case GARadioButton:
				key literal longint = 'grad';
				PP_StdRadioButtonData;

			case GAIconButton:
				key literal longint = 'gibt';
				PP_GAIconButtonData;

			case GACmdIconButton:
				key literal longint = 'gcib';
				PP_GACmdIconButtonData;

			case GATextButton:
				key literal longint = 'gtxb';
				PP_StdControlData;

			case GACmdTextButton:
				key literal longint = 'gctb';
				PP_GACmdTextButtonData;

			case GADisclosureTriangle:
				key literal longint = 'gdcl';
				PP_GADisclosureTriangleData;

			case GAIconTextButton:
				key literal longint = 'gitb';
				PP_GAIconTextButtonData;

			case GACmdIconTextButton:
				key literal longint = 'gcit';
				PP_GACmdIconTextButtonData;

			case GAIconButtonPopup:
				key literal longint = 'gibp';
				PP_GAIconButtonPopupData;

			case GAIconSuiteControl:
				key literal longint = 'gict';
				PP_GAIconSuiteControlData;

			case GACmdIconSuiteControl:
				key literal longint = 'gcic';
				PP_GACmdIconSuiteControlData;

			case GAPopup:
				key literal longint = 'gpop';
				PP_GAPopupData;

			case GABox:
				key literal longint = 'gabx';
				PP_GABoxData;

			case GABoxGroup:
				key literal longint = 'gbgp';
				PP_GABoxData;

			case GACaption:
				key literal longint = 'gcap';
				PP_CaptionData;

			case GAColorSwatch:
				key literal longint = 'gswt';
				PP_GAColorSwatchData;

			case GADialogBox:
				key literal longint = 'gdlb';
				PP_DialogBoxData;

			case GAEditField:
				key literal longint = 'gedt';
				PP_EditFieldData;

			case GAIconSuite:
				key literal longint = 'gicn';
				PP_GAIconSuiteData;

			case GAPrimaryBox:
				key literal longint = 'gpbx';
				PP_GAPrimaryBoxData;

			case GAPrimaryGroup:
				key literal longint = 'gpgp';
				PP_GAPrimaryBoxData;

			case GASecondaryBox:
				key literal longint = 'gsbx';
				PP_GAPrimaryBoxData;

			case GASecondaryGroup:
				key literal longint = 'gsgp';
				PP_GAPrimaryBoxData;

			case GASeparator:
				key literal longint = 'gsep';
				PP_PaneData;

			case GAFocusBorder:
				key literal longint = 'gfbd';
				PP_GAFocusBorderData;

			case GALittleArrows:
				key literal longint = 'glar';
				PP_GALittleArrowsData;

			case GATabPanel:
				key literal longint = 'gtbp';
				PP_GATabPanelData;

			case GAWindowHeader:
				key literal longint = 'whdr';
				PP_GAWindowHeaderData;

			case PageController:
				key literal longint = 'pctl';
				PP_PageControllerData;

			case GAColorSwatchControl:
				key literal longint = 'cswt';
				PP_GAColorSwatchControlData;

			case GADialog:
				key literal longint = 'gdlg';
				PP_DialogBoxData;

			case NullObject:
				key literal longint = 'null';

#ifdef Include_User_Template				// User-defined Object definitions
	#if defined(mw_rez)
		#include User_Template_File
	#else
		#include $$Shell("User_Template_File")
	#endif
#endif

			}; // End Object Type Switch
			ObjectDataEnd:

		case BeginSubs:						// Beginning of sub-object list
			key literal longint = 'begs';

		case EndSubs:						// End of sub-object list
			key literal longint = 'ends';

		case UserObject:
			key literal longint = 'user';
			literal longint;				// Superclass ID for next Object

		case ClassAlias:
			key literal longint = 'dopl';
			literal longint;				// Class ID for next Object

		case Comment:						// Data used by PPob editors, ignored by PP
			key literal longint = 'comm';
			CommentStart:
			longint = (CommentEnd[$$ArrayIndex(TagArray)] - CommentStart[$$ArrayIndex(TagArray)]) / 8 - 4;
			hex string[$$Long(CommentStart[$$ArrayIndex(TagArray)])];
			CommentEnd:

		}; // End Tag Switch

	}; // End TagArray

	longint = 'end.';						// End of Tags Marker

}; // End 'PPob'

// ===========================================================================

type 'Mcmd' {								// Menu Commands
	integer = $$CountOf(CommandArray);		//	Number of commands
	wide array CommandArray{
		longint								//	Command number
				cmd_Nothing=0,	msg_Nothing=0, cmd_UseMenuItem=-1,
				cmd_About=1,
				cmd_New=2, cmd_Open=3, cmd_Close=4, cmd_Save=5, cmd_SaveAs=6, cmd_SaveCopyAs=18,
				cmd_Revert=7, cmd_PageSetup=8, cmd_Print=9, cmd_PrintOne=17, cmd_Quit=10,

				cmd_Undo=11, cmd_Cut=12, cmd_Copy=13, cmd_Paste=14, cmd_Clear=15,
				cmd_SelectAll=16, cmd_ShowClipboard=19,

				cmd_FontMenu=250, cmd_SizeMenu=251, cmd_StyleMenu=252,
				cmd_FontLarger=301, cmd_FontSmaller=302, cmd_FontOther=303,
				cmd_Plain=401, cmd_Bold=402, cmd_Italic=403, cmd_Underline=404,
				cmd_Outline=405, cmd_Shadow=406, cmd_Condense=407, cmd_Extend=408,

				cmd_JustifyDefault=411, cmd_JustifyLeft=412, cmd_JustifyCenter=413,
				cmd_JustifyRight=414, cmd_JustifyFull=415;
	};
};

// ===========================================================================

type 'RidL' {								// Resource ID List (long integers)
	integer = $$CountOf(ResourceIDList);
	wide array ResourceIDList {
		longint;
	};
};

// ===========================================================================

type 'RID#' {								// Resource ID List (short integers)
	integer = $$CountOf(ResIDList);
	wide array ResIDList {
		integer;
	};
};

// ===========================================================================

type 'Txtr' {								// Text Traits
	integer		defaultSize = 0;			//	Size
	integer;								//	Style
	integer		flushDefault = 0,			//	Justification
				center = 1,
				flushRight = -1,
				flushLeft = -2;
	integer									//	Transfer mode
				srcCopy, srcOr, srcXor, srcBic,
				notSrcCopy, notSrcOr, notSrcXor, notSrcBic,
				patCopy, patOr, patXor, patBic,
				notPatCopy, notPatOr, notPatXor, notPatBic,
				blend = 32, addPin, addOver, subPin, transparent,
				adMax, subOver, adMin,
				grayishTextOr = 49, hilitetransfermode, ditherCopy = 64;
	unsigned integer;						//	Color:	red
	unsigned integer;						//			green
	unsigned integer;						//			blue
	integer		useName = -1;				//	Font number
	pstring;								//	Font name
};


#endif	// __POWERPLANT_R__
