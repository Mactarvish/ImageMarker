#include "ImageMarker.h"
#include "ui_ImageMarker.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->label_image->setMouseTracking(true);
    ui->label_image->installEventFilter(this);

    //image = new QPixmap("/home/hdl2/Desktop/Sono_nofolder/158_HAOTINGTING_1_Labeled.jpg");
    painter = new QPainter();
    points.resize(0);
    coordinates.resize(0);
    database = new iVConfDB("SonoDataset");

    //fileNames_ap = new QList<QString>();
    //imageFolderDirectory = QDir("/home/hdl2/Desktop/SonoDataset/Images");
    SelectDirectory(QDir("/home/hdl2/Desktop/SonoDataset/Images"));

//    QDir root = QDir(imageFolderDirectory);
//    QFileInfoList list = root.entryInfoList();
//    for (int i = 2; i < list.length(); i++)
//    {
//        //qDebug() << list[i].fileName();
//        if (list[i].fileName().contains(".jpg"))
//        {
//            //qDebug() << (*fileNames_ap)[i - 2];
//            int currentImageNum = list[i].fileName().split(".")[0].toInt();
//            qDebug() << currentImageNum;
//            //fileNames_ap->append(root.absolutePath() + "/" + list[i].fileName());
//            fileNames_ap[currentImageNum] = root.absolutePath() + "/" + list[i].fileName();
//        }
//    }
//    for (int i = 0; i < fileNames_ap.size(); i++)
//    {
//        qDebug() << fileNames_ap[i];
//    }
//    qDebug() << fileNames_ap.size();

    InitStatus();
//    currentImageIndex = 0;
//    ChangeImages(0);
//    currentPixmap = QPixmap(*(ui->label_image->pixmap()));
//    ui->lineEdit->setText((fileNames_ap)[0]);
    /*
    ui->label_image->setPixmap(image->copy(279, 115, 815, 597));
    ui->label_image->resize(image->size());

    QPixmap small_image = image->scaled(image->width() / 5, image->height() / 5);
    ui->label_small->setPixmap(small_image);
    ui->label_small->resize(small_image.size());
    */
    //QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg *.png)"));
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
    if (event->key() == Qt::Key_Right | Qt::Key_Left)
    {
        if (event->key() == Qt::Key_Right)
        {
            //qDebug() << fileNames_ap.size();
            if (currentImageIndex == fileNames_ap.size() - 1)
            {
                currentImageIndex = 0;
            }
            else
            {
                currentImageIndex++;
            }
        }
        if (event->key() == Qt::Key_Left)
        {
            if (currentImageIndex == 0)
            {
                currentImageIndex = fileNames_ap.size() - 1;
            }
            else
            {
                currentImageIndex--;
            }
            qDebug() << currentImageIndex;
        }
        ChangeImages(currentImageIndex);
        currentPixmap = QPixmap(*(ui->label_image->pixmap()));
        QSqlRecord record;
        database->GetLabel(QString::number(currentImageIndex), record);
        qDebug() << record;
        ui->lineEdit_P1->setText(record.value(3).toString());
    }
    qDebug() << currentImageIndex;
    if (event->key() == Qt::Key_Enter)
    {
        qDebug() << "Warning: Enter event is unsetting.";
        //imageChanged = true;
    }

    ui->lineEdit->setText((fileNames_ap)[currentImageIndex]);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << event->pos();
}

bool Widget::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->label_image)
    {
        static bool drawFinish = true;
        static QPoint beginPoint = QPoint();
        QPixmap drawedPixmap = currentPixmap;
        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (!drawFinish && rectTrace == true)
            {
                //qDebug() << "Keep drawing...";
                painter->begin(&drawedPixmap);
                painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
                painter->drawRect(QRect(beginPoint, mouseEvent->pos()));
                painter->end();
            }
            ui->label_image->setPixmap(drawedPixmap);
        }
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            drawFinish = !drawFinish;
            if (!drawFinish)
            {
                //drawedPixmap = QPixmap(*(ui->label_image->pixmap()));
                beginPoint = mouseEvent->pos();
            }
            // If all points in current image have been noted, change to next image.
            if (HandleLabels_Circle(mouseEvent->pos(), 4))
            {
                // Simulate right key pressing
                QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));
            }
            //HandleLabels_Rect(mouseEvent->pos());
            //delete currentPixmap;
            //currentPixmap = new QPixmap(*(ui->label_image->pixmap()));
            //currentPixmap = ui->label_image->pixmap();
            //qDebug() << currentPixmap;
        }
    }
    return QWidget::eventFilter(target, event);
}

void Widget::InitStatus()
{
    rectTrace = false;
    currentImageIndex = 0;
    ChangeImages(0);
    currentPixmap = QPixmap(*(ui->label_image->pixmap()));
    ui->lineEdit->setText((fileNames_ap)[0]);
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
    QPixmap image = QPixmap(fileNames_ap[index]);
    ui->label_image->setPixmap(image);
    ui->label_image->resize(image.size());
}

// If numPos has been arrived, return true, otherwise return false.
bool Widget::HandleLabels_Circle(const QPoint &pos, const int& numPos)
{
    QString coordinate = Point2Str(pos);
    points.append(pos);
    //points.append(pos);

    QLabel* label = new QLabel("<h5><font size=2 color=red>" + coordinate + "</font></h5>", ui->label_image);
    coordinates.append(label);
    label->setMouseTracking(true);
    label->adjustSize();
    label->move(pos);
    label->raise();
    label->show();

    if (points.size() == numPos)
    {
        NotePoints();
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

void Widget::NotePoints()
{
//    QString str_points = "";
//    for (int i = 0; i < points.size(); i++)
//    {
//        if (i == points.size() - 1)
//        {
//            str_points.append(points[i]);
//        }
//        else
//        {
//            str_points.append(points[i] + " ");
//        }
//    }
//    qDebug() << "Points string is" << str_points;
    QVector<int> coors;
    for (int i = 0; i < points.size(); i++)
    {
        coors.append(points[i].x());
        coors.append(points[i].y());
    }
    database->AddLabel(QString::number(currentImageIndex), coors);

    //MarkCurrentImage("points.txt", str_points);
}

void Widget::MarkCurrentImage(const QString &fileName, const QString &label)
{
//    //QString fileName = "coordinates.txt";
//    QString fileName_ap = labelFolderDirectory.absoluteFilePath(fileName);
//    qDebug() << fileName_ap;
//    QFile file(fileName_ap);
//    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
//    QTextStream in(&file);
//    QString imageName = ui->lineEdit->text().split("/").last();

//    in << imageName << " " << label << "\n";
//    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << label << imageName;
//    //in << label << " ";
//    file.flush();
//    file.close();

    QVector<int> coors;
    for (int i = 0; i < points.size(); i++)
    {
        coors.append(points[i].x());
        coors.append(points[i].y());
    }
    database->AddLabel(QString::number(currentImageIndex), coors);
}

QString Widget::Point2Str(const QPoint &pos)
{
    return QString("(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")");
}

// All images in the folder must be named by 0~size-1 with format jpg.
void Widget::SelectDirectory(QDir direct)
{
    imageFolderDirectory = direct;
    direct.cd("../Labels");
    labelFolderDirectory = direct;
    qDebug() << "current labelFolderDirectory is" << labelFolderDirectory;
    qDebug() << "current imageFolderDirectory is" << imageFolderDirectory;
    //imageFolderDirectory = QString("small_images/");
    QFileInfoList list = imageFolderDirectory.entryInfoList();

    QStringList jpgNames;
    for (int i = 2; i < list.length(); i++)
    {
        //qDebug() << list[i].fileName();
        if (list[i].fileName().contains(".jpg"))
        {
            jpgNames.append(list[i].fileName());
        }
    }
    for (int i = 0; i < jpgNames.size(); i++)
    {
        bool ok = true;
        int currentImageNum = jpgNames[i].split(".")[0].toInt(&ok);
        //qDebug() << "currentImageNum is" << currentImageNum << "ok is" << ok;
        assert(ok);
        fileNames_ap[currentImageNum] = imageFolderDirectory.absoluteFilePath(jpgNames[i]);
        assert(fileNames_ap.contains(0));
    }

//    for (int i = 2; i < list.length(); i++)
//    {
//        //qDebug() << list[i].fileName();
//        if (list[i].fileName().contains(".jpg"))
//        {
//            //qDebug() << (*fileNames_ap)[i - 2];
//            bool ok = true;
//            int currentImageNum = list[i].fileName().split(".")[0].toInt(&ok);
//            qDebug() << "currentImageNum is" << currentImageNum << "ok is" << ok;
//            //assert(ok);
//            //fileNames_ap->append(root.absolutePath() + "/" + list[i].fileName());
//            fileNames_ap[currentImageNum] = root.absolutePath() + "/" + list[i].fileName();
//        }
//    }


//    for (int i = 0; i < fileNames_ap.size(); i++)
//    {
//        qDebug() << fileNames_ap[i];
//        qDebug() << fileNames_ap.size() << i;
//    }
//    qDebug() << fileNames_ap.size();
}

void Widget::MarkCircle()
{

}

void Widget::MarkLine()
{

}

void Widget::MarkOther()
{

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




void Widget::on_button_open_clicked()
{
    imageFolderDirectory = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "/home/hdl2/Desktop/SonoDataset/");
    //qDebug() << imageFolderDirectory;
    SelectDirectory(imageFolderDirectory);
    InitStatus();
}

void Widget::on_button_circle_clicked()
{
//    //QString fileName = "coordinates.txt";
//    QString fileName = "/home/hdl2/Desktop/SonoDataset/Labels/classify.txt";
//    QFile file(fileName);
//    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
//    QTextStream in(&file);

//    in << "\n" << ui->lineEdit->text().split("/").last() << " ";
//    in << "circle";
//    file.flush();
//    file.close();
    MarkCurrentImage("classify.txt", "circle");
    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));
}

void Widget::on_button_line_clicked()
{
//    //QString fileName = "coordinates.txt";
//    QString fileName = "/home/hdl2/Desktop/SonoDataset/Labels/classify.txt";
//    QFile file(fileName);
//    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
//    QTextStream in(&file);

//    in << "\n" << ui->lineEdit->text().split("/").last() << " ";
//    in << "line";
//    file.flush();
//    file.close();
    MarkCurrentImage("classify.txt", "line");
    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));
}

void Widget::on_button_other_clicked()
{
//    //QString fileName = "coordinates.txt";
//    QString fileName = "/home/hdl2/Desktop/SonoDataset/Labels/classify.txt";
//    QFile file(fileName);
//    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
//    QTextStream in(&file);

//    in << "\n" << ui->lineEdit->text().split("/").last() << " ";
//    in << "other";
//    file.flush();
//    file.close();
    MarkCurrentImage("classify.txt", "other");
    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0));
}
