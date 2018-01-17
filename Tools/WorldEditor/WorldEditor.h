#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WorldEditor.h"
#include "FireFlameHeader.h"

class WorldEditor : public QMainWindow
{
    Q_OBJECT

public:
    WorldEditor(QWidget *parent = Q_NULLPTR);

    void NewMap(int width, int depth);

private slots:
    void OnNewMap();

private:
    Ui::WorldEditorClass ui;

    FireFlame::Engine mEngine;
    bool              mEngineInitialized = false;
};
