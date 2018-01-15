#pragma once

#include <QWidget>
#include "ui_NewMapCreation.h"

class WorldEditor;
class NewMapCreation : public QWidget
{
    Q_OBJECT

public:
    NewMapCreation(WorldEditor* editor, QWidget *parent = Q_NULLPTR);
    ~NewMapCreation();

private slots:
    void OnOK();
    void OnCancel();

private:
    Ui::NewMapCreation ui;
    WorldEditor* mEditor = nullptr;
};
