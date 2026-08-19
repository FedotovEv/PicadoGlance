#ifndef DRILLDATABASEPROPERTIES_H
#define DRILLDATABASEPROPERTIES_H

//(*Headers(DrillDatabaseProperties)
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
//*)

#include "TBLFileServer.h"
#include "redefine_.h"

class DrillDatabaseProperties: public wxDialog
{
    public:

        DrillDatabaseProperties(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
        virtual ~DrillDatabaseProperties();
        void SetTBLDataServer(TBLFileServer* p_use_tbl_server);

        //(*Declarations(DrillDatabaseProperties)
        wxCheckBox* DrillFileSortingCheck;
        wxCheckBox* DrillTBLDatabaseStrictCheck;
        wxComboBox* DrillDatabaseEncodingCombo;
        //*)

    protected:

        //(*Identifiers(DrillDatabaseProperties)
        static const wxWindowID ID_COMBO_DATABASE_ENCODING;
        static const wxWindowID ID_CHECKBOX_TBL_FORMAT_STRICT;
        static const wxWindowID ID_CHECKBOX_SORTING_DRILL_FILE;
        //*)

    private:

        //(*Handlers(DrillDatabaseProperties)
        void OnDrillTBLDatabaseStrictCheckClick(wxCommandEvent& event);
        void OnDrillFileSortingCheckClick(wxCommandEvent& event);
        void OnDrillDatabaseEncodingComboSelected(wxCommandEvent& event);
        //*)

        DECLARE_EVENT_TABLE()

        // Локальные поля объекта диалога.
        TBLFileServer* m_use_tbl_server = nullptr;
        TBLFileServer::DrillDatabasePropertiesSaver m_property_saver;
};

#endif
