#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QMap>

#include "../IoT/IoT_global.h"

class IOT_EXPORT Jitterbit : public QObject
{
	Q_OBJECT

public:

	Jitterbit(QObject* parent, QString url, QString data, QMap<QString, QPair<QString, QString>> hdwconfigmap, int timeoutms = 10000);
	bool post(QString &errorMsg);
	void convertEzXmlToJbXml(QString ezspecHdwXml, QMap<QString, QPair<QString, QString>> map);
	QString getJbXml();
	void setPostData(QString data);

private:

	QNetworkRequest m_request;
	QUrl m_url;
	int m_timeoutms;
	QByteArray m_postData;

	const QMap<QString, QPair<QString, QString>> m_HdwConfigMap;
	void generateJbXmlByRecursion(const QDomNode& node, const QMap<QString, QVector<QPair<QString, QString>>> map, QString &xml);
	QMap<QString, QVector<QPair<QString, QString>>> reverseEzspecMap(QMap<QString, QPair<QString, QString>> map);

	QString m_jbXml;
};