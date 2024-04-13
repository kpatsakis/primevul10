long Socket::checkCertValid(String &hostname)
{
    //check we have a certificate
    X509 *peerCert = SSL_get_peer_certificate(ssl);
    if (peerCert == NULL) {
        return -1;
    }
    X509_free(peerCert);

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#else
// section for openssl1.1
X509_VERIFY_PARAM *param;
param = X509_VERIFY_PARAM_new() ;
X509_VERIFY_PARAM_set1_host(param,hostname.c_str(), hostname.length());
SSL_CTX_set1_param(ctx,param);
X509_VERIFY_PARAM_free(param);
#endif
    return SSL_get_verify_result(ssl);
}