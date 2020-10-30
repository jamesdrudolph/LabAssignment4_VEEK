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
    QPixmap pixmap = QPixmap::fromImage(QImage::fromData(imageData));

    baseScene = new QGraphicsScene();
    baseScene->addPixmap(pixmap);

    ui->graphicsView->setScene(baseScene);
}

void OverlayForm::displayOverlay() {
    QPixmap pixmapBase = QPixmap::fromImage(QImage::fromData(imageData));
    QImage overlay = QImage::fromData(overlayData);
    overlay = overlay.convertToFormat(QImage::Format_ARGB32);

    for (int i = 0; i < overlay.height(); i++) {
        for (int j = 0; j < overlay.width(); j++) {
            if (overlay.pixelColor(i, j).rgb() == 0xffffffff) {
                overlay.setPixel(i, j, 0x00000000);
            }
        }
    }

    QPixmap pixmapOverlay = QPixmap::fromImage(overlay);

    overlaidScene = new QGraphicsScene();
    overlaidScene->addPixmap(pixmapBase);
    overlaidScene->addPixmap(pixmapOverlay);

    ui->graphicsView->setScene(overlaidScene);
}

void OverlayForm::toggleOverlay() {
    if (overlaidScene != nullptr) {
        if (overlayShowing) {
            overlayShowing = false;
            ui->graphicsView->setScene(baseScene);
        } else {
            overlayShowing = true;
            ui->graphicsView->setScene(overlaidScene);
        }
    }
}

//refactor everything below this to a server class later for reuse in b/c
void OverlayForm::initSocket() {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 420);

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
    } else if (datagram.data() == QStringLiteral("START_OVERLAY_SEND")) {
        qDebug() << "received START_OVERLAY_SEND";
        overlayData.clear();
        receivingOverlay = true;
    } else if (datagram.data() == QStringLiteral("END_OVERLAY_SEND")) {
        qDebug() << "received END_OVERLAY_SEND";
        displayOverlay();
        receivingOverlay = false;
    } else if (datagram.data() == QStringLiteral("TOGGLE_OVERLAY")) {
        qDebug() << "received TOGGLE_OVERLAY";
        toggleOverlay();
    }else if (receivingImage) {
        qDebug() << "received " << datagram.length() << " bytes of image data";
        imageData.append(datagram);
    } else if (receivingOverlay) {
        qDebug() << "received " << datagram.length() << " bytes of overlay data";
        overlayData.append(datagram);
    } else {
        qDebug() << datagram;
    }
}
