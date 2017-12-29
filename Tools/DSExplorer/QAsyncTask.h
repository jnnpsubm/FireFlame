#pragma once
#include <memory>
#include <functional>
#include <future>
#include "qprogressdialog.h"
#include "qtimer.h"

class QAsyncTask : public QObject{
    Q_OBJECT

public:
    void Run(QWidget* parent, std::function<void()> task, std::function<void()> doneCallback);
    bool Running() const { return mStatus == Status::Running; }

private slots:
    void OnTimer();

private:
    int                              mProgressVal = 0;
    std::unique_ptr<QProgressDialog> mProgressDlg = nullptr;
    std::unique_ptr<QTimer>          mProgressTimer = nullptr;

    enum class Status {
        Running,
        Finished
    };
    Status mStatus = Status::Finished;
    std::future<void> mFuture;
    std::function<void()> mDoneCallBack;
};