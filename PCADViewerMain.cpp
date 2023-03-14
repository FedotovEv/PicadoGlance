
/***************************************************************
 * Name:      PCADViewerMain.cpp
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-01
 * Copyright: Dichrograph ()
 * License:   GPL-3.0-or-later
 **************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>
#include <clocale>
#include <chrono>
#include <fnmatch.h>

#include "wx_pch.h"
#include "ApertureFile.h"
#include "PCADViewDraw.h"
#include "PCADFile.h"
#include "PCADViewerApp.h"
#include "PCADViewerMain.h"
#include "PCADTextExport.h"
#include "version.h"
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/clipbrd.h>
#include <wx/toolbar.h>
#include <wx/utils.h>
#include <wx/fontdlg.h>
#include "DrawGraphicalEntities.h"
#include "FileInfoDialog.h"
#include "SetCanvasSize.h"
#include "LayersSettings.h"
#include "OptionsDialog.h"
#include "svg.h"

using namespace std;
using namespace std::filesystem;
using namespace aperture;
using namespace DrawEntities;

extern const StdWXObjects std_wx_objects;

//(*InternalHeaders(PCADViewerFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

const long PCADViewerFrame::SELECT_TIMER_ID = wxNewId();

//(*IdInit(PCADViewerFrame)
const long PCADViewerFrame::ID_PANEL1 = wxNewId();
const long PCADViewerFrame::ID_SCROLLBAR3 = wxNewId();
const long PCADViewerFrame::ID_SCROLLBAR_HOR_CANVAS = wxNewId();
const long PCADViewerFrame::ID_SCROLLBAR1 = wxNewId();
const long PCADViewerFrame::ID_SCROLLBAR_VERT_CANVAS = wxNewId();
const long PCADViewerFrame::ID_STATUSBAR1 = wxNewId();
const long PCADViewerFrame::ToolOpenFile = wxNewId();
const long PCADViewerFrame::ToolCloseFile = wxNewId();
const long PCADViewerFrame::ToolExit = wxNewId();
const long PCADViewerFrame::ToolExport = wxNewId();
const long PCADViewerFrame::ToolPrint = wxNewId();
const long PCADViewerFrame::ToolFileInfo = wxNewId();
const long PCADViewerFrame::ToolMark = wxNewId();
const long PCADViewerFrame::ToolInvertMark = wxNewId();
const long PCADViewerFrame::ToolUnmarkAll = wxNewId();
const long PCADViewerFrame::ToolCopy = wxNewId();
const long PCADViewerFrame::ToolScalDownX = wxNewId();
const long PCADViewerFrame::ID_COMBOBOX_SCALE_X = wxNewId();
const long PCADViewerFrame::ToolScaleUpX = wxNewId();
const long PCADViewerFrame::ToolOneToOneX = wxNewId();
const long PCADViewerFrame::ToolEqualScales = wxNewId();
const long PCADViewerFrame::ToolScalDownY = wxNewId();
const long PCADViewerFrame::ID_COMBOBOX_SCALE_Y = wxNewId();
const long PCADViewerFrame::ToolScaleUpY = wxNewId();
const long PCADViewerFrame::ToolOneToOneY = wxNewId();
const long PCADViewerFrame::ID_SPINCTRL_CANVAS_HOR = wxNewId();
const long PCADViewerFrame::ID_SPINCTRL_CANVAS_VERT = wxNewId();
const long PCADViewerFrame::ToolCanvasYesNo = wxNewId();
const long PCADViewerFrame::ToolCanvasLandscape = wxNewId();
const long PCADViewerFrame::ToolHelp = wxNewId();
const long PCADViewerFrame::ID_TOOLBAR_MAIN = wxNewId();
const long PCADViewerFrame::IdMenuLoad = wxNewId();
const long PCADViewerFrame::IdMenuLoadAs = wxNewId();
const long PCADViewerFrame::IdMenuClose = wxNewId();
const long PCADViewerFrame::IdMenuExportCanvas = wxNewId();
const long PCADViewerFrame::IdMenuExportSelected = wxNewId();
const long PCADViewerFrame::IdMenuExportContour = wxNewId();
const long PCADViewerFrame::IdMenuExportText = wxNewId();
const long PCADViewerFrame::IdMenuExportDXF = wxNewId();
const long PCADViewerFrame::IdMenuExportSVG = wxNewId();
const long PCADViewerFrame::IdMenuExportWMF = wxNewId();
const long PCADViewerFrame::IdMenuExportBMP = wxNewId();
const long PCADViewerFrame::IdMenuExportJPG = wxNewId();
const long PCADViewerFrame::ID_MENUITEM_EXPORT = wxNewId();
const long PCADViewerFrame::IdMenuFileInfo = wxNewId();
const long PCADViewerFrame::IdMenuPrintCanvas = wxNewId();
const long PCADViewerFrame::IdMenuPrintSelected = wxNewId();
const long PCADViewerFrame::IdMenuPrintContour = wxNewId();
const long PCADViewerFrame::IdMenuPrint = wxNewId();
const long PCADViewerFrame::ID_MENUITEM_PRINT = wxNewId();
const long PCADViewerFrame::IdMenuExitProgram = wxNewId();
const long PCADViewerFrame::IdMenuDoCopyToClipboardAuto = wxNewId();
const long PCADViewerFrame::IdMenuHandleCanvas = wxNewId();
const long PCADViewerFrame::IdMenuHandleSelected = wxNewId();
const long PCADViewerFrame::IdMenuHandleContour = wxNewId();
const long PCADViewerFrame::IdMenuSelect = wxNewId();
const long PCADViewerFrame::IdMenuInvertSelection = wxNewId();
const long PCADViewerFrame::IdMenuUnselect = wxNewId();
const long PCADViewerFrame::IdMenuDoCopyToClipboardSpec = wxNewId();
const long PCADViewerFrame::IdMenuScaleMode = wxNewId();
const long PCADViewerFrame::IdMenuItemEqualScales = wxNewId();
const long PCADViewerFrame::IdMenuItemDiffScales = wxNewId();
const long PCADViewerFrame::IdMenuScaleUpX = wxNewId();
const long PCADViewerFrame::IdMenuScaleDownX = wxNewId();
const long PCADViewerFrame::IdMenuScaleOneX = wxNewId();
const long PCADViewerFrame::IdMenuNearestStdScaleX = wxNewId();
const long PCADViewerFrame::IdMenuScaleUpY = wxNewId();
const long PCADViewerFrame::IdMenuScaleDownY = wxNewId();
const long PCADViewerFrame::IdMenuScaleOneY = wxNewId();
const long PCADViewerFrame::IdMenuNearestStdScaleY = wxNewId();
const long PCADViewerFrame::IdMenuScaleUpBoth = wxNewId();
const long PCADViewerFrame::IdMenuScaleDownBoth = wxNewId();
const long PCADViewerFrame::IdMenuScaleOneBoth = wxNewId();
const long PCADViewerFrame::IdMenuNearestStdScaleXY = wxNewId();
const long PCADViewerFrame::IdMenuScaleEqualX = wxNewId();
const long PCADViewerFrame::IdMenuScaleEqualY = wxNewId();
const long PCADViewerFrame::IdMenuScale = wxNewId();
const long PCADViewerFrame::IdMenuLayers = wxNewId();
const long PCADViewerFrame::IdMenuFonts = wxNewId();
const long PCADViewerFrame::IdMenuLanguages = wxNewId();
const long PCADViewerFrame::IdMenuCanvasA3 = wxNewId();
const long PCADViewerFrame::IdMenuCanvasA4 = wxNewId();
const long PCADViewerFrame::IdMenuCanvasA5 = wxNewId();
const long PCADViewerFrame::IdMenuCanvasA6 = wxNewId();
const long PCADViewerFrame::IdMenuCanvasA7 = wxNewId();
const long PCADViewerFrame::IdMenuCanvasA8 = wxNewId();
const long PCADViewerFrame::IdMenuCanvasVary = wxNewId();
const long PCADViewerFrame::IdMenuStdSizes = wxNewId();
const long PCADViewerFrame::IdMenuCanvasFixRatio = wxNewId();
const long PCADViewerFrame::IdMenuCanvasMetric = wxNewId();
const long PCADViewerFrame::IdMenuCanvasPortrait = wxNewId();
const long PCADViewerFrame::IdMenuCanvasLandscape = wxNewId();
const long PCADViewerFrame::IdMenuCanvasYesNo = wxNewId();
const long PCADViewerFrame::IdMenuSetCanvasSize = wxNewId();
const long PCADViewerFrame::IdMenuOptions = wxNewId();
const long PCADViewerFrame::IdMenuHelpIndex = wxNewId();
const long PCADViewerFrame::IdMenuAbout = wxNewId();
//*)

BEGIN_EVENT_TABLE(PCADViewerFrame,wxFrame)
    //(*EventTable(PCADViewerFrame)
    //*)
    EVT_TIMER(SELECT_TIMER_ID, PCADViewerFrame::OnSelectTimer)
END_EVENT_TABLE()

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

PCADViewerFrame::PCADViewerFrame(wxWindow* parent, wxWindowID id) : select_timer(this, SELECT_TIMER_ID)
{
    //(*Initialize(PCADViewerFrame)
    Create(parent, wxID_ANY, _("Просмотрщик-конвертер PCAD-файлов"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxFULL_REPAINT_ON_RESIZE, _T("wxID_ANY"));
    SetClientSize(wxSize(380,214));
    SetMinSize(wxSize(50,50));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    ScrolledCanvas = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, _T("ID_PANEL1"));
    BoxSizer2->Add(ScrolledCanvas, 10, wxALL|wxEXPAND, 0);
    ScrollBarHorPos = new wxScrollBar(this, ID_SCROLLBAR3, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL, wxDefaultValidator, _T("ID_SCROLLBAR3"));
    ScrollBarHorPos->SetScrollbar(0, 1, 10, 1);
    BoxSizer2->Add(ScrollBarHorPos, 0, wxALL|wxEXPAND, 0);
    ScrollBarHorCanvasPos = new wxScrollBar(this, ID_SCROLLBAR_HOR_CANVAS, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL, wxDefaultValidator, _T("ID_SCROLLBAR_HOR_CANVAS"));
    ScrollBarHorCanvasPos->SetScrollbar(0, 1, 10, 1);
    BoxSizer2->Add(ScrollBarHorCanvasPos, 0, wxALL|wxEXPAND, 0);
    BoxSizer1->Add(BoxSizer2, 10, wxALL|wxEXPAND, 0);
    ScrollBarVertPos = new wxScrollBar(this, ID_SCROLLBAR1, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL, wxDefaultValidator, _T("ID_SCROLLBAR1"));
    ScrollBarVertPos->SetScrollbar(0, 1, 10, 1);
    BoxSizer1->Add(ScrollBarVertPos, 0, wxALL|wxEXPAND, 0);
    ScrollBarVertCanvasPos = new wxScrollBar(this, ID_SCROLLBAR_VERT_CANVAS, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL, wxDefaultValidator, _T("ID_SCROLLBAR_VERT_CANVAS"));
    ScrollBarVertCanvasPos->SetScrollbar(0, 1, 10, 1);
    BoxSizer1->Add(ScrollBarVertCanvasPos, 0, wxALL|wxEXPAND, 0);
    SetSizer(BoxSizer1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[4] = { -1, 180, 150, 100 };
    int __wxStatusBarStyles_1[4] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(4,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(4,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    MainToolBar = new wxToolBar(this, ID_TOOLBAR_MAIN, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL, _T("ID_TOOLBAR_MAIN"));
    ToolBarItemOpenFile = MainToolBar->AddTool(ToolOpenFile, _("Открыть изображение"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR), wxITEM_NORMAL, _("Открыть PCAD-файл"), _("Открыть PCAD-файл"));
    ToolBarItemCloseFile = MainToolBar->AddTool(ToolCloseFile, _("Закрыть изображение"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_ERROR")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_ERROR")),wxART_TOOLBAR), wxITEM_NORMAL, _("Закрыть текущий открытый файл"), _("Закрыть текущий открытый файл"));
    ToolBarItemExit = MainToolBar->AddTool(ToolExit, _("Выйти"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUIT")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUIT")),wxART_TOOLBAR), wxITEM_NORMAL, _("Выйти из программы"), _("Выйти из программы"));
    MainToolBar->AddSeparator();
    ToolBarItemExport = MainToolBar->AddTool(ToolExport, _("Экспорт рисунка"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXPORT_PICTURE")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXPORT_PICTURE")),wxART_TOOLBAR), wxITEM_NORMAL, _("Экспорт рисунка"), _("Преобразование рисунка в другие графические форматы"));
    ToolBarItemPrint = MainToolBar->AddTool(ToolPrint, _("Печать рисунка"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_PRINT")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_PRINT")),wxART_TOOLBAR), wxITEM_NORMAL, _("Печать рисунка"), _("Печать рисунка на принтере"));
    ToolBarItemFileInfo = MainToolBar->AddTool(ToolFileInfo, _("Информация о рисунке"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_INFORMATION")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_INFORMATION")),wxART_TOOLBAR), wxITEM_NORMAL, _("Информация о рисунке"), _("Информация о рисунке"));
    MainToolBar->AddSeparator();
    ToolBarItemMark = MainToolBar->AddTool(ToolMark, _("Пометить элементы"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_MARK_ELEMENTS")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_MARK_ELEMENTS")),wxART_TOOLBAR), wxITEM_NORMAL, _("Пометить элементы на холсте, внутри контура или все сразу"), _("Пометить элементы на холсте, внутри контура или все сразу"));
    ToolBarItemInvertMark = MainToolBar->AddTool(ToolInvertMark, _("Обратить выделение элементов"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_ADD_BOOKMARK")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_ADD_BOOKMARK")),wxART_TOOLBAR), wxITEM_NORMAL, _("Обратить выделение  элементов на холсте, внутри контура или все сразу"), _("Обратить выделение  элементов на холсте, внутри контура или все сразу"));
    ToolBarItemUnmark = MainToolBar->AddTool(ToolUnmarkAll, _("Снять отметки"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_UNMARK_ELEMENTS")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_UNMARK_ELEMENTS")),wxART_TOOLBAR), wxITEM_NORMAL, _("Снять все отметки"), _("Снять все отметки"));
    ToolBarItemCopy = MainToolBar->AddTool(ToolCopy, _("Копировать"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_COPY")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_COPY")),wxART_TOOLBAR), wxITEM_NORMAL, _("Копировать помеченные элементы, элементы на холсте или всё"), _("Копировать помеченные элементы, элементы на холсте или всё"));
    MainToolBar->AddSeparator();
    ToolBarItemDownX = MainToolBar->AddTool(ToolScalDownX, _("Уменьшить масштаб по X"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_MINUS")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_MINUS")),wxART_TOOLBAR), wxITEM_NORMAL, _("Уменьшить масштаб на ступень по оси X или изотропно"), _("Уменьшить масштаб на ступень по оси X или изотропно"));
    ComboBoxScaleX = new wxComboBox(MainToolBar, ID_COMBOBOX_SCALE_X, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(40,-1)), 0, 0, wxCB_DROPDOWN, wxDefaultValidator, _T("ID_COMBOBOX_SCALE_X"));
    MainToolBar->AddControl(ComboBoxScaleX);
    ToolBarItemUpX = MainToolBar->AddTool(ToolScaleUpX, _("Увеличить масштаб по X"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_PLUS")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_PLUS")),wxART_TOOLBAR), wxITEM_NORMAL, _("Увеличить масштаб на ступень по оси X или изотропно"), _("Увеличить масштаб на ступень по оси X или изотропно"));
    ToolBarItemScale1To1X = MainToolBar->AddTool(ToolOneToOneX, _("Масштаб 1:1 по X"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_ONE_TO_ONE")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_ONE_TO_ONE")),wxART_TOOLBAR), wxITEM_NORMAL, _("Установить масштаб 1:1 по оси X или по обоим осям"), _("Установить масштаб 1:1 по оси X или по обоим осям"));
    MainToolBar->AddSeparator();
    ToolBarItemEqualScales = MainToolBar->AddTool(ToolEqualScales, _("Равные"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EQUAL_SCALES")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EQUAL_SCALES")),wxART_TOOLBAR), wxITEM_CHECK, _("Включить/выключить режим равных масштабов"), _("Включить/выключить режим равных масштабов по обоим осям"));
    MainToolBar->AddSeparator();
    ToolBarItemDownY = MainToolBar->AddTool(ToolScalDownY, _("Уменьшить масштаб по Y"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_MINUS")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_MINUS")),wxART_TOOLBAR), wxITEM_NORMAL, _("Уменьшить масштаб на ступень по оси Y или изотропно"), _("Уменьшить масштаб на ступень по оси Y или изотропно"));
    ComboBoxScaleY = new wxComboBox(MainToolBar, ID_COMBOBOX_SCALE_Y, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(40,-1)), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX_SCALE_Y"));
    MainToolBar->AddControl(ComboBoxScaleY);
    ToolBarItemUpY = MainToolBar->AddTool(ToolScaleUpY, _("Увеличить масштаб по Y"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_PLUS")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_PLUS")),wxART_TOOLBAR), wxITEM_NORMAL, _("Увеличить масштаб на ступень по оси Y или изотропно"), _("Увеличить масштаб на ступень по оси Y или изотропно"));
    ToolBarItemScale1To1Y = MainToolBar->AddTool(ToolOneToOneY, _("Масштаб 1:1 по Y"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_ONE_TO_ONE")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_SCALE_ONE_TO_ONE")),wxART_TOOLBAR), wxITEM_NORMAL, _("Установить масштаб 1:1 по оси Y или по обоим осям"), _("Установить масштаб 1:1 по оси Y или по обоим осям"));
    MainToolBar->AddSeparator();
    SpinCanvasSizeHor = new wxSpinCtrl(MainToolBar, ID_SPINCTRL_CANVAS_HOR, _T("0"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(40,-1)), 0, 0, 100, 0, _T("ID_SPINCTRL_CANVAS_HOR"));
    SpinCanvasSizeHor->SetValue(_T("0"));
    MainToolBar->AddControl(SpinCanvasSizeHor);
    SpinCanvasSizeVert = new wxSpinCtrl(MainToolBar, ID_SPINCTRL_CANVAS_VERT, _T("0"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(40,-1)), 0, 0, 100, 0, _T("ID_SPINCTRL_CANVAS_VERT"));
    SpinCanvasSizeVert->SetValue(_T("0"));
    MainToolBar->AddControl(SpinCanvasSizeVert);
    MainToolBar->AddSeparator();
    ToolBarCanvasYesNo = MainToolBar->AddTool(ToolCanvasYesNo, _("Включить/Выключить холст"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_TOOLBAR), wxITEM_CHECK, _("Включить/выключить отображение холста"), _("Включить/выключить режим отображения холста"));
    ToolBarItemCanvasLandscape = MainToolBar->AddTool(ToolCanvasLandscape, _("Ландшафтная ориентация холста"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_PORTRAIT_LANDSCAPE")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_PORTRAIT_LANDSCAPE")),wxART_TOOLBAR), wxITEM_CHECK, _("Переключение между портретной и альбомной ориентацией холста"), _("Переключение между портретной и альбомной ориентацией холста"));
    MainToolBar->AddSeparator();
    ToolBarItemHelp = MainToolBar->AddTool(ToolHelp, _("Помощь"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_HELP")),wxART_TOOLBAR), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_HELP")),wxART_TOOLBAR), wxITEM_NORMAL, _("Вывод помощи по работе с программой"), _("Вывод помощи по работе с программой"));
    MainToolBar->Realize();
    SetToolBar(MainToolBar);
    MenuBar1 = new wxMenuBar();
    MenuFile = new wxMenu();
    MenuItem1 = new wxMenuItem(MenuFile, IdMenuLoad, _("Открыть\tCtrl-O"), _("Открыть PCAD-файл"), wxITEM_NORMAL);
    MenuFile->Append(MenuItem1);
    MenuItemLoadAs = new wxMenu();
    MenuItemLoadAs->AppendSeparator();
    MenuFile->Append(IdMenuLoadAs, _("Открыть как"), MenuItemLoadAs, _("Открыть файл конкретным загрузчиком"));
    MenuItemClose = new wxMenuItem(MenuFile, IdMenuClose, _("Закрыть\tCtrl-W"), _("Закрыть текущий файл"), wxITEM_NORMAL);
    MenuFile->Append(MenuItemClose);
    MenuFile->AppendSeparator();
    MenuItemExportHead = new wxMenu();
    MenuItemExportCanvas = new wxMenuItem(MenuItemExportHead, IdMenuExportCanvas, _("С холста"), _("Экспорт только содержимого холста"), wxITEM_CHECK);
    MenuItemExportHead->Append(MenuItemExportCanvas);
    MenuItemExportSelected = new wxMenuItem(MenuItemExportHead, IdMenuExportSelected, _("Отмеченное"), _("Экспорт только отмеченных элементов"), wxITEM_CHECK);
    MenuItemExportHead->Append(MenuItemExportSelected);
    MenuItemExportContour = new wxMenuItem(MenuItemExportHead, IdMenuExportContour, _("Внутри контура"), _("Экспорт только элементов внутри выделяющего контура"), wxITEM_CHECK);
    MenuItemExportHead->Append(MenuItemExportContour);
    MenuItemExportHead->AppendSeparator();
    MenuItemExportText = new wxMenuItem(MenuItemExportHead, IdMenuExportText, _("Текст"), _("Экспорт в текстовый файл"), wxITEM_NORMAL);
    MenuItemExportHead->Append(MenuItemExportText);
    MenuItemExportDXF = new wxMenuItem(MenuItemExportHead, IdMenuExportDXF, _("DXF"), _("Экспорт в формат DXF"), wxITEM_NORMAL);
    MenuItemExportHead->Append(MenuItemExportDXF);
    MenuItemExportSVG = new wxMenuItem(MenuItemExportHead, IdMenuExportSVG, _("SVG"), _("Экспорт в формат SVG"), wxITEM_NORMAL);
    MenuItemExportHead->Append(MenuItemExportSVG);
    MenuItemExportWMF = new wxMenuItem(MenuItemExportHead, IdMenuExportWMF, _("Metafile"), _("Экспорт в Windows Metafile"), wxITEM_NORMAL);
    MenuItemExportHead->Append(MenuItemExportWMF);
    MenuItemExportHead->AppendSeparator();
    MenuItemExportBMP = new wxMenuItem(MenuItemExportHead, IdMenuExportBMP, _("BMP"), _("Экспорт в формат BMP"), wxITEM_NORMAL);
    MenuItemExportHead->Append(MenuItemExportBMP);
    MenuItemExportJPEG = new wxMenuItem(MenuItemExportHead, IdMenuExportJPG, _("JPEG"), _("Экспорт в формат JPEG"), wxITEM_NORMAL);
    MenuItemExportHead->Append(MenuItemExportJPEG);
    MenuFile->Append(ID_MENUITEM_EXPORT, _("Экспорт"), MenuItemExportHead, wxEmptyString);
    MenuFile->AppendSeparator();
    MenuItemFileInfo = new wxMenuItem(MenuFile, IdMenuFileInfo, _("Информация о файле\tCtrl-I"), _("Вывод информации об открытом файле"), wxITEM_NORMAL);
    MenuFile->Append(MenuItemFileInfo);
    MenuItemPrintHead = new wxMenu();
    MenuItemPrintCanvas = new wxMenuItem(MenuItemPrintHead, IdMenuPrintCanvas, _("С холста"), _("Печать только содержимого холста"), wxITEM_CHECK);
    MenuItemPrintHead->Append(MenuItemPrintCanvas);
    MenuItemPrintSelected = new wxMenuItem(MenuItemPrintHead, IdMenuPrintSelected, _("Отмеченное"), _("Печать только отмеченных элементов"), wxITEM_CHECK);
    MenuItemPrintHead->Append(MenuItemPrintSelected);
    MenuItemPrintContour = new wxMenuItem(MenuItemPrintHead, IdMenuPrintContour, _("Внутри контура"), _("Печать только элементов внутри выделяющего контура"), wxITEM_CHECK);
    MenuItemPrintHead->Append(MenuItemPrintContour);
    MenuItemPrintHead->AppendSeparator();
    MenuItemPrint = new wxMenuItem(MenuItemPrintHead, IdMenuPrint, _("Печатать"), _("Предпросмотр результатов и выполнение печати"), wxITEM_NORMAL);
    MenuItemPrintHead->Append(MenuItemPrint);
    MenuFile->Append(ID_MENUITEM_PRINT, _("Печать"), MenuItemPrintHead, _("Печать изображения"));
    MenuFile->AppendSeparator();
    MenuItemExitProgram = new wxMenuItem(MenuFile, IdMenuExitProgram, _("Выйти\tAlt-F4"), _("Завершить программу"), wxITEM_NORMAL);
    MenuFile->Append(MenuItemExitProgram);
    MenuBar1->Append(MenuFile, _("Файл"));
    MenuEdit = new wxMenu();
    MenuItem2 = new wxMenuItem(MenuEdit, IdMenuDoCopyToClipboardAuto, _("Копировать\tCtrl-C"), _("Копирвать в буфер обмена выделенные элементы рисунка"), wxITEM_NORMAL);
    MenuEdit->Append(MenuItem2);
    MenuEdit->AppendSeparator();
    MenuItemHandleCanvas = new wxMenuItem(MenuEdit, IdMenuHandleCanvas, _("С холста"), _("Обрабатывать только содержимое холста"), wxITEM_CHECK);
    MenuEdit->Append(MenuItemHandleCanvas);
    MenuItemHandleSelected = new wxMenuItem(MenuEdit, IdMenuHandleSelected, _("Отмеченное"), _("Обрабатывать только выделенные элементы"), wxITEM_CHECK);
    MenuEdit->Append(MenuItemHandleSelected);
    MenuItemHandleContour = new wxMenuItem(MenuEdit, IdMenuHandleContour, _("Внутри контура"), _("Обрабатывать только элементы внутри выделяющего контура"), wxITEM_CHECK);
    MenuEdit->Append(MenuItemHandleContour);
    MenuEdit->AppendSeparator();
    MenuItemSelect = new wxMenuItem(MenuEdit, IdMenuSelect, _("Отметить указанное"), _("Отметить указанную группу примитивов"), wxITEM_NORMAL);
    MenuEdit->Append(MenuItemSelect);
    MenuItemInvertSelection = new wxMenuItem(MenuEdit, IdMenuInvertSelection, _("Инвертировать указанное"), _("Обратить выделение для выбранной группы примитивов рисунка"), wxITEM_NORMAL);
    MenuEdit->Append(MenuItemInvertSelection);
    MenuItemUnselect = new wxMenuItem(MenuEdit, IdMenuUnselect, _("Снять указанные отметки"), _("Снять отметки с выбранной группы примитивов"), wxITEM_NORMAL);
    MenuEdit->Append(MenuItemUnselect);
    MenuItemDoCopyToClipboard = new wxMenuItem(MenuEdit, IdMenuDoCopyToClipboardSpec, _("Копировать указанное\tShift-Ctrl-C"), _("Выполнить копирование указанных групп примитивов в буфер обмена"), wxITEM_NORMAL);
    MenuEdit->Append(MenuItemDoCopyToClipboard);
    MenuBar1->Append(MenuEdit, _("Правка"));
    MenuView = new wxMenu();
    MenuScale = new wxMenu();
    MenuItemScaleMode = new wxMenuItem(MenuScale, IdMenuScaleMode, _("Масштабы по осям"), _("Выбор соотношения масштабов по осям"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleMode);
    MenuItemScaleMode->Enable(false);
    MenuItemEqualScales = new wxMenuItem(MenuScale, IdMenuItemEqualScales, _("Равные"), _("Устанавливает режим равных масштабов по осям"), wxITEM_RADIO);
    MenuScale->Append(MenuItemEqualScales);
    MenuItemDiffScales = new wxMenuItem(MenuScale, IdMenuItemDiffScales, _("Различные"), _("Устанавливает режим различных масштабов по осям"), wxITEM_RADIO);
    MenuScale->Append(MenuItemDiffScales);
    MenuScale->AppendSeparator();
    MenuItemScaleUpX = new wxMenuItem(MenuScale, IdMenuScaleUpX, _("Увеличить по X\tCtrl-Alt-+"), _("Увеличить масштаб изображения по оси X"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleUpX);
    MenuItemScaleDownX = new wxMenuItem(MenuScale, IdMenuScaleDownX, _("Уменьшить по X\tCtrl-Alt--"), _("Уменьшить масштаб изображения по оси X"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleDownX);
    MenuItemScaleOneX = new wxMenuItem(MenuScale, IdMenuScaleOneX, _("Масштаб 1:1 по X\tCtrl-Alt-1"), _("Установить масштаб 1:1 по оси X"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleOneX);
    MenuItemNearestStdScaleX = new wxMenuItem(MenuScale, IdMenuNearestStdScaleX, _("Ближайший стандартный по X\tCtrl-Alt-N"), _("Установка ближайшего стандартного масштаба по оси X"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemNearestStdScaleX);
    MenuScale->AppendSeparator();
    MenuItemScaleUpY = new wxMenuItem(MenuScale, IdMenuScaleUpY, _("Увеличить по Y\tCtrl-Shift-+"), _("Увеличить масштаб изображения по оси Y"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleUpY);
    MenuItemScaleDownY = new wxMenuItem(MenuScale, IdMenuScaleDownY, _("Уменьшить по Y\tCtrl-Shift--"), _("Уменьшить масштаб изображения по оси Y"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleDownY);
    MenuItemScaleOneY = new wxMenuItem(MenuScale, IdMenuScaleOneY, _("Масштаб 1:1 по Y\tCtrl-Shift-1"), _("Установить масштаб 1:1 по оси Y"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleOneY);
    MenuItemNearestStdScaleY = new wxMenuItem(MenuScale, IdMenuNearestStdScaleY, _("Ближайший стандартный по Y\tCtrl-Shift-N"), _("Установка ближайшего стандартного масштаба по оси Y"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemNearestStdScaleY);
    MenuScale->AppendSeparator();
    MenuItem8 = new wxMenuItem(MenuScale, IdMenuScaleUpBoth, _("Увеличить по X и Y\tCtrl-+"), _("Увеличить масштаб изображения по обеим осям"), wxITEM_NORMAL);
    MenuScale->Append(MenuItem8);
    MenuItem9 = new wxMenuItem(MenuScale, IdMenuScaleDownBoth, _("Уменьшить по X и Y\tCtrl--"), _("Уменьшить масштаб изображения по обеим осям"), wxITEM_NORMAL);
    MenuScale->Append(MenuItem9);
    MenuItem10 = new wxMenuItem(MenuScale, IdMenuScaleOneBoth, _("Масштаб 1:1 по X и Y\tCtrl-1"), _("Установить масштаб 1:1 по обеим осям"), wxITEM_NORMAL);
    MenuScale->Append(MenuItem10);
    MenuItemNearestStdScaleBoth = new wxMenuItem(MenuScale, IdMenuNearestStdScaleXY, _("Ближайшие стандартные по X и Y\tCtrl-N"), _("Установка ближайшего стандартного масштаба по обеим осям"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemNearestStdScaleBoth);
    MenuScale->AppendSeparator();
    MenuItemScaleEqualX = new wxMenuItem(MenuScale, IdMenuScaleEqualX, _("Выровнять масштабы с опорой на X\tCtrl-Alt-X"), _("Устанавливает масштаб по оси Y аналогично оси X"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleEqualX);
    MenuItemScaleEqualY = new wxMenuItem(MenuScale, IdMenuScaleEqualY, _("Выровнять масштабы с опорой на Y\tCtrl-Alt-Y"), _("Устанавливает масштаб по оси X аналогично оси Y"), wxITEM_NORMAL);
    MenuScale->Append(MenuItemScaleEqualY);
    MenuView->Append(IdMenuScale, _("Масштаб"), MenuScale, _("Изменение масштаба изображения"));
    MenuItemLayers = new wxMenuItem(MenuView, IdMenuLayers, _("Слои"), _("Настройка цветовой палитры"), wxITEM_NORMAL);
    MenuView->Append(MenuItemLayers);
    MenuItemFonts = new wxMenuItem(MenuView, IdMenuFonts, _("Шрифты"), _("Выбор шрифта для вывода текстовой информации"), wxITEM_NORMAL);
    MenuView->Append(MenuItemFonts);
    MenuLanguages = new wxMenu();
    MenuLanguages->AppendSeparator();
    MenuView->Append(IdMenuLanguages, _("Доступные языки"), MenuLanguages, _("Выбор рабочего языка программы из списка доступных"));
    MenuBar1->Append(MenuView, _("Вид"));
    MenuCanvas = new wxMenu();
    MenuItemStdSizes = new wxMenu();
    MenuItemCanvasA3 = new wxMenuItem(MenuItemStdSizes, IdMenuCanvasA3, _("A3"), _("Холст формата A3"), wxITEM_NORMAL);
    MenuItemStdSizes->Append(MenuItemCanvasA3);
    MenuItemCanvasA4 = new wxMenuItem(MenuItemStdSizes, IdMenuCanvasA4, _("A4"), _("Холст формата A4"), wxITEM_NORMAL);
    MenuItemStdSizes->Append(MenuItemCanvasA4);
    MenuItemCanvasA5 = new wxMenuItem(MenuItemStdSizes, IdMenuCanvasA5, _("A5"), _("Холст формата A5"), wxITEM_NORMAL);
    MenuItemStdSizes->Append(MenuItemCanvasA5);
    MenuItemCanvasA6 = new wxMenuItem(MenuItemStdSizes, IdMenuCanvasA6, _("A6"), _("Холст формата A6"), wxITEM_NORMAL);
    MenuItemStdSizes->Append(MenuItemCanvasA6);
    MenuItemCanvasA7 = new wxMenuItem(MenuItemStdSizes, IdMenuCanvasA7, _("A7"), _("Холст формата A7"), wxITEM_NORMAL);
    MenuItemStdSizes->Append(MenuItemCanvasA7);
    MenuItemCanvasA8 = new wxMenuItem(MenuItemStdSizes, IdMenuCanvasA8, _("A8"), _("Холст формата A8"), wxITEM_NORMAL);
    MenuItemStdSizes->Append(MenuItemCanvasA8);
    MenuItemCanvasVary = new wxMenu();
    MenuItemCanvasVary->AppendSeparator();
    MenuItemStdSizes->Append(IdMenuCanvasVary, _("Пользовательские холсты"), MenuItemCanvasVary, _("Выбор холстов нестандартного размера"));
    MenuCanvas->Append(IdMenuStdSizes, _("Тип"), MenuItemStdSizes, wxEmptyString);
    MenuCanvas->AppendSeparator();
    MenuItemCanvasFixRatio = new wxMenuItem(MenuCanvas, IdMenuCanvasFixRatio, _("Фикс. пропорция"), _("Фиксация/расфиксация соотношения сторон холста"), wxITEM_CHECK);
    MenuCanvas->Append(MenuItemCanvasFixRatio);
    MenuItemCanvasMetric = new wxMenuItem(MenuCanvas, IdMenuCanvasMetric, _("Метрический"), _("Переключение типа единиц ихмерения размеров холста"), wxITEM_CHECK);
    MenuCanvas->Append(MenuItemCanvasMetric);
    MenuCanvas->AppendSeparator();
    MenuItemCanvasPortrait = new wxMenuItem(MenuCanvas, IdMenuCanvasPortrait, _("Портретный"), _("Установка портретной (вертикальной) ориентации холста"), wxITEM_RADIO);
    MenuCanvas->Append(MenuItemCanvasPortrait);
    MenuItemCanvasLandscape = new wxMenuItem(MenuCanvas, IdMenuCanvasLandscape, _("Ландшафтный"), _("Установка ландшафтной (горзонтальной) ориентации холста"), wxITEM_RADIO);
    MenuCanvas->Append(MenuItemCanvasLandscape);
    MenuCanvas->AppendSeparator();
    MenuItemCanvasYesNo = new wxMenuItem(MenuCanvas, IdMenuCanvasYesNo, _("Включить/Выключить"), _("Отключить вывод холста"), wxITEM_CHECK);
    MenuCanvas->Append(MenuItemCanvasYesNo);
    MenuItemSetCanvasSize = new wxMenuItem(MenuCanvas, IdMenuSetCanvasSize, _("Установка размера"), wxEmptyString, wxITEM_NORMAL);
    MenuCanvas->Append(MenuItemSetCanvasSize);
    MenuBar1->Append(MenuCanvas, _("Холст"));
    MenuParams = new wxMenu();
    MenuItemOptions = new wxMenuItem(MenuParams, IdMenuOptions, _("Параметры"), _("Установка некоторых параметров программы"), wxITEM_NORMAL);
    MenuParams->Append(MenuItemOptions);
    MenuBar1->Append(MenuParams, _("Настройка"));
    MenuHelp = new wxMenu();
    MenuItemHelpIndex = new wxMenuItem(MenuHelp, IdMenuHelpIndex, _("Индекс\tF1"), _("Главная страница системы помощи"), wxITEM_NORMAL);
    MenuHelp->Append(MenuItemHelpIndex);
    MenuItem21 = new wxMenuItem(MenuHelp, IdMenuAbout, _("О программе"), _("Вывод сведений о программе"), wxITEM_NORMAL);
    MenuHelp->Append(MenuItem21);
    MenuBar1->Append(MenuHelp, _("Помощь"));
    SetMenuBar(MenuBar1);
    SetSizer(BoxSizer1);
    Layout();

    ScrolledCanvas->Connect(wxEVT_PAINT,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasPaint,0,this);
    ScrolledCanvas->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasKeyDown,0,this);
    ScrolledCanvas->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasLeftDown,0,this);
    ScrolledCanvas->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasLeftUp,0,this);
    ScrolledCanvas->Connect(wxEVT_MIDDLE_DOWN,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasMiddleDown,0,this);
    ScrolledCanvas->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasRightDown,0,this);
    ScrolledCanvas->Connect(wxEVT_MOTION,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasMouseMove,0,this);
    ScrolledCanvas->Connect(wxEVT_SIZE,(wxObjectEventFunction)&PCADViewerFrame::OnScrolledCanvasResize,0,this);
    Connect(ID_SCROLLBAR3,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&PCADViewerFrame::OnScrollBarHorPosScrollChanged);
    Connect(ID_SCROLLBAR_HOR_CANVAS,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&PCADViewerFrame::OnScrollBarHorCanvasPosScrollChanged);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&PCADViewerFrame::OnScrollBarVertPosScrollChanged);
    Connect(ID_SCROLLBAR_VERT_CANVAS,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&PCADViewerFrame::OnScrollBarVertCanvasPosScrollChanged);
    Connect(ToolOpenFile,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuOpenSelected);
    Connect(ToolCloseFile,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuCloseSelected);
    Connect(ToolExit,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnQuit);
    Connect(ToolExport,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemExportClicked);
    Connect(ToolPrint,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemPrintClicked);
    Connect(ToolFileInfo,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuFileInfoSelected);
    Connect(ToolMark,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemMarkClicked);
    Connect(ToolInvertMark,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemInvertMarkClicked);
    Connect(ToolUnmarkAll,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemUnmarkClicked);
    Connect(ToolCopy,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemCopyClicked);
    Connect(ToolScalDownX,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleDownXSelected);
    Connect(ID_COMBOBOX_SCALE_X,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnComboBoxScaleXTextEnter);
    Connect(ID_COMBOBOX_SCALE_X,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&PCADViewerFrame::OnComboBoxScaleXTextEnter);
    Connect(ToolScaleUpX,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleUpXSelected);
    Connect(ToolOneToOneX,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScale1to1XSelected);
    Connect(ToolEqualScales,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemEqualScalesClicked);
    Connect(ToolScalDownY,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleDownYSelected);
    Connect(ID_COMBOBOX_SCALE_Y,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnComboBoxScaleYTextEnter);
    Connect(ID_COMBOBOX_SCALE_Y,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&PCADViewerFrame::OnComboBoxScaleYTextEnter);
    Connect(ToolScaleUpY,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleUpYSelected);
    Connect(ToolOneToOneY,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScale1to1YSelected);
    Connect(ID_SPINCTRL_CANVAS_HOR,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&PCADViewerFrame::OnSpinCanvasSizeHorChange);
    Connect(ID_SPINCTRL_CANVAS_VERT,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&PCADViewerFrame::OnSpinCanvasSizeVertChange);
    Connect(ToolCanvasYesNo,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarCanvasYesNoClicked);
    Connect(ToolCanvasLandscape,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemCanvasLandscapeClicked);
    Connect(ToolHelp,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemHelpIndexSelected);
    Connect(IdMenuLoad,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuOpenSelected);
    Connect(IdMenuClose,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuCloseSelected);
    Connect(IdMenuExportText,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuExportToTextSelected);
    Connect(IdMenuExportDXF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemExportDXFSelected);
    Connect(IdMenuExportSVG,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemExportSVGSelected);
    Connect(IdMenuExportWMF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemExportWMFSelected);
    Connect(IdMenuExportBMP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemExportBMPSelected);
    Connect(IdMenuExportJPG,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemExportJPEGSelected);
    Connect(IdMenuFileInfo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuFileInfoSelected);
    Connect(IdMenuPrint,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemPrintSelected);
    Connect(IdMenuExitProgram,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnQuit);
    Connect(IdMenuDoCopyToClipboardAuto,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnToolBarItemCopyClicked);
    Connect(IdMenuSelect,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemSelectSelected);
    Connect(IdMenuInvertSelection,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemInvertSelectionSelected);
    Connect(IdMenuUnselect,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemUnselectSelected);
    Connect(IdMenuDoCopyToClipboardSpec,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemDoCopyToClipboardSelected);
    Connect(IdMenuItemEqualScales,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemEqualScalesSelected);
    Connect(IdMenuItemDiffScales,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemDiffScalesSelected);
    Connect(IdMenuScaleUpX,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleUpXSelected);
    Connect(IdMenuScaleDownX,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleDownXSelected);
    Connect(IdMenuScaleOneX,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScale1to1XSelected);
    Connect(IdMenuNearestStdScaleX,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemNearestStdScaleXSelected);
    Connect(IdMenuScaleUpY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleUpYSelected);
    Connect(IdMenuScaleDownY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScaleDownYSelected);
    Connect(IdMenuScaleOneY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScale1to1YSelected);
    Connect(IdMenuNearestStdScaleY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemNearestStdScaleYSelected);
    Connect(IdMenuScaleUpBoth,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemScaleUpBothSelected);
    Connect(IdMenuScaleDownBoth,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemScaleDownBothSelected);
    Connect(IdMenuScaleOneBoth,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuScale1to1Selected);
    Connect(IdMenuNearestStdScaleXY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemNearestStdScaleBothSelected);
    Connect(IdMenuScaleEqualX,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuEqualScaleXSelected);
    Connect(IdMenuScaleEqualY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuEqualScaleYSelected);
    Connect(IdMenuLayers,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemLayersSelected);
    Connect(IdMenuFonts,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemFontsSelected);
    Connect(IdMenuCanvasA3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasA3Selected);
    Connect(IdMenuCanvasA4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasA4Selected);
    Connect(IdMenuCanvasA5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasA5Selected);
    Connect(IdMenuCanvasA6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasA6Selected);
    Connect(IdMenuCanvasA7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasA7Selected);
    Connect(IdMenuCanvasA8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasA8Selected);
    Connect(IdMenuCanvasFixRatio,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasFixRatioSelected);
    Connect(IdMenuCanvasMetric,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasMetricSelected);
    Connect(IdMenuCanvasPortrait,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasPortraitSelected);
    Connect(IdMenuCanvasLandscape,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasLandscapeSelected);
    Connect(IdMenuCanvasYesNo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasYesNoSelected);
    Connect(IdMenuSetCanvasSize,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemSetCanvasSizeSelected);
    Connect(IdMenuOptions,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemOptionsSelected);
    Connect(IdMenuHelpIndex,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnMenuItemHelpIndexSelected);
    Connect(IdMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PCADViewerFrame::OnAbout);
    //*)

    ComboBoxScaleX->Connect(wxEVT_KILL_FOCUS, (wxObjectEventFunction)&PCADViewerFrame::OnKillFocusComboBoxScaleX, 0, this);
    ComboBoxScaleY->Connect(wxEVT_KILL_FOCUS, (wxObjectEventFunction)&PCADViewerFrame::OnKillFocusComboBoxScaleY, 0, this);

    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->return_code = 0;
    vector<PCADViewerApp::MenuLoadAsItemDesc> menu_load_as_items;

    // Обходим все зарегистрированные цеха "загрузочной фабрики", создавая для каждого пункт в меню MenuItemLoadAs.
    for (FileWorkshop* file_workshop_ptr : this_app->file_factory)
    {
        PCADViewerApp::MenuLoadAsItemDesc new_memu_item;

        new_memu_item.file_workshop_ptr = file_workshop_ptr;
        new_memu_item.menu_item_name = file_workshop_ptr->GetFileWorkshopDescription();
        new_memu_item.menu_item_id = wxNewId();

        wxString wxmenu_item_name(new_memu_item.menu_item_name.c_str(), wxConvUTF8);
        wxString wxmenu_comment = _("Загрузить как ") + wxmenu_item_name;
        new_memu_item.menu_item_ptr =
            new wxMenuItem(MenuItemLoadAs, new_memu_item.menu_item_id, wxmenu_item_name,
                           wxmenu_comment, wxITEM_NORMAL);

        MenuItemLoadAs->Append(new_memu_item.menu_item_ptr);
        Connect(new_memu_item.menu_item_id, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction)&PCADViewerFrame::OnMenuLoadAsItems);

        this_app->menu_load_as_desc.push_back(move(new_memu_item));
    }

    for (double scan_scale : scale_value_array)
    { // Составляем перечни стандартных масштабов для выпадающих списков их установки
        ostringstream ostr;
        ostr << fixed << setprecision(3) << scan_scale;
        wxString wxstr_scale = wxString(ostr.str().c_str(), wxConvUTF8);
        ComboBoxScaleX->Append(wxstr_scale);
        ComboBoxScaleY->Append(wxstr_scale);
    }

    // Перечисляем все найденные в дереве каталогов доступные языки (переводы) для программы,
    // создавая для каждого пункт в меню MenuLanguages.
    wxMenuItem* selected_lang_menu_ptr = nullptr;
    for (LanguageDescriptType& lang_desc : this_app->languages_descs)
    {
        lang_desc.menu_item_id = wxNewId();

        wxString wxmenu_item_name(lang_desc.language_name.c_str(), wxConvUTF8);
        wxString wxmenu_comment = _("Выбор языка") + wxmenu_item_name;
        lang_desc.menu_item_ptr =
            new wxMenuItem(MenuLanguages, lang_desc.menu_item_id, wxmenu_item_name,
                           wxmenu_comment, wxITEM_RADIO);

        MenuLanguages->Append(lang_desc.menu_item_ptr);
        Connect(lang_desc.menu_item_id, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction)&PCADViewerFrame::OnMenuChangeLanguage);
        if (this_app->m_locale &&
            lang_desc.language_identifier == this_app->m_locale->GetLanguage())
            selected_lang_menu_ptr = lang_desc.menu_item_ptr;
    }
    if (selected_lang_menu_ptr)
        selected_lang_menu_ptr->Check();

    // Настройка полос прокрутки
    ScrollBarHorPos->SetScrollbar(0, 10, DEFAULT_SCROLL_RANGE_VALUE, 10);
    ScrollBarVertPos->SetScrollbar(0, 10, DEFAULT_SCROLL_RANGE_VALUE, 10);
    ScrollBarHorCanvasPos->SetScrollbar(0, 10, DEFAULT_SCROLL_RANGE_VALUE, 10);
    ScrollBarVertCanvasPos->SetScrollbar(0, 10, DEFAULT_SCROLL_RANGE_VALUE, 10);
    // Настроим органы управления состоянием холста
    this_app->vary_canvas_status.is_metric = true;
    this_app->vary_canvas_status.is_fixed_ratio = false;
    this_app->vary_canvas_status.canvas_size = wxSize(210, 297);
    this_app->canvas_context.is_portrait = true;
    this_app->canvas_context.is_metric = true;
    this_app->canvas_context.is_fixed_ratio = false;
    SetCanvasSpinSizeRange();
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A8);
    SwitchCanvasYesNo(false);
    MainToolBar->ToggleTool(ToolCanvasLandscape, !this_app->canvas_context.is_portrait);
    MenuItemCanvasFixRatio->Check(this_app->canvas_context.is_fixed_ratio);
    MenuItemCanvasMetric->Check(this_app->canvas_context.is_metric);
    MenuItemCanvasPortrait->Check(this_app->canvas_context.is_portrait);
    // Пределы установки масштаба
    this_app->min_scale_x = 0.1;
    this_app->max_scale_x = 10;
    this_app->min_scale_y = 0.1;
    this_app->max_scale_y = 10;
    // Создаём стартовую поверхность опознавания примитивов
    wxSizeEvent wxsz;
    OnScrolledCanvasResize(wxsz);
    //Настраиваем начальное состояние органов управления масштабом
    SetBothScales(1, 1);
    this_app->is_equal_scales = false;
    MainToolBar->ToggleTool(ToolEqualScales, this_app->is_equal_scales);
    if (this_app->is_equal_scales)
        MenuItemEqualScales->Check();
    else
        MenuItemDiffScales->Check();
    // Загрузим требуемый файл апертур, если его имя указано в командной строке
    if (this_app->options_data.aperture_filename.size())
    {
        ApertureLoadlInfo aperture_load_info;
        aperture_load_info.aper_file_type = ApertureFileType::APERTURE_FILE_AUTO;
        aperture_load_info.aperture_file_path = string(this_app->options_data.aperture_filename.mb_str());
        ifstream aper_stream(aperture_load_info.aperture_file_path, ios::binary);
        if (aper_stream)
        {
            aperture_load_info.aper_file_stream_ptr = &aper_stream;
            this_app->aperture_provider.LoadApertureFile(aperture_load_info);
        }
    }
    // Выберем указанное в командной строке (или по умолчанию, если не указано) рабочее множество засветок
    if (this_app->options_data.is_aperture_gerber_laser)
        this_app->aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER_LASER);
    else
        this_app->aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER32);
    //Загружаем файл, если его имя передано через командную строку
    if (this_app->options_data.picture_filename.size())
    {
        LoadPCADFileAllWorkshops(this_app->options_data.picture_filename);
    }
    else
    {
        wxCommandEvent wxcm;
        OnMenuCloseSelected(wxcm);
    }
}

PCADViewerFrame::~PCADViewerFrame()
{
    //(*Destroy(PCADViewerFrame)
    //*)

    is_closing = true;
}

void PCADViewerFrame::OnQuit(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->return_code = 0;
    Close();
}

void PCADViewerFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = _("ПиКАДоГляд - просмотрщик-преобразователь файлов PCAD для DOS.");
    msg += _("Обеспечивает просмотр, печать и конвертирование ряда форматов ");
    msg += _("графических файлов PCAD3 - PCAD8 в некоторые другие широко ");
    msg += _("распространённые типы растровых и векторных графических файлов.\n");
    msg += _("Федотов Евгений (fedotov_ev@rambler.ru)\n");
    msg += _("Версия ") + wxString(AutoVersion::FULLVERSION_STRING, wxConvUTF8);
    msg += _(", сборка ") + wxString(wxT(__DATE__));
    msg += _("\nhttp://www.github.com/FedotovEv/PicadoGlance");
    wxMessageBox(msg, _("Кто я??? Где я???"));
}

void PCADViewerFrame::SetBothScales(double new_scale_x, double new_scale_y)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SizePositionData old_pd = CountSizePosition();

    this_app->current_x_scale = new_scale_x;
    this_app->current_y_scale = new_scale_y;
    ostringstream ostr;
    ostr << fixed << setprecision(3) << new_scale_x;
    ComboBoxScaleX->SetValue(wxString(ostr.str().c_str(), wxConvUTF8));
    ostr.str(string());
    ostr << fixed << setprecision(3) << new_scale_y;
    ComboBoxScaleY->SetValue(wxString(ostr.str().c_str(), wxConvUTF8));
    SetThumbsSizes(old_pd);
    SetStatusText(WhatStatusType::SET_STATUS_WORKSCALE_INFO);
}

void PCADViewerFrame::SetScaleX(double new_scale_x)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SizePositionData old_pd = CountSizePosition();

    this_app->current_x_scale = new_scale_x;
    ostringstream ostr;
    ostr << fixed << setprecision(3) << new_scale_x;
    ComboBoxScaleX->SetValue(wxString(ostr.str().c_str(), wxConvUTF8));
    SetThumbsSizes(old_pd);
    SetStatusText(WhatStatusType::SET_STATUS_WORKSCALE_INFO);
}

void PCADViewerFrame::SetScaleY(double new_scale_y)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SizePositionData old_pd = CountSizePosition();

    this_app->current_y_scale = new_scale_y;
    ostringstream ostr;
    ostr << fixed << setprecision(3) << new_scale_y;
    ComboBoxScaleY->SetValue(wxString(ostr.str().c_str(), wxConvUTF8));
    SetThumbsSizes(old_pd);
    SetStatusText(WhatStatusType::SET_STATUS_WORKSCALE_INFO);
}

int PCADViewerFrame::FindScaleStage(double physical_scale)
{
    int select_scale_num = 0;
    double select_scale = scale_value_array[select_scale_num];

    for (int i = 0; i < static_cast<int>(ArraySize(scale_value_array)); ++i)
        if (abs(physical_scale - scale_value_array[i]) < abs(physical_scale - select_scale))
        {
            select_scale_num = i;
            select_scale = scale_value_array[select_scale_num];
        }
    return select_scale_num;
}

void PCADViewerFrame::SetEqualScalesStatus(bool new_equal_scale_status)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->is_equal_scales = new_equal_scale_status;
    MainToolBar->ToggleTool(ToolEqualScales, this_app->is_equal_scales);
    if (this_app->is_equal_scales)
        MenuItemEqualScales->Check(true);
    else
        MenuItemDiffScales->Check(true);
    SetStatusText(WhatStatusType::SET_STATUS_WORKSCALE_INFO);
}

double PCADViewerFrame::CountRatioFromScroll(wxScrollBar* scroll_bar_ptr)
{
    if (scroll_bar_ptr->GetRange() <= scroll_bar_ptr->GetThumbSize())
        return 0.0;
    else
        return static_cast<double>(scroll_bar_ptr->GetThumbPosition()) /
                                  (scroll_bar_ptr->GetRange() - scroll_bar_ptr->GetThumbSize());
}

void PCADViewerFrame::SetScrollByRatio(wxScrollBar* scroll_bar_ptr, double scroll_ratio)
{
    if (scroll_ratio < 0)
        scroll_ratio = 0;
    if (scroll_ratio > 1)
        scroll_ratio = 1;
    int max_thumb_position = scroll_bar_ptr->GetRange() - scroll_bar_ptr->GetThumbSize();
    scroll_bar_ptr->SetThumbPosition(round(scroll_ratio * max_thumb_position));
}

void PCADViewerFrame::OnScrolledCanvasPaint(wxPaintEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    DrawContext& use_context = this_app->last_draw_context;
    SizePositionData pd = CountSizePosition();

    wxPaintDC target_dc(ScrolledCanvas);
    use_context.ResetTargetDC(&target_dc, true);
    use_context.InitColorPenBrush();
    use_context.context_font = this_app->font_data.GetChosenFont();
    use_context.measure_unit_type = this_app->options_data.measure_unit_type;
    // И так, прямоугольник-источник pd.source_draw_part нужно спроецировать на target_dc.
    use_context.ResetProjectRect(pd.source_draw_part);
    // Прямоугольник холста в пространстве исходного изображения - pd.source_canvas.
    this_app->canvas_context.canvas_position = pd.source_canvas.GetPosition();

    SetStatusText(WhatStatusType::SET_STATUS_VIEW_AREA_INFO, &pd);

    wxRect update_rect = ScrolledCanvas->GetUpdateClientRect();
    use_context.clipping_rect = use_context.ConvertRectFromDevice(update_rect);
    this_app->pcad_file.DrawFile(use_context, this_app->canvas_context, this_app->select_contour);
}

void PCADViewerFrame::InitOpenedFile()
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();
    this_app->last_draw_context.pcad_doc_ptr = &this_app->pcad_file;

    SetStatusText(WhatStatusType::SET_STATUS_PICTURE_INFO);

    for (size_t i = 0; i < this_app->pcad_file.layers_size(); ++i)
        this_app->pcad_file.SetLayerAttributes(i, LayerAttributes{LAYER_ON_AVL_ACT_ON});
    SetLabel(main_windows_name + wxT(" - ") + this_app->pcad_file.GetPictureFileName());
    // Перевычисляем подмасштабы для вывода на экран
    wxSizeEvent wxsz;
    OnScrolledCanvasResize(wxsz);
    ScrolledCanvas->Refresh();
}

bool PCADViewerFrame::LoadPCADFileAllWorkshops(const wxString& pict_filename)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileWorkshop::LoadFileResult load_result{PCADFile(this_app->aperture_provider),
                                             PCADLoadError::LOAD_FILE_READ_ERROR};
    path picture_path = path(string(pict_filename.mb_str()));

    wxString wxerror_text = _("Ошибка при открытии файла ");
    wxString error_label = _("Ошибка");
    wxString summ_err_mess = _("Не удалось загрузить файл ");

    // Выделим имя файла картинки без маршрута. Оно понадобится нам при проверке его на соответствие маскам.
    string picture_filename_only = picture_path.filename().string();

    for (FileWorkshop* file_workshop_ptr : this_app->file_factory)
        for (const pair<string, string>& mask_pair : file_workshop_ptr->GetFileExtensions())
            if (!fnmatch(mask_pair.second.c_str(), picture_filename_only.c_str(),
                         FNM_NOESCAPE | FNM_PERIOD | FNM_CASEFOLD))
            {
                ifstream pict_stream(picture_path, ios::binary);
                if (!pict_stream)
                {
                    wxString picture_wxname(picture_path.string().c_str(), wxConvUTF8);
                    wxMessageBox(wxerror_text + picture_wxname, error_label);
                    return false;
                }

                FileWorkshop::AdditionalLoadInfo additional_info
                    {picture_path, this_app->aperture_provider, true};
                load_result = file_workshop_ptr->LoadPCADFile(pict_stream, additional_info);
                if (load_result.second == PCADLoadError::LOAD_FILE_NO_ERROR)
                    goto LoadPictureSuccessfully;
            }

LoadPictureSuccessfully:

    if (load_result.second != PCADLoadError::LOAD_FILE_NO_ERROR)
    {
        wxMessageBox(summ_err_mess + wxString(picture_path.string().c_str(), wxConvUTF8), error_label);
        return false;
    }
    else
    {
        this_app->pcad_file = move(load_result.first);
        InitOpenedFile();
        return true;
    }
}

void PCADViewerFrame::OnMenuOpenSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileWorkshop::LoadFileResult load_result{PCADFile(this_app->aperture_provider),
                                             PCADLoadError::LOAD_FILE_READ_ERROR};
    wxString summ_extensions;

    // Перебираем доступные "цеха загрузочной фабрики", для каждого получаем все поддерживаемые им типы
    // загружаемых файлов и формируем из них отдельную строку в соответствующем поле стандартного загрузочного диалога.
    for (FileWorkshop* file_workshop_ptr : this_app->file_factory)
        for (const pair<string, string>& mask_pair : file_workshop_ptr->GetFileExtensions())
        { // Значение, возвращаемое GetFileExtensions(), - вектор пар, в каждой из которых первое (.first) поле -
          // комментарий к маске, а второе поле (.second) - сама маска.
            wxString wx_comment(mask_pair.first.c_str(), wxConvUTF8);
            wxString wx_mask(mask_pair.second.c_str(), wxConvUTF8);
            if (summ_extensions.size())
                summ_extensions += wxT("|");
            summ_extensions += wx_comment + wxT("|") + wx_mask;
        }

    wxFileDialog load_file_dialog(this, _("Открыть файл изображения PCAD"), {}, {},
                    summ_extensions, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (load_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    LoadPCADFileAllWorkshops(load_file_dialog.GetPath());
}

void PCADViewerFrame::OnMenuChangeLanguage(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    for (LanguageDescriptType lang_desc : this_app->languages_descs)
    {
        if (lang_desc.menu_item_id == event.GetId())
        {
            if (!this_app->m_locale ||
                lang_desc.language_identifier != this_app->m_locale->GetLanguage())
            {
                delete this_app->m_locale;
                this_app->m_locale = nullptr;
                this_app->m_locale = new wxLocale(lang_desc.language_identifier);
                this_app->m_locale->AddCatalogLookupPathPrefix(wxPathOnly(this_app->argv[0]));
                this_app->m_locale->AddCatalog(this_app->GetAppName());
                this_app->return_code = -1;
                Close();
            }
            break;
        }
    }
}

void PCADViewerFrame::OnMenuLoadAsItems(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileWorkshop::LoadFileResult load_result{PCADFile(this_app->aperture_provider),
                                             PCADLoadError::LOAD_FILE_READ_ERROR};

    wxString wxerror_text = _("Ошибка при открытии файла ");
    wxString error_label = _("Ошибка");
    wxString vertical_bar = wxT("|");
    wxString summ_err_mess = _("Не удалось загрузить файл ");

    for (PCADViewerApp::MenuLoadAsItemDesc menu_item_desc : this_app->menu_load_as_desc)
    {
        if (menu_item_desc.menu_item_id == event.GetId())
        {
            // Будет возвращён массив пар, в каждой из которых первое (.first) поле - комментарий к маске,
            // а второе поле (.second) - сама маска.
            vector<pair<string, string>> mask_pair_vector = menu_item_desc.file_workshop_ptr->GetFileExtensions();
            wxString summ_extensions;
            for (const pair<string, string>& mask_info_pair : mask_pair_vector)
            {
                wxString pict_mask(mask_info_pair.second.c_str(), wxConvUTF8);
                wxString pict_comment(mask_info_pair.first.c_str(), wxConvUTF8);
                if (summ_extensions.size())
                    summ_extensions += vertical_bar;
                summ_extensions += pict_comment + vertical_bar + pict_mask;
            }
            summ_extensions += vertical_bar + _("Все файлы") + wxT("|*.*");

            wxFileDialog load_file_dialog(this, _("Открыть файл изображения PCAD"), wxString(), wxString(),
                            summ_extensions, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
            if (load_file_dialog.ShowModal() == wxID_CANCEL)
                return;

            wxString wxload_file_name = load_file_dialog.GetPath();
            path picture_path = path(string(wxload_file_name.mb_str()));

            ifstream pict_stream(picture_path, ios::binary);
            if (!pict_stream)
            {
                wxMessageBox(wxerror_text + wxload_file_name, error_label);
                return;
            }

            FileWorkshop::AdditionalLoadInfo additional_info
                {picture_path, this_app->aperture_provider, true};
            load_result = menu_item_desc.file_workshop_ptr->LoadPCADFile(pict_stream, additional_info);

            if (load_result.second != PCADLoadError::LOAD_FILE_NO_ERROR)
            {
                wxMessageBox(summ_err_mess + wxString(picture_path.string().c_str(), wxConvUTF8), error_label);
                return;
            }
            else
            {
                this_app->pcad_file = move(load_result.first);
                InitOpenedFile();
            }

            break;
        }
    }
}

void PCADViewerFrame::OnMenuCloseSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->pcad_file = PCADFile(this_app->aperture_provider);

    InitOpenedFile();

    SetLabel(main_windows_name);
    // Перевычисляем подмасштабы для вывода на экран
    wxSizeEvent wxsz;
    OnScrolledCanvasResize(wxsz);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuExportToTextSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;
    wxString export_wxfilename = SaveExportDialog(_("текстовый файл"), _("Текстовые файлы"), wxT("*.txt"));
    if (!export_wxfilename.size())
        return;

    string export_filename(export_wxfilename.mb_str());
    ofstream out_file(export_filename);
    if (!out_file)
    {
        ExportErrorMessageBox(ExportErrorType::EXPORT_CREATE_FILE_ERROR);
        return;
    }

    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();
    wxSize canvas_size = wxSize(this_app->canvas_context.canvas_size.GetWidth() / this_app->GetEffScaleX(),
                         this_app->canvas_context.canvas_size.GetHeight() / this_app->GetEffScaleY());
    wxRect canvas_rect = wxRect(this_app->canvas_context.canvas_position, canvas_size);

    DrawContext draw_context;
    draw_context.measure_unit_type = this_app->options_data.measure_unit_type;
    draw_context.text_export_style = TextExportStyleType::TEXT_EXPORT_PRECISION_STYLE;
    string picture_filename = this_app->pcad_file.GetPictureFilePath().filename().string();
    string zpt = " , ";
    out_file << "File : " << picture_filename << zpt << string(fdv.file_signature.mb_str()) << zpt;
    out_file << ConvertRectToString(fdv.frame_rect, draw_context) << endl;

    for (auto layers_it = this_app->pcad_file.layers_begin();
         layers_it != this_app->pcad_file.layers_end(); ++layers_it)
    {
        LayerDesc ld  = *layers_it;
        out_file << "Layer : " << "text(" << ld.layer_name << ')' << zpt;
        out_file << "number(" << ld.layer_number << ')' << zpt;
        out_file << ConvertColorToString(static_cast<unsigned char>(ld.layer_color)) << zpt;
        out_file << "attribute(" << ld.layer_attributes << ')' << endl;
    }

    const wxRect contour_copy(this_app->select_contour.contour_rect);
    for (GraphObj* graph_obj_ptr : this_app->pcad_file)
    {
        if (!out_file)
            break;
        const wxRect graph_obj_frame(graph_obj_ptr->GetFrameRect());
        int lay_num = graph_obj_ptr->GetLayerNumber();
        if (lay_num >= 0)
        {
            //Проверим активность слоя, которому принадлежит элемент graph_obj_ptr.
            if (lay_num >= static_cast<int>(this_app->pcad_file.layers_size()) ||
                this_app->pcad_file.GetLayerAttributes(lay_num).layer_on_avl_act == LAYER_ON_AVL_ACT_OFF)
                continue;
            // Проверяем пересечение описанного прямоугольника примитива с прямоугольником холста-форматки.
            if (MenuItemExportCanvas->IsChecked())
                if (graph_obj_frame.Intersect(canvas_rect).IsEmpty())
                    continue;
            // Отбрасываем невыделенные элементы, если нужно экспортировать только выделенные
            if (MenuItemExportSelected->IsChecked() && !graph_obj_ptr->GetGraphObjAttributes().is_selected)
                continue;
            // Проверяем перекрытие элемента и выделяющего контура, если такой режим включен
            if (MenuItemExportContour->IsChecked())
                if (contour_copy.Intersect(graph_obj_frame).IsEmpty())
                        continue;
        }
        out_file << graph_obj_ptr->GetObjectLongText(draw_context) << endl;
    }
    if (!out_file)
        ExportErrorMessageBox(ExportErrorType::EXPORT_WRITE_FILE_ERROR);
}

SizePositionData PCADViewerFrame::CountSizePosition()
{  //Функция определяет текущие параметры (положение и размер) рисуемого прямоугольника и холста-подложки в
   //пространстве (координатах) исходного изображения.
   //Рисуемый прямоугольник - часть исходного изображения, видимая в настоящий момент в просмотровой области -
   // - элементе управления ScrolledCanvas.
   //Параметры рассчитываются, исходя из текущих масщтабов по осям (возвращаются методами this_app->GetEffScaleX()
   //и this_app->GetEffScaleY()) и относительных положений рисуемого прямоугольника и холста, определяемых
   //парами полос прокрутки (ScrollBarHorPos, ScrollBarVertPos) для прямоугольника и
   //(ScrollBarHorCanvasPos, ScrollBarVertCanvasPos) для холста-подложки. Также для расчетов используются
   //"абсолютные" размеры региона просмотра ScrolledCanvas->GetClientSize() и холста-подложки
   //this_app->canvas_context.canvas_size.

    wxRect source_draw_part, source_canvas;
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();

    // Рассчитаем размер "источникового" рисуемого прямоугольника,
    // т. е. прямоугольника в пространстве исходного изображения.
    wxSize output_size = ScrolledCanvas->GetClientSize();
    source_draw_part.SetWidth(max(round(output_size.GetWidth() / this_app->GetEffScaleX()), 1.0));
    source_draw_part.SetHeight(max(round(output_size.GetHeight() / this_app->GetEffScaleY()), 1.0));
    // Размеры форматки-холста на плоскости исходного рисунка. Так как в пространстве целевого
    // изображения эти размеры не масштабируются (должны всегда оставаться равными указанным,
    // несмотря на текущий масштаб), то в плоскости исходного рисунка размер холста будет вычисляться так:
    source_canvas.SetWidth(max(round(this_app->canvas_context.canvas_size.GetWidth() /
                           this_app->GetEffScaleX()), 1.0));
    source_canvas.SetHeight(max(round(this_app->canvas_context.canvas_size.GetHeight() /
                            this_app->GetEffScaleY()), 1.0));

    // Рассчитаем текущее положение левого верхнего угла рисуемого прямоугольника
    // и холста-подложки в системе координат исходного изображения.
    // Сначала вычисление координат рисуемого прямоугольника.
    double pos_x_ratio = CountRatioFromScroll(ScrollBarHorPos);
    double pos_y_ratio = CountRatioFromScroll(ScrollBarVertPos);
    source_draw_part.SetX(max(fdv.frame_rect.x + pos_x_ratio *
        (fdv.frame_rect.width - source_draw_part.GetWidth() - 1), static_cast<double>(fdv.frame_rect.x)));
    source_draw_part.SetY(max(fdv.frame_rect.y + pos_y_ratio *
        (fdv.frame_rect.height - source_draw_part.GetHeight() - 1), static_cast<double>(fdv.frame_rect.y)));

    // А теперь - вычисление координат холста-подложки.
    double cnv_pos_x_ratio = CountRatioFromScroll(ScrollBarHorCanvasPos);
    double cnv_pos_y_ratio = CountRatioFromScroll(ScrollBarVertCanvasPos);
    source_canvas.SetX(fdv.frame_rect.x - source_canvas.GetWidth() +
                       cnv_pos_x_ratio * (fdv.frame_rect.width + source_canvas.GetWidth()));
    source_canvas.SetY(fdv.frame_rect.y - source_canvas.GetHeight() +
                       cnv_pos_y_ratio * (fdv.frame_rect.height + source_canvas.GetHeight()));
    return {source_draw_part, source_canvas};
}

void PCADViewerFrame::SetThumbsSizes(const SizePositionData& old_pd)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();

    wxWindow* FocusedWindow = wxWindow::FindFocus();
    if (FocusedWindow == ScrollBarHorPos || FocusedWindow == ScrollBarVertPos ||
        FocusedWindow == ScrollBarHorCanvasPos || FocusedWindow == ScrollBarVertCanvasPos)
        ScrolledCanvas->SetFocus();

    SizePositionData new_pd = CountSizePosition();
    // Установим размеры лифтов полос прокрутки перемещения изображения
    double src_fract_x = static_cast<double>(new_pd.source_draw_part.GetWidth()) /
                         fdv.frame_rect.width;
    double src_fract_y = static_cast<double>(new_pd.source_draw_part.GetHeight()) /
                         fdv.frame_rect.height;
    ScrollBarHorPos->SetThumbSize(src_fract_x * ScrollBarHorPos->GetRange());
    ScrollBarVertPos->SetThumbSize(src_fract_y * ScrollBarVertPos->GetRange());
    if (MenuItemCanvasYesNo->IsChecked())
    {
        // Установим размеры лифтов полос прокрутки перемещения холста
        double cnv_fract_x = static_cast<double>(new_pd.source_canvas.GetWidth()) /
                             (fdv.frame_rect.width + new_pd.source_canvas.GetWidth());
        double cnv_fract_y = static_cast<double>(new_pd.source_canvas.GetHeight()) /
                             (fdv.frame_rect.height + new_pd.source_canvas.GetHeight());
        ScrollBarHorCanvasPos->SetThumbSize(cnv_fract_x * ScrollBarHorCanvasPos->GetRange());
        ScrollBarVertCanvasPos->SetThumbSize(cnv_fract_y * ScrollBarVertCanvasPos->GetRange());
        // Теперь следует переставить лифты полос прокрутки холста так, чтобы получаемые от них
        // координаты были равны исходным - (old_pd.source_canvas.GetX(), old_pd.source_canvas.GetY()).
        double cnv_pos_x_ratio =
            static_cast<double>(old_pd.source_canvas.GetX() - fdv.frame_rect.x + new_pd.source_canvas.GetWidth()) /
            (fdv.frame_rect.width + new_pd.source_canvas.GetWidth());
        double cnv_pos_y_ratio =
            static_cast<double>(old_pd.source_canvas.GetY() - fdv.frame_rect.y + new_pd.source_canvas.GetHeight()) /
            (fdv.frame_rect.height + new_pd.source_canvas.GetHeight());
        SetScrollByRatio(ScrollBarHorCanvasPos, cnv_pos_x_ratio);
        SetScrollByRatio(ScrollBarVertCanvasPos, cnv_pos_y_ratio);
    }
}

void PCADViewerFrame::OnScrolledCanvasResize(wxSizeEvent& event)
{
    if (is_closing)
        return;
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->last_draw_context.ResetRecognizeDC(ScrolledCanvas->GetSize());
    if (this_app->options_data.is_use_screen_subscale)
    {
        wxScreenDC scr_dc;
        auto subscale_pair = CountSubScales(&scr_dc);
        this_app->current_x_screen_subscale = subscale_pair.first;
        this_app->current_y_screen_subscale = subscale_pair.second;
    }
    else
    {
        this_app->current_x_screen_subscale = 1;
        this_app->current_y_screen_subscale = 1;
    }
    SetThumbsSizes(CountSizePosition());
}

void PCADViewerFrame::OnScrollBarHorPosScrollChanged(wxScrollEvent& event)
{
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnScrollBarVertPosScrollChanged(wxScrollEvent& event)
{
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuEqualScaleXSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SetEqualScalesStatus(true);

    SetBothScales(this_app->current_x_scale, this_app->current_x_scale);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuEqualScaleYSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SetEqualScalesStatus(true);

    SetBothScales(this_app->current_y_scale, this_app->current_y_scale);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScaleUpXSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    int current_scale_stage = FindScaleStage(this_app->current_x_scale);
    int next_scale_stage = min(static_cast<int>(ArraySize(scale_value_array) - 1),
                               current_scale_stage + 1);
    double new_scale_x_value = this_app->current_x_scale *
        scale_value_array[next_scale_stage] / scale_value_array[current_scale_stage];

    SetScaleX(new_scale_x_value);
    if (this_app->is_equal_scales)
        SetScaleY(new_scale_x_value);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScaleDownXSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    int current_scale_stage = FindScaleStage(this_app->current_x_scale);
    int prev_scale_stage = max(0, current_scale_stage - 1);
    double new_scale_x_value = this_app->current_x_scale *
        scale_value_array[prev_scale_stage] / scale_value_array[current_scale_stage];

    SetScaleX(new_scale_x_value);
    if (this_app->is_equal_scales)
        SetScaleY(new_scale_x_value);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScale1to1XSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    SetScaleX(1);
    if (this_app->is_equal_scales)
        SetScaleY(1);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScaleUpYSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    int current_scale_stage = FindScaleStage(this_app->current_y_scale);
    int next_scale_stage = min(static_cast<int>(ArraySize(scale_value_array) - 1),
                               current_scale_stage + 1);
    double scale_y_value = this_app->current_y_scale *
        scale_value_array[next_scale_stage] / scale_value_array[current_scale_stage];

    SetScaleY(scale_y_value);
    if (this_app->is_equal_scales)
        SetScaleX(scale_y_value);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScaleDownYSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    int current_scale_stage = FindScaleStage(this_app->current_y_scale);
    int prev_scale_stage = max(0, current_scale_stage - 1);
    double scale_y_value = this_app->current_y_scale *
        scale_value_array[prev_scale_stage] / scale_value_array[current_scale_stage];

    SetScaleY(scale_y_value);
    if (this_app->is_equal_scales)
        SetScaleX(scale_y_value);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScale1to1YSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    SetScaleY(1);
    if (this_app->is_equal_scales)
        SetScaleX(1);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuScale1to1Selected(wxCommandEvent& event)
{
    SetBothScales(1, 1);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuFileInfoSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();
    if (!fdv.file_workshop)
        return;

    FileInfoDialog file_info_dialog(this);
    file_info_dialog.FileNameText->SetValue(this_app->pcad_file.GetPictureFileName());
    file_info_dialog.FileTypeText->SetValue(wxString(fdv.file_signature.c_str(), wxConvUTF8));
    file_info_dialog.FileWorkshopText->SetValue(wxString(fdv.file_workshop->GetFileWorkshopDescription().c_str(), wxConvUTF8));

    file_info_dialog.DBUInMeasureUnitText->SetValue(wxString(to_string(fdv.DBU_in_measure_unit).c_str(), wxConvUTF8));
    if (fdv.file_flags & FILE_FLAG_EDITOR_PCCAPS)
        file_info_dialog.FileEditorText->SetValue(wxT("PCCAPS"));
    else
        file_info_dialog.FileEditorText->SetValue(wxT("PCCARDS/PCPLACE"));
    if (fdv.file_flags & FILE_FLAG_UNIT_INCHES)
        file_info_dialog.FileMeasureUnitText->SetValue(_("Дюйм"));
    else
        file_info_dialog.FileMeasureUnitText->SetValue(_("Миллиметр"));
    file_info_dialog.PictElemCountText->SetValue(wxString(to_string(this_app->pcad_file.size()).c_str(), wxConvUTF8));
    wxRect frc(fdv.frame_rect);
    string tx_rect = '(' + to_string(frc.x) + ',' + to_string(frc.y) + ") - ("s +
           to_string(frc.x + frc.width - 1) + ',' + to_string(frc.y + frc.height - 1) + ')';
    file_info_dialog.PictSizeText->SetValue(wxString(tx_rect.c_str(), wxConvUTF8));
    string dblpnt = " : ";
    file_info_dialog.LayerList->Clear();
    for (auto lay_desc_it = this_app->pcad_file.layers_begin();
         lay_desc_it != this_app->pcad_file.layers_end(); ++lay_desc_it)
    {
        LayerDesc ld = *lay_desc_it;
        string tx_ld = to_string(ld.layer_number) + dblpnt + ld.layer_name + dblpnt +
            to_string(ld.layer_color) + dblpnt + to_string(ld.layer_attributes);
        file_info_dialog.LayerList->Append(wxString(tx_ld.c_str(), wxConvUTF8));
    }

    file_info_dialog.ApertureFileNameText->SetValue(this_app->pcad_file.GetApertureFileName());
    file_info_dialog.FlashList->Clear();
    for (auto flash_desc_it = this_app->pcad_file.flashes_begin();
         flash_desc_it != this_app->pcad_file.flashes_end(); ++flash_desc_it)
    {
        FlashDesc fd = *flash_desc_it;
        string tx_fd = to_string(fd.flash_number) + dblpnt + fd.d_code + dblpnt +
                       to_string(fd.flash_size_mm) + dblpnt + to_string(fd.flash_size_inch) + dblpnt;
        switch (fd.flash_type)
        {
        case FlashType::FLASH_FLASHER:
            tx_fd += "FLASH"s;
            break;
        case FlashType::FLASH_LINER:
            tx_fd += "LINE"s;
            break;
        default:
            tx_fd += "UNKNOWN"s;
        }
        tx_fd += dblpnt;
        switch (fd.flash_shape)
        {
        case FlashShape::FLASH_ROUND:
            tx_fd += "ROUND"s;
            break;
        case FlashShape::FLASH_SQUARE:
            tx_fd += "SQUARE"s;
            break;
        default:
            tx_fd += "UNKNOWN"s;
        }
        file_info_dialog.FlashList->Append(wxString(tx_fd.c_str(), wxConvUTF8));
    }

    file_info_dialog.ShowModal();
}

void PCADViewerFrame::OnScrolledCanvasKeyDown(wxKeyEvent& event)
{
    int km = event.GetModifiers();
    // Переменные для отслеживания положения отображаемой области PCAD-картинки
    int pos_hor = ScrollBarHorPos->GetThumbPosition();
    int pos_vert = ScrollBarVertPos->GetThumbPosition();
    int range_hor = ScrollBarHorPos->GetRange();
    int range_vert = ScrollBarVertPos->GetRange();
    bool hor_changed = false, vert_changed = false;
    // Переменные для отслеживания положения холста-подложки
    int cnv_pos_hor = ScrollBarHorCanvasPos->GetThumbPosition();
    int cnv_pos_vert = ScrollBarVertCanvasPos->GetThumbPosition();
    int cnv_range_hor = ScrollBarHorCanvasPos->GetRange();
    int cnv_range_vert = ScrollBarVertCanvasPos->GetRange();
    bool cnv_hor_changed = false, cnv_vert_changed = false;
    // ----------------------

    if (km & wxMOD_CONTROL)
    {  // Нажата Ctrl - сдвигаем холст
        switch (event.GetKeyCode())
        {
        case WXK_LEFT:
            cnv_pos_hor -= cnv_range_hor / 100;
            cnv_hor_changed = true;
            break;
        case WXK_RIGHT:
            cnv_pos_hor += cnv_range_hor / 100;
            cnv_hor_changed = true;
            break;
        case WXK_UP:
            cnv_pos_vert -= range_vert / 100;
            cnv_vert_changed = true;
            break;
        case WXK_DOWN:
            cnv_pos_vert += range_vert / 100;
            cnv_vert_changed = true;
            break;
        case WXK_PAGEUP:
            if (km & wxMOD_SHIFT)
                cnv_pos_vert = 0;
            else
                cnv_pos_vert -= cnv_range_vert / 10;
            cnv_vert_changed = true;
            break;
        case WXK_PAGEDOWN:
            if (km & wxMOD_SHIFT)
                cnv_pos_vert = cnv_range_vert;
            else
                cnv_pos_vert += cnv_range_vert / 10;
            cnv_vert_changed = true;
            break;
        case WXK_END:
            if (km & wxMOD_SHIFT)
                cnv_pos_hor = cnv_range_hor;
            else
                cnv_pos_hor += cnv_range_hor / 10;
            cnv_hor_changed = true;
            break;
        case WXK_HOME:
            if (km & wxMOD_SHIFT)
                cnv_pos_hor = 0;
            else
                cnv_pos_hor -= cnv_range_hor / 10;
            cnv_hor_changed = true;
            break;
        }
    }
    else
    { // Нет модификаторов - двигаем саму картинку
        switch (event.GetKeyCode())
        {
        case WXK_LEFT:
            pos_hor -= range_hor / 100;
            hor_changed = true;
            break;
        case WXK_RIGHT:
            pos_hor += range_hor / 100;
            hor_changed = true;
            break;
        case WXK_UP:
            pos_vert -= range_vert / 100;
            vert_changed = true;
            break;
        case WXK_DOWN:
            pos_vert += range_vert / 100;
            vert_changed = true;
            break;
        case WXK_PAGEUP:
            if (km & wxMOD_SHIFT)
                pos_vert = 0;
            else
                pos_vert -= range_vert / 10;
            vert_changed = true;
            break;
        case WXK_PAGEDOWN:
            if (km & wxMOD_SHIFT)
                pos_vert = range_vert;
            else
                pos_vert += range_vert / 10;
            vert_changed = true;
            break;
        case WXK_END:
            if (km & wxMOD_SHIFT)
                pos_hor = range_hor;
            else
                pos_hor += range_hor / 10;
            hor_changed = true;
            break;
        case WXK_HOME:
            if (km & wxMOD_SHIFT)
                pos_hor = 0;
            else
                pos_hor -= range_hor / 10;
            hor_changed = true;
            break;
        }
    }

    if (hor_changed)
    {
        if (pos_hor < 0)
            pos_hor = 0;
        if (pos_hor >= range_hor)
            pos_hor = range_hor - 1;
        ScrollBarHorPos->SetThumbPosition(pos_hor);
    }
    if (vert_changed)
    {
        if (pos_vert < 0)
            pos_vert = 0;
        if (pos_vert >= range_vert)
            pos_vert = range_vert - 1;
        ScrollBarVertPos->SetThumbPosition(pos_vert);
    }

    if (cnv_hor_changed)
    {
        if (cnv_pos_hor < 0)
            cnv_pos_hor = 0;
        if (cnv_pos_hor >= cnv_range_hor)
            cnv_pos_hor = cnv_range_hor - 1;
        ScrollBarHorCanvasPos->SetThumbPosition(cnv_pos_hor);
    }
    if (cnv_vert_changed)
    {
        if (cnv_pos_vert < 0)
            cnv_pos_vert = 0;
        if (cnv_pos_vert >= cnv_range_vert)
            cnv_pos_vert = cnv_range_vert - 1;
        ScrollBarVertCanvasPos->SetThumbPosition(cnv_pos_vert);
    }

    if (hor_changed || vert_changed || cnv_hor_changed || cnv_vert_changed)
        ScrolledCanvas->Refresh();
    else
        event.Skip();
}

void PCADViewerFrame::SwitchCanvasYesNo(bool is_yes)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    MenuItemCanvasYesNo->Check(is_yes);
    MainToolBar->ToggleTool(ToolCanvasYesNo, is_yes);
    this_app->canvas_context.is_canvas = is_yes;
    if (is_yes)
    {
        ScrollBarHorCanvasPos->Enable();
        ScrollBarVertCanvasPos->Enable();
        SetThumbsSizes(CountSizePosition());
    }
    else
    {
        ScrollBarHorCanvasPos->Disable();
        ScrollBarVertCanvasPos->Disable();
    }
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::SetContextCanvasSize(CanvasSizeCode canvas_size_code)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->canvas_context.canvas_size_code = canvas_size_code;
    double cnv_width, cnv_height;
    if (canvas_size_code == CanvasSizeCode::CANVAS_SIZE_NOT_STANDART)
    { // Сейчас размеры холста внутри this_app->vary_canvas_status.canvas_size хранятся либо
      // в миллиметрах (в метрической системе), либо в сотых дюйма (в английской системе).
        cnv_width = this_app->vary_canvas_status.canvas_size.GetWidth();
        cnv_height = this_app->vary_canvas_status.canvas_size.GetHeight();
        if (!this_app->vary_canvas_status.is_metric)
        { // В английской системе размер холста возвращается редактором в сотых долях дюйма - точках.
            cnv_width /= POINTS_IN_MILLIMETER;
            cnv_height /= POINTS_IN_MILLIMETER;
        }
        this_app->canvas_context.is_fixed_ratio = this_app->vary_canvas_status.is_fixed_ratio;
        this_app->canvas_context.is_metric = this_app->vary_canvas_status.is_metric;
    }
    else
    {
        wxSize tmp_cnv_sz = std_wx_objects.GetStandartCanvasSize(canvas_size_code, true);
        cnv_width = tmp_cnv_sz.GetWidth();
        cnv_height = tmp_cnv_sz.GetHeight();
        this_app->canvas_context.is_fixed_ratio = true;
        this_app->canvas_context.is_metric = true;
    }
    this_app->canvas_context.canvas_ratio = cnv_width / cnv_height;

    MenuItemCanvasFixRatio->Check(this_app->canvas_context.is_fixed_ratio);
    MenuItemCanvasMetric->Check(this_app->canvas_context.is_metric);
    SetCanvasSpinSizeRange();
    // На вход функции ChangeCanvasSize новые размеры холста должны подаваться в DBU.
    ChangeCanvasSize(this_app->pcad_file.ConvertMMToDBU(cnv_width),
                     this_app->pcad_file.ConvertMMToDBU(cnv_height),
                     UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN |
                     UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN);
}

void PCADViewerFrame::OnMenuItemCanvasA4Selected(wxCommandEvent& event)
{
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A4);
}

void PCADViewerFrame::OnMenuItemCanvasA3Selected(wxCommandEvent& event)
{
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A3);
}

void PCADViewerFrame::OnMenuItemCanvasA5Selected(wxCommandEvent& event)
{
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A5);
}

void PCADViewerFrame::OnMenuItemCanvasYesNoSelected(wxCommandEvent& event)
{
    SwitchCanvasYesNo(MenuItemCanvasYesNo->IsChecked());
}

void PCADViewerFrame::OnMenuItemCanvasPortraitSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    if (!this_app->canvas_context.is_portrait)
    {
        MainToolBar->ToggleTool(ToolCanvasLandscape, false);
        this_app->canvas_context.is_portrait = true;
        ChangeCanvasSize(-1, -1,
                         UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN |
                         UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN);
    }
}

void PCADViewerFrame::OnMenuItemCanvasLandscapeSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    if (this_app->canvas_context.is_portrait)
    {
        MainToolBar->ToggleTool(ToolCanvasLandscape, true);
        this_app->canvas_context.is_portrait = false;
        ChangeCanvasSize(-1, -1,
                         UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN |
                         UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN);
    }
}

void PCADViewerFrame::OnMenuItemCanvasVarySelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    for (PCADViewerApp::MenuLoadAsItemDesc menu_item_desc :
         this_app->menu_vary_canvas_select_desc)
    {
        if (menu_item_desc.menu_item_id == event.GetId())
        { // Описатель сработавшего пункта меню найден - копируем соответствующий холст
          // в this_app->vary_canvas_status.
            this_app->vary_canvas_status = this_app->vary_canvas_list
                [reinterpret_cast<intptr_t>(menu_item_desc.file_workshop_ptr)];
            break;
        }
    }
    // Следующий вызов делает холст из this_app->vary_canvas_status текущим.
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_NOT_STANDART);
}

void PCADViewerFrame::OnMenuItemSetCanvasSizeSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SetCanvasSize set_canvas_size_dialog(this);

    if (set_canvas_size_dialog.ShowModal())
    {
        this_app->vary_canvas_list = set_canvas_size_dialog.canvas_list_;
        if (this_app->vary_canvas_list.size() > VARY_CANVASES_MAX_NAME_LEN)
            this_app->vary_canvas_list.resize(VARY_CANVASES_MAX_NAME_LEN);
        // Пересоздаём меню пользовательских (нестандартных) форматов холста
        MenuItemStdSizes->Destroy(IdMenuCanvasVary);
        MenuItemCanvasVary = new wxMenu();
        MenuItemCanvasVary->AppendSeparator();
        size_t i = 0;
        for (const VaryCanvasEditorStatus& current_canvas : this_app->vary_canvas_list)
        {
            PCADViewerApp::MenuLoadAsItemDesc new_memu_item;
            if (current_canvas.canvas_name.size())
                new_memu_item.menu_item_name = current_canvas.canvas_name;
            else
                new_memu_item.menu_item_name =
                    to_string(current_canvas.canvas_size.GetWidth()) + 'x' +
                    to_string(current_canvas.canvas_size.GetHeight());

            new_memu_item.menu_item_id = wxNewId();
            new_memu_item.file_workshop_ptr = reinterpret_cast<FileWorkshop*>(i++);
            wxString wxmenu_item_name(new_memu_item.menu_item_name.c_str(), wxConvLocal);
            wxString wxmenu_comment = _("Выбрать холст формата ") + wxmenu_item_name;
            new_memu_item.menu_item_ptr =
                new wxMenuItem(MenuItemLoadAs, new_memu_item.menu_item_id, wxmenu_item_name,
                               wxmenu_comment, wxITEM_NORMAL);
            // Очередной пункт меню выбора пользовательских холстов приготовлен - присоединим
            // его к меню MenuItemCanvasVary и добавим его в список menu_vary_canvas_select_desc.
            MenuItemCanvasVary->Append(new_memu_item.menu_item_ptr);
            Connect(new_memu_item.menu_item_id, wxEVT_COMMAND_MENU_SELECTED,
                    (wxObjectEventFunction)&PCADViewerFrame::OnMenuItemCanvasVarySelected);
            this_app->menu_vary_canvas_select_desc.push_back(move(new_memu_item));
        }
        // Новое меню MenuItemCanvasVary сформировано, привязываем его к вышележащему
        // меню MenuItemStdSizes.
        MenuItemStdSizes->Append(IdMenuCanvasVary, _("Пользовательские холсты"),
                                 MenuItemCanvasVary, _("Выбор холстов нестандартного размера"));
    }
}

void PCADViewerFrame::OnScrollBarHorCanvasPosScrollChanged(wxScrollEvent& event)
{
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnScrollBarVertCanvasPosScrollChanged(wxScrollEvent& event)
{
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::ExportErrorMessageBox(ExportErrorType export_error_type)
{
    wxString export_err_mess;
    switch (export_error_type)
    {
    case ExportErrorType::EXPORT_CREATE_FILE_ERROR:
        export_err_mess = _("Ошибка создания файла при экспорте изображения");
        break;
    case ExportErrorType::EXPORT_WRITE_FILE_ERROR:
        export_err_mess = _("Ошибка записи файла при экспорте изображения");
        break;
    case ExportErrorType::EXPORT_CATCH_CLIPBOARD_ERROR:
        export_err_mess = _("Не удалось захватить буфер обмена");
        break;
    default:
        return;
    }
    wxMessageBox(export_err_mess, _("Ошибка экспорта"));
}

wxString PCADViewerFrame::SaveExportDialog(const wxString& head_file_type,
                                           const wxString& list_file_type,
                                           const wxString& file_ext)
{
    wxString extension_line, file_ext_text;
    size_t op = 0;
    do
    {
        if (file_ext_text.size())
            file_ext_text += wxT(", ");
        size_t fp = file_ext.find(wxChar(';'), op);
        if (fp == wxString::npos)
            fp = file_ext.size();
        file_ext_text += file_ext.SubString(op, fp - 1).Trim().Trim(false);
        op = fp + 1;
    }
    while (op < file_ext.size());

    extension_line = list_file_type + wxT("(") + file_ext_text + wxT(")|") + file_ext;
    wxFileDialog save_file_dialog(this, _("Экспорт изображения в ") + head_file_type,
                                  wxEmptyString, wxEmptyString, extension_line,
                                  wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return {};
    else
        return save_file_dialog.GetPath();
}

pair<double, double> PCADViewerFrame::CountSubScales(wxDC* dc_ptr)
{ // Функция возвращает поправки к текущему масштабу, связанные с физическим размером
  // точек на поверхности wxDC.
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    wxSize dc_size(dc_ptr->GetSize());
    wxSize dc_size_mm(dc_ptr->GetSizeMM());
    double pict_size_x_mm = static_cast<double>(dc_size_mm.GetWidth()) / dc_size.GetWidth();
    double pict_size_y_mm = static_cast<double>(dc_size_mm.GetHeight()) / dc_size.GetHeight();
    double DBU_size_mm = this_app->pcad_file.ConvertDBUToMM(1); // Размер DBU в миллиметрах
    return {DBU_size_mm / pict_size_x_mm, DBU_size_mm / pict_size_y_mm};
}

bool PCADViewerFrame::DoGraphExporting(GraphExportContext& export_context)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return true;
    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();
    DrawContext draw_context;
    wxMemoryDC target_dc;
    wxBitmap use_bmp;
    svg::Document svg_document;
    wxSize export_size; // Размер (в точках) выходного, результирующего экспортного изображения.
    wxRect source_rect; // Прямоугольный фрагмент исходника, содержащий часть исходного изображения,
                        // которая будет отправлена на экспорт.
    // Рассчитаем параметры выходного (экспортируемого) мзображения
    if (export_context.is_canvas_export)
    { // В этом случае прямоугольник экспортируемого изображения совпадает с текущим очертанием холста.
        export_size = wxSize(this_app->canvas_context.canvas_size.GetWidth(),
                             this_app->canvas_context.canvas_size.GetHeight());
        wxSize source_size = wxSize(export_size.GetWidth() / this_app->GetEffScaleX(),
                                    export_size.GetHeight() / this_app->GetEffScaleY());
        source_rect = wxRect(this_app->canvas_context.canvas_position, source_size);
    }
    else
    { // Во всех остальных случаях размер экспортируемого изображения равен всему полю рисунка
        export_size = wxSize(fdv.frame_rect.width * this_app->GetEffScaleX(),
                             fdv.frame_rect.height * this_app->GetEffScaleY());
        source_rect = fdv.frame_rect;
    }
    // draw_context.clipping_rect - регион отсечения, который будет использоваться для удаления лишних
    // элементов как функциями типа DrawObject, так и будет установлен как регион отсечения для wxWidgets.
    draw_context.clipping_rect = source_rect;
    if (export_context.is_contour_export)
        draw_context.clipping_rect.Intersect(this_app->select_contour.contour_rect);

    draw_context.InitColorPenBrush();
    draw_context.context_font = this_app->font_data.GetChosenFont();
    draw_context.is_use_selected = false;
    // Совершаем предварительные предэкспортные действия, необходимые до рисовки экспортного изображения
    if (export_context.export_type == GraphExportType::GRAPH_EXPORT_PRINTER)
    {
        draw_context.ResetProjectRect(source_rect);
        draw_context.ResetTargetDC(export_context.draw_dc_ptr, false);
        // Рассчитаем поправки к текущему масштабу, связанные с тем, что физические размеры
        // точек на принтере отличаются от размера точек на экране.
        auto prn_subscale_pair = CountSubScales(export_context.draw_dc_ptr);
        wxSize mod_export_size(export_size.GetWidth() * prn_subscale_pair.first / this_app->current_x_screen_subscale,
                               export_size.GetHeight() * prn_subscale_pair.second / this_app->current_y_screen_subscale);
        draw_context.ResetTargetSize(mod_export_size);
        export_context.draw_dc_ptr->SetClippingRegion(draw_context.ConvertRectToDevice(draw_context.clipping_rect));
    }
    else if (export_context.export_type == GraphExportType::GRAPH_EXPORT_SVG)
    {
        draw_context.ResetProjectRect(source_rect);
        draw_context.ResetTargetSize(export_size);
        if (!export_context.svg_document_ptr)
            export_context.svg_document_ptr = &svg_document;
    }
    else
    {
        draw_context.ResetProjectRect(source_rect);
        use_bmp = wxBitmap(export_size.GetWidth(), export_size.GetHeight(), 32);
        target_dc.SelectObject(use_bmp);
        draw_context.ResetTargetDC(&target_dc, false);
        target_dc.SetClippingRegion(draw_context.ConvertRectToDevice(draw_context.clipping_rect));
    }

    // Строим экспортное изображение - выводим на поверхность *draw_context.target_dc_ptr или SVG-документ
    // *export_context.svg_document_ptr все подходящие примитивы.
    const wxRect clipping_copy(draw_context.clipping_rect);
    const wxRect project_copy(draw_context.project_rect);
    const wxRect contour_copy(this_app->select_contour.contour_rect);
    for (GraphObj* graph_obj_ptr : this_app->pcad_file)
    {
        const wxRect graph_obj_frame = graph_obj_ptr->GetFrameRect();
        const int lay_num = graph_obj_ptr->GetLayerNumber();
        if (lay_num >= 0)
        {
            //Проверим активность слоя, которому принадлежит элемент graph_obj_ptr.
            if (lay_num >= static_cast<int>(this_app->pcad_file.layers_size()) ||
                this_app->pcad_file.GetLayerAttributes(lay_num).layer_on_avl_act == LAYER_ON_AVL_ACT_OFF)
                continue;
            // Проверим нахождение примитива (хотя бы частично) в пределах прямоугольника проекции.
            if (project_copy.Intersect(graph_obj_frame).IsEmpty())
                continue;
            // Отбрасываем невыделенные элементы, если нужно экспортировать только выделенные
            if (export_context.is_selected_export && !graph_obj_ptr->GetGraphObjAttributes().is_selected)
                continue;
            // Проверим нахождение примитива (хотя бы частично) в пределах прямоугольника отсечения, если он установлен.
            if (!clipping_copy.IsEmpty() && clipping_copy.Intersect(graph_obj_frame).IsEmpty())
                continue;
            // Проверяем перекрытие элемента и выделяющего контура, если такой режим включен
            if (export_context.is_contour_export)
                if (contour_copy.Intersect(graph_obj_frame).IsEmpty())
                        continue;
        }
        // Наконец, отрисовываем все прошедшие проверки элементы
        if (export_context.export_type == GraphExportType::GRAPH_EXPORT_SVG)
            graph_obj_ptr->DrawObjectSVG(*export_context.svg_document_ptr, draw_context);
        else
            graph_obj_ptr->DrawObject(draw_context);
    }

    //Выполняем завершающие операции экспорта, следующие после построения (рисовки) экспортного изображения
    bool result = false;
    ofstream out_file;
    switch (export_context.export_type)
    {
    case GraphExportType::GRAPH_EXPORT_FILE:
        result = use_bmp.SaveFile(export_context.filename, export_context.bitmap_type);
        if (!result)
            ExportErrorMessageBox(ExportErrorType::EXPORT_CREATE_FILE_ERROR);
        break;
    case GraphExportType::GRAPH_EXPORT_CLIPBOARD:
        result = wxTheClipboard->Open();
        if (result)
        {
            wxTheClipboard->SetData(new wxBitmapDataObject(use_bmp));
            wxTheClipboard->Close();
        }
        else
        {
            ExportErrorMessageBox(ExportErrorType::EXPORT_CATCH_CLIPBOARD_ERROR);
        }
        break;
    case GraphExportType::GRAPH_EXPORT_SVG:
        out_file.open(export_context.filename);
        if (!(result = static_cast<bool>(out_file)))
        {
            ExportErrorMessageBox(ExportErrorType::EXPORT_CREATE_FILE_ERROR);
            break;
        }
        export_context.svg_document_ptr->Render(out_file);
        if (!out_file)
            ExportErrorMessageBox(ExportErrorType::EXPORT_WRITE_FILE_ERROR);
        break;
    default:
        break;
    }

    return result;
}

void PCADViewerFrame::OnMenuItemExportBMPSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;
    wxString export_filename = SaveExportDialog(_("BMP-файл"), _("Растровые графические BMP-файлы"), wxT("*.bmp"));
    if (!export_filename.size())
        return;

    GraphExportContext export_context;
    export_context.export_type = GraphExportType::GRAPH_EXPORT_FILE;
    export_context.filename = export_filename;
    export_context.bitmap_type = wxBITMAP_TYPE_BMP;
    export_context.is_canvas_export = MenuItemExportCanvas->IsChecked();
    export_context.is_selected_export = MenuItemExportSelected->IsChecked();
    export_context.is_contour_export = MenuItemExportContour->IsChecked();
    DoGraphExporting(export_context);
}

void PCADViewerFrame::OnMenuItemExportJPEGSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;
    wxString export_filename = SaveExportDialog(_("JPEG-файл"), _("Растровые графические JPEG-файлы"), wxT("*.jpg;*.jpeg"));
    if (!export_filename.size())
        return;

    GraphExportContext export_context;
    export_context.export_type = GraphExportType::GRAPH_EXPORT_FILE;
    export_context.filename = export_filename;
    export_context.bitmap_type = wxBITMAP_TYPE_JPEG;
    export_context.is_canvas_export = MenuItemExportCanvas->IsChecked();
    export_context.is_selected_export = MenuItemExportSelected->IsChecked();
    export_context.is_contour_export = MenuItemExportContour->IsChecked();
    DoGraphExporting(export_context);
}

void PCADViewerFrame::OnMenuItemHelpIndexSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->HtmlHelp.DisplayContents();
}

void PCADViewerFrame::OnMenuItemSelectSelected(wxCommandEvent& event)
{
    GraphExportContext export_context;

    export_context.is_canvas_export = MenuItemHandleCanvas->IsChecked();
    export_context.is_contour_export = MenuItemHandleContour->IsChecked();
    DoSelectObjectsOp(export_context, SelectOpType::SELECT_OP_SELECT);
}

void PCADViewerFrame::DoSelectObjectsOp(GraphExportContext export_context, SelectOpType select_op)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;

    wxSize dev_canvas_size = wxSize(this_app->canvas_context.canvas_size.GetWidth() / this_app->GetEffScaleX(),
                                    this_app->canvas_context.canvas_size.GetHeight() / this_app->GetEffScaleY());
    wxRect dev_canvas_rect = wxRect(this_app->canvas_context.canvas_position, dev_canvas_size);

    for (GraphObj* graph_obj_ptr : this_app->pcad_file)
    {
        const wxRect frame_rect(graph_obj_ptr->GetFrameRect());

        if (export_context.is_canvas_export)
            if (frame_rect.Intersect(dev_canvas_rect).IsEmpty())
                continue;
        if (export_context.is_contour_export && this_app->select_contour.is_contour_builded)
            if (frame_rect.Intersect(this_app->select_contour.contour_rect).IsEmpty())
                continue;
        switch (select_op)
        {
        case SelectOpType::SELECT_OP_SELECT:
            graph_obj_ptr->SetGraphObjAttributes({true});
            break;
        case SelectOpType::SELECT_OP_UNSELECT:
            graph_obj_ptr->SetGraphObjAttributes({false});
            break;
        case SelectOpType::SELECT_OP_INVERT:
            graph_obj_ptr->SetGraphObjAttributes({!graph_obj_ptr->GetGraphObjAttributes().is_selected});
            break;
        }
    }

    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuItemUnselectSelected(wxCommandEvent& event)
{
    GraphExportContext export_context;

    export_context.is_canvas_export = MenuItemHandleCanvas->IsChecked();
    export_context.is_contour_export = MenuItemHandleContour->IsChecked();
    DoSelectObjectsOp(export_context, SelectOpType::SELECT_OP_UNSELECT);
}

void PCADViewerFrame::OnMenuItemInvertSelectionSelected(wxCommandEvent& event)
{
    GraphExportContext export_context;

    export_context.is_canvas_export = MenuItemHandleCanvas->IsChecked();
    export_context.is_contour_export = MenuItemHandleContour->IsChecked();
    DoSelectObjectsOp(export_context, SelectOpType::SELECT_OP_INVERT);
}

void PCADViewerFrame::OnMenuItemLayersSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.layers_size())
    {
        wxMessageBox(_("Информация о слоях рисунка не найдена"), _("Ошибка"));
        return;
    }

    LayersSettings layer_settings_dialog(this);
    if (layer_settings_dialog.ShowModal())
        ScrolledCanvas->Refresh();
}

const GraphObj* PCADViewerFrame::RecognizeGraphObj(int coord_x, int coord_y)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    const GraphObj* graph_obj_ptr = nullptr;
    wxPoint src_point(this_app->last_draw_context.ConvertPointFromDevice(wxPoint(coord_x, coord_y)));
    wxColor pixel_col;

    if (this_app->pcad_file.size())
    {
        this_app->last_draw_context.recognize_dc.GetPixel(coord_x, coord_y, &pixel_col);
        if (!StdWXObjects::CompareColorLong(pixel_col, 0))
            graph_obj_ptr = this_app->pcad_file.ScanForGraphObject(pixel_col);
    }

    SetStatusText(WhatStatusType::SET_STATUS_CURSOR_INFO, nullptr, graph_obj_ptr, &src_point);
    return graph_obj_ptr;
}

void PCADViewerFrame::OnSelectTimer(wxTimerEvent& event)
{
    RecognizeGraphObj(save_mouse_xcoord, save_mouse_ycoord);
}

void PCADViewerFrame::OnScrolledCanvasMouseMove(wxMouseEvent& event)
{
    if (event.AltDown())
    {
        PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
        if (this_app->select_contour.contour_catch != ContourSideDetectType::CONTOUR_NOT_INTERSECT)
        {
            //Создадим временный DC для рисования на поверхности отображения рисунка (ScrolledCanvas)
            wxClientDC scrolled_canvas_dc(ScrolledCanvas);
            DrawContext use_context;
            use_context.ResetProjectRect(this_app->last_draw_context.project_rect);
            use_context.ResetTargetDC(&scrolled_canvas_dc, false);
            //Восстановим изображение под старым положением контура
            RestoreContourBackgroud(use_context, this_app->select_contour);

            //Рассчитаем новую конфигурацию выделяющего прямоугольника
            wxPoint source_mouse_point = use_context.ConvertPointFromDevice(wxPoint(event.GetX(), event.GetY()));
            wxPoint top_left(this_app->select_contour.contour_rect.GetTopLeft());
            wxPoint bottom_right(this_app->select_contour.contour_rect.GetBottomRight());
            switch (this_app->select_contour.contour_catch)
            {
            case ContourSideDetectType::CONTOUR_INTERSECT_LEFT:
                top_left = wxPoint(source_mouse_point.x, this_app->select_contour.contour_rect.GetTop());
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_RIGHT:
                bottom_right = wxPoint(source_mouse_point.x, this_app->select_contour.contour_rect.GetBottom());
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_TOP:
                top_left = wxPoint(this_app->select_contour.contour_rect.GetLeft(), source_mouse_point.y);
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_BOTTOM:
                bottom_right = wxPoint(this_app->select_contour.contour_rect.GetRight(), source_mouse_point.y);
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_LEFT_TOP:
                top_left = wxPoint(source_mouse_point.x, source_mouse_point.y);
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_LEFT_BOTTOM:
                top_left = wxPoint(source_mouse_point.x, this_app->select_contour.contour_rect.GetTop());
                bottom_right = wxPoint(this_app->select_contour.contour_rect.GetRight(), source_mouse_point.y);
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_TOP:
                top_left = wxPoint(this_app->select_contour.contour_rect.GetLeft(), source_mouse_point.y);
                bottom_right = wxPoint(source_mouse_point.x, this_app->select_contour.contour_rect.GetBottom());
                break;
            case ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_BOTTOM:
                bottom_right = wxPoint(source_mouse_point.x, source_mouse_point.y);
                break;
            default:
                break;
            }
            this_app->select_contour.contour_rect = wxRect(top_left, bottom_right);
            DrawSelectingContour(use_context, this_app->select_contour);
        }
    }
    else
    {
        select_timer.Stop();
        save_mouse_xcoord = event.GetX();
        save_mouse_ycoord = event.GetY();
        select_timer.Start(100, true);
    }
}

void PCADViewerFrame::OnScrolledCanvasLeftDown(wxMouseEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    //Создадим временный DC для рисования на поверхности отображения рисунка (ScrolledCanvas)
    wxClientDC scrolled_canvas_dc(ScrolledCanvas);
    DrawContext use_context;
    use_context.ResetProjectRect(this_app->last_draw_context.project_rect);
    use_context.ResetTargetDC(&scrolled_canvas_dc, false);

    ScrolledCanvas->SetFocus();
    if (event.AltDown())
    { // Нажата клавиша Alt - производится управление очертанием выбирающего контура
        wxPoint dev_mouse_point = wxPoint(event.GetX(), event.GetY());
        wxPoint source_mouse_point = use_context.ConvertPointFromDevice(dev_mouse_point);
        ContourSideDetectType contour_collide =
            IsPointOnContour(use_context, dev_mouse_point, this_app->select_contour);

        if (contour_collide == ContourSideDetectType::CONTOUR_NOT_INTERSECT)
        {
            RestoreContourBackgroud(use_context, this_app->select_contour);
            this_app->select_contour.contour_rect = wxRect(source_mouse_point, wxSize(0, 0));
            this_app->select_contour.contour_catch = ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_BOTTOM;
            DrawSelectingContour(use_context, this_app->select_contour);
        }
        else
        {
            this_app->select_contour.contour_catch = contour_collide;
        }
    }
    else
    { // Щелчок левой кнопкой мыши без модификаторов - изменение состояния выбранности отдельных примитивов рисунка
        const GraphObj* graph_obj_ptr = RecognizeGraphObj(event.GetX(), event.GetY());
        if (graph_obj_ptr)
        {
            GraphObjAttributes graph_attr = graph_obj_ptr->GetGraphObjAttributes();
            graph_attr.is_selected = !graph_attr.is_selected;
            graph_obj_ptr->SetGraphObjAttributes(graph_attr);
            use_context.is_use_selected = true;
            graph_obj_ptr->DrawObject(use_context);
        }
    }
}

void PCADViewerFrame::OnMenuItemCanvasA6Selected(wxCommandEvent& event)
{
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A6);
}

void PCADViewerFrame::OnMenuItemCanvasA7Selected(wxCommandEvent& event)
{
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A7);
}

void PCADViewerFrame::OnMenuItemCanvasA8Selected(wxCommandEvent& event)
{
    SetContextCanvasSize(CanvasSizeCode::CANVAS_SIZE_A8);
}

void PCADViewerFrame::OnScrolledCanvasRightDown(wxMouseEvent& event)
{
    ScrolledCanvas->SetFocus();
}

void PCADViewerFrame::OnScrolledCanvasMiddleDown(wxMouseEvent& event)
{
    ScrolledCanvas->SetFocus();
}

void PCADViewerFrame::OnMenuItemDoCopyToClipboardSelected(wxCommandEvent& event)
{
    GraphExportContext export_context;
    export_context.export_type = GraphExportType::GRAPH_EXPORT_CLIPBOARD;
    export_context.is_canvas_export = MenuItemHandleCanvas->IsChecked();
    export_context.is_selected_export = MenuItemHandleSelected->IsChecked();
    export_context.is_contour_export = MenuItemHandleContour->IsChecked();
    DoGraphExporting(export_context);
}

void PCADViewerFrame::OnMenuItemPrintSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    GraphExportContext export_context;
    export_context.export_type = GraphExportType::GRAPH_EXPORT_PRINTER;
    export_context.is_canvas_export = MenuItemPrintCanvas->IsChecked();
    export_context.is_selected_export = MenuItemPrintSelected->IsChecked();
    export_context.is_contour_export = MenuItemPrintContour->IsChecked();

    wxPrintPreview* preview =
        new wxPrintPreview(new MyPrintout(this, export_context, this_app->pcad_file.GetPictureFileName(), true),
                           new MyPrintout(this, export_context, this_app->pcad_file.GetPictureFileName(), false));
    wxPreviewFrame* frame = new wxPreviewFrame(preview, this,
                                    _("Предпросмотр печати ") + this_app->pcad_file.GetPictureFileName());
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
}

void PCADViewerFrame::OnToolBarCanvasYesNoClicked(wxCommandEvent& event)
{
    SwitchCanvasYesNo(ToolBarCanvasYesNo->IsToggled());
    MenuItemCanvasYesNo->Check(ToolBarCanvasYesNo->IsToggled());
}

void PCADViewerFrame::OnToolBarItemEqualScalesClicked(wxCommandEvent& event)
{
    if (ToolBarItemEqualScales->IsToggled())
        OnMenuEqualScaleXSelected(event);
    else
        SetEqualScalesStatus(false);
}

void PCADViewerFrame::OnKillFocusComboBoxScaleX(wxFocusEvent& event)
{
    wxCommandEvent wxcmd_event;
    OnComboBoxScaleXTextEnter(wxcmd_event);
    event.Skip();
}

void PCADViewerFrame::OnKillFocusComboBoxScaleY(wxFocusEvent& event)
{
    wxCommandEvent wxcmd_event;
    OnComboBoxScaleYTextEnter(wxcmd_event);
    event.Skip();
}

void PCADViewerFrame::OnComboBoxScaleXTextEnter(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    double new_x_scale;

    try
    {
        new_x_scale = stod(string(ComboBoxScaleX->GetValue().mb_str()));
    }
    catch(...)
    {
        new_x_scale = this_app->current_x_scale;
    }

    SetScaleX(new_x_scale);
    if (this_app->is_equal_scales)
        SetScaleY(new_x_scale);

    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnComboBoxScaleYTextEnter(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    double new_y_scale;

    try
    {
        new_y_scale = stod(string(ComboBoxScaleY->GetValue().mb_str()));
    }
    catch(...)
    {
        new_y_scale = this_app->current_y_scale;
    }

    SetScaleY(new_y_scale);
    if (this_app->is_equal_scales)
        SetScaleX(new_y_scale);

    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnToolBarItemExportClicked(wxCommandEvent& event)
{
    PopupMenu(MenuItemExportHead);
}

void PCADViewerFrame::OnToolBarItemPrintClicked(wxCommandEvent& event)
{
    PopupMenu(MenuItemPrintHead);
}

void PCADViewerFrame::OnToolBarItemCanvasLandscapeClicked(wxCommandEvent& event)
{
    if (ToolBarItemCanvasLandscape->IsToggled())
    {
        MenuItemCanvasLandscape->Check(true);
        OnMenuItemCanvasLandscapeSelected(event);
    }
    else
    {
        MenuItemCanvasPortrait->Check(true);
        OnMenuItemCanvasPortraitSelected(event);
    }
}

void PCADViewerFrame::OnToolBarItemMarkClicked(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    GraphExportContext export_context;

    if (this_app->select_contour.is_contour_builded)
        export_context.is_contour_export = true;
    else if (this_app->canvas_context.is_canvas)
        export_context.is_canvas_export = true;

    DoSelectObjectsOp(export_context, SelectOpType::SELECT_OP_SELECT);
}

void PCADViewerFrame::ChangeCanvasSize(int new_cnv_width, int new_cnv_height, int update_spin_mode)
{ // Входные аргументы функции - новые размеры холста - задаются здесь в единиццах DBU.
  // Если какой-либо аргумент имеет специальное сигнальное значение (< 0), то соответствующая
  // величина размера холста не меняется (заимствуется для пересчёта из текущих значений поля
  // this_app->canvas_context.canvas_size).
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    SizePositionData old_pd = CountSizePosition();
    if (new_cnv_width > 0)
        this_app->canvas_context.canvas_size.SetWidth(new_cnv_width);
    else
        new_cnv_width = this_app->canvas_context.canvas_size.GetWidth();
    if (new_cnv_height > 0)
        this_app->canvas_context.canvas_size.SetHeight(new_cnv_height);
    else
        new_cnv_height = this_app->canvas_context.canvas_size.GetHeight();
    // Сначала устанавливаем правильную ориентацию холста нового размера. При портретной
    // ориентации меньший размер - ширина, больший - высота. При ландшафтной ориентации - наоборот.
    if (this_app->canvas_context.is_portrait)
    {
        if (new_cnv_width > new_cnv_height)
        {
            this_app->canvas_context.canvas_ratio = 1 / this_app->canvas_context.canvas_ratio;
            this_app->canvas_context.canvas_size = wxSize(new_cnv_height, new_cnv_width);
        }
        else
        {
            this_app->canvas_context.canvas_size = wxSize(new_cnv_width, new_cnv_height);
        }
    }
    else
    {
        if (new_cnv_height > new_cnv_width)
        {
            this_app->canvas_context.canvas_ratio = 1 / this_app->canvas_context.canvas_ratio;
            this_app->canvas_context.canvas_size = wxSize(new_cnv_height, new_cnv_width);
        }
        else
        {
            this_app->canvas_context.canvas_size = wxSize(new_cnv_width, new_cnv_height);
        }
    }
    // Ну а теперь обновляем содержимое полей управления размерами холста на панельке инструментов.
    wxSize cnvs(this_app->canvas_context.canvas_size);
    if (this_app->canvas_context.is_metric)
    {
        if (update_spin_mode & UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN)
            SpinCanvasSizeHor->SetValue(this_app->pcad_file.ConvertDBUToMM(cnvs.GetWidth()));
        if (update_spin_mode & UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN)
            SpinCanvasSizeVert->SetValue(this_app->pcad_file.ConvertDBUToMM(cnvs.GetHeight()));
    }
    else
    {
        if (update_spin_mode & UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN)
            SpinCanvasSizeHor->SetValue(this_app->pcad_file.ConvertDBUToInch(cnvs.GetWidth()) * POINTS_IN_INCH);
        if (update_spin_mode & UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN)
            SpinCanvasSizeVert->SetValue(this_app->pcad_file.ConvertDBUToInch(cnvs.GetHeight()) * POINTS_IN_INCH);
    }
    SwitchCanvasYesNo(true);
    SetThumbsSizes(old_pd);
}

void PCADViewerFrame::SetCanvasSpinSizeRange()
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    bool save_is_event_handling = is_event_handling;
    is_event_handling = false;
    if (this_app->canvas_context.is_metric)
    {
        SpinCanvasSizeHor->SetRange(std_wx_objects.MinCanvasSizeInMM.GetWidth(),
                                    std_wx_objects.MaxCanvasSizeInMM.GetWidth());
        SpinCanvasSizeVert->SetRange(std_wx_objects.MinCanvasSizeInMM.GetHeight(),
                                     std_wx_objects.MaxCanvasSizeInMM.GetHeight());
    }
    else
    {
        SpinCanvasSizeHor->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetWidth(),
                                    std_wx_objects.MaxCanvasSizeInPoints.GetWidth());
        SpinCanvasSizeVert->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetHeight(),
                                     std_wx_objects.MaxCanvasSizeInPoints.GetHeight());
    }
    is_event_handling = save_is_event_handling;
}

void PCADViewerFrame::OnSpinCanvasSizeHorChange(wxSpinEvent& event)
{
    if (!is_event_handling)
        return;
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    double cnv_width = SpinCanvasSizeHor->GetValue();
    // В поле canvas_size структуры canvas_context размер холста должен
    // задаваться в DBU загруженной картинки.
    if (!this_app->canvas_context.is_metric)
        cnv_width = this_app->pcad_file.ConvertInchToDBU(cnv_width / POINTS_IN_INCH);
    else
        cnv_width = this_app->pcad_file.ConvertMMToDBU(cnv_width);
    this_app->canvas_context.canvas_size.SetWidth(round(cnv_width));
    // Если нужно обеспечить нужную фиксированную пропорцию, подправим также и высоту холста.
    if (this_app->canvas_context.is_fixed_ratio)
    {
        ProportionateCanvas(ProportionateWhat::PROPORTION_HEIGHT);
        ChangeCanvasSize(-1, -1, UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN);
    }
    else
    {
        ChangeCanvasSize(-1, -1, UpdateSpinCanvasMode::UPDATE_NONE_CANVAS_SPIN);
    }
}

void PCADViewerFrame::OnSpinCanvasSizeVertChange(wxSpinEvent& event)
{
    if (!is_event_handling)
        return;
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    double cnv_height = SpinCanvasSizeVert->GetValue();
    // На выходе, в поле canvas_size состояния холста, его размер должен
    // задаваться в DBU загруженной картинки.
    if (!this_app->canvas_context.is_metric)
        cnv_height = this_app->pcad_file.ConvertInchToDBU(cnv_height / POINTS_IN_INCH);
    else
        cnv_height = this_app->pcad_file.ConvertMMToDBU(cnv_height);
    this_app->canvas_context.canvas_size.SetHeight(round(cnv_height));
    // Если нужно обеспечить нужную фиксированную пропорцию, подправим также и ширину холста.
    if (this_app->canvas_context.is_fixed_ratio)
    {
        ProportionateCanvas(ProportionateWhat::PROPORTION_WIDTH);
        ChangeCanvasSize(-1, -1, UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN);
    }
    else
    {
        ChangeCanvasSize(-1, -1, UpdateSpinCanvasMode::UPDATE_NONE_CANVAS_SPIN);
    }
}

void PCADViewerFrame::OnMenuItemExportSVGSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;
    wxString export_filename = SaveExportDialog(_("SVG-файл"), _("Векторные графические SVG-файлы"), wxT("*.svg"));
    if (!export_filename.size())
        return;

    GraphExportContext export_context;
    export_context.export_type = GraphExportType::GRAPH_EXPORT_SVG;
    export_context.filename = export_filename;
    export_context.is_canvas_export = MenuItemExportCanvas->IsChecked();
    export_context.is_selected_export = MenuItemExportSelected->IsChecked();
    export_context.is_contour_export = MenuItemExportContour->IsChecked();
    export_context.svg_document_ptr = nullptr;
    DoGraphExporting(export_context);
}

void PCADViewerFrame::OnMenuItemExportDXFSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;
    wxString export_filename = SaveExportDialog(_("DXF-файл"), _("Векторные графические DXF-файлы AutoCAD"), wxT("*.dxf"));
    if (!export_filename.size())
        return;

    wxMessageBox(_("Не реализовано. Надеемся, пока."), _("Увы..."), wxICON_HAND | wxCENTRE);
}

void PCADViewerFrame::OnMenuItemExportWMFSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    if (!this_app->pcad_file.size())
        return;
    wxString export_filename = SaveExportDialog(_("WMF-метафайл"), _("Векторные графические Windows-метафайлы"), wxT("*.wmf"));
    if (!export_filename.size())
        return;

    wxMessageBox(_("Не реализовано. Надеемся, пока."), _("Увы..."), wxICON_HAND | wxCENTRE);
}

void PCADViewerFrame::OnToolBarItemCopyClicked(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    GraphExportContext export_context;
    export_context.export_type = GraphExportType::GRAPH_EXPORT_CLIPBOARD;
    export_context.is_canvas_export = this_app->canvas_context.is_canvas;

    export_context.is_selected_export = false;
    for (GraphObj* graph_obj_ptr : this_app->pcad_file)
        if (graph_obj_ptr->GetGraphObjAttributes().is_selected)
        {
            export_context.is_selected_export = true;
            break;
        }
    DoGraphExporting(export_context);
}

void PCADViewerFrame::OnMenuItemScaleUpBothSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    OnMenuScaleUpXSelected(event);
    if (!this_app->is_equal_scales)
        OnMenuScaleUpYSelected(event);
}

void PCADViewerFrame::OnMenuItemScaleDownBothSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    OnMenuScaleDownXSelected(event);
    if (!this_app->is_equal_scales)
        OnMenuScaleDownYSelected(event);
}

void PCADViewerFrame::OnMenuItemEqualScalesSelected(wxCommandEvent& event)
{
    OnMenuEqualScaleXSelected(event);
}

void PCADViewerFrame::OnMenuItemDiffScalesSelected(wxCommandEvent& event)
{
    SetEqualScalesStatus(false);
}

void PCADViewerFrame::OnMenuItemNearestStdScaleBothSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    double new_x_scale_value = scale_value_array[FindScaleStage(this_app->current_x_scale)];
    SetScaleX(new_x_scale_value);
    if (this_app->is_equal_scales)
        SetScaleY(new_x_scale_value);
    else
        SetScaleY(scale_value_array[FindScaleStage(this_app->current_y_scale)]);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuItemNearestStdScaleYSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    double new_y_scale_value = scale_value_array[FindScaleStage(this_app->current_y_scale)];
    SetScaleY(new_y_scale_value);
    if (this_app->is_equal_scales)
        SetScaleX(new_y_scale_value);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuItemNearestStdScaleXSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    double new_x_scale_value = scale_value_array[FindScaleStage(this_app->current_x_scale)];
    SetScaleX(new_x_scale_value);
    if (this_app->is_equal_scales)
        SetScaleY(new_x_scale_value);
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuItemFontsSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    wxFontDialog font_dialog(this, this_app->font_data);

    if (font_dialog.ShowModal() != wxID_OK)
        return;
    this_app->font_data = font_dialog.GetFontData();
    ScrolledCanvas->Refresh();
}

void PCADViewerFrame::OnMenuItemOptionsSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    OptionsDialog options_dialog(this);

    options_dialog.TextApertureFilename->SetValue(this_app->pcad_file.GetApertureFileName());
    if (this_app->pcad_file.GetApertureType() == UsingApertureType::USING_APERTURE_GERBER_LASER)
        options_dialog.ComboBoxApertureSection->SetValue(wxT("GERBER LASER"));
    else
        options_dialog.ComboBoxApertureSection->SetValue(wxT("GERBER 32"));

    options_dialog.CheckBoxScreenSubscale->SetValue(this_app->options_data.is_use_screen_subscale);

    switch (this_app->options_data.additional_scale_mode)
    {
    case AdditionalScaleModeType::ADDITIONAL_SCALE_NONE:
		options_dialog.RadioAdditionalUnitScaleOff->SetValue(true);
        options_dialog.TextAdditionalUnitScaleXValue->Enable(false);
        options_dialog.TextAdditionalUnitScaleYValue->Enable(false);
        this_app->options_data.additional_scale_x = 1;
        this_app->options_data.additional_scale_y = 1;
        break;
    case AdditionalScaleModeType::ADDITIONAL_SCALE_RUSSIAN_INCH:
		options_dialog.RadioAdditionalUnitScaleRussianInch->SetValue(true);
        options_dialog.TextAdditionalUnitScaleXValue->Enable(false);
        options_dialog.TextAdditionalUnitScaleYValue->Enable(false);
        this_app->options_data.additional_scale_x = RUSSIAN_INCH_SCALE;
        this_app->options_data.additional_scale_y = RUSSIAN_INCH_SCALE;
        break;
    case AdditionalScaleModeType::ADDITIONAL_SCALE_ANY:
        options_dialog.RadioAdditionalUnitScaleAny->SetValue(true);
        options_dialog.TextAdditionalUnitScaleXValue->Enable(true);
        options_dialog.TextAdditionalUnitScaleYValue->Enable(true);
        break;
    }
    options_dialog.TextAdditionalUnitScaleXValue->SetValue(wxString
        (to_string(this_app->options_data.additional_scale_x).c_str(), wxConvUTF8));
    options_dialog.TextAdditionalUnitScaleYValue->SetValue(wxString
        (to_string(this_app->options_data.additional_scale_y).c_str(), wxConvUTF8));

    switch(this_app->options_data.measure_unit_type)
    {
    case MeasureUnitTypeData::MEASURE_UNIT_MM:
        options_dialog.RadioUseMillimeter->SetValue(true);
        break;
    case MeasureUnitTypeData::MEASURE_UNIT_INCH:
        options_dialog.RadioUseInch->SetValue(true);
        break;
    case MeasureUnitTypeData::MEASURE_UNIT_DBU:
    default:
        options_dialog.RadioUseDBU->SetValue(true);
        break;
    }

    options_dialog.temp_aperture_provider = this_app->aperture_provider;

    if (options_dialog.ShowModal() != wxID_OK)
        return;

    this_app->options_data.is_use_screen_subscale = options_dialog.CheckBoxScreenSubscale->GetValue();

    if (options_dialog.RadioAdditionalUnitScaleOff->GetValue())
        this_app->options_data.additional_scale_mode = AdditionalScaleModeType::ADDITIONAL_SCALE_NONE;
    else if (options_dialog.RadioAdditionalUnitScaleRussianInch->GetValue())
        this_app->options_data.additional_scale_mode = AdditionalScaleModeType::ADDITIONAL_SCALE_RUSSIAN_INCH;
    else if (options_dialog.RadioAdditionalUnitScaleAny->GetValue())
        this_app->options_data.additional_scale_mode = AdditionalScaleModeType::ADDITIONAL_SCALE_ANY;

    if (options_dialog.ComboBoxApertureSection->GetValue() == wxT("GERBER LASER"))
        this_app->pcad_file.SetApertureType(UsingApertureType::USING_APERTURE_GERBER_LASER);
    else
        this_app->pcad_file.SetApertureType(UsingApertureType::USING_APERTURE_GERBER32);

    try
    {
        this_app->options_data.additional_scale_x =
            stod(string(options_dialog.TextAdditionalUnitScaleXValue->GetValue().mb_str()));
    }
    catch (...)
    {}

    try
    {
        this_app->options_data.additional_scale_y =
            stod(string(options_dialog.TextAdditionalUnitScaleYValue->GetValue().mb_str()));
    }
    catch (...)
    {}

    if (options_dialog.RadioUseMillimeter->GetValue())
        this_app->options_data.measure_unit_type = MeasureUnitTypeData::MEASURE_UNIT_MM;
    else if (options_dialog.RadioUseInch->GetValue())
        this_app->options_data.measure_unit_type = MeasureUnitTypeData::MEASURE_UNIT_INCH;
    else
        this_app->options_data.measure_unit_type = MeasureUnitTypeData::MEASURE_UNIT_DBU;
    this_app->last_draw_context.measure_unit_type = this_app->options_data.measure_unit_type;

    this_app->aperture_provider = options_dialog.temp_aperture_provider;

    wxSizeEvent wxsz;
    OnScrolledCanvasResize(wxsz);
    ScrolledCanvas->Refresh();
    SetStatusText(WhatStatusType::SET_STATUS_PICTURE_INFO);
}

void PCADViewerFrame::SetStatusText(WhatStatusType what_status, const SizePositionData* pd_ptr,
                                    const GraphObj* graph_obj_ptr, const wxPoint* src_point_ptr)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileDefValues fdv = this_app->pcad_file.GetFileDefValues();
    DrawContext& use_context = this_app->last_draw_context;
    string status_text;

    use_context.text_export_style = TextExportStyleType::TEXT_EXPORT_CONDENSED_STYLE;
    if (what_status & WhatStatusType::SET_STATUS_CURSOR_INFO)
    {
        status_text = ConvertPointToString(*src_point_ptr, use_context);
        if (graph_obj_ptr)
            status_text += ':' + graph_obj_ptr->GetObjectShortText(use_context) +
                           ':' + to_string(graph_obj_ptr->GetGraphObjectOrdinal());
        StatusBar1->SetStatusText(wxString(status_text.c_str(), wxConvUTF8));
    }

    if (what_status & WhatStatusType::SET_STATUS_PICTURE_INFO)
    {
        wxString wx_status_text = fdv.file_signature +
            wxString(ConvertRectToStringShortEx(fdv.frame_rect, use_context).c_str(), wxConvUTF8);
        StatusBar1->SetStatusText(wx_status_text, 1);
    }

    if (what_status & WhatStatusType::SET_STATUS_VIEW_AREA_INFO)
    {
        status_text = ConvertRectToStringShortEx(pd_ptr->source_draw_part, use_context);
        StatusBar1->SetStatusText(wxString(status_text.c_str(), wxConvUTF8), 2);
    }

    if (what_status & WhatStatusType::SET_STATUS_WORKSCALE_INFO)
    {
        ostringstream ostr_x, ostr_y;
        ostr_x << fixed << setprecision(3) << this_app->current_x_scale;
        ostr_y << fixed << setprecision(3) << this_app->current_y_scale;
        if (this_app->is_equal_scales)
            status_text = ostr_x.str() + " ="s;
        else
            status_text = ostr_x.str() + " : "s + ostr_y.str();
        StatusBar1->SetStatusText(wxString(status_text.c_str(), wxConvUTF8), 3);
    }
}

void PCADViewerFrame::OnToolBarItemInvertMarkClicked(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    GraphExportContext export_context;

    if (this_app->select_contour.is_contour_builded)
        export_context.is_contour_export = true;
    else if (this_app->canvas_context.is_canvas)
        export_context.is_canvas_export = true;

    DoSelectObjectsOp(export_context, SelectOpType::SELECT_OP_INVERT);
}

void PCADViewerFrame::OnToolBarItemUnmarkClicked(wxCommandEvent& event)
{
    GraphExportContext export_context;
    DoSelectObjectsOp(export_context, SelectOpType::SELECT_OP_UNSELECT);
}

void PCADViewerFrame::OnScrolledCanvasLeftUp(wxMouseEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->select_contour.contour_catch = ContourSideDetectType::CONTOUR_NOT_INTERSECT;
    if (this_app->select_contour.is_contour_builded &&
        this_app->select_contour.contour_rect.IsEmpty())
    { // Если при отпускании левой кнопки мыши определен пустой выделяющий контур - удалим его.
        wxClientDC scrolled_canvas_dc(ScrolledCanvas);
        DrawContext use_context;
        use_context.ResetProjectRect(this_app->last_draw_context.project_rect);
        use_context.ResetTargetDC(&scrolled_canvas_dc, false);
        RestoreContourBackgroud(use_context, this_app->select_contour);
    }
}

void PCADViewerFrame::ProportionateCanvas(ProportionateWhat what_proportionate)
{ // Функция пропорционирует холст - изменяет его размеры
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    if (what_proportionate == ProportionateWhat::PROPORTION_WIDTH)
    { // Пропорционируем ширину холста, сохраняя высоту
        int new_width = round(this_app->canvas_context.canvas_size.GetHeight() *
                        this_app->canvas_context.canvas_ratio);
        this_app->canvas_context.canvas_size.SetWidth(max(new_width, 1));
    }
    else
    {  // Пропорционируем высоту холста, сохраняя ширину
        int new_height = round(this_app->canvas_context.canvas_size.GetWidth() /
                         this_app->canvas_context.canvas_ratio);
        this_app->canvas_context.canvas_size.SetHeight(max(new_height, 1));
    }
}

void PCADViewerFrame::OnMenuItemCanvasFixRatioSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    if (this_app->canvas_context.is_fixed_ratio != MenuItemCanvasFixRatio->IsChecked())
    {
        this_app->canvas_context.is_fixed_ratio = MenuItemCanvasFixRatio->IsChecked();
        ProportionateCanvas(ProportionateWhat::PROPORTION_HEIGHT);
        ChangeCanvasSize(-1, -1, UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN);
    }
}

void PCADViewerFrame::OnMenuItemCanvasMetricSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    if (this_app->canvas_context.is_metric != MenuItemCanvasMetric->IsChecked())
    {
        this_app->canvas_context.is_metric = MenuItemCanvasMetric->IsChecked();
        SetCanvasSpinSizeRange();
        ChangeCanvasSize(-1, -1,
                         UpdateSpinCanvasMode::UPDATE_HOR_CANVAS_SPIN |
                         UpdateSpinCanvasMode::UPDATE_VERT_CANVAS_SPIN);
    }
}
