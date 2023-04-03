#pragma once
#include "macroses.h"
#include "ApertureProvider.h"
#ifndef WX_PRECOMP
	//(*HeadersPCH(OptionsDialog)
	#include <wx/button.h>
	#include <wx/checkbox.h>
	#include <wx/combobox.h>
	#include <wx/dialog.h>
	#include <wx/listbox.h>
	#include <wx/radiobut.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	//*)
#endif
//(*Headers(OptionsDialog)
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
		wxComboBox* ComboBoxApertureSection;
		wxListBox* ListBoxApertures;
		wxRadioButton* RadioAdditionalUnitScaleAny;
		wxRadioButton* RadioAdditionalUnitScaleOff;
		wxRadioButton* RadioAdditionalUnitScaleRussianInch;
		wxRadioButton* RadioUseDBU;
		wxRadioButton* RadioUseInch;
		wxRadioButton* RadioUseMillimeter;
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
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL_APERTURE_FILENAME;
		static const long ID_BUTTON_LOAD_APERTURE;
		static const long ID_BUTTONAPERTURE_DEFAULT;
		static const long ID_STATICTEXT3;
		static const long ID_COMBOBOX_APERTURE_SECTION;
		static const long ID_LISTBOXAPERTURES_LIST;
		static const long ID_RADIOBUTTON_USE_DBU;
		static const long ID_RADIOBUTTON_USE_MILLIMETER;
		static const long ID_RADIOBUTTON_USE_INCH;
		static const long ID_RADIOBUTTON_ADDIT_SCALE_OFF;
		static const long ID_RADIOBUTTON_ADDIT_SCALE_RUSSIAN_INCH;
		static const long ID_RADIOBUTTON_ADDIT_SCALE_ANY;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL_UNIT_SCALE_VALUE_X;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL_UNIT_SCALE_VALUE_Y;
		static const long ID_CHECKBOX_SCREEN_SUBSCALE;
		static const long ID_CHECKBOX_OWN_TEXT_ENGINE;
		static const long ID_BUTTON_OPTION_OK;
		static const long ID_BUTTON_OPTION_CANCEL;
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
