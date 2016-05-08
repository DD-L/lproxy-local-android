#include "webchannelobject/webchannelobject.h"

#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocketServer>
#include <QClipboard>
#include <QDir>
#include <QFile>

QT_BEGIN_NAMESPACE

CommunObject::CommunObject(QGuiApplication* app)
        : QObject(nullptr), m_app(app),
#if defined(Q_OS_ANDROID)
            m_local_json_file("/mnt/sdcard/Android/data/lproxy/local.json")
#else
            m_local_json_file("config/local.json")
#endif
{
    QObject::connect(this, &CommunObject::appExit, app, &QGuiApplication::quit);
}
CommunObject::~CommunObject() {
    //qDebug() << "~CommunObject()";
    //delete m_engine;
}

void CommunObject::receiveStatus(const QString& text) {
    qDebug() << "receiveText: " << text;
    emit sendStatus(text);
}

//void CommunObject::receiveKey(const QString& key) {
//    qDebug() << "receiveKey: " << key;
//    // debug
//    emit sendStatus(key);
//    // do something
//    // ...
//}
//
//void CommunObject::receiveInfo2Encrypt(const QString& info) {
//    qDebug() << "receiveInfo2Encrypt: " << info;
//    // debug
//    emit sendStatus(info);
//
//    // do something
//    // ...
//
//    emit sendResults(info);
//}
//void CommunObject::receiveInfo2Decrypt(const QString& info) {
//    qDebug() << "receiveInfo2Decrypt: " << info;
//    // debug
//    emit sendStatus(info);
//
//    // do something
//    // ...
//
//
//    emit sendResults(info);
//}
//
//void CommunObject::copy2clipboard(const QString& results) {
//    qDebug() << "copy2clipboard: " << results;
//    //if (results == "") return false;
//
//    QClipboard* clipboard = QGuiApplication::clipboard();
//    clipboard->setText(results);
//    const QString originalText = clipboard->text();
//
//    // debug
//    emit sendStatus("Clipboard text: " + originalText);
//}
//
//void CommunObject::clearClipboard() {
//    QClipboard* clipboard = QGuiApplication::clipboard();
//
//    clipboard->clear();
//    /*
//     * Qt bug:          'Android: QClipboard::clear() not implemented'
//     * Bug description: 'This is done by calling setMimeData() with a null pointer,
//     *                   but when that happens in the Android plugin we just return
//     *                   without doing anything.'
//     */
//#if defined(Q_OS_ANDROID)
//    clipboard->setMimeData(nullptr);
//#endif
//}
//

void CommunObject::run(const QString& local_json, const QString& id) {
    // /mnt/sdcard/Android/data/lproxy/local1.json
    const QString local_json_file = m_local_json_file + id;
    {
    QFileInfo json_file_info(local_json_file);
    const QString path = json_file_info.absolutePath();
    QDir json_dir(path);
    if (! json_dir.exists()) {
        qDebug() << "mkdir " << path;
        bool ret = json_dir.mkdir(path);
        if (! ret) {
            qDebug() << "mkdir error";
            emit offConfig(id);
            return;
        }
    }
    }
    QFile json_file(local_json_file);
    if (!json_file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "Write: open file error";
        emit offConfig(id);
        return;
    }
    QTextStream out(&json_file);
    out << local_json;
    json_file.flush();
    json_file.close();


}

void CommunObject::load_json(const QString &id) {
    // read local.json1
    const QString local_json_file = m_local_json_file + id;
    QFile json_file(local_json_file);
    if (! json_file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Read: open file error";
        emit offConfig(id);
        return;
    }
    QTextStream in(&json_file);
    QString json = QString(in.readAll());
    json_file.close();

    // 发送给 html
    emit sendJson(json, id);
}

void CommunObject::appQuit() {
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    //emit appExit();
    //QGuiApplication::exit(0);
    qApp->exit(0);
#else
    // desktop using QtWebKit
    // Force Quit the Application regardless of memory leaks
    exit(0); /* LEAK: 1 WebPageProxy; 1 WebContext */

    // In QtWebKit environment, if the program is executed one of these statements:
    // 'emit appExit()', 'QGuiApplication::exit(0)', 'qApp->exit(0)'
    // WITHOUT main.cpp:Force_Quit_regardless_of_memory_leaks(),
    // then an ERROR occur: 'pure virtual method called'
#endif
}

QT_END_NAMESPACE
