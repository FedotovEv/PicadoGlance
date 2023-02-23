#ifndef SETCANVASSIZE_H
#define SETCANVASSIZE_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(SetCanvasSize)
	#include <wx/button.h>
	#include <wx/checkbox.h>
	#include <wx/dialog.h>
	#include <wx/radiobut.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	//*)
#endif
//(*Headers(SetCanvasSize)
#include <wx/spinctrl.h>
//*)

#include "PCADViewDraw.h"

class SetCanvasSize: public wxDialog
{
	public:

		SetCanvasSize(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SetCanvasSize();

		//(*Declarations(SetCanvasSize)
		wxButton* ButtonCancel;
		wxButton* ButtonReset;
		wxButton* ButtonSet;
		wxButton* ButtonSetStandart;
		wxCheckBox* FixedAspectRatio;
		wxRadioButton* MeasureUnitInch;
		wxRadioButton* MeasureUnitMM;
		wxSpinCtrl* HorSize;
		wxSpinCtrl* VertSize;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxTextCtrl* AspectRatio;
		//*)

	protected:

		//(*Identifiers(SetCanvasSize)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL_HOR_SIZE;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL_VERT_SIZE;
		static const long ID_STATICTEXT4;
		static const long ID_RADIOBUTTON_MM;
		static const long ID_RADIOBUTTON_INCH;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL_ASPECT_RATIO;
		static const long ID_CHECKBOX_FIXED_ASPECT_RATIO;
		static const long ID_BUTTON_SET;
		static const long ID_BUTTON_CANCEL;
		static const long ID_BUTTON_RESET;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(SetCanvasSize)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonResetClick(wxCommandEvent& event);
		void OnButtonSetClick(wxCommandEvent& event);
		void OnHorSizeChange(wxSpinEvent& event);
		void OnVertSizeChange(wxSpinEvent& event);
		void OnAspectRatioTextEnter(wxCommandEvent& event);
		void OnFixedAspectRatioClick(wxCommandEvent& event);
		void OnMeasureUnitMMSelect(wxCommandEvent& event);
		void OnMeasureUnitInchSelect(wxCommandEvent& event);
		void OnButtonSetStandartClick(wxCommandEvent& event);
		//*)

		static constexpr double CANVAS_ASPECT_MIN = 0.01;
		static constexpr double CANVAS_ASPECT_MAX = 100;

        void CorrectAspectRatioValue();
        void CorrectAspectRatioAndSetSizes();
        void SetAspectByCurrentSizes();
        void SetSizesByCurrentAspect();
        void ResetData();

        void OnAspectRatioKillFocus(wxFocusEvent& event);
        void OnMenuItemSetStdSizeSelected(wxCommandEvent& event);

        static const long IdMenuCanvasA3;
        static const long IdMenuCanvasA4;
        static const long IdMenuCanvasA5;
        static const long IdMenuCanvasA6;
        static const long IdMenuCanvasA7;
        static const long IdMenuCanvasA8;
        static const long IdMenuCanvasCurrent;

        wxMenu* MenuStdCanvasFormat;
        wxMenuItem* MenuItemCanvasA3;
        wxMenuItem* MenuItemCanvasA4;
        wxMenuItem* MenuItemCanvasA5;
        wxMenuItem* MenuItemCanvasA6;
        wxMenuItem* MenuItemCanvasA7;
        wxMenuItem* MenuItemCanvasA8;
        wxMenuItem* MenuItemCanvasCurrent;

		DECLARE_EVENT_TABLE()
};

#endif
