#include "include/wx_pch.h"
#include "DatabaseResBrowser.h"
#include "PCADViewerApp.h"
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "PCADTextExport.h"

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(DatabaseResBrowser)
    #include <wx/string.h>
    #include <wx/intl.h>
    //*)
#endif
//(*InternalHeaders(DatabaseResBrowser)
//*)

using namespace std;

//(*IdInit(DatabaseResBrowser)
const wxWindowID DatabaseResBrowser::ID_CHOICE_COMPONENTS_LIST = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PACKAGE_ID = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_TY_ID = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_ORG_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHECK_IS_COMPONENT_PLANAR = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHECK_IS_COMPONENT_JUMPER = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TREE_COMPONENT_SECT_INFO = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_REFDES_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_REFDES_LAYER = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_REFDES_HEIGHT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_COMPONENT_REFDES_ORIENT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_COMPONENT_REFDES_ALIGN = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_COMPONENT_PINS_LIST = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_ALNUM = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_LAYER = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT1 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT3 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_TYPE = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT4 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_EQUIV = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT2 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_LABEL_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT5 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_LABEL_LAYER = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT6 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_COMPONENT_PIN_LABEL_HEIGHT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT7 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_COMPONENT_PIN_LABEL_ORIENT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT8 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_COMPONENT_PIN_LABEL_ALIGN = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_NETS_LIST = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_NET_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHECK_IS_NET_USER_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_NET_OBJECTS_LIST = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_NET_OBJECT_DESCRIPTION = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_INSERTIONS_LIST = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_NAME_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHECK_IS_INSERT_USER_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_COMPONENT_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHECK_IS_INSERT_MIRRORING = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHECK_IS_INSERT_ON_TOP = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_SCALE_X = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_SCALE_Y = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_ROTATE_FACTOR = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_SET_ANGLE = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_REFDES_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_REFDES_LAYER = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_REFDES_HEIGHT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_INSERT_REFDES_ORIENT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_INSERT_REFDES_ALIGN = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_INSERT_PIN_NAMES_LIST = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_ALNUM = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_NAME = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT9 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_TYPE = wxNewId();
const wxWindowID DatabaseResBrowser::ID_STATICTEXT10 = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_NET_NAME_CONNECT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_LABEL_COORDS = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_LABEL_LAYER = wxNewId();
const wxWindowID DatabaseResBrowser::ID_TEXT_INSERT_PIN_LABEL_HEIGHT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_INSERT_PIN_LABEL_ORIENT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_CHOICE_INSERT_PIN_LABEL_ALIGN = wxNewId();
const wxWindowID DatabaseResBrowser::ID_BUTTON_EXPORT_AS_TEXT = wxNewId();
const wxWindowID DatabaseResBrowser::ID_BUTTON_EXPORT_AS_XML = wxNewId();
const wxWindowID DatabaseResBrowser::ID_BUTTON_EXIT_DIALOG = wxNewId();
//*)

BEGIN_EVENT_TABLE(DatabaseResBrowser,wxDialog)
    //(*EventTable(DatabaseResBrowser)
    //*)
END_EVENT_TABLE()

const wxString DatabaseResBrowser::ZERO_VALUE_STR = wxF8("0");
const std::unordered_map<DatabaseResBrowser::TextParamGroup, DatabaseResBrowser::TextParamWidgets> DatabaseResBrowser::text_groups_map_
{
    {
        DatabaseResBrowser::TextParamGroup::TEXT_GROUP_COMPONENT_REFDES,
        {.CoordsText = ComponentRefDesCoordsText, .LayerText = ComponentRefDesLayerText, .HeightText = ComponentRefDesHeightText,
         .AlignChoice = ComponentRefDesOrientChoice, .OrientChoice = ComponentRefDesAlignChoice}
    },
    {
        DatabaseResBrowser::TextParamGroup::TEXT_GROUP_COMPONENT_PIN_LABEL,
        {.CoordsText = ComponentPinLabelCoordsText, .LayerText = ComponentPinLabelLayerText, .HeightText = ComponentPinLabelHeightText,
         .AlignChoice = ComponentPinLabelOrientChoice, .OrientChoice = ComponentPinLabelAlignChoice}
    },
    {
        DatabaseResBrowser::TextParamGroup::TEXT_GROUP_INSERT_REFDES,
        {.CoordsText = InsertRefDesCoordsText, .LayerText = InsertRefDesLayerText, .HeightText = InsertRefDesHeightText,
         .AlignChoice = InsertRefDesOrientChoice, .OrientChoice = InsertRefDesAlignChoice}
    },
    {
        DatabaseResBrowser::TextParamGroup::TEXT_GROUP_INSERT_PIN_LABEL,
        {.CoordsText = InsertPinLabelCoordsText, .LayerText = InsertPinLabelLayerText, .HeightText = InsertPinLabelHeightText,
         .AlignChoice = InsertPinLabelOrientChoice, .OrientChoice = InsertPinLabelAlignChoice}
    }
};

const DatabaseResBrowser::TextParamValues DatabaseResBrowser::CLEAR_TEXT_PARAMS_
{
    .text_height = INT_MIN
};

// Класс дополнительной информации, сопровождающий соответствующую ячейку древовидного виджета ComponentSectInfoTree.
// Может характеризовать группу однородных секций (в этом случае section_name и pin_name пусты, а pin_index == -1),
// саму секцию (section_name не пуст и содержит имя секции, а pin_name пуст и pin_index == -1),
// а также отдельный вывод и его упаковочное соответствие (все поля имеют значащее содержимое).
class SectDefTreeItemData : public wxTreeItemData
{
public:
    int component_index = -1;   // Порядковый индекс радиокомпонента, секция которого описывается характеристикой.
    int group_index = -1;       // Индекс группы, с которой связана данная характеристика.
    string section_name;        // Имя секции (обычно буква в диапазоне A-Z), с которой связана данная характеристика.
    // Данные конкретного вывода.
    string pin_name;            // Имя вывода.
    string pin_al_number;       // Его "алфавитно-цифровой номер ножки", на который картируется вывод с именем pin_name данной секции.
    int pin_index = -1;         // Порядковый индекс в базе.
};

class NetObjectDefTreeItemData : public wx
{
public:
    int component_index = -1;
    int group_index = -1;
    string section_name;
};

DatabaseResBrowser::DatabaseResBrowser(wxWindow* parent, wxWindowID id)
{
    BuildContent(parent, id);
}

void DatabaseResBrowser::BuildContent(wxWindow* parent, wxWindowID id)
{
    //(*Initialize(DatabaseResBrowser)
    wxBoxSizer* CommonDatabaseContentSizer;
    wxBoxSizer* ComponentCommonPropertiesSizer;
    wxBoxSizer* ComponentOrgPropertiesSizer;
    wxBoxSizer* ComponentPinAlNumNameLayerSizer;
    wxBoxSizer* ComponentPinLabelCoordsLayerSizer;
    wxBoxSizer* ComponentPinPropertiesSizer;
    wxBoxSizer* ComponentRefDesCoordsLayerSizer;
    wxBoxSizer* FooterButtonsSizer;
    wxBoxSizer* InsertCoordsSizer;
    wxBoxSizer* InsertNamesSizer;
    wxBoxSizer* InsertPinLabelCoordsLayerSizer;
    wxBoxSizer* InsertPinNamesSizer;
    wxBoxSizer* InsertRefDesCoordsLayerSizer;
    wxBoxSizer* InsertRotateScaleSizer;
    wxBoxSizer* MainDialogSizer;
    wxBoxSizer* NetNameSizer;
    wxBoxSizer* PinPropertiesSizer;
    wxStaticBoxSizer* ComponentPinLabelPropertiesSizer;
    wxStaticBoxSizer* ComponentPropertiesSizer;
    wxStaticBoxSizer* ComponentRefDesPropertiesSizer;
    wxStaticBoxSizer* ComponentRefDesTextPropertiesSizer;
    wxStaticBoxSizer* InsertPinLabelPropertiesSizer;
    wxStaticBoxSizer* InsertPinLabelTextParamsSizer;
    wxStaticBoxSizer* InsertPropertiesSizer;
    wxStaticBoxSizer* InsertRefDesSizer;
    wxStaticBoxSizer* InsertRefDesTextParamsSizer;
    wxStaticBoxSizer* StaticBoxSizer2ComponentPinLabelTextParamsSizer;
    wxStaticText* ComponentNameTitle;
    wxStaticText* ComponentOrgCoordsTitle;
    wxStaticText* ComponentPackageIDTitle;
    wxStaticText* ComponentPinAlNumTitle;
    wxStaticText* ComponentPinLayerTitle;
    wxStaticText* ComponentPinNameTitle;
    wxStaticText* ComponentRefDesCoordsTitle;
    wxStaticText* ComponentRefDesLayerTitle;
    wxStaticText* ComponentRefDesTextAlignTitle;
    wxStaticText* ComponentRefDesTextHeightTitle;
    wxStaticText* ComponentRefDesTextOrientTitle;
    wxStaticText* ComponentTyIDTitle;
    wxStaticText* InsertComponentNameTitle;
    wxStaticText* InsertCoordsTitle;
    wxStaticText* InsertNameCoordsTitle;
    wxStaticText* InsertNameTitle;
    wxStaticText* InsertPinAlNumTitle;
    wxStaticText* InsertPinLabelAlignTitle;
    wxStaticText* InsertPinLabelCoordsTitle;
    wxStaticText* InsertPinLabelHeightTitle;
    wxStaticText* InsertPinLabelLayerTitle;
    wxStaticText* InsertPinLabelOrientTitle;
    wxStaticText* InsertPinNameTitle;
    wxStaticText* InsertRefDesAlignTitle;
    wxStaticText* InsertRefDesCoordsTitle;
    wxStaticText* InsertRefDesHeightTitle;
    wxStaticText* InsertRefDesLayerTitle;
    wxStaticText* InsertRefDesOrientTitle;
    wxStaticText* InsertRotateFactorTitle;
    wxStaticText* InsertScaleXTitle;
    wxStaticText* InsertScaleYTitle;
    wxStaticText* InsertSetAngleTitle;
    wxStaticText* NetNameTitle;

    Create(parent, wxID_ANY, _("Объекты базы данных"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(1195,506));
    MainDialogSizer = new wxBoxSizer(wxVERTICAL);
    CommonDatabaseContentSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentsBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Компонентов - 0"));
    ComponentsListChoice = new wxChoice(ComponentsBoxSizer->GetStaticBox(), ID_CHOICE_COMPONENTS_LIST, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_COMPONENTS_LIST"));
    ComponentsListChoice->SetToolTip(_("Список компонентов базы данных"));
    ComponentsBoxSizer->Add(ComponentsListChoice, 0, wxEXPAND, 5);
    ComponentPropertiesSizer = new wxStaticBoxSizer(wxVERTICAL, ComponentsBoxSizer->GetStaticBox(), _("Свойства компонента"));
    ComponentCommonPropertiesSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentNameTitle = new wxStaticText(ComponentPropertiesSizer->GetStaticBox(), wxID_ANY, _("Имя"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentCommonPropertiesSizer->Add(ComponentNameTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentNameText = new wxTextCtrl(ComponentPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_NAME, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_NAME"));
    ComponentCommonPropertiesSizer->Add(ComponentNameText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPackageIDTitle = new wxStaticText(ComponentPropertiesSizer->GetStaticBox(), wxID_ANY, _("Упаковка"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentCommonPropertiesSizer->Add(ComponentPackageIDTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPackageIDText = new wxTextCtrl(ComponentPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_PACKAGE_ID, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PACKAGE_ID"));
    ComponentCommonPropertiesSizer->Add(ComponentPackageIDText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentTyIDTitle = new wxStaticText(ComponentPropertiesSizer->GetStaticBox(), wxID_ANY, _("Код типа"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentCommonPropertiesSizer->Add(ComponentTyIDTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentTyIDText = new wxTextCtrl(ComponentPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_TY_ID, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_TY_ID"));
    ComponentCommonPropertiesSizer->Add(ComponentTyIDText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPropertiesSizer->Add(ComponentCommonPropertiesSizer, 0, wxBOTTOM|wxEXPAND, 5);
    ComponentOrgPropertiesSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentOrgCoordsTitle = new wxStaticText(ComponentPropertiesSizer->GetStaticBox(), wxID_ANY, _("Координаты привязки"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentOrgPropertiesSizer->Add(ComponentOrgCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentOrgCoordsText = new wxTextCtrl(ComponentPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_ORG_COORDS, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_ORG_COORDS"));
    ComponentOrgPropertiesSizer->Add(ComponentOrgCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IsComponentPlanar = new wxCheckBox(ComponentPropertiesSizer->GetStaticBox(), ID_CHECK_IS_COMPONENT_PLANAR, _("Планар"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_IS_COMPONENT_PLANAR"));
    IsComponentPlanar->SetValue(false);
    IsComponentPlanar->Disable();
    ComponentOrgPropertiesSizer->Add(IsComponentPlanar, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IsComponentJumper = new wxCheckBox(ComponentPropertiesSizer->GetStaticBox(), ID_CHECK_IS_COMPONENT_JUMPER, _("Перемычка"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_IS_COMPONENT_JUMPER"));
    IsComponentJumper->SetValue(false);
    IsComponentJumper->Disable();
    ComponentOrgPropertiesSizer->Add(IsComponentJumper, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPropertiesSizer->Add(ComponentOrgPropertiesSizer, 1, wxEXPAND, 5);
    ComponentsBoxSizer->Add(ComponentPropertiesSizer, 1, wxTOP|wxEXPAND, 5);
    ComponentSectionsInfoSizer = new wxStaticBoxSizer(wxHORIZONTAL, ComponentsBoxSizer->GetStaticBox(), _("Секций - 0"));
    ComponentSectInfoTree = new wxTreeCtrl(ComponentSectionsInfoSizer->GetStaticBox(), ID_TREE_COMPONENT_SECT_INFO, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREE_COMPONENT_SECT_INFO"));
    ComponentSectionsInfoSizer->Add(ComponentSectInfoTree, 1, wxEXPAND, 5);
    ComponentsBoxSizer->Add(ComponentSectionsInfoSizer, 2, wxTOP|wxEXPAND, 5);
    ComponentRefDesPropertiesSizer = new wxStaticBoxSizer(wxVERTICAL, ComponentsBoxSizer->GetStaticBox(), _("Конструкторское обозначение"));
    ComponentRefDesCoordsLayerSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentRefDesCoordsTitle = new wxStaticText(ComponentRefDesPropertiesSizer->GetStaticBox(), wxID_ANY, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentRefDesCoordsLayerSizer->Add(ComponentRefDesCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesCoordsText = new wxTextCtrl(ComponentRefDesPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_REFDES_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_REFDES_COORDS"));
    ComponentRefDesCoordsLayerSizer->Add(ComponentRefDesCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesLayerTitle = new wxStaticText(ComponentRefDesPropertiesSizer->GetStaticBox(), wxID_ANY, _("Слой"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentRefDesCoordsLayerSizer->Add(ComponentRefDesLayerTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesLayerText = new wxTextCtrl(ComponentRefDesPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_REFDES_LAYER, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_REFDES_LAYER"));
    ComponentRefDesCoordsLayerSizer->Add(ComponentRefDesLayerText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesPropertiesSizer->Add(ComponentRefDesCoordsLayerSizer, 1, wxEXPAND, 5);
    ComponentRefDesTextPropertiesSizer = new wxStaticBoxSizer(wxHORIZONTAL, ComponentRefDesPropertiesSizer->GetStaticBox(), _("Параметры текста"));
    ComponentRefDesTextHeightTitle = new wxStaticText(ComponentRefDesTextPropertiesSizer->GetStaticBox(), wxID_ANY, _("Высота"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentRefDesTextPropertiesSizer->Add(ComponentRefDesTextHeightTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesHeightText = new wxTextCtrl(ComponentRefDesTextPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_REFDES_HEIGHT, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_REFDES_HEIGHT"));
    ComponentRefDesTextPropertiesSizer->Add(ComponentRefDesHeightText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesTextOrientTitle = new wxStaticText(ComponentRefDesTextPropertiesSizer->GetStaticBox(), wxID_ANY, _("Ориентация"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentRefDesTextPropertiesSizer->Add(ComponentRefDesTextOrientTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesOrientChoice = new wxChoice(ComponentRefDesTextPropertiesSizer->GetStaticBox(), ID_CHOICE_COMPONENT_REFDES_ORIENT, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_COMPONENT_REFDES_ORIENT"));
    ComponentRefDesOrientChoice->Disable();
    ComponentRefDesTextPropertiesSizer->Add(ComponentRefDesOrientChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesTextAlignTitle = new wxStaticText(ComponentRefDesTextPropertiesSizer->GetStaticBox(), wxID_ANY, _("Выравнивание"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentRefDesTextPropertiesSizer->Add(ComponentRefDesTextAlignTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesAlignChoice = new wxChoice(ComponentRefDesTextPropertiesSizer->GetStaticBox(), ID_CHOICE_COMPONENT_REFDES_ALIGN, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_COMPONENT_REFDES_ALIGN"));
    ComponentRefDesAlignChoice->Disable();
    ComponentRefDesTextPropertiesSizer->Add(ComponentRefDesAlignChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentRefDesPropertiesSizer->Add(ComponentRefDesTextPropertiesSizer, 1, wxTOP|wxEXPAND, 5);
    ComponentsBoxSizer->Add(ComponentRefDesPropertiesSizer, 1, wxTOP|wxEXPAND, 5);
    ComponentPinsInfoSizer = new wxStaticBoxSizer(wxVERTICAL, ComponentsBoxSizer->GetStaticBox(), _("Выводов - 0"));
    ComponentPinsListChoice = new wxChoice(ComponentPinsInfoSizer->GetStaticBox(), ID_CHOICE_COMPONENT_PINS_LIST, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_COMPONENT_PINS_LIST"));
    ComponentPinsListChoice->SetToolTip(_("Список выводов компонента"));
    ComponentPinsInfoSizer->Add(ComponentPinsListChoice, 0, wxBOTTOM|wxEXPAND, 5);
    PinPropertiesSizer = new wxBoxSizer(wxVERTICAL);
    ComponentPinAlNumNameLayerSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentPinAlNumTitle = new wxStaticText(ComponentPinsInfoSizer->GetStaticBox(), wxID_ANY, _("БЦН"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentPinAlNumNameLayerSizer->Add(ComponentPinAlNumTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinAlNumText = new wxTextCtrl(ComponentPinsInfoSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_ALNUM, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_ALNUM"));
    ComponentPinAlNumNameLayerSizer->Add(ComponentPinAlNumText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinNameTitle = new wxStaticText(ComponentPinsInfoSizer->GetStaticBox(), wxID_ANY, _("Имя"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentPinAlNumNameLayerSizer->Add(ComponentPinNameTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinNameText = new wxTextCtrl(ComponentPinsInfoSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_NAME, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_NAME"));
    ComponentPinAlNumNameLayerSizer->Add(ComponentPinNameText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLayerTitle = new wxStaticText(ComponentPinsInfoSizer->GetStaticBox(), wxID_ANY, _("Слой"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    ComponentPinAlNumNameLayerSizer->Add(ComponentPinLayerTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLayerText = new wxTextCtrl(ComponentPinsInfoSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_LAYER, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_LAYER"));
    ComponentPinAlNumNameLayerSizer->Add(ComponentPinLayerText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PinPropertiesSizer->Add(ComponentPinAlNumNameLayerSizer, 1, wxBOTTOM|wxEXPAND, 5);
    ComponentPinPropertiesSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentPinCoordsTitle = new wxStaticText(ComponentPinsInfoSizer->GetStaticBox(), ID_STATICTEXT1, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    ComponentPinPropertiesSizer->Add(ComponentPinCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinCoordsText = new wxTextCtrl(ComponentPinsInfoSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_COORDS"));
    ComponentPinPropertiesSizer->Add(ComponentPinCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinTypeTitle = new wxStaticText(ComponentPinsInfoSizer->GetStaticBox(), ID_STATICTEXT3, _("Тип"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    ComponentPinPropertiesSizer->Add(ComponentPinTypeTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinTypeText = new wxTextCtrl(ComponentPinsInfoSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_TYPE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_TYPE"));
    ComponentPinPropertiesSizer->Add(ComponentPinTypeText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinEquivTitle = new wxStaticText(ComponentPinsInfoSizer->GetStaticBox(), ID_STATICTEXT4, _("Эквивалент"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    ComponentPinPropertiesSizer->Add(ComponentPinEquivTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinEquivText = new wxTextCtrl(ComponentPinsInfoSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_EQUIV, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_EQUIV"));
    ComponentPinPropertiesSizer->Add(ComponentPinEquivText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PinPropertiesSizer->Add(ComponentPinPropertiesSizer, 1, wxEXPAND, 5);
    ComponentPinsInfoSizer->Add(PinPropertiesSizer, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    ComponentPinLabelPropertiesSizer = new wxStaticBoxSizer(wxVERTICAL, ComponentPinsInfoSizer->GetStaticBox(), _("Этикетка вывода"));
    ComponentPinLabelCoordsLayerSizer = new wxBoxSizer(wxHORIZONTAL);
    ComponentPinLabelCoordsTitle = new wxStaticText(ComponentPinLabelPropertiesSizer->GetStaticBox(), ID_STATICTEXT2, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    ComponentPinLabelCoordsLayerSizer->Add(ComponentPinLabelCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelCoordsText = new wxTextCtrl(ComponentPinLabelPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_LABEL_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_LABEL_COORDS"));
    ComponentPinLabelCoordsLayerSizer->Add(ComponentPinLabelCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelLayerTitle = new wxStaticText(ComponentPinLabelPropertiesSizer->GetStaticBox(), ID_STATICTEXT5, _("Слой"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    ComponentPinLabelCoordsLayerSizer->Add(ComponentPinLabelLayerTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelLayerText = new wxTextCtrl(ComponentPinLabelPropertiesSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_LABEL_LAYER, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_LABEL_LAYER"));
    ComponentPinLabelCoordsLayerSizer->Add(ComponentPinLabelLayerText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelPropertiesSizer->Add(ComponentPinLabelCoordsLayerSizer, 1, wxEXPAND, 5);
    StaticBoxSizer2ComponentPinLabelTextParamsSizer = new wxStaticBoxSizer(wxHORIZONTAL, ComponentPinLabelPropertiesSizer->GetStaticBox(), _("Параметры текста"));
    ComponentPinLabelHeightTitle = new wxStaticText(StaticBoxSizer2ComponentPinLabelTextParamsSizer->GetStaticBox(), ID_STATICTEXT6, _("Высота"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    StaticBoxSizer2ComponentPinLabelTextParamsSizer->Add(ComponentPinLabelHeightTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelHeightText = new wxTextCtrl(StaticBoxSizer2ComponentPinLabelTextParamsSizer->GetStaticBox(), ID_TEXT_COMPONENT_PIN_LABEL_HEIGHT, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_COMPONENT_PIN_LABEL_HEIGHT"));
    StaticBoxSizer2ComponentPinLabelTextParamsSizer->Add(ComponentPinLabelHeightText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelOrientTitle = new wxStaticText(StaticBoxSizer2ComponentPinLabelTextParamsSizer->GetStaticBox(), ID_STATICTEXT7, _("Ориентация"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    StaticBoxSizer2ComponentPinLabelTextParamsSizer->Add(ComponentPinLabelOrientTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelOrientChoice = new wxChoice(StaticBoxSizer2ComponentPinLabelTextParamsSizer->GetStaticBox(), ID_CHOICE_COMPONENT_PIN_LABEL_ORIENT, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_COMPONENT_PIN_LABEL_ORIENT"));
    ComponentPinLabelOrientChoice->Disable();
    StaticBoxSizer2ComponentPinLabelTextParamsSizer->Add(ComponentPinLabelOrientChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelAlignTitle = new wxStaticText(StaticBoxSizer2ComponentPinLabelTextParamsSizer->GetStaticBox(), ID_STATICTEXT8, _("Выравнивание"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    StaticBoxSizer2ComponentPinLabelTextParamsSizer->Add(ComponentPinLabelAlignTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelAlignChoice = new wxChoice(StaticBoxSizer2ComponentPinLabelTextParamsSizer->GetStaticBox(), ID_CHOICE_COMPONENT_PIN_LABEL_ALIGN, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_COMPONENT_PIN_LABEL_ALIGN"));
    ComponentPinLabelAlignChoice->Disable();
    StaticBoxSizer2ComponentPinLabelTextParamsSizer->Add(ComponentPinLabelAlignChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ComponentPinLabelPropertiesSizer->Add(StaticBoxSizer2ComponentPinLabelTextParamsSizer, 1, wxTOP|wxEXPAND, 5);
    ComponentPinsInfoSizer->Add(ComponentPinLabelPropertiesSizer, 2, wxEXPAND, 5);
    ComponentsBoxSizer->Add(ComponentPinsInfoSizer, 2, wxTOP|wxEXPAND, 5);
    CommonDatabaseContentSizer->Add(ComponentsBoxSizer, 1, wxALL|wxEXPAND, 5);
    NetsBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Цепей - 0"));
    NetsListChoice = new wxChoice(NetsBoxSizer->GetStaticBox(), ID_CHOICE_NETS_LIST, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_NETS_LIST"));
    NetsBoxSizer->Add(NetsListChoice, 0, wxBOTTOM|wxEXPAND, 5);
    NetNameSizer = new wxBoxSizer(wxHORIZONTAL);
    NetNameTitle = new wxStaticText(NetsBoxSizer->GetStaticBox(), wxID_ANY, _("Имя"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    NetNameSizer->Add(NetNameTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    NetNameText = new wxTextCtrl(NetsBoxSizer->GetStaticBox(), ID_TEXT_NET_NAME, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_NET_NAME"));
    NetNameSizer->Add(NetNameText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IsNetUserNameCheck = new wxCheckBox(NetsBoxSizer->GetStaticBox(), ID_CHECK_IS_NET_USER_NAME, _("Назначено пользователем"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_IS_NET_USER_NAME"));
    IsNetUserNameCheck->SetValue(false);
    IsNetUserNameCheck->Disable();
    NetNameSizer->Add(IsNetUserNameCheck, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    NetsBoxSizer->Add(NetNameSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    NetObjectsInfoSizer = new wxStaticBoxSizer(wxVERTICAL, NetsBoxSizer->GetStaticBox(), _("Фрагментов - 0"));
    NetObjectsListChoice = new wxChoice(NetObjectsInfoSizer->GetStaticBox(), ID_CHOICE_NET_OBJECTS_LIST, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_NET_OBJECTS_LIST"));
    NetObjectsInfoSizer->Add(NetObjectsListChoice, 0, wxBOTTOM|wxEXPAND, 5);
    NetObjectDescriptionText = new wxTextCtrl(NetObjectsInfoSizer->GetStaticBox(), ID_TEXT_NET_OBJECT_DESCRIPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_NET_OBJECT_DESCRIPTION"));
    NetObjectsInfoSizer->Add(NetObjectDescriptionText, 1, wxTOP|wxEXPAND, 5);
    NetsBoxSizer->Add(NetObjectsInfoSizer, 1, wxALL|wxEXPAND, 5);
    CommonDatabaseContentSizer->Add(NetsBoxSizer, 1, wxALL|wxEXPAND, 5);
    InsertionsBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Вставок - 0"));
    InsertionsListChoice = new wxChoice(InsertionsBoxSizer->GetStaticBox(), ID_CHOICE_INSERTIONS_LIST, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_INSERTIONS_LIST"));
    InsertionsBoxSizer->Add(InsertionsListChoice, 0, wxBOTTOM|wxEXPAND, 5);
    InsertPropertiesSizer = new wxStaticBoxSizer(wxVERTICAL, InsertionsBoxSizer->GetStaticBox(), _("Свойства вставки"));
    InsertNamesSizer = new wxBoxSizer(wxHORIZONTAL);
    InsertNameTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Имя"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertNamesSizer->Add(InsertNameTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertNameText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_NAME, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_NAME"));
    InsertNamesSizer->Add(InsertNameText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertNameCoordsTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertNamesSizer->Add(InsertNameCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertNameCoordsText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_NAME_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_NAME_COORDS"));
    InsertNamesSizer->Add(InsertNameCoordsText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IsInsertUserNameCheck = new wxCheckBox(InsertPropertiesSizer->GetStaticBox(), ID_CHECK_IS_INSERT_USER_NAME, _("Назначено пользователем"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_IS_INSERT_USER_NAME"));
    IsInsertUserNameCheck->SetValue(false);
    IsInsertUserNameCheck->Disable();
    InsertNamesSizer->Add(IsInsertUserNameCheck, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertComponentNameTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Компонент"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertNamesSizer->Add(InsertComponentNameTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertComponentNameText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_COMPONENT_NAME, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_COMPONENT_NAME"));
    InsertNamesSizer->Add(InsertComponentNameText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPropertiesSizer->Add(InsertNamesSizer, 1, wxTOP|wxEXPAND, 5);
    InsertCoordsSizer = new wxBoxSizer(wxHORIZONTAL);
    InsertCoordsTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertCoordsSizer->Add(InsertCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertCoordsText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_COORDS"));
    InsertCoordsSizer->Add(InsertCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IsInsertMirroring = new wxCheckBox(InsertPropertiesSizer->GetStaticBox(), ID_CHECK_IS_INSERT_MIRRORING, _("Зеркально"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_IS_INSERT_MIRRORING"));
    IsInsertMirroring->SetValue(false);
    IsInsertMirroring->Disable();
    InsertCoordsSizer->Add(IsInsertMirroring, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IsInsertOnTop = new wxCheckBox(InsertPropertiesSizer->GetStaticBox(), ID_CHECK_IS_INSERT_ON_TOP, _("На стороне деталей"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_IS_INSERT_ON_TOP"));
    IsInsertOnTop->SetValue(false);
    IsInsertOnTop->Disable();
    InsertCoordsSizer->Add(IsInsertOnTop, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPropertiesSizer->Add(InsertCoordsSizer, 1, wxTOP|wxEXPAND, 5);
    InsertRotateScaleSizer = new wxBoxSizer(wxHORIZONTAL);
    InsertScaleXTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Масштаб по X"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRotateScaleSizer->Add(InsertScaleXTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertScaleXText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_SCALE_X, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_SCALE_X"));
    InsertRotateScaleSizer->Add(InsertScaleXText, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertScaleYTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Масштаб по Y"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRotateScaleSizer->Add(InsertScaleYTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertScaleYText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_SCALE_Y, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_SCALE_Y"));
    InsertRotateScaleSizer->Add(InsertScaleYText, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRotateFactorTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Фактор поворота"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRotateScaleSizer->Add(InsertRotateFactorTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRotateFactorText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_ROTATE_FACTOR, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_ROTATE_FACTOR"));
    InsertRotateScaleSizer->Add(InsertRotateFactorText, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertSetAngleTitle = new wxStaticText(InsertPropertiesSizer->GetStaticBox(), wxID_ANY, _("Угол установки"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRotateScaleSizer->Add(InsertSetAngleTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertSetAngleText = new wxTextCtrl(InsertPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_SET_ANGLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_SET_ANGLE"));
    InsertRotateScaleSizer->Add(InsertSetAngleText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPropertiesSizer->Add(InsertRotateScaleSizer, 1, wxTOP|wxEXPAND, 5);
    InsertionsBoxSizer->Add(InsertPropertiesSizer, 1, wxTOP|wxEXPAND, 5);
    InsertRefDesSizer = new wxStaticBoxSizer(wxVERTICAL, InsertionsBoxSizer->GetStaticBox(), _("Конструкторское обозначение"));
    InsertRefDesCoordsLayerSizer = new wxBoxSizer(wxHORIZONTAL);
    InsertRefDesCoordsTitle = new wxStaticText(InsertRefDesSizer->GetStaticBox(), wxID_ANY, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRefDesCoordsLayerSizer->Add(InsertRefDesCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesCoordsText = new wxTextCtrl(InsertRefDesSizer->GetStaticBox(), ID_TEXT_INSERT_REFDES_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_REFDES_COORDS"));
    InsertRefDesCoordsLayerSizer->Add(InsertRefDesCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesLayerTitle = new wxStaticText(InsertRefDesSizer->GetStaticBox(), wxID_ANY, _("Слой"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRefDesCoordsLayerSizer->Add(InsertRefDesLayerTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesLayerText = new wxTextCtrl(InsertRefDesSizer->GetStaticBox(), ID_TEXT_INSERT_REFDES_LAYER, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_REFDES_LAYER"));
    InsertRefDesCoordsLayerSizer->Add(InsertRefDesLayerText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesSizer->Add(InsertRefDesCoordsLayerSizer, 1, wxTOP|wxEXPAND, 5);
    InsertRefDesTextParamsSizer = new wxStaticBoxSizer(wxHORIZONTAL, InsertRefDesSizer->GetStaticBox(), _("Параметры текста"));
    InsertRefDesHeightTitle = new wxStaticText(InsertRefDesTextParamsSizer->GetStaticBox(), wxID_ANY, _("Высота"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRefDesTextParamsSizer->Add(InsertRefDesHeightTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesHeightText = new wxTextCtrl(InsertRefDesTextParamsSizer->GetStaticBox(), ID_TEXT_INSERT_REFDES_HEIGHT, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_REFDES_HEIGHT"));
    InsertRefDesTextParamsSizer->Add(InsertRefDesHeightText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesOrientTitle = new wxStaticText(InsertRefDesTextParamsSizer->GetStaticBox(), wxID_ANY, _("Ориентация"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRefDesTextParamsSizer->Add(InsertRefDesOrientTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesOrientChoice = new wxChoice(InsertRefDesTextParamsSizer->GetStaticBox(), ID_CHOICE_INSERT_REFDES_ORIENT, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_INSERT_REFDES_ORIENT"));
    InsertRefDesOrientChoice->Disable();
    InsertRefDesTextParamsSizer->Add(InsertRefDesOrientChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesAlignTitle = new wxStaticText(InsertRefDesTextParamsSizer->GetStaticBox(), wxID_ANY, _("Выравнивание"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertRefDesTextParamsSizer->Add(InsertRefDesAlignTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesAlignChoice = new wxChoice(InsertRefDesTextParamsSizer->GetStaticBox(), ID_CHOICE_INSERT_REFDES_ALIGN, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_INSERT_REFDES_ALIGN"));
    InsertRefDesAlignChoice->Disable();
    InsertRefDesTextParamsSizer->Add(InsertRefDesAlignChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertRefDesSizer->Add(InsertRefDesTextParamsSizer, 1, wxTOP|wxEXPAND, 5);
    InsertionsBoxSizer->Add(InsertRefDesSizer, 1, wxTOP|wxEXPAND, 5);
    InsertPinsInfoSizer = new wxStaticBoxSizer(wxVERTICAL, InsertionsBoxSizer->GetStaticBox(), _("Выводов - 0"));
    InsertPinNamesListChoice = new wxChoice(InsertPinsInfoSizer->GetStaticBox(), ID_CHOICE_INSERT_PIN_NAMES_LIST, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_INSERT_PIN_NAMES_LIST"));
    InsertPinsInfoSizer->Add(InsertPinNamesListChoice, 0, wxBOTTOM|wxEXPAND, 5);
    InsertPinNamesSizer = new wxBoxSizer(wxHORIZONTAL);
    InsertPinAlNumTitle = new wxStaticText(InsertPinsInfoSizer->GetStaticBox(), wxID_ANY, _("БЦН"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinNamesSizer->Add(InsertPinAlNumTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinAlNumText = new wxTextCtrl(InsertPinsInfoSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_ALNUM, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_ALNUM"));
    InsertPinNamesSizer->Add(InsertPinAlNumText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinNameTitle = new wxStaticText(InsertPinsInfoSizer->GetStaticBox(), wxID_ANY, _("Имя"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinNamesSizer->Add(InsertPinNameTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinNameText = new wxTextCtrl(InsertPinsInfoSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_NAME, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_NAME"));
    InsertPinNamesSizer->Add(InsertPinNameText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinTypeTitle = new wxStaticText(InsertPinsInfoSizer->GetStaticBox(), ID_STATICTEXT9, _("Тип"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    InsertPinNamesSizer->Add(InsertPinTypeTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinTypeText = new wxTextCtrl(InsertPinsInfoSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_TYPE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_TYPE"));
    InsertPinNamesSizer->Add(InsertPinTypeText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinNetNameConnectTitle = new wxStaticText(InsertPinsInfoSizer->GetStaticBox(), ID_STATICTEXT10, _("Соединён с цепью"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    InsertPinNamesSizer->Add(InsertPinNetNameConnectTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinNetNameConnectText = new wxTextCtrl(InsertPinsInfoSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_NET_NAME_CONNECT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_NET_NAME_CONNECT"));
    InsertPinNamesSizer->Add(InsertPinNetNameConnectText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinsInfoSizer->Add(InsertPinNamesSizer, 1, wxTOP|wxEXPAND, 5);
    InsertPinLabelPropertiesSizer = new wxStaticBoxSizer(wxVERTICAL, InsertPinsInfoSizer->GetStaticBox(), _("Этикетка вывода"));
    InsertPinLabelCoordsLayerSizer = new wxBoxSizer(wxHORIZONTAL);
    InsertPinLabelCoordsTitle = new wxStaticText(InsertPinLabelPropertiesSizer->GetStaticBox(), wxID_ANY, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinLabelCoordsLayerSizer->Add(InsertPinLabelCoordsTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelCoordsText = new wxTextCtrl(InsertPinLabelPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_LABEL_COORDS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_LABEL_COORDS"));
    InsertPinLabelCoordsLayerSizer->Add(InsertPinLabelCoordsText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelLayerTitle = new wxStaticText(InsertPinLabelPropertiesSizer->GetStaticBox(), wxID_ANY, _("Слой"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinLabelCoordsLayerSizer->Add(InsertPinLabelLayerTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelLayerText = new wxTextCtrl(InsertPinLabelPropertiesSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_LABEL_LAYER, _("Нет"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_LABEL_LAYER"));
    InsertPinLabelCoordsLayerSizer->Add(InsertPinLabelLayerText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelPropertiesSizer->Add(InsertPinLabelCoordsLayerSizer, 1, wxEXPAND, 5);
    InsertPinLabelTextParamsSizer = new wxStaticBoxSizer(wxHORIZONTAL, InsertPinLabelPropertiesSizer->GetStaticBox(), _("Параметры текста"));
    InsertPinLabelHeightTitle = new wxStaticText(InsertPinLabelTextParamsSizer->GetStaticBox(), wxID_ANY, _("Высота"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinLabelTextParamsSizer->Add(InsertPinLabelHeightTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelHeightText = new wxTextCtrl(InsertPinLabelTextParamsSizer->GetStaticBox(), ID_TEXT_INSERT_PIN_LABEL_HEIGHT, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXT_INSERT_PIN_LABEL_HEIGHT"));
    InsertPinLabelTextParamsSizer->Add(InsertPinLabelHeightText, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelOrientTitle = new wxStaticText(InsertPinLabelTextParamsSizer->GetStaticBox(), wxID_ANY, _("Ориентация"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinLabelTextParamsSizer->Add(InsertPinLabelOrientTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelOrientChoice = new wxChoice(InsertPinLabelTextParamsSizer->GetStaticBox(), ID_CHOICE_INSERT_PIN_LABEL_ORIENT, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_INSERT_PIN_LABEL_ORIENT"));
    InsertPinLabelOrientChoice->Disable();
    InsertPinLabelTextParamsSizer->Add(InsertPinLabelOrientChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelAlignTitle = new wxStaticText(InsertPinLabelTextParamsSizer->GetStaticBox(), wxID_ANY, _("Выравнивание"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    InsertPinLabelTextParamsSizer->Add(InsertPinLabelAlignTitle, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelAlignChoice = new wxChoice(InsertPinLabelTextParamsSizer->GetStaticBox(), ID_CHOICE_INSERT_PIN_LABEL_ALIGN, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_INSERT_PIN_LABEL_ALIGN"));
    InsertPinLabelAlignChoice->Disable();
    InsertPinLabelTextParamsSizer->Add(InsertPinLabelAlignChoice, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    InsertPinLabelPropertiesSizer->Add(InsertPinLabelTextParamsSizer, 1, wxTOP|wxEXPAND, 5);
    InsertPinsInfoSizer->Add(InsertPinLabelPropertiesSizer, 2, wxTOP|wxEXPAND, 5);
    InsertionsBoxSizer->Add(InsertPinsInfoSizer, 2, wxTOP|wxEXPAND, 5);
    CommonDatabaseContentSizer->Add(InsertionsBoxSizer, 1, wxALL|wxEXPAND, 5);
    MainDialogSizer->Add(CommonDatabaseContentSizer, 0, wxEXPAND, 5);
    FooterButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    ExportAsTextButton = new wxButton(this, ID_BUTTON_EXPORT_AS_TEXT, _("Экспортировать как текст"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_EXPORT_AS_TEXT"));
    FooterButtonsSizer->Add(ExportAsTextButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ExportAsXMLButton = new wxButton(this, ID_BUTTON_EXPORT_AS_XML, _("Экспортировать как XML"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_EXPORT_AS_XML"));
    FooterButtonsSizer->Add(ExportAsXMLButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ExitDialogButton = new wxButton(this, ID_BUTTON_EXIT_DIALOG, _("Выйти"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_EXIT_DIALOG"));
    FooterButtonsSizer->Add(ExitDialogButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    MainDialogSizer->Add(FooterButtonsSizer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(MainDialogSizer);
    Layout();
    Center();

    Connect(ID_CHOICE_COMPONENTS_LIST, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&DatabaseResBrowser::OnComponentsListChoiceSelect);
    Connect(ID_CHOICE_COMPONENT_PINS_LIST, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&DatabaseResBrowser::OnComponentPinsListChoiceSelect);
    Connect(ID_CHOICE_NETS_LIST, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&DatabaseResBrowser::OnNetsListChoiceSelect);
    Connect(ID_CHOICE_NET_OBJECTS_LIST, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&DatabaseResBrowser::OnNetObjectsListChoiceSelect);
    Connect(ID_CHOICE_INSERTIONS_LIST, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&DatabaseResBrowser::OnInsertionsListChoiceSelect);
    Connect(ID_CHOICE_INSERT_PIN_NAMES_LIST, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&DatabaseResBrowser::OnInsertPinNamesListChoiceSelect);
    Connect(ID_BUTTON_EXPORT_AS_TEXT, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&DatabaseResBrowser::OnExportAsTextButtonClick);
    Connect(ID_BUTTON_EXPORT_AS_XML, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&DatabaseResBrowser::OnExportAsXMLButtonClick);
    Connect(ID_BUTTON_EXIT_DIALOG, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&DatabaseResBrowser::OnExitDialogButtonClick);
    Connect(wxID_ANY, wxEVT_INIT_DIALOG, (wxObjectEventFunction)&DatabaseResBrowser::OnInit);
    Connect(wxID_ANY, wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&DatabaseResBrowser::OnClose);
    //*)

    // Установим постоянное содержимое списков ориентации и выравнивания текстов.
    for (int scan_text_orient = static_cast<int>(TextOrientation::TEXT_ORIENT_MIN);
         scan_text_orient <= static_cast<int>(TextOrientation::TEXT_ORIENT_MAX); ++scan_text_orient)
    {
        wxString text_orient_string = wxTR(TextOrientToString(static_cast<TextOrientation>(scan_text_orient)));
        ComponentRefDesOrientChoice->Append(text_orient_string);
        ComponentPinLabelOrientChoice->Append(text_orient_string);
        InsertRefDesOrientChoice->Append(text_orient_string);
        InsertPinLabelOrientChoice->Append(text_orient_string);
    }

    for (int scan_text_align = static_cast<int>(TextAlign::TEXT_ALIGN_MIN);
         scan_text_align <= static_cast<int>(TextAlign::TEXT_ALIGN_MAX); ++scan_text_align)
    {
        wxString text_align_string = wxTR(TextAlignToString(static_cast<TextAlign>(scan_text_align)));
        ComponentRefDesAlignChoice->Append(text_align_string);
        ComponentPinLabelAlignChoice->Append(text_align_string);
        InsertRefDesAlignChoice->Append(text_align_string);
        InsertPinLabelAlignChoice->Append(text_align_string);
    }
    // Установка минимального и оптимального (текущего) размеров окна диалога средствами его главного разместителя.
    MainDialogSizer->SetSizeHints(this);
}

DatabaseResBrowser::~DatabaseResBrowser()
{
    //(*Destroy(DatabaseResBrowser)
    //*)
}

wxString DatabaseResBrowser::ConvertToUnicode(const std::string& narrow_string) const
{
    static wchar_t output_buffer[CONVERT_UNICODE_BUFFER_LEN];
    // narrow_string полагается однобайтовой строкой, имеющей кодировку, на которую настроен вход перекодировщика pdif_encoding_conv_.
    // Целевая кодировка перекодировщика всегда wxFONTENCODING_UNICODE (Юникод).
    if (narrow_string.size() <= (CONVERT_UNICODE_BUFFER_LEN / 4))
        pdif_encoding_conv_.Convert(narrow_string.c_str(), output_buffer);
    else
        pdif_encoding_conv_.Convert(narrow_string.substr(0, CONVERT_UNICODE_BUFFER_LEN / 4).c_str(), output_buffer);
    return wxString(output_buffer);
}

// Получение текстового описания для ошибки с кодом code.
wxString DatabaseResBrowser::DatabaseErrorToString(DatabaseErrorCode code)
{
    switch (code)
    {
    case DatabaseErrorCode::DATABASE_ERROR_NONE:
        return _("Нет ошибки");
    default:
        return _("Другая ошибка");
    }
    return {};
}

// Функция-член
void DatabaseResBrowser::LoadNewNetDescription(int net_index, int net_part_index)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    decltype(view_pcad_database_->nets_begin()) use_net_it;
    bool in_clear_mode = !view_pcad_database_ || net_index < 0 || net_index >= static_cast<int>(view_pcad_database_->nets_size());
    if (!in_clear_mode)
    {
        use_net_it = view_pcad_database_->nets_begin() + static_cast<size_t>(net_index);
        if (net_part_index < 0 || net_part_index >= static_cast<size_t>(use_net_it->size()))
            in_clear_mode = true;
    }
    if (in_clear_mode)
    {
        NetObjectDescriptionText->Clear();
    }
    else
    {
        const GraphObj* net_part_graph = *(use_net_it->begin() + net_part_index);
        NetObjectDescriptionText->SetValue(ConvertToUnicode(net_part_graph->GetObjectLongText()));
    }

    block_signals_ = old_block_signals;
}

void DatabaseResBrowser::LoadNewNet(int net_index)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    if (!view_pcad_database_ || net_index < 0 || net_index >= static_cast<int>(view_pcad_database_->nets_size()))
    { // Индекс net_index не соответствует какой-либо реальной цепи - выполняем очистку этой группы информационных виджетов.
        NetNameText->Clear();
        IsNetUserNameCheck->SetValue(false);
        // ----------
        NetObjectsInfoSizer->GetStaticBox()->SetLabel(wxTR(FRAGMENTS_TEXT) + ZERO_VALUE_STR);
        NetObjectsListChoice->Clear();
        LoadNewNetDescription(int net_index, -1);
        // ----------
        block_signals_ = old_block_signals;
        return;
    }

    auto use_net_it = view_pcad_database_->nets_begin() + static_cast<size_t>(net_index);
    // Разыменовывание итератор use_net_it даёт переменную типа NetDefDesc.
    NetNameText->SetValue(ConvertToUnicode(use_net_it->GetName()));
    IsNetUserNameCheck->SetValue(use_net_it->IsUsetNetName());
    // ----------
    NetObjectsInfoSizer->GetStaticBox()->SetLabel(wxTR(FRAGMENTS_TEXT) + wxFI(use_net_it->size()));
    NetObjectsListChoice->Clear();
    for (auto net_part_it = use_net_it->begin(); net_part_it != use_net_it->end(); ++net_part_it)
    { // Разыменование итератора net_part_it ведёт к указателю типа GraphObj*.
        GraphObj* net_graph_obj = *net_part_it;
        NetObjectsListChoice->Append(ConvertToUnicode(net_graph_obj->GetObjectShortText()));
    }
    if (NetObjectsListChoice->GetCount())
    { // Список фрагментов цепи не пуст.
        NetObjectsListChoice->SetSelection(0);
        LoadNewNetDescription(int net_index, 0);
    }
    else
    { // Токопроводящая цепь не содержит никаких фрагментов.
        NetObjectsListChoice->SetSelection(wxNOT_FOUND);
        LoadNewNetDescription(int net_index, -1);
    }

    block_signals_ = old_block_signals;
}

void DatabaseResBrowser::LoadNewNet(const string& net_name)
{
    if (!view_pcad_database_)
    {
        LoadNewNet(-1);
        return;
    }

    auto database_nets_it = view_pcad_database_->nets_begin();
    for (; database_nets_it != view_pcad_database_->nets_end(); ++database_nets_it)
    {
        if (database_nets_it->GetName() == net_name)
            break;
    }
    if (database_nets_it != view_pcad_database_->nets_end())
        LoadNewNet(database_nets_it - view_pcad_database_->nets_begin());
    else
        LoadNewNet(-1);
}

void DatabaseResBrowser::SetPinTypeToText(const PinType& pin_type, wxTextCtrl* pin_type_text_field)
{
    if (std::holds_alternative<int>(pin_type))
        pin_type_text_field->SetValue(wxFI(std::get<int>(pin_type)));
    else if (std::holds_alternative<std::string>(pin_type))
        pin_type_text_field->SetValue(ConvertToUnicode(std::get<std::string>(pin_type)));
    else
        pin_type_text_field->Clear();
}

// Метод загрузки в виджеты формы диалога информации об определённом выводе библиотечного радиокомпонента.
void DatabaseResBrowser::LoadCompPinDescription(int component_index, int pin_index)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    decltype(view_pcad_database_->radio_components_begin()) use_component_it;
    const RadioComponentDesc* component_desc = nullptr;
    decltype(use_component_it->pins_begin()) use_pin_it;
    const ComponentPinDef* pin_def = nullptr;

    bool in_clear_mode = !view_pcad_database_ || component_index < 0 ||
        component_index >= static_cast<int>(view_pcad_database_->radio_components_size());
    if (!in_clear_mode)
    {
        use_component_it = view_pcad_database_->radio_components_begin() + static_cast<size_t>(component_index);
        component_desc = &(*use_component_it);
        if (pin_index < 0 || pin_index >= static_cast<int>(use_component_it->pins_size()))
        {
            in_clear_mode = true;
        }
        else
        {
            use_pin_it = use_component_it->pins_begin() + static_cast<size_t>(pin_index);
            pin_def = &(*use_pin_it);
        }
    }

    if (in_clear_mode)
    { // Режим очистки виджетов с информацией
        ComponentPinAlNumText->Clear();
        ComponentPinNameText->Clear();
        // ----------
        ComponentPinLayerText->Clear();
        ComponentPinCoordsText->Clear();
        ComponentPinTypeText->Clear();
        ComponentPinEquivText->Clear();
        // ----------
        ComponentPinLabelCoordsText->Clear();
        ComponentPinLabelLayerText->Clear();
        ComponentPinLabelHeightText->SetValue(ZERO_VALUE_STR);
        //
        ComponentPinLabelAlignChoice->Clear();
        ComponentPinLabelAlignChoice->SetSelection(wxNOT_FOUND);
        ComponentPinLabelOrientChoice->Clear();
        ComponentPinLabelOrientChoice->SetSelection(wxNOT_FOUND);

        block_signals_ = old_block_signals;
        return;
    }
    // Мз базы данных документа выделены корректные указатели на описатель вывода (ножки) с порядковым индексом pin_index
    // библиотечного элемента component_index.
    ComponentPinAlNumText->SetValue(ConvertToUnicode(pin_def->pin_al_number));
    ComponentPinNameText->SetValue(ConvertToUnicode(pin_def->pin_name));
    // ----------
    ComponentPinLayerText->SetValue(ConvertToUnicode(view_pcad_database_->GetLayerDesc(pin_def->layer_number).layer_name));
    ComponentPinCoordsText->SetValue(ConvertPointToString(pin_def->pin_coords, view_draw_context_));
    SetPinTypeToText(pin_def->pin_type, ComponentPinTypeText);
    ComponentPinEquivText->SetValue(wxFI(pin_def->equive_code));
    // ----------
    ComponentPinLabelCoordsText->SetValue(ConvertPointToString(pin_def->pin_label.pin_name_coords, view_draw_context_));
    ComponentPinLabelLayerText->SetValue(ConvertToUnicode(view_pcad_database_->GetLayerDesc(pin_def->pin_label.layer_number).layer_name));
    ComponentPinLabelHeightText->SetValue(wxFI(pin_def->pin_label.text_height));
    ComponentPinLabelAlignChoice->SetSelection(static_cast<int>(pin_def->pin_label.text_align));
    ComponentPinLabelOrientChoice->SetSelection(static_cast<int>(pin_def->pin_label.text_orient));

    block_signals_ = old_block_signals;
}

// Метод загрузки в виджеты формы диалога информации об определённом выводе вставленной копии радиокомпонента.
// insertion_index - порядковый индекс вставки в базе документа, insert_pin_index - .
// pin_label - указатель на этикету, connected_pin_info - сведения о соединении вывода с токопроводящими цепями.
void DatabaseResBrowser::LoadInsertPinDescription(int insertion_index, int insert_pin_index)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;
    // Указатели на объект обрабатываемой вставки.
    decltype(view_pcad_database_->radio_comp_inserts_begin()) use_component_insert_it;
    const RadioComponentInsertion* component_insert = nullptr;
    // Указатели на библиотечный объект вставляемого радиокомпонента (то есть того из них, копия которого вставляется).
    decltype(view_pcad_database_->radio_components_begin()) use_component_it;
    const RadioComponentDesc* component_desc = nullptr;
    // Указатели на информацию о выводе (ножке) вставляемого экземпляра с индексом insert_pin_index.
    decltype(component_insert->connect_info_begin()) use_connected_pin_info_it;
    const RadioComponentInsertion::PinNetConnectInfo* connected_pin_info = nullptr;
    // Указатели на вывод (ножку) библиотечного компонента, соответствующего выводу (ножке) вставляемого экземпляра с индексом insert_pin_index.
    decltype(use_component_it->pins_begin()) use_component_pin_it;
    const ComponentPinDef* component_pin_def = nullptr;

    auto do_clear_op = [this, &old_block_signals](bool is_clear_all) -> void
        {
            if (is_clear_all)
            {
                InsertPinNameText->Clear();
                InsertPinNetNameConnectText->Clear();
            }
            InsertPinAlNumText->Clear();
            InsertPinTypeText->Clear();
            // ----------
            LoadTextParams(text_groups_map_.at(TextParamGroup::TEXT_GROUP_INSERT_PIN_LABEL), CLEAR_TEXT_PARAMS_);
            // ----------
            block_signals_ = old_block_signals;
        };

    if (view_pcad_database_ && insertion_index >= 0 && insertion_index < static_cast<int>(view_pcad_database_->radio_comp_inserts_size()))
    {
        use_component_insert_it = view_pcad_database_->radio_comp_inserts_begin() + static_cast<size_t>(insertion_index);
        component_insert = &(*use_component_insert_it);
        // component_index - порядковый индекс компонента внутри их библиотеки, входящей в состав текущего документа.
        int component_index = component_insert->GetComponentNumber();
        if (component_index >= 0 && component_index < static_cast<int>(view_pcad_database_->radio_components_size()))
        {
            use_component_it = view_pcad_database_->radio_components_begin() + static_cast<size_t>(component_index);
            component_desc = &(*use_component_it);
        }
        else
        {
            do_clear_op(true);
            return;
        }
    }
    else
    {
        do_clear_op(true);
        return;
    }

    if (insert_pin_index >= 0 && insert_pin_index <= static_cast<int>(component_insert->connect_info_size()))
    {
        use_connected_pin_info_it = component_insert->connect_info_begin() + insert_pin_index;
        connected_pin_info = &(*use_connected_pin_info_it);
        InsertPinNameText->SetValue(ConvertToUnicode(connected_pin_info.pin_name));
        InsertPinNetNameConnectText->SetValue(ConvertToUnicode(connected_pin_info.net_name));
    }
    else
    {
        do_clear_op(true);
        return;
    }

    if (connected_pin_info->pin_index >= 0 && connected_pin_info->pin_index < static_cast<int>(component_desc->pins_size()))
    { // Подсоединённый вывод insert_pin_index имеет соответствие среди выводов компонента component_desc.
        use_component_pin_it = component_desc->pins_begin() + static_cast<size_t>(connected_pin_info.pin_index);
        component_pin_def = &(*use_component_pin_it);
    }
    else
    {
        do_clear_op(false);
        return;
    }

    // Удалось успешно получить весь комплекс необходимых данных о выводе (ножке).
    InsertPinAlNumText->SetValue(ConvertToUnicode(component_pin_def->pin_al_number));
    SetPinTypeToText(component_pin_def->pin_type, InsertPinTypeText);
    // ----------
    LoadTextParams(text_groups_map_.at(TextParamGroup::TEXT_GROUP_INSERT_PIN_LABEL),
                   dynamic_cast<const ObjText*>(component_insert->GetRefDes()));

    block_signals_ = old_block_signals;
}

// Функция-член загрузки в ветвь информационного дерева ComponentSectInfoTree с основанием root_section_item данных об упаковке
// единичной секции с именем sect_name и распределением выводов one_sect_pack_info. Секция принадлежит компоненту component_index
// и группе секций group_index.
void DatabaseResBrowser::LoadSectPackInfoToTree
    (int component_index, int group_index, const string& sect_name, const PinNameToALNumber& one_sect_pack_info, wxTreeItemId& root_section_item)
{
    int sect_pin_index = 0;
    for (const auto& pin_info_pair : one_sect_pack_info)
    { // Загрузка информации об "алфавитно-цифровой ножке" конструктива, соответствующей очередному выводу УГО.
        const string& pin_name = pin_info_pair.first;
        const string& pin_al_number = pin_info_pair.second;
        // Создаем конечный узел дерева с информацией о парном отображении вывода pin_name на вывод pin_al_number.
        // Опять сконструируем структуру сопровождающих данных.
        SectDefTreeItemData* sect_tree_def = new SectDefTreeItemData;
        sect_tree_def->component_index = component_index;
        sect_tree_def->group_index = group_index;
        sect_tree_def->section_name = sect_name;
        sect_tree_def->pin_index = -1;
        // Данные конкретного вывода.
        sect_tree_def->pin_name;
        sect_tree_def->pin_al_number;
        sect_tree_def->pin_index = sect_pin_index;
        //
        wxTreeItemId pin_reflex_item = ComponentSectInfoTree->AppendItem
            (root_section_group, ConvertToUnicode(pin_name) + " <-> " + ConvertToUnicode(pin_al_number), -1, -1, root_section_item);
        ++sect_pin_index;
    }
}

// Функция-член загрузки в ветвь информационного дерева ComponentSectInfoTree с основанием root_section_group данных о группе
// секций sect_group_pack_info.
void DatabaseResBrowser::LoadSectGroupPackInfoToTree
    (int component_index, int group_index, const SectNameToPackInfo& sect_group_pack_info, wxTreeItemId& root_section_group)
{
    for (const auto& section_info_pair : sect_group_pack_info)
    {
        const string& section_name = section_info_pair.first;
        const PinNameToALNumber& section_pack_info = section_info_pair.second;
        // Создаём подкорень (основание новой ветви) для очередной секции - потомка корня общей секции root_section_group.
        // Создаём сопровождающий объект дополнительной информации для этого подкорня.
        SectDefTreeItemData* sect_tree_def = new SectDefTreeItemData;
        sect_tree_def->component_index = component_index;
        sect_tree_def->group_index = group_index;
        sect_tree_def->section_name = section_name;
        sect_tree_def->pin_index = -1;

        wxTreeItemId root_section_item = ComponentSectInfoTree->AppendItem
            (root_section_group, _("Секция ") + ConvertToUnicode(section_name), -1, -1, sect_tree_def);
        // Загружаем всю компоновочную информацию о секции в ветвь с основанием root_section_item.
        LoadSectPackInfoToTree(component_index, group_index, section_name, section_pack_info, root_section_item);
    }
}

void DatabaseResBrowser::LoadPKGData(int component_index)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    ComponentSectInfoTree->DeleteAllItems();
    if (component_index < 0 || component_index >= view_pcad_database_->radio_components_size())
    { // Режим очистки упаковочной информации.
        block_signals_ = old_block_signals;
        return;
    }

    auto use_component_it = view_pcad_database_->radio_components_begin();
    const RadioComponentDesc& component_desc = *use_component_it;
    const ComponentPKGSectDef& pkg_sect_def = component_desc.GetSectionsDefData().GetPKGSectDef();

    // Группа секций для УГО всегда одна, поэтому создаём для неё единственный условный корень.
    // Создаём сопровождающий объект дополнительной информации.
    SectDefTreeItemData* sect_tree_def = new SectDefTreeItemData;
    sect_tree_def->component_index = component_index;
    sect_tree_def->group_index = 0;
    sect_tree_def->pin_index = -1;
    wxTreeItemId root_section_group = ComponentSectInfoTree->AddRoot(_("УГО"), -1, -1, sect_tree_def);
    // Загружаем в информационное дерево информацию об этой единственной группе секций, делая её потомком корня root_section_group.
    LoadSectGroupPackInfoToTree(component_index, 0, pkg_sect_def.pin_pkg_data, root_section_group);

    block_signals_ = old_block_signals;
}

void DatabaseResBrowser::LoadSPKGData(int component_index, const ComponentSPKGSectDef& spkg_sect_def)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    ComponentSectInfoTree->DeleteAllItems();
    if (component_index < 0 || component_index >= view_pcad_database_->radio_components_size())
    { // Режим очистки упаковочной информации.
        block_signals_ = old_block_signals;
        return;
    }

    auto use_component_it = view_pcad_database_->radio_components_begin();
    const RadioComponentDesc& component_desc = *use_component_it;
    const ComponentSPKGSectDef& spkg_sect_def = component_desc.GetSectionsDefData().GetSPKGSectDef();

    // Для конструктива радиокомпонента групп секций может быть несколько. Каждую из них делаем отдельным корнем дерева.
    int root_group_index = 0;
    for (const SectNameToPackInfo& sect_pack_info : spkg_sect_def.sect_spkg_data)
    {
        // Создаём сопровождающий объект дополнительной информации.
        SectDefTreeItemData* sect_tree_def = new SectDefTreeItemData;
        sect_tree_def->component_index = component_index;
        sect_tree_def->group_index = root_group_index;
        sect_tree_def->pin_index = -1;
        // Создаём ещё один корень "леса" групп секций для группы sect_pack_info.
        wxTreeItemId current_sect_root =
            ComponentSectInfoTree->AddRoot(_("Секция ") + wxFI(root_group_index), -1, -1, sect_tree_def);
        // Загружаем в информационное дерево информацию о текущей группе секций, делая её потомком созданного
        // для неё корня current_sect_root.
        LoadSectGroupPackInfoToTree(component_index, root_group_index, sect_pack_info, current_sect_root);
        ++root_group_index;
    }

    block_signals_ = old_block_signals;
}

// Перегруженные функции-члены загрузки в группу виджетов параметров некоторой текстовой надписи.
void DatabaseResBrowser::LoadTextParams(const TextParamWidgets& param_widgets, const ObjText* text_object)
{
    if (text_object)
    {
        TextParamValues param_values
        {
            .pos = text_object->GetTextPoint(),
            .layer_number = text_object->GetLayerNumber(),
            .text_height = text_object->GetTextHeight(),
            .text_align = text_object->GetTextAlign(),
            .text_orient = text_object->GetTextOrientation()
        };
        LoadTextParams(param_widgets, param_values);
    }
    else
    {
        LoadTextParams(param_widgets, CLEAR_TEXT_PARAMS_);
    }
}

void DatabaseResBrowser::LoadTextParams(const TextParamWidgets& param_widgets, const TextParamValues& param_values)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    if (param_values.text_height == INT_MIN)
    { // Режим очистки группы виджетов с параметрами текста.
        param_widgets.CoordsText->Clear();
        param_widgets.LayerText->Clear();
        param_widgets.HeightText->Clear();
        param_widgets.AlignChoice->SetSelection(wxNOT_FOUND);
        param_widgets.OrientChoice->SetSelection(wxNOT_FOUND);
    }
    else
    { // Установка нового содержимого для виджетов, содержащих параметры текста.
        param_widgets.CoordsText->SetValue(ConvertPointToString(param_values.pos, view_draw_context_));
        param_widgets.LayerText->SetValue(ConvertToUnicode(view_pcad_database_->GetLayerDesc(param_values.layer_number).layer_name));
        param_widgets.HeightText->SetValue(wxFI(param_values.text_height));
        param_widgets.AlignChoice->SetSelection(static_cast<int>(param_values.text_align));
        param_widgets.OrientChoice->SetSelection(static_cast<int>(param_values.text_orient));
    }

    block_signals_ = old_block_signals;
}

void DatabaseResBrowser::LoadNewComponent(int component_index)
{
    bool old_block_signals = block_signals_;
    block_signals_ = true;

    if (!view_pcad_database_ || component_index < 0 || component_index >= static_cast<int>(view_pcad_database_->radio_components_size()))
    { // Индекс component_index не соответствует какому-либо существующему радиокомпоненту - выполняем очистку этой
      // группы информационных виджетов.
        ComponentNameText->Clear();
        ComponentPackageIDText->Clear();
        ComponentTyIDText->Clear();
        ComponentOrgCoordsText->Clear();
        IsComponentJumper->SetValue(false);
        IsComponentPlanar->SetValue(false);
        // ----------
        ComponentSectionsInfoSizer->GetStaticBox()->SetLabel(wxTR(SECTIONS_TEXT) + ZERO_VALUE_STR);
        ComponentSectInfoTree->DeleteAllItems();
        // ----------
        ComponentRefDesCoordsText->Clear();
        ComponentRefDesLayerText->Clear();
        ComponentRefDesHeightText->SetValue(ZERO_VALUE_STR);
        ComponentRefDesAlignChoice->SetSelection(0);
        ComponentRefDesOrientChoice->SetSelection(0);
        // ----------
        ComponentPinsListChoice->Clear();
        ComponentPinsListChoice->SetSelection(wxNOT_FOUND);
        // ----------
        ComponentPinsInfoSizer->GetStaticBox()->SetLabel(wxTR(PINS_TEXT) + ZERO_VALUE_STR);
        ComponentPinAlNumText->Clear();
        ComponentPinNameText->Clear();
        ComponentPinLayerText->Clear();
        ComponentPinCoordsText->Clear();
        ComponentPinTypeText->Clear();
        ComponentPinEquivText->Clear();
        // ----------
        ComponentPinLabelCoordsText->Clear();
        ComponentPinLabelLayerText->Clear();
        ComponentPinLabelHeightText->SetValue(ZERO_VALUE_STR);
        ComponentPinLabelAlignChoice->SetSelection(0);
        ComponentPinLabelOrientChoice->SetSelection(0);
        block_signals_ = old_block_signals;
        return;
    }

    auto load_component_it = view_pcad_database_->radio_components_begin() + component_index;
    const RadioComponentDesc& loaded_component = *load_component_it;
    // ----------
    ComponentNameText->SetValue(ConvertToUnicode(loaded_component.GetName()));
    ComponentPackageIDText->SetValue(ConvertToUnicode(loaded_component.GetPackageID()));
    ComponentTyIDText->SetValue(wxFI(loaded_component.GetTyID()));
    ComponentOrgCoordsText->SetValue(ConvertPointToString(loaded_component.GetOrgPos(), view_draw_context_));
    IsComponentJumper->SetValue(loaded_component.IsJumper());
    IsComponentPlanar->SetValue(loaded_component.IsSMD());
    // ----- Загружаем описание секций радиокомпонента в поле ComponentSectInfoTree в древовидной форме. -----
    const ComponentSectDef& sects_data = loaded_component.GetSectionsDefData();
    ComponentSectionsInfoSizer->GetStaticBox()->SetLabel(wxTR(SECTIONS_TEXT) + wxFI(sects_data.GetCount()));
    if (sects_data.IsValid())
    {
        if (sects_data.IsSPKG())
            LoadSPKGData(component_index, sects_data.GetSPKGSectDef());
        else
            LoadPKGData(component_index, sects_data.GetPKGSectDef());
    }
    else
    {
        LoadPKGData(-1, PCADFile::COMPONENT_PKG_INVALID);
    }
    // ---- Производим загрузку предварительной информации о параметрах конструкторского (позиционного) обозначения компонента.
    const RefDesDef& component_ref_des = loaded_component.GetRefDes();
    TextParamValues component_text_params
    {
        .pos = component_ref_des.pos,
        .layer_number = component_ref_des.layer_number,
        .text_height = component_ref_des.text_height,
        .text_align = component_ref_des.text_align,
        .text_orient = component_ref_des.text_orient
    };
    LoadTextParams(text_groups_map_.at(TextParamGroup::TEXT_GROUP_COMPONENT_REFDES), component_text_params);
    // Загрузка информации об имеющихся выводах компонента.
    ComponentPinsInfoSizer->GetStaticBox()->SetLabel(wxTR(PINS_TEXT) + wxFI(loaded_component.pins_size()));
    ComponentPinsListChoice->Clear();
    for (auto current_pin_it = loaded_component.pins_begin(); current_pin_it = loaded_component.pins_end(); ++current_pin_it)
        ComponentPinsListChoice->Append(ConvertToUnicode(current_pin_it->pin_name));
    if (loaded_component.pins_size())
    {
        ComponentPinsListChoice->SetSelection(0);
        LoadCompPinDescription(component_index, 0, *loaded_component.pins_begin());
    }
    else
    {
        ComponentPinsListChoice->SetSelection(wxNOT_FOUND);
        LoadCompPinDescription(component_index, -1, PCADFile::COMPONENT_PIN_INVALID);
    }

    block_signals_ = old_block_signals;
}

void DatabaseResBrowser::LoadNewComponent(const string& component_name)
{
    if (!view_pcad_database_)
    {
        LoadNewComponent(-1);
        return;
    }

    auto radio_components_it = view_pcad_database_->radio_components_begin();
    for (; radio_components_it != view_pcad_database_->radio_components_end(); ++radio_components_it)
    {
        if (radio_components_it->GetName() == component_name)
            break;
    }
    if (radio_components_it != view_pcad_database_->radio_components_end())
        LoadNewComponent(radio_components_it - view_pcad_database_->radio_components_begin());
    else
        LoadNewComponent(-1);
}

void DatabaseResBrowser::ClearDialog()
{
    block_signals_ = true;
    // ---- Очистка и инициализация виджетов с информацией об отдельном библиотечном радиокомпоненте. -----
    ComponentsBoxSizer->GetStaticBox()->SetLabel(wxTR(COMPONENTS_TEXT) + ZERO_VALUE_STR);
    ComponentsListChoice->Clear();
    ComponentsListChoice->SetSelection(wxNOT_FOUND);
    LoadNewComponent(-1);

    // ----- Очистка и инициализация виджетов с информацией о токопроводящей цепи. -----
    NetsBoxSizer->GetStaticBox()->SetLabel(wxTR(NETS_TEXT) + ZERO_VALUE_STR);
    NetsListChoice->Clear();
    NetsListChoice->SetSelection(wxNOT_FOUND);
    LoadNewNet(-1);

    // ---- Очистка и инициализация группы виджетов с информацией о вставке экземпляра радиокомпонента. ----
    InsertionsBoxSizer->GetStaticBox()->SetLabel(wxTR(INSERTS_TEXT) + ZERO_VALUE_STR);
    InsertionsListChoice->Clear();
    InsertionsListChoice->SetSelection(wxNOT_FOUND);
    // ----------
    InsertNameText->Clear();
    InsertNameCoordsText->Clear();
    IsInsertUserNameCheck->SetValue(false);
    InsertComponentNameText->Clear();
    // ----------
    InsertCoordsText->Clear();
    IsInsertMirroring->SetValue(false);
    IsInsertOnTop->SetValue(false);
    // ----------
    InsertRotateFactorText->SetValue(ZERO_VALUE_STR);
    InsertScaleXText->SetValue(wxFSC(1.0));
    InsertScaleYText->SetValue(wxFSC(1.0));
    InsertSetAngleText->SetValue(ZERO_VALUE_STR);
    // ----------
    InsertRefDesCoordsText->Clear();
    InsertRefDesLayerText->Clear();
    InsertRefDesHeightText->SetValue(ZERO_VALUE_STR);
    InsertRefDesAlignChoice->SetSelection(0);
    InsertRefDesOrientChoice->SetSelection(0);
    // ----------
    InsertPinsInfoSizer->GetStaticBox()->SetLabel(wxTR(PINS_TEXT) + ZERO_VALUE_STR);
    InsertPinNamesListChoice->Clear();
    InsertPinNamesListChoice->SetSelection(wxNOT_FOUND);
    // ----------
    InsertPinAlNumText->Clear();
    InsertPinNameText->Clear();
    InsertPinTypeText->Clear();
    InsertPinNetNameConnectText->Clear();
    // ----------
    InsertPinLabelCoordsText->Clear();
    InsertPinLabelLayerText->Clear();
    InsertPinLabelHeightText->SetValue(ZERO_VALUE_STR);
    InsertPinLabelAlignChoice->SetSelection(0);
    InsertPinLabelOrientChoice->SetSelection(0);
    // ----------
    block_signals_ = false;
}

std::vector<DatabaseResBrowser::LoadDatabaseError> DatabaseResBrowser::LoadPCADFileData(const PCADFile* pcad_database)
{
    ClearDialog();
    view_pcad_database_ = pcad_database;
    if (!view_pcad_database_)
        return {};

    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    // Настроим "псевдоконтекст рисования", который потребуется нам для некоторых операций над коодинатами.
    view_draw_context_.pcad_doc_ptr = const_cast<PCADFile*>(view_pcad_database_);
    view_draw_context_.measure_unit_type = this_app->options_data.measure_unit_type;
    view_draw_context_.text_export_style = TextExportStyleType::TEXT_EXPORT_PRECISION_STYLE;

    block_signals_ = true;
    // Настраиваем перекодировщик в соответствии с кодировкой просматриваемого документа.
    pdif_encoding_conv_.Init(pcad_database->GetFileDefValues().pdif_encoding, wxFONTENCODING_UNICODE, wxCONVERT_SUBSTITUTE);

    // Составляем и загружаем в виджет выбора список доступных библиотечных радиокомпонентов документа.
    ComponentSectionsInfoSizer->GetStaticBox()->SetLabel(wxTR(SECTIONS_TEXT) + wxFI(view_pcad_database_->radio_components_size()));
    for (auto radio_components_it = view_pcad_database_->radio_components_begin();
         radio_components_it != view_pcad_database_->radio_components_end(); ++radio_components_it)
        ComponentsListChoice->Append(radio_components_it->GetName());
    // Формируем полный список имеющихся токопроводящих цепей.
    NetsBoxSizer->GetStaticBox()->SetLabel(wxTR(NETS_TEXT) + wxFI(view_pcad_database_->nets_size()));
    for (auto database_nets_it = view_pcad_database_->nets_begin();
         database_nets_it != view_pcad_database_->nets_end(); ++database_nets_it)
        NetsListChoice->Append(database_nets_it->GetName());
    // Наконец, занесём в соответствующий виджет список имеющихся в базе данных вставок копий радиокомпонентов из библиотеки.
    InsertionsBoxSizer->GetStaticBox()->SetLabel(wxTR(INSERTS_TEXT) + wxFI(view_pcad_database_->radio_comp_inserts_size()));
    for (auto radio_comp_inserts_it = view_pcad_database_->radio_comp_inserts_begin();
         radio_comp_inserts_it != view_pcad_database_->radio_comp_inserts_end(); ++radio_comp_inserts_it)
        InsertionsListChoice->Append(radio_comp_inserts_it->GetName());
    // Если список компонент не пуст, выбираем первый из них в качестве активного.
    if (ComponentsListChoice->GetCount() != 0)
        LoadNewComponent(0);

    block_signals_ = false;
    return {};
}

// Возврат текстового описания ошибки, если диалог завершился ненормально (с кодом, отличным от wxID_OK).
wxString DatabaseResBrowser::GetErrorMessage() const
{
    return error_message_;
}

void DatabaseResBrowser::OnComponentsListChoiceSelect(wxCommandEvent& event)
{
    if (block_signals_)
        return;
}

void DatabaseResBrowser::OnComponentPinsListChoiceSelect(wxCommandEvent& event)
{
    if (block_signals_)
        return;
}

void DatabaseResBrowser::OnNetsListChoiceSelect(wxCommandEvent& event)
{
    if (block_signals_)
        return;

}

void DatabaseResBrowser::OnNetObjectsListChoiceSelect(wxCommandEvent& event)
{
    if (block_signals_)
        return;

}

void DatabaseResBrowser::OnInsertionsListChoiceSelect(wxCommandEvent& event)
{
    if (block_signals_)
        return;

}

void DatabaseResBrowser::OnInsertPinNamesListChoiceSelect(wxCommandEvent& event)
{
    if (block_signals_)
        return;

}

void DatabaseResBrowser::OnExportAsTextButtonClick(wxCommandEvent& event)
{
    wxFileDialog export_file_dialog(this, _("Экспортировать данные PCAD-документа в текстовый файл"), {}, {},
                                    _("Текстовые файлы (*.txt)|*.txt|Любые|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (export_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    // Выполняем экспорт отображаемой в данный момент базы в текстовый формат.

}

void DatabaseResBrowser::OnExportAsXMLButtonClick(wxCommandEvent& event)
{
    wxFileDialog export_file_dialog(this, _("Экспортировать данные PCAD-документа в XML-файл"), {}, {},
                                    _("XML-файлы (*.xml)|*.xml"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (export_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    // Выполняем экспорт отображаемой в данный момент базы в XML-представление.
}

void DatabaseResBrowser::CloseDialogProc()
{
    EndModal(wxID_OK);
}

void DatabaseResBrowser::OnExitDialogButtonClick(wxCommandEvent& event)
{
    CloseDialogProc();
}

void DatabaseResBrowser::OnClose(wxCloseEvent& event)
{
    CloseDialogProc();
}

void DatabaseResBrowser::OnInit(wxInitDialogEvent& event)
{}
