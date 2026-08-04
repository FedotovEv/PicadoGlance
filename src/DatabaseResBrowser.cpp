#include "include/wx_pch.h"
#include "DatabaseResBrowser.h"

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(DatabaseResBrowser)
    #include <wx/string.h>
    #include <wx/intl.h>
    //*)
#endif
//(*InternalHeaders(DatabaseResBrowser)
//*)

//(*IdInit(DatabaseResBrowser)
const wxWindowID DatabaseResBrowser::ID_LISTBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DatabaseResBrowser,wxDialog)
    //(*EventTable(DatabaseResBrowser)
    //*)
END_EVENT_TABLE()

DatabaseResBrowser::DatabaseResBrowser(wxWindow* parent,wxWindowID id)
{
    BuildContent(parent,id);
}

void DatabaseResBrowser::BuildContent(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(DatabaseResBrowser)
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
    SetClientSize(wxSize(1195,506));
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Компоненты"));
    ListBox1 = new wxListBox(StaticBoxSizer1->GetStaticBox(), ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
    StaticBoxSizer1->Add(ListBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(StaticBoxSizer1);
    Layout();
    //*)
}

DatabaseResBrowser::~DatabaseResBrowser()
{
    //(*Destroy(DatabaseResBrowser)
    //*)
}

