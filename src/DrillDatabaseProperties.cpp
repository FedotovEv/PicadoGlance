#include "DrillDatabaseProperties.h"

//(*InternalHeaders(DrillDatabaseProperties)
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DrillDatabaseProperties)
const wxWindowID DrillDatabaseProperties::ID_COMBO_DATABASE_ENCODING = wxNewId();
const wxWindowID DrillDatabaseProperties::ID_CHECKBOX_TBL_FORMAT_STRICT = wxNewId();
const wxWindowID DrillDatabaseProperties::ID_CHECKBOX_SORTING_DRILL_FILE = wxNewId();
//*)

BEGIN_EVENT_TABLE(DrillDatabaseProperties,wxDialog)
    //(*EventTable(DrillDatabaseProperties)
    //*)
END_EVENT_TABLE()

#include "redefine_.h"

using namespace std;

DrillDatabaseProperties::DrillDatabaseProperties(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(DrillDatabaseProperties)
    wxBoxSizer* DrillDatabaseOptionsSizer;
    wxBoxSizer* DrillDatabasePropMainSizer;
    wxStaticBoxSizer* DrillDatabaseEncodingSizer;
    wxStdDialogButtonSizer* DrillDatabasePropsButtons;

    Create(parent, wxID_ANY, _("Свойства базы данных инструментов сверловки"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    DrillDatabasePropMainSizer = new wxBoxSizer(wxVERTICAL);
    DrillDatabaseEncodingSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Кодировка информационных строк TBL-файла"));
    DrillDatabaseEncodingCombo = new wxComboBox(DrillDatabaseEncodingSizer->GetStaticBox(), ID_COMBO_DATABASE_ENCODING, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBO_DATABASE_ENCODING"));
    DrillDatabaseEncodingSizer->Add(DrillDatabaseEncodingCombo, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillDatabasePropMainSizer->Add(DrillDatabaseEncodingSizer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillDatabaseOptionsSizer = new wxBoxSizer(wxVERTICAL);
    DrillTBLDatabaseStrictCheck = new wxCheckBox(this, ID_CHECKBOX_TBL_FORMAT_STRICT, _("Строгий стандарт PCAD4 TBL"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_TBL_FORMAT_STRICT"));
    DrillTBLDatabaseStrictCheck->SetValue(false);
    DrillDatabaseOptionsSizer->Add(DrillTBLDatabaseStrictCheck, 1, wxALL|wxALIGN_LEFT, 5);
    DrillFileSortingCheck = new wxCheckBox(this, ID_CHECKBOX_SORTING_DRILL_FILE, _("Сортировка файла сверловки"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_SORTING_DRILL_FILE"));
    DrillFileSortingCheck->SetValue(false);
    DrillDatabaseOptionsSizer->Add(DrillFileSortingCheck, 1, wxALL|wxALIGN_LEFT, 5);
    DrillDatabasePropMainSizer->Add(DrillDatabaseOptionsSizer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillDatabasePropsButtons = new wxStdDialogButtonSizer();
    DrillDatabasePropsButtons->AddButton(new wxButton(this, wxID_OK, _("Принять")));
    DrillDatabasePropsButtons->AddButton(new wxButton(this, wxID_CANCEL, _("Отменить")));
    DrillDatabasePropsButtons->Realize();
    dynamic_cast <wxButton *> (this->FindWindow(wxID_OK))->SetDefault();
    DrillDatabasePropMainSizer->Add(DrillDatabasePropsButtons, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    SetSizer(DrillDatabasePropMainSizer);
    DrillDatabasePropMainSizer->SetSizeHints(this);

    Connect(ID_COMBO_DATABASE_ENCODING, wxEVT_COMMAND_COMBOBOX_SELECTED, (wxObjectEventFunction)&DrillDatabaseProperties::OnDrillDatabaseEncodingComboSelected);
    Connect(ID_CHECKBOX_TBL_FORMAT_STRICT, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&DrillDatabaseProperties::OnDrillTBLDatabaseStrictCheckClick);
    Connect(ID_CHECKBOX_SORTING_DRILL_FILE, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&DrillDatabaseProperties::OnDrillFileSortingCheckClick);
    //*)

    // Заполнение списка DrillDatabaseEncodingCombo выбора кодировок информационных строк TBL-файлов их доступными значениями.
    for (int enc_index = 1; enc_index <= static_cast<int>(TBLFileServer::DrillFileStringEncoding::ENCODE_TEXT_MAX); ++enc_index)
        DrillDatabaseEncodingCombo->AppendText
            (wxString::FromUTF8
                (TBLFileServer::DatabaseTextEncodingToString(static_cast<TBLFileServer::DrillFileStringEncoding>(enc_index)).c_str())
            );
}

DrillDatabaseProperties::~DrillDatabaseProperties()
{
    //(*Destroy(DrillDatabaseProperties)
    //*)
}

void DrillDatabaseProperties::SetTBLDataServer(TBLFileServer* p_use_tbl_server)
{
    m_use_tbl_server = p_use_tbl_server;
    if (m_use_tbl_server)
    { // Установим органы управления нашего диалога в соответствии с конфигурацией, хранящейся в сервере m_use_tbl_server.
        m_property_saver = m_use_tbl_server->GetDrillDatabaseProperties();
        DrillFileSortingCheck->SetValue(m_property_saver.is_sorting_during_generate);
        DrillTBLDatabaseStrictCheck->SetValue(m_property_saver.is_PCAD4_TBL_strict_format);

        std::string encoding_text = TBLFileServer::DatabaseTextEncodingToString(m_property_saver.text_encoding);
        if (int encoding_combo_index = DrillDatabaseEncodingCombo->FindString(wxFS8(encoding_text)); encoding_combo_index >= 0)
            DrillDatabaseEncodingCombo->SetSelection(encoding_combo_index);
        else
            DrillDatabaseEncodingCombo->SetSelection(-1);
    }
}

void DrillDatabaseProperties::OnDrillTBLDatabaseStrictCheckClick(wxCommandEvent& event)
{
    m_property_saver.is_PCAD4_TBL_strict_format = DrillTBLDatabaseStrictCheck->GetValue();
}

void DrillDatabaseProperties::OnDrillFileSortingCheckClick(wxCommandEvent& event)
{
    m_property_saver.is_sorting_during_generate = DrillFileSortingCheck->GetValue();
}

void DrillDatabaseProperties::OnDrillDatabaseEncodingComboSelected(wxCommandEvent& event)
{
    wxString text_encoding_name = DrillDatabaseEncodingCombo->GetValue();
    if (!text_encoding_name.empty())
    {
        m_property_saver.text_encoding =
            TBLFileServer::DatabaseTextEncodingFromString(string(text_encoding_name.mb_str(wxConvUTF8)));
    }
}
