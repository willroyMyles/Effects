#pragma once

#include <irisgl/IrisGL.h>

class QMouseEvent;

class NodeGraph;
class SceneWidget : public iris::RenderWidget
{
    iris::MeshPtr mesh;
    iris::ShaderPtr shader;
    iris::MaterialPtr mat;

    iris::FontPtr font;
    float fps;

    iris::VertexBufferPtr vertexBuffer;
    iris::Texture2DPtr tex;

    iris::CameraNodePtr cam;

    QString vertString;
    QString fragString;

    float renderTime;
    QList<iris::LightNodePtr> lights;

    NodeGraph* graph;

	bool dragging;
	QPoint lastMousePos;
	QQuaternion rot;
	float scale;
public:
    SceneWidget();

    void start();

    void update(float dt);

    void render();

    void updateShader(QString shaderCode);

    void resetRenderTime();

    void passNodeGraphUniforms();
    void setNodeGraph(NodeGraph* graph);


	// mouse events
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);	
	void mouseReleaseEvent(QMouseEvent* evt);
};
