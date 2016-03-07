#ifndef VIEW_H
#define VIEW_H

#include <QFrame>
#include <QGraphicsView>
#include <vector>
#include "VertexItem.h"
#include "MyMesh.h"
#include "View3D.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

class View2D;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(View2D *v) : QGraphicsView(), view(v) { }

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
#endif

	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    View2D *view;

};

class View2D : public QFrame
{
    Q_OBJECT
public:
    explicit View2D(const QString &name, QWidget *parent = 0);

    QGraphicsView *view() const;
	void changeSelection();
	void changeModel();
	void updateSelection();
	void updatePos();
	std::vector<VertexItem*> vertexList;
	trimesh::MyMesh *mm;
    GraphicsView *graphicsView;
	View2D *v1, *v2;
	View3D *v3;

public slots:
    void zoomIn(int level = 1);
    void zoomOut(int level = 1);

private slots:
    void resetView();
    void setResetButtonEnabled();
    void setupMatrix();
    void togglePointerMode();
    void toggleOpenGL();
    void toggleAntialiasing();
    void print();
    void rotateLeft();
    void rotateRight();
	void deleteVertex();

private:
    QLabel *label;
    QLabel *label2;
	QToolButton *deleteVertexButton;
    QToolButton *selectModeButton;
    QToolButton *dragModeButton;
    QToolButton *openGlButton;
    QToolButton *antialiasButton;
    QToolButton *printButton;
    QToolButton *resetButton;
    QSlider *zoomSlider;
    QSlider *rotateSlider;

};

#endif // VIEW_H
