#include "wx_pch.h"
#include "OptionsDialog.h"
#include "ApertureProvider.h"
#include "PCADViewerApp.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(OptionsDialog)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(OptionsDialog)
//*)

using namespace std;
using namespace aperture;

//(*IdInit(OptionsDialog)
const long OptionsDialog::ID_STATICTEXT2 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL_APERTURE_FILENAME = wxNewId();
const long OptionsDialog::ID_BUTTON_LOAD_APERTURE = wxNewId();
const long OptionsDialog::ID_BUTTONAPERTURE_DEFAULT = wxNewId();
const long OptionsDialog::ID_STATICTEXT3 = wxNewId();
const long OptionsDialog::ID_COMBOBOX_APERTURE_SECTION = wxNewId();
const long OptionsDialog::ID_LISTBOXAPERTURES_LIST = wxNewId();
const long OptionsDialog::ID_RADIOBUTTON_USE_DBU = wxNewId();
const long OptionsDialog::ID_RADIOBUTTON_USE_MILLIMETER = wxNewId();
const long OptionsDialog::ID_RADIOBUTTON_USE_INCH = wxNewId();
const long OptionsDialog::ID_RADIOBUTTON_ADDIT_SCALE_OFF = wxNewId();
const long OptionsDialog::ID_RADIOBUTTON_ADDIT_SCALE_RUSSIAN_INCH = wxNewId();
const long OptionsDialog::ID_RADIOBUTTON_ADDIT_SCALE_ANY = wxNewId();
const long OptionsDialog::ID_STATICTEXT4 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL_UNIT_SCALE_VALUE_X = wxNewId();
const long OptionsDialog::ID_STATICTEXT5 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL_UNIT_SCALE_VALUE_Y = wxNewId();
const long OptionsDialog::ID_CHECKBOX_SCREEN_SUBSCALE = wxNewId();
const long OptionsDialog::ID_CHECKBOX_OWN_TEXT_ENGINE = wxNewId();
const long OptionsDialog::ID_BUTTON_OPTION_OK = wxNewId();
const long OptionsDialog::ID_BUTTON_OPTION_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(OptionsDialog,wxDialog)
	//(*EventTable(OptionsDialog)
	//*)
END_EVENT_TABLE()

OptionsDialog::OptionsDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(OptionsDialog)
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
	wxGridSizer* GridSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;

	Create(parent, wxID_ANY, _("Параметры программы"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, _("Управление апертурами (засветками)"));
	BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT2, _("Имя файла апертур"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer4->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextApertureFilename = new wxTextCtrl(this, ID_TEXTCTRL_APERTURE_FILENAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL_APERTURE_FILENAME"));
	BoxSizer4->Add(TextApertureFilename, 0, wxALL|wxEXPAND, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	ButtonLoadAperture = new wxButton(this, ID_BUTTON_LOAD_APERTURE, _("Загрузить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_LOAD_APERTURE"));
	BoxSizer8->Add(ButtonLoadAperture, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonApertureDefault = new wxButton(this, ID_BUTTONAPERTURE_DEFAULT, _("По умолчанию"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONAPERTURE_DEFAULT"));
	BoxSizer8->Add(ButtonApertureDefault, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(BoxSizer8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer9->Add(BoxSizer4, 1, wxALL|wxEXPAND, 5);
	BoxSizer6 = new wxBoxSizer(wxVERTICAL);
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT3, _("Множество"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer7->Add(StaticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ComboBoxApertureSection = new wxComboBox(this, ID_COMBOBOX_APERTURE_SECTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY|wxCB_DROPDOWN, wxDefaultValidator, _T("ID_COMBOBOX_APERTURE_SECTION"));
	ComboBoxApertureSection->Append(_("GERBER 32"));
	ComboBoxApertureSection->Append(_("GERBER LASER"));
	BoxSizer7->Add(ComboBoxApertureSection, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6->Add(BoxSizer7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ListBoxApertures = new wxListBox(this, ID_LISTBOXAPERTURES_LIST, wxDefaultPosition, wxSize(173,70), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOXAPERTURES_LIST"));
	BoxSizer6->Add(ListBoxApertures, 1, wxALL|wxSHAPED, 5);
	BoxSizer9->Add(BoxSizer6, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer3->Add(BoxSizer9, 2, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(StaticBoxSizer3, 6, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Используемые единицы координат"));
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	RadioUseDBU = new wxRadioButton(this, ID_RADIOBUTTON_USE_DBU, _("DBU"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON_USE_DBU"));
	RadioUseDBU->SetHelpText(_("Измерение координат в DBU - единицах базы данных рисунка"));
	BoxSizer5->Add(RadioUseDBU, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	RadioUseMillimeter = new wxRadioButton(this, ID_RADIOBUTTON_USE_MILLIMETER, _("Миллиметр"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_USE_MILLIMETER"));
	RadioUseMillimeter->SetHelpText(_("Измерение координат в миллиметрах"));
	BoxSizer5->Add(RadioUseMillimeter, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	RadioUseInch = new wxRadioButton(this, ID_RADIOBUTTON_USE_INCH, _("Дюйм"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_USE_INCH"));
	RadioUseInch->SetHelpText(_("Измерение координат в дюймах"));
	BoxSizer5->Add(RadioUseInch, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Дополнительный масштабный коэффициент"));
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	RadioAdditionalUnitScaleOff = new wxRadioButton(this, ID_RADIOBUTTON_ADDIT_SCALE_OFF, _("Отключить"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON_ADDIT_SCALE_OFF"));
	BoxSizer2->Add(RadioAdditionalUnitScaleOff, 1, wxALL|wxEXPAND, 5);
	RadioAdditionalUnitScaleRussianInch = new wxRadioButton(this, ID_RADIOBUTTON_ADDIT_SCALE_RUSSIAN_INCH, _("\"Русский дюйм\""), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_ADDIT_SCALE_RUSSIAN_INCH"));
	BoxSizer2->Add(RadioAdditionalUnitScaleRussianInch, 1, wxALL|wxEXPAND, 5);
	RadioAdditionalUnitScaleAny = new wxRadioButton(this, ID_RADIOBUTTON_ADDIT_SCALE_ANY, _("Произвольно"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON_ADDIT_SCALE_ANY"));
	BoxSizer2->Add(RadioAdditionalUnitScaleAny, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer2->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1 = new wxGridSizer(2, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT4, _("По X:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	GridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextAdditionalUnitScaleXValue = new wxTextCtrl(this, ID_TEXTCTRL_UNIT_SCALE_VALUE_X, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_UNIT_SCALE_VALUE_X"));
	TextAdditionalUnitScaleXValue->SetHelpText(_("Установка дополнительного масштабного коэффициента"));
	GridSizer1->Add(TextAdditionalUnitScaleXValue, 1, wxALL|wxEXPAND, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("По Y:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	GridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextAdditionalUnitScaleYValue = new wxTextCtrl(this, ID_TEXTCTRL_UNIT_SCALE_VALUE_Y, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_UNIT_SCALE_VALUE_Y"));
	GridSizer1->Add(TextAdditionalUnitScaleYValue, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(GridSizer1, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer2, 4, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	CheckBoxScreenSubscale = new wxCheckBox(this, ID_CHECKBOX_SCREEN_SUBSCALE, _("Подмасштабы для экрана"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_SCREEN_SUBSCALE"));
	CheckBoxScreenSubscale->SetValue(false);
	CheckBoxScreenSubscale->SetHelpText(_("Выводить изображение на экран с учётом его физического разрешения"));
	BoxSizer10->Add(CheckBoxScreenSubscale, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CheckBoxOwnTextEngine = new wxCheckBox(this, ID_CHECKBOX_OWN_TEXT_ENGINE, _("Использовать собственный текстовый движок"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_OWN_TEXT_ENGINE"));
	CheckBoxOwnTextEngine->SetValue(false);
	BoxSizer10->Add(CheckBoxOwnTextEngine, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	ButtonOptionOk = new wxButton(this, ID_BUTTON_OPTION_OK, _("Принять"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_OPTION_OK"));
	BoxSizer3->Add(ButtonOptionOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonOptionCancel = new wxButton(this, ID_BUTTON_OPTION_CANCEL, _("Отменить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_OPTION_CANCEL"));
	BoxSizer3->Add(ButtonOptionCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON_LOAD_APERTURE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::OnButtonLoadApertureClick);
	Connect(ID_BUTTONAPERTURE_DEFAULT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::OnButtonApertureDefaultClick);
	Connect(ID_COMBOBOX_APERTURE_SECTION,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&OptionsDialog::OnComboBoxApertureSectionSelected);
	Connect(ID_RADIOBUTTON_ADDIT_SCALE_OFF,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&OptionsDialog::OnRadioAdditionalUnitScaleOffSelect);
	Connect(ID_RADIOBUTTON_ADDIT_SCALE_RUSSIAN_INCH,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&OptionsDialog::OnRadioAdditionalUnitScaleRussianInchSelect);
	Connect(ID_RADIOBUTTON_ADDIT_SCALE_ANY,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&OptionsDialog::OnRadioAdditionalUnitScaleAnySelect);
	Connect(ID_BUTTON_OPTION_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::OnButtonOptionOkClick);
	Connect(ID_BUTTON_OPTION_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::OnButtonOptionCancelClick);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&OptionsDialog::OnInit);
	//*)
}

OptionsDialog::~OptionsDialog()
{
	//(*Destroy(OptionsDialog)
	//*)
}

void OptionsDialog::UpdateApertureList()
{
    string dblpnt = " : ";

    ListBoxApertures->Clear();
    for (auto flash_desc_it = temp_aperture_provider.begin();
         flash_desc_it != temp_aperture_provider.end(); ++flash_desc_it)
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
        ListBoxApertures->Append(wxString(tx_fd.c_str(), wxConvUTF8));
    }
}

void OptionsDialog::OnButtonOptionOkClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void OptionsDialog::OnButtonOptionCancelClick(wxCommandEvent& event)
{
    EndModal(0);
}

void OptionsDialog::OnRadioAdditionalUnitScaleOffSelect(wxCommandEvent& event)
{
    TextAdditionalUnitScaleXValue->Enable(false);
    TextAdditionalUnitScaleYValue->Enable(false);
}

void OptionsDialog::OnRadioAdditionalUnitScaleRussianInchSelect(wxCommandEvent& event)
{
    TextAdditionalUnitScaleXValue->Enable(false);
    TextAdditionalUnitScaleYValue->Enable(false);
    wxString russian_inch_scale = wxString(to_string(RUSSIAN_INCH_SCALE).c_str(), wxConvUTF8);
    TextAdditionalUnitScaleXValue->SetValue(russian_inch_scale);
    TextAdditionalUnitScaleYValue->SetValue(russian_inch_scale);
}

void OptionsDialog::OnRadioAdditionalUnitScaleAnySelect(wxCommandEvent& event)
{
    TextAdditionalUnitScaleXValue->Enable();
    TextAdditionalUnitScaleYValue->Enable();
}

void OptionsDialog::OnButtonLoadApertureClick(wxCommandEvent& event)
{
    wxFileDialog load_aperture_dialog(this, _("Открыть файл апертур PCAD"), {}, {},
                    wxString(_("Файлы апертур PC_PHOTO|*.apr")) + wxString(_("|Файлы апертур CAM350|*.gap")),
                    wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (load_aperture_dialog.ShowModal() == wxID_CANCEL)
        return;

    wxString error_label = _("Ошибка");
    ApertureLoadlInfo load_info;
    load_info.aper_file_type = ApertureFileType::APERTURE_FILE_AUTO;
    load_info.aperture_file_path = string(load_aperture_dialog.GetPath().mb_str());
    load_info.using_aperture_type = UsingApertureType::USING_APERTURE_GERBER32;

    ifstream load_aperture_stream(load_info.aperture_file_path);
    if (!load_aperture_stream)
    {
        wxMessageBox(_("Файл апертур не найден или ошибка при его открытии"), error_label);
        return;
    }
    load_info.aper_file_stream_ptr = &load_aperture_stream;

    PCADLoadError process_error_code = temp_aperture_provider.LoadApertureFile(load_info);

    if (process_error_code != PCADLoadError::LOAD_FILE_NO_ERROR)
    {
        wxMessageBox(_("Ошибка при чтении файла апертур или он имеет неверный формат"), error_label);
        return;
    }

    TextApertureFilename->SetValue(load_aperture_dialog.GetPath());
    if (ComboBoxApertureSection->GetValue() == wxT("GERBER LASER"))
        temp_aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER_LASER);
    else
        temp_aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER32);

    UpdateApertureList();
}

void OptionsDialog::OnButtonApertureDefaultClick(wxCommandEvent& event)
{
    if (wxMessageBox(_("Загрузить апертуры по умолчанию?"), _("Есть вопрос"), wxYES_NO) != wxYES)
        return;

    temp_aperture_provider.LoadApertureFile(ApertureLoadlInfo{});
    TextApertureFilename->Clear();
    if (ComboBoxApertureSection->GetValue() == wxT("GERBER LASER"))
        temp_aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER_LASER);
    else
        temp_aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER32);

    UpdateApertureList();
}

void OptionsDialog::OnComboBoxApertureSectionSelected(wxCommandEvent& event)
{
    if (ComboBoxApertureSection->GetValue() == wxT("GERBER LASER"))
        temp_aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER_LASER);
    else
        temp_aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER32);

    UpdateApertureList();
}

void OptionsDialog::OnInit(wxInitDialogEvent& event)
{
    UpdateApertureList();
}
