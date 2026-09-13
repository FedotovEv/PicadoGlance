#pragma once
#include "macroses.h"
#include "ApertureProvider.h"
#ifndef WX_PRECOMP
	//(*HeadersPCH(OptionsDialog)
	#include <wx/button.h>
	#include <wx/checkbox.h>
	#include <wx/choice.h>
	#include <wx/combobox.h>
	#include <wx/dialog.h>
	#include <wx/listbox.h>
	#include <wx/radiobut.h>
	#include <wx/sizer.h>
	#include <wx/statbox.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	//*)
#endif
//(*Headers(OptionsDialog)
#include <wx/spinctrl.h>
//*)

class OptionsDialog: public wxDialog
{
	public:

		OptionsDialog(wxWindow* parent, wxWindowID id = wxID_ANY);
		virtual ~OptionsDialog();

		//(*Declarations(OptionsDialog)
		wxButton* ButtonApertureDefault;
		wxButton* ButtonLoadAperture;
		wxButton* ButtonOptionCancel;
		wxButton* ButtonOptionOk;
		wxCheckBox* CheckBoxOwnTextEngine;
		wxCheckBox* CheckBoxScreenSubscale;
		wxCheckBox* DrillGenIsSortCheck;
		wxChoice* PDIFEncodingChoice;
		wxComboBox* ComboBoxApertureSection;
		wxListBox* ListBoxApertures;
		wxRadioButton* RadioAdditionalUnitScaleAny;
		wxRadioButton* RadioAdditionalUnitScaleOff;
		wxRadioButton* RadioAdditionalUnitScaleRussianInch;
		wxRadioButton* RadioUseDBU;
		wxRadioButton* RadioUseInch;
		wxRadioButton* RadioUseMillimeter;
		wxSpinCtrl* DrillGenClusterSizeSpin;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxTextCtrl* TextAdditionalUnitScaleXValue;
		wxTextCtrl* TextAdditionalUnitScaleYValue;
		wxTextCtrl* TextApertureFilename;
		//*)

        aperture::ApertureProvider temp_aperture_provider;

	protected:

		//(*Identifiers(OptionsDialog)
		static const wxWindowID ID_STATICTEXT2;
		static const wxWindowID ID_TEXTCTRL_APERTURE_FILENAME;
		static const wxWindowID ID_BUTTON_LOAD_APERTURE;
		static const wxWindowID ID_BUTTONAPERTURE_DEFAULT;
		static const wxWindowID ID_STATICTEXT3;
		static const wxWindowID ID_COMBOBOX_APERTURE_SECTION;
		static const wxWindowID ID_LISTBOXAPERTURES_LIST;
		static const wxWindowID ID_RADIOBUTTON_USE_DBU;
		static const wxWindowID ID_RADIOBUTTON_USE_MILLIMETER;
		static const wxWindowID ID_RADIOBUTTON_USE_INCH;
		static const wxWindowID ID_RADIOBUTTON_ADDIT_SCALE_OFF;
		static const wxWindowID ID_RADIOBUTTON_ADDIT_SCALE_RUSSIAN_INCH;
		static const wxWindowID ID_RADIOBUTTON_ADDIT_SCALE_ANY;
		static const wxWindowID ID_STATICTEXT4;
		static const wxWindowID ID_TEXTCTRL_UNIT_SCALE_VALUE_X;
		static const wxWindowID ID_STATICTEXT5;
		static const wxWindowID ID_TEXTCTRL_UNIT_SCALE_VALUE_Y;
		static const wxWindowID ID_CHECKBOX_SCREEN_SUBSCALE;
		static const wxWindowID ID_CHECKBOX_OWN_TEXT_ENGINE;
		static const wxWindowID ID_CHOICE_PDIF_ENCODING;
		static const wxWindowID ID_CHECKBOX_DRILL_GEN_IS_SORT;
		static const wxWindowID ID_SPIN_DRILL_GEN_CLUSTER_SIZE;
		static const wxWindowID ID_BUTTON_OPTION_OK;
		static const wxWindowID ID_BUTTON_OPTION_CANCEL;
		//*)

	private:

		//(*Handlers(OptionsDialog)
		void OnButtonOptionOkClick(wxCommandEvent& event);
		void OnButtonOptionCancelClick(wxCommandEvent& event);
		void OnRadioAdditionalUnitScaleOffSelect(wxCommandEvent& event);
		void OnRadioAdditionalUnitScaleRussianInchSelect(wxCommandEvent& event);
		void OnRadioAdditionalUnitScaleAnySelect(wxCommandEvent& event);
		void OnButtonLoadApertureClick(wxCommandEvent& event);
		void OnButtonApertureDefaultClick(wxCommandEvent& event);
		void OnComboBoxApertureSectionSelected(wxCommandEvent& event);
		void OnInit(wxInitDialogEvent& event);
		//*)

        void UpdateApertureList();

		DECLARE_EVENT_TABLE()
};
