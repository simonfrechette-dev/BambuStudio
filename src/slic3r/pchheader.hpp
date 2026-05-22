#ifdef WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
	#include <CommCtrl.h>
#endif

#ifdef __APPLE__
  #include <ApplicationServices/ApplicationServices.h>
#endif

#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <Eigen/Dense>
#include <exception> 
#include <exception>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <ostream>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/locale.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/log/trivial.hpp>
#include <boost/nowide/cenv.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/optional.hpp>

// boost/property_tree/json_parser/detail/parser.hpp includes boost/bind.hpp, which is deprecated.
// Suppress the following boost message:
// The practice of declaring the Bind placeholders (_1, _2, ...) in the global namespace is deprecated.
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#undef BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/system/error_code.hpp>

#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>

#ifdef _MSC_VER
	// avoid some "macro redefinition" warnings
	#include <urlmon.h>
#endif /* _MSC_VER */

// Qt headers (replacing wxWidgets)
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QMainWindow>
#include <QFrame>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QTimer>
#include <QColor>
#include <QFont>
#include <QPixmap>
#include <QIcon>
#include <QPainter>
#include <QSize>
#include <QRect>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QGroupBox>
#include <QSplitter>
#include <QStatusBar>
#include <QToolTip>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>

#include "libslic3r/Config.hpp"
#include "libslic3r/PrintConfig.hpp"
#include "libslic3r/TriangleMesh.hpp"
#include "libslic3r/Point.hpp"
#include "libslic3r/MultiPoint.hpp"
#include "libslic3r/Polygon.hpp"
#include "libslic3r/Polyline.hpp"
#include "libslic3r/BoundingBox.hpp"
#include "libslic3r/ClipperUtils.hpp"
#include "libslic3r/libslic3r.h"

#ifdef _WIN32
#include "GUI/format.hpp"
#endif // _WIN32

#endif // __cplusplus
