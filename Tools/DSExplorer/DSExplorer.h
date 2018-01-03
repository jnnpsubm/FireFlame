#pragma once
#include <memory>
#include <future>
#include <thread>
#include <QtWidgets/QMainWindow>
#include "ui_DSExplorer.h"
#include <QFileInfo>
#include <qprogressdialog.h>
#include <qtimer.h>
#include "QAsyncTask.h"
#include "DSGameFileType.h"
#include "DSFileNameDictionary.h"
#include "DSBdt5FileStream.h"
#include "DSBhd5File.h"

struct TreeItemData {
    TreeItemData(bool is_directory) :is_directory(is_directory) {}
    bool is_directory = true;
    bool meaningful = false;
};

struct TreeItemDataDS : TreeItemData {
    TreeItemDataDS(const DSFS::GameFileType& type, const std::string& path)
        :TreeItemData(false), absFilePath(path), gameFileType(type) {}
    DSFS::GameFileType gameFileType;
    std::string absFilePath;
};

struct TreeItemDataEBdt : TreeItemDataDS {
    TreeItemDataEBdt(const DSFS::GameFileType& type, const std::string& path) 
        :TreeItemDataDS(type, path) {}
    std::unique_ptr<DSFS::Bdt5FileStream> bdt5FileStream = nullptr;
    std::unique_ptr<DSFS::Bhd5File>       bhd5File = nullptr;
};

struct TreeItemDataDSEntry : TreeItemDataDS {
    TreeItemDataDSEntry(const DSFS::GameFileType& type, const std::string& path)
        :TreeItemDataDS(type, path) {}
    std::string fileName;
    DSFS::Bhd5BucketEntry* entry = nullptr;
};

struct TreeItemDataHelper {
    static TreeItemData* AllocDSTreeItemData(DSFS::GameFileType gameFileType, const std::string& filePath) {
        switch (gameFileType.fileType)
        {
        case DSFS::FileType::EncryptedBdt:
            return new TreeItemDataEBdt(gameFileType, filePath);
        default:
            return new TreeItemDataDS(gameFileType, filePath);
        }
    }
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

class QTreeWidgetHelper
{
public:
    static QTreeWidgetItem* GetChild(const QTreeWidgetItem* parent, const QString& text) {
        int chindCount = parent->childCount();
        for (int i = 0; i < chindCount; i++) // todo : search
        {
            if (parent->child(i)->text(0) == text) return parent->child(i);
        }
        return nullptr;
    }
    static void SortChildren(QTreeWidgetItem* parent) {
        parent->sortChildren(0, Qt::SortOrder::AscendingOrder);
        int chindCount = parent->childCount();
        for (int i = 0; i < chindCount; i++)
        {
            SortChildren(parent->child(i));
        }
    }
};

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

private:
    void FormFileTree(const QString& path);
    void FormFileTree(const QString& path, QTreeWidgetItem* parent);
    void TrimFileTree();
    void ClearFileTree();

    void FormNameDictionaryTree();

    void ExpandEBdtNode(QTreeWidgetItem* node);

    QAsyncTask mAsyncTask;
    void LoadNameDictionary();
    void ParseEncryptedBdtFile(TreeItemData* data);

    QFileInfoList GetFileList(QString path);

    Ui::DSExplorerClass ui;

private:
    QString mDSPath;
    std::unique_ptr<DSFS::FileNameDictionary> mFileNameDictionary = nullptr;
};
