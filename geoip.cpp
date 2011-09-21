#include "geoip.h"
#include <QDir>

GeoIP::GeoIP(QString& filename)
{
    FileInput.setFileName(filename);

    try
    {
        FileInput.open(QIODevice::ReadOnly);
    }
    catch (...) { }
}

GeoIP::~GeoIP()
{
    try
    {
        FileInput.close();
    }
    catch (...) { }
}

QString GeoIP::lookupCountryCode(QString str)
{
    QHostAddress addr(str);
    return lookupCountryCode(addr);
}

QString GeoIP::lookupCountryCode(QHostAddress addr)
{
    return(countryCode[(int)seekCountry(0, addr.toIPv4Address(), 31)]);
}

QString GeoIP::lookupCountryName(QString str)
{
    QHostAddress addr(str);
    return lookupCountryName(addr);
}

QString GeoIP::lookupCountryName(QHostAddress addr)
{
    return(countryName[(int)seekCountry(0, addr.toIPv4Address(), 31)]);
}

quint32 GeoIP::seekCountry(quint32 offset, quint32 ipnum, int depth)
{
    QByteArray buf;
    quint32 x[2];

    if (depth < 0)
    {
        return 0; // N/A
    }

    try
    {
        FileInput.seek(6 * offset);
        buf = FileInput.read(6);
    }
    catch (...)
    {
        return 0;
    }

    for (int i = 0; i<2; i++)
    {
        x[i] = 0;
        for (int j = 0; j<3; j++)
        {
            int y = buf[i*3+j];
            if (y < 0)
            {
                y+= 256;
            }
            x[i] += (y << (j * 8));
        }
    }

    if ((ipnum & (1 << depth)) > 0)
    {
        if (x[1] >= COUNTRY_BEGIN)
        {
            return x[1] - COUNTRY_BEGIN;
        }
        return seekCountry(x[1], ipnum, depth-1);
    }
    else
    {
        if (x[0] >= COUNTRY_BEGIN)
        {
            return x[0] - COUNTRY_BEGIN;
        }
        return seekCountry(x[0], ipnum, depth-1);
    }
}
