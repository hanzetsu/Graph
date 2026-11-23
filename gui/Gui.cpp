#include "Gui.hpp"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphView::GraphView(QWidget* parent) : QWidget(parent) {
    setMinimumSize(600, 600);
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor(250, 250, 252));
    setPalette(p);
}

void GraphView::setGraph(IntGraph* g) {
    graph = g;
    recomputePositions();
    resetState();
}

void GraphView::setStart(int v, bool has) { start_v = v; has_start = has; update(); }
void GraphView::setTarget(int v, bool has) { target_v = v; has_target = has; update(); }

void GraphView::resetState() {
    dist.clear();
    visited.clear();
    current = -1;
    relax_from = relax_to = -1;
    relax_ok = false;
    finished = false;
    if (graph && has_start) {
        for (const auto& v : graph->get_vertices())
            dist[v] = (v == start_v) ? 0 : std::numeric_limits<int>::max();
    }
    update();
}

void GraphView::applyStep(const IntStep& step) {
    relax_from = relax_to = -1;
    if (step.kind == IntStep::Select) {
        current = step.current;
        visited.insert(step.current);
        dist[step.current] = step.new_dist;
    } else if (step.kind == IntStep::Relax) {
        dist[step.neighbor] = step.new_dist;
        relax_from = step.current;
        relax_to = step.neighbor;
        relax_ok = true;
    } else if (step.kind == IntStep::Reject) {
        relax_from = step.current;
        relax_to = step.neighbor;
        relax_ok = false;
    } else if (step.kind == IntStep::Done) {
        finished = true;
        current = -1;
    }
    update();
}

void GraphView::markFinished() {
    finished = true;
    current = -1;
    relax_from = relax_to = -1;
    update();
}

void GraphView::resizeEvent(QResizeEvent*) { recomputePositions(); update(); }

void GraphView::recomputePositions() {
    positions.clear();
    if (!graph) return;
    auto vs = graph->get_vertices();
    std::size_t n = vs.GetLength();
    if (n == 0) return;
    double cx = width() / 2.0;
    double cy = height() / 2.0;
    double r = std::min(cx, cy) * 0.72;
    for (std::size_t i = 0; i < n; ++i) {
        double a = 2.0 * M_PI * i / n - M_PI / 2.0;
        positions[vs[i]] = QPointF(cx + r * std::cos(a), cy + r * std::sin(a));
    }
}

int GraphView::distanceOf(int v) const {
    auto it = dist.find(v);
    return it == dist.end() ? std::numeric_limits<int>::max() : it.value();
}

void GraphView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (!graph) return;
    if (positions.isEmpty()) recomputePositions();

    const int R = 26;
    auto vs = graph->get_vertices();
    auto es = graph->get_edges();

    for (const auto& e : es) {
        int a = e.GetFirst(), b = e.GetSecond();
        if (!positions.contains(a) || !positions.contains(b)) continue;
        QPointF pa = positions[a], pb = positions[b];

        QPen pen(QColor(180, 180, 180), 2);
        if (a == relax_from && b == relax_to)
            pen = QPen(relax_ok ? QColor(40, 180, 60) : QColor(220, 50, 50), 4);
        p.setPen(pen);
        p.drawLine(pa, pb);

        int w = graph->get_weight_edge(a, b);
        QPointF mid = (pa + pb) / 2.0;
        p.setPen(QColor(60, 60, 60));
        QFont f = p.font();
        f.setPointSize(9);
        p.setFont(f);
        p.drawText(QRectF(mid.x() - 15, mid.y() - 10, 30, 20),
                   Qt::AlignCenter, QString::number(w));
    }

    QFont vf = p.font();
    vf.setBold(true);
    vf.setPointSize(11);
    for (const auto& v : vs) {
        if (!positions.contains(v)) continue;
        QPointF pos = positions[v];
        QRectF rect(pos.x() - R, pos.y() - R, 2*R, 2*R);

        QColor fill(255, 255, 255);
        QColor border(120, 120, 120);
        int d = distanceOf(v);
        bool inf = (d == std::numeric_limits<int>::max());

        if (v == current) { fill = QColor(255, 220, 60); border = QColor(200, 150, 0); }
        else if (visited.contains(v)) { fill = QColor(160, 230, 160); border = QColor(40, 150, 40); }
        else if (!inf) { fill = QColor(180, 210, 255); border = QColor(60, 100, 200); }

        if (finished && inf) { fill = QColor(240, 120, 120); border = QColor(180, 30, 30); }

        p.setBrush(fill);
        p.setPen(QPen(border, v == current ? 3 : 2));
        p.drawEllipse(rect);

        p.setPen(QColor(20, 20, 20));
        p.setFont(vf);
        p.drawText(rect, Qt::AlignCenter, QString::number(v));

        if (has_start) {
            QFont df = p.font();
            df.setBold(false);
            df.setPointSize(9);
            p.setFont(df);
            p.setPen(QColor(60, 60, 60));
            QString label = inf ? "d=INF" : "d=" + QString::number(d);
            p.drawText(QRectF(pos.x() - 40, pos.y() + R + 2, 80, 18),
                       Qt::AlignCenter, label);
            p.setFont(vf);
        }

        if (has_start && v == start_v) {
            p.setPen(QPen(QColor(0, 100, 200), 2));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(rect.adjusted(-4, -4, 4, 4));
        }
        if (has_target && v == target_v) {
            p.setPen(QPen(QColor(180, 100, 0), 3));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(rect.adjusted(-6, -6, 6, 6));
        }
    }

    p.setPen(QColor(80, 80, 80));
    QFont lf = p.font();
    lf.setPointSize(9);
    lf.setBold(false);
    p.setFont(lf);
    int y = 15;
    auto legend = [&](const QColor& c, const QString& t) {
        p.setBrush(c);
        p.setPen(QPen(QColor(100,100,100), 1));
        p.drawRect(10, y, 16, 16);
        p.setPen(QColor(60,60,60));
        p.drawText(32, y + 13, t);
        y += 22;
    };
    legend(QColor(255,255,255), "unvisited");
    legend(QColor(180,210,255), "frontier");
    legend(QColor(255,220,60),  "current");
    legend(QColor(160,230,160), "visited");
    legend(QColor(240,120,120), "unreachable");
}

// ---------------- MainWindow ----------------

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    auto* central = new QWidget;
    auto* mainLay = new QHBoxLayout(central);

    view = new GraphView;
    mainLay->addWidget(view, 1);

    auto* panel = new QVBoxLayout;

    auto* gbGraph = new QGroupBox("Graph");
    auto* gl = new QVBoxLayout(gbGraph);
    btnDemo = new QPushButton("Demo (8)");
    btnRandom = new QPushButton("Random (10/15)");
    btnComplete = new QPushButton("Complete K8");
    btnTree = new QPushButton("Tree (10)");
    gl->addWidget(btnDemo);
    gl->addWidget(btnRandom);
    gl->addWidget(btnComplete);
    gl->addWidget(btnTree);
    panel->addWidget(gbGraph);

    auto* gbRun = new QGroupBox("Search");
    auto* rl = new QVBoxLayout(gbRun);
    auto* hl1 = new QHBoxLayout;
    hl1->addWidget(new QLabel("Start:"));
    spinStart = new QSpinBox;
    spinStart->setRange(0, 999);
    hl1->addWidget(spinStart);
    rl->addLayout(hl1);
    auto* hl2 = new QHBoxLayout;
    hl2->addWidget(new QLabel("Target:"));
    spinTarget = new QSpinBox;
    spinTarget->setRange(-1, 999);
    spinTarget->setSpecialValueText("(none)");
    spinTarget->setValue(-1);
    hl2->addWidget(spinTarget);
    rl->addLayout(hl2);
    btnRun = new QPushButton("Run");
    btnStep = new QPushButton("Step");
    btnPlay = new QPushButton("Play");
    btnReset = new QPushButton("Reset");
    rl->addWidget(btnRun);
    rl->addWidget(btnStep);
    rl->addWidget(btnPlay);
    rl->addWidget(btnReset);
    panel->addWidget(gbRun);

    auto* gbSpeed = new QGroupBox("Speed, ms");
    auto* sl = new QVBoxLayout(gbSpeed);
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(50, 2000);
    speedSlider->setValue(500);
    sl->addWidget(speedSlider);
    panel->addWidget(gbSpeed);

    statusLabel = new QLabel("Ready");
    statusLabel->setWordWrap(true);
    panel->addWidget(statusLabel);
    panel->addStretch();

    mainLay->addLayout(panel);
    setCentralWidget(central);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    connect(btnDemo, &QPushButton::clicked, this, &MainWindow::onDemo);
    connect(btnRandom, &QPushButton::clicked, this, &MainWindow::onRandom);
    connect(btnComplete, &QPushButton::clicked, this, &MainWindow::onComplete);
    connect(btnTree, &QPushButton::clicked, this, &MainWindow::onTree);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(btnPlay, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(spinTarget, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onTargetChanged);

    onDemo();
    resize(1000, 720);
}

void MainWindow::onDemo() {
    graph.clear();
    for (int i = 0; i < 8; ++i) graph.add_vertex(i, "v" + std::to_string(i));
    graph.add_undirected_edge(0, 1, 4);
    graph.add_undirected_edge(0, 2, 1);
    graph.add_undirected_edge(2, 1, 2);
    graph.add_undirected_edge(1, 3, 5);
    graph.add_undirected_edge(2, 3, 8);
    graph.add_undirected_edge(3, 4, 3);
    graph.add_undirected_edge(4, 5, 1);
    graph.add_undirected_edge(5, 6, 2);
    spinStart->setRange(0, 7);
    spinStart->setValue(0);
    spinTarget->setRange(-1, 7);
    spinTarget->setValue(-1);
    view->setGraph(&graph);
    view->setStart(0, true);
    view->setTarget(-1, false);
    view->resetState();
    steps = MutableArraySequence<IntStep>();
    step_idx = 0;
    statusLabel->setText("Demo loaded");
}

void MainWindow::onRandom() {
    gen_random(graph, 10, 15);
    spinStart->setRange(0, 9);
    spinStart->setValue(0);
    spinTarget->setRange(-1, 9);
    spinTarget->setValue(-1);
    view->setGraph(&graph);
    view->setStart(0, true);
    view->setTarget(-1, false);
    view->resetState();
    steps = MutableArraySequence<IntStep>();
    step_idx = 0;
    statusLabel->setText("Random graph");
}

void MainWindow::onComplete() {
    gen_complete(graph, 8);
    spinStart->setRange(0, 7);
    spinStart->setValue(0);
    spinTarget->setRange(-1, 7);
    spinTarget->setValue(-1);
    view->setGraph(&graph);
    view->setStart(0, true);
    view->setTarget(-1, false);
    view->resetState();
    steps = MutableArraySequence<IntStep>();
    step_idx = 0;
    statusLabel->setText("Complete K8");
}

void MainWindow::onTree() {
    gen_tree(graph, 10);
    spinStart->setRange(0, 9);
    spinStart->setValue(0);
    spinTarget->setRange(-1, 9);
    spinTarget->setValue(-1);
    view->setGraph(&graph);
    view->setStart(0, true);
    view->setTarget(-1, false);
    view->resetState();
    steps = MutableArraySequence<IntStep>();
    step_idx = 0;
    statusLabel->setText("Tree graph");
}

void MainWindow::onRun() {
    if (graph.get_count_vertex() == 0) { statusLabel->setText("No graph"); return; }
    int start = spinStart->value();
    int tv = spinTarget->value();
    bool has_t = (tv >= 0);
    if (has_t && !graph.contains_vertex(tv)) {
        statusLabel->setText("Invalid target");
        return;
    }
    view->setStart(start, true);
    view->setTarget(tv, has_t);
    view->resetState();

    MutableArraySequence<int> prev;
    try {
        steps = dijkstra_steps(graph, start, prev, has_t, tv);
    } catch (const std::exception& e) {
        statusLabel->setText(QString("Error: ") + e.what());
        return;
    }
    step_idx = 0;
    playing = false;
    btnPlay->setText("Play");
    timer->stop();
    statusLabel->setText(QString("Ready: %1 steps").arg(steps.GetLength()));
}

void MainWindow::onStep() {
    if (step_idx >= steps.GetLength()) {
        view->markFinished();
        return;
    }
    view->applyStep(steps[step_idx]);
    statusLabel->setText(QString("Step %1 / %2").arg(step_idx + 1).arg(steps.GetLength()));
    ++step_idx;
    if (step_idx >= steps.GetLength()) {
        view->markFinished();
        playing = false;
        btnPlay->setText("Play");
        timer->stop();
    }
}

void MainWindow::onPlayPause() {
    if (playing) {
        playing = false;
        btnPlay->setText("Play");
        timer->stop();
        return;
    }
    if (steps.GetLength() == 0) onRun();
    if (steps.GetLength() == 0) return;
    if (step_idx >= steps.GetLength()) {
        view->resetState();
        step_idx = 0;
    }
    playing = true;
    btnPlay->setText("Pause");
    timer->start(speed_ms);
}

void MainWindow::onReset() {
    timer->stop();
    playing = false;
    btnPlay->setText("Play");
    step_idx = 0;
    view->resetState();
    statusLabel->setText("Reset");
}

void MainWindow::onTimerTick() {
    onStep();
    if (step_idx >= steps.GetLength()) {
        playing = false;
        btnPlay->setText("Play");
        timer->stop();
    }
}

void MainWindow::onSpeedChanged(int v) {
    speed_ms = v;
    if (timer->isActive()) timer->start(speed_ms);
}

void MainWindow::onTargetChanged(int v) {
    view->setTarget(v, v >= 0);
}