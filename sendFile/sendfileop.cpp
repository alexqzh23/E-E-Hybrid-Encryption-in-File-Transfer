#include "sendfileop.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ctype.h>
#include <time.h>
#include <iostream>
#include <sstream>
using namespace std;

#pragma execution_character_set("utf-8")
const quint16 PORT = 6666;			/*TCP port*/
qint64 LOADBYTES = 1* 1024; // every time send 1 kilo-byte(can be changed)
const int DATA_STREAM_VERSION = QDataStream::Qt_5_8;
QString IP;
int e, n;

/* Initialize & main functions */
sendfileop::sendfileop(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("send file"));
    ui.stLabel->setText(tr("please Select the file "));

    /* Initialize socket, Initialize sending file*/
    m_TcpSocket = new QTcpSocket(this);
    fileBytes = sentBytes = restBytes = 0;
    loadBytes = LOADBYTES;
    m_File = Q_NULLPTR;
    O_File =  Q_NULLPTR;
    m_pMyAesCBC1 = nullptr;

    ui.sendProg->setRange(0, 100);
    ui.sendProg->setValue(0);				// reset progressbar
    ui.sendBtn->setEnabled(false);
    ui.cancelBtn->setEnabled(false);

    /* connected -> start sending(send header) */
    connect(m_TcpSocket, SIGNAL(connected()),
        this, SLOT(start_transfer()));
    /* connected -> continue sending */
    connect(m_TcpSocket, SIGNAL(bytesWritten(qint64))
        , this, SLOT(continue_transfer(qint64)));
    /* socket error -> error handling */
    connect(m_TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(show_error(QAbstractSocket::SocketError)));
}


void sendfileop::start_transfer()
{
    const char *c = "0123456789abcdefghijklmnopqrstuvwxyz";
    key = new char[17];
    memset(key, 0, 17);
    for (int j = 0; j < 16;j++)
    {
        int index = rand() % strlen(c);
        key[j] = c[index];
    }

    cipher = new int[16];
    for (int j = 0; j < 16;j++){
        int value = toascii(key[j]);
        cipher[j] = Encryption(value, 2997, 20131);
    }

    m_pMyAesCBC1 = new MyAesCBC(16, (unsigned char *)key);
    qDebug() << "keysize:" << strlen(key);
    qDebug() << "key:" << key;
    qDebug() << "cipher:" << cipher[0];
    QString cip[16];
    for(int k=0;k<16;k++){
        cip[k] = QString::number(cipher[k],10);

    }
    qDebug() << "cip0:" << cip[0];
    qDebug() << "cip15:" << cip[15];

    QString path1 = "C:/Users/n8300/Desktop/send/time.mp4";

    // Read the data to be encrypted
    QFile file1(m_strFileName);
    if (!file1.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "warning", "open file failed");
        return;
    }

    QByteArray srcImage = file1.readAll();
    int length1 = srcImage.size();
    file1.close();


    QByteArray enImage1;
    int i = m_pMyAesCBC1->OnAesEncrypt(srcImage, length1, enImage1);

    QString FileName1= m_strFileName.left(m_strFileName.size() - 3);
    FileName1 = FileName1.append("aes");

    QFile wfile1(FileName1);
    if (!wfile1.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "warning", "can't write enImge");
        return;
    }
    wfile1.write(enImage1);
    wfile1.close();
    QMessageBox::warning(this, "hint", "write engine into "+m_strFileName);

    m_strFileName = FileName1;

    m_File = new QFile(FileName1);

    if (!m_File->open(QFile::ReadOnly))
    {
        ui.stLabel->setText(QString("*** FILE OPEN ERROR"));
        qDebug() << "*** start_transfer(): File-Open-Error";
        return;
    }
    fileBytes = m_File->size();
    ui.sendProg->setValue(0);
    ui.stLabel->setText(QString("Connection Established!"));

    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    qDebug() << "1" << endl;
    out.setVersion(DATA_STREAM_VERSION);

    /* fileName */
    QString sfName = m_strFileName.right(m_strFileName.size() -
        m_strFileName.lastIndexOf('/') - 1);

    /* header = total size + fileName length + fileName */
    out << qint64(0) << qint64(0) << sfName;
    for(int k=0;k<16;k++){
        out <<cip[k];
    }
    qDebug() << "sfName" << sfName<<endl;

    /* total size + the size of header */
    fileBytes += buf.size();

    /* Rewrite the first two length fields of header */
    out.device()->seek(0);
    qDebug() << "fileName length" << (qint64(buf.size()) - 2 * sizeof(qint64)) << endl;
    out << fileBytes << (qint64(buf.size()) - 2 * sizeof(qint64));
    qDebug() << "fileBytes" << fileBytes<< endl;
    qDebug() << "fileBytes" << (qint64(buf.size()) - 2 * sizeof(qint64)) << endl;

    /* Send header, calculate remaining size */
    restBytes = fileBytes - m_TcpSocket->write(buf);
    buf.resize(0);
    qDebug() << "restBytes" << restBytes<<endl;

}

/*--- continue sending ---*/
void sendfileop::continue_transfer(qint64 sentSize)
{
    int bufsize = 0;
    qDebug() << "continue_transfer" << endl;
    sentBytes += sentSize;

    /* sending not finish */
    if (restBytes > 0)
    {
        /* read data from file */
        QByteArray buf = m_File->read(qMin(loadBytes, restBytes));
        /* send */
        bufsize = m_TcpSocket->write(buf);
        qDebug() << "bufsize" << bufsize << endl;
        restBytes -= bufsize;
        ui.sendProg->setValue((fileBytes - restBytes) * 100 / fileBytes);
        buf.resize(0);
        qDebug() << "restBytes" << restBytes << endl;
        //m_File->flush();
    }
    else{
        m_File->close();
    }
    qDebug() << "fileBytes" << fileBytes << endl;

    /* finish sending */
    if (0==restBytes)
    {
        //m_TcpSocket->close(); // close socket
        ui.sendProg->setValue((fileBytes - restBytes) * 100 / fileBytes);
        m_strFileName.clear(); // clear file
        ui.stLabel->setText(QString("Finish sending!"));
        QMessageBox::warning(this, tr("OK"), QString("Finish sending!"));
    }
    else ui.sendProg->setValue((fileBytes - restBytes) * 100 / fileBytes);
}

/*--- connect error ---*/
void sendfileop::show_error(QAbstractSocket::SocketError)
{
    qDebug() << "*** Socket Error";
    m_TcpSocket->close();
    ui.stLabel->setText(QString("*** SOCKET ERROR, RESEND LATER"));
    ui.sendBtn->setEnabled(true);
    ui.sendProg->reset(); // reset progressbar
    m_strFileName.clear();
}

/*--- select file ---*/
void sendfileop::on_selectBtn_clicked()
{
    QDir dir;
    dir.setPath("/storage/emulated/0");
    m_strFileName = QFileDialog::getOpenFileName(this, tr("Open file"), dir.absolutePath());

    if (!m_strFileName.isEmpty())
    {
        ui.stLabel->setText(
            QString("File %1 Opened!").arg(m_strFileName));
        ui.sendBtn->setEnabled(true);
    }
    else
        ui.stLabel->setText(QString("*** FAIL OPENING FILE"));    
}

/*--- connect ---*/
void sendfileop::on_sendBtn_clicked()
{
        /* send connect request */
        m_bStateFlag = true;
        m_TcpSocket->setProxy(QNetworkProxy::NoProxy);
        m_TcpSocket->connectToHost(QHostAddress(IP), PORT);

        sentBytes = 0;
        ui.sendBtn->setEnabled(false);
    //	ui.sendBtn->setText(tr("pause"));
        ui.cancelBtn->setEnabled(true);
        ui.stLabel->setText(QString("Linking..."));
}

/*--- pause & continue ---*/
void sendfileop::on_pauseBtn_clicked()
{
    if (m_bStateFlag) {
        m_bStateFlag = false;
        disconnect(m_TcpSocket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(continue_transfer(qint64)));

        ui.stLabel->setText(QString("pause..."));
    }
    else {
        m_bStateFlag = true;
        connect(m_TcpSocket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(continue_transfer(qint64)));

        int bufsize = 0;
        QByteArray buf = m_File->read(qMin(loadBytes, restBytes));
        bufsize = m_TcpSocket->write(buf);
        qDebug() << "bufsize" << bufsize << endl;
        restBytes -= bufsize;
        ui.sendProg->setValue((fileBytes - restBytes) * 100 / fileBytes);
        buf.resize(0);
        qDebug() << "restBytes" << restBytes << endl;
        qDebug() << "****continue";
        ui.stLabel->setText(QString("continue..."));
    }
}

/*--- exit ---*/
void sendfileop::on_cancelBtn_clicked()
{
    close();
}

/*--- input IP & define send speed ---*/
void sendfileop::on_pushButton_clicked()
{
    IP = ui.IPinput->text();
    //IP="localhost";
    int num = 1024;
    loadBytes = num * ui.speedInput->text().toFloat();
}
