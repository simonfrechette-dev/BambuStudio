#ifndef _
#define _(s)    	Slic3r::GUI::I18N::translate((s))
#define _L(s)    	Slic3r::GUI::I18N::translate((s))
#define _devL(s)	QString::fromUtf8((s))
#define _omitL(s)   ("")
#define _utf8(s)    Slic3r::GUI::I18N::translate_utf8((s))
#define _u8L(s)     Slic3r::GUI::I18N::translate_utf8((s))
#endif /* _ */

#ifndef _CTX
#define _CTX(s, ctx) 	  Slic3r::GUI::I18N::translate((s), (ctx))
#define _CTX_utf8(s, ctx) Slic3r::GUI::I18N::translate_utf8((s), (ctx))
#endif /* _ */

#ifndef L
// !!! If you needed to translate some QString,
// !!! please use _L(string)
// !!! _() - is a standard wxWidgets macro to translate
// !!! L() is used only for marking localizable string 
// !!! It will be used in "xgettext" to create a Locating Message Catalog.
#define L(s) s
#endif /* L */

#ifndef L_CONTEXT
#define L_CONTEXT(s, context) s
#endif /* L */

#ifndef _CHB
//! macro used to localization, return std::string (UTF-8)
#define _CHB(s) Slic3r::GUI::I18N::translate_utf8(s)
#endif /* _CHB */

#ifndef slic3r_GUI_I18N_hpp_
#define slic3r_GUI_I18N_hpp_

#include <QString>
#include <QCoreApplication>
#include <string>

// Translation context used for all BambuStudio strings
#define SLIC3R_I18N_CTX "BambuStudio"

namespace Slic3r { namespace GUI { 

namespace I18N {
	// Translate from char* / std::string (assumed UTF-8)
	inline QString translate(const char        *s) { return QCoreApplication::translate(SLIC3R_I18N_CTX, s); }
	inline QString translate(const std::string &s) { return QCoreApplication::translate(SLIC3R_I18N_CTX, s.c_str()); }
	inline QString translate(const QString     &s) { return s; }
	// Translate with plural form
	inline QString translate(const char        *s, const char *plural, unsigned int n) { return (n == 1) ? QCoreApplication::translate(SLIC3R_I18N_CTX, s) : QCoreApplication::translate(SLIC3R_I18N_CTX, plural); }
	inline QString translate(const std::string &s, const std::string &plural, unsigned int n) { return translate(s.c_str(), plural.c_str(), n); }
	// Translate with context disambiguation
	inline QString translate(const char        *s, const char *ctx) { return QCoreApplication::translate(ctx, s); }
	inline QString translate(const std::string &s, const char *ctx) { return QCoreApplication::translate(ctx, s.c_str()); }

	// UTF-8 std::string variants (for callers that need std::string back)
	inline std::string translate_utf8(const char        *s) { return QCoreApplication::translate(SLIC3R_I18N_CTX, s).toStdString(); }
	inline std::string translate_utf8(const std::string &s) { return QCoreApplication::translate(SLIC3R_I18N_CTX, s.c_str()).toStdString(); }
	inline std::string translate_utf8(const char        *s, const char *plural, unsigned int n) { return translate(s, plural, n).toStdString(); }
	inline std::string translate_utf8(const std::string &s, const std::string &plural, unsigned int n) { return translate(s, plural, n).toStdString(); }
	inline std::string translate_utf8(const char        *s, const char *ctx) { return QCoreApplication::translate(ctx, s).toStdString(); }
	inline std::string translate_utf8(const std::string &s, const char *ctx) { return QCoreApplication::translate(ctx, s.c_str()).toStdString(); }
	// Qt port: allow passing QString directly
	inline std::string translate_utf8(const QString     &s) { return s.toStdString(); }
} // namespace I18N

// Return translated std::string as a QString
QString	L_str(const std::string &str);

} // namespace GUI
} // namespace Slic3r

// Macro to function both as a marker for xgettext and to actually perform the translation.
#ifndef _L_PLURAL
#define _L_PLURAL(s, plural, n) Slic3r::GUI::I18N::translate(s, plural, n)
#endif /* L */

#endif /* slic3r_GUI_I18N_hpp_ */
