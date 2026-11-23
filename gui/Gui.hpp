#pragma once
#include <QWidget>
#include <QMainWindow>
#include <QHash>
#include <QSet>
#include <QPointF>
#include "../graph/Graph.hpp"
#include "../graph/GraphGenerator.hpp"
#include "../graph/DijkstraSteps.hpp"

class QSpinBox;
class QPushButton;
class QLabel;
class QSlider;
class QTimer;

using IntStep = DijkstraStep<int, int>;

class GraphView : public QWidget {
    Q_OBJECT
public:
    explicit GraphView(QWidget* parent = nullptr);
    void setGraph(IntGraph* g);
    void setStart(int v, bool has);
    void setTarget(int v, bool has);
    void resetState();
    void applyStep(const IntStep& step);
    void markFinished();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    IntGraph* graph = nullptr;
    QHash<int, QPointF> positions;
    QHash<int, int> dist;
    QSet<int> visited;
    int current = -1;
    int relax_from = -1, relax_to = -1;
    bool relax_ok = false;
    bool finished = false;
    int start_v = -1;
    bool has_start = false;
    int target_v = -1;
    bool has_target = false;

    void recomputePositions();
    int distanceOf(int v) const;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void onDemo();
    void onRandom();
    void onComplete();
    void onTree();
    void onRun();
    void onStep();
    void onPlayPause();
    void onReset();
    void onTimerTick();
    void onSpeedChanged(int v);
    void onTargetChanged(int v);

private:
    GraphView* view;
    QPushButton* btnDemo;
    QPushButton* btnRandom;
    QPushButton* btnComplete;
    QPushButton* btnTree;
    QPushButton* btnRun;
    QPushButton* btnStep;
    QPushButton* btnPlay;
    QPushButton* btnReset;
    QSpinBox* spinStart;
    QSpinBox* spinTarget;
    QSlider* speedSlider;
    QLabel* statusLabel;
    QTimer* timer;

    IntGraph graph;
    MutableArraySequence<IntStep> steps;
    std::size_t step_idx = 0;
    bool playing = false;
    int speed_ms = 500;
};