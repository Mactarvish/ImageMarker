#include "ImageMarker.h"
#include "ui_ImageMarker.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    /*
    delete ui->pushButton;
    ui->pushButton = new MyButton(this);
    ui->pushButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ui->pushButton->setFocusPolicy(Qt::NoFocus);
    ui->horizontalLayout->addWidget(ui->pushButton);
    ui->pushButton->setMouseTracking(true);
    */

    ui->label_big->setMouseTracking(true);
    //setMouseTracking(true);
    ui->label_big->installEventFilter(this);

    image = new QPixmap("/home/hdl2/Desktop/Sono_nofolder/158_HAOTINGTING_1_Labeled.jpg");
    painter = new QPainter();
    fileNames_ap = new QList<QString>();

    rootPath = new QString("/home/hdl2/Desktop/Circle_Images/");
    QDir root = QDir("small_images/");
    QFileInfoList list = root.entryInfoList();
    points.resize(0);
    coordiantes.resize(0);

    imageChanged = false;

    for (int i = 2; i < list.length(); i++)
    {
        qDebug() << list[i].fileName();
        if (list[i].fileName().contains(".jpg"))
        {
            //qDebug() << (*fileNames_ap)[i - 2];
            fileNames_ap->append(root.absolutePath() + "/" + list[i].fileName());
        }
    }
    ChangeImages(0);
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
        static QPixmap currentPixmap = *(ui->label_big->pixmap());
        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            //qDebug() << mouseEvent->pos();
            //QPixmap* pixmap = const_cast<QPixmap*>(ui->label_big->pixmap());
            QPixmap drawedPixmap = currentPixmap;
            /*
            if (!drawFinish)
            {
                painter->begin(&drawedPixmap);
                painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
                painter->drawRect(50, 50, 300, 300);
                //qDebug() << painter->isActive();
                painter->end();
                //ui->label_big->hide();
            }
            else
            {
                painter->begin(&drawedPixmap);
                painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
                painter->setBackground(currentPixmap);
                painter->eraseRect(30, 30, 310, 310);
                //qDebug() << painter->isActive();
                painter->end();
                //ui->label_big->hide();
                ui->label_big->setPixmap(drawedPixmap);
                ui->label_big->show();
                qDebug() << "Drawing is over.";
            }
            */
            ui->label_big->setPixmap(drawedPixmap);
        }
        if (event->type() == QEvent::MouseButtonPress)
        {
            drawFinish = !drawFinish;
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            qDebug() << mouseEvent->pos() << (mouseEvent->type() == QEvent::MouseButtonPress);
            //HandleLabels_Circle(mouseEvent->pos());
            //HandleLabels_Rect(mouseEvent->pos());
            QApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));
            currentPixmap = *(ui->label_big->pixmap());
            qDebug() << currentPixmap;
        }
    }
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

void Widget::HandleLabels_Circle(const QPoint &pos)
{
    QString coordinate = Point2Str(pos);
    points.append(coordinate);
    QLabel* label = new QLabel("<h5><font size=3 color=red>" + coordinate + "</font></h5>", ui->label_big);
    coordiantes.append(label);
    label->setMouseTracking(true);
    label->adjustSize();
    label->move(pos);
    label->raise();
    label->show();

    if (points.size() == 4)
    {
        NoteLabels();
        points.clear();
        // Simulate right key pressing
        QApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));

        foreach(QLabel* label, coordiantes)
        {
            delete label;
        }
        coordiantes.clear();;
    }
}

void Widget::HandleLabels_Rect(const QPoint &pos)
{
    static bool draw = false;
}

void Widget::NoteLabels()
{
    //QString fileName = "coordiantes.txt";
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



