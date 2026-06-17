/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: FontPicker.c /main/1 2025/06/17 cde-font-picker $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FontPicker.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Font picker dialog.
 **                Builds its own UI (a family TitleBox + scrolled list on
 **                the left, a size TitleBox + scrolled list in the middle,
 **                a preview TitleBox on the right with a system font
 **                sample label and an editable user font text field,
 **                a Source: option menu on top, and Apply / Apply
 **                System-Wide / Cancel buttons at the bottom).
 **
 **                Enumerates available fonts through the Dt/FontEnum API
 **                (X11 core XListFonts, and fontconfig if --enable-xft
 **                was selected at configure time). The Source option
 **                menu toggles which backend the picker queries.
 **
 **                The preview is a SEPARATE preview from the one inside
 **                Font.c's Font dialog — this dialog is launched from
 **                a "Customize..." button on the Font dialog and feeds
 **                the chosen font back to it (or to the system-wide
 **                resource database).
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#ifdef USE_XFT
#define _CDE_SAVED_USE_XFT 1
#endif
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/PushBG.h>
#include <Xm/VendorSEP.h>
#ifdef _CDE_SAVED_USE_XFT
#define USE_XFT 1
#undef _CDE_SAVED_USE_XFT
#endif

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>
#include <Dt/Message.h>
#include <Dt/SessionM.h>
#include <Dt/HourGlass.h>
#include <Dt/GetDispRes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * <Dt/FontEnum.h> forward-declares XmFontList as `struct _XmFontListRec *`,
 * guarded by the symbol _XmFontList_H (which Motif's own Xm.h does not
 * actually use). The real Motif typedef is `struct __XmRenderTableRec **`.
 * Pre-defining the guard here suppresses the broken forward declaration
 * and lets the real Xm.h typedef from above stand. Same trick used in
 * lib/DtFont/FontEnum.c.
 */
#ifndef _XmFontList_H
#define _XmFontList_H
#endif
#include <Dt/FontEnum.h>

#include "Font.h"
#include "FontPicker.h"
#include "Main.h"
#include "Resource.h"
#include "Help.h"
#include "Protocol.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/

/*
 * Message catalog IDs. Set 5 (font messages). Font.c owns IDs 1-26;
 * this file starts at 27.
 */
#define PICKER_MSG_TITLE     ((char *)GETMESSAGE(5, 27, "Select Font"))
#define PICKER_MSG_CUSTOMIZE ((char *)GETMESSAGE(5, 28, "Customize..."))
#define PICKER_MSG_APPLY     ((char *)GETMESSAGE(5, 29, "Apply"))
#define PICKER_MSG_APPLYSYS  ((char *)GETMESSAGE(5, 30, "Apply System-Wide"))
#define PICKER_MSG_SOURCE    ((char *)GETMESSAGE(5, 31, "Source:"))
#define PICKER_MSG_COREX11   ((char *)GETMESSAGE(5, 32, "Core X11"))
#define PICKER_MSG_FONTCFG   ((char *)GETMESSAGE(5, 33, "Fontconfig"))
#define PICKER_MSG_ALL       ((char *)GETMESSAGE(5, 34, "All"))
#define PICKER_MSG_FAMILY    ((char *)GETMESSAGE(5, 35, "Family"))
#define PICKER_MSG_SIZE      ((char *)GETMESSAGE(5, 36, "Size"))
#define PICKER_MSG_PREVIEW   ((char *)GETMESSAGE(5, 37, "Preview"))
#define PICKER_MSG_SAMPLE    ((char *)GETMESSAGE(5, 38, "AaBbCcDdEeFfGg0123456789"))
#define PICKER_MSG_PRIV      ((char *)GETMESSAGE(5, 39, \
    "Applying the font system-wide requires root privileges."))
#define PICKER_MSG_SYSAPPLY_NOSEL ((char *)GETMESSAGE(5, 40, \
    "Unable to apply font system-wide: no font selected."))
#define PICKER_MSG_SYSAPPLY_OK    ((char *)GETMESSAGE(5, 41, \
    "Font applied system-wide. New users will see this font as the default after logging in."))
#define PICKER_MSG_SYSAPPLY_FAIL  ((char *)GETMESSAGE(5, 42, \
    "System-wide font application failed. You may need administrator privileges."))

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

FontPickerData fontPicker = {0};

/*
 * Track initial family/size to highlight when the picker pops up.
 * Set by PopupFontPicker(); consumed by CreateFontPicker().
 */
static int initialFamily = -1;
static int initialSize   = -1;

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Function prototypes          */
/*+++++++++++++++++++++++++++++++++++++++*/

static void PopulateFamilyList(void);
static void PopulateSizeList(void);
static void UpdatePreview(void);

static void FamilySelectCB(Widget w, XtPointer client_data, XtPointer call_data);
static void SizeSelectCB(Widget w, XtPointer client_data, XtPointer call_data);
static void SourceSelectCB(Widget w, XtPointer client_data, XtPointer call_data);
static void ApplyCB(Widget w, XtPointer client_data, XtPointer call_data);
static void SystemWideCB(Widget w, XtPointer client_data, XtPointer call_data);
static void CancelCB(Widget w, XtPointer client_data, XtPointer call_data);
static void PickerMapCB(Widget w, XtPointer client_data, XtPointer call_data);

static char *xlfd_to_family(const char *xlfd);
static DtFontEnumSource detect_pattern_type(const char *pattern);
static char *pattern_to_display_name(const char *pattern);

static DtFontInfo *find_info_by_family(const char *family);

/*+++++++++++++++++++++++++++++++++++++++*/
/* Public entry points                   */
/*+++++++++++++++++++++++++++++++++++++++*/

/*
 * CreateFontPicker
 *   Build the dialog shell + every child widget. Called from
 *   PopupFontPicker() the first time the picker is shown. Subsequent
 *   popups reuse the same widget tree.
 *
 *   Layout:
 *     pickerDialog (DialogShell)
 *       pickerForm (XmForm — main work area)
 *         sourceLabel + sourceOption (top, XmATTACH_FORM)
 *         familyTB  + sizeTB      (left + middle)
 *         previewTB               (right)
 *         applyButton / systemWideButton / cancelButton (bottom row)
 */
void
CreateFontPicker(Widget parent)
{
    int             n;
    Arg             args[MAX_ARGS];
    Widget          pickerForm;
    Widget          sourceLabel;
    Widget          familyTB;
    Widget          sizeTB;
    Widget          previewTB;
    Widget          previewForm;
    Widget          buttonRowForm;
    XmString        string;

    /* Create the dialog shell. We use xmDialogShellWidgetClass directly
     * so the picker is a true modal-style dialog (not a DialogBox, since
     * the layout is multi-section and we want a different button set). */
    n = 0;
    XtSetArg(args[n], XmNtitle, PICKER_MSG_TITLE); n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    fontPicker.pickerDialog =
        XtVaCreatePopupShell("fontPickerDialog",
                             xmDialogShellWidgetClass,
                             parent,
                             XmNtitle, PICKER_MSG_TITLE,
                             XmNallowShellResize, True,
                             NULL);

    n = 0;
    XtSetArg(args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetValues(fontPicker.pickerDialog, args, n);

    /* The main work area is a Form that holds everything. */
    pickerForm = XtVaCreateWidget("pickerForm",
                                  xmFormWidgetClass,
                                  fontPicker.pickerDialog,
                                  XmNhorizontalSpacing, style.horizontalSpacing,
                                  XmNverticalSpacing,   style.verticalSpacing,
                                  XmNallowOverlap,      False,
                                  NULL);

    /* ----- Source option menu (top of dialog) ----- */
    n = 0;
    string = CMPSTR(PICKER_MSG_SOURCE);
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,      style.verticalSpacing); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,     style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    sourceLabel = XmCreateLabelGadget(pickerForm, "sourceLabel", args, n);
    XmStringFree(string);

    {
        Widget  optionMenu;
        Widget  coreBtn;
        Widget  allBtn = NULL;
        XmString coreLabel, fontcfgLabel = NULL, allLabel = NULL;
        int     defaultIdx = 0;
#ifdef USE_XFT
        Widget fontcfgBtn;
#endif

        coreLabel  = CMPSTR(PICKER_MSG_COREX11);

        n = 0;
        XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget,      sourceLabel);     n++;
        XtSetArg(args[n], XmNtopOffset,      0);               n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNleftWidget,     sourceLabel);     n++;
        XtSetArg(args[n], XmNleftOffset,     4);               n++;
        XtSetArg(args[n], XmNspacing,        0);               n++;
        optionMenu = XmCreateOptionMenu(pickerForm, "sourceOption", args, n);

        coreBtn = XmCreatePushButtonGadget(optionMenu, "coreX11", NULL, 0);
        XtVaSetValues(coreBtn, XmNlabelString, coreLabel, NULL);
        XmStringFree(coreLabel);
        XtManageChild(coreBtn);

#ifdef USE_XFT
        fontcfgLabel = CMPSTR(PICKER_MSG_FONTCFG);
        fontcfgBtn = XmCreatePushButtonGadget(optionMenu, "fontconfig", NULL, 0);
        XtVaSetValues(fontcfgBtn, XmNlabelString, fontcfgLabel, NULL);
        XmStringFree(fontcfgLabel);
        XtManageChild(fontcfgBtn);

        allLabel = CMPSTR(PICKER_MSG_ALL);
        allBtn = XmCreatePushButtonGadget(optionMenu, "all", NULL, 0);
        XtVaSetValues(allBtn, XmNlabelString, allLabel, NULL);
        XmStringFree(allLabel);
        XtManageChild(allBtn);

        defaultIdx = 2;  /* DtFontEnumAll */
#endif

        XtAddCallback(optionMenu, XmNcommandChangedCallback, SourceSelectCB, NULL);
        XtManageChild(optionMenu);
        fontPicker.sourceOption = optionMenu;
        fontPicker.currentSource = (DtFontEnumSource)defaultIdx;
    }

    /* ----- Family TitleBox + scrolled list (left column) ----- */
    n = 0;
    string = CMPSTR(PICKER_MSG_FAMILY);
    XtSetArg(args[n], XmNtitleString,    string); n++;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,      sourceLabel);     n++;
    XtSetArg(args[n], XmNtopOffset,      style.verticalSpacing + 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,     style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,   style.verticalSpacing); n++;
    familyTB = _DtCreateTitleBox(pickerForm, "familyTB", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy,    XmBROWSE_SELECT); n++;
    XtSetArg(args[n], XmNautomaticSelection, True);             n++;
    XtSetArg(args[n], XmNvisibleItemCount,   7);                n++;
    XtSetArg(args[n], XmNlistSizePolicy,     XmCONSTANT);       n++;
    fontPicker.familyList = XmCreateScrolledList(familyTB, "familyList", args, n);
    XtManageChild(fontPicker.familyList);
    XtAddCallback(fontPicker.familyList, XmNbrowseSelectionCallback,
                  FamilySelectCB, NULL);

    /* ----- Size TitleBox + scrolled list (middle column) ----- */
    n = 0;
    string = CMPSTR(PICKER_MSG_SIZE);
    XtSetArg(args[n], XmNtitleString,    string); n++;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,      sourceLabel);     n++;
    XtSetArg(args[n], XmNtopOffset,      style.verticalSpacing + 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,     familyTB);        n++;
    XtSetArg(args[n], XmNleftOffset,     style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,   style.verticalSpacing); n++;
    sizeTB = _DtCreateTitleBox(pickerForm, "sizeTB", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy,    XmBROWSE_SELECT); n++;
    XtSetArg(args[n], XmNautomaticSelection, True);             n++;
    XtSetArg(args[n], XmNvisibleItemCount,   7);                n++;
    XtSetArg(args[n], XmNlistSizePolicy,     XmCONSTANT);       n++;
    fontPicker.sizeList = XmCreateScrolledList(sizeTB, "sizeList", args, n);
    XtManageChild(fontPicker.sizeList);
    XtAddCallback(fontPicker.sizeList, XmNbrowseSelectionCallback,
                  SizeSelectCB, NULL);

    /* ----- Preview TitleBox + form (right column) ----- */
    n = 0;
    string = CMPSTR(PICKER_MSG_PREVIEW);
    XtSetArg(args[n], XmNtitleString,    string); n++;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,      sourceLabel);     n++;
    XtSetArg(args[n], XmNtopOffset,      style.verticalSpacing + 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,     sizeTB);          n++;
    XtSetArg(args[n], XmNleftOffset,     style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,    style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,   style.verticalSpacing); n++;
    previewTB = _DtCreateTitleBox(pickerForm, "previewTB", args, n);
    XmStringFree(string);

    previewForm = XmCreateForm(previewTB, "previewForm", NULL, 0);

    fontPicker.previewLabel = XtVaCreateWidget("previewLabel",
        xmLabelGadgetClass, previewForm,
        XmNlabelString,      CMPSTR(PICKER_MSG_SAMPLE),
        XmNalignment,        XmALIGNMENT_BEGINNING,
        XmNtopAttachment,    XmATTACH_FORM,
        XmNtopOffset,        2 * style.verticalSpacing,
        XmNleftAttachment,   XmATTACH_FORM,
        XmNrightAttachment,  XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_NONE,
        NULL);

    fontPicker.previewText = XtVaCreateWidget("previewText",
        xmTextWidgetClass, previewForm,
        XmNvalue,            PICKER_MSG_SAMPLE,
        XmNtopAttachment,    XmATTACH_WIDGET,
        XmNtopWidget,        fontPicker.previewLabel,
        XmNtopOffset,        2 * style.verticalSpacing,
        XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
        XmNleftWidget,       fontPicker.previewLabel,
        XmNrightAttachment,  XmATTACH_OPPOSITE_WIDGET,
        XmNrightWidget,      fontPicker.previewLabel,
        XmNbottomAttachment, XmATTACH_NONE,
        XmNeditMode,         XmSINGLE_LINE_EDIT,
        NULL);

    XtManageChild(fontPicker.previewLabel);
    XtManageChild(fontPicker.previewText);
    XtManageChild(previewForm);

    /* ----- Button row at the bottom of the dialog ----- */
    buttonRowForm = XtVaCreateWidget("buttonRowForm",
                                     xmFormWidgetClass,
                                     pickerForm,
                                     XmNfractionBase,     3,
                                     XmNtopAttachment,    XmATTACH_NONE,
                                     XmNleftAttachment,   XmATTACH_FORM,
                                     XmNrightAttachment,  XmATTACH_FORM,
                                     XmNbottomAttachment, XmATTACH_FORM,
                                     XmNbottomOffset,     style.verticalSpacing,
                                     XmNhorizontalSpacing, style.horizontalSpacing,
                                     NULL);

    fontPicker.applyButton = XtVaCreateManagedWidget("applyButton",
        xmPushButtonGadgetClass, buttonRowForm,
        XmNlabelString,    CMPSTR(PICKER_MSG_APPLY),
        XmNtopAttachment,    XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment,   XmATTACH_POSITION,
        XmNleftPosition,     0,
        XmNrightAttachment,  XmATTACH_POSITION,
        XmNrightPosition,    1,
        NULL);
    XtAddCallback(fontPicker.applyButton, XmNactivateCallback,
                  ApplyCB, NULL);

    fontPicker.systemWideButton = XtVaCreateManagedWidget("systemWideButton",
        xmPushButtonGadgetClass, buttonRowForm,
        XmNlabelString,    CMPSTR(PICKER_MSG_APPLYSYS),
        XmNtopAttachment,    XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment,   XmATTACH_POSITION,
        XmNleftPosition,     1,
        XmNrightAttachment,  XmATTACH_POSITION,
        XmNrightPosition,    2,
        NULL);
    XtAddCallback(fontPicker.systemWideButton, XmNactivateCallback,
                  SystemWideCB, NULL);

    fontPicker.cancelButton = XtVaCreateManagedWidget("cancelButton",
        xmPushButtonGadgetClass, buttonRowForm,
        XmNlabelString,    CMPSTR((String)_DtCancelString),
        XmNtopAttachment,    XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment,   XmATTACH_POSITION,
        XmNleftPosition,     2,
        XmNrightAttachment,  XmATTACH_POSITION,
        XmNrightPosition,    3,
        NULL);
    XtAddCallback(fontPicker.cancelButton, XmNactivateCallback,
                  CancelCB, NULL);

    XtManageChild(buttonRowForm);

    /* Manage the dialog form, but don't popup yet — PopupFontPicker does that. */
    XtManageChild(pickerForm);

    /* We want the dialog to know when it is mapped, so the family list
     * is populated at that point. */
    XtAddCallback(fontPicker.pickerDialog, XmNpopupCallback, PickerMapCB, NULL);

    /* Suppress unused-variable warnings for build without USE_XFT. */
    (void)initialFamily;
    (void)initialSize;
}

/*
 * PopupFontPicker
 *   Show the picker. Lazy-creates it on first call. Populates the
 *   family list every call (the X server's font list may have changed
 *   between sessions). Restores initial selection if `familyIdx` and
 *   `sizeIdx` are valid (>=-1).
 */
void
PopupFontPicker(int familyIdx, int sizeIdx)
{
    if (fontPicker.pickerDialog == NULL) {
        initialFamily = familyIdx;
        initialSize   = sizeIdx;
        CreateFontPicker(style.shell);
    }

    fontPicker.pickerActive = True;

    /*
     * Re-populate family list. If the user previously closed the dialog
     * with selections, those are still in fontPicker.*; the new list
     * itself is fresh from the X server.
     */
    PopulateFamilyList();

    XtPopup(fontPicker.pickerDialog, XtGrabNone);
    raiseWindow(XtWindow(fontPicker.pickerDialog));
}

/*
 * PopdownFontPicker
 *   Hide the dialog, free per-show allocations, leave FontPickerData
 *   in a state ready for the next popup.
 */
void
PopdownFontPicker(void)
{
    if (!fontPicker.pickerActive)
        return;

    fontPicker.pickerActive = False;
    XtUnmanageChild(fontPicker.pickerDialog);
    XtPopdown(fontPicker.pickerDialog);

    /* Free per-show allocations. The widgets themselves are reused. */
    if (fontPicker.availableFonts != NULL) {
        DtFreeFontList(fontPicker.availableFonts);
        fontPicker.availableFonts = NULL;
    }
    if (fontPicker.selectedFamily != NULL) {
        XtFree(fontPicker.selectedFamily);
        fontPicker.selectedFamily = NULL;
    }
    if (fontPicker.selectedXlfd != NULL) {
        XtFree(fontPicker.selectedXlfd);
        fontPicker.selectedXlfd = NULL;
    }
    if (fontPicker.selectedFcPattern != NULL) {
        XtFree(fontPicker.selectedFcPattern);
        fontPicker.selectedFcPattern = NULL;
    }
    if (fontPicker.currentFontList != NULL) {
        XmFontListFree(fontPicker.currentFontList);
        fontPicker.currentFontList = NULL;
    }
    fontPicker.selectedSize = 0;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* Apply stubs                           */
/*+++++++++++++++++++++++++++++++++++++++*/

/*
 * FontPickerApply / FontPickerApplySystemWide
 *   Public hooks invoked from the Font.c ButtonCB after Task 9-13
 *   integration. They are also called from the picker's own ApplyCB /
 *   SystemWideCB. For now they just look up the currently selected
 *   font and stash it into style.xrdb so subsequent previews use it.
 *   The real xrdb/session-manager write happens in Font.c's ButtonCB.
 */
void
FontPickerApply(void)
{
    const char *sysStr;
    const char *userStr;

    if (fontPicker.selectedXlfd == NULL && fontPicker.selectedFcPattern == NULL)
        return;

    /*
     * The XLFD pattern is the canonical choice for the *systemFont slot
     * because core X11 widgets can resolve it directly. The fontconfig
     * pattern (when present) is the better fit for the *userFont slot
     * because Motif 2.3+ prefers fontconfig when Xft is available.
     * Fall back to the other if only one is present.
     */
    sysStr  = fontPicker.selectedXlfd ? fontPicker.selectedXlfd
                                      : fontPicker.selectedFcPattern;
    userStr = fontPicker.selectedFcPattern ? fontPicker.selectedFcPattern
                                           : fontPicker.selectedXlfd;

    FontDataSetCustomFont((String)sysStr, (String)userStr,
                          (int)fontPicker.currentSource);
}

void
FontPickerApplySystemWide(void)
{
    char *fontres;
    int   result;

    /* First apply to current session so the picker is consistent with
     * what the system-wide apply will write. */
    FontPickerApply();

    fontres = BuildFontResourceString();
    if (fontres == NULL) {
        InfoDialog(PICKER_MSG_SYSAPPLY_NOSEL, style.shell, False);
        return;
    }

    result = RequestSystemWideApply(fontres);
    XtFree(fontres);

    if (result != 0)
        InfoDialog(PICKER_MSG_SYSAPPLY_FAIL, style.shell, False);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal: enumeration                 */
/*+++++++++++++++++++++++++++++++++++++++*/

/*
 * PopulateFamilyList
 *   Re-enumerate available font families and refill the familyList
 *   ScrolledList widget. The previous DtFontList (if any) is freed.
 */
static void
PopulateFamilyList(void)
{
    DtFontList   *list;
    XmStringTable items;
    int           i;

    if (fontPicker.availableFonts != NULL) {
        DtFreeFontList(fontPicker.availableFonts);
        fontPicker.availableFonts = NULL;
    }

    list = DtEnumerateFontFamilies(style.display,
                                   DefaultScreen(style.display));
    if (list == NULL) {
        XtVaSetValues(fontPicker.familyList,
                      XmNitems,       NULL,
                      XmNitemCount,   0,
                      NULL);
        return;
    }

    fontPicker.availableFonts = list;

    items = (XmStringTable)XtMalloc((Cardinal)list->count * sizeof(XmString));
    if (items == NULL)
        return;

    for (i = 0; i < list->count; i++) {
        char *name = (list->fonts[i].family_name != NULL) ?
                     list->fonts[i].family_name : (char *)"";
        items[i] = CMPSTR(name);
    }

    XtVaSetValues(fontPicker.familyList,
                  XmNitems,     items,
                  XmNitemCount, list->count,
                  NULL);

    for (i = 0; i < list->count; i++)
        XmStringFree(items[i]);
    XtFree((char *)items);

    /* Re-populate the size list too — its contents depend on the
     * selected family, but if the user wants "size for the first
     * family" we need at least a placeholder selection. */
    if (initialFamily >= 0 && initialFamily < list->count) {
        XmStringTable sitems;
        char         *fam = list->fonts[initialFamily].family_name;
        if (fam != NULL) {
            XtFree(fontPicker.selectedFamily);
            fontPicker.selectedFamily = XtNewString(fam);
            PopulateSizeList();

            /* Highlight the requested family. */
            sitems = (XmStringTable)XtMalloc(sizeof(XmString));
            sitems[0] = CMPSTR(fam);
            XtVaSetValues(fontPicker.familyList,
                          XmNselectedItems,     sitems,
                          XmNselectedItemCount, 1,
                          NULL);
            XmStringFree(sitems[0]);
            XtFree((char *)sitems);

            if (initialSize >= 0) {
                int       scount;
                XmStringTable sel = NULL;
                XtVaGetValues(fontPicker.sizeList,
                              XmNitemCount, &scount,
                              NULL);
                if (scount > 0 && initialSize < scount) {
                    int pos = initialSize + 1;
                    XmListSelectPos(fontPicker.sizeList, pos, False);
                    XmListSetPos(fontPicker.sizeList, pos);
                    XtVaGetValues(fontPicker.sizeList,
                                  XmNselectedItems, &sel,
                                  NULL);
                    (void)sel;
                }
            }
        }
    }
}

/*
 * PopulateSizeList
 *   Fill the sizeList with the available sizes for the currently
 *   selected family.
 */
static void
PopulateSizeList(void)
{
    DtFontList   *list;
    XmStringTable items;
    int           i;

    if (fontPicker.selectedFamily == NULL)
        return;

    list = DtEnumerateFontSizes(style.display,
                                DefaultScreen(style.display),
                                fontPicker.selectedFamily,
                                fontPicker.currentSource);
    if (list == NULL) {
        XtVaSetValues(fontPicker.sizeList,
                      XmNitems,     NULL,
                      XmNitemCount, 0,
                      NULL);
        return;
    }

    items = (XmStringTable)XtMalloc((Cardinal)list->count * sizeof(XmString));
    if (items == NULL) {
        DtFreeFontList(list);
        return;
    }

    for (i = 0; i < list->count; i++) {
        char buf[32];
        if (list->fonts[i].pixel_size > 0)
            snprintf(buf, sizeof(buf), "%d", list->fonts[i].pixel_size);
        else
            snprintf(buf, sizeof(buf), "scalable");
        items[i] = CMPSTR(buf);
    }

    XtVaSetValues(fontPicker.sizeList,
                  XmNitems,     items,
                  XmNitemCount, list->count,
                  NULL);

    for (i = 0; i < list->count; i++)
        XmStringFree(items[i]);
    XtFree((char *)items);

    /* The size list ownership belongs to us, but the family list owns
     * the DtFontList* that produced the families. We just used it; free. */
    DtFreeFontList(list);
}

/*
 * UpdatePreview
 *   Re-resolve the currently selected font pattern into an XmFontList
 *   and apply it to the preview widgets. The previous XmFontList
 *   (if any) is freed — this matches the reference-counting rules
 *   for XmFontList.
 */
static void
UpdatePreview(void)
{
    const char  *pattern = NULL;
    XmFontList   newFontList;

    if (fontPicker.selectedFcPattern != NULL) {
        pattern = fontPicker.selectedFcPattern;
    } else if (fontPicker.selectedXlfd != NULL) {
        pattern = fontPicker.selectedXlfd;
    } else {
        return;
    }

    newFontList = DtGetFontXmFontList(style.display, pattern);
    if (newFontList == NULL)
        return;

    if (fontPicker.currentFontList != NULL)
        XmFontListFree(fontPicker.currentFontList);

    XtVaSetValues(fontPicker.previewLabel, XmNfontList, newFontList, NULL);
    XtVaSetValues(fontPicker.previewText,  XmNfontList, newFontList, NULL);
    fontPicker.currentFontList = newFontList;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal: callbacks                   */
/*+++++++++++++++++++++++++++++++++++++++*/

static void
FamilySelectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cb = (XmListCallbackStruct *)call_data;
    DtFontInfo           *info;
    int                   pos;

    (void)w;
    (void)client_data;

    if (cb == NULL || cb->item_position <= 0)
        return;
    pos = cb->item_position - 1;

    if (fontPicker.availableFonts == NULL ||
        pos >= fontPicker.availableFonts->count)
        return;

    info = &fontPicker.availableFonts->fonts[pos];

    /* Free old selections, capture new ones. */
    if (fontPicker.selectedFamily != NULL) {
        XtFree(fontPicker.selectedFamily);
        fontPicker.selectedFamily = NULL;
    }
    if (fontPicker.selectedXlfd != NULL) {
        XtFree(fontPicker.selectedXlfd);
        fontPicker.selectedXlfd = NULL;
    }
    if (fontPicker.selectedFcPattern != NULL) {
        XtFree(fontPicker.selectedFcPattern);
        fontPicker.selectedFcPattern = NULL;
    }

    if (info->family_name != NULL)
        fontPicker.selectedFamily = XtNewString(info->family_name);
    if (info->xlfd != NULL)
        fontPicker.selectedXlfd = XtNewString(info->xlfd);
    if (info->fc_pattern != NULL)
        fontPicker.selectedFcPattern = XtNewString(info->fc_pattern);

    /* Reset the preview until a size is picked. */
    if (fontPicker.currentFontList != NULL) {
        XmFontListFree(fontPicker.currentFontList);
        fontPicker.currentFontList = NULL;
    }

    PopulateSizeList();
}

static void
SizeSelectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cb = (XmListCallbackStruct *)call_data;
    int                   pos;

    (void)w;
    (void)client_data;

    if (cb == NULL || cb->item_position <= 0)
        return;
    pos = cb->item_position - 1;

    if (fontPicker.selectedFamily == NULL)
        return;

    /*
     * Re-enumerate just for this family and pick the entry at `pos`.
     * The list is in a stable order across calls because DtEnumerateFontSizes
     * sorts by pixel size.
     */
    {
        DtFontList *list;
        list = DtEnumerateFontSizes(style.display,
                                    DefaultScreen(style.display),
                                    fontPicker.selectedFamily,
                                    fontPicker.currentSource);
        if (list != NULL && pos < list->count) {
            DtFontInfo *info = &list->fonts[pos];

            if (fontPicker.selectedXlfd != NULL) {
                XtFree(fontPicker.selectedXlfd);
                fontPicker.selectedXlfd = NULL;
            }
            if (fontPicker.selectedFcPattern != NULL) {
                XtFree(fontPicker.selectedFcPattern);
                fontPicker.selectedFcPattern = NULL;
            }
            if (info->xlfd != NULL)
                fontPicker.selectedXlfd = XtNewString(info->xlfd);
            if (info->fc_pattern != NULL)
                fontPicker.selectedFcPattern = XtNewString(info->fc_pattern);

            fontPicker.selectedSize = info->pixel_size;
            DtFreeFontList(list);
        } else {
            if (list != NULL) DtFreeFontList(list);
            return;
        }
    }

    UpdatePreview();
}

static void
SourceSelectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmRowColumnCallbackStruct *cb = (XmRowColumnCallbackStruct *)call_data;

    (void)w;
    (void)client_data;

    if (cb == NULL)
        return;

    /* Map the visible button index in the option menu to the enum. */
#ifdef USE_XFT
    if (cb->widget != NULL) {
        String name = XtName(cb->widget);
        if (name != NULL && strcmp(name, "coreX11") == 0)
            fontPicker.currentSource = DtFontEnumCoreX11;
        else if (name != NULL && strcmp(name, "fontconfig") == 0)
            fontPicker.currentSource = DtFontEnumFontconfig;
        else
            fontPicker.currentSource = DtFontEnumAll;
    } else
#endif
    {
        fontPicker.currentSource = DtFontEnumCoreX11;
    }

    /* Wipe the previous size list / preview, then re-enumerate families
     * under the new source. */
    if (fontPicker.currentFontList != NULL) {
        XmFontListFree(fontPicker.currentFontList);
        fontPicker.currentFontList = NULL;
    }
    XtVaSetValues(fontPicker.sizeList, XmNitemCount, 0, NULL);
    PopulateFamilyList();
}

static void
ApplyCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)w;
    (void)client_data;
    (void)call_data;

    if (fontPicker.selectedFamily == NULL ||
        (fontPicker.selectedXlfd == NULL && fontPicker.selectedFcPattern == NULL))
        return;

    /* Stash the selection so Font.c's ButtonCB can pick it up later. */
    FontPickerApply();

    /* Close the picker; the Font dialog remains mapped for the user to
     * commit via its own OK button. */
    PopdownFontPicker();
}

static void
SystemWideCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *fontres;
    int   result;

    (void)w;
    (void)client_data;
    (void)call_data;

    if (fontPicker.selectedFamily == NULL ||
        (fontPicker.selectedXlfd == NULL && fontPicker.selectedFcPattern == NULL))
        return;

    /*
     * First apply to the current session so the preview is consistent
     * with what we are about to write to /etc/dt/app-defaults/Dtstyle.
     * Then ask Protocol.c to invoke the pkexec helper that performs the
     * actual privileged write.
     */
    FontPickerApply();

    fontres = BuildFontResourceString();
    if (fontres == NULL) {
        InfoDialog(PICKER_MSG_SYSAPPLY_NOSEL, style.shell, False);
        PopdownFontPicker();
        return;
    }

    result = RequestSystemWideApply(fontres);
    XtFree(fontres);

    if (result == 0)
        InfoDialog(PICKER_MSG_SYSAPPLY_OK, style.shell, False);
    else
        InfoDialog(PICKER_MSG_SYSAPPLY_FAIL, style.shell, False);

    PopdownFontPicker();
}

static void
CancelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)w;
    (void)client_data;
    (void)call_data;
    PopdownFontPicker();
}

static void
PickerMapCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)w;
    (void)client_data;
    (void)call_data;

    /* If PopulateFamilyList already ran (it does, in PopupFontPicker),
     * the list is already populated. This callback is here as a hook
     * for any future per-show work. */
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal: pattern helpers             */
/*+++++++++++++++++++++++++++++++++++++++*/

/*
 * detect_pattern_type
 *   Identify whether a pattern is XLFD (starts with '-') or fontconfig.
 *   Without USE_XFT, only XLFD patterns are ever valid.
 */
static DtFontEnumSource
detect_pattern_type(const char *pattern)
{
    if (pattern == NULL)
        return DtFontEnumCoreX11;
    if (pattern[0] == '-')
        return DtFontEnumCoreX11;
#ifdef USE_XFT
    return DtFontEnumFontconfig;
#else
    return DtFontEnumCoreX11;
#endif
}

/*
 * pattern_to_display_name
 *   Best-effort human-readable label for a pattern. The returned string
 *   is malloc'd; the caller is responsible for XtFree()ing it.
 *
 *   For XLFD: extracts the family name.
 *   For fontconfig: returns the family name (the pattern itself).
 */
static char *
pattern_to_display_name(const char *pattern)
{
    if (pattern == NULL)
        return XtNewString("unknown");

    if (pattern[0] == '-')
        return xlfd_to_family(pattern);

    return XtNewString(pattern);
}

/*
 * xlfd_to_family
 *   Extract the family name (2nd field) from an XLFD. Local copy of the
 *   helper in lib/DtFont/FontEnum.c. Returns an XtMalloc'd string the
 *   caller must XtFree.
 */
static char *
xlfd_to_family(const char *xlfd)
{
    const char *p;
    const char *dash;
    size_t      len;

    if (xlfd == NULL || xlfd[0] != '-')
        return NULL;

    p = xlfd + 1;
    dash = strchr(p, '-');
    if (dash == NULL)
        len = strlen(p);
    else
        len = (size_t)(dash - p);

    if (len == 0)
        return NULL;

    {
        char *family = (char *)XtMalloc(len + 1);
        if (family == NULL)
            return NULL;
        memcpy(family, p, len);
        family[len] = '\0';
        return family;
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal: lookup helpers              */
/*+++++++++++++++++++++++++++++++++++++++*/

/*
 * find_info_by_family
 *   Linear scan through fontPicker.availableFonts for an entry whose
 *   family_name matches. Returns the pointer (into the DtFontList, do
 *   not free) or NULL.
 */
static DtFontInfo *
find_info_by_family(const char *family)
{
    int i;

    if (fontPicker.availableFonts == NULL || family == NULL)
        return NULL;

    for (i = 0; i < fontPicker.availableFonts->count; i++) {
        DtFontInfo *fi = &fontPicker.availableFonts->fonts[i];
        if (fi->family_name != NULL && strcmp(fi->family_name, family) == 0)
            return fi;
    }
    return NULL;
}


