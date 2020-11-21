#include "Jitterbit.h"

/*
 *Object used to convert hardware configuration file to xml file recognized by middleware.
 *
 *Object takes in a QMap of <QString -> QPair>, with QPair being a pair of QStrings.
 *Intention being able to map a singular tag from hardware configuration with attributes to a vector of attributes.
 */
Jitterbit::Jitterbit(QObject* parent, QString url, QString data, QMap<QString, QPair<QString, QString>> hdwconfigmap, int timeoutms)
	:QObject(parent)
	, m_request(QNetworkRequest())
	, m_timeoutms(timeoutms)
	, m_HdwConfigMap(hdwconfigmap)
{
	m_url.setUrl(url);
	m_request.setUrl(m_url);
	m_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/raw");
	m_postData = data.toUtf8();
}

/*
 *post method to post converted data to specific web API
 */
bool Jitterbit::post(QString &errorMsg)
{
	QElapsedTimer timer;
	timer.start();

	QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager(this);
	QNetworkReply *reply = networkAccessManager->post(m_request, m_postData);

	int response = 0;

	while (timer.elapsed() < m_timeoutms)
	{
		QCoreApplication::processEvents();

		if (reply->isFinished())
		{
			response = 1;
			break;
		}

		if (reply->error() != QNetworkReply::NoError)
		{
			response = 2;
			break;
		}
	}

	switch (response)
	{
	case 0:
		errorMsg = "Timed out";
		return false;
	case 1:
		errorMsg = reply->readAll();
		return true;
	case 2:
		errorMsg = reply->errorString();
		return false;
	}
}


/*
 *Intermediate QMap used in converting from instrument xml to middleware xml
 *
 *Middleware requires specific xml format, need to properly place keys and attributes into proper positions
 */
QMap<QString, QVector<QPair<QString, QString>>> Jitterbit::reverseEzspecMap(QMap<QString, QPair<QString, QString>> map)
{
	QMap<QString, QVector<QPair<QString, QString>>> reverseMap;
	QMapIterator<QString, QPair<QString, QString>> i(map);

	while (i.hasNext())
	{
		i.next();

		const QString jbElement = i.key();
		const QString ezElement = i.value().first;
		const QString ezAttribute = i.value().second;

		QPair<QString, QString> pair(ezAttribute, jbElement);

		if (!ezElement.isEmpty())
		{
			QVector<QPair<QString, QString>> vectorPair = reverseMap.value(ezElement);
			vectorPair.append(pair);

			reverseMap.insert(ezElement, vectorPair);
		}
		//TODO - Implement Not Applicable
		//else 
		//{
		//    QVector<QPair<QString, QString>> vectorPair = reverseMap.value(ezElement);
		//    vectorPair.append({ "Not Applicable", jbElement });

		//    reverseMap.insert(ezElement, vectorPair);
		//}
	}

	return reverseMap;
}

/*
 *Recursive function to reverse a QDomDocument tree, used to generate the "reversed" QMap.
 */
void Jitterbit::generateJbXmlByRecursion(const QDomNode& node, const QMap<QString, QVector<QPair<QString, QString>>> map, QString &xml)
{
	QDomNode domNode = node.firstChild();
	QDomElement domElement;

	while (!(domNode.isNull()))
	{
		if (domNode.isElement())
		{
			domElement = domNode.toElement();
			if (!(domElement.isNull()))
			{
				QDomNamedNodeMap nodeMap = domElement.attributes();
				for (int i = 0; i < nodeMap.length(); i++)
				{
					QDomAttr attribute = nodeMap.item(i).toAttr();
					if (!attribute.isNull())
					{
						const QString elementName = domElement.tagName();
						QVector<QPair<QString, QString>> mapPair = map.value(elementName);

						for (int j = 0; j < mapPair.count(); j++)
						{
							if (mapPair.at(j).first == attribute.name())
							{
								const QString jbElement = mapPair.at(j).second;
								QXmlStreamWriter writer(&xml);

								writer.writeStartElement(jbElement);
								writer.writeCharacters(attribute.value());
								writer.writeEndElement();
							}
						}
					}
				}
			}
		}
		domNode = domNode.nextSibling();
		generateJbXmlByRecursion(domNode, map, xml);
	}
}

/*
 *Method to convert hardware configuration xml into a tree, taking advantage of the QDomDocument class
 */
void Jitterbit::convertEzXmlToJbXml(QString ezspecHdwXml, QMap<QString, QPair<QString, QString>> map)
{
	QDomDocument doc;
	doc.setContent(ezspecHdwXml);
	QDomNode node = (QDomNode)doc;

	QMap<QString, QVector<QPair<QString, QString>>> reverseMap = reverseEzspecMap(map);
	generateJbXmlByRecursion(node, reverseMap, m_jbXml);
}

/*
 *Getter
 */
QString Jitterbit::getJbXml()
{
	return m_jbXml;
}

/*
 *Setter
 */
void Jitterbit::setPostData(QString data)
{
	m_postData = data.toUtf8();
}
