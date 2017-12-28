#pragma once
#include <memory>
#include <future>
#include <thread>
#include <QtWidgets/QMainWindow>
#include "ui_DSExplorer.h"
#include <QFileInfo>
#include <qprogressdialog.h>
#include <qtimer.h>
#include "DSGameFileType.h"
#include "DSFileNameDictionary.h"
#include "DSBdt5FileStream.h"

class DSExplorer : public QMainWindow
{
    Q_OBJECT

public:
    DSExplorer(QWidget *parent = Q_NULLPTR);
    ~DSExplorer();

private slots:
    void OnLoadDSStorage();
    void OnViewDS3NameDictionary();
    void OnTreeItemDBClick(QTreeWidgetItem* item, int);
    void OnProgressTimer();

private:
    void FormFileTree(QString path, QTreeWidgetItem* parent);
    void TrimFileTree();
    void ClearFileTree();

    void FormNameDictionaryTree();

    std::future<void> mFutureLoad;
    void LoadNameDictionary();

    QFileInfoList GetFileList(QString path);

    Ui::DSExplorerClass ui;
    std::unique_ptr<QProgressDialog> mProgressDlg   = nullptr;
    std::unique_ptr<QTimer>          mProgressTimer = nullptr;
    int             mProgressVal = 0;

private:
    std::unique_ptr<DSFS::FileNameDictionary> mFileNameDictionary = nullptr;
    struct TreeItemData
    {
        TreeItemData(const std::string& path, bool bDir)
            :absFilePath(path), bDir(bDir){}
        TreeItemData(const DSFS::GameFileType& type, const std::string& path, bool bDir) 
            :absFilePath(path), bDir(bDir), type(type) {}
        DSFS::GameFileType type;
        std::string absFilePath;
        bool bDir = true;
        bool meaningful = false;

        std::unique_ptr<DSFS::Bdt5FileStream> bdt5FileStream = nullptr;

        static TreeItemData* GetItemData(QTreeWidgetItem* item) {
            QVariant var = item->data(0, Qt::UserRole);
            return reinterpret_cast<TreeItemData*>(var.value<void*>());
        }
        static void SetMeaningful(QTreeWidgetItem* item) {
            auto data = GetItemData(item);
            data->meaningful = true;
            if (item->parent()) SetMeaningful(item->parent());
        }
    };
};
