#include <iostream>
#include <consoleapi.h>
#include <sstream>
#include <iomanip>
#include "DSExplorer.h"
#include "QFileDialog"
#include "QMessageBox"
#include "src\FLStringUtils.h"

DSExplorer::DSExplorer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    AllocConsole();
    FILE *stream;
    freopen_s(&stream, "CONOUT$", "w+t", stdout);
    freopen_s(&stream, "CONIN$", "r+t", stdin);
}

DSExplorer::~DSExplorer()
{
    ClearFileTree();
}

void DSExplorer::OnLoadDSStorage()
{
    QString filePath = QFileDialog::getExistingDirectory();
    if (filePath.size() > 0)
    {
        if (!mFileNameDictionary)
        {
            mFileNameDictionary = std::make_unique<DSFS::FileNameDictionary>();
            mFileNameDictionary->Init(DSFS::GameVersion::DarkSouls3);
            //mFileNameDictionary->SaveDictionary2File("fileNameDictionary.txt");
        }

        ClearFileTree();
        std::cout << "DS path:" << filePath.toStdString() << std::endl;
        FormFileTree(filePath, nullptr);
        TrimFileTree();
    }
}

void DSExplorer::OnViewDS3NameDictionary()
{
    if (!mFileNameDictionary)
    {
        mProgressDlg.reset(new QProgressDialog());
        mProgressDlg->setMinimumDuration(0);
        mProgressDlg->setWindowTitle("Please wait...");
        mProgressDlg->setWindowModality(Qt::WindowModal);
        mProgressDlg->setModal(true);
        mProgressDlg->setCancelButton(0);
        mProgressDlg->setFixedWidth(300);
        mProgressDlg->setRange(0, 100);
        mProgressDlg->setValue(0);

        mProgressTimer.reset(new QTimer());
        connect(mProgressTimer.get(), SIGNAL(timeout()), this, SLOT(OnProgressTimer()));
        mProgressTimer->setInterval(100);
        mProgressTimer->start();
        mProgressDlg->show();

        mFutureLoad = std::async(std::launch::async, [&]() { LoadNameDictionary(); });
        //LoadNameDictionary();
    }
    else 
    {
        FormNameDictionaryTree();
    }
}

void DSExplorer::LoadNameDictionary()
{
    if (!mFileNameDictionary)
    {
        mFileNameDictionary = std::make_unique<DSFS::FileNameDictionary>();
        mFileNameDictionary->Init(DSFS::GameVersion::DarkSouls3);
    }
}

void DSExplorer::FormNameDictionaryTree()
{
    ClearFileTree();

    const auto& Dictionary = mFileNameDictionary->GetDictionary();
    for (const auto& itDictionary : Dictionary)
    {
        QTreeWidgetItem* itemArchive = new QTreeWidgetItem(QStringList() << itDictionary.first.c_str());
        ui.treeFiles->addTopLevelItem(itemArchive);
        for (const auto& itHash : itDictionary.second)
        {
            std::ostringstream oss;
            oss << std::setw(10) << std::setfill('0') << std::right << itHash.first;
            QTreeWidgetItem* itemHash = new QTreeWidgetItem(QStringList() << oss.str().c_str());
            itemArchive->addChild(itemHash);
            for (const auto& fileName : itHash.second)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << fileName.c_str());
                itemHash->addChild(item);
            }
            QCoreApplication::processEvents();
        }
    }
}

void DSExplorer::OnTreeItemDBClick(QTreeWidgetItem* item, int)
{
    TreeItemData* data = TreeItemData::GetItemData(item);
    if (data == nullptr) return;

    std::cout << "Processing " << data->absFilePath 
        << " type:" << DSFS::GetGameFileTypeStr(data->type) << std::endl;
    if (data->type.fileType == DSFS::FileType::EncryptedBdt && data->bdt5FileStream == nullptr)
    {
        std::string fileNameWithoutExtension = FireFlame::StringUtils::file_name(data->absFilePath);
        FireFlame::StringUtils::replace(fileNameWithoutExtension, "Ebl.bdt", "");
        FireFlame::StringUtils::replace(fileNameWithoutExtension, ".bdt", "");
        std::string archiveName = fileNameWithoutExtension;
        FireFlame::StringUtils::tolower(archiveName);
        std::cout << "Archive: " << archiveName << std::endl;

        data->bdt5FileStream = std::make_unique<DSFS::Bdt5FileStream>(data->absFilePath);
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
        auto fileType = DSFS::GetGameFileType(itemName.toStdString());
        auto fileDes = DSFS::GetGameFileTypeStr(fileType);
        std::cout << "file:" << itemName.toStdString() << " possible type:" << fileDes << std::endl;
        if (fileType.fileType == DSFS::FileType::Unknown) continue;

        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << itemName);
        QVariant var = QVariant::fromValue((void*)new TreeItemData(fileType, absFilePath.toStdString(), false));
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

        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << itemName);
        QVariant var = QVariant::fromValue((void*)new TreeItemData(absFilePath.toStdString(), true));
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

void DSExplorer::ClearFileTree()
{
    QTreeWidgetItemIterator it(ui.treeFiles);
    while (*it) {
        TreeItemData* data = TreeItemData::GetItemData(*it);
        delete data;
        ++it;
    }
    ui.treeFiles->clear();
}

void DSExplorer::OnProgressTimer()
{
    mProgressVal++;
    if (mProgressVal == 100)
        mProgressVal = 0;
    mProgressDlg->setValue(mProgressVal);
    QCoreApplication::processEvents();

    if (mFutureLoad.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        mProgressTimer->stop();
        mProgressDlg->setValue(100);
        mProgressDlg->hide();
        mProgressTimer.reset(nullptr);
        mProgressDlg.reset(nullptr);

        FormNameDictionaryTree();
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
