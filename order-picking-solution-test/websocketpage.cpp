#include "websocketpage.h"
#include "ui_websocketpage.h"

WebsocketPage::WebsocketPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebsocketPage)
{
    ui->setupUi(this);

    server = new QWebSocketServer("rb_websocket", QWebSocketServer::NonSecureMode, this);
    if(server->listen(QHostAddress::Any, 28081)){
        connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
        connect(server, SIGNAL(closed()), this, SLOT(onClosed()));
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(100);
}

WebsocketPage::~WebsocketPage()
{
    server->close();
    qDeleteAll(clients.begin(), clients.end());
    delete ui;
}


void WebsocketPage::onTimeout(){
    static int cnt = 0;
    cnt++;

    if(clients.size() > 0){
        if(cnt%2 == 0){
            for(int i=0; i<clients.size(); i++){
                QWebSocket *pSocket = clients[i];
                sendNotice(pSocket);
            }
        }
    }
}

void WebsocketPage::onNewConnection(){
    QWebSocket *pSocket = server->nextPendingConnection();

    connect(pSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
    connect(pSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onBinaryMessageReceived(QByteArray)));
    connect(pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    clients << pSocket;

    QString status = "New Client Connected..";
    ui->te_msg->append(status);
    qDebug() << status;
}

void WebsocketPage::onClosed(){

}


void WebsocketPage::onTextMessageReceived(QString message){
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient){
        QJsonObject json;
        QJsonDocument doc_json = QJsonDocument::fromJson(message.toUtf8());
        json = doc_json.object();

        if(json["mst_type"] == "command"){
            // Command Msg
            QString entry = json["entry"].toString();
            QString action = json["do"].toString();
            QJsonObject params = json["params"].toObject();
            QString uuid = json["uuid"].toString();

            QJsonObject error_info;
            sendCommandAck(pClient, "success", error_info, uuid);
        }
    }
}

void WebsocketPage::onBinaryMessageReceived(QByteArray message){
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient){
        pClient->sendBinaryMessage(message);
    }
}

void WebsocketPage::onDisconnected(){
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient){
        clients.removeAll(pClient);
        pClient->deleteLater();
    }
}



void WebsocketPage::sendNotice(QWebSocket *client_socket){
    QJsonObject json;
    json["msg_type"] = "notice";
    json["robot_state"] = "ready";
    json["navi_mode"] = "navigate";
    QJsonObject json_robot;
    json_robot["x"] = 0.0;
    json_robot["y"] = 0.0;
    json_robot["theta"] = 0.0;
    json["robot_pos"] = json_robot;
    QJsonObject json_map;
    json_map["map_id"] = "5f4f2db7429b4a0012d40c53";
    json_map["map_alias"] = "1F";
    json["map"] = json_map;
    QJsonObject json_battery;
    json_battery["level"] = 90;
    json_battery["in_charging"] = false;
    json["battery"] = json_battery;

    QJsonDocument doc_json(json);
    //QString str_json(doc_json.toJson(QJsonDocument::Compact));
    QString str_json(doc_json.toJson(QJsonDocument::Indented));

    client_socket->sendTextMessage(str_json);
}


void WebsocketPage::sendCommandAck(QWebSocket *client_socket, QString result, QJsonObject error_info, QString uuid){
    QJsonObject json;
    json["msg_type"] = "ack";
    json["result"] = result;
    if(!error_info.empty()){
        json["error_info"] = error_info;
    }
    json["uuid"] = uuid;

    QJsonDocument doc_json(json);
    //QString str_json(doc_json.toJson(QJsonDocument::Compact));
    QString str_json(doc_json.toJson(QJsonDocument::Indented));

    client_socket->sendTextMessage(str_json);
}
