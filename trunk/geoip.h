#ifndef GEOIP_H
#define GEOIP_H
#include <QtGui/QApplication>
#include <QString>
#include <QFile>
#include <QHostAddress>

class GeoIP
{
    Q_DECLARE_TR_FUNCTIONS(GeoIP)
private:
    static const quint32 COUNTRY_BEGIN = 16776960;

    QFile FileInput;
    quint32 seekCountry(quint32 offset, quint32 ipnum, int depth);
    QString lookupCountryCode(QHostAddress addr);
    QString lookupCountryName(QHostAddress addr);

public:
    GeoIP(QString& filename);
    ~GeoIP();

    QString lookupCountryCode(QString str);
    QString lookupCountryName(QString str);
};

#endif // QGEOIP_H
