#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(FileInfoDialog)
	#include <wx/button.h>
	#include <wx/dialog.h>
	#include <wx/listbox.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	//*)
#endif
//(*Headers(FileInfoDialog)
//*)

class FileInfoDialog: public wxDialog
{
	public:

		FileInfoDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~FileInfoDialog();

		//(*Declarations(FileInfoDialog)
		wxButton* FileInfoButtonExit;
		wxButton* FileInfoButtonSave;
		wxListBox* FlashList;
		wxListBox* LayerList;
		wxStaticText* ApertureFileNameStatic;
		wxStaticText* DBUInMeasureUnitStatic;
		wxStaticText* FileEditorStatic;
		wxStaticText* FileMeasureUnitStatic;
		wxStaticText* FileNameStatic;
		wxStaticText* FileTypeStatic;
		wxStaticText* FileWorkshopStatic;
		wxStaticText* FlashStatic;
		wxStaticText* LayerStatic;
		wxStaticText* PictElemCountStatic;
		wxStaticText* PictSizeStatic;
		wxTextCtrl* ApertureFileNameText;
		wxTextCtrl* DBUInMeasureUnitText;
		wxTextCtrl* FileEditorText;
		wxTextCtrl* FileMeasureUnitText;
		wxTextCtrl* FileNameText;
		wxTextCtrl* FileTypeText;
		wxTextCtrl* FileWorkshopText;
		wxTextCtrl* PictElemCountText;
		wxTextCtrl* PictSizeText;
		//*)

	protected:

		//(*Identifiers(FileInfoDialog)
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL3;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT6;
		static const long ID_TEXTCTRL5;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT8;
		static const long ID_TEXTCTRL6;
		static const long ID_STATICTEXT9;
		static const long ID_TEXTCTRL7;
		static const long ID_TEXTCTRL8;
		static const long ID_LISTBOX1;
		static const long ID_LISTBOX2;
		static const long ID_STATICTEXT10;
		static const long ID_STATICTEXT11;
		static const long ID_STATICTEXT12;
		static const long ID_TEXTCTRL9;
		//*)

	private:

		//(*Handlers(FileInfoDialog)
		void OnFileInfoButtonExitClick(wxCommandEvent& event);
		void OnFileInfoButtonSaveClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
