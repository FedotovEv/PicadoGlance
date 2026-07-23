#include "wx_pch.h"
#include "EditDrillBitsProperties.h"

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(EditDrillBitsProperties)
    #include <wx/intl.h>
    #include <wx/string.h>
    //*)
#endif
//(*InternalHeaders(EditDrillBitsProperties)
//*)

#include "redefine_.h"

using namespace std;

// Переводимые строковые литералы с сообщениями об ошибках.
static constexpr char ERROR_FIELD_VALUE[] = wxTRANSLATE("Недопустимое значение поля");
static constexpr char DRILL_BIT_MUST_NUMBER[] = wxTRANSLATE("Номер сверла должен быть целым числом");
static constexpr char DRILL_BIT_NUMBER_LESSER[] = wxTRANSLATE("Номер сверла должен быть неотрицательным и не превышать ");
static constexpr char DRILL_BIT_ABSENT_IN_BASE[] = wxTRANSLATE("Сверло с таким номером не описано в инструментальной таблице");
static constexpr char DRILL_BIT_DUPLICATES[] = wxTRANSLATE("Сверло с таким номером уже есть в таблице");
static constexpr char DRILL_BIT_DIAMETER_MUST_NUMBER[] = wxTRANSLATE("Диметр сверла должен быть числом");
static constexpr char DRILL_BIT_DIAMETER_LESSER[] = wxTRANSLATE("Диаметр сверла не должен превышать ");

//(*IdInit(EditDrillBitsProperties)
const wxWindowID EditDrillBitsProperties::ID_STATIC_EXPLAIN_DATA_TABLE = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_TEXT_EXPLAIN_TABLE_TEXT = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_STATICTEXT1 = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_STATICTEXT2 = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_GRID_PIN_TO_DRILL_BIT = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_GRID_DRILL_BIT_DIAMETER = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_STATICTEXT3 = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_SPIN_PIN_COUNT = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_ADD_NEW_DRILL_BIT = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_DELETE_DRILL_BIT = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_DRILL_BIT_PROP_SAVE_TBL = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_DRILL_BIT_PROP_LOAD_TBL = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_DRILL_BIT_PROP_SAVE_INI = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_DRILL_BIT_PROP_LOAD_INI = wxNewId();
const wxWindowID EditDrillBitsProperties::ID_BUTTON_DRILL_BIT_PROP_EXIT = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditDrillBitsProperties,wxDialog)
    //(*EventTable(EditDrillBitsProperties)
    //*)
END_EVENT_TABLE()

EditDrillBitsProperties::EditDrillBitsProperties(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(EditDrillBitsProperties)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer3;
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* DrillBitButtonFooterSizer;
    wxBoxSizer* DrillBitTablesSizer;
    wxBoxSizer* DrillBitTablesTitlesSizer;
    wxBoxSizer* MainDrillBitPropSizer;

    Create(parent, wxID_ANY, _("Редактор инструментов сверления"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(700,370));
    MainDrillBitPropSizer = new wxBoxSizer(wxVERTICAL);
    BoxSizer5 = new wxBoxSizer(wxVERTICAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    ExplainDrillBitStatic = new wxStaticText(this, ID_STATIC_EXPLAIN_DATA_TABLE, _("Пояснительный текст инструментальной таблицы"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATIC_EXPLAIN_DATA_TABLE"));
    BoxSizer4->Add(ExplainDrillBitStatic, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer4, 0, wxALL|wxEXPAND, 5);
    ExplainDrillBitText = new wxTextCtrl(this, ID_TEXT_EXPLAIN_TABLE_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXT_EXPLAIN_TABLE_TEXT"));
    BoxSizer5->Add(ExplainDrillBitText, 1, wxALL|wxEXPAND, 5);
    MainDrillBitPropSizer->Add(BoxSizer5, 0, wxALL|wxEXPAND, 5);
    DrillBitTablesTitlesSizer = new wxBoxSizer(wxHORIZONTAL);
    PinDrillBitHeaderText = new wxStaticText(this, ID_STATICTEXT1, _("Назначение сверл выводам"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
    DrillBitTablesTitlesSizer->Add(PinDrillBitHeaderText, 10, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitTablesTitlesSizer->Add(-1,-1,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitPropHeaderText = new wxStaticText(this, ID_STATICTEXT2, _("Параметры сверл"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
    DrillBitTablesTitlesSizer->Add(DrillBitPropHeaderText, 10, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    MainDrillBitPropSizer->Add(DrillBitTablesTitlesSizer, 0, wxALL|wxEXPAND, 5);
    DrillBitTablesSizer = new wxBoxSizer(wxHORIZONTAL);
    PinToDrilBitlNumberGrid = new wxGrid(this, ID_GRID_PIN_TO_DRILL_BIT, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GRID_PIN_TO_DRILL_BIT"));
    PinToDrilBitlNumberGrid->CreateGrid(10,1);
    PinToDrilBitlNumberGrid->EnableEditing(true);
    PinToDrilBitlNumberGrid->EnableGridLines(true);
    PinToDrilBitlNumberGrid->SetRowLabelSize(100);
    PinToDrilBitlNumberGrid->SetDefaultColSize(200, true);
    PinToDrilBitlNumberGrid->SetColLabelValue(0, _("Номер сверла"));
    PinToDrilBitlNumberGrid->SetDefaultCellFont( PinToDrilBitlNumberGrid->GetFont() );
    PinToDrilBitlNumberGrid->SetDefaultCellTextColour( PinToDrilBitlNumberGrid->GetForegroundColour() );
    DrillBitTablesSizer->Add(PinToDrilBitlNumberGrid, 10, wxLEFT|wxEXPAND, 5);
    DrillBitTablesSizer->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitDiameterGrid = new wxGrid(this, ID_GRID_DRILL_BIT_DIAMETER, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GRID_DRILL_BIT_DIAMETER"));
    DrillBitDiameterGrid->CreateGrid(10,2);
    DrillBitDiameterGrid->EnableEditing(true);
    DrillBitDiameterGrid->EnableGridLines(true);
    DrillBitDiameterGrid->SetRowLabelSize(1);
    DrillBitDiameterGrid->SetDefaultColSize(200, true);
    DrillBitDiameterGrid->SetColLabelValue(0, _("Номер сверла"));
    DrillBitDiameterGrid->SetColLabelValue(1, _("Диаметр сверла"));
    DrillBitDiameterGrid->SetDefaultCellFont( DrillBitDiameterGrid->GetFont() );
    DrillBitDiameterGrid->SetDefaultCellTextColour( DrillBitDiameterGrid->GetForegroundColour() );
    DrillBitTablesSizer->Add(DrillBitDiameterGrid, 10, wxRIGHT|wxEXPAND, 5);
    MainDrillBitPropSizer->Add(DrillBitTablesSizer, 1, wxEXPAND, 5);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT3, _("Количество типов выводов"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    BoxSizer3->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PinCountSpin = new wxSpinCtrl(this, ID_SPIN_PIN_COUNT, _T("10"), wxDefaultPosition, wxDefaultSize, 0, 0, 3000, 10, _T("ID_SPIN_PIN_COUNT"));
    PinCountSpin->SetValue(_T("10"));
    BoxSizer3->Add(PinCountSpin, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    AddNewDrillBit = new wxButton(this, ID_BUTTON_ADD_NEW_DRILL_BIT, _("Добавить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_ADD_NEW_DRILL_BIT"));
    BoxSizer2->Add(AddNewDrillBit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DeleteDrillBit = new wxButton(this, ID_BUTTON_DELETE_DRILL_BIT, _("Удалить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DELETE_DRILL_BIT"));
    BoxSizer2->Add(DeleteDrillBit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    MainDrillBitPropSizer->Add(BoxSizer1, 0, wxALL|wxEXPAND, 5);
    DrillBitButtonFooterSizer = new wxBoxSizer(wxHORIZONTAL);
    DrillBitPropSaveTBL = new wxButton(this, ID_BUTTON_DRILL_BIT_PROP_SAVE_TBL, _("Сохранить как TBL"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DRILL_BIT_PROP_SAVE_TBL"));
    DrillBitButtonFooterSizer->Add(DrillBitPropSaveTBL, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitPropLoadTBL = new wxButton(this, ID_BUTTON_DRILL_BIT_PROP_LOAD_TBL, _("Загрузить из TBL"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DRILL_BIT_PROP_LOAD_TBL"));
    DrillBitButtonFooterSizer->Add(DrillBitPropLoadTBL, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitPropSaveINI = new wxButton(this, ID_BUTTON_DRILL_BIT_PROP_SAVE_INI, _("Сохранить как INI"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DRILL_BIT_PROP_SAVE_INI"));
    DrillBitButtonFooterSizer->Add(DrillBitPropSaveINI, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitPropLoadINI = new wxButton(this, ID_BUTTON_DRILL_BIT_PROP_LOAD_INI, _("Загрузить из INI"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DRILL_BIT_PROP_LOAD_INI"));
    DrillBitButtonFooterSizer->Add(DrillBitPropLoadINI, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DrillBitPropExit = new wxButton(this, ID_BUTTON_DRILL_BIT_PROP_EXIT, _("Выйти"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DRILL_BIT_PROP_EXIT"));
    DrillBitButtonFooterSizer->Add(DrillBitPropExit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    MainDrillBitPropSizer->Add(DrillBitButtonFooterSizer, 0, wxEXPAND, 5);
    SetSizer(MainDrillBitPropSizer);
    Layout();
    Center();

    Connect(ID_TEXT_EXPLAIN_TABLE_TEXT, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&EditDrillBitsProperties::OnExplainDrillBitTextText);
    Connect(ID_TEXT_EXPLAIN_TABLE_TEXT, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)&EditDrillBitsProperties::OnExplainDrillBitTextTextEnter);
    Connect(ID_GRID_PIN_TO_DRILL_BIT, wxEVT_GRID_CELL_CHANGED, (wxObjectEventFunction)&EditDrillBitsProperties::OnPinToDrilBitlNumberGridCellChanged);
    Connect(ID_GRID_PIN_TO_DRILL_BIT, wxEVT_GRID_CELL_CHANGING, (wxObjectEventFunction)&EditDrillBitsProperties::OnPinToDrilBitlNumberGridCellChanging);
    Connect(ID_GRID_PIN_TO_DRILL_BIT, wxEVT_GRID_SELECT_CELL, (wxObjectEventFunction)&EditDrillBitsProperties::OnPinToDrilBitlNumberGridCellSelect);
    Connect(ID_GRID_DRILL_BIT_DIAMETER, wxEVT_GRID_LABEL_LEFT_CLICK, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitDiameterGridLabelLeftClick);
    Connect(ID_GRID_DRILL_BIT_DIAMETER, wxEVT_GRID_CELL_CHANGED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitDiameterGridCellChanged);
    Connect(ID_GRID_DRILL_BIT_DIAMETER, wxEVT_GRID_CELL_CHANGING, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitDiameterGridCellChanging);
    Connect(ID_GRID_DRILL_BIT_DIAMETER, wxEVT_GRID_SELECT_CELL, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitDiameterGridCellSelect);
    Connect(ID_SPIN_PIN_COUNT, wxEVT_COMMAND_SPINCTRL_UPDATED, (wxObjectEventFunction)&EditDrillBitsProperties::OnPinCountSpinChange);
    Connect(ID_BUTTON_ADD_NEW_DRILL_BIT, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnAddNewDrillBitClick);
    Connect(ID_BUTTON_DELETE_DRILL_BIT, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDeleteDrillBitClick);
    Connect(ID_BUTTON_DRILL_BIT_PROP_SAVE_TBL, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitPropSaveClick);
    Connect(ID_BUTTON_DRILL_BIT_PROP_LOAD_TBL, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitPropLoadClick);
    Connect(ID_BUTTON_DRILL_BIT_PROP_SAVE_INI, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitPropSaveINIClick);
    Connect(ID_BUTTON_DRILL_BIT_PROP_LOAD_INI, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitPropLoadINIClick);
    Connect(ID_BUTTON_DRILL_BIT_PROP_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&EditDrillBitsProperties::OnDrillBitPropExitClick);
    //*)

    // Перехват события потери фокуса ввода для некоторых виджетов, для которых нам нужно его обрабатывать.
    ExplainDrillBitText->Connect(wxEVT_KILL_FOCUS, (wxObjectEventFunction)&EditDrillBitsProperties::OnExplainDrillBitTextKillFocus, nullptr, this);

    // Настройка колонок таблиц на требуемый тип и формат значений.
    PinToDrilBitlNumberGrid->SetColFormatNumber(0);
    DrillBitDiameterGrid->SetColFormatNumber(0);
    DrillBitDiameterGrid->SetColFormatFloat(1, -1, 3);

    ReloadDataFromServer();
}

EditDrillBitsProperties::~EditDrillBitsProperties()
{
    //(*Destroy(EditDrillBitsProperties)
    //*)
}

void EditDrillBitsProperties::SetTBLDataServer(TBLFileServer* p_use_tbl_server)
{
    bool try_reload_server_data = m_use_tbl_server != p_use_tbl_server;
    m_use_tbl_server = p_use_tbl_server;
    if (m_use_tbl_server && try_reload_server_data)
        ReloadDataFromServer();
}

void EditDrillBitsProperties::SetGridsRows(int pin_dril_bit_rows, int drill_bit_params_rows)
{
    if (pin_dril_bit_rows >= 0)
    {
        if (PinToDrilBitlNumberGrid->GetNumberRows() > pin_dril_bit_rows)
        {
            int delta_rows = PinToDrilBitlNumberGrid->GetNumberRows() - pin_dril_bit_rows;
            PinToDrilBitlNumberGrid->DeleteRows(PinToDrilBitlNumberGrid->GetNumberRows() - delta_rows, delta_rows);
        }
        else
        {
            int delta_rows = pin_dril_bit_rows - PinToDrilBitlNumberGrid->GetNumberRows();
            PinToDrilBitlNumberGrid->InsertRows(PinToDrilBitlNumberGrid->GetNumberRows(), delta_rows);
        }
    }

    if (drill_bit_params_rows >= 0)
    {
        if (DrillBitDiameterGrid->GetNumberRows() > drill_bit_params_rows)
        {
            int delta_rows = DrillBitDiameterGrid->GetNumberRows() - drill_bit_params_rows;
            DrillBitDiameterGrid->DeleteRows(DrillBitDiameterGrid->GetNumberRows() - delta_rows, delta_rows);
        }
        else
        {
            int delta_rows = drill_bit_params_rows - DrillBitDiameterGrid->GetNumberRows();
            DrillBitDiameterGrid->InsertRows(DrillBitDiameterGrid->GetNumberRows(), delta_rows);
        }
    }
}

void EditDrillBitsProperties::FillDrillBitParamsRow(int drill_bit_table_row, const TBLFileServer::DrillBitParams& drill_bit_params)
{
    DrillBitDiameterGrid->SetCellValue(drill_bit_table_row, 0, wxFI(drill_bit_params.drill_bit));
    DrillBitDiameterGrid->SetCellValue(drill_bit_table_row, 1, wxFDN(drill_bit_params.diameter, 3));
}

void EditDrillBitsProperties::ReloadDataFromServer()
{
    if (!m_use_tbl_server)
    {
        ExplainDrillBitText->ChangeValue({});
        SetGridsRows(1, 1);
        PinToDrilBitlNumberGrid->ClearGrid();
        DrillBitDiameterGrid->ClearGrid();
        PinCountSpin->SetValue(1);
        return;
    }

    ExplainDrillBitText->ChangeValue(wxFS8(m_use_tbl_server->GetExplainDataTextAsString("\n")));

    std::pair<TBLFileServer::PinDrillBitV, TBLFileServer::DrillBitParamsV> drill_data_pair = m_use_tbl_server->GetAsVectors();
    int pin_drill_bit_size = static_cast<int>(drill_data_pair.first.size()),
        drill_bit_params_size = static_cast<int>(drill_data_pair.second.size());
    SetGridsRows(pin_drill_bit_size, drill_bit_params_size);
    for (const TBLFileServer::PinDrillBit& pin_drill_bit : drill_data_pair.first)
    {
        if (pin_drill_bit.pin >= 0 && pin_drill_bit.pin < pin_drill_bit_size)
            PinToDrilBitlNumberGrid->SetCellValue(pin_drill_bit.pin, 0, wxFI(pin_drill_bit.drill_bit));
    }

    int drill_bit_table_row = 0;
    for (const TBLFileServer::DrillBitParams& drill_bit_params : drill_data_pair.second)
        FillDrillBitParamsRow(drill_bit_table_row++, drill_bit_params);

    PinCountSpin->SetValue(pin_drill_bit_size);
}

void EditDrillBitsProperties::UpdatePinDrillBitRecords(const vector<int>& corrected_pins)
{
    if (!m_use_tbl_server)
        return;

    for (int pin_number : corrected_pins)
    {
        if (const TBLFileServer::PinDrillBit* pin_drill_bit = m_use_tbl_server->GetPinDrillBit(pin_number))
            PinToDrilBitlNumberGrid->SetCellValue(pin_drill_bit->pin, 0, wxFI(pin_drill_bit->drill_bit));
    }
}

// Сохранение накопленных данных в виде TBL-файла.
void EditDrillBitsProperties::OnDrillBitPropSaveClick(wxCommandEvent& event)
{
    wxFileDialog save_file_dialog(this, _("Сохранить данные об инструментах сверления в TBL-файл"),
                                  wxEmptyString, wxEmptyString, _("Таблицы инструментов (*.tbl)|*.tbl"),
                                  wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    if (m_use_tbl_server)
    {
        FileWorkshop::ErrorInfo save_error = m_use_tbl_server->SaveTBLData(string(save_file_dialog.GetPath().mb_str()));
        if (save_error.first != PCADLoadError::LOAD_FILE_NO_ERROR)
            wxMessageBox(_("Ошибка при создании TBL-файла - ") +
                wxFS8(FileWorkshop::FormErrMess(save_error.first, save_error.second)), _("Ошибка при записи"));
    }
}

void EditDrillBitsProperties::OnDrillBitPropLoadClick(wxCommandEvent& event)
{
    wxFileDialog load_file_dialog(this, _("Открыть TBL-файл таблицы инструментов сверления"), {}, {},
                                  _("Таблицы инструментов (*.tbl)|*.tbl|все файлы|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (load_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    if (m_use_tbl_server)
    {
        FileWorkshop::ErrorInfo save_error = m_use_tbl_server->LoadTBLData(string(load_file_dialog.GetPath().mb_str()));
        if (save_error.first != PCADLoadError::LOAD_FILE_NO_ERROR)
            wxMessageBox(_("Ошибка при загрузке TBL-файла - ") +
                wxFS8(FileWorkshop::FormErrMess(save_error.first, save_error.second)), _("Ошибка чтения"));
        else // Загрузка завершена без ошибок. Перезагрузим содержимое базы m_use_tbl_server в редактирующие виджеты.
            ReloadDataFromServer();
    }
}

void EditDrillBitsProperties::OnDrillBitPropExitClick(wxCommandEvent& event)
{
    if (m_use_tbl_server && m_use_tbl_server->IsDataChanged())
    {
        if (wxMessageBox(_("Таблицы инструментов изменялись и не сохранены. Все равно выйти?"), _("Подтверждение"),
                         wxYES_NO | wxCANCEL) == wxYES)
            Close();
    }
    else
    {
        Close();
    }
}

void EditDrillBitsProperties::OnPinCountSpinChange(wxSpinEvent& event)
{
    int old_row_count = PinToDrilBitlNumberGrid->GetNumberRows();
    int new_row_count = PinCountSpin->GetValue();

    if (new_row_count > old_row_count)
    { // Добавляем новые типы выводов в конец таблицы.
        // Сначала добавляем нужное количество строк к таблице редактирования.
        PinToDrilBitlNumberGrid->InsertRows(old_row_count, new_row_count - old_row_count);
        // А затем заполняем эти строки "холостым" (нулевым) номером сверла и добавляем требуемые записи в хранилище базы данных.
        for (int current_pin_number = old_row_count; current_pin_number < new_row_count; ++current_pin_number)
        {
            TBLFileServer::PinDrillBit new_pin_drill_bit;
            new_pin_drill_bit.pin = current_pin_number;
            new_pin_drill_bit.drill_bit = 0;
            //
            PinToDrilBitlNumberGrid->SetCellValue(current_pin_number, 0, wxFI(new_pin_drill_bit.drill_bit));
            if (m_use_tbl_server)
                m_use_tbl_server->SetPinDrillBit(move(new_pin_drill_bit));
        }
    }
    else if (new_row_count < old_row_count)
    { // Удаляем существующие типы выводов с максимальными номерами (снизу сопоставительной таблицы).
        // Сначала удаляем лишние нижние строки редактирующей таблицы.
        PinToDrilBitlNumberGrid->DeleteRows(new_row_count,  old_row_count - new_row_count);
        // А затем вычистим соответствующие лишние записи из базы данных конфигурации сверловки.
        if (m_use_tbl_server)
        {
            for (int current_pin_number = old_row_count - 1; current_pin_number >= new_row_count; --current_pin_number)
                m_use_tbl_server->RemovePinDrillBit(current_pin_number);
        }
    }
}

void EditDrillBitsProperties::OnPinToDrilBitlNumberGridCellSelect(wxGridEvent& event)
{}

void EditDrillBitsProperties::OnPinToDrilBitlNumberGridCellChanging(wxGridEvent& event)
{
    // Проверим новое устанавливаемое пользователем значение на корректность. При выявлении его некорректности выводим
    // диагностику и запретим зменять значение ячейки.
    int pin_number = event.GetRow();
    wxString new_cell_text = event.GetString();

    unsigned long new_drill_bit = 0;
    if (!new_cell_text.ToULong(&new_drill_bit))
    { // Номер привязаннорго сверла должен быть целым положительным числом.
        wxMessageBox(wxTR(DRILL_BIT_MUST_NUMBER), wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
        event.Veto();
        return;
    }

    if (new_drill_bit < 0 || new_drill_bit > TBLFileServer::DRILL_BIT_MAX_NUMBER)
    { // Номер сверла должен укладываться в разрешенный диапазон.
        // Исходя из ограничений PC-DRILL, номер сверла не может превышать DRILL_BIT_MAX_NUMBER.
        wxMessageBox(wxTR(DRILL_BIT_NUMBER_LESSER) + wxString::FromDouble(TBLFileServer::DRILL_BIT_MAX_NUMBER, 0),
                     wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
        event.Veto();
        return;
    }

    if (new_drill_bit && m_use_tbl_server && !m_use_tbl_server->GetDrillBitParams(new_drill_bit))
    { // Сверло с указанным номером отсутствует в базе данных.
        wxMessageBox(wxTR(DRILL_BIT_ABSENT_IN_BASE), wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
        event.Veto();
        return;
    }
}

void EditDrillBitsProperties::OnPinToDrilBitlNumberGridCellChanged(wxGridEvent& event)
{
    int pin_number = event.GetRow();
    unsigned long new_drill_bit = 0;
    PinToDrilBitlNumberGrid->GetCellValue(pin_number, 0).ToULong(&new_drill_bit);

    if (m_use_tbl_server)
    {
        TBLFileServer::PinDrillBit new_pin_drill_bit;
        new_pin_drill_bit.pin = pin_number;
        new_pin_drill_bit.drill_bit = new_drill_bit;
        m_use_tbl_server->SetPinDrillBit(move(new_pin_drill_bit));
    }
}

void EditDrillBitsProperties::OnDrillBitDiameterGridCellSelect(wxGridEvent& event)
{}

void EditDrillBitsProperties::OnDrillBitDiameterGridCellChanging(wxGridEvent& event)
{
    int row_number = event.GetRow(), col_number = event.GetCol();
    wxString new_cell_text = event.GetString();

    if (col_number == 0)
    { // Пользователь изменил условный номер сверла.
        unsigned long new_drill_bit = 0;
        if (!new_cell_text.ToULong(&new_drill_bit))
        { // Номер привязаннорго сверла должен быть целым положительным числом.
            wxMessageBox(wxTR(DRILL_BIT_MUST_NUMBER), wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
            event.Veto();
            return;
        }

        if (new_drill_bit < 1 || new_drill_bit > TBLFileServer::DRILL_BIT_MAX_NUMBER)
        { // Номер сверла должен укладываться в разрешенный диапазон [1; DRILL_BIT_MAX_NUMBER].
          // Условный номер 0 зарезервирован для отсутствующего сверла.
            // Исходя из ограничений PC-DRILL, номер сверла не может превышать DRILL_BIT_MAX_NUMBER.
            wxMessageBox(wxTR(DRILL_BIT_NUMBER_LESSER) + wxString::FromDouble(TBLFileServer::DRILL_BIT_MAX_NUMBER, 0),
                         wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
            event.Veto();
            return;
        }
        // Наличие дубликатов (сверл с одним и тем же номером) в инструментальной таблице не допускается.
        if (m_use_tbl_server && m_use_tbl_server->GetDrillBitParams(new_drill_bit))
        { // В таблице параметров сверл такое сверло уже описано.
            wxMessageBox(wxTR(DRILL_BIT_DUPLICATES), wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
            event.Veto();
            return;
        }
    }
    else if (col_number == 1)
    {  // Пользователь изменил диаметр сверла.
        // Проверим корректность заданного пользователм диаметра сверла.
       // Проверим корректность заданного пользователм диаметра сверла.
        double new_drill_bit_diameter = 0.0;
        if (!new_cell_text.ToDouble(&new_drill_bit_diameter))
        { //
            wxMessageBox(wxTR(DRILL_BIT_DIAMETER_MUST_NUMBER), wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
            event.Veto();
            return;
        }

        if (new_drill_bit_diameter < 0.0 || new_drill_bit_diameter > TBLFileServer::DRILL_BIT_MAX_DIAMETER)
        {
            wxMessageBox(wxTR(DRILL_BIT_DIAMETER_LESSER) + wxString::FromDouble(TBLFileServer::DRILL_BIT_MAX_DIAMETER, 3),
                         wxTR(ERROR_FIELD_VALUE), wxCENTER | wxICON_ERROR);
            event.Veto();
            return;
        }
    }
}

// Метод обработки изменения пользователем параметров инструментов сверловки.
void EditDrillBitsProperties::OnDrillBitDiameterGridCellChanged(wxGridEvent& event)
{
    int row_number = event.GetRow(), col_number = event.GetCol();
    unsigned long new_drill_bit = 0;
    DrillBitDiameterGrid->GetCellValue(row_number, 0).ToULong(&new_drill_bit);
    double new_drill_bit_diameter = 0.0;
    DrillBitDiameterGrid->GetCellValue(row_number, 1).ToDouble(&new_drill_bit_diameter);

    if (col_number == 0)
    { // Пользователь изменил условный номер сверла.
        unsigned long old_drill_bit = 0;
        bool old_drill_bit_ok =  event.GetString().ToULong(&old_drill_bit);

        if (m_use_tbl_server && old_drill_bit != new_drill_bit)
        {
            if (old_drill_bit_ok && old_drill_bit > 0)
            { // Удалим из базы данных запись об инструменте со "старым" номером old_drill_bit.
                auto [err_info, corrected_pins] = m_use_tbl_server->RemoveDrillBitParams(old_drill_bit, true, new_drill_bit);
                // Обновим записи в таблице назначений для выводов, тип которых указан в массиве corrected_pins.
                UpdatePinDrillBitRecords(corrected_pins);
            }
            // Переставим строку с описанием сверла new_drill_bit на надлежащее место в таблице DrillBitDiameterGrid,
            // так как данная таблица полагается упорядоченной по возрастанию индексов сверл.
            DrillBitDiameterGrid->DeleteRows(row_number);
            // Найдем положение в инструментальной таблице для сверла new_drill_bit и вставим туда новую строку под нее.
            pair<int, bool> finded_new_table_row = FindParamsTableRowForDrillBit(new_drill_bit, true);
            DrillBitDiameterGrid->InsertRows(finded_new_table_row.first);
            // Заполним эту строку правильной информацией об инструменте.
            TBLFileServer::DrillBitParams new_drill_bit_params;
            new_drill_bit_params.drill_bit = new_drill_bit;
            new_drill_bit_params.diameter = new_drill_bit_diameter;
            FillDrillBitParamsRow(finded_new_table_row.first, new_drill_bit_params);
            // Наконец, создадим в базе данных новую запись о параметрах сверла new_drill_bit.
            m_use_tbl_server->SetDrillBitParams(move(new_drill_bit_params));
        }
    }
    else if (col_number == 1)
    {  // Пользователь изменил диаметр сверла.
        if (m_use_tbl_server)
        {
            TBLFileServer::DrillBitParams new_drill_bit_params;
            new_drill_bit_params.drill_bit = new_drill_bit;
            new_drill_bit_params.diameter = new_drill_bit_diameter;
            m_use_tbl_server->SetDrillBitParams(move(new_drill_bit_params));
        }
    }
}

void EditDrillBitsProperties::OnDrillBitPropSaveINIClick(wxCommandEvent& event)
{
    wxFileDialog save_file_dialog(this, _("Сохранить информацию о настройках сверленияв INI-формате"),
                                  wxEmptyString, wxEmptyString, _("Таблицы инструментов (*.ini)|*.ini"),
                                  wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    if (m_use_tbl_server)
    {
        FileWorkshop::ErrorInfo save_error = m_use_tbl_server->SaveINIData(string(save_file_dialog.GetPath().mb_str()));
        if (save_error.first != PCADLoadError::LOAD_FILE_NO_ERROR)
            wxMessageBox(_("Ошибка при сохранении INI-файла - ") +
                wxString::FromUTF8(FileWorkshop::FormErrMess(save_error.first, save_error.second).c_str()), _("Ошибка при записи"));
    }
}

void EditDrillBitsProperties::OnDrillBitPropLoadINIClick(wxCommandEvent& event)
{
    wxFileDialog load_file_dialog(this, _("Открыть INI-файл таблицы инстументов"), {}, {},
                                  _("Таблицы инструментов (*.ini)|*.ini|все файлы|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (load_file_dialog.ShowModal() == wxID_CANCEL)
        return;

   if (m_use_tbl_server)
    {
        FileWorkshop::ErrorInfo save_error = m_use_tbl_server->LoadINIData(string(load_file_dialog.GetPath().mb_str()));
        if (save_error.first != PCADLoadError::LOAD_FILE_NO_ERROR)
            wxMessageBox(_("Ошибка при загрузке INI-файла - ") +
                wxString::FromUTF8(FileWorkshop::FormErrMess(save_error.first, save_error.second).c_str()), _("Ошибка чтения"));
        else // Загрузка завершена без ошибок. Перезагрузим содержимое базы m_use_tbl_server в редактирующие виджеты.
            ReloadDataFromServer();
    }
}

void EditDrillBitsProperties::OnExplainDrillBitTextText(wxCommandEvent& event)
{
    m_is_explain_text_changed = true;
}

void EditDrillBitsProperties::OnExplainDrillBitTextTextEnter(wxCommandEvent& event)
{
    if (m_use_tbl_server)
        m_use_tbl_server->SetExplainDataText(string(ExplainDrillBitText->GetValue().mb_str(wxConvUTF8)));
    m_is_explain_text_changed = false;
}

void EditDrillBitsProperties::OnExplainDrillBitTextKillFocus(wxFocusEvent& event)
{
    if (m_is_explain_text_changed)
    {
        if (m_use_tbl_server)
            m_use_tbl_server->SetExplainDataText(string(ExplainDrillBitText->GetValue().mb_str(wxConvUTF8)));
        m_is_explain_text_changed = false;
    }
    event.Skip();
}

// Метод поиска строки в инструментальной таблице DrillBitDiameterGrid строку, описывающую сверло find_drill_bit,
// или строку, следующую сразу после неё (при is_strict_greater ==  true).
pair<int, bool> EditDrillBitsProperties::FindParamsTableRowForDrillBit(int find_drill_bit, bool is_strict_greater)
{
    int scan_row = 0;
    bool strict_found = false;
    for (; scan_row < DrillBitDiameterGrid->GetNumberRows(); ++scan_row)
    {
        unsigned long test_scan_drill_bit;
        if (DrillBitDiameterGrid->GetCellValue(scan_row, 0).ToULong(&test_scan_drill_bit))
        {
            if (is_strict_greater)
            { // Поиск строки для сверла с индексом строго больше, чем find_drill_bit.
                if (static_cast<int>(test_scan_drill_bit) > find_drill_bit)
                    break;  // Найден номер строки таблицы, содержей индекс сверла, следующий сразу за find_drill_bit.
            }
            else
            { // Ищем сверло с номером, больше или равным find_drill_bit.
                if (static_cast<int>(test_scan_drill_bit) >= find_drill_bit)
                {
                    strict_found = static_cast<int>(test_scan_drill_bit) == find_drill_bit;
                    break;  // Найден номер строки таблицы, содержащей описание искомого сверла.
                }
            }
        }
    }
    return {scan_row, strict_found};
}

// Нажата кнопка добавления нового сверла в инструментальную таблицу.
void EditDrillBitsProperties::OnAddNewDrillBitClick(wxCommandEvent& event)
{
    // Выберем первый свободный номер сверла, ещё не описанный в базе данных.
    int use_drill_bit = m_use_tbl_server->GetUnusedPinDrillBitNumber();
    pair<int, bool> finded_table_row = FindParamsTableRowForDrillBit(use_drill_bit, true);
    // Вставляем новую строку в найденное выше положение finded_table_row.first.
    DrillBitDiameterGrid->InsertRows(finded_table_row.first);
    TBLFileServer::DrillBitParams drill_bit_params;
    drill_bit_params.drill_bit = use_drill_bit;
    // Сохраним содержимое записи по умолчанию для вновь созданного сверла как в строку таблицы finded_table_row.first,
    // так затем и в базу данных.
    FillDrillBitParamsRow(finded_table_row.first, drill_bit_params);
    m_use_tbl_server->SetDrillBitParams(move(drill_bit_params));
    //
    DrillBitDiameterGrid->SetGridCursor(finded_table_row.first, 0);
}

// Нажатие кнопки удаления текущего сверла из инструментальной таблицы.
void EditDrillBitsProperties::OnDeleteDrillBitClick(wxCommandEvent& event)
{
    if (int deleted_drill_bit_pos = DrillBitDiameterGrid->GetGridCursorRow(); deleted_drill_bit_pos >= 0)
    {
        unsigned long deleted_drill_bit;
        if (DrillBitDiameterGrid->GetCellValue(deleted_drill_bit_pos, 0).ToULong(&deleted_drill_bit))
        {
            // Удалим из базы данных запись об инструменте с номером deleted_drill_bit.
            auto [err_info, corrected_pins] = m_use_tbl_server->RemoveDrillBitParams(deleted_drill_bit);
            // Обновим записи в таблице назначений для выводов, тип которых указан в массиве corrected_pins.
            UpdatePinDrillBitRecords(corrected_pins);
        }
        DrillBitDiameterGrid->DeleteRows(deleted_drill_bit_pos);
    }
}

void EditDrillBitsProperties::OnDrillBitDiameterGridLabelLeftClick(wxGridEvent& event)
{}
