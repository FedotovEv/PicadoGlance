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

#include "PCADTextExport.h"
#include "redefine_.h"
#include <wx/encconv.h>

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

        DatabaseResBrowser(wxWindow* parent, wxWindowID id = wxID_ANY);
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
        wxCheckBox* InsertIsSelectedComponentOnly;
        wxCheckBox* InsertIsSelectedNetOnly;
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
        wxStaticBoxSizer* ComponentsBoxSizer;
        wxStaticBoxSizer* InsertPinsInfoSizer;
        wxStaticBoxSizer* InsertionsBoxSizer;
        wxStaticBoxSizer* NetObjectsInfoSizer;
        wxStaticBoxSizer* NetsBoxSizer;
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
        static const wxWindowID ID_CHECK_INSERT_IS_SELECT_COMPONENT_ONLY;
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
        static const wxWindowID ID_CHECK_INSERT_IS_SELECT_NET_ONLY;
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

        struct TextParamWidgets
        {
            wxTextCtrl* CoordsText = nullptr;
            wxTextCtrl* LayerText = nullptr;
            wxTextCtrl* HeightText = nullptr;
            wxChoice* AlignChoice = nullptr;
            wxChoice* OrientChoice = nullptr;
        };

        enum class TextParamGroup
        {
            TEXT_GROUP_COMPONENT_REFDES = 1,
            TEXT_GROUP_COMPONENT_PIN_LABEL,
            TEXT_GROUP_INSERT_REFDES,
            TEXT_GROUP_INSERT_PIN_LABEL
        };

        struct TextParamValues
        {
            wxPoint pos;
            int layer_number = -1;
            int text_height = 1;
            TextAlign text_align = TextAlign::TEXT_CENTER_DOWN;
            TextOrientation text_orient = TextOrientation::TEXT_LEFT_RIGHT;
        };

        static const std::unordered_map<TextParamGroup, TextParamWidgets> text_groups_map_;
        static const TextParamValues CLEAR_TEXT_PARAMS_;

    protected:
        static const wxString ZERO_VALUE_STR;
        static constexpr char SECTIONS_TEXT[] = wxTRANSLATE("Секций - ");
        static constexpr char PINS_TEXT[] = wxTRANSLATE("Выводов - ");
        static constexpr char FRAGMENTS_TEXT[] = wxTRANSLATE("Фрагментов - ");
        static constexpr char COMPONENTS_TEXT[] = wxTRANSLATE("Компонентов - ");
        static constexpr char NETS_TEXT[] = wxTRANSLATE("Цепей - ");
        static constexpr char INSERTS_TEXT[] = wxTRANSLATE("Вставок - ");
        static constexpr size_t CONVERT_UNICODE_BUFFER_LEN = 1024;

        void BuildContent(wxWindow* parent, wxWindowID id);
        void CloseDialogProc();
        void ClearDialog();

        // Методы (функции-члены) загрузки соответствующих групп виджетов информацией об определённых объектах
        // базы данных обозреваемого документа.
        void LoadNewNet(int net_index);
        void LoadNewNet(const std::string& net_name);
        void LoadNewComponent(int component_index);
        void LoadNewComponent(const std::string& component_name);
        // ------ Работа с данными описания вставленной копии радиокомпонента. ------
        // Функции-члены преобазования "логического" индекса вставки в абсолютный её индекс и обратно.
        int LogInsertionIndexToAbs(int log_insertion_index);
        int AbsInsertionIndexToLog(int abs_insertion_index);
        // Заполнение списка доступных вставок в соответствии с выбранным пользователем режимом - всех имеющихся или только отдельного компонента.
        void FillInsertionsList();
        void LoadNewInsertion(int abs_insertion_index);
        void LoadNewInsertion(const std::string& insertion_name);
        // ------
        // --- Загрузка информационных подгрупп (подблоков).
        // Перегрузки функций-членов загрузки в группу виджетов параметров некоторой текстовой надписи.
        void LoadTextParams(const TextParamWidgets& param_widgets, const ObjText* text_object);
        void LoadTextParams(const TextParamWidgets& param_widgets, const TextParamValues& param_values);
        // Заполнение дерева ComponentSectInfoTree секционных данных радиокомпонента component_index, если он содержит
        // информацию о секционировании в формате PKG (секционное деление УГО).
        void LoadPKGData(int component_index);
        // Заполнение дерева ComponentSectInfoTree секционных данных радиокомпонента component_index, если он содержит
        // информацию о секционировании в формате SPKG (секционирование конструктива).
        void LoadSPKGData(int component_index);
        // Загрузка данных в группу виджетов (ComponentPinAlNumText, и.т.д.) информации о некотором выводе (ножке) pin_index
        // библиотечного радиокомпонента component_index.
        void LoadCompPinDescription(int component_index, int pin_index);
        // ------ Работа с данными описания вывода (ножки) вставленной копии радиокомпонента. ------
        int LogInsertPinIndexToAbs(int log_insert_pin_index);
        int AbsInsertPinIndexToLog(int abs_insert_pin_index);
        // Заполнение списка выводов текущей вставки в соответствии с выбранным пользователем режимом - всех имеющихся или только
        // присоединённых к выбранной цепи.
        void FillInsertPinsList(int abs_insertion_index);
        // Загрузка данных в группу виджетов (InsertPinAlNumText, и.т.д.) информации о некотором выводе (ножке) insert_pin_index
        // вставленной копии некоторого радиокомпонента, описанной во вставке с порядковым индексом insertion_index.
        void LoadInsertPinDescription(int abs_insertion_index, int abs_insert_pin_index);
        // ------
        // Функция-член загрузки в указанную ветвь информационного дерева данных об упаковке единичной секции УГО.
        void LoadSectPackInfoToTree
            (int component_index, int group_index, const std::string& sect_name, const PinNameToALNumber& one_sect_pack_info,
             wxTreeItemId& root_section_item);
        // Функция-член загрузки в назначенную ветвь информационного дерева данных о некоторой группе секций.
        void LoadSectGroupPackInfoToTree
            (int component_index, int group_index, const SectNameToPackInfo& sect_group_pack_info, wxTreeItemId& root_section_group);
        // ---
        wxString ConvertToUnicode(const std::string& narrow_string) const;
        void SetPinTypeToText(const PinType& pin_type, wxTextCtrl* pin_type_text_field);

        const PCADFile* view_pcad_database_ = nullptr;
        DrawContext view_draw_context_;
        wxString error_message_;
        bool block_signals_ = false;
        wxEncodingConverter pdif_encoding_conv_;

        DECLARE_EVENT_TABLE()
};

#endif
