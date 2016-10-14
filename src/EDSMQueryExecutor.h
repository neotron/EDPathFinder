//
//  Copyright (C) 2016  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
