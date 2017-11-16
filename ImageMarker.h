#ifndef IMAGEMARKER_H
#define IMAGEMARKER_H

#include <QWidget>
#include <QPixmap>
#include <QDebug>
#include <QKeyEvent>
#include <QDir>
#include <QFile>
#include <QPushButton>
#include <QMessageBox>
#include <QVector>
#include <QPainter>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    bool eventFilter(QObject *target, QEvent *event);

private:
    Ui::Widget *ui;
    QPixmap* image;
    QList<QString>* fileNames_ap;
    QString* rootPath;
    QVector<QString> points;
    QVector<QLabel*> coordinates;
    QPainter* painter;
    const QPixmap* currentPixmap;
    int currentImageIndex;

    bool imageChanged;

    void ShowAllfiles();
    bool CheckIfMarked(const QString& fileName) const;
    void ChangeImages(const int& index);
    bool HandleLabels_Circle(const QPoint &pos, const int& numPos);
    void NoteLabels();
    QString Point2Str(const QPoint& pos);
};

class MyButton : public QPushButton
{
public:
    explicit MyButton(QWidget* parent = 0);
    ~MyButton();
    
protected:
    void mouseMoveEvent(QMouseEvent* event);

private:    
    QString* signature;
};

#endif // IMAGEMARKER_H
