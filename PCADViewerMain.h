/***************************************************************
 * Name:      PCADViewerMain.h
 * Purpose:   Defines Application Frame
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-01
 * Copyright: Dichrograph ()
 * License:
 **************************************************************/

#ifndef PCADVIEWERMAIN_H
#define PCADVIEWERMAIN_H

#include <filesystem>
#include "PCADViewerApp.h"
#include "ApertureFile.h"

//(*Headers(PCADViewerFrame)
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
//*)

class PCADViewerFrame: public wxFrame
{
    public:

        PCADViewerFrame(wxWindow* parent, wxWindowID id = -1);
        virtual ~PCADViewerFrame();
        bool DoGraphExporting(GraphExportContext& export_context);

    private:

        //(*Handlers(PCADViewerFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnScrolledCanvasPaint(wxPaintEvent& event);
        void OnMenuOpenSelected(wxCommandEvent& event);
        void OnMenuCloseSelected(wxCommandEvent& event);
        void OnMenuExportToTextSelected(wxCommandEvent& event);
        void OnScrolledCanvasResize(wxSizeEvent& event);
        void OnScrollBarHorPosScrollChanged(wxScrollEvent& event);
        void OnScrollBarVertPosScrollChanged(wxScrollEvent& event);
        void OnMenuEqualScaleXSelected(wxCommandEvent& event);
        void OnMenuEqualScaleYSelected(wxCommandEvent& event);
        void OnMenuScaleUpXSelected(wxCommandEvent& event);
        void OnMenuScaleDownXSelected(wxCommandEvent& event);
        void OnMenuScale1to1XSelected(wxCommandEvent& event);
        void OnMenuScaleUpYSelected(wxCommandEvent& event);
        void OnMenuScaleDownYSelected(wxCommandEvent& event);
        void OnMenuScale1to1YSelected(wxCommandEvent& event);
        void OnMenuScale1to1Selected(wxCommandEvent& event);
        void OnMenuFileInfoSelected(wxCommandEvent& event);
        void OnScrolledCanvasKeyDown(wxKeyEvent& event);
        void OnMenuItemCanvasA4Selected(wxCommandEvent& event);
        void OnMenuItemCanvasA3Selected(wxCommandEvent& event);
        void OnMenuItemCanvasA5Selected(wxCommandEvent& event);
        void OnMenuItemCanvasYesNoSelected(wxCommandEvent& event);
        void OnMenuItemCanvasPortraitSelected(wxCommandEvent& event);
        void OnMenuItemCanvasLandscapeSelected(wxCommandEvent& event);
        void OnMenuItemCanvasVarySelected(wxCommandEvent& event);
        void OnMenuItemSetCanvasSizeSelected(wxCommandEvent& event);
        void OnScrollBarHorCanvasPosScrollChanged(wxScrollEvent& event);
        void OnScrollBarVertCanvasPosScrollChanged(wxScrollEvent& event);
        void OnMenuItemExportBMPSelected(wxCommandEvent& event);
        void OnMenuItemExportJPEGSelected(wxCommandEvent& event);
        void OnMenuItemHelpIndexSelected(wxCommandEvent& event);
        void OnMenuItemSelectSelected(wxCommandEvent& event);
        void OnMenuItemInvertSelectionSelected(wxCommandEvent& event);
        void OnMenuItemUnselectSelected(wxCommandEvent& event);
        void OnMenuItemLayersSelected(wxCommandEvent& event);
        void OnScrolledCanvasMouseMove(wxMouseEvent& event);
        void OnScrolledCanvasLeftDown(wxMouseEvent& event);
        void OnMenuItemCanvasA6Selected(wxCommandEvent& event);
        void OnMenuItemCanvasA7Selected(wxCommandEvent& event);
        void OnMenuItemCanvasA8Selected(wxCommandEvent& event);
        void OnScrolledCanvasRightDown(wxMouseEvent& event);
        void OnScrolledCanvasMiddleDown(wxMouseEvent& event);
        void OnMenuItemDoCopyToClipboardSelected(wxCommandEvent& event);
        void OnMenuItemPrintSelected(wxCommandEvent& event);
        void OnToolBarCanvasYesNoClicked(wxCommandEvent& event);
        void OnToolBarItemEqualScalesClicked(wxCommandEvent& event);
        void OnComboBoxScaleXTextEnter(wxCommandEvent& event);
        void OnComboBoxScaleYTextEnter(wxCommandEvent& event);
        void OnToolBarItemExportClicked(wxCommandEvent& event);
        void OnToolBarItemPrintClicked(wxCommandEvent& event);
        void OnToolBarItemCanvasLandscapeClicked(wxCommandEvent& event);
        void OnToolBarItemMarkClicked(wxCommandEvent& event);
        void OnSpinCanvasSizeHorChange(wxSpinEvent& event);
        void OnSpinCanvasSizeVertChange(wxSpinEvent& event);
        void OnMenuItemExportSVGSelected(wxCommandEvent& event);
        void OnMenuItemExportDXFSelected(wxCommandEvent& event);
        void OnMenuItemExportWMFSelected(wxCommandEvent& event);
        void OnToolBarItemCopyClicked(wxCommandEvent& event);
        void OnMenuItemScaleUpBothSelected(wxCommandEvent& event);
        void OnMenuItemScaleDownBothSelected(wxCommandEvent& event);
        void OnMenuItemEqualScalesSelected(wxCommandEvent& event);
        void OnMenuItemDiffScalesSelected(wxCommandEvent& event);
        void OnMenuItemScaleArbitrarySelected(wxCommandEvent& event);
        void OnToolBarItemArbitraryScalesClicked(wxCommandEvent& event);
        void OnMenuItemNearestStdScaleBothSelected(wxCommandEvent& event);
        void OnMenuItemNearestStdScaleYSelected(wxCommandEvent& event);
        void OnMenuItemNearestStdScaleXSelected(wxCommandEvent& event);
        void OnMenuItemFontsSelected(wxCommandEvent& event);
        void OnMenuItemOptionsSelected(wxCommandEvent& event);
        void OnToolBarItemInvertMarkClicked(wxCommandEvent& event);
        void OnToolBarItemUnmarkClicked(wxCommandEvent& event);
        void OnScrolledCanvasLeftUp(wxMouseEvent& event);
        //*)

        wxString main_windows_name = _("ПиКАДоГляд - просмотрщик-конвертер PCAD-файлов");
        static constexpr double scale_value_array[] =
            {0.01, 0.0133, 0.02, 0.025, 0.04, 0.05, 0.0667, 0.1, 0.2, 0.25, 0.4, 0.5, 1,
             2, 2.5, 4, 5, 10, 20, 40, 50, 100};

        int save_mouse_xcoord, save_mouse_ycoord;
        bool is_event_handling = true;
        bool is_closing = false;
        // Дополнительные обработчики событий для управленцев, создающихся отдельно от wxSmith
        void OnMenuLoadAsItems(wxCommandEvent& event);
        void OnMenuChangeLanguage(wxCommandEvent& event);
        void OnSelectTimer(wxTimerEvent& event);
        void OnKillFocusComboBoxScaleX(wxFocusEvent& event);
        void OnKillFocusComboBoxScaleY(wxFocusEvent& event);
        // Вспомогательные методы класса для внутреннего употребления
        double CountPhysicalScale(bool is_count_scale_x, int scale_pos, int scale_range);
        int CountScrollScale(bool is_count_scale_x, double physical_scale);
        int FindScaleStage(double physical_scale);
        void SetBothScales(double new_scale_x, double new_scale_y);
        void SetScaleX(double new_scale_x);
        void SetScaleY(double new_scale_y);
        void SetEqualScalesStatus(bool new_equal_scale_status);
        void SetContextCanvasSize(CanvasSizeCode canvas_size_code);
        void SwitchCanvasYesNo(bool is_yes);
        void InitOpenedFile();
        const GraphObj* RecognizeGraphObj(int coord_x, int coord_y);
        bool LoadPCADFileAllWorkshops(const wxString& pict_filename);
        double CountRatioFromScroll(wxScrollBar* scroll_bar_ptr);
        void SetScrollByRatio(wxScrollBar* scroll_bar_ptr, double scroll_ratio);
        void SetCanvasSpinSizeRange();
        void ChangeCanvasSize(int new_cnv_width, int new_cnv_height);
        void ExportErrorMessageBox(ExportErrorType export_error_type);
        wxString SaveExportDialog(const wxString& head_file_type, const wxString& list_file_type,
                                  const wxString& file_ext);
        std::pair<double, double> CountSubScales(wxDC* dc_ptr);
        SizePositionData CountSizePosition();
        void SetThumbsSizes(const SizePositionData& old_pd);
        void SetStatusText(WhatStatusType what_status, const SizePositionData* pd_ptr = nullptr,
                           const GraphObj* graph_obj_ptr = nullptr, const wxPoint* src_point_ptr = nullptr);
        void DoSelectObjectsOp(GraphExportContext export_context, SelectOpType select_op);
        //--------------------
        static const long SELECT_TIMER_ID;

        wxTimer select_timer;

        //(*Identifiers(PCADViewerFrame)
        static const long ID_PANEL1;
        static const long ID_SCROLLBAR3;
        static const long ID_SCROLLBAR_HOR_CANVAS;
        static const long ID_SCROLLBAR1;
        static const long ID_SCROLLBAR_VERT_CANVAS;
        static const long ID_STATUSBAR1;
        static const long ToolOpenFile;
        static const long ToolCloseFile;
        static const long ToolExit;
        static const long ToolExport;
        static const long ToolPrint;
        static const long ToolFileInfo;
        static const long ToolMark;
        static const long ToolInvertMark;
        static const long ToolUnmarkAll;
        static const long ToolCopy;
        static const long ToolScalDownX;
        static const long ID_COMBOBOX_SCALE_X;
        static const long ToolScaleUpX;
        static const long ToolOneToOneX;
        static const long ToolEqualScales;
        static const long ToolScalDownY;
        static const long ID_COMBOBOX_SCALE_Y;
        static const long ToolScaleUpY;
        static const long ToolOneToOneY;
        static const long ID_SPINCTRL_CANVAS_HOR;
        static const long ID_SPINCTRL_CANVAS_VERT;
        static const long ToolCanvasYesNo;
        static const long ToolCanvasLandscape;
        static const long ToolHelp;
        static const long ID_TOOLBAR_MAIN;
        static const long IdMenuLoad;
        static const long IdMenuLoadAs;
        static const long IdMenuClose;
        static const long IdMenuExportCanvas;
        static const long IdMenuExportSelected;
        static const long IdMenuExportContour;
        static const long IdMenuExportText;
        static const long IdMenuExportDXF;
        static const long IdMenuExportSVG;
        static const long IdMenuExportWMF;
        static const long IdMenuExportBMP;
        static const long IdMenuExportJPG;
        static const long ID_MENUITEM_EXPORT;
        static const long IdMenuFileInfo;
        static const long IdMenuPrintCanvas;
        static const long IdMenuPrintSelected;
        static const long IdMenuPrintContour;
        static const long IdMenuPrint;
        static const long ID_MENUITEM_PRINT;
        static const long IdMenuExitProgram;
        static const long IdMenuDoCopyToClipboardAuto;
        static const long IdMenuHandleCanvas;
        static const long IdMenuHandleSelected;
        static const long IdMenuHandleContour;
        static const long IdMenuSelect;
        static const long IdMenuInvertSelection;
        static const long IdMenuUnselect;
        static const long IdMenuDoCopyToClipboardSpec;
        static const long IdMenuScaleMode;
        static const long IdMenuItemEqualScales;
        static const long IdMenuItemDiffScales;
        static const long IdMenuScaleUpX;
        static const long IdMenuScaleDownX;
        static const long IdMenuScaleOneX;
        static const long IdMenuNearestStdScaleX;
        static const long IdMenuScaleUpY;
        static const long IdMenuScaleDownY;
        static const long IdMenuScaleOneY;
        static const long IdMenuNearestStdScaleY;
        static const long IdMenuScaleUpBoth;
        static const long IdMenuScaleDownBoth;
        static const long IdMenuScaleOneBoth;
        static const long IdMenuNearestStdScaleXY;
        static const long IdMenuScaleEqualX;
        static const long IdMenuScaleEqualY;
        static const long IdMenuScale;
        static const long IdMenuLayers;
        static const long IdMenuFonts;
        static const long IdMenuLanguages;
        static const long IdMenuCanvasA3;
        static const long IdMenuCanvasA4;
        static const long IdMenuCanvasA5;
        static const long IdMenuCanvasA6;
        static const long IdMenuCanvasA7;
        static const long IdMenuCanvasA8;
        static const long IdMenuCanvasVary;
        static const long IdMenuStdSizes;
        static const long IdMenuCanvasPortrait;
        static const long IdMenuCanvasLandscape;
        static const long IdMenuCanvasYesNo;
        static const long IdMenuSetConvasSize;
        static const long IdMenuOptions;
        static const long IdMenuHelpIndex;
        static const long IdMenuAbout;
        //*)

        //(*Declarations(PCADViewerFrame)
        wxBoxSizer* BoxSizer1;
        wxBoxSizer* BoxSizer2;
        wxComboBox* ComboBoxScaleX;
        wxComboBox* ComboBoxScaleY;
        wxMenu* MenuCanvas;
        wxMenu* MenuEdit;
        wxMenu* MenuFile;
        wxMenu* MenuHelp;
        wxMenu* MenuItemExportHead;
        wxMenu* MenuItemLoadAs;
        wxMenu* MenuItemPrintHead;
        wxMenu* MenuItemStdSizes;
        wxMenu* MenuLanguages;
        wxMenu* MenuParams;
        wxMenu* MenuScale;
        wxMenu* MenuView;
        wxMenuBar* MenuBar1;
        wxMenuItem* MenuItem10;
        wxMenuItem* MenuItem1;
        wxMenuItem* MenuItem21;
        wxMenuItem* MenuItem2;
        wxMenuItem* MenuItem8;
        wxMenuItem* MenuItem9;
        wxMenuItem* MenuItemCanvasA3;
        wxMenuItem* MenuItemCanvasA4;
        wxMenuItem* MenuItemCanvasA5;
        wxMenuItem* MenuItemCanvasA6;
        wxMenuItem* MenuItemCanvasA7;
        wxMenuItem* MenuItemCanvasA8;
        wxMenuItem* MenuItemCanvasLandscape;
        wxMenuItem* MenuItemCanvasPortrait;
        wxMenuItem* MenuItemCanvasVary;
        wxMenuItem* MenuItemCanvasYesNo;
        wxMenuItem* MenuItemClose;
        wxMenuItem* MenuItemDiffScales;
        wxMenuItem* MenuItemDoCopyToClipboard;
        wxMenuItem* MenuItemEqualScales;
        wxMenuItem* MenuItemExitProgram;
        wxMenuItem* MenuItemExportBMP;
        wxMenuItem* MenuItemExportCanvas;
        wxMenuItem* MenuItemExportContour;
        wxMenuItem* MenuItemExportDXF;
        wxMenuItem* MenuItemExportJPEG;
        wxMenuItem* MenuItemExportSVG;
        wxMenuItem* MenuItemExportSelected;
        wxMenuItem* MenuItemExportText;
        wxMenuItem* MenuItemExportWMF;
        wxMenuItem* MenuItemFileInfo;
        wxMenuItem* MenuItemFonts;
        wxMenuItem* MenuItemHandleCanvas;
        wxMenuItem* MenuItemHandleContour;
        wxMenuItem* MenuItemHandleSelected;
        wxMenuItem* MenuItemHelpIndex;
        wxMenuItem* MenuItemInvertSelection;
        wxMenuItem* MenuItemLayers;
        wxMenuItem* MenuItemNearestStdScaleBoth;
        wxMenuItem* MenuItemNearestStdScaleX;
        wxMenuItem* MenuItemNearestStdScaleY;
        wxMenuItem* MenuItemOptions;
        wxMenuItem* MenuItemPrint;
        wxMenuItem* MenuItemPrintCanvas;
        wxMenuItem* MenuItemPrintContour;
        wxMenuItem* MenuItemPrintSelected;
        wxMenuItem* MenuItemScaleDownX;
        wxMenuItem* MenuItemScaleDownY;
        wxMenuItem* MenuItemScaleEqualX;
        wxMenuItem* MenuItemScaleEqualY;
        wxMenuItem* MenuItemScaleMode;
        wxMenuItem* MenuItemScaleOneX;
        wxMenuItem* MenuItemScaleOneY;
        wxMenuItem* MenuItemScaleUpX;
        wxMenuItem* MenuItemScaleUpY;
        wxMenuItem* MenuItemSelect;
        wxMenuItem* MenuItemSetCanvasSize;
        wxMenuItem* MenuItemUnselect;
        wxPanel* ScrolledCanvas;
        wxScrollBar* ScrollBarHorCanvasPos;
        wxScrollBar* ScrollBarHorPos;
        wxScrollBar* ScrollBarVertCanvasPos;
        wxScrollBar* ScrollBarVertPos;
        wxSpinCtrl* SpinCanvasSizeHor;
        wxSpinCtrl* SpinCanvasSizeVert;
        wxStatusBar* StatusBar1;
        wxToolBar* MainToolBar;
        wxToolBarToolBase* ToolBarCanvasYesNo;
        wxToolBarToolBase* ToolBarItemCanvasLandscape;
        wxToolBarToolBase* ToolBarItemCloseFile;
        wxToolBarToolBase* ToolBarItemCopy;
        wxToolBarToolBase* ToolBarItemDownX;
        wxToolBarToolBase* ToolBarItemDownY;
        wxToolBarToolBase* ToolBarItemEqualScales;
        wxToolBarToolBase* ToolBarItemExit;
        wxToolBarToolBase* ToolBarItemExport;
        wxToolBarToolBase* ToolBarItemFileInfo;
        wxToolBarToolBase* ToolBarItemHelp;
        wxToolBarToolBase* ToolBarItemInvertMark;
        wxToolBarToolBase* ToolBarItemMark;
        wxToolBarToolBase* ToolBarItemOpenFile;
        wxToolBarToolBase* ToolBarItemPrint;
        wxToolBarToolBase* ToolBarItemScale1To1X;
        wxToolBarToolBase* ToolBarItemScale1To1Y;
        wxToolBarToolBase* ToolBarItemUnmark;
        wxToolBarToolBase* ToolBarItemUpX;
        wxToolBarToolBase* ToolBarItemUpY;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // PCADVIEWERMAIN_H
