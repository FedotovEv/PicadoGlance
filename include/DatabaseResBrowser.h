#ifndef DATABASERESBROWSER_H
#define DATABASERESBROWSER_H

#ifndef WX_PRECOMP
    //(*HeadersPCH(DatabaseResBrowser)
    #include <wx/dialog.h>
    #include <wx/listbox.h>
    #include <wx/sizer.h>
    #include <wx/statbox.h>
    //*)
#endif
//(*Headers(DatabaseResBrowser)
//*)

class DatabaseResBrowser: public wxDialog
{
    public:

        DatabaseResBrowser(wxWindow* parent,wxWindowID id=wxID_ANY);
        virtual ~DatabaseResBrowser();

        //(*Declarations(DatabaseResBrowser)
        wxListBox* ListBox1;
        //*)

    protected:

        //(*Identifiers(DatabaseResBrowser)
        static const wxWindowID ID_LISTBOX1;
        //*)

    private:

        //(*Handlers(DatabaseResBrowser)
        //*)

    protected:

        void BuildContent(wxWindow* parent,wxWindowID id);

        DECLARE_EVENT_TABLE()
};

#endif
