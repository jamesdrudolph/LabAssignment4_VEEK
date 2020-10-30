#ifndef OVERLAYFORM_H
#define OVERLAYFORM_H

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

private:
    Ui::OverlayForm *ui;
    QUdpSocket *udpSocket;
    QByteArray imageData;
    bool receivingImage = false;
    void initSocket();
    void readPendingDatagrams();
    void processDatagram(QByteArray datagram);
    void displayImage();
};

#endif // OVERLAYFORM_H
