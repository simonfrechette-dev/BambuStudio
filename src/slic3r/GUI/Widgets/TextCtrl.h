#ifdef __WXMSW__
class TextCtrl : public QLineEdit
{
public:
    using QLineEdit::QLineEdit;
    WXHBRUSH DoMSWControlColor(WXHDC pDC, QColor colBg, WXHWND hWnd) { return QLineEdit::DoMSWControlColor(pDC, QColor(), hWnd); }
};
#else
typedef QLineEdit TextCtrl;
#endif
