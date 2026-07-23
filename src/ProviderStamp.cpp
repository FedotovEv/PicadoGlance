
#include <string>
#include <vector>
#include <algorithm>
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/xrc/xmlres.h>

#include "ProviderStamp.h"
#include "PCADViewerApp.h"

using namespace std;

//Это наш маленький, домашний торговец предметами искусства. Этакий внутрипроектный картиночный барыга.
//   "Давайте будем
//    нести искусство людям.
//    Берут они охотно"
//    старинные полотна."

const vector<wxString> StampProvider::serviced_art_id =
    {
        wxT("wxART_EXPORT_PICTURE"),
        wxT("wxART_PRINT"),
        wxT("wxART_MARK_ELEMENTS"),
        wxT("wxART_UNMARK_ELEMENTS"),
        wxT("wxART_SCALE_MINUS"),
        wxT("wxART_SCALE_PLUS"),
        wxT("wxART_SCALE_ONE_TO_ONE"),
        wxT("wxART_EQUAL_SCALES"),
        wxT("wxART_PORTRAIT_LANDSCAPE")
    };

const vector<wxString> StampProvider::serviced_art_name =
    {
        wxT("export_sign"),
        wxT("print_sign"),
        wxT("check_sign_d"),
        wxT("cross_sign_d"),
        wxT("minus_sign"),
        wxT("plus_sign"),
        wxT("one_to_one_sign"),
        wxT("equal_sign"),
        wxT("portrait_landscape_sign")
    };

wxBitmap StampProvider::CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                     const wxSize& size)
{
    if (client == wxART_OTHER || client == wxART_TOOLBAR)
    {
        auto find_name_it = find(serviced_art_id.begin(), serviced_art_id.end(), id);
        if (find_name_it != serviced_art_id.end())
            return wxXmlResource::Get()->
                   LoadBitmap(serviced_art_name[find_name_it - serviced_art_id.begin()]);
    }
    return wxNullBitmap;
}

void StampProvider::InitStampProvider()
{
    wxArtProvider::Push(new StampProvider);
}
