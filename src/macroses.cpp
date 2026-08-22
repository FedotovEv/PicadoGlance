
#include "macroses.h"
#include <string>

#include "redefine_.h"

using namespace std;

// Список определяющих пар (первый член пары - тип кодировки - элемент перечисления wxFontEncoding,
// второй - текстовое имя этой кодировки) допустимых кодировок для PDIF-документов.
const std::vector<std::pair<wxFontEncoding, wxString>> pdif_supported_encodings
{
    {wxFONTENCODING_ISO8859_1, wxTRANSLATE("West European (Latin1)")},
    {wxFONTENCODING_ISO8859_2, wxTRANSLATE("Central and East European (Latin2)")},
    {wxFONTENCODING_ISO8859_3, wxTRANSLATE("Esperanto (Latin3)")},
    {wxFONTENCODING_ISO8859_4, wxTRANSLATE("Baltic (old) (Latin4)")},
    {wxFONTENCODING_ISO8859_5, wxTRANSLATE("Cyrillic")},
    {wxFONTENCODING_ISO8859_6, wxTRANSLATE("Arabic")},
    {wxFONTENCODING_ISO8859_7, wxTRANSLATE("Greek")},
    {wxFONTENCODING_ISO8859_8, wxTRANSLATE("Hebrew")},
    {wxFONTENCODING_ISO8859_9, wxTRANSLATE("Turkish (Latin5)")},
    {wxFONTENCODING_ISO8859_10, wxTRANSLATE("Variation of Latin4 (Latin6)")},
    {wxFONTENCODING_ISO8859_11, wxTRANSLATE("Thai")},
    {wxFONTENCODING_ISO8859_13, wxTRANSLATE("Baltic (Latin7)")},
    {wxFONTENCODING_ISO8859_14, wxTRANSLATE("Latin8")},
    {wxFONTENCODING_ISO8859_15, wxTRANSLATE("Latin9")},
    {wxFONTENCODING_KOI8, wxTRANSLATE("KOI8 Russian")},
    {wxFONTENCODING_CP866, wxTRANSLATE("MS-DOS cyrillic encoding")},
    {wxFONTENCODING_CP1250, wxTRANSLATE("WinLatin2")},
    {wxFONTENCODING_CP1251, wxTRANSLATE("WinCyrillic")},
    {wxFONTENCODING_CP1252, wxTRANSLATE("WinLatin1")},
    {wxFONTENCODING_CP1253, wxTRANSLATE("WinGreek (8859-7)")},
    {wxFONTENCODING_CP1254, wxTRANSLATE("WinTurkish")},
    {wxFONTENCODING_CP1255, wxTRANSLATE("WinHebrew")},
    {wxFONTENCODING_CP1256, wxTRANSLATE("WinArabic")},
    {wxFONTENCODING_CP1257, wxTRANSLATE("WinBaltic (same as Latin 7)")}
};

string UpcaseString(const string& arg_string)
{
    string result;
    for (char c : arg_string)
        result += toupper(c);
    return result;
}

string TrimString(const string& arg_string)
{
    string result;
    size_t begin_not_space = arg_string.find_first_not_of(" \t\r\n"s);
    size_t end_not_space = arg_string.find_last_not_of(" \t\r\n"s);
    if (begin_not_space != string::npos && end_not_space != string::npos)
        result = arg_string.substr(begin_not_space, end_not_space - begin_not_space + 1);
    return result;
}

vector<pair<wxFontEncoding, wxString>>::const_iterator FindEncodingPair(wxFontEncoding scan_pdif_encoding)
{
    auto scan_enc_it = find_if(pdif_supported_encodings.begin(), pdif_supported_encodings.end(),
        [scan_pdif_encoding](const pair<wxFontEncoding, wxString>& test_enc_pair) -> bool
        {
            return test_enc_pair.first == scan_pdif_encoding;
        });
    if (scan_enc_it != pdif_supported_encodings.end())
        return scan_enc_it;
    else
        return pdif_supported_encodings.begin();
}
