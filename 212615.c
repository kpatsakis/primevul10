void LanLinkProvider::configureSslSocket(QSslSocket* socket, const QString& deviceId, bool isDeviceTrusted)
{
    // Setting supported ciphers manually, to match those on Android (FIXME: Test if this can be left unconfigured and still works for Android 4)
    QList<QSslCipher> socketCiphers;
    socketCiphers.append(QSslCipher(QStringLiteral("ECDHE-ECDSA-AES256-GCM-SHA384")));
    socketCiphers.append(QSslCipher(QStringLiteral("ECDHE-ECDSA-AES128-GCM-SHA256")));
    socketCiphers.append(QSslCipher(QStringLiteral("ECDHE-RSA-AES128-SHA")));

    // Configure for ssl
    QSslConfiguration sslConfig;
    sslConfig.setCiphers(socketCiphers);
    sslConfig.setLocalCertificate(KdeConnectConfig::instance().certificate());

    QFile privateKeyFile(KdeConnectConfig::instance().privateKeyPath());
    QSslKey privateKey;
    if (privateKeyFile.open(QIODevice::ReadOnly)) {
        privateKey = QSslKey(privateKeyFile.readAll(), QSsl::Rsa);
    }
    privateKeyFile.close();
    sslConfig.setPrivateKey(privateKey);

    if (isDeviceTrusted) {
        QString certString = KdeConnectConfig::instance().getDeviceProperty(deviceId, QStringLiteral("certificate"), QString());
        sslConfig.setCaCertificates({QSslCertificate(certString.toLatin1())});
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    } else {
        sslConfig.setPeerVerifyMode(QSslSocket::QueryPeer);
    }
    socket->setSslConfiguration(sslConfig);
    socket->setPeerVerifyName(deviceId);


    //Usually SSL errors are only bad for trusted devices. Uncomment this section to log errors in any case, for debugging.
    //QObject::connect(socket, static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors), [](const QList<QSslError>& errors)
    //{
    //    Q_FOREACH (const QSslError& error, errors) {
    //        qCDebug(KDECONNECT_CORE) << "SSL Error:" << error.errorString();
    //    }
    //});
}