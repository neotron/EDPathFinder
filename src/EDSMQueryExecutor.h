//
// Created by David Hedbor on 10/13/16.
//

#ifndef ELITETSP_EDSMQUERY_H
#define ELITETSP_EDSMQUERY_H
#include <QUrl>
#include <QThread>
class QNetworkReply;
class QNetworkAccessManager;
class EDSMQueryExecutor : public QThread {
    Q_OBJECT

public:

    static EDSMQueryExecutor *systemCoordinateRequest(const QString &systemName);

    virtual ~EDSMQueryExecutor() override;
protected:
    virtual void run() override;

signals:
    void coordinatesReceived(double x, double y, double z);
    void coordinateRequestFailed();
public slots:
    void replyFinished(QNetworkReply *reply);

private:
    enum RequestType {
        Coordinates
    };

    explicit EDSMQueryExecutor(const QUrl &url, RequestType requestType);

    QNetworkAccessManager *_mgr;
    RequestType _requestType;
    const QUrl _url;
};


#endif //ELITETSP_EDSMQUERY_H
