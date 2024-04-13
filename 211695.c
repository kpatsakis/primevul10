  OpenSSLInitializer() {
    SSL_library_init();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();
    OpenSSL_add_all_algorithms();

// CCM ciphers are not added by default, so let's add them!
#if !defined(OPENSSL_NO_AES) && defined(EVP_CIPH_CCM_MODE) && \
    OPENSSL_VERSION_NUMBER < 0x100020000
    EVP_add_cipher(EVP_aes_128_ccm());
    EVP_add_cipher(EVP_aes_192_ccm());
    EVP_add_cipher(EVP_aes_256_ccm());
#endif

    ERR_load_ERR_strings();
    ERR_load_crypto_strings();
    ERR_load_EVP_strings();

    /* Determine default SSL configuration file */
    char *config_filename = getenv("OPENSSL_CONF");
    if (config_filename == nullptr) {
      config_filename = getenv("SSLEAY_CONF");
    }

    /* default to 'openssl.cnf' if no environment variable is set */
    if (config_filename == nullptr) {
      snprintf(default_ssl_conf_filename, sizeof(default_ssl_conf_filename),
               "%s/%s", X509_get_default_cert_area(), "openssl.cnf");
    } else {
      always_assert(
        strlen(config_filename) < sizeof(default_ssl_conf_filename));
      strcpy(default_ssl_conf_filename, config_filename);
    }
  }