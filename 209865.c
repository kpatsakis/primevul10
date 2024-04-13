static const char* jsw_getReasonStr(enum lws_callback_reasons reason) {
    typedef struct { enum lws_callback_reasons r; const char *name; } ssType;
    static ssType ss[] = {
#define MKLCBS(n) { n, #n }
    MKLCBS(LWS_CALLBACK_PROTOCOL_INIT),
    MKLCBS(LWS_CALLBACK_PROTOCOL_DESTROY), MKLCBS(LWS_CALLBACK_WSI_CREATE),
    MKLCBS(LWS_CALLBACK_WSI_DESTROY),MKLCBS(LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS),
    MKLCBS(LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS),
    MKLCBS(LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION),
    MKLCBS(LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY),
    MKLCBS(LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED), MKLCBS(LWS_CALLBACK_HTTP),
    MKLCBS(LWS_CALLBACK_HTTP_BODY), MKLCBS(LWS_CALLBACK_HTTP_BODY_COMPLETION),
    MKLCBS(LWS_CALLBACK_HTTP_FILE_COMPLETION), MKLCBS(LWS_CALLBACK_HTTP_WRITEABLE),
    MKLCBS(LWS_CALLBACK_CLOSED_HTTP), MKLCBS(LWS_CALLBACK_FILTER_HTTP_CONNECTION),
    MKLCBS(LWS_CALLBACK_ESTABLISHED),
    MKLCBS(LWS_CALLBACK_CLOSED),
    MKLCBS(LWS_CALLBACK_SERVER_WRITEABLE),
    MKLCBS(LWS_CALLBACK_RECEIVE), MKLCBS(LWS_CALLBACK_RECEIVE_PONG),
    MKLCBS(LWS_CALLBACK_WS_PEER_INITIATED_CLOSE), MKLCBS(LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION),
    MKLCBS(LWS_CALLBACK_CONFIRM_EXTENSION_OKAY),
    MKLCBS(LWS_CALLBACK_CLIENT_CONNECTION_ERROR),
    MKLCBS(LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH), MKLCBS(LWS_CALLBACK_CLIENT_ESTABLISHED),
    MKLCBS(LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER), MKLCBS(LWS_CALLBACK_CLIENT_RECEIVE),
    MKLCBS(LWS_CALLBACK_CLIENT_RECEIVE_PONG),
    MKLCBS(LWS_CALLBACK_CLIENT_WRITEABLE), MKLCBS(LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED),
    MKLCBS(LWS_CALLBACK_WS_EXT_DEFAULTS), MKLCBS(LWS_CALLBACK_FILTER_NETWORK_CONNECTION),
    MKLCBS(LWS_CALLBACK_GET_THREAD_ID), MKLCBS(LWS_CALLBACK_ADD_POLL_FD),
    MKLCBS(LWS_CALLBACK_DEL_POLL_FD), MKLCBS(LWS_CALLBACK_CHANGE_MODE_POLL_FD), MKLCBS(LWS_CALLBACK_LOCK_POLL),
    MKLCBS(LWS_CALLBACK_UNLOCK_POLL),
#if (LWS_LIBRARY_VERSION_MAJOR>1)
    MKLCBS(LWS_CALLBACK_CGI),
    MKLCBS(LWS_CALLBACK_CGI_TERMINATED),
    MKLCBS(LWS_CALLBACK_CGI_STDIN_DATA),
    MKLCBS(LWS_CALLBACK_CGI_STDIN_COMPLETED),
    MKLCBS(LWS_CALLBACK_SESSION_INFO),
    MKLCBS(LWS_CALLBACK_GS_EVENT),
    MKLCBS(LWS_CALLBACK_HTTP_PMO),
    MKLCBS(LWS_CALLBACK_RAW_RX),
    MKLCBS(LWS_CALLBACK_RAW_CLOSE),
    MKLCBS(LWS_CALLBACK_RAW_WRITEABLE),
    MKLCBS(LWS_CALLBACK_RAW_ADOPT),
    MKLCBS(LWS_CALLBACK_RAW_ADOPT_FILE),
    MKLCBS(LWS_CALLBACK_ADD_HEADERS),
    MKLCBS(LWS_CALLBACK_CHECK_ACCESS_RIGHTS),
    MKLCBS(LWS_CALLBACK_PROCESS_HTML),
    MKLCBS(LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP),
    MKLCBS(LWS_CALLBACK_CLOSED_CLIENT_HTTP),
    MKLCBS(LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ),
    MKLCBS(LWS_CALLBACK_RECEIVE_CLIENT_HTTP),
    MKLCBS(LWS_CALLBACK_COMPLETED_CLIENT_HTTP),
    MKLCBS(LWS_CALLBACK_CLIENT_HTTP_WRITEABLE),
    MKLCBS(LWS_CALLBACK_HTTP_BIND_PROTOCOL),
    MKLCBS(LWS_CALLBACK_HTTP_DROP_PROTOCOL),
    MKLCBS(LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION),
    MKLCBS(LWS_CALLBACK_RAW_RX_FILE),
    MKLCBS(LWS_CALLBACK_RAW_WRITEABLE_FILE),
    MKLCBS(LWS_CALLBACK_RAW_CLOSE_FILE),
    MKLCBS(LWS_CALLBACK_USER),
#endif
#if (LWS_LIBRARY_VERSION_NUMBER>=3000000)
    MKLCBS(LWS_CALLBACK_SSL_INFO),
    MKLCBS(LWS_CALLBACK_CGI_PROCESS_ATTACH),
    MKLCBS(LWS_CALLBACK_CLIENT_CLOSED),
    MKLCBS(LWS_CALLBACK_TIMER),
    MKLCBS(LWS_CALLBACK_EVENT_WAIT_CANCELLED),
    MKLCBS(LWS_CALLBACK_CHILD_CLOSING),
    MKLCBS(LWS_CALLBACK_CHILD_WRITE_VIA_PARENT),
    MKLCBS(LWS_CALLBACK_VHOST_CERT_AGING),
    MKLCBS(LWS_CALLBACK_VHOST_CERT_UPDATE),
#endif
    {(enum lws_callback_reasons)0, NULL }
    };
    int i = -1;
    while (ss[++i].name)
        if (ss[i].r == reason)
            return ss[i].name;
    return "";
}