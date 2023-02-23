
#include <string>
#include <vector>

#include "wx_pch.h"
#include "LayersSettings.h"
#include "PCADViewerApp.h"

#ifndef WX_PRECOMP
	#include <wx/intl.h>
	#include <wx/string.h>
#endif

using namespace std;

extern const StdWXObjects std_wx_objects;

const long LayersSettings::ID_BUTTON_EXIT = wxNewId();
const long LayersSettings::ID_BUTTON_CANCEL = wxNewId();

BEGIN_EVENT_TABLE(LayersSettings,wxScrollingDialog)
END_EVENT_TABLE()

LayersSettings::LayersSettings(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

	Create(parent, id, _("Информация о слоях рисунка"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	MainBoxSizer = new wxBoxSizer(wxVERTICAL);

    for (auto layer_it = this_app->pcad_file.layers_begin();
         layer_it != this_app->pcad_file.layers_end(); ++layer_it)
    {
        LayerRecordEditBlock lreb;
        lreb.layer_it = layer_it;
        lreb.id_layer_number_text = wxNewId();
        lreb.id_layer_name_text = wxNewId();
        lreb.id_colour_picker_ctrl = wxNewId();
        lreb.id_on_checkbox = wxNewId();
        // Отдельный разместитель для каждого слоя
        lreb.layer_box_sizer = new wxBoxSizer(wxHORIZONTAL);
        // Создаём статическое поле для номера слоя
        wxString temp_string(to_string(layer_it->layer_number).c_str(), wxConvUTF8);
        lreb.layer_number_text = new wxStaticText(this, lreb.id_layer_number_text, temp_string);
        // Создаём статическое поле для его имени
        temp_string = wxString(layer_it->layer_name.c_str(), wxConvUTF8);
        lreb.layer_name_text = new wxStaticText(this, lreb.id_layer_name_text, temp_string);
        // Создаём управленца для выбора цвета слоя
        lreb.colour_picker_ctrl = new wxColourPickerCtrl(this, lreb.id_colour_picker_ctrl, layer_it->layer_wx_color);
        // Крыжик для управления включением/выключением слоя
        lreb.on_checkbox = new wxCheckBox(this, lreb.id_on_checkbox, _("Включён"));
        // Добавляем в разместитель всех созданных управленцев
        lreb.layer_box_sizer->Add(lreb.layer_number_text, 1, wxALL|wxEXPAND, 5);
        lreb.layer_box_sizer->Add(lreb.layer_name_text, 2, wxALL|wxEXPAND, 5);
        lreb.layer_box_sizer->Add(lreb.colour_picker_ctrl, 1, wxALL|wxEXPAND, 5);
        lreb.layer_box_sizer->Add(lreb.on_checkbox, 0, wxALL|wxEXPAND, 5);
        MainBoxSizer->Add(lreb.layer_box_sizer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
        if (layer_it->layer_attributes == LayerAttributes{LAYER_ON_AVL_ACT_OFF})
            lreb.on_checkbox->SetValue(false);
        else
            lreb.on_checkbox->SetValue(true);
        layer_contols.push_back(lreb);
    }
	ButtonExit = new wxButton(this, ID_BUTTON_EXIT, _("Принять"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_EXIT"));
	ButtonCancel = new wxButton(this, ID_BUTTON_CANCEL, _("Отменить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANCEL"));
    wxSizer* ButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    ButtonSizer->Add(ButtonExit, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonSizer->Add(ButtonCancel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	MainBoxSizer->Add(ButtonSizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(MainBoxSizer);
	MainBoxSizer->Fit(this);
	MainBoxSizer->SetSizeHints(this);

	Connect(ID_BUTTON_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&LayersSettings::OnLayersSettingsButtonExitClick);
	Connect(ID_BUTTON_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&LayersSettings::OnLayersSettingsButtonCancelClick);
}

LayersSettings::~LayersSettings()
{}

void LayersSettings::OnLayersSettingsButtonExitClick(wxCommandEvent& event)
{
    for (LayerRecordEditBlock& lreb : layer_contols)
    {
        const_cast<wxColor&>(lreb.layer_it->layer_wx_color) = lreb.colour_picker_ctrl->GetColour();
        if (lreb.on_checkbox->IsChecked())
            lreb.layer_it->layer_attributes = LayerAttributes{LAYER_ON_AVL_ACT_ON};
        else
            lreb.layer_it->layer_attributes = LayerAttributes{LAYER_ON_AVL_ACT_OFF};
    }
    EndModal(1);
}

void LayersSettings::OnLayersSettingsButtonCancelClick(wxCommandEvent& event)
{
    EndModal(0);
}
