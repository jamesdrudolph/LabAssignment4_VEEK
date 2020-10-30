#ifndef OVERLAYFORM_H
#define OVERLAYFORM_H

#include <QGraphicsScene>
#include <QUdpSocket>
#include <QWidget>

namespace Ui {
class OverlayForm;
}

class OverlayForm : public QWidget
{
    Q_OBJECT

public:
    explicit OverlayForm(QWidget *parent = nullptr);
    ~OverlayForm();

private slots:

private:
    Ui::OverlayForm *ui;
    QUdpSocket *udpSocket = nullptr;
    QGraphicsScene *baseScene = nullptr;
    QGraphicsScene *overlaidScene = nullptr;
    QByteArray imageData;
    QByteArray overlayData;
    bool overlayShowing =  false;
    bool receivingImage = false;
    bool receivingOverlay = false;
    void initSocket();
    void readPendingDatagrams();
    void processDatagram(QByteArray datagram);
    void displayImage();
    void toggleOverlay();
    void displayOverlay();
};

#endif // OVERLAYFORM_H
