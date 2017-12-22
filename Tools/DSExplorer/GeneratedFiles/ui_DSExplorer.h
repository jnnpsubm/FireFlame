/********************************************************************************
** Form generated from reading UI file 'DSExplorer.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DSEXPLORER_H
#define UI_DSEXPLORER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DSExplorerClass
{
public:
    QAction *actionLoad_DS_Folder;
    QWidget *centralWidget;
    QTreeWidget *treeFiles;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *DSExplorerClass)
    {
        if (DSExplorerClass->objectName().isEmpty())
            DSExplorerClass->setObjectName(QStringLiteral("DSExplorerClass"));
        DSExplorerClass->resize(1528, 861);
        actionLoad_DS_Folder = new QAction(DSExplorerClass);
        actionLoad_DS_Folder->setObjectName(QStringLiteral("actionLoad_DS_Folder"));
        centralWidget = new QWidget(DSExplorerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        treeFiles = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeFiles->setHeaderItem(__qtreewidgetitem);
        treeFiles->setObjectName(QStringLiteral("treeFiles"));
        treeFiles->setGeometry(QRect(0, 0, 341, 791));
        treeFiles->setFrameShape(QFrame::StyledPanel);
        treeFiles->setFrameShadow(QFrame::Plain);
        treeFiles->setIndentation(2);
        treeFiles->header()->setVisible(false);
        DSExplorerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(DSExplorerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1528, 26));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        DSExplorerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(DSExplorerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        DSExplorerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(DSExplorerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        DSExplorerClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad_DS_Folder);

        retranslateUi(DSExplorerClass);
        QObject::connect(actionLoad_DS_Folder, SIGNAL(triggered()), DSExplorerClass, SLOT(LoadDSStorage()));

        QMetaObject::connectSlotsByName(DSExplorerClass);
    } // setupUi

    void retranslateUi(QMainWindow *DSExplorerClass)
    {
        DSExplorerClass->setWindowTitle(QApplication::translate("DSExplorerClass", "DSExplorer", nullptr));
        actionLoad_DS_Folder->setText(QApplication::translate("DSExplorerClass", "Load DS Folder", nullptr));
        menuFile->setTitle(QApplication::translate("DSExplorerClass", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DSExplorerClass: public Ui_DSExplorerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DSEXPLORER_H
