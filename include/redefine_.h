
#ifdef _
    #undef _
#endif // _

// Макросы для образования "широких" wx-строк(wxString, состоящих из wchar_t) из их "узких" Юникодных
// UTF8-вариантов (состоящих из обычных char).
#define _(x) (wxGetTranslation(wxString::FromUTF8(x)))
#define wxTR(x) (wxGetTranslation(wxString::FromUTF8(x)))
#define wxF8(x) (wxString::FromUTF8(x))
#define wxFS8(x) (wxString::FromUTF8((x).c_str()))
// Макросы для конверсии чисел в wx-строку.
#define wxFD(x) (wxString::FromDouble(x))
#define wxFDN(x, N) (wxString::FromDouble(x, N))
#define wxFI(x) (wxString::FromDouble(x, 0))
// Специальный макрос для преобразования масштабов, которые всегда имеют SCALE_PRECISION цифр после запятой.
#define SCALE_PRECISION 3
#define wxFSC(x) (wxString::FromDouble(x, SCALE_PRECISION))
