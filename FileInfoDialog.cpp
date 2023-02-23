#include <fstream>

#include "wx_pch.h"
#include "FileInfoDialog.h"
#include "PCADViewerApp.h"
#include "PCADFile.h"
#include "ApertureFile.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(FileInfoDialog)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(FileInfoDialog)
//*)

using namespace std;
using namespace aperture;

namespace
{
    string DelNewLine(const wxString& str)
    {
        string result(str.mb_str());
        for (size_t i = 0; i < result.size(); ++i)
            if (result[i] == '\n')
                result[i] = ' ';
        return result;
    }
}

//(*IdInit(FileInfoDialog)
const long FileInfoDialog::ID_BUTTON1 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT1 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL1 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT3 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL2 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT4 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL3 = wxNewId();
const long FileInfoDialog::ID_BUTTON2 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT5 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL4 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT6 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL5 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT7 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT8 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL6 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT9 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL7 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL8 = wxNewId();
const long FileInfoDialog::ID_LISTBOX1 = wxNewId();
const long FileInfoDialog::ID_LISTBOX2 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT10 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT11 = wxNewId();
const long FileInfoDialog::ID_STATICTEXT12 = wxNewId();
const long FileInfoDialog::ID_TEXTCTRL9 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FileInfoDialog,wxDialog)
	//(*EventTable(FileInfoDialog)
	//*)
END_EVENT_TABLE()

FileInfoDialog::FileInfoDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(FileInfoDialog)
	Create(parent, id, _("Информация о файле"), wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP|wxDEFAULT_DIALOG_STYLE|wxBORDER_STATIC, _T("id"));
	SetClientSize(wxSize(400,378));
	FileInfoButtonExit = new wxButton(this, ID_BUTTON1, _("Выход"), wxPoint(256,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FileNameStatic = new wxStaticText(this, ID_STATICTEXT1, _("Имя файла"), wxPoint(8,16), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FileNameText = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(80,8), wxSize(312,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FileTypeStatic = new wxStaticText(this, ID_STATICTEXT3, _("Тип файла"), wxPoint(8,48), wxSize(56,14), 0, _T("ID_STATICTEXT3"));
	FileTypeText = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxPoint(80,40), wxSize(110,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FileWorkshopStatic = new wxStaticText(this, ID_STATICTEXT4, _("Загрузчик"), wxPoint(208,48), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FileWorkshopText = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxPoint(272,40), wxSize(120,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	FileInfoButtonSave = new wxButton(this, ID_BUTTON2, _("Сохранить в файл"), wxPoint(72,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	PictSizeStatic = new wxStaticText(this, ID_STATICTEXT5, _("Размер"), wxPoint(16,80), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	PictSizeText = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxPoint(80,72), wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	PictElemCountStatic = new wxStaticText(this, ID_STATICTEXT6, _("Количество\nэлементов"), wxPoint(208,72), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	PictElemCountText = new wxTextCtrl(this, ID_TEXTCTRL5, _("0"), wxPoint(280,72), wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	FileEditorStatic = new wxStaticText(this, ID_STATICTEXT7, _("Редактор"), wxPoint(16,112), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FileMeasureUnitStatic = new wxStaticText(this, ID_STATICTEXT8, _("Единицы\nизмерения"), wxPoint(160,104), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FileEditorText = new wxTextCtrl(this, ID_TEXTCTRL6, wxEmptyString, wxPoint(80,104), wxSize(72,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	DBUInMeasureUnitStatic = new wxStaticText(this, ID_STATICTEXT9, _("Плотность"), wxPoint(280,112), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FileMeasureUnitText = new wxTextCtrl(this, ID_TEXTCTRL7, wxEmptyString, wxPoint(224,104), wxSize(48,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	DBUInMeasureUnitText = new wxTextCtrl(this, ID_TEXTCTRL8, wxEmptyString, wxPoint(336,104), wxSize(56,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	LayerList = new wxListBox(this, ID_LISTBOX1, wxPoint(80,136), wxSize(312,80), 0, 0, wxLB_SINGLE|wxLB_ALWAYS_SB, wxDefaultValidator, _T("ID_LISTBOX1"));
	FlashList = new wxListBox(this, ID_LISTBOX2, wxPoint(80,256), wxSize(312,80), 0, 0, wxLB_SINGLE|wxLB_ALWAYS_SB, wxDefaultValidator, _T("ID_LISTBOX2"));
	LayerStatic = new wxStaticText(this, ID_STATICTEXT10, _("Слои"), wxPoint(24,168), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlashStatic = new wxStaticText(this, ID_STATICTEXT11, _("Апертуры:"), wxPoint(8,288), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	ApertureFileNameStatic = new wxStaticText(this, ID_STATICTEXT12, _("Имя файла\nапертур"), wxPoint(8,224), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	ApertureFileNameText = new wxTextCtrl(this, ID_TEXTCTRL9, wxEmptyString, wxPoint(80,224), wxSize(312,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL9"));

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FileInfoDialog::OnFileInfoButtonExitClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FileInfoDialog::OnFileInfoButtonSaveClick);
	//*)
}

FileInfoDialog::~FileInfoDialog()
{
	//(*Destroy(FileInfoDialog)
	//*)
}

void FileInfoDialog::OnFileInfoButtonExitClick(wxCommandEvent& event)
{
    EndModal(0);
}

void FileInfoDialog::OnFileInfoButtonSaveClick(wxCommandEvent& event)
{
    wxFileDialog save_file_dialog(this, _("Сохранить отчёт о загруженном рисунке в файл"), _(""), _(""),
                    _("Текстовые файлы (*.txt)|*.txt"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    ofstream out_file(string(save_file_dialog.GetPath().mb_str()));
    if (!out_file)
    {
        wxMessageBox(_("Ошибка при создании выходного файла"), _("Ошибка"));
        return;
    }
    const string dblpnt = " : ";
    out_file << DelNewLine(FileNameStatic->GetLabelText()) << dblpnt << FileNameText->GetValue().mb_str() << endl;
    out_file << DelNewLine(FileTypeStatic->GetLabelText()) << dblpnt << FileTypeText->GetValue().mb_str() << endl;
    out_file << DelNewLine(FileWorkshopStatic->GetLabelText()) << dblpnt << FileWorkshopText->GetValue().mb_str() << endl;
    out_file << DelNewLine(FileEditorStatic->GetLabelText()) << dblpnt << FileEditorText->GetValue().mb_str() << endl;
    out_file << DelNewLine(FileMeasureUnitStatic->GetLabelText()) << dblpnt << FileMeasureUnitText->GetValue().mb_str() << endl;
    out_file << DelNewLine(DBUInMeasureUnitStatic->GetLabelText()) << dblpnt << DBUInMeasureUnitText->GetValue().mb_str() << endl;
    out_file << DelNewLine(PictSizeStatic->GetLabelText()) << dblpnt << PictSizeText->GetValue().mb_str() << endl;
    out_file << DelNewLine(PictElemCountStatic->GetLabelText()) << dblpnt << PictElemCountText->GetValue().mb_str() << endl;

    out_file << DelNewLine(LayerStatic->GetLabelText()) << endl;
    for (unsigned int i = 0; i < LayerList->GetCount(); ++i)
        out_file << LayerList->GetString(i).mb_str() << endl;

    out_file << DelNewLine(ApertureFileNameStatic->GetLabelText()) << dblpnt << ApertureFileNameText->GetValue().mb_str() << endl;

    out_file << DelNewLine(FlashStatic->GetLabelText()) << endl;
    for (unsigned int i = 0; i < FlashList->GetCount(); ++i)
        out_file << FlashList->GetString(i).mb_str() << endl;
}
