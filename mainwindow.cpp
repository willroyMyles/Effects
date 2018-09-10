#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nodegraph.h"
#include "nodemodel.h"
#include <QMouseEvent>
#include <QDebug>
#include "scenewidget.h"
#include <QLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QByteArray>
#include "graphtest.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new GraphNodeScene(this);
    scene->setBackgroundBrush(QBrush(QColor(60, 60, 60)));
    ui->graphicsView->setScene(scene);

    // set default
    /*
    auto masterNode = new NodeModel();
    masterNode->typeName = "Material";
    masterNode->inSockets.append(new Vector3SocketModel("diffuse"));
    masterNode->inSockets.append(new Vector3SocketModel("specular"));
    masterNode->inSockets.append(new FloatSocketModel("shininess"));
    masterNode->inSockets.append(new Vector3SocketModel("normal", "v_normal"));
    masterNode->inSockets.append(new Vector3SocketModel("ambient"));
    masterNode->inSockets.append(new Vector3SocketModel("emission"));
    masterNode->inSockets.append(new FloatSocketModel("alpha"));
    */



    connect(scene, &GraphNodeScene::newConnection, [this](SocketConnection* connection)
    {

        auto graph = scene->getNodeGraph();
        ShaderGenerator shaderGen;
        auto code = shaderGen.generateShader(graph);
        ui->textEdit->setPlainText(code);
        sceneWidget->updateShader(code);
        sceneWidget->resetRenderTime();

    });

    connect(scene, &GraphNodeScene::nodeValueChanged, [this](NodeModel* model, int index)
    {
        auto graph = scene->getNodeGraph();
        ShaderGenerator shaderGen;
        auto code = shaderGen.generateShader(graph);
        ui->textEdit->setPlainText(code);
        sceneWidget->updateShader(code);
        sceneWidget->resetRenderTime();
    });

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveGraph);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadGraph);

    // preview widget
    sceneWidget = new SceneWidget();
    auto grid = new QGridLayout();
    grid->setSpacing(0);
    grid->setMargin(0);
    grid->addWidget(sceneWidget);
    ui->sceneContainer->setLayout(grid);

    // add menu items to property widget
    newNodeGraph();
}

void MainWindow::setNodeGraph(NodeGraph *graph)
{
    registerModels(graph);
    scene->setNodeGraph(graph);
    ui->propertyContainerPage1->setNodeGraph(graph);
    sceneWidget->setNodeGraph(graph);
}

void MainWindow::newNodeGraph()
{
    auto graph = new NodeGraph;
    auto masterNode = new SurfaceMasterNode();
    graph->addNode(masterNode);
    graph->setMasterNode(masterNode);

    setNodeGraph(graph);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveGraph()
{
    QString path = QFileDialog::getSaveFileName(this, "Choose file name","material.json","Material File (*.json)");

    QJsonDocument doc;
    doc.setObject(scene->serialize());

    QFile file(path);
    file.open(QFile::WriteOnly | QFile::Truncate);
    file.write(doc.toJson());
    file.close();
}

void MainWindow::loadGraph()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose file name","material.json","Material File (*.json)");

    QFile file;
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val);

    auto graph = NodeGraph::deserialize(d.object());
    this->setNodeGraph(graph);
    this->restoreGraphPositions(d.object());
}

void MainWindow::restoreGraphPositions(const QJsonObject &data)
{
    auto scene = data["scene"].toObject();
    auto nodeList = scene["nodes"].toArray();

    for(auto nodeVal : nodeList) {
        auto nodeObj = nodeVal.toObject();
        auto nodeId = nodeObj["id"].toString();
        auto node = this->scene->getNodeById(nodeId);
        node->setX(nodeObj["x"].toDouble());
        node->setY(nodeObj["y"].toDouble());
    }
}
