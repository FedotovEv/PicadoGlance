#pragma once

#include <string>
#include <vector>
#include <wx/artprov.h>
#include <wx/bitmap.h>

class StampProvider : public wxArtProvider
{
public:
    static void InitStampProvider();

protected:
    wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                          const wxSize& size) override;
private:
    static const std::vector<wxString> serviced_art_id;
    static const std::vector<wxString> serviced_art_name;
};
