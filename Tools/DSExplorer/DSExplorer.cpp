#include <iostream>
#include <consoleapi.h>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include "DSExplorer.h"
#include "QFileDialog"
#include "QMessageBox"
#include "qtextstream.h"
#include "qmutex.h"
#include "qdatetime.h"
#include "src\FLStringUtils.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch (type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("Message:%1 File:%2  Line:%3  Function:%4  DateTime:%5")
        .arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);

    // 输出信息至文件中（读写、追加形式）
    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    std::cout << strMessage.toStdString() << std::endl;

    // 解锁
    mutex.unlock();
}

DSExplorer::DSExplorer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    qInstallMessageHandler(myMessageOutput);

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
    QString filePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"D:\\DSIII\\Datas2",QFileDialog::ShowDirsOnly);
    if (filePath.size() > 0)
    {
        mDSPath = filePath;
        if (!mFileNameDictionary)
        {
            if (mAsyncTask.Running()) return;
            mAsyncTask.Run(this, [&]() { LoadNameDictionary(); }, [&]() {FormFileTree(mDSPath); });
        }
        else
        {
            FormFileTree(mDSPath);
        }
    }
}

void DSExplorer::OnViewDS3NameDictionary()
{
    if (!mFileNameDictionary)
    {
        if (mAsyncTask.Running()) return;
        mAsyncTask.Run(this, [&]() { LoadNameDictionary(); }, [&]() {FormNameDictionaryTree(); });
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

void DSExplorer::ParseEncryptedBdtFile(TreeItemData* dataBase)
{
    TreeItemDataEBdt* data = reinterpret_cast<TreeItemDataEBdt*>(dataBase);

    std::string fileNameWithoutExtension = FireFlame::StringUtils::file_name(data->absFilePath);
    FireFlame::StringUtils::replace(fileNameWithoutExtension, "Ebl.bdt", "");
    FireFlame::StringUtils::replace(fileNameWithoutExtension, ".bdt", "");
    std::string archiveName = fileNameWithoutExtension;
    FireFlame::StringUtils::tolower(archiveName);
    std::cout << "Archive: " << archiveName << std::endl;

    data->bdt5FileStream = std::make_unique<DSFS::Bdt5FileStream>(data->absFilePath);

    std::string bhdFileName = FireFlame::StringUtils::change_extension(data->absFilePath, "bhd");
    auto bhd5File = std::make_unique<DSFS::Bhd5File>(bhdFileName, data->gameFileType.gameVersion);
    bhd5File->Decipher();
    bhd5File->Parse();
    for (auto& bucket : bhd5File->GetBuckets())
    {
        for (auto& entry : bucket.GetEntries())
        {
            if (entry.GetFileSize() == 0)
            {
                entry.TryGetFileSize(*data->bdt5FileStream.get());
            }

            std::string dataExtension = entry.GetDataExtension(*data->bdt5FileStream.get());
            /*std::cout << "hash:" << entry.GetFileNameHash() 
                << " size:" << entry.GetFileSize() 
                << " dataExtension:" << dataExtension << std::endl;*/

            std::string fileName;
            bool fileNameFound 
                = mFileNameDictionary->TryGetFileName(entry.GetFileNameHash(), archiveName, fileName);
            if (!fileNameFound)
            {
                fileNameFound 
                    = mFileNameDictionary->TryGetFileName(entry.GetFileNameHash(), archiveName, dataExtension, fileName);
            }

            std::string extension;
            if (fileNameFound)
            {
                extension = FireFlame::StringUtils::file_extension(fileName);
                if (dataExtension == ".dcx" && extension != ".dcx")
                {
                    extension = ".dcx";
                    fileName += ".dcx";
                }
            }
            else
            {
                extension = dataExtension;
                std::ostringstream oss;
                oss << std::setw(10) << std::setfill('0') << std::right << entry.GetFileNameHash() 
                    << '_' << fileNameWithoutExtension << extension;
                fileName = oss.str();
            }
            bucket.AddEntryMap(fileName, &entry);
        }
    }
    data->bhd5File = std::move(bhd5File);
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
    TreeItemDataDS* data = (TreeItemDataDS*)TreeItemDataHelper::GetItemData(item);
    if (data == nullptr) return;

    std::cout << "Processing " << data->absFilePath 
        << " type:" << DSFS::GetGameFileTypeStr(data->gameFileType) << std::endl;
    if (data->gameFileType.fileType == DSFS::FileType::EncryptedBdt)
    {
        TreeItemDataEBdt* dataEBdt = (TreeItemDataEBdt*)data;
        if (dataEBdt->bdt5FileStream == nullptr)
        {
            while (mAsyncTask.Running())
            {
                QCoreApplication::processEvents();
            }
            //std::cout << "this:" << this << " item:" << item << std::endl;
            mAsyncTask.Run(this, [this,data]() {ParseEncryptedBdtFile(data); }, [item,this]() {ExpandEBdtNode(item); });
        }
    }
    else if (data->gameFileType.fileType == DSFS::FileType::Dcx)
    {
        TreeItemDataDSEntry* dataEntry = (TreeItemDataDSEntry*)data;
    }
}

void DSExplorer::ExpandEBdtNode(QTreeWidgetItem* node)
{
    TreeItemDataEBdt* data = (TreeItemDataEBdt*)TreeItemDataHelper::GetItemData(node);
    auto bdt5FileStream = data->bdt5FileStream.get();
    auto bhd5File = data->bhd5File.get();
    assert(bdt5FileStream!=nullptr&&bhd5File!=nullptr);
    
    for (auto& bucket : bhd5File->GetBuckets())
    {
        for (auto& itEntry : bucket.GetEntryMap())
        {
            //std::cout << itEntry.first << std::endl;
            std::string filePath = itEntry.first;

            size_t pos1 = 0;
            size_t pos2 = 0;
            QTreeWidgetItem* parent = node;
            while ((pos2 = filePath.find('\\',pos1)) != std::string::npos)
            {
                QString dirName = (filePath.substr(pos1, pos2 - pos1)).c_str();
                auto item = QTreeWidgetHelper::GetChild(parent, dirName);
                if (!item)
                {
                    item = new QTreeWidgetItem(QStringList() << dirName);
                    parent->addChild(item);
                }
                parent = item;
                pos1 = pos2+1;
            }
            QString fileName = (filePath.substr(pos1)).c_str();
            QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << fileName);
            auto gameFileType = DSFS::GetGameFileType(filePath);
            TreeItemDataDSEntry* dataEntry = new TreeItemDataDSEntry(gameFileType, filePath);
            dataEntry->entry = itEntry.second;
            dataEntry->bdt5FileStream = bdt5FileStream;
            QVariant var = QVariant::fromValue((void*)dataEntry);
            item->setData(0, Qt::UserRole, var);
            parent->addChild(item);

            QApplication::processEvents();
        }
    }
    //node->sortChildren(0, Qt::SortOrder::AscendingOrder);
    QTreeWidgetHelper::SortChildren(node);
}

void DSExplorer::FormFileTree(const QString& path)
{
    ClearFileTree();
    std::cout << "DS path:" << path.toStdString() << std::endl;
    FormFileTree(path, nullptr);
    TrimFileTree();
}

void DSExplorer::FormFileTree(const QString& path, QTreeWidgetItem* parent)
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
        QVariant var = QVariant::fromValue((void*)TreeItemDataHelper::AllocDSTreeItemData(fileType, absFilePath.toStdString()));
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
        QVariant var = QVariant::fromValue((void*)new TreeItemData(true));
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
        TreeItemData* data = TreeItemDataHelper::GetItemData(*it);
        if (!data->is_directory) TreeItemDataHelper::SetMeaningful(*it);
        ++it;
    }
    auto root = ui.treeFiles->topLevelItemCount();
    for (int i = 0; i < ui.treeFiles->topLevelItemCount(); i++)
    {
        auto item = ui.treeFiles->topLevelItem(i);
        auto data = TreeItemDataHelper::GetItemData(item);
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
        TreeItemData* data = TreeItemDataHelper::GetItemData(*it);
        delete data;
        ++it;
    }
    ui.treeFiles->clear();
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
