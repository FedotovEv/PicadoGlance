
#include <string>

#include "wx_pch.h"
#include "SetCanvasSize.h"
#include "PCADViewerApp.h"

using namespace std;

extern const StdWXObjects std_wx_objects;

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(SetCanvasSize)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(SetCanvasSize)
//*)

//(*IdInit(SetCanvasSize)
const long SetCanvasSize::ID_STATICTEXT1 = wxNewId();
const long SetCanvasSize::ID_SPINCTRL_HOR_SIZE = wxNewId();
const long SetCanvasSize::ID_STATICTEXT2 = wxNewId();
const long SetCanvasSize::ID_SPINCTRL_VERT_SIZE = wxNewId();
const long SetCanvasSize::ID_STATICTEXT4 = wxNewId();
const long SetCanvasSize::ID_RADIOBUTTON_MM = wxNewId();
const long SetCanvasSize::ID_RADIOBUTTON_INCH = wxNewId();
const long SetCanvasSize::ID_STATICTEXT3 = wxNewId();
const long SetCanvasSize::ID_TEXTCTRL_ASPECT_RATIO = wxNewId();
const long SetCanvasSize::ID_CHECKBOX_FIXED_ASPECT_RATIO = wxNewId();
const long SetCanvasSize::ID_BUTTON_SET = wxNewId();
const long SetCanvasSize::ID_BUTTON_CANCEL = wxNewId();
const long SetCanvasSize::ID_BUTTON_RESET = wxNewId();
const long SetCanvasSize::ID_BUTTON1 = wxNewId();
//*)

const long SetCanvasSize::IdMenuCanvasA3 = wxNewId();
const long SetCanvasSize::IdMenuCanvasA4 = wxNewId();
const long SetCanvasSize::IdMenuCanvasA5 = wxNewId();
const long SetCanvasSize::IdMenuCanvasA6 = wxNewId();
const long SetCanvasSize::IdMenuCanvasA7 = wxNewId();
const long SetCanvasSize::IdMenuCanvasA8 = wxNewId();
const long SetCanvasSize::IdMenuCanvasCurrent = wxNewId();

BEGIN_EVENT_TABLE(SetCanvasSize,wxDialog)
	//(*EventTable(SetCanvasSize)
	//*)
END_EVENT_TABLE()

SetCanvasSize::SetCanvasSize(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    FileDefValues fd = this_app->pcad_file.GetFileDefValues();

	//(*Initialize(SetCanvasSize)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer7;

	Create(parent, wxID_ANY, _("Размер области операций"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Ширина"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer4->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	HorSize = new wxSpinCtrl(this, ID_SPINCTRL_HOR_SIZE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 5000, 0, _T("ID_SPINCTRL_HOR_SIZE"));
	HorSize->SetValue(_T("0"));
	BoxSizer4->Add(HorSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Высота"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer5->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	VertSize = new wxSpinCtrl(this, ID_SPINCTRL_VERT_SIZE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 5000, 0, _T("ID_SPINCTRL_VERT_SIZE"));
	VertSize->SetValue(_T("0"));
	BoxSizer5->Add(VertSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7 = new wxBoxSizer(wxVERTICAL);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Единицы измерения"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer7->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	MeasureUnitMM = new wxRadioButton(this, ID_RADIOBUTTON_MM, _("миллиметр"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_MM"));
	MeasureUnitMM->SetValue(true);
	BoxSizer7->Add(MeasureUnitMM, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	MeasureUnitInch = new wxRadioButton(this, ID_RADIOBUTTON_INCH, _("сотая дюйма"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_INCH"));
	BoxSizer7->Add(MeasureUnitInch, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer7, 1, wxALL, 5);
	BoxSizer6 = new wxBoxSizer(wxVERTICAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Соотношение сторон"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer6->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	AspectRatio = new wxTextCtrl(this, ID_TEXTCTRL_ASPECT_RATIO, _("1"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL_ASPECT_RATIO"));
	AspectRatio->SetMinSize(wxSize(50,-1));
	BoxSizer6->Add(AspectRatio, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	FixedAspectRatio = new wxCheckBox(this, ID_CHECKBOX_FIXED_ASPECT_RATIO, _("Фикс"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_FIXED_ASPECT_RATIO"));
	FixedAspectRatio->SetValue(false);
	BoxSizer3->Add(FixedAspectRatio, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	ButtonSet = new wxButton(this, ID_BUTTON_SET, _("Установить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SET"));
	BoxSizer2->Add(ButtonSet, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonCancel = new wxButton(this, ID_BUTTON_CANCEL, _("Отменить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANCEL"));
	BoxSizer2->Add(ButtonCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonReset = new wxButton(this, ID_BUTTON_RESET, _("Сбросить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_RESET"));
	BoxSizer2->Add(ButtonReset, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonSetStandart = new wxButton(this, ID_BUTTON1, _("Стандартные"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(ButtonSetStandart, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_SPINCTRL_HOR_SIZE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&SetCanvasSize::OnHorSizeChange);
	Connect(ID_SPINCTRL_VERT_SIZE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&SetCanvasSize::OnVertSizeChange);
	Connect(ID_RADIOBUTTON_MM,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&SetCanvasSize::OnMeasureUnitMMSelect);
	Connect(ID_RADIOBUTTON_INCH,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&SetCanvasSize::OnMeasureUnitInchSelect);
	Connect(ID_TEXTCTRL_ASPECT_RATIO,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&SetCanvasSize::OnAspectRatioTextEnter);
	Connect(ID_CHECKBOX_FIXED_ASPECT_RATIO,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnFixedAspectRatioClick);
	Connect(ID_BUTTON_SET,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonSetClick);
	Connect(ID_BUTTON_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonCancelClick);
	Connect(ID_BUTTON_RESET,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonResetClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonSetStandartClick);
	//*)

	AspectRatio->Connect(wxEVT_KILL_FOCUS,(wxObjectEventFunction)&SetCanvasSize::OnAspectRatioKillFocus, 0, this);
    // Создаём меню размеров стандартных форматов
    MenuStdCanvasFormat = new wxMenu();
    MenuItemCanvasA3 = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasA3, _("Формат A3"), _("Выбрать формат A3"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasA3);
    MenuItemCanvasA4 = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasA4, _("Формат A4"), _("Выбрать формат A4"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasA4);
    MenuItemCanvasA5 = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasA5, _("Формат A5"), _("Выбрать формат A5"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasA5);
    MenuItemCanvasA6 = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasA6, _("Формат A6"), _("Выбрать формат A6"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasA6);
    MenuItemCanvasA7 = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasA7, _("Формат A7"), _("Выбрать формат A7"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasA7);
    MenuItemCanvasA8 = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasA8, _("Формат A8"), _("Выбрать формат A8"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasA8);
    MenuItemCanvasCurrent = new wxMenuItem(MenuStdCanvasFormat, IdMenuCanvasCurrent, _("Текущий Формат"), _("Выбрать текущий формат"), wxITEM_NORMAL);
    MenuStdCanvasFormat->Append(MenuItemCanvasCurrent);
    // Подключаем обработчики для этого меню
    Connect(IdMenuCanvasA3, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    Connect(IdMenuCanvasA4, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    Connect(IdMenuCanvasA5, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    Connect(IdMenuCanvasA6, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    Connect(IdMenuCanvasA7, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    Connect(IdMenuCanvasA8, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    Connect(IdMenuCanvasCurrent, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SetCanvasSize::OnMenuItemSetStdSizeSelected);
    if (!this_app->last_draw_context.pcad_doc_ptr || !fd.DBU_in_measure_unit ||
        !this_app->canvas_context.canvas_size.GetWidth() || !this_app->canvas_context.canvas_size.GetHeight())
        MenuItemCanvasCurrent->Enable(false);

    ResetData();
}

SetCanvasSize::~SetCanvasSize()
{
	//(*Destroy(SetCanvasSize)
	//*)

    delete MenuStdCanvasFormat;
}

void SetCanvasSize::SetSizesByCurrentAspect()
{
    double canvas_area = static_cast<double>(HorSize->GetValue()) * VertSize->GetValue();
    double cnv_aspect_ratio = atof(AspectRatio->GetValue().mb_str());
    HorSize->SetValue(round(sqrt(canvas_area * cnv_aspect_ratio)));
    VertSize->SetValue(round(sqrt(canvas_area / cnv_aspect_ratio)));
}

void SetCanvasSize::SetAspectByCurrentSizes()
{
    double cnv_aspect_ratio = static_cast<double>(HorSize->GetValue()) / VertSize->GetValue();
    AspectRatio->SetValue(wxString(to_string(cnv_aspect_ratio).c_str(), wxConvUTF8));
}

void SetCanvasSize::CorrectAspectRatioValue()
{
    double cnv_aspect_ratio = atof(AspectRatio->GetValue().mb_str());
    if (cnv_aspect_ratio < CANVAS_ASPECT_MIN)
    {
        cnv_aspect_ratio = CANVAS_ASPECT_MIN;
        AspectRatio->SetValue(wxString(to_string(cnv_aspect_ratio).c_str(), wxConvUTF8));
    }
    else if (cnv_aspect_ratio > CANVAS_ASPECT_MAX)
    {
        cnv_aspect_ratio = CANVAS_ASPECT_MAX;
        AspectRatio->SetValue(wxString(to_string(cnv_aspect_ratio).c_str(), wxConvUTF8));
    }
}

void SetCanvasSize::CorrectAspectRatioAndSetSizes()
{
    CorrectAspectRatioValue();
    SetSizesByCurrentAspect();
}

void SetCanvasSize::ResetData()
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    int cnv_width = this_app->vary_canvas_status.canvas_size.GetWidth();
    int cnv_height = this_app->vary_canvas_status.canvas_size.GetHeight();
    if (this_app->vary_canvas_status.is_metric)
    {
        HorSize->SetRange(std_wx_objects.MinCanvasSizeInMM.GetWidth(),
                          std_wx_objects.MaxCanvasSizeInMM.GetWidth());
        VertSize->SetRange(std_wx_objects.MinCanvasSizeInMM.GetHeight(),
                           std_wx_objects.MaxCanvasSizeInMM.GetHeight());
        MeasureUnitMM->SetValue(true);
    }
    else
    {
        HorSize->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetWidth(),
                          std_wx_objects.MaxCanvasSizeInPoints.GetWidth());
        VertSize->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetHeight(),
                           std_wx_objects.MaxCanvasSizeInPoints.GetHeight());
        MeasureUnitInch->SetValue(true);
    }
    HorSize->SetValue(cnv_width);
    VertSize->SetValue(cnv_height);
    FixedAspectRatio->SetValue(this_app->vary_canvas_status.is_fixed_ratio);
    SetAspectByCurrentSizes();
    if (FixedAspectRatio->GetValue())
        CorrectAspectRatioAndSetSizes();
    else
        AspectRatio->Disable();
}

void SetCanvasSize::OnButtonCancelClick(wxCommandEvent& event)
{
    EndModal(0);
}

void SetCanvasSize::OnButtonResetClick(wxCommandEvent& event)
{
    ResetData();
}

void SetCanvasSize::OnButtonSetClick(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->vary_canvas_status.canvas_size = wxSize(HorSize->GetValue(), VertSize->GetValue());
    this_app->vary_canvas_status.is_metric = MeasureUnitMM->GetValue();
    this_app->vary_canvas_status.is_fixed_ratio = FixedAspectRatio->GetValue();
    EndModal(1);
}

void SetCanvasSize::OnHorSizeChange(wxSpinEvent& event)
{
    if (FixedAspectRatio->GetValue())
    {
        CorrectAspectRatioValue();
        double cnv_aspect_ratio = atof(AspectRatio->GetValue().mb_str());
        VertSize->SetValue(round(HorSize->GetValue() / cnv_aspect_ratio));
    }
    else
    {
        SetAspectByCurrentSizes();
    }
}

void SetCanvasSize::OnVertSizeChange(wxSpinEvent& event)
{
    if (FixedAspectRatio->GetValue())
    {
        CorrectAspectRatioValue();
        double cnv_aspect_ratio = atof(AspectRatio->GetValue().mb_str());
        HorSize->SetValue(round(VertSize->GetValue() * cnv_aspect_ratio));
    }
    else
    {
        SetAspectByCurrentSizes();
    }
}

void SetCanvasSize::OnAspectRatioKillFocus(wxFocusEvent& event)
{
    CorrectAspectRatioAndSetSizes();
}

void SetCanvasSize::OnAspectRatioTextEnter(wxCommandEvent& event)
{
    CorrectAspectRatioAndSetSizes();
}

void SetCanvasSize::OnFixedAspectRatioClick(wxCommandEvent& event)
{
    if (FixedAspectRatio->GetValue())
    {
        CorrectAspectRatioAndSetSizes();
        AspectRatio->Enable();
    }
    else
    {
        AspectRatio->Disable();
    }
}

void SetCanvasSize::OnMeasureUnitMMSelect(wxCommandEvent& event)
{
    HorSize->SetRange(std_wx_objects.MinCanvasSizeInMM.GetWidth(),
                      std_wx_objects.MaxCanvasSizeInMM.GetWidth());
    VertSize->SetRange(std_wx_objects.MinCanvasSizeInMM.GetHeight(),
                       std_wx_objects.MaxCanvasSizeInMM.GetHeight());
    HorSize->SetValue(round(HorSize->GetValue() / POINTS_IN_MILLIMETER));
    VertSize->SetValue(round(VertSize->GetValue() / POINTS_IN_MILLIMETER));
    SetAspectByCurrentSizes();
}

void SetCanvasSize::OnMeasureUnitInchSelect(wxCommandEvent& event)
{
    HorSize->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetWidth(),
                      std_wx_objects.MaxCanvasSizeInPoints.GetWidth());
    VertSize->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetHeight(),
                       std_wx_objects.MaxCanvasSizeInPoints.GetHeight());
    HorSize->SetValue(round(HorSize->GetValue() * POINTS_IN_MILLIMETER));
    VertSize->SetValue(round(VertSize->GetValue() * POINTS_IN_MILLIMETER));
    SetAspectByCurrentSizes();
}

void SetCanvasSize::OnButtonSetStandartClick(wxCommandEvent& event)
{
    PopupMenu(MenuStdCanvasFormat);
}

void SetCanvasSize::OnMenuItemSetStdSizeSelected(wxCommandEvent& event)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    wxSize std_size;
    double format_width, format_height;
    CanvasSizeCode cnv_size_code;
    FileDefValues fd = this_app->pcad_file.GetFileDefValues();

    if (event.GetId() == IdMenuCanvasA3)
        cnv_size_code = CanvasSizeCode::CANVAS_SIZE_A3;
    else if (event.GetId() == IdMenuCanvasA4)
        cnv_size_code = CanvasSizeCode::CANVAS_SIZE_A4;
    else if (event.GetId() == IdMenuCanvasA5)
        cnv_size_code = CanvasSizeCode::CANVAS_SIZE_A5;
    else if (event.GetId() == IdMenuCanvasA6)
        cnv_size_code = CanvasSizeCode::CANVAS_SIZE_A6;
    else if (event.GetId() == IdMenuCanvasA7)
        cnv_size_code = CanvasSizeCode::CANVAS_SIZE_A7;
    else if (event.GetId() == IdMenuCanvasA8)
        cnv_size_code = CanvasSizeCode::CANVAS_SIZE_A8;
    else if (event.GetId() == IdMenuCanvasCurrent)
    {
        format_width = this_app->pcad_file.ConvertDBUToMM(this_app->canvas_context.canvas_size.GetWidth());
        format_height = this_app->pcad_file.ConvertDBUToMM(this_app->canvas_context.canvas_size.GetHeight());
        if (MeasureUnitMM->GetValue())
            std_size = wxSize(format_width, format_height);
        else
            std_size = wxSize(format_width * POINTS_IN_MILLIMETER, format_height * POINTS_IN_MILLIMETER);
        HorSize->SetValue(std_size.GetWidth());
        VertSize->SetValue(std_size.GetHeight());
        SetAspectByCurrentSizes();
        return;
    }
    else
        return;

    std_size = std_wx_objects.GetStandartCanvasSize(cnv_size_code, true);
    if (!MeasureUnitMM->GetValue())
        std_size = wxSize(round(static_cast<double>(std_size.GetWidth()) * POINTS_IN_MILLIMETER),
                                round(static_cast<double>(std_size.GetHeight()) * POINTS_IN_MILLIMETER));
    HorSize->SetValue(std_size.GetWidth());
    VertSize->SetValue(std_size.GetHeight());
    SetAspectByCurrentSizes();
}
