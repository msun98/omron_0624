#ifndef WEBSOCKETPAGE_H
#define WEBSOCKETPAGE_H

#include <QWidget>
#include <QWebSocketServer>
#include <QWebSocket>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QTimer>

namespace Ui {
class WebsocketPage;
}

class WebsocketPage : public QWidget
{
    Q_OBJECT

public:
    explicit WebsocketPage(QWidget *parent = nullptr);
    ~WebsocketPage();


public slots:
    void onTimeout();

    void onNewConnection();
    void onClosed();

    void onTextMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray message);
    void onDisconnected();



private:
    Ui::WebsocketPage *ui;
    QTimer  *timer;

    QWebSocketServer *server;
    QList<QWebSocket *> clients;

    void sendNotice(QWebSocket *client_socket);
    void sendCommandAck(QWebSocket *client_socket, QString result, QJsonObject error_info, QString uuid);


};

#endif // WEBSOCKETPAGE_H
