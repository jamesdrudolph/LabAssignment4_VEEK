#ifndef ADJUSTMENTSFORM_H
#define ADJUSTMENTSFORM_H

#include <QGraphicsScene>
#include <QUdpSocket>
#include <QWidget>

#include <cmath>

namespace Ui {
class AdjustmentsForm;
}

class AdjustmentsForm : public QWidget
{
    Q_OBJECT

public:
    explicit AdjustmentsForm(QWidget *parent = nullptr);
    ~AdjustmentsForm();

private:
    Ui::AdjustmentsForm *ui;
    QUdpSocket *udpSocket = nullptr;
    QGraphicsScene *baseScene = nullptr;
    QByteArray imageData;
    QImage orignalImage;
    QImage adjustedImage;
    bool receivingImage = false;
    int contrastAdjust = 0;
    int brightnessAdjust = 0;

    void initSocket();
    void readPendingDatagrams();
    void processDatagram(QByteArray datagram);
    void displayImage();
    unsigned char truncate(int value);
    void adjustBrigtness(int adjustFactor);
    void adjustContrast(int adjustFactor);
};

#endif // ADJUSTMENTSFORM_H
