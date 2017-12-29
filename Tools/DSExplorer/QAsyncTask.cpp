#include "QAsyncTask.h"

void QAsyncTask::Run(QWidget* parent, std::function<void()> task, std::function<void()> doneCallback) 
{
    mStatus = Status::Running;
    mFuture = std::async(std::launch::async, task);
    mDoneCallBack = doneCallback;

    mProgressDlg.reset(new QProgressDialog(parent));
    mProgressDlg->setMinimumDuration(0);
    mProgressDlg->setWindowTitle("Please wait...");
    mProgressDlg->setWindowModality(Qt::WindowModal);
    mProgressDlg->setCancelButton(0);
    mProgressDlg->setFixedWidth(300);
    mProgressDlg->setRange(0, 100);
    mProgressDlg->setValue(0);

    mProgressTimer.reset(new QTimer(this));
    connect(mProgressTimer.get(), SIGNAL(timeout()), this, SLOT(OnTimer()));
    mProgressTimer->setInterval(100);
    mProgressTimer->start();
    mProgressDlg->show();
}

void QAsyncTask::OnTimer()
{
    mProgressVal++;
    if (mProgressVal == 100)
        mProgressVal = 0;
    mProgressDlg->setValue(mProgressVal);

    if (mFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        mProgressTimer->stop();
        mProgressDlg->setValue(100);
        mProgressDlg->hide();
        mProgressTimer.reset(nullptr);
        mProgressDlg.reset(nullptr);

        mStatus = Status::Finished;
        if (mDoneCallBack) mDoneCallBack();
    }
}