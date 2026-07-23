#ifndef EDITDRILLBITSPROPERTIES_H
#define EDITDRILLBITSPROPERTIES_H

// Заголовок диалога редактора настроек инструментов сверления и их сопоставления с
// типами выводов радиоэлементов и отверстий в печатной плате.

#ifndef WX_PRECOMP
    //(*HeadersPCH(EditDrillBitsProperties)
    #include <wx/button.h>
    #include <wx/dialog.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    //*)
#endif
//(*Headers(EditDrillBitsProperties)
#include <wx/grid.h>
#include <wx/spinctrl.h>
//*)

#include "TBLFileServer.h"

class EditDrillBitsProperties: public wxDialog
{
    public:

        EditDrillBitsProperties(wxWindow* parent, wxWindowID id = wxID_ANY);
        virtual ~EditDrillBitsProperties();
        void SetTBLDataServer(TBLFileServer* p_use_tbl_server);

        //(*Declarations(EditDrillBitsProperties)
        wxButton* AddNewDrillBit;
        wxButton* DeleteDrillBit;
        wxButton* DrillBitPropExit;
        wxButton* DrillBitPropLoadINI;
        wxButton* DrillBitPropLoadTBL;
        wxButton* DrillBitPropSaveINI;
        wxButton* DrillBitPropSaveTBL;
        wxGrid* DrillBitDiameterGrid;
        wxGrid* PinToDrilBitlNumberGrid;
        wxSpinCtrl* PinCountSpin;
        wxStaticText* DrillBitPropHeaderText;
        wxStaticText* ExplainDrillBitStatic;
        wxStaticText* PinDrillBitHeaderText;
        wxStaticText* StaticText1;
        wxTextCtrl* ExplainDrillBitText;
        //*)

    protected:

        //(*Identifiers(EditDrillBitsProperties)
        static const wxWindowID ID_STATIC_EXPLAIN_DATA_TABLE;
        static const wxWindowID ID_TEXT_EXPLAIN_TABLE_TEXT;
        static const wxWindowID ID_STATICTEXT1;
        static const wxWindowID ID_STATICTEXT2;
        static const wxWindowID ID_GRID_PIN_TO_DRILL_BIT;
        static const wxWindowID ID_GRID_DRILL_BIT_DIAMETER;
        static const wxWindowID ID_STATICTEXT3;
        static const wxWindowID ID_SPIN_PIN_COUNT;
        static const wxWindowID ID_BUTTON_ADD_NEW_DRILL_BIT;
        static const wxWindowID ID_BUTTON_DELETE_DRILL_BIT;
        static const wxWindowID ID_BUTTON_DRILL_BIT_PROP_SAVE_TBL;
        static const wxWindowID ID_BUTTON_DRILL_BIT_PROP_LOAD_TBL;
        static const wxWindowID ID_BUTTON_DRILL_BIT_PROP_SAVE_INI;
        static const wxWindowID ID_BUTTON_DRILL_BIT_PROP_LOAD_INI;
        static const wxWindowID ID_BUTTON_DRILL_BIT_PROP_EXIT;
        //*)

    private:

        //(*Handlers(EditDrillBitsProperties)
        void OnDrillBitPropSaveClick(wxCommandEvent& event);
        void OnDrillBitPropLoadClick(wxCommandEvent& event);
        void OnDrillBitPropExitClick(wxCommandEvent& event);
        void OnPinCountSpinChange(wxSpinEvent& event);
        void OnPinToDrilBitlNumberGridCellSelect(wxGridEvent& event);
        void OnPinToDrilBitlNumberGridCellChanging(wxGridEvent& event);
        void OnPinToDrilBitlNumberGridCellChanged(wxGridEvent& event);
        void OnDrillBitDiameterGridCellSelect(wxGridEvent& event);
        void OnDrillBitDiameterGridCellChanged(wxGridEvent& event);
        void OnDrillBitDiameterGridCellChanging(wxGridEvent& event);
        void OnAddNewDrillBitClick(wxCommandEvent& event);
        void OnDrillBitPropSaveINIClick(wxCommandEvent& event);
        void OnDrillBitPropLoadINIClick(wxCommandEvent& event);
        void OnExplainDrillBitTextTextEnter(wxCommandEvent& event);
        void OnDeleteDrillBitClick(wxCommandEvent& event);
        void OnDrillBitDiameterGridLabelLeftClick(wxGridEvent& event);
        void OnExplainDrillBitTextText(wxCommandEvent& event);
        //*)

        DECLARE_EVENT_TABLE()

        // Дополнительные обработчики событий, не управляемые wxSmith.
        void OnExplainDrillBitTextKillFocus(wxFocusEvent& event);
        // Прочие локальные поля класса редактора инструментов сверления.
        TBLFileServer* m_use_tbl_server = nullptr;
        bool m_is_explain_text_changed = false;
        //
        void ReloadDataFromServer();
        void UpdatePinDrillBitRecords(const std::vector<int>& corrected_pins);
        void SetGridsRows(int pin_dril_bit_rows = -1, int drill_bit_params_rows = -1);
        void FillDrillBitParamsRow(int drill_bit_table_row, const TBLFileServer::DrillBitParams& drill_bit_params);
        // Поиск в инструментальной таблице диалога сверла с номером find_drill_bit.
        std::pair<int, bool> FindParamsTableRowForDrillBit(int find_drill_bit, bool is_strict_greater);
};

#endif
