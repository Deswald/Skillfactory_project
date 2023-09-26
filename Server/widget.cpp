#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets/QMessageBox>
#include <QTime>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->lineEdit_Port->setText("1234");
    ui->pushButton_Send->setEnabled(false);


    m_server = new QTcpServer();

    connect(m_server,&QTcpServer::newConnection,this,&Widget::server_New_Connect);

}

Widget::~Widget()
{
    m_server->close();
    m_server->deleteLater();
    delete ui;
}

 void Widget::server_New_Connect()
 {
    m_socket = m_server->nextPendingConnection();

    QObject::connect(m_socket,&QTcpSocket::readyRead,this,&Widget::socket_Recv_Data);
    QObject::connect(m_socket,&QTcpSocket::disconnected,this,&Widget::socket_Disconnect);

    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("Client connected %1\n").arg(QTime::currentTime().toString()));
    ui->pushButton_Send->setEnabled(true);

 }

void Widget:: socket_Recv_Data()
{
    QByteArray data_tmp;
    data_tmp = m_socket->readAll();
    if(!data_tmp.isEmpty())
    {
        QString str = QString(data_tmp);

        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser->append("From Client:  "+QTime::currentTime().toString());

         ui->textBrowser->setTextColor(Qt::black);
         ui->textBrowser->setCurrentFont(QFont("Times New Roman",16));
        ui->textBrowser->append(str);

        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(cursor);
    }
}

void Widget:: socket_Disconnect()
{
    ui->pushButton_Send->setEnabled(false);
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("Client disconnected %1\n").arg(QTime::currentTime().toString()));

}

void Widget::on_pushButton_Listen_clicked()
{
    if (ui->pushButton_Listen->text() ==QString("Listen"))
        {
        qint16 port = ui->lineEdit_Port->text().toInt();

        if(!m_server->listen(QHostAddress::Any, port))
        {
            QMessageBox::critical(this,"Error！",m_server->errorString(),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes );
            return;

        }
        ui->pushButton_Listen->setText("Cancel listening");

    }
    else
    {
        if(m_socket->state() == QAbstractSocket::ConnectedState)
        {
            m_socket->disconnectFromHost();
        }
        m_server->close();

        ui->pushButton_Listen->setText("Listen");
        ui->pushButton_Send->setEnabled(false);
    }
}

void Widget::on_pushButton_Send_clicked()
{
    if (ui->textEdit->toPlainText() == QString())
    {
        QMessageBox msgb;
        msgb.setText("Empty message");
        msgb.resize(60,40);
        msgb.exec();
        return;
    }
    m_socket->write(ui->textEdit->toPlainText().toUtf8());

    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append("From Server:  "+QTime::currentTime().toString());

     ui->textBrowser->setTextColor(Qt::black);
     ui->textBrowser->setCurrentFont(QFont("Times New Roman",16));
    ui->textBrowser->append(ui->textEdit->toPlainText().toUtf8());
    m_socket->flush();
    ui->textEdit->clear();


}
