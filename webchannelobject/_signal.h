#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <QObject>
// Qt 5.5.1 版本的 QWebChannel 不允许跨线程发送信号, 所以采取迂回方式发送信号
class _Signal : public QObject {
    Q_OBJECT
public: signals:
    void send_message(const QString& text);
    void brigde(const QString& text);
};

#endif // _SIGNAL_H
