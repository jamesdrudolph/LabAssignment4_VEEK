#include "overlayform.h"
#include "ui_overlayform.h"

OverlayForm::OverlayForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverlayForm)
{
    ui->setupUi(this);
    initSocket();
}

OverlayForm::~OverlayForm()
{
    delete ui;
}

void OverlayForm::displayImage() {
    QGraphicsScene *scene = new QGraphicsScene();
    QPixmap pixmap = QPixmap::fromImage(QImage::fromData(imageData));

    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);
}

//refactor everything below this to a server class later for reuse in b/c
void OverlayForm::initSocket() {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::AnyIPv4, 420);

    connect(udpSocket, &QUdpSocket::readyRead, this, &OverlayForm::readPendingDatagrams);
}

void OverlayForm::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        processDatagram(datagram);
    }
}

void OverlayForm::processDatagram(QByteArray datagram) {
    if (datagram.data() == QStringLiteral("START_IMAGE_SEND")) {
        qDebug() << "received START_IMAGE_SEND";
        imageData.clear();
        receivingImage = true;
    } else if (datagram.data() == QStringLiteral("END_IMAGE_SEND")) {
        qDebug() << "received END_IMAGE_SEND";
        displayImage();
        receivingImage = false;
    } else if (receivingImage) {
        qDebug() << "received " << datagram.length() << " bytes of image data";
        imageData.append(datagram);
        return;
    }
}
