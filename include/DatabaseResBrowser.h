#ifndef DATABASERESBROWSER_H
#define DATABASERESBROWSER_H

#ifndef WX_PRECOMP
    //(*HeadersPCH(DatabaseResBrowser)
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/choice.h>
    #include <wx/dialog.h>
    #include <wx/sizer.h>
    #include <wx/statbox.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    //*)
#endif
//(*Headers(DatabaseResBrowser)
#include <wx/treectrl.h>
//*)

class PCADFile;
class DatabaseResBrowser: public wxDialog
{
    public:
        enum class DatabaseErrorCode
        {
            DATABASE_ERROR_NONE = 0     // Загрузка данных из входного документа выполнена успешно.
        };

        struct LoadDatabaseError
        {
            DatabaseErrorCode code = DatabaseErrorCode::DATABASE_ERROR_NONE;
            wxString position;
        };

        DatabaseResBrowser(wxWindow* parent,wxWindowID id=wxID_ANY);
        virtual ~DatabaseResBrowser();
        // Получение текстового описания для ошибки с кодом code.
        static wxString DatabaseErrorToString(DatabaseErrorCode code);
        // Метод загрузки данных в форму диалога из сформированного документа типа PCADFile.
        std::vector<LoadDatabaseError> LoadPCADFileData(const PCADFile* pcad_database);
        // Возврат текстового описания ошибки, если диалог завершился ненормально (с кодом, отличным от wxID_OK).
        wxString GetErrorMessage() const;

        //(*Declarations(DatabaseResBrowser)
        wxButton* ExitDialogButton;
        wxButton* ExportAsTextButton;
        wxButton* ExportAsXMLButton;
        wxCheckBox* IsComponentJumper;
        wxCheckBox* IsComponentPlanar;
        wxCheckBox* IsInsertMirroring;
        wxCheckBox* IsInsertOnTop;
        wxCheckBox* IsInsertUserNameCheck;
        wxCheckBox* IsNetUserNameCheck;
        wxChoice* ComponentPinLabelAlignChoice;
        wxChoice* ComponentPinLabelOrientChoice;
        wxChoice* ComponentPinsListChoice;
        wxChoice* ComponentRefDesAlignChoice;
        wxChoice* ComponentRefDesOrientChoice;
        wxChoice* ComponentsListChoice;
        wxChoice* InsertPinLabelAlignChoice;
        wxChoice* InsertPinLabelOrientChoice;
        wxChoice* InsertPinNamesListChoice;
        wxChoice* InsertRefDesAlignChoice;
        wxChoice* InsertRefDesOrientChoice;
        wxChoice* InsertionsListChoice;
        wxChoice* NetObjectsListChoice;
        wxChoice* NetsListChoice;
        wxStaticBoxSizer* ComponentPinsInfoSizer;
        wxStaticBoxSizer* ComponentSectionsInfoSizer;
        wxStaticBoxSizer* InsertPinsInfoSizer;
        wxStaticBoxSizer* NetObjectsInfoSizer;
        wxStaticText* ComponentPinCoordsTitle;
        wxStaticText* ComponentPinEquivTitle;
        wxStaticText* ComponentPinLabelAlignTitle;
        wxStaticText* ComponentPinLabelCoordsTitle;
        wxStaticText* ComponentPinLabelHeightTitle;
        wxStaticText* ComponentPinLabelLayerTitle;
        wxStaticText* ComponentPinLabelOrientTitle;
        wxStaticText* ComponentPinTypeTitle;
        wxStaticText* InsertPinNetNameConnectTitle;
        wxStaticText* InsertPinTypeTitle;
        wxTextCtrl* ComponentNameText;
        wxTextCtrl* ComponentOrgCoordsText;
        wxTextCtrl* ComponentPackageIDText;
        wxTextCtrl* ComponentPinAlNumText;
        wxTextCtrl* ComponentPinCoordsText;
        wxTextCtrl* ComponentPinEquivText;
        wxTextCtrl* ComponentPinLabelCoordsText;
        wxTextCtrl* ComponentPinLabelHeightText;
        wxTextCtrl* ComponentPinLabelLayerText;
        wxTextCtrl* ComponentPinLayerText;
        wxTextCtrl* ComponentPinNameText;
        wxTextCtrl* ComponentPinTypeText;
        wxTextCtrl* ComponentRefDesCoordsText;
        wxTextCtrl* ComponentRefDesHeightText;
        wxTextCtrl* ComponentRefDesLayerText;
        wxTextCtrl* ComponentTyIDText;
        wxTextCtrl* InsertComponentNameText;
        wxTextCtrl* InsertCoordsText;
        wxTextCtrl* InsertNameCoordsText;
        wxTextCtrl* InsertNameText;
        wxTextCtrl* InsertPinAlNumText;
        wxTextCtrl* InsertPinLabelCoordsText;
        wxTextCtrl* InsertPinLabelHeightText;
        wxTextCtrl* InsertPinLabelLayerText;
        wxTextCtrl* InsertPinNameText;
        wxTextCtrl* InsertPinNetNameConnectText;
        wxTextCtrl* InsertPinTypeText;
        wxTextCtrl* InsertRefDesCoordsText;
        wxTextCtrl* InsertRefDesHeightText;
        wxTextCtrl* InsertRefDesLayerText;
        wxTextCtrl* InsertRotateFactorText;
        wxTextCtrl* InsertScaleXText;
        wxTextCtrl* InsertScaleYText;
        wxTextCtrl* InsertSetAngleText;
        wxTextCtrl* NetNameText;
        wxTextCtrl* NetObjectDescriptionText;
        wxTreeCtrl* ComponentSectInfoTree;
        //*)

    protected:

        //(*Identifiers(DatabaseResBrowser)
        static const wxWindowID ID_CHOICE_COMPONENTS_LIST;
        static const wxWindowID ID_TEXT_COMPONENT_NAME;
        static const wxWindowID ID_TEXT_COMPONENT_PACKAGE_ID;
        static const wxWindowID ID_TEXT_COMPONENT_TY_ID;
        static const wxWindowID ID_TEXT_COMPONENT_ORG_COORDS;
        static const wxWindowID ID_CHECK_IS_COMPONENT_PLANAR;
        static const wxWindowID ID_CHECK_IS_COMPONENT_JUMPER;
        static const wxWindowID ID_TREE_COMPONENT_SECT_INFO;
        static const wxWindowID ID_TEXT_COMPONENT_REFDES_COORDS;
        static const wxWindowID ID_TEXT_COMPONENT_REFDES_LAYER;
        static const wxWindowID ID_TEXT_COMPONENT_REFDES_HEIGHT;
        static const wxWindowID ID_CHOICE_COMPONENT_REFDES_ORIENT;
        static const wxWindowID ID_CHOICE_COMPONENT_REFDES_ALIGN;
        static const wxWindowID ID_CHOICE_COMPONENT_PINS_LIST;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_ALNUM;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_NAME;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_LAYER;
        static const wxWindowID ID_STATICTEXT1;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_COORDS;
        static const wxWindowID ID_STATICTEXT3;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_TYPE;
        static const wxWindowID ID_STATICTEXT4;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_EQUIV;
        static const wxWindowID ID_STATICTEXT2;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_LABEL_COORDS;
        static const wxWindowID ID_STATICTEXT5;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_LABEL_LAYER;
        static const wxWindowID ID_STATICTEXT6;
        static const wxWindowID ID_TEXT_COMPONENT_PIN_LABEL_HEIGHT;
        static const wxWindowID ID_STATICTEXT7;
        static const wxWindowID ID_CHOICE_COMPONENT_PIN_LABEL_ORIENT;
        static const wxWindowID ID_STATICTEXT8;
        static const wxWindowID ID_CHOICE_COMPONENT_PIN_LABEL_ALIGN;
        static const wxWindowID ID_CHOICE_NETS_LIST;
        static const wxWindowID ID_TEXT_NET_NAME;
        static const wxWindowID ID_CHECK_IS_NET_USER_NAME;
        static const wxWindowID ID_CHOICE_NET_OBJECTS_LIST;
        static const wxWindowID ID_TEXT_NET_OBJECT_DESCRIPTION;
        static const wxWindowID ID_CHOICE_INSERTIONS_LIST;
        static const wxWindowID ID_TEXT_INSERT_NAME;
        static const wxWindowID ID_TEXT_INSERT_NAME_COORDS;
        static const wxWindowID ID_CHECK_IS_INSERT_USER_NAME;
        static const wxWindowID ID_TEXT_INSERT_COMPONENT_NAME;
        static const wxWindowID ID_TEXT_INSERT_COORDS;
        static const wxWindowID ID_CHECK_IS_INSERT_MIRRORING;
        static const wxWindowID ID_CHECK_IS_INSERT_ON_TOP;
        static const wxWindowID ID_TEXT_INSERT_SCALE_X;
        static const wxWindowID ID_TEXT_INSERT_SCALE_Y;
        static const wxWindowID ID_TEXT_INSERT_ROTATE_FACTOR;
        static const wxWindowID ID_TEXT_INSERT_SET_ANGLE;
        static const wxWindowID ID_TEXT_INSERT_REFDES_COORDS;
        static const wxWindowID ID_TEXT_INSERT_REFDES_LAYER;
        static const wxWindowID ID_TEXT_INSERT_REFDES_HEIGHT;
        static const wxWindowID ID_CHOICE_INSERT_REFDES_ORIENT;
        static const wxWindowID ID_CHOICE_INSERT_REFDES_ALIGN;
        static const wxWindowID ID_CHOICE_INSERT_PIN_NAMES_LIST;
        static const wxWindowID ID_TEXT_INSERT_PIN_ALNUM;
        static const wxWindowID ID_TEXT_INSERT_PIN_NAME;
        static const wxWindowID ID_STATICTEXT9;
        static const wxWindowID ID_TEXT_INSERT_PIN_TYPE;
        static const wxWindowID ID_STATICTEXT10;
        static const wxWindowID ID_TEXT_INSERT_PIN_NET_NAME_CONNECT;
        static const wxWindowID ID_TEXT_INSERT_PIN_LABEL_COORDS;
        static const wxWindowID ID_TEXT_INSERT_PIN_LABEL_LAYER;
        static const wxWindowID ID_TEXT_INSERT_PIN_LABEL_HEIGHT;
        static const wxWindowID ID_CHOICE_INSERT_PIN_LABEL_ORIENT;
        static const wxWindowID ID_CHOICE_INSERT_PIN_LABEL_ALIGN;
        static const wxWindowID ID_BUTTON_EXPORT_AS_TEXT;
        static const wxWindowID ID_BUTTON_EXPORT_AS_XML;
        static const wxWindowID ID_BUTTON_EXIT_DIALOG;
        //*)

    private:

        //(*Handlers(DatabaseResBrowser)
        void OnComponentsListChoiceSelect(wxCommandEvent& event);
        void OnComponentPinsListChoiceSelect(wxCommandEvent& event);
        void OnNetsListChoiceSelect(wxCommandEvent& event);
        void OnNetObjectsListChoiceSelect(wxCommandEvent& event);
        void OnInsertionsListChoiceSelect(wxCommandEvent& event);
        void OnInsertPinNamesListChoiceSelect(wxCommandEvent& event);
        void OnExportAsTextButtonClick(wxCommandEvent& event);
        void OnExportAsXMLButtonClick(wxCommandEvent& event);
        void OnExitDialogButtonClick(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnInit(wxInitDialogEvent& event);
        //*)

    protected:

        void BuildContent(wxWindow* parent,wxWindowID id);
        void CloseDialogProc();
        const PCADFile* view_pcad_database_ = nullptr;
        wxString error_message_;

        DECLARE_EVENT_TABLE()
};

#endif
