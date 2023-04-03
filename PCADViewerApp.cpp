/***************************************************************
 * Name:      PCADViewerApp.cpp
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-01
 * Copyright: Dichrograph ()
 * License:   GPL-3.0-or-later
 **************************************************************/

#include "wx_pch.h"
#include "PCADFile.h"
#include "HandlerPLT.h"
#include "HandlerPLT8.h"
#include "gerber.h"
#include "PCADViewerApp.h"
#include "PCADViewDraw.h"
#include "ProviderStamp.h"

//(*AppHeaders
#include "PCADViewerMain.h"
#include <wx/image.h>
#include <wx/cshelp.h>
#include <wx/html/helpctrl.h>
#include <wx/cmdline.h>
#include <wx/print.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/xrc/xmlres.h>

#include "wx/filesys.h"
#include "wx/fs_zip.h"
//*)

#include <wx/xrc/xh_bmp.h>

extern const StdWXObjects std_wx_objects;

using namespace std;
using namespace std::filesystem;

namespace
{
    wxString GetLocalizedDir()
    {
        wxFileName localized_dir;
        // Получаем местонахождение (маршрут) каталогов с языковыми ресурсами(переводами)
        const wxLanguageInfo* default_lang_ptr = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
        if (default_lang_ptr)
        {
            localized_dir = wxFileName::DirName(wxStandardPaths::Get().
                GetLocalizedResourcesDir(default_lang_ptr->CanonicalName, wxStandardPaths::ResourceCat_Messages));

            wxArrayString localized_part = localized_dir.GetDirs();
            size_t i = localized_part.GetCount();
            for (; i > 0; --i)
                #ifdef __APPLE__
                    if (localized_part[i - 1] == default_lang_ptr->CanonicalName + wxT(".lproj"))
                        break;
                #else
                    if (localized_part[i - 1] == default_lang_ptr->CanonicalName)
                        break;
                #endif

            if (i > 0)
            {
                wxString result;
                if (localized_dir.HasVolume())
                    result += localized_dir.GetVolume() + wxChar(':') + wxFileName::GetPathSeparator();
                for (size_t j = 0; j < i - 1; ++j)
                    result += localized_part[j] + wxFileName::GetPathSeparator();

                return result;
            }
            else
            {
                return wxString();
            }
        }
        else
        {
            localized_dir = wxFileName::DirName(wxStandardPaths::Get().GetResourcesDir());
            return localized_dir.GetFullPath();
        }
    }
} //namespace

bool MyPrintout::OnPrintPage(int page_num)
{
    if (page_num != 1)
        return false;
    GraphExportContext export_context = export_context_;
    export_context.draw_dc_ptr = GetDC();
    viewer_frame_ptr_->DoGraphExporting(export_context);
    return true;
}

wxCmdLineEntryDesc const static cmd_line_desc[] =
{
    {wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("вывод помощи по формату командной строки программы"),
     wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
    {wxCMD_LINE_OPTION, wxT("a"), wxT("aperture-name"), wxT("имя файла апертур")},
    {wxCMD_LINE_SWITCH, wxT("l"), wxT("aperture-laser"), wxT("использовать множество апертур GERBER LASER")},
    {wxCMD_LINE_SWITCH, wxT("t"), wxT("text-engine"), wxT("использовать собственный движок рисования текста")},
    {wxCMD_LINE_PARAM, NULL, NULL, wxT("входной файл"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_NONE}
};

//IMPLEMENT_APP(PCADViewerApp);
IMPLEMENT_APP_NO_MAIN(PCADViewerApp);
IMPLEMENT_WX_THEME_SUPPORT;

int main(int argc, char *argv[])
{
    wxEntryStart(argc, argv);
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);
    this_app->CallOnInit();

    while (true)
    {
        this_app->OnRun();
        if (this_app->return_code < 0)
            this_app->RecreateGUI();
        else
            break;
    }

    return this_app->return_code;
}

void PCADViewerApp::GetInstalledLanguages()
{
    const wxLanguageInfo* langinfo;
    LanguageDescriptType lang_desc;
    std::error_code ec;
    // Получаем местонахождение (маршрут) каталогов с языковыми ресурсами(переводами)
    wxString localized_dir = GetLocalizedDir();

    languages_descs.clear();
    // Добавляем в список русский язык. Это язык по умолчанию, он существует всегда
    lang_desc.language_identifier = wxLANGUAGE_RUSSIAN;
    lang_desc.language_name = wxT("Русский");
    languages_descs.push_back(lang_desc);
    if (!localized_dir.size())
        return;

    // В последующем цикле перебираем все существующие подкаталоги, находящиеся в каталоге localized_dir.
    // Если их имя совпадает с одним из стандартных имён локалей (корректно принимаемых функцией
    // wxLocale::FindLanguageInfo), то такой каталог проверяется на наличие данных перевода (файла .mo)
    // для очередного поддерживаемого языка. Если такой файл есть, то найденный язык добавляется в список
    // дескрипторов languages_descs.
    string mo_filename = string((wxFileName(GetAppName()).GetName() + wxT(".mo")).mb_str());
    auto dir_it = directory_iterator(string(localized_dir.mb_str()), ec);
    if (ec)
        return;
    for(const auto& current_dir_entry : dir_it)
    {
        if (current_dir_entry.is_directory())
        {
            string dirname = current_dir_entry.path().stem().string();
            langinfo = wxLocale::FindLanguageInfo(wxString(dirname.c_str(), wxConvUTF8));
            if(langinfo)
            {
                path mo_file_path(current_dir_entry.path() / path(mo_filename));
                bool is_mo_file_exist = exists(mo_file_path, ec);
                #if defined(__unix__) || defined(__linux__)
                    mo_file_path = current_dir_entry.path() / path("LC_MESSAGES") / path(mo_filename);
                    is_mo_file_exist = exists(mo_file_path, ec);
                #endif // __unix__ или __linux__
                if (is_mo_file_exist)
                {
                    lang_desc.language_identifier = static_cast<wxLanguage>(langinfo->Language);
                    lang_desc.language_name = langinfo->CanonicalName;
                    lang_desc.language_path = current_dir_entry.path();
                    lang_desc.mo_file_path = mo_file_path;
                    languages_descs.push_back(lang_desc);
                }
            }
        }
    }
}

void PCADViewerApp::RecreateGUI()
{
    PCADViewerFrame* Frame = new PCADViewerFrame(0);
    Frame->Centre();
    Frame->Show();
    SetTopWindow(Frame);
    setlocale(LC_NUMERIC, "C");
}

void PCADViewerApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc(cmd_line_desc);
    // Используем для предварения параметров символ '-', чтобы не путать его с маршрутами файлов
    parser.SetSwitchChars(wxT("-"));
}

bool PCADViewerApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!parser.Found(wxT("a"), &options_data.aperture_filename))
        options_data.aperture_filename.clear();

    options_data.is_aperture_gerber_laser = parser.Found(wxT("l"));
    options_data.is_use_chr_text_engine = parser.Found(wxT("t"));

    if (parser.GetParamCount() > 0)
        options_data.picture_filename = parser.GetParam(0);
    else
        options_data.picture_filename.clear();

    return true;
}

bool PCADViewerApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    options_data.is_use_chr_text_engine = true;
    // Настроим указатели на данные конфигурации, которые могут использоваться графическими
    // примитивами при своём конструировании и/или рисовании.
    GraphObjGlobalConfig set_graph_glob_conf;
    set_graph_glob_conf.options_data_ptr = &options_data;
    set_graph_glob_conf.font_data_ptr = &font_data;
    set_graph_glob_conf.chr_proc_ptr = &chr_processor;
    GraphObj::SetGlobalConfigPtrs(set_graph_glob_conf);

    executable_path = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
    resource_filename = executable_path + wxT("\\PCADViewer.xrs");
    //Загружаем файл с ресурсами программы (пока там только иконки кнопок инструментария)
    wxFileSystem::AddHandler(new wxZipFSHandler);
    //wxXmlResource::Get()->InitAllHandlers();
    wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler); // Других ресурсов, кроме растров, у нас пока нет
    wxXmlResource::Get()->AddHandler(new chr::CHRFontXmlHandler(resource_filename));
    wxXmlResource::Get()->Load(resource_filename);

    wxObject* chr_font_obj_ptr =
        wxXmlResource::Get()->LoadObject(nullptr, wxT("default_chr_font"), wxT("fontCHR"));
    if (chr_font_obj_ptr)
    {
        chr_processor.LoadFont(static_cast<wxFSFile*>(chr_font_obj_ptr));
        delete chr_font_obj_ptr;
    }
    StampProvider::InitStampProvider();
    GetInstalledLanguages();

    file_factory.RegisterFileWorkshop(new HandlerPLT::PLT4FileWorkshop);
    file_factory.RegisterFileWorkshop(new HandlerPLT8::PLT8FileWorkshop);
    file_factory.RegisterFileWorkshop(new HandlerGERBER::GERBERFileWorkshop);
    file_factory.RegisterFileWorkshop(new HandlerGERBER::EXCELLONFileWorkshop);
    pcad_file = PCADFile(aperture_provider);
    // Инициализируем состояние форматного холста-подложки
    canvas_context.is_canvas = false;
    canvas_context.canvas_size_code = CanvasSizeCode::CANVAS_SIZE_A4;
    canvas_context.canvas_size = std_wx_objects.GetStandartCanvasSize(canvas_context.canvas_size_code, true);
    canvas_context.canvas_position = wxPoint(0, 0);
    canvas_context.canvas_color = *wxWHITE;

    //Попробуем сразу установить язык, совпадающий с языком по умолчанию операционной системы
    if (const wxLanguageInfo* default_lang_ptr = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT))
    {
        for (const LanguageDescriptType& lang_desc : languages_descs)
        {
            if (lang_desc.language_identifier == default_lang_ptr->Language)
            {
                m_locale = new wxLocale(lang_desc.language_identifier);
                break;
            }
        }
    }
    if (!m_locale)
        m_locale = new wxLocale(wxLANGUAGE_RUSSIAN);

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if (wxsOK)
    {
    	PCADViewerFrame* Frame = new PCADViewerFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)

    wxHelpControllerHelpProvider* provider = new wxHelpControllerHelpProvider;
    wxHelpProvider::Set(provider);
    provider->SetHelpController(&HtmlHelp);
    HtmlHelp.AddBook(wxFileName(executable_path + wxT("\\PCADViewerHelp.zip")));
    setlocale(LC_NUMERIC, "C");

    return wxsOK;
}
