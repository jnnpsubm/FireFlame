#include "WorldEditor.h"
#include "NewMapCreation.h"
#include "qlabel.h"
#include "FireFlameHeader.h"

WorldEditor::WorldEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void WorldEditor::OnNewMap()
{
    NewMapCreation* newMapCreation = new NewMapCreation(this);
    newMapCreation->setWindowModality(Qt::WindowModality::ApplicationModal);
    newMapCreation->showNormal();
}

void WorldEditor::NewMap(int width, int depth)
{
    using namespace FireFlame;

    GeometryGenerator geoGenerator;
    auto grid = geoGenerator.CreateGrid(width, depth, width, depth);
}