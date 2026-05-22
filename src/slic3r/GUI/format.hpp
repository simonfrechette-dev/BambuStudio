#ifndef slic3r_GUI_format_hpp_
#define slic3r_GUI_format_hpp_

// Functional wrapper around boost::format.
// One day we may replace this wrapper with C++20 format
// https://en.cppreference.com/w/cpp/utility/format/format
// though C++20 format uses a different template pattern for position independent parameters.
// This wrapper also manages implicit conversion from QString to UTF8 and format_wxstr() variants are provided to format into QString.

#include <libslic3r/format.hpp>

#include <QString>
#include <QByteArray>

namespace Slic3r { 
namespace GUI { 

// Format input mixing UTF8 encoded strings (const char*, std::string) and QStrings, return a QString.
template<typename... TArgs>
inline QString format_wxstr(const char* fmt, TArgs&&... args) {
	boost::format message(fmt);
	return QString::fromUtf8(Slic3r::internal::format::format_recursive(message, std::forward<TArgs>(args)...).c_str());
}
template<typename... TArgs>
inline QString format_wxstr(const std::string& fmt, TArgs&&... args) {
	boost::format message(fmt);
	return QString::fromUtf8(Slic3r::internal::format::format_recursive(message, std::forward<TArgs>(args)...).c_str());
}
template<typename... TArgs>
inline QString format_wxstr(const QString& fmt, TArgs&&... args) {
	return format_wxstr(fmt.toUtf8().constData(), std::forward<TArgs>(args)...);
}
template<typename... TArgs>
inline std::string format(const char* fmt, TArgs&&... args) {
    return Slic3r::format(fmt, std::forward<TArgs>(args)...);
}
template<typename... TArgs>
inline std::string format(const std::string& fmt, TArgs&&... args) {
    return Slic3r::format(fmt, std::forward<TArgs>(args)...);
}
template<typename... TArgs>
inline std::string format(const QString& fmt, TArgs&&... args) {
    return Slic3r::format(fmt.toUtf8().constData(), std::forward<TArgs>(args)...);
}

} // namespace GUI

namespace internal {
	namespace format {
		// Wrapper for QString to be processed by Slic3r::format().
		struct utf8_buffer { 
			QByteArray data;
		};
		// Accept QString and convert it to UTF8 to be processed by Slic3r::format().
		inline const utf8_buffer cook(const QString &arg) {
			return utf8_buffer { arg.toUtf8() };
		}
		inline const utf8_buffer cook(QString &arg) {
			return utf8_buffer { arg.toUtf8() };
		}
		inline const utf8_buffer cook(QString &&arg) {
			return utf8_buffer{ arg.toUtf8() };
		}
	}
}

} // namespace Slic3r

namespace boost {
	namespace io {
		namespace detail {
			// Adaptor for boost::format to accept QString converted to UTF8.
			inline std::ostream& operator<<(std::ostream& os, const Slic3r::internal::format::utf8_buffer& str) {
				os << str.data.constData();
				return os;
			}
		}
	}
}

#endif /* slic3r_GUI_format_hpp_ */
