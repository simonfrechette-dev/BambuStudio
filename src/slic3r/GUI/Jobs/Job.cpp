#include <algorithm>
#include <exception>

#include "Job.hpp"
#include <libslic3r/Thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/chrono.hpp>
#include <QApplication>

namespace Slic3r {

void GUI::Job::run(std::exception_ptr &eptr)
{
    m_running.store(true);
    try {
        process();
    } catch (...) {
        eptr = std::current_exception();
    }
    m_running.store(false);

    // ensure to call the last status to finalize the job
    update_status(status_range(), QString());
}

void GUI::Job::update_status(int st, const QString &msg)
{
    // Marshal to the main thread via queued connection
    // (replaces wxQueueEvent + wxThreadEvent handler bound in ctor)
    QMetaObject::invokeMethod(this, [this, st, msg]() {
        if (m_finalizing) return;

        if (this->is_print_job() && st >= 100)
            update_percent_finish();

        if (!msg.isEmpty() && !m_worker_error)
            m_progress->set_status_text(msg.toUtf8().constData());

        if (m_finalized) return;

        m_progress->set_progress(st);

        if (st == status_range() || m_worker_error) {
            // set back the original range and cancel callback
            m_progress->set_range(m_range);
            m_progress->set_progress(m_range);
            m_progress->set_cancel_callback();

            if (m_worker_error) {
                m_finalized = true;
                m_progress->set_status_text("");
                m_progress->set_progress(m_range);
                on_exception(m_worker_error);
            } else {
                // RAII guard prevents re-entrant finalization if yield occurs
                struct Finalizing {
                    bool &flag;
                    Finalizing(bool &f) : flag(f) { flag = true; }
                    ~Finalizing() { flag = false; }
                } fin(m_finalizing);

                finalize();
            }

            QApplication::restoreOverrideCursor();
            m_finalized = true;
        }
    }, Qt::QueuedConnection);
}

void GUI::Job::update_percent_finish()
{
    m_progress->clear_percent();
}

void GUI::Job::show_error_info(const QString &msg, int code,
                               const QString &description,
                               const QString &extra)
{
    m_progress->show_error_info(msg, code, description, extra);
}

GUI::Job::Job(std::shared_ptr<ProgressIndicator> pri)
    : m_progress(std::move(pri))
{
    // No wx event binding needed — handler is inlined in update_status lambda
}

void GUI::Job::start()
{
    if (!m_running.load()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        prepare();

        // Save the current status indicator range and push the new one
        m_range = m_progress->get_range();
        m_progress->set_range(status_range());

        m_canceled.store(false);
        m_progress->set_cancel_callback([this]() { m_canceled.store(true); });

        m_finalized  = false;
        m_finalizing = false;

        try {
            m_worker_error = nullptr;
            m_thread = create_thread([this] { this->run(m_worker_error); });
        } catch (std::exception &) {
            update_status(status_range(), _L("Error! Unable to create thread!"));
        }
    }
}

bool GUI::Job::join(int timeout_ms)
{
    if (!m_thread.joinable()) return true;

    if (timeout_ms <= 0)
        m_thread.join();
    else if (!m_thread.try_join_for(boost::chrono::milliseconds(timeout_ms)))
        return false;

    return true;
}

void GUI::ExclusiveJobGroup::start(size_t jid)
{
    assert(jid < m_jobs.size());
    stop_all();
    m_jobs[jid]->start();
}

void GUI::ExclusiveJobGroup::join_all(int wait_ms)
{
    std::vector<bool> aborted(m_jobs.size(), false);

    for (size_t jid = 0; jid < m_jobs.size(); ++jid)
        aborted[jid] = m_jobs[jid]->join(wait_ms);

    if (!std::all_of(aborted.begin(), aborted.end(), [](bool t) { return t; }))
        BOOST_LOG_TRIVIAL(error) << "Could not abort a job!";
}

bool GUI::ExclusiveJobGroup::is_any_running() const
{
    return std::any_of(m_jobs.begin(), m_jobs.end(),
                       [](const std::unique_ptr<GUI::Job> &j) {
        return j->is_running();
    });
}

} // namespace Slic3r
