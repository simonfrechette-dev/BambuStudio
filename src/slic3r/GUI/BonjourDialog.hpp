#ifndef slic3r_BonjourDialog_hpp_
#define slic3r_BonjourDialog_hpp_
#include <QWidget>
#include <QString>

#include <memory>


#include "libslic3r/PrintConfig.hpp"

class QLabel;
class QTimer;
class QTimerEvent;


namespace Slic3r {

class Bonjour;
class BonjourReplyEvent;
class ReplySet;


class BonjourDialog: public QDialog
{
public:
	BonjourDialog(QWidget *parent, Slic3r::PrinterTechnology);
	BonjourDialog(BonjourDialog &&) = delete;
	BonjourDialog(const BonjourDialog &) = delete;
	BonjourDialog &operator=(BonjourDialog &&) = delete;
	BonjourDialog &operator=(const BonjourDialog &) = delete;
	~BonjourDialog();

	bool show_and_lookup();
	QString get_selected() const;
private:
	QListView *list;
	std::unique_ptr<ReplySet> replies;
	QLabel *label;
	std::shared_ptr<Bonjour> bonjour;
	std::unique_ptr<QTimer> timer;
	unsigned timer_state;
	Slic3r::PrinterTechnology tech;

	void on_reply(BonjourReplyEvent &);
	void on_timer(QTimerEvent &);
    void on_timer_process();
};



}

#endif
