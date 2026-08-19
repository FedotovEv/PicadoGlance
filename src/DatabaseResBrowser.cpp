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

DatabaseResBrowser::DatabaseResBrowser(wxWindow* parent,wxWindowID id)
{
    BuildContent(parent,id);
}

void DatabaseResBrowser::BuildContent(wxWindow* parent,wxWindowID id)
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
    wxStaticBoxSizer* ComponentsBoxSizer;
    wxStaticBoxSizer* InsertPinLabelPropertiesSizer;
    wxStaticBoxSizer* InsertPinLabelTextParamsSizer;
    wxStaticBoxSizer* InsertPropertiesSizer;
    wxStaticBoxSizer* InsertRefDesSizer;
    wxStaticBoxSizer* InsertRefDesTextParamsSizer;
    wxStaticBoxSizer* InsertionsBoxSizer;
    wxStaticBoxSizer* NetsBoxSizer;
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
    ComponentsBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Компоненты"));
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
    ComponentOrgCoordsTitle = new wxStaticText(ComponentPropertiesSizer->GetStaticBox(), wxID_ANY, _("Координаты"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
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
    NetsBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Цепи"));
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
    InsertionsBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Вставки"));
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

    MainDialogSizer->SetSizeHints(this);
}

DatabaseResBrowser::~DatabaseResBrowser()
{
    //(*Destroy(DatabaseResBrowser)
    //*)
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

std::vector<DatabaseResBrowser::LoadDatabaseError> DatabaseResBrowser::LoadPCADFileData(const PCADFile* pcad_database)
{
    view_pcad_database_ = pcad_database;

    return {};
}

// Возврат текстового описания ошибки, если диалог завершился ненормально (с кодом, отличным от wxID_OK).
wxString DatabaseResBrowser::GetErrorMessage() const
{
    return error_message_;
}

void DatabaseResBrowser::OnComponentsListChoiceSelect(wxCommandEvent& event)
{
}

void DatabaseResBrowser::OnComponentPinsListChoiceSelect(wxCommandEvent& event)
{
}

void DatabaseResBrowser::OnNetsListChoiceSelect(wxCommandEvent& event)
{
}

void DatabaseResBrowser::OnNetObjectsListChoiceSelect(wxCommandEvent& event)
{
}

void DatabaseResBrowser::OnInsertionsListChoiceSelect(wxCommandEvent& event)
{
}

void DatabaseResBrowser::OnInsertPinNamesListChoiceSelect(wxCommandEvent& event)
{
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
