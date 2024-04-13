static void s_server_init(void)
{
    accept_socket = -1;
    verify_depth = 0;
    s_server_verify = SSL_VERIFY_NONE;
    s_dcert_file = NULL;
    s_dkey_file = NULL;
    s_dchain_file = NULL;
    s_cert_file = TEST_CERT;
    s_key_file = NULL;
    s_chain_file = NULL;
    s_cert_file2 = TEST_CERT2;
    s_key_file2 = NULL;
    ctx2 = NULL;
    s_nbio = 0;
    s_nbio_test = 0;
    ctx = NULL;
    www = 0;
    bio_s_out = NULL;
    s_debug = 0;
    s_msg = 0;
    s_quiet = 0;
    s_brief = 0;
    async = 0;
#ifndef OPENSSL_NO_ENGINE
    engine_id = NULL;
#endif
}