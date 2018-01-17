#include "WorldEditor.h"
#include "NewMapCreation.h"
#include "qdesktopwidget.h"
#include "qlabel.h"
#include "FireFlameHeader.h"

WorldEditor::WorldEditor(QWidget *parent)
    : QMainWindow(parent), mEngine(nullptr)
{
    ui.setupUi(this);

    FireFlame::OpenConsole();
}

void WorldEditor::OnNewMap()
{
    NewMapCreation* newMapCreation = new NewMapCreation(this);
    newMapCreation->setWindowModality(Qt::WindowModality::ApplicationModal);
    newMapCreation->showNormal();

    QRect rect = centralWidget()->geometry();
}

void WorldEditor::NewMap(int width, int depth)
{
    using namespace FireFlame;

    if (!mEngineInitialized)
    {
        mEngine.InitMainWindow((HWND)centralWidget()->winId(), centralWidget()->normalGeometry().width(), centralWidget()->normalGeometry().height());
    }

    GeometryGenerator geoGenerator;
    auto grid = geoGenerator.CreateGrid(width, depth, width/10.f, depth/10.f);
    std::vector<FLVertexTex> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); i++)
    {
        vertices[i].Pos = { grid.Vertices[i].Position.x,grid.Vertices[i].Position.y, grid.Vertices[i].Position.z };
        vertices[i].Tex = { grid.Vertices[i].TexC.x,grid.Vertices[i].TexC.y };
    }


}