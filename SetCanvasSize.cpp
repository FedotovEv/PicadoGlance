
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
const long SetCanvasSize::ID_LISTBOX1 = wxNewId();
const long SetCanvasSize::ID_BUTTON_CANVAS_VARY_ADD = wxNewId();
const long SetCanvasSize::ID_BUTTON_CANVAS_VARY_DELETE = wxNewId();
const long SetCanvasSize::ID_BUTTON_CANVAS_VARY_NAME = wxNewId();
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
const long SetCanvasSize::ID_BUTTON_STD_CANVAS = wxNewId();
const long SetCanvasSize::ID_BUTTON_SAVE_TO_FILE = wxNewId();
const long SetCanvasSize::ID_BUTTON_LOAD_FROM_FILE = wxNewId();
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
	wxBoxSizer* BoxSizer10;
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer8;
	wxBoxSizer* BoxSizer9;

	Create(parent, wxID_ANY, _("Размер области операций"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	CanvasVaryList = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	BoxSizer4->Add(CanvasVaryList, 2, wxALL|wxEXPAND, 5);
	BoxSizer10 = new wxBoxSizer(wxVERTICAL);
	CanvasVaryAdd = new wxButton(this, ID_BUTTON_CANVAS_VARY_ADD, _("Добавить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANVAS_VARY_ADD"));
	CanvasVaryAdd->SetToolTip(_("Добавить формат в список"));
	CanvasVaryAdd->SetHelpText(_("Добавить ещё один формат в список пользователя"));
	BoxSizer10->Add(CanvasVaryAdd, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CanvasVaryDelete = new wxButton(this, ID_BUTTON_CANVAS_VARY_DELETE, _("Удалить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANVAS_VARY_DELETE"));
	CanvasVaryDelete->SetToolTip(_("Удалить формат из списка"));
	CanvasVaryDelete->SetHelpText(_("Удалить текущий формат из пользовательского списка"));
	BoxSizer10->Add(CanvasVaryDelete, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CanvasVaryName = new wxButton(this, ID_BUTTON_CANVAS_VARY_NAME, _("Имя"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANVAS_VARY_NAME"));
	BoxSizer10->Add(CanvasVaryName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(BoxSizer10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Ширина"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer9->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	HorSize = new wxSpinCtrl(this, ID_SPINCTRL_HOR_SIZE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 5000, 0, _T("ID_SPINCTRL_HOR_SIZE"));
	HorSize->SetValue(_T("0"));
	BoxSizer9->Add(HorSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5->Add(BoxSizer9, 0, wxALL|wxEXPAND, 5);
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Высота"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer8->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	VertSize = new wxSpinCtrl(this, ID_SPINCTRL_VERT_SIZE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 5000, 0, _T("ID_SPINCTRL_VERT_SIZE"));
	VertSize->SetValue(_T("0"));
	BoxSizer8->Add(VertSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5->Add(BoxSizer8, 0, wxALL|wxEXPAND, 5);
	BoxSizer3->Add(BoxSizer5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer7 = new wxBoxSizer(wxVERTICAL);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Единицы измерения"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer7->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	MeasureUnitMM = new wxRadioButton(this, ID_RADIOBUTTON_MM, _("миллиметр"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_MM"));
	MeasureUnitMM->SetValue(true);
	BoxSizer7->Add(MeasureUnitMM, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	MeasureUnitInch = new wxRadioButton(this, ID_RADIOBUTTON_INCH, _("сотая дюйма"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_INCH"));
	BoxSizer7->Add(MeasureUnitInch, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6 = new wxBoxSizer(wxVERTICAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Соотношение сторон"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer6->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	AspectRatio = new wxTextCtrl(this, ID_TEXTCTRL_ASPECT_RATIO, _("1"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL_ASPECT_RATIO"));
	AspectRatio->SetMinSize(wxSize(50,-1));
	BoxSizer6->Add(AspectRatio, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FixedAspectRatio = new wxCheckBox(this, ID_CHECKBOX_FIXED_ASPECT_RATIO, _("Фикс"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_FIXED_ASPECT_RATIO"));
	FixedAspectRatio->SetValue(false);
	BoxSizer6->Add(FixedAspectRatio, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	BoxSizer3->Add(BoxSizer6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	ButtonSet = new wxButton(this, ID_BUTTON_SET, _("Установить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SET"));
	BoxSizer2->Add(ButtonSet, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonCancel = new wxButton(this, ID_BUTTON_CANCEL, _("Отменить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANCEL"));
	BoxSizer2->Add(ButtonCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonReset = new wxButton(this, ID_BUTTON_RESET, _("Сбросить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_RESET"));
	BoxSizer2->Add(ButtonReset, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonSetStandart = new wxButton(this, ID_BUTTON_STD_CANVAS, _("Стандартные"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_STD_CANVAS"));
	BoxSizer2->Add(ButtonSetStandart, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonSaveToFile = new wxButton(this, ID_BUTTON_SAVE_TO_FILE, _("Сохранить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SAVE_TO_FILE"));
	BoxSizer2->Add(ButtonSaveToFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonLoadFromFile = new wxButton(this, ID_BUTTON_LOAD_FROM_FILE, _("Загрузить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_LOAD_FROM_FILE"));
	BoxSizer2->Add(ButtonLoadFromFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&SetCanvasSize::OnCanvasVaryListSelect);
	Connect(ID_BUTTON_CANVAS_VARY_ADD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnCanvasVaryAddClick);
	Connect(ID_BUTTON_CANVAS_VARY_DELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnCanvasVaryDeleteClick);
	Connect(ID_BUTTON_CANVAS_VARY_NAME,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnCanvasVaryNameClick);
	Connect(ID_SPINCTRL_HOR_SIZE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&SetCanvasSize::OnHorSizeChange);
	Connect(ID_SPINCTRL_VERT_SIZE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&SetCanvasSize::OnVertSizeChange);
	Connect(ID_RADIOBUTTON_MM,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&SetCanvasSize::OnMeasureUnitMMSelect);
	Connect(ID_RADIOBUTTON_INCH,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&SetCanvasSize::OnMeasureUnitInchSelect);
	Connect(ID_TEXTCTRL_ASPECT_RATIO,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&SetCanvasSize::OnAspectRatioTextEnter);
	Connect(ID_CHECKBOX_FIXED_ASPECT_RATIO,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnFixedAspectRatioClick);
	Connect(ID_BUTTON_SET,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonSetClick);
	Connect(ID_BUTTON_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonCancelClick);
	Connect(ID_BUTTON_RESET,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonResetClick);
	Connect(ID_BUTTON_STD_CANVAS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonSetStandartClick);
	Connect(ID_BUTTON_SAVE_TO_FILE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonSaveToFileClick);
	Connect(ID_BUTTON_LOAD_FROM_FILE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SetCanvasSize::OnButtonLoadFromFileClick);
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

double SetCanvasSize::CorrectAspectRatioValue()
{ // Исправляем соотношение сторон холста так, чтобы оно укладывалось в интервал [CANVAS_ASPECT_MIN, CANVAS_ASPECT_MAX]
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
    return cnv_aspect_ratio;
}

void SetCanvasSize::SetSizesByCurrentAspect()
{ // Функция исправляет размеры холста в соответствии с фиксированным коэффициентом соотношения сторон
    if (FixedAspectRatio->GetValue())
    { // Операция выполняется, только если фиксация соотношения сторон включена
        double cnv_aspect_ratio = CorrectAspectRatioValue();
        double canvas_area = static_cast<double>(HorSize->GetValue()) * VertSize->GetValue();
        HorSize->SetValue(round(sqrt(canvas_area * cnv_aspect_ratio)));
        VertSize->SetValue(round(sqrt(canvas_area / cnv_aspect_ratio)));
    }
}

void SetCanvasSize::SetAspectByCurrentSizes()
{  // Функция вычисляет и устанавливает коэффициент соотношения, исходя из текущих размеров сторон холста
    double cnv_aspect_ratio = static_cast<double>(HorSize->GetValue()) / VertSize->GetValue();
    AspectRatio->SetValue(wxString(to_string(cnv_aspect_ratio).c_str(), wxConvUTF8));
}

void SetCanvasSize::UpdateCurrentCanvas()
{ // Обновляет данные текущего элемента списка нестандартных холстов, на который указывает current_canvas_ptr_.
    if (!current_canvas_ptr_ )
    {
        if (CanvasVaryList->GetCount())
        {
            CanvasVaryList->SetSelection(0);
            current_canvas_ptr_ = &canvas_list_[0];
        }
        else
        {
            return;
        }
    }

    current_canvas_ptr_->canvas_size = wxSize(HorSize->GetValue(), VertSize->GetValue());
    current_canvas_ptr_->is_metric = MeasureUnitMM->GetValue();
    current_canvas_ptr_->is_fixed_ratio = FixedAspectRatio->GetValue();
    if (int i = CanvasVaryList->GetSelection(); i != wxNOT_FOUND)
        CanvasVaryList->SetString(i, GetCanvasVaryListValue(i));
}

void SetCanvasSize::SwitchMeasureUnit(bool is_unit_metric)
{
    if (is_unit_metric)
    {
        HorSize->SetRange(std_wx_objects.MinCanvasSizeInMM.GetWidth(),
                          std_wx_objects.MaxCanvasSizeInMM.GetWidth());
        VertSize->SetRange(std_wx_objects.MinCanvasSizeInMM.GetHeight(),
                           std_wx_objects.MaxCanvasSizeInMM.GetHeight());
        if (!MeasureUnitMM->GetValue())
            MeasureUnitMM->SetValue(true);
    }
    else
    {
        HorSize->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetWidth(),
                          std_wx_objects.MaxCanvasSizeInPoints.GetWidth());
        VertSize->SetRange(std_wx_objects.MinCanvasSizeInPoints.GetHeight(),
                           std_wx_objects.MaxCanvasSizeInPoints.GetHeight());
        if (!MeasureUnitInch->GetValue())
            MeasureUnitInch->SetValue(true);
    }
}

void SetCanvasSize::ResetData()
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    canvas_list_ = this_app->vary_canvas_list;
    if (!canvas_list_.size())
        canvas_list_.push_back(VaryCanvasEditorStatus{});
    current_canvas_ptr_ = &canvas_list_[0];

    ConstructCanvasVaryList();
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
    EndModal(1);
}

void SetCanvasSize::OnHorSizeChange(wxSpinEvent& event)
{ // Обработчик изменения ширины холста. При фиксированном коэффе соотношения
  // пересчитывается вертикальный размер (высота), при плавающем коэффе - он сам.
    if (FixedAspectRatio->GetValue())
        VertSize->SetValue(round(HorSize->GetValue() / CorrectAspectRatioValue()));
    else
        SetAspectByCurrentSizes();

    UpdateCurrentCanvas();
}

void SetCanvasSize::OnVertSizeChange(wxSpinEvent& event)
{ // Обработчик изменения высоты холста. При фиксированном коэффе соотношения
  // пересчитывается горизонтальный размер (ширина), при плавающем коэффе - он сам.
    if (FixedAspectRatio->GetValue())
        HorSize->SetValue(round(VertSize->GetValue() * CorrectAspectRatioValue()));
    else
        SetAspectByCurrentSizes();

    UpdateCurrentCanvas();
}

void SetCanvasSize::OnAspectRatioKillFocus(wxFocusEvent& event)
{ // Изменение коэффа соотношения сторон холста. Пересчитываем обе стороны
  // применительно к новому значению коэффа.
    SetSizesByCurrentAspect();
    UpdateCurrentCanvas();
}

void SetCanvasSize::OnAspectRatioTextEnter(wxCommandEvent& event)
{ // Аналогично изменился коэффициент соотношения сторон холста. Пересчитываем
  // обе стороны применительно к новому значению коэффа.
    SetSizesByCurrentAspect();
    UpdateCurrentCanvas();
}

void SetCanvasSize::OnFixedAspectRatioClick(wxCommandEvent& event)
{
    if (FixedAspectRatio->GetValue())
        AspectRatio->Enable();
    else
        AspectRatio->Disable();

    SetAspectByCurrentSizes();
    UpdateCurrentCanvas();
}

void SetCanvasSize::OnMeasureUnitMMSelect(wxCommandEvent& event)
{
    SwitchMeasureUnit(true);
    HorSize->SetValue(round(HorSize->GetValue() / POINTS_IN_MILLIMETER));
    VertSize->SetValue(round(VertSize->GetValue() / POINTS_IN_MILLIMETER));
    SetAspectByCurrentSizes();
    UpdateCurrentCanvas();
}

void SetCanvasSize::OnMeasureUnitInchSelect(wxCommandEvent& event)
{
    SwitchMeasureUnit(false);
    HorSize->SetValue(round(HorSize->GetValue() * POINTS_IN_MILLIMETER));
    VertSize->SetValue(round(VertSize->GetValue() * POINTS_IN_MILLIMETER));
    SetAspectByCurrentSizes();
    UpdateCurrentCanvas();
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
        UpdateCurrentCanvas();
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
    UpdateCurrentCanvas();
}

wxString SetCanvasSize::GetCanvasVaryListValue(int list_index)
{ // Используя параметры холста с номером list_index в списке canvas_list_, функция формирует
  // соответствующую строку для списка CanvasVaryList.
    if (list_index >= static_cast<int>(canvas_list_.size()))
        return {};
    const VaryCanvasEditorStatus& current_canvas = canvas_list_[list_index];
    string current_canvas_desc;
    if (current_canvas.canvas_name.size())
        current_canvas_desc = current_canvas.canvas_name + ':';

    current_canvas_desc += to_string(current_canvas.canvas_size.GetWidth()) + 'x' +
                           to_string(current_canvas.canvas_size.GetHeight()) + ' ';
    if (current_canvas.is_metric)
        current_canvas_desc += 'M';
    else
        current_canvas_desc += 'I';

    current_canvas_desc += ' ';
    if (current_canvas.is_fixed_ratio)
        current_canvas_desc += 'F';
    else
        current_canvas_desc += 'N';

    return wxString(current_canvas_desc.c_str(), wxConvLocal);
}

void SetCanvasSize::ConstructCanvasVaryList()
{
    int current_list_selection = CanvasVaryList->GetSelection();
    CanvasVaryList->Clear();
    for (size_t i = 0; i < canvas_list_.size(); ++i)
        CanvasVaryList->Append(GetCanvasVaryListValue(i));

    if (current_list_selection == wxNOT_FOUND)
        current_list_selection = 0;
    else if (current_list_selection >= static_cast<int>(CanvasVaryList->GetCount()))
        current_list_selection = CanvasVaryList->GetCount() - 1;

    CanvasVaryList->SetSelection(current_list_selection);
    SetControlsByCanvasVaryList();
}

void SetCanvasSize::SetControlsByCanvasVaryList()
{
    if (CanvasVaryList->GetSelection() == wxNOT_FOUND)
        CanvasVaryList->SetSelection(0);

    current_canvas_ptr_ = &canvas_list_[CanvasVaryList->GetSelection()];
    HorSize->SetValue(current_canvas_ptr_->canvas_size.GetWidth());
    VertSize->SetValue(current_canvas_ptr_->canvas_size.GetHeight());
    FixedAspectRatio->SetValue(current_canvas_ptr_->is_fixed_ratio);
    SetAspectByCurrentSizes();
    if (FixedAspectRatio->GetValue())
        AspectRatio->Enable();
    else
        AspectRatio->Disable();

    if (current_canvas_ptr_->is_metric)
        MeasureUnitMM->SetValue(true);
    else
        MeasureUnitInch->SetValue(true);
}

void SetCanvasSize::OnCanvasVaryAddClick(wxCommandEvent& event)
{
    if (canvas_list_.size() < MAX_VARY_CANVASES)
    {
        VaryCanvasEditorStatus new_canvas;
        new_canvas.canvas_size = wxSize(HorSize->GetValue(), VertSize->GetValue());
        new_canvas.is_fixed_ratio = FixedAspectRatio->GetValue();
        new_canvas.is_metric = MeasureUnitMM->GetValue();
        canvas_list_.push_back(new_canvas);
        ConstructCanvasVaryList();
    }
}

void SetCanvasSize::OnCanvasVaryDeleteClick(wxCommandEvent& event)
{
    if (canvas_list_.size() > 1 && CanvasVaryList->GetSelection() != wxNOT_FOUND)
    {
        canvas_list_.erase(canvas_list_.begin() + CanvasVaryList->GetSelection());
        ConstructCanvasVaryList();
    }
}

void SetCanvasSize::OnCanvasVaryListSelect(wxCommandEvent& event)
{
    SetControlsByCanvasVaryList();
}

void SetCanvasSize::OnCanvasVaryNameClick(wxCommandEvent& event)
{
    if (canvas_list_.size() && CanvasVaryList->GetSelection() != wxNOT_FOUND)
    {
        string current_name = canvas_list_[CanvasVaryList->GetSelection()].canvas_name;
        wxTextEntryDialog text_entry_dialog(this, _("Ввод имени холста"), _("Имя холста"),
                                            wxString(current_name.c_str(), wxConvUTF8));
        if (text_entry_dialog.ShowModal() != wxID_OK)
            return;
        int i = CanvasVaryList->GetSelection();
        canvas_list_[i].canvas_name =
            string(text_entry_dialog.GetValue().mb_str()).substr(0, VARY_CANVASES_MAX_NAME_LEN);

        CanvasVaryList->SetString(i, GetCanvasVaryListValue(i));
    }
}

void SetCanvasSize::OnButtonSaveToFileClick(wxCommandEvent& event)
{
    wxFileDialog save_file_dialog(this, _("Сохранить коллекцию холстов"),
                                  wxEmptyString, wxEmptyString, _("холсты (*.txt)|*.txt"),
                                  wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    ofstream ofile(string(save_file_dialog.GetPath().mb_str()));
    for (size_t i = 0; i < canvas_list_.size(); ++i)
    {
        const VaryCanvasEditorStatus& current_canvas = canvas_list_[i];
        ofile << noboolalpha;
        ofile << "canvas=" << i << "\nname=" << current_canvas.canvas_name
              << "\nwidth=" << current_canvas.canvas_size.GetWidth()
              << "\nheight=" << current_canvas.canvas_size.GetHeight()
              << "\nis_metric=" << current_canvas.is_metric
              << "\nis_fixed_ratio=" << current_canvas.is_fixed_ratio << endl;
    }
}

void SetCanvasSize::OnButtonLoadFromFileClick(wxCommandEvent& event)
{
    wxFileDialog load_file_dialog(this, _("Открыть файл коллекции холстов"), {}, {},
                    _("холсты (*.txt)|*.txt|все файлы|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (load_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    ifstream ifile(string(load_file_dialog.GetPath().mb_str()));
    canvas_list_.clear();

    VaryCanvasEditorStatus current_canvas;
    int canvas_number = -1;

    while (ifile)
    {
        string cur_str;
        getline(ifile, cur_str);
        cur_str = TrimString(cur_str);
        if (!cur_str.size() || cur_str[0] == '#')
            continue;
        string param_name, param_value;
        size_t eq_pos = cur_str.find('=');
        if (eq_pos != string::npos)
        {
            param_name = TrimString(cur_str.substr(0, eq_pos));
            param_value = TrimString(cur_str.substr(eq_pos + 1));
        }
        else
        {
            param_name = cur_str;
        }

        if (param_name == "canvas")
        {
            if (canvas_number >= 0)
            {
                canvas_list_.push_back(current_canvas);
                current_canvas = VaryCanvasEditorStatus{};
            }
            canvas_number = stoi(param_value);
        }
        else if (param_name == "name" && canvas_number >= 0)
        {
            current_canvas.canvas_name = param_value;
        }
        else if (param_name == "width" && canvas_number >= 0)
        {
            current_canvas.canvas_size.SetWidth(stoi(param_value));
        }
        else if (param_name == "height" && canvas_number >= 0)
        {
            current_canvas.canvas_size.SetHeight(stoi(param_value));
        }
        else if (param_name == "is_metric" && canvas_number >= 0)
        {
            current_canvas.is_metric = stoi(param_value);
        }
        else if (param_name == "is_fixed_ratio" && canvas_number >= 0)
        {
            current_canvas.is_fixed_ratio = stoi(param_value);
        }
        else
        {
            wxMessageBox(_("Неверный формат файла"), _("Ошибка при загрузке"));
            return;
        }
    }

    if (canvas_number >= 0)
        canvas_list_.push_back(current_canvas);
    if (!canvas_list_.size())
        canvas_list_.push_back(VaryCanvasEditorStatus{});
    current_canvas_ptr_ = &canvas_list_[0];

    ConstructCanvasVaryList();
}
