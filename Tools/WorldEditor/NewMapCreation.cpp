#include "NewMapCreation.h"
#include "WorldEditor.h"

NewMapCreation::NewMapCreation(WorldEditor* editor, QWidget *parent)
    : QWidget(parent), mEditor(editor)
{
    ui.setupUi(this);

    ui.widthEditor->setValidator(new QIntValidator(0, 10240, this));
    ui.heightEditor->setValidator(new QIntValidator(0, 10240, this));
}

NewMapCreation::~NewMapCreation()
{
}

void NewMapCreation::OnOK()
{
    QString val = ui.widthEditor->text();
    int width = val.toInt();
    val = ui.heightEditor->text();
    int height = val.toInt();
    close();

    mEditor->NewMap(width, height);
    delete this;
}

void NewMapCreation::OnCancel()
{
    close();
    delete this;
}