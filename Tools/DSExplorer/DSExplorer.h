#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_DSExplorer.h"
#include <QFileInfo>

class DSExplorer : public QMainWindow
{
    Q_OBJECT

public:
    DSExplorer(QWidget *parent = Q_NULLPTR);

private slots:
    void LoadDSStorage();

private:
    void FormFileTree(QString path, QTreeWidgetItem* parent);
    void TrimFileTree();

    QFileInfoList GetFileList(QString path);

    Ui::DSExplorerClass ui;

private:
    struct TreeItemData
    {
        TreeItemData(const std::string& path, bool bDir) :absFilePath(path), bDir(bDir) {}
        std::string absFilePath;
        bool bDir = true;
        bool meaningful = false;

        static TreeItemData* GetItemData(QTreeWidgetItem* item) {
            QVariant var = item->data(0, Qt::UserRole);
            return reinterpret_cast<TreeItemData*>(var.toLongLong());
        }
        static void SetMeaningful(QTreeWidgetItem* item) {
            auto data = GetItemData(item);
            data->meaningful = true;
            if (item->parent()) SetMeaningful(item->parent());
        }
    };
};
