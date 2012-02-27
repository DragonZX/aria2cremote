#ifndef FEATURES_H
#define FEATURES_H
namespace util
{
    //Aria2c versions
    const quint32 ARIA2C_VERSION_NONE = 0x00000000;
    const quint32 ARIA2C_VERSION_181  = 0x00010801;
    const quint32 ARIA2C_VERSION_182  = 0x00010802;
    const quint32 ARIA2C_VERSION_183  = 0x00010803;
    const quint32 ARIA2C_VERSION_190  = 0x00010900;
    const quint32 ARIA2C_VERSION_191  = 0x00010901;
    const quint32 ARIA2C_VERSION_192  = 0x00010902;
    const quint32 ARIA2C_VERSION_193  = 0x00010903;
    const quint32 ARIA2C_VERSION_194  = 0x00010904;
    const quint32 ARIA2C_VERSION_195  = 0x00010905;
    const quint32 ARIA2C_VERSION_1100 = 0x00010A00;
    const quint32 ARIA2C_VERSION_1101 = 0x00010A01;
    const quint32 ARIA2C_VERSION_1102 = 0x00010A02;
    const quint32 ARIA2C_VERSION_1103 = 0x00010A03;
    const quint32 ARIA2C_VERSION_1104 = 0x00010A04;
    const quint32 ARIA2C_VERSION_1105 = 0x00010A05;
    const quint32 ARIA2C_VERSION_1106 = 0x00010A06;
    const quint32 ARIA2C_VERSION_1107 = 0x00010A07;
    const quint32 ARIA2C_VERSION_1108 = 0x00010A08;
    const quint32 ARIA2C_VERSION_1109 = 0x00010A09;
    const quint32 ARIA2C_VERSION_1110 = 0x00010B00;
    const quint32 ARIA2C_VERSION_1111 = 0x00010B01;
    const quint32 ARIA2C_VERSION_1112 = 0x00010B02;
    const quint32 ARIA2C_VERSION_1120 = 0x00010C00;
    const quint32 ARIA2C_VERSION_1121 = 0x00010C01;
    const quint32 ARIA2C_VERSION_1130 = 0x00010D00;
    const quint32 ARIA2C_VERSION_1140 = 0x00010E00;
    const quint32 ARIA2C_VERSION_1141 = 0x00010E01;
    const quint32 ARIA2C_VERSION_MASK = 0x00FFFFFF;

    //Aria2c Features
    const quint32 ARIA2C_FEATURES_NONE           = 0x00000000;
    const quint32 ARIA2C_FEATURES_ASYNCDNS       = 0x01000000;
    const quint32 ARIA2C_FEATURES_BITTORRENT     = 0x02000000;
    const quint32 ARIA2C_FEATURES_FIREFOX3COOKIE = 0x04000000;
    const quint32 ARIA2C_FEATURES_GZIP           = 0x08000000;
    const quint32 ARIA2C_FEATURES_HTTPS          = 0x10000000;
    const quint32 ARIA2C_FEATURES_MESSAGEDIGEST  = 0x20000000;
    const quint32 ARIA2C_FEATURES_METALINK       = 0x40000000;
    const quint32 ARIA2C_FEATURES_XMLRPC         = 0x80000000;
    const quint32 ARIA2C_FEATURES_ALL            = 0xFF000000;
    const quint32 ARIA2C_FEATURES_OBSOLETE       = 0x01000000;

}
#endif // FEATURES_H
