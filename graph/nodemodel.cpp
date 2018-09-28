#include <QUuid>
#include <QString>
#include <QColor>

#include "nodemodel.h"
#include "socketmodel.h"

NodeModel::NodeModel()
{
	id = QUuid::createUuid().toString();
	widget = nullptr;
	connect(this, &NodeModel::titleColorChanged, [=]() {
		setNodeTitleColor();
	});
}

void NodeModel::addInputSocket(SocketModel *sock)
{
	inSockets.append(sock);
	sock->setNode(this);
}

void NodeModel::addOutputSocket(SocketModel *sock)
{
	outSockets.append(sock);
	sock->setNode(this);
}

QString NodeModel::getValueFromInputSocket(int index)
{
	auto sock = inSockets[index];
	if (sock->hasConnection()) {
		//return sock->getConnectedSocket()->getVarName();

		// converts the var before sending it back
		return sock->getConnectedSocket()->convertVarTo(sock);
	}

	return sock->getValue();
}

QString NodeModel::getOutputSocketVarName(int index)
{
	auto sock = outSockets[index];
	return sock->getVarName();
}

NodeGraph *NodeModel::getGraph() const
{
	return graph;
}

void NodeModel::setGraph(NodeGraph *value)
{
	graph = value;
}

QColor NodeModel::setNodeTitleColor()
{
	switch (nodeType) {
	case NodeType::Number:
		return titleColor = QColor(0, 103, 91);
		break;
	case NodeType::Calculation:
		return titleColor = QColor(0, 103, 181);
		break;
	case NodeType::Modifier:
		return titleColor = QColor(190, 20, 10);
		break;
	case NodeType::Surface:
		return titleColor = QColor(52, 81, 94);
		break;
	default:
		return titleColor = QColor(0, 0, 0, 0);
		break;
	}

}

void NodeModel::setNodeType(NodeType type)
{
	nodeType = type;
	emit titleColorChanged();

}