#include "recvfileop.h"
#include "rsa.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <QFileDialog>
#include <QFile>
#include <sstream>

const quint16 PORT = 6666;
const int DATA_STREAM_VERSION = QDataStream::Qt_5_8;
QString path;

recvfileop::recvfileop(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle(tr("recvfile"));
    /* reset progressbar */
	ui.recvProg->setRange(0, 100);
	ui.recvProg->setValue(0);
	fileBytes = gotBytes = nameSize = 0;
	m_File = Q_NULLPTR;
    new_File = Q_NULLPTR;
	m_TcpSocket = Q_NULLPTR;
    m_pMyAesCBC1 = nullptr;
    key = new char[17];
    memset(key, 0, 17);
    int a[3] = {0,0,0};
    int *k = KeyGeneration(a);
    qDebug() << "e = " << k[0];
    qDebug() << "d = " << k[1];
    qDebug() << "n = " << k[2];
    e = k[0];
    d = k[1];
    n = k[2];
	m_TcpServer = new QTcpServer(this);
    /* connect request -> accept connection */
	connect(m_TcpServer, SIGNAL(newConnection()),
        this, SLOT(accept_connect()));

    if (!m_TcpServer->listen(QHostAddress::Any, PORT))
    {
        std::cerr << "*** Listen to Port Failed ***" << std::endl;
        qDebug() << m_TcpServer->errorString();
        //close();
        return;
    }
    ui.stLabel->setText(QString("Linking to Port %1").arg(PORT));
}

/*--- accept connection ---*/
void recvfileop::accept_connect()
{
    qDebug() << "receive_file 1";
	m_TcpSocket = m_TcpServer->nextPendingConnection();
    qDebug() << "receive_file 1";
	connect(m_TcpSocket, SIGNAL(readyRead()),
        this, SLOT(receive_file()));

    /* socket error -> error handling */
	connect(m_TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(show_error(QAbstractSocket::SocketError)));
	ui.stLabel->setText(QString("Connection Established!"));
	gotBytes = 0;
}

/*--- receive file ---*/
void recvfileop::receive_file()
{
    qDebug() << "receive_file 1 ";
	QDataStream in(m_TcpSocket);
	in.setVersion(DATA_STREAM_VERSION);

    if(gotBytes <= sizeof(qint64)*2){
        qDebug() << "check0";
        if((m_TcpSocket->bytesAvailable()>=sizeof(qint64)*2)&&(nameSize==0)){
            qDebug() << "check1";
            in >> fileBytes >> nameSize;
            gotBytes += 2 * sizeof(qint64);
        }
        if((m_TcpSocket->bytesAvailable()>=nameSize)&&(nameSize!=0)){
            qDebug() << "check2";

            in >> m_FileName;

//            in >> key;
            QString cip[16];
            for(int k=0;k<16;k++){
                in >> cip[k];
            }


            qDebug() << "cip0:" << cip[0];
            qDebug() << "cip15:" << cip[15];

            cipher = new int[16];
            for(int k=0;k<16;k++){
                cipher[k] = cip[k].toInt();
            }
            qDebug() << "cip0:" << cipher[0];
            for(int j=0;j<16;j++){
                key[j] = Decryption(cipher[j],d,n);
            }
            qDebug() << "key:" << key;
            qDebug() << "cipher:" << cipher[0];


            ui.stLabel->setText(QString("Open %1 Successfully!").arg(m_FileName));
            gotBytes += nameSize;       
            QString path1 = "C:/Users/n8300/Desktop/send/time.aes";
            m_File = new QFile(path+m_FileName);
            if (!m_File->open(QIODevice::WriteOnly)) // open failed
            {
                std::cerr << "*** File Open Failed ***" << std::endl;
                return;
            }
        }
        else return;
    }

    if(gotBytes < fileBytes)
    {
        qDebug() << "check";
        gotBytes += m_TcpSocket->bytesAvailable();
        m_File->write(m_TcpSocket->readAll());

    }

	qDebug() << "fileBytes = " << fileBytes << endl;
    qDebug() << "gotBytes = " << gotBytes << endl;
	ui.recvProg->setValue(gotBytes * 100 / fileBytes);
    qDebug() << "receive_file m_FileName = " << m_FileName << endl;

    if(gotBytes == fileBytes){
        qDebug() << "key:" << key;
        m_pMyAesCBC1 = new MyAesCBC(16, (unsigned char *)key);

        m_File->close(); // close file

        QFile file(path+m_FileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, "warning", "open file failed");
            return;
        }

        QByteArray enImage = file.readAll();
        int length = enImage.size();
        file.close();
        QByteArray deImage;
        int ii = m_pMyAesCBC1->OnAesUncrypt(enImage, length, deImage);
        Q_UNUSED(ii);
        QString FileName1= m_FileName.left(m_FileName.size() - 3);
        FileName1 = FileName1.append("txt");
        QFile rfile(path+FileName1);
        if (!rfile.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, "warning", "can't write deImage");
            return;
        }
        rfile.write(deImage);
        rfile.close();
        ui.stLabel->setText(QString("Finish receiving %1").arg(m_FileName));
    }

}


/*--- connect error ---*/
void recvfileop::show_error(QAbstractSocket::SocketError)
{
	std::cerr << "*** Socket Error ***" << std::endl;
	qDebug() << m_TcpSocket->errorString();
    m_TcpSocket->close(); // close cocket
	m_TcpSocket = Q_NULLPTR;
	m_File = Q_NULLPTR;
    m_FileName.clear(); // clear fileName
	fileBytes = gotBytes = nameSize = 0;
    ui.recvProg->reset(); // reset progressbar
	ui.stLabel->setText(QString("*** SOCKET ERROR"));
	close();
}

/*---input file save path ---*/
void recvfileop::on_pushButton_clicked()
{
    path = ui.savePath->text();
}
