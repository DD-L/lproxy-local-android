#include "webchannelobject/webchannelobject.h"

#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocketServer>
#include <QClipboard>
#include <QDir>
#include <QFile>

#include <thread>
#include <system_error>

#include "local/log.h"
#include "lss/config_local.h"

QT_BEGIN_NAMESPACE

CommunObject* CommunObject::m_this = nullptr;

CommunObject::CommunObject(QGuiApplication* app)
        : QObject(nullptr), m_app(app),
#if defined(Q_OS_ANDROID)
            m_local_json_file("/mnt/sdcard/Android/data/lproxy/local.json")
#else
            m_local_json_file("config/local.json")
#endif
{
    m_locals = std::make_shared<lproxy::local::lss_server>(m_ios_left, m_ios_right);
    assert(m_locals);
    QObject::connect(this, &CommunObject::appExit, app, &QGuiApplication::quit);
    m_this = this; // 线程不安全的
}
CommunObject::~CommunObject() {
    //qDebug() << "~CommunObject()";
    //delete m_engine;

    /*
    if (m_logt) {
        try {
        m_logt->interrupt();
        }
        catch(const std::system_error& e) {
            qDebug() << e.what() << " in CommunObject::~CommunObject";
        }
    }
    */
}

void CommunObject::sendlog(const QString &log) {
    emit sendStatus(log);
}
void CommunObject::sendlog(const std::string &log) {
    sendlog(QString::fromStdString(log));
}

void CommunObject::receiveStatus(const QString& text) {
    qDebug() << "receiveText: " << text;
    emit sendStatus(text);
}

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

    // 加载配置文件
    lproxy::local::config::get_instance().configure(local_json_file.toStdString());

    // 启动日志输出线程
    // 目前不支持多个日志输出实例同时进行
    //boost::thread logt(std::bind(lproxy::mobile::log::output_thread, ""));
    //m_logt = std::move(logt);

    if (m_logt) {
        try {
            if (m_logt->joinable())
                m_logt->join();
            else
                m_logt->detach();
        }
        catch (std::system_error&) {}
        catch (...) {}
    }
    m_logt = std::make_shared<boost::thread>(std::bind(lproxy::mobile::log::output_thread, ""));
    //m_logt->detach();

    if (m_localt) {
        try {
            if (m_localt->joinable())
                m_localt->join();
            else
                m_localt->detach();
        }
        catch (std::system_error&) {}
        catch (...) {}
    }
    m_localt = std::make_shared<std::thread>(std::bind(&CommunObject::localst, this, id));
    m_localt->detach();
}

void CommunObject::stop(const QString &id) {
    (void)id;

    try {
        if (m_locals) {
            if (! m_locals->stopped()) {
                m_locals->stop();
            }

            while (! m_locals->stopped()) {
               std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }

            //if (m_localt) {
            //    //m_localt->detach();
            //    //m_localt->join();
            //}
        }
        if (m_logt) {
            //loginfo("log_output interrupt...");
            //loginfo("log_output interrupt...");
            //loginfo("log_output interrupt...");
            //loginfo("log_output interrupt...");
            //loginfo("log_output interrupt...");
            //loginfo("log_output interrupt...");
            m_logt->interrupt();
            //loginfo("log_output interrupt...");
            //m_logt->detach();
            m_logt->join();
        }
    }
    catch (const std::system_error& e) {
        qDebug() << e.what() << " CommunObject::stop";
    }
}

void CommunObject::localst(const QString& id) {
    (void)id;
    auto& bind_addr = lproxy::local::config::get_instance().get_bind_addr();
    uint16_t bind_port = lproxy::local::config::get_instance().get_bind_port();
    //启动 lss_server
    if (m_locals) {
        while (! m_locals->stopped()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        //delete m_locals;
        //m_locals = nullptr;
    }
    m_locals->run(bind_addr, bind_port);
    //
    if (m_locals && m_locals->stopped()) {
        //emit offConfig(id);
    }
    qDebug() << "localst exit..";
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
