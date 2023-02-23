#ifndef LAYERSSETTINGS_H
#define LAYERSSETTINGS_H

#include <vector>
#include "scrollingdialog.h"
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include "PCADFile.h"

class LayersSettings: public wxScrollingDialog
{
	public:

		LayersSettings(wxWindow* parent, wxWindowID id=wxID_ANY, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);
		virtual ~LayersSettings();

	private:

        struct LayerRecordEditBlock
        {
            decltype(std::declval<PCADFile>().layers_begin()) layer_it;
            // Идентификаторы элементов управления для одного, данного слоя
            long id_layer_number_text;
            long id_layer_name_text;
            long id_colour_picker_ctrl;
            long id_on_checkbox;
            // Указатели на элементы управления для данного слоя
            wxSizer* layer_box_sizer;
            wxStaticText* layer_number_text;
            wxStaticText* layer_name_text;
            wxColourPickerCtrl* colour_picker_ctrl;
            wxCheckBox* on_checkbox;
        };

        wxSizer* MainBoxSizer;
        wxButton* ButtonExit;
        wxButton* ButtonCancel;
        static const long ID_BUTTON_EXIT;
        static const long ID_BUTTON_CANCEL;

        std::vector<LayerRecordEditBlock> layer_contols;

        void OnLayersSettingsButtonExitClick(wxCommandEvent& event);
        void OnLayersSettingsButtonCancelClick(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
