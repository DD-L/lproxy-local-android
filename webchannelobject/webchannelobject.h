#ifndef WEBCHANNELOBJECT_H
#define WEBCHANNELOBJECT_H

#include <QObject>
#include <QGuiApplication>
#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocketServer>


class CommunObject : public QObject {
    Q_OBJECT
public:
    //explicit CommunObject(QObject* parent = 0) : QObject(parent) {}

    explicit CommunObject(QGuiApplication* app);
    virtual ~CommunObject();

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
    void load_json(const QString& id);
    void appQuit();
private:
    QGuiApplication* m_app;
    const QString m_local_json_file;
};

#endif // WEBCHANNELOBJECT_H
