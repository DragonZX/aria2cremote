#ifndef GEOIP_H
#define GEOIP_H
#include <QString>
#include <QFile>
#include <QHostAddress>
#include <QObject>


static const quint32 COUNTRY_BEGIN = 16776960;
 const QString countryCode[] = { "--","AP","EU","AD","AE","AF","AG","AI","AL","AM","AN","AO","AQ","AR","AS","AT","AU","AW","AZ","BA","BB","BD","BE","BF","BG","BH","BI","BJ","BM","BN","BO","BR","BS","BT","BV","BW","BY","BZ","CA","CC","CD","CF","CG","CH","CI","CK","CL","CM","CN","CO","CR","CU","CV","CX","CY","CZ","DE","DJ","DK","DM","DO","DZ",
                                 "EC","EE","EG","EH","ER","ES","ET","FI","FJ","FK","FM","FO","FR","FX","GA","GB","GD","GE","GF","GH","GI","GL","GM","GN","GP","GQ","GR","GS","GT","GU","GW","GY","HK","HM","HN","HR","HT","HU","ID","IE","IL","IN","IO","IQ","IR","IS","IT","JM","JO","JP","KE","KG","KH","KI","KM","KN","KP","KR","KW","KY","KZ",
                                 "LA","LB","LC","LI","LK","LR","LS","LT","LU","LV","LY","MA","MC","MD","MG","MH","MK","ML","MM","MN","MO","MP","MQ","MR","MS","MT","MU","MV","MW","MX","MY","MZ","NA","NC","NE","NF","NG","NI","NL","NO","NP","NR","NU","NZ","OM","PA","PE","PF","PG","PH","PK","PL","PM","PN","PR","PS","PT","PW","PY","QA",
                                 "RE","RO","RU","RW","SA","SB","SC","SD","SE","SG","SH","SI","SJ","SK","SL","SM","SN","SO","SR","ST","SV","SY","SZ","TC","TD","TF","TG","TH","TJ","TK","TM","TN","TO","TL","TR","TT","TV","TW","TZ","UA","UG","UM","US","UY","UZ","VA","VC","VE","VG","VI","VN","VU","WF","WS","YE","YT","RS","ZA","ZM","ME","ZW","A1","A2",
                                 "O1","AX","GG","IM","JE","BL","MF"
                               };

 const QString countryName[] =  {QObject::tr("N/A"),QObject::tr("Asia/Pacific Region"),QObject::tr("Europe"),QObject::tr("Andorra"),QObject::tr("United Arab Emirates"),QObject::tr("Afghanistan"),QObject::tr("Antigua and Barbuda"),QObject::tr("Anguilla"),QObject::tr("Albania"),QObject::tr("Armenia"),QObject::tr("Netherlands Antilles"),QObject::tr("Angola"),QObject::tr("Antarctica"),QObject::tr("Argentina"),QObject::tr("American Samoa"),QObject::tr("Austria"),QObject::tr("Australia"),QObject::tr("Aruba"),QObject::tr("Azerbaijan"),QObject::tr("Bosnia and Herzegovina"),QObject::tr("Barbados"),QObject::tr("Bangladesh"),QObject::tr("Belgium"),
                                 QObject::tr("Burkina Faso"),QObject::tr("Bulgaria"),QObject::tr("Bahrain"),QObject::tr("Burundi"),QObject::tr("Benin"),QObject::tr("Bermuda"),QObject::tr("Brunei Darussalam"),QObject::tr("Bolivia"),QObject::tr("Brazil"),QObject::tr("Bahamas"),QObject::tr("Bhutan"),QObject::tr("Bouvet Island"),QObject::tr("Botswana"),QObject::tr("Belarus"),QObject::tr("Belize"),QObject::tr("Canada"),QObject::tr("Cocos (Keeling) Islands"),QObject::tr("Congo, The Democratic Republic of the"),QObject::tr("Central African Republic"),QObject::tr("Congo"),QObject::tr("Switzerland"),QObject::tr("Cote D'Ivoire"),
                                 QObject::tr("Cook Islands"),QObject::tr("Chile"),QObject::tr("Cameroon"),QObject::tr("China"),QObject::tr("Colombia"),QObject::tr("Costa Rica"),QObject::tr("Cuba"),QObject::tr("Cape Verde"),QObject::tr("Christmas Island"),QObject::tr("Cyprus"),QObject::tr("Czech Republic"),QObject::tr("Germany"),QObject::tr("Djibouti"),QObject::tr("Denmark"),QObject::tr("Dominica"),QObject::tr("Dominican Republic"),QObject::tr("Algeria"),QObject::tr("Ecuador"),QObject::tr("Estonia"),QObject::tr("Egypt"),QObject::tr("Western Sahara"),QObject::tr("Eritrea"),QObject::tr("Spain"),QObject::tr("Ethiopia"),QObject::tr("Finland"),QObject::tr("Fiji"),QObject::tr("Falkland Islands (Malvinas)"),
                                 QObject::tr("Micronesia, Federated States of"),QObject::tr("Faroe Islands"),QObject::tr("France"),QObject::tr("France, Metropolitan"),QObject::tr("Gabon"),QObject::tr("United Kingdom"),QObject::tr("Grenada"),QObject::tr("Georgia"),QObject::tr("French Guiana"),QObject::tr("Ghana"),QObject::tr("Gibraltar"),QObject::tr("Greenland"),QObject::tr("Gambia"),QObject::tr("Guinea"),QObject::tr("Guadeloupe"),QObject::tr("Equatorial Guinea"),QObject::tr("Greece"),QObject::tr("South Georgia and the South Sandwich Islands"),QObject::tr("Guatemala"),QObject::tr("Guam"),QObject::tr("Guinea-Bissau"),QObject::tr("Guyana"),
                                 QObject::tr("Hong Kong"),QObject::tr("Heard Island and McDonald Islands"),QObject::tr("Honduras"),QObject::tr("Croatia"),QObject::tr("Haiti"),QObject::tr("Hungary"),QObject::tr("Indonesia"),QObject::tr("Ireland"),QObject::tr("Israel"),QObject::tr("India"),QObject::tr("British Indian Ocean Territory"),QObject::tr("Iraq"),QObject::tr("Iran, Islamic Republic of"),QObject::tr("Iceland"),QObject::tr("Italy"),QObject::tr("Jamaica"),QObject::tr("Jordan"),QObject::tr("Japan"),QObject::tr("Kenya"),QObject::tr("Kyrgyzstan"),QObject::tr("Cambodia"),QObject::tr("Kiribati"),QObject::tr("Comoros"),QObject::tr("Saint Kitts and Nevis"),
                                 QObject::tr("Korea, Democratic People's Republic of"),QObject::tr("Korea, Republic of"),QObject::tr("Kuwait"),QObject::tr("Cayman Islands"),QObject::tr("Kazakstan"),QObject::tr("Lao People's Democratic Republic"),QObject::tr("Lebanon"),QObject::tr("Saint Lucia"),QObject::tr("Liechtenstein"),QObject::tr("Sri Lanka"),QObject::tr("Liberia"),QObject::tr("Lesotho"),QObject::tr("Lithuania"),QObject::tr("Luxembourg"),QObject::tr("Latvia"),QObject::tr("Libyan Arab Jamahiriya"),QObject::tr("Morocco"),QObject::tr("Monaco"),QObject::tr("Moldova, Republic of"),QObject::tr("Madagascar"),
                                 QObject::tr("Marshall Islands"),QObject::tr("Macedonia"),QObject::tr("Mali"),QObject::tr("Myanmar"),QObject::tr("Mongolia"),QObject::tr("Macau"),QObject::tr("Northern Mariana Islands"),QObject::tr("Martinique"),QObject::tr("Mauritania"),QObject::tr("Montserrat"),QObject::tr("Malta"),QObject::tr("Mauritius"),QObject::tr("Maldives"),QObject::tr("Malawi"),QObject::tr("Mexico"),QObject::tr("Malaysia"),QObject::tr("Mozambique"),QObject::tr("Namibia"),QObject::tr("New Caledonia"),QObject::tr("Niger"),QObject::tr("Norfolk Island"),QObject::tr("Nigeria"),QObject::tr("Nicaragua"),QObject::tr("Netherlands"),
                                 QObject::tr("Norway"),QObject::tr("Nepal"),QObject::tr("Nauru"),QObject::tr("Niue"),QObject::tr("New Zealand"),QObject::tr("Oman"),QObject::tr("Panama"),QObject::tr("Peru"),QObject::tr("French Polynesia"),QObject::tr("Papua New Guinea"),QObject::tr("Philippines"),QObject::tr("Pakistan"),QObject::tr("Poland"),QObject::tr("Saint Pierre and Miquelon"),QObject::tr("Pitcairn Islands"),QObject::tr("Puerto Rico"),QObject::tr("Palestinian Territory"),QObject::tr("Portugal"),QObject::tr("Palau"),QObject::tr("Paraguay"),QObject::tr("Qatar"),QObject::tr("Reunion"),QObject::tr("Romania"),QObject::tr("Russian Federation"),QObject::tr("Rwanda"),QObject::tr("Saudi Arabia"),
                                 QObject::tr("Solomon Islands"),QObject::tr("Seychelles"),QObject::tr("Sudan"),QObject::tr("Sweden"),QObject::tr("Singapore"),QObject::tr("Saint Helena"),QObject::tr("Slovenia"),QObject::tr("Svalbard and Jan Mayen"),QObject::tr("Slovakia"),QObject::tr("Sierra Leone"),QObject::tr("San Marino"),QObject::tr("Senegal"),QObject::tr("Somalia"),QObject::tr("Suriname"),QObject::tr("Sao Tome and Principe"),QObject::tr("El Salvador"),QObject::tr("Syrian Arab Republic"),QObject::tr("Swaziland"),QObject::tr("Turks and Caicos Islands"),QObject::tr("Chad"),QObject::tr("French Southern Territories"),QObject::tr("Togo"),
                                 QObject::tr("Thailand"),QObject::tr("Tajikistan"),QObject::tr("Tokelau"),QObject::tr("Turkmenistan"),QObject::tr("Tunisia"),QObject::tr("Tonga"),QObject::tr("Timor-Leste"),QObject::tr("Turkey"),QObject::tr("Trinidad and Tobago"),QObject::tr("Tuvalu"),QObject::tr("Taiwan"),QObject::tr("Tanzania, United Republic of"),QObject::tr("Ukraine"),QObject::tr("Uganda"),QObject::tr("United States Minor Outlying Islands"),QObject::tr("United States"),QObject::tr("Uruguay"),QObject::tr("Uzbekistan"),QObject::tr("Holy See (Vatican City State)"),QObject::tr("Saint Vincent and the Grenadines"),
                                 QObject::tr("Venezuela"),QObject::tr("Virgin Islands, British"),QObject::tr("Virgin Islands, U.S."),QObject::tr("Vietnam"),QObject::tr("Vanuatu"),QObject::tr("Wallis and Futuna"),QObject::tr("Samoa"),QObject::tr("Yemen"),QObject::tr("Mayotte"),QObject::tr("Serbia"),QObject::tr("South Africa"),QObject::tr("Zambia"),QObject::tr("Montenegro"),QObject::tr("Zimbabwe"),QObject::tr("Anonymous Proxy"),QObject::tr("Satellite Provider"),
                                 QObject::tr("Other"),QObject::tr("Aland Islands"),QObject::tr("Guernsey"),QObject::tr("Isle of Man"),QObject::tr("Jersey"),QObject::tr("Saint Barthelemy"),QObject::tr("Saint Martin")
                                };

class GeoIP
{
private:
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
