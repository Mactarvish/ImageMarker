#include "ImageMarker.h"
#include "ui_ImageMarker.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->label_big->setMouseTracking(true);
    //setMouseTracking(true);
    ui->label_big->installEventFilter(this);

    image = new QPixmap("/home/hdl2/Desktop/Sono_nofolder/158_HAOTINGTING_1_Labeled.jpg");
    painter = new QPainter();
    fileNames_ap = new QList<QString>();

    //rootPath = new QString("/home/hdl2/Desktop/Circle_Images/");
    QDir root = QDir("small_images/");
    QFileInfoList list = root.entryInfoList();
    points.resize(0);
    coordinates.resize(0);

    imageChanged = false;

    for (int i = 2; i < list.length(); i++)
    {
        //qDebug() << list[i].fileName();
        if (list[i].fileName().contains(".jpg"))
        {
            //qDebug() << (*fileNames_ap)[i - 2];
            fileNames_ap->append(root.absolutePath() + "/" + list[i].fileName());
        }
    }
    qDebug() << fileNames_ap->size();
    ChangeImages(0);
    currentPixmap = new QPixmap(*(ui->label_big->pixmap()));
    ui->lineEdit->setText((*fileNames_ap)[0]);
    /*
    ui->label_big->setPixmap(image->copy(279, 115, 815, 597));
    ui->label_big->resize(image->size());

    QPixmap small_image = image->scaled(image->width() / 5, image->height() / 5);
    ui->label_small->setPixmap(small_image);
    ui->label_small->resize(small_image.size());
    */
}

Widget::~Widget()
{
    delete ui;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        int r = QMessageBox::warning(this, "ImageMarker",
                                     "Are you sure to exit?",
                                     QMessageBox::Yes | QMessageBox::No);
        if (r == QMessageBox::Yes)
        {
            this->close();
        }
    }
    static int image_index = 0;
    if (event->key() == Qt::Key_Right)
    {
        qDebug() << fileNames_ap->length();
        if (image_index == fileNames_ap->length() - 1)
        {
            image_index = 0;
        }
        else
        {
            image_index++;
        }
        ChangeImages(image_index);
        delete currentPixmap;
        currentPixmap = new QPixmap(*(ui->label_big->pixmap()));
    }
    if (event->key() == Qt::Key_Left)
    {
        if (image_index == 0)
        {
            image_index = fileNames_ap->length() - 1;
        }
        else
        {
            image_index--;
        }
        qDebug() << image_index;
        ChangeImages(image_index);
        delete currentPixmap;
        currentPixmap = new QPixmap(*(ui->label_big->pixmap()));
    }
    if (event->key() == Qt::Key_Enter)
    {
        qDebug() << "Warning: Enter event is unsetting.";
        //imageChanged = true;
    }


    /*
     * For distinguish if an image has been labeled.
     *
    //QString& fileName = (*fileNames_ap)[image_index];
    //QString filePath = *rootPath + fileName;


    if (event->key() == Qt::Key_Enter)
    {
        QString newPath = QString();
        if (this->CheckIfMarked(fileName))
        {
            fileName = fileName.remove("_Circle");
        }
        else
        {
            fileName = fileName.insert(fileName.indexOf(".jpg"), "_Circle");
        }
        newPath = *rootPath + fileName;

        qDebug() << newPath;
        QFile::rename(filePath, newPath);
    }
    */
    ui->lineEdit->setText((*fileNames_ap)[image_index]);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << event->pos();
}

bool Widget::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->label_big)
    {
        static bool drawFinish = true;
        static QPoint beginPoint = QPoint();
        QPixmap drawedPixmap = *currentPixmap;
        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (!drawFinish)
            {
                qDebug() << "Keep drawing...";
                painter->begin(&drawedPixmap);
                painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
                painter->drawRect(QRect(beginPoint, mouseEvent->pos()));
                painter->end();
            }
            else
            {
                qDebug() << "Drawing Finished.";
            }
            ui->label_big->setPixmap(drawedPixmap);
        }
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            drawFinish = !drawFinish;
            if (!drawFinish)
            {
                //drawedPixmap = QPixmap(*(ui->label_big->pixmap()));
                beginPoint = mouseEvent->pos();
            }
            // If all points in current image have been noted, change to next image.
            if (HandleLabels_Circle(mouseEvent->pos(), 2))
            {
                // Simulate right key pressing
                QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));
            }
            //HandleLabels_Rect(mouseEvent->pos());
            //delete currentPixmap;
            //currentPixmap = new QPixmap(*(ui->label_big->pixmap()));
            //currentPixmap = ui->label_big->pixmap();
            //qDebug() << currentPixmap;
        }
    }
    return QWidget::eventFilter(target, event);
}

bool Widget::CheckIfMarked(const QString &fileName) const
{
    if (fileName.contains("_Circle"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Widget::ChangeImages(const int &index)
{
    QPixmap bigImage = QPixmap((*fileNames_ap)[index]).copy(279, 115, 815, 597);
    //QPixmap smallImage = bigImage.scaled(bigImage.width() / 3, bigImage.height() / 3);

    //ui->label_small->setPixmap(smallImage);
    //ui->label_small->resize(smallImage.size());
    ui->label_big->setPixmap(bigImage);
    ui->label_big->resize(bigImage.size());
}

// If numPos has been arrived, return true, otherwise return false.
bool Widget::HandleLabels_Circle(const QPoint &pos, const int& numPos)
{
    QString coordinate = Point2Str(pos);
    points.append(coordinate);
    QLabel* label = new QLabel("<h5><font size=3 color=red>" + coordinate + "</font></h5>", ui->label_big);
    coordinates.append(label);
    label->setMouseTracking(true);
    label->adjustSize();
    label->move(pos);
    label->raise();
    label->show();

    if (points.size() == numPos)
    {
        NoteLabels();
        points.clear();
        foreach(QLabel* label, coordinates)
        {
            delete label;
        }
        coordinates.clear();
        return true;
    }
    else
    {
        return false;
    }
}

void Widget::NoteLabels()
{
    //QString fileName = "coordinates.txt";
    QString fileName = "rectangle.txt";
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream in(&file);

    in << "\n" << ui->lineEdit->text().split("/").last() << " ";
    for (int i = 0; i < points.size(); i++)
    {
        in << points[i] << " ";
    }
    file.flush();
    file.close();
}

QString Widget::Point2Str(const QPoint &pos)
{
    return QString("(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")");
}

MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{
    signature = new QString("The more things change, the more they stay the same.");
    //setMouseTracking(true);
}

MyButton::~MyButton()
{
    
}

void MyButton::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "My own class!" << event->pos();
    return QPushButton::mouseMoveEvent(event);
}



