#ifndef WEBCHANNELOBJECT_H
#define WEBCHANNELOBJECT_H

#include <QObject>
#include <QGuiApplication>
#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocketServer>
#include <vector>
#include <lss/lss_server.h>
#include <boost/thread.hpp> // C++11 中 std::thread 未提供中断点接口


class CommunObject : public QObject {
    Q_OBJECT
public:
    //explicit CommunObject(QObject* parent = 0) : QObject(parent) {}

    explicit CommunObject(QGuiApplication* app);
    virtual ~CommunObject();

    void sendlog(const QString& log);
    void sendlog(const std::string& log);
    // 并非单例，只是充当全局变量而已
    static CommunObject* get_instance(void) {
        return m_this;
    }

public: signals:
    /*!
        This signal is emitted from the C++ side and the text displayed on the HTML client side.
    */
    void sendStatus(const QString& text);
    void sendResults(const QString& text);
    void appExit();
    void sendJson(const QString& json, const QString& id);
    void offConfig(const QString& id);
public slots:
    /*!
        This slot is invoked from the HTML client side and the text displayed on the server side.
    */
    void receiveStatus(const QString& text);
    //void receiveKey(const QString& key);
    //void receiveInfo2Encrypt(const QString& info);
    //void receiveInfo2Decrypt(const QString& info);
    //void copy2clipboard(const QString& results);
    //void clearClipboard();
    void run(const QString& local_json, const QString& id);
    void stop(const QString &id);
    void load_json(const QString& id);
    void appQuit();
private:
    // 线程函数
    void localst(const QString &id);
private:
    QGuiApplication* m_app;
    const QString m_local_json_file;
private:
    //struct __locals_thread {
    //    std::shared_ptr<lproxy::local::lss_server> s;
    //    std::shared_ptr<std::thread> t;
    //};
    //std::vector<__locals_thread> m_locals;
    std::shared_ptr<lproxy::local::lss_server> m_locals;
    std::thread                                m_localt; // 目前不允许多个配置同时运行
    boost::thread                              m_logt;   // 同上
    static CommunObject*                       m_this;
};

#endif // WEBCHANNELOBJECT_H
