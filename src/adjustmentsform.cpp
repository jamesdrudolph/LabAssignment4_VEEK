#include "adjustmentsform.h"
#include "ui_adjustmentsform.h"

AdjustmentsForm::AdjustmentsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdjustmentsForm)
{
    ui->setupUi(this);
    initSocket();
}

AdjustmentsForm::~AdjustmentsForm()
{
    delete ui;
}

void AdjustmentsForm::displayImage() {
    orignalImage = QImage::fromData(imageData);
    adjustedImage = QImage::fromData(imageData);

    QPixmap pixmap = QPixmap::fromImage(orignalImage);
    baseScene = new QGraphicsScene();
    baseScene->addPixmap(pixmap);

    ui->graphicsView->setScene(baseScene);
}

void AdjustmentsForm::adjustBrigtness(int adjustFactor) {
    QVector<QRgb> colors = adjustedImage.colorTable();

    for (int i = 0; i < colors.length(); i++) {
        QColor color = colors[i];

        color.setRed(truncate(color.red() + adjustFactor));
        color.setGreen(truncate(color.green() + adjustFactor));
        color.setBlue(truncate(color.blue() + adjustFactor));

        colors[i] = color.rgb();
    }

    adjustedImage.setColorTable(colors);

    QPixmap qPixmap = QPixmap::fromImage(adjustedImage);
    delete baseScene;
    baseScene = new QGraphicsScene();
    baseScene->addPixmap(qPixmap);

    ui->graphicsView->setScene(baseScene);
}

void AdjustmentsForm::adjustContrast(int adjustFactor) {
    adjustFactor = (int)(2.55*(float)adjustFactor);
    std::vector<int> histogram(256,0);
    std::vector<float> cumulative_probability(256,0);
    std::vector<int> hist_equalized(256,0);

    for (int i = 0; i < orignalImage.height(); i++) {
        for (int j = 0; j < orignalImage.width(); j++) {
            histogram[orignalImage.pixelIndex(i, j)]++;
        }
    }

    for(unsigned int i = 0; i < 256; i++) {
        cumulative_probability[i] = ((float)histogram[i] / (float)(orignalImage.width() * orignalImage.height())) + (i > 0)*cumulative_probability[i-1];
        hist_equalized[i] = floor((adjustFactor-1)*cumulative_probability[i]);
    }

    QVector<QRgb> colors = orignalImage.colorTable();

    for(int i = 0; i < colors.length(); i++) {
        QColor color = colors[i];
        color.setRed(hist_equalized[color.red()]);
        color.setGreen(hist_equalized[color.green()]);
        color.setBlue(hist_equalized[color.blue()]);
        colors[i] = color.rgb();
    }

    adjustedImage.setColorTable(colors);

    QPixmap qPixmap = QPixmap::fromImage(adjustedImage);
    delete baseScene;
    baseScene = new QGraphicsScene();
    baseScene->addPixmap(qPixmap);

    ui->graphicsView->setScene(baseScene);
}

unsigned char AdjustmentsForm::truncate(int value) {
    if (value > 255) {
        return 255;
    } else if (value < 0) {
        return 0;
    }

    return value;
}

//refactor everything below this to a server class later for reuse in b/c
void AdjustmentsForm::initSocket() {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 420);

    connect(udpSocket, &QUdpSocket::readyRead, this, &AdjustmentsForm::readPendingDatagrams);
}

void AdjustmentsForm::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        processDatagram(datagram);
    }
}

void AdjustmentsForm::processDatagram(QByteArray datagram) {
    if (datagram.data() == QStringLiteral("START_IMAGE_SEND")) {
        qDebug() << "received START_IMAGE_SEND";
        imageData.clear();
        receivingImage = true;
    } else if (datagram.data() == QStringLiteral("END_IMAGE_SEND")) {
        qDebug() << "received END_IMAGE_SEND";
        displayImage();
        receivingImage = false;
    } else if (datagram.startsWith("BRIGHTNESS")) {
        qDebug() << "received BRIGHTNESS";
        brightnessAdjust = std::stoi(datagram.split(':')[1].data());
        adjustContrast(contrastAdjust);
        adjustBrigtness(brightnessAdjust);
    } else if (datagram.startsWith("CONTRAST")) {
        qDebug() << "received CONTRAST";
        contrastAdjust = std::stoi(datagram.split(':')[1].data());
        adjustContrast(contrastAdjust);
        adjustBrigtness(brightnessAdjust);
    }else if (receivingImage) {
        qDebug() << "received " << datagram.length() << " bytes of image data";
        imageData.append(datagram);
    }else {
        qDebug() << datagram;
    }
}
