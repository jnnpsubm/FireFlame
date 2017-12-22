#include <iostream>
#include "DSExplorer.h"
#include "QFileDialog"
#include "QMessageBox"
#include "FireFlameHeader.h"
#include "..\CBinderToolLib\Utils.h"

DSExplorer::DSExplorer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //ui.treeFiles->setHeaderLabels(QStringList() << "File" << "Type(Maybe)");
    /*ui.treeFiles->setStyleSheet
    (
        QString::fromUtf8
        (
            "::item {\n"
            "    border: 1px solid blue;\n"
            "    border-top-color: transparent;\n"
            "    border-bottom-color: transparent;\n"
            "}"
        )
    );*/

    FireFlame::OpenConsole();
}

void DSExplorer::LoadDSStorage()
{
    QString filePath = QFileDialog::getExistingDirectory();
    if (filePath.size() > 0)
    {
        //QMessageBox::information(this, tr("Info"), filePath, QMessageBox::Yes);
        std::cout << "DS path:" << filePath.toStdString() << std::endl;
        ui.treeFiles->clear();
        FormFileTree(filePath, nullptr);
        TrimFileTree();
        /*auto filelist = GetFileList(filePath);
        std::cout << "file count:" << filelist.size() << std::endl;
        for (const auto& filename : filelist)
        {
            auto absFilePath = filename.absoluteFilePath();
            std::cout << "file:" << absFilePath.toStdString() << std::endl;
        }*/
    }
}

void DSExplorer::FormFileTree(QString path, QTreeWidgetItem* parent)
{
    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto& item : fileList)
    {
        auto absFilePath = item.absoluteFilePath();
        auto itemName = absFilePath.mid(path.size() + 1);
        auto fileType = CBinderToolLib::Utils::GetFileType(itemName.toStdString());
        auto fileDes = CBinderToolLib::Utils::GetFileTypeDescription(fileType);
        std::cout << "file:" << itemName.toStdString() << " possible type:" << fileDes << std::endl;
        if (fileType.first == CBinderToolLib::FileType::Unknown) continue;
        // todo : delete?
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << itemName);
        auto data = new TreeItemData(absFilePath.toStdString(), false);
        QVariant var(QVariant::LongLong, &data);
        item->setData(0, Qt::UserRole, var);
        if (parent)
            parent->addChild(item);
        else 
            ui.treeFiles->addTopLevelItem(item);
    }
    for (auto& item : folderList)
    {
        auto absFilePath = item.absoluteFilePath();
        auto itemName = absFilePath.mid(path.size() + 1);
        // todo : delete?
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << itemName);
        auto data = new TreeItemData(absFilePath.toStdString(), true);
        QVariant var(QVariant::LongLong, &data);
        item->setData(0, Qt::UserRole, var);
        if (parent)
            parent->addChild(item);
        else
            ui.treeFiles->addTopLevelItem(item);
        FormFileTree(absFilePath, item);
    }
}

void DSExplorer::TrimFileTree()
{
    QTreeWidgetItemIterator it(ui.treeFiles);
    while (*it) {
        TreeItemData* data = TreeItemData::GetItemData(*it);
        std::cout << "tree item data:" << data->absFilePath << std::endl;
        if (!data->bDir) TreeItemData::SetMeaningful(*it);
        ++it;
    }
    /*QTreeWidgetItemIterator it2(ui.treeFiles);
    while (*it2) {
        TreeItemData* data = TreeItemData::GetItemData(*it2);
        std::cout << "tree item data:" << data->absFilePath << " meaningful:" << data->meaningful << std::endl;
        ++it2;
    }*/
    auto root = ui.treeFiles->topLevelItemCount();
    for (int i = 0; i < ui.treeFiles->topLevelItemCount(); i++)
    {
        auto item = ui.treeFiles->topLevelItem(i);
        auto data = TreeItemData::GetItemData(item);
        if (!data->meaningful)
        {
            ui.treeFiles->takeTopLevelItem(i);
            --i;
        }
    }
}

QFileInfoList DSExplorer::GetFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}
