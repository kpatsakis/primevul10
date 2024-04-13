int s_server_main(int argc, char *argv[])
{
    ENGINE *e = NULL;
    EVP_PKEY *s_key = NULL, *s_dkey = NULL;
    SSL_CONF_CTX *cctx = NULL;
    const SSL_METHOD *meth = TLS_server_method();
    SSL_EXCERT *exc = NULL;
    STACK_OF(OPENSSL_STRING) *ssl_args = NULL;
    STACK_OF(X509) *s_chain = NULL, *s_dchain = NULL;
    STACK_OF(X509_CRL) *crls = NULL;
    X509 *s_cert = NULL, *s_dcert = NULL;
    X509_VERIFY_PARAM *vpm = NULL;
    char *CApath = NULL, *CAfile = NULL, *chCApath = NULL, *chCAfile = NULL;
#ifndef OPENSSL_NO_DH
    char *dhfile = NULL;
#endif
    char *dpassarg = NULL, *dpass = NULL, *inrand = NULL;
    char *passarg = NULL, *pass = NULL, *vfyCApath = NULL, *vfyCAfile = NULL;
    char *crl_file = NULL, *prog;
#ifndef OPENSSL_NO_PSK
    char *p;
#endif
#ifdef AF_UNIX
    int unlink_unix_path = 0;
#endif
    do_server_cb server_cb;
    int vpmtouched = 0, build_chain = 0, no_cache = 0, ext_cache = 0;
#ifndef OPENSSL_NO_DH
    int no_dhe = 0;
#endif
    int nocert = 0, ret = 1;
    int noCApath = 0, noCAfile = 0;
    int s_cert_format = FORMAT_PEM, s_key_format = FORMAT_PEM;
    int s_dcert_format = FORMAT_PEM, s_dkey_format = FORMAT_PEM;
    int rev = 0, naccept = -1, sdebug = 0;
    int socket_family = AF_UNSPEC, socket_type = SOCK_STREAM;
    int state = 0, crl_format = FORMAT_PEM, crl_download = 0;
    char *host = NULL;
    char *port = BUF_strdup(PORT);
    unsigned char *context = NULL;
    OPTION_CHOICE o;
    EVP_PKEY *s_key2 = NULL;
    X509 *s_cert2 = NULL;
    tlsextctx tlsextcbp = { NULL, NULL, SSL_TLSEXT_ERR_ALERT_WARNING };
    const char *ssl_config = NULL;
#ifndef OPENSSL_NO_NEXTPROTONEG
    const char *next_proto_neg_in = NULL;
    tlsextnextprotoctx next_proto = { NULL, 0 };
#endif
    const char *alpn_in = NULL;
    tlsextalpnctx alpn_ctx = { NULL, 0 };
#ifndef OPENSSL_NO_PSK
    /* by default do not send a PSK identity hint */
    static char *psk_identity_hint = NULL;
#endif
#ifndef OPENSSL_NO_SRP
    char *srpuserseed = NULL;
    char *srp_verifier_file = NULL;
#endif

    local_argc = argc;
    local_argv = argv;

    s_server_init();
    cctx = SSL_CONF_CTX_new();
    vpm = X509_VERIFY_PARAM_new();
    if (cctx == NULL || vpm == NULL)
        goto end;
    SSL_CONF_CTX_set_flags(cctx, SSL_CONF_FLAG_SERVER | SSL_CONF_FLAG_CMDLINE);

    prog = opt_init(argc, argv, s_server_options);
    while ((o = opt_next()) != OPT_EOF) {
        switch (o) {
        case OPT_EOF:
        case OPT_ERR:
 opthelp:
            BIO_printf(bio_err, "%s: Use -help for summary.\n", prog);
            goto end;
        case OPT_HELP:
            opt_help(s_server_options);
            ret = 0;
            goto end;

        case OPT_4:
#ifdef AF_UNIX
            if (socket_family == AF_UNIX) {
                OPENSSL_free(host); host = NULL;
                OPENSSL_free(port); port = NULL;
            }
#endif
            socket_family = AF_INET;
            break;
        case OPT_6:
            if (1) {
#ifdef AF_INET6
#ifdef AF_UNIX
                if (socket_family == AF_UNIX) {
                    OPENSSL_free(host); host = NULL;
                    OPENSSL_free(port); port = NULL;
                }
#endif
                socket_family = AF_INET6;
            } else {
#endif
                BIO_printf(bio_err, "%s: IPv6 domain sockets unsupported\n", prog);
                goto end;
            }
            break;
        case OPT_PORT:
#ifdef AF_UNIX
            if (socket_family == AF_UNIX) {
                socket_family = AF_UNSPEC;
            }
#endif
            OPENSSL_free(port); port = NULL;
            OPENSSL_free(host); host = NULL;
            if (BIO_parse_hostserv(opt_arg(), NULL, &port, BIO_PARSE_PRIO_SERV) < 1) {
                BIO_printf(bio_err,
                           "%s: -port argument malformed or ambiguous\n",
                           port);
                goto end;
            }
            break;
        case OPT_ACCEPT:
#ifdef AF_UNIX
            if (socket_family == AF_UNIX) {
                socket_family = AF_UNSPEC;
            }
#endif
            OPENSSL_free(port); port = NULL;
            OPENSSL_free(host); host = NULL;
            if (BIO_parse_hostserv(opt_arg(), &host, &port, BIO_PARSE_PRIO_SERV) < 1) {
                BIO_printf(bio_err,
                           "%s: -accept argument malformed or ambiguous\n",
                           port);
                goto end;
            }
            break;
#ifdef AF_UNIX
        case OPT_UNIX:
            socket_family = AF_UNIX;
            OPENSSL_free(host); host = BUF_strdup(opt_arg());
            OPENSSL_free(port); port = NULL;
            break;
        case OPT_UNLINK:
            unlink_unix_path = 1;
            break;
#endif
        case OPT_NACCEPT:
            naccept = atol(opt_arg());
            break;
        case OPT_VERIFY:
            s_server_verify = SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE;
            verify_depth = atoi(opt_arg());
            if (!s_quiet)
                BIO_printf(bio_err, "verify depth is %d\n", verify_depth);
            break;
        case OPT_UPPER_V_VERIFY:
            s_server_verify =
                SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT |
                SSL_VERIFY_CLIENT_ONCE;
            verify_depth = atoi(opt_arg());
            if (!s_quiet)
                BIO_printf(bio_err,
                           "verify depth is %d, must return a certificate\n",
                           verify_depth);
            break;
        case OPT_CONTEXT:
            context = (unsigned char *)opt_arg();
            break;
        case OPT_CERT:
            s_cert_file = opt_arg();
            break;
        case OPT_CRL:
            crl_file = opt_arg();
            break;
        case OPT_CRL_DOWNLOAD:
            crl_download = 1;
            break;
        case OPT_SERVERINFO:
            s_serverinfo_file = opt_arg();
            break;
        case OPT_CERTFORM:
            if (!opt_format(opt_arg(), OPT_FMT_PEMDER, &s_cert_format))
                goto opthelp;
            break;
        case OPT_KEY:
            s_key_file = opt_arg();
            break;
        case OPT_KEYFORM:
            if (!opt_format(opt_arg(), OPT_FMT_ANY, &s_key_format))
                goto opthelp;
            break;
        case OPT_PASS:
            passarg = opt_arg();
            break;
        case OPT_CERT_CHAIN:
            s_chain_file = opt_arg();
            break;
        case OPT_DHPARAM:
#ifndef OPENSSL_NO_DH
            dhfile = opt_arg();
#endif
            break;
        case OPT_DCERTFORM:
            if (!opt_format(opt_arg(), OPT_FMT_PEMDER, &s_dcert_format))
                goto opthelp;
            break;
        case OPT_DCERT:
            s_dcert_file = opt_arg();
            break;
        case OPT_DKEYFORM:
            if (!opt_format(opt_arg(), OPT_FMT_PEMDER, &s_dkey_format))
                goto opthelp;
            break;
        case OPT_DPASS:
            dpassarg = opt_arg();
            break;
        case OPT_DKEY:
            s_dkey_file = opt_arg();
            break;
        case OPT_DCERT_CHAIN:
            s_dchain_file = opt_arg();
            break;
        case OPT_NOCERT:
            nocert = 1;
            break;
        case OPT_CAPATH:
            CApath = opt_arg();
            break;
        case OPT_NOCAPATH:
            noCApath = 1;
            break;
        case OPT_CHAINCAPATH:
            chCApath = opt_arg();
            break;
        case OPT_VERIFYCAPATH:
            vfyCApath = opt_arg();
            break;
        case OPT_NO_CACHE:
            no_cache = 1;
            break;
        case OPT_EXT_CACHE:
            ext_cache = 1;
            break;
        case OPT_CRLFORM:
            if (!opt_format(opt_arg(), OPT_FMT_PEMDER, &crl_format))
                goto opthelp;
            break;
        case OPT_S_CASES:
            if (ssl_args == NULL)
                ssl_args = sk_OPENSSL_STRING_new_null();
            if (ssl_args == NULL
                || !sk_OPENSSL_STRING_push(ssl_args, opt_flag())
                || !sk_OPENSSL_STRING_push(ssl_args, opt_arg())) {
                BIO_printf(bio_err, "%s: Memory allocation failure\n", prog);
                goto end;
            }
            break;
        case OPT_V_CASES:
            if (!opt_verify(o, vpm))
                goto end;
            vpmtouched++;
            break;
        case OPT_X_CASES:
            if (!args_excert(o, &exc))
                goto end;
            break;
        case OPT_VERIFY_RET_ERROR:
            verify_return_error = 1;
            break;
        case OPT_VERIFY_QUIET:
            verify_quiet = 1;
            break;
        case OPT_BUILD_CHAIN:
            build_chain = 1;
            break;
        case OPT_CAFILE:
            CAfile = opt_arg();
            break;
        case OPT_NOCAFILE:
            noCAfile = 1;
            break;
        case OPT_CHAINCAFILE:
            chCAfile = opt_arg();
            break;
        case OPT_VERIFYCAFILE:
            vfyCAfile = opt_arg();
            break;
        case OPT_NBIO:
            s_nbio = 1;
            break;
        case OPT_NBIO_TEST:
            s_nbio = s_nbio_test = 1;
            break;
        case OPT_IGN_EOF:
            s_ign_eof = 1;
            break;
        case OPT_NO_IGN_EOF:
            s_ign_eof = 0;
            break;
        case OPT_DEBUG:
            s_debug = 1;
            break;
        case OPT_TLSEXTDEBUG:
            s_tlsextdebug = 1;
            break;
        case OPT_STATUS:
            s_tlsextstatus = 1;
            break;
        case OPT_STATUS_VERBOSE:
            s_tlsextstatus = tlscstatp.verbose = 1;
            break;
        case OPT_STATUS_TIMEOUT:
            s_tlsextstatus = 1;
            tlscstatp.timeout = atoi(opt_arg());
            break;
        case OPT_STATUS_URL:
            s_tlsextstatus = 1;
            if (!OCSP_parse_url(opt_arg(),
                                &tlscstatp.host,
                                &tlscstatp.port,
                                &tlscstatp.path, &tlscstatp.use_ssl)) {
                BIO_printf(bio_err, "Error parsing URL\n");
                goto end;
            }
            break;
        case OPT_MSG:
            s_msg = 1;
            break;
        case OPT_MSGFILE:
            bio_s_msg = BIO_new_file(opt_arg(), "w");
            break;
        case OPT_TRACE:
#ifndef OPENSSL_NO_SSL_TRACE
            s_msg = 2;
#else
            break;
#endif
        case OPT_SECURITY_DEBUG:
            sdebug = 1;
            break;
        case OPT_SECURITY_DEBUG_VERBOSE:
            sdebug = 2;
            break;
        case OPT_STATE:
            state = 1;
            break;
        case OPT_CRLF:
            s_crlf = 1;
            break;
        case OPT_QUIET:
            s_quiet = 1;
            break;
        case OPT_BRIEF:
            s_quiet = s_brief = verify_quiet = 1;
            break;
        case OPT_NO_DHE:
#ifndef OPENSSL_NO_DH
            no_dhe = 1;
#endif
            break;
        case OPT_NO_RESUME_EPHEMERAL:
            no_resume_ephemeral = 1;
            break;
        case OPT_PSK_HINT:
#ifndef OPENSSL_NO_PSK
            psk_identity_hint = opt_arg();
#endif
            break;
        case OPT_PSK:
#ifndef OPENSSL_NO_PSK
            for (p = psk_key = opt_arg(); *p; p++) {
                if (isxdigit(_UC(*p)))
                    continue;
                BIO_printf(bio_err, "Not a hex number '%s'\n", *argv);
                goto end;
            }
#endif
            break;
        case OPT_SRPVFILE:
#ifndef OPENSSL_NO_SRP
            srp_verifier_file = opt_arg();
            meth = TLSv1_server_method();
#endif
            break;
        case OPT_SRPUSERSEED:
#ifndef OPENSSL_NO_SRP
            srpuserseed = opt_arg();
            meth = TLSv1_server_method();
#endif
            break;
        case OPT_REV:
            rev = 1;
            break;
        case OPT_WWW:
            www = 1;
            break;
        case OPT_UPPER_WWW:
            www = 2;
            break;
        case OPT_HTTP:
            www = 3;
            break;
        case OPT_SSL_CONFIG:
            ssl_config = opt_arg();
            break;
        case OPT_SSL3:
#ifndef OPENSSL_NO_SSL3
            meth = SSLv3_server_method();
#endif
            break;
        case OPT_TLS1_2:
#ifndef OPENSSL_NO_TLS1_2
            meth = TLSv1_2_server_method();
#endif
            break;
        case OPT_TLS1_1:
#ifndef OPENSSL_NO_TLS1_1
            meth = TLSv1_1_server_method();
#endif
            break;
        case OPT_TLS1:
#ifndef OPENSSL_NO_TLS1
            meth = TLSv1_server_method();
#endif
            break;
        case OPT_DTLS:
#ifndef OPENSSL_NO_DTLS
            meth = DTLS_server_method();
            socket_type = SOCK_DGRAM;
#endif
            break;
        case OPT_DTLS1:
#ifndef OPENSSL_NO_DTLS1
            meth = DTLSv1_server_method();
            socket_type = SOCK_DGRAM;
#endif
            break;
        case OPT_DTLS1_2:
#ifndef OPENSSL_NO_DTLS1_2
            meth = DTLSv1_2_server_method();
            socket_type = SOCK_DGRAM;
#endif
            break;
        case OPT_TIMEOUT:
#ifndef OPENSSL_NO_DTLS
            enable_timeouts = 1;
#endif
            break;
        case OPT_MTU:
#ifndef OPENSSL_NO_DTLS
            socket_mtu = atol(opt_arg());
#endif
            break;
        case OPT_CHAIN:
#ifndef OPENSSL_NO_DTLS
            cert_chain = 1;
#endif
            break;
        case OPT_LISTEN:
#ifndef OPENSSL_NO_DTLS
            dtlslisten = 1;
#endif
            break;
        case OPT_ID_PREFIX:
            session_id_prefix = opt_arg();
            break;
        case OPT_ENGINE:
            e = setup_engine(opt_arg(), 1);
            break;
        case OPT_RAND:
            inrand = opt_arg();
            break;
        case OPT_SERVERNAME:
            tlsextcbp.servername = opt_arg();
            break;
        case OPT_SERVERNAME_FATAL:
            tlsextcbp.extension_error = SSL_TLSEXT_ERR_ALERT_FATAL;
            break;
        case OPT_CERT2:
            s_cert_file2 = opt_arg();
            break;
        case OPT_KEY2:
            s_key_file2 = opt_arg();
            break;
        case OPT_NEXTPROTONEG:
# ifndef OPENSSL_NO_NEXTPROTONEG
            next_proto_neg_in = opt_arg();
#endif
            break;
        case OPT_ALPN:
            alpn_in = opt_arg();
            break;
        case OPT_SRTP_PROFILES:
            srtp_profiles = opt_arg();
            break;
        case OPT_KEYMATEXPORT:
            keymatexportlabel = opt_arg();
            break;
        case OPT_KEYMATEXPORTLEN:
            keymatexportlen = atoi(opt_arg());
            break;
        case OPT_ASYNC:
            async = 1;
            break;
        }
    }
    argc = opt_num_rest();
    argv = opt_rest();

#ifndef OPENSSL_NO_DTLS
    if (www && socket_type == SOCK_DGRAM) {
        BIO_printf(bio_err, "Can't use -HTTP, -www or -WWW with DTLS\n");
        goto end;
    }

    if (dtlslisten && socket_type != SOCK_DGRAM) {
        BIO_printf(bio_err, "Can only use -listen with DTLS\n");
        goto end;
    }
#endif

#ifdef AF_UNIX
    if (socket_family == AF_UNIX && socket_type != SOCK_STREAM) {
        BIO_printf(bio_err,
                   "Can't use unix sockets and datagrams together\n");
        goto end;
    }
#endif

    if (!app_passwd(passarg, dpassarg, &pass, &dpass)) {
        BIO_printf(bio_err, "Error getting password\n");
        goto end;
    }

    if (s_key_file == NULL)
        s_key_file = s_cert_file;

    if (s_key_file2 == NULL)
        s_key_file2 = s_cert_file2;

    if (!load_excert(&exc))
        goto end;

    if (nocert == 0) {
        s_key = load_key(s_key_file, s_key_format, 0, pass, e,
                         "server certificate private key file");
        if (!s_key) {
            ERR_print_errors(bio_err);
            goto end;
        }

        s_cert = load_cert(s_cert_file, s_cert_format,
                           "server certificate file");

        if (!s_cert) {
            ERR_print_errors(bio_err);
            goto end;
        }
        if (s_chain_file) {
            if (!load_certs(s_chain_file, &s_chain, FORMAT_PEM, NULL,
                            "server certificate chain"))
                goto end;
        }

        if (tlsextcbp.servername) {
            s_key2 = load_key(s_key_file2, s_key_format, 0, pass, e,
                              "second server certificate private key file");
            if (!s_key2) {
                ERR_print_errors(bio_err);
                goto end;
            }

            s_cert2 = load_cert(s_cert_file2, s_cert_format,
                                "second server certificate file");

            if (!s_cert2) {
                ERR_print_errors(bio_err);
                goto end;
            }
        }
    }
#if !defined(OPENSSL_NO_NEXTPROTONEG)
    if (next_proto_neg_in) {
        unsigned short len;
        next_proto.data = next_protos_parse(&len, next_proto_neg_in);
        if (next_proto.data == NULL)
            goto end;
        next_proto.len = len;
    } else {
        next_proto.data = NULL;
    }
#endif
    alpn_ctx.data = NULL;
    if (alpn_in) {
        unsigned short len;
        alpn_ctx.data = next_protos_parse(&len, alpn_in);
        if (alpn_ctx.data == NULL)
            goto end;
        alpn_ctx.len = len;
    }

    if (crl_file) {
        X509_CRL *crl;
        crl = load_crl(crl_file, crl_format);
        if (!crl) {
            BIO_puts(bio_err, "Error loading CRL\n");
            ERR_print_errors(bio_err);
            goto end;
        }
        crls = sk_X509_CRL_new_null();
        if (!crls || !sk_X509_CRL_push(crls, crl)) {
            BIO_puts(bio_err, "Error adding CRL\n");
            ERR_print_errors(bio_err);
            X509_CRL_free(crl);
            goto end;
        }
    }

    if (s_dcert_file) {

        if (s_dkey_file == NULL)
            s_dkey_file = s_dcert_file;

        s_dkey = load_key(s_dkey_file, s_dkey_format,
                          0, dpass, e, "second certificate private key file");
        if (!s_dkey) {
            ERR_print_errors(bio_err);
            goto end;
        }

        s_dcert = load_cert(s_dcert_file, s_dcert_format,
                            "second server certificate file");

        if (!s_dcert) {
            ERR_print_errors(bio_err);
            goto end;
        }
        if (s_dchain_file) {
            if (!load_certs(s_dchain_file, &s_dchain, FORMAT_PEM, NULL,
                            "second server certificate chain"))
                goto end;
        }

    }

    if (!app_RAND_load_file(NULL, 1) && inrand == NULL
        && !RAND_status()) {
        BIO_printf(bio_err,
                   "warning, not much extra random data, consider using the -rand option\n");
    }
    if (inrand != NULL)
        BIO_printf(bio_err, "%ld semi-random bytes loaded\n",
                   app_RAND_load_files(inrand));

    if (bio_s_out == NULL) {
        if (s_quiet && !s_debug) {
            bio_s_out = BIO_new(BIO_s_null());
            if (s_msg && !bio_s_msg)
                bio_s_msg = dup_bio_out(FORMAT_TEXT);
        } else {
            if (bio_s_out == NULL)
                bio_s_out = dup_bio_out(FORMAT_TEXT);
        }
    }
#if !defined(OPENSSL_NO_RSA) || !defined(OPENSSL_NO_DSA) || !defined(OPENSSL_NO_EC)
    if (nocert)
#endif
    {
        s_cert_file = NULL;
        s_key_file = NULL;
        s_dcert_file = NULL;
        s_dkey_file = NULL;
        s_cert_file2 = NULL;
        s_key_file2 = NULL;
    }

    ctx = SSL_CTX_new(meth);
    if (ctx == NULL) {
        ERR_print_errors(bio_err);
        goto end;
    }
    if (sdebug)
        ssl_ctx_security_debug(ctx, sdebug);
    if (ssl_config) {
        if (SSL_CTX_config(ctx, ssl_config) == 0) {
            BIO_printf(bio_err, "Error using configuration \"%s\"\n",
                       ssl_config);
        ERR_print_errors(bio_err);
        goto end;
        }
    }

    if (session_id_prefix) {
        if (strlen(session_id_prefix) >= 32)
            BIO_printf(bio_err,
                       "warning: id_prefix is too long, only one new session will be possible\n");
        if (!SSL_CTX_set_generate_session_id(ctx, generate_session_id)) {
            BIO_printf(bio_err, "error setting 'id_prefix'\n");
            ERR_print_errors(bio_err);
            goto end;
        }
        BIO_printf(bio_err, "id_prefix '%s' set.\n", session_id_prefix);
    }
    SSL_CTX_set_quiet_shutdown(ctx, 1);
    if (exc)
        ssl_ctx_set_excert(ctx, exc);

    if (state)
        SSL_CTX_set_info_callback(ctx, apps_ssl_info_callback);
    if (no_cache)
        SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
    else if (ext_cache)
        init_session_cache_ctx(ctx);
    else
        SSL_CTX_sess_set_cache_size(ctx, 128);

    if (async) {
        SSL_CTX_set_mode(ctx, SSL_MODE_ASYNC);
    }

#ifndef OPENSSL_NO_SRTP
    if (srtp_profiles != NULL) {
        /* Returns 0 on success! */
        if (SSL_CTX_set_tlsext_use_srtp(ctx, srtp_profiles) != 0) {
            BIO_printf(bio_err, "Error setting SRTP profile\n");
            ERR_print_errors(bio_err);
            goto end;
        }
    }
#endif

    if (!ctx_set_verify_locations(ctx, CAfile, CApath, noCAfile, noCApath)) {
        ERR_print_errors(bio_err);
        goto end;
    }
    if (vpmtouched && !SSL_CTX_set1_param(ctx, vpm)) {
        BIO_printf(bio_err, "Error setting verify params\n");
        ERR_print_errors(bio_err);
        goto end;
    }

    ssl_ctx_add_crls(ctx, crls, 0);
    if (!config_ctx(cctx, ssl_args, ctx))
        goto end;

    if (!ssl_load_stores(ctx, vfyCApath, vfyCAfile, chCApath, chCAfile,
                         crls, crl_download)) {
        BIO_printf(bio_err, "Error loading store locations\n");
        ERR_print_errors(bio_err);
        goto end;
    }

    if (s_cert2) {
        ctx2 = SSL_CTX_new(meth);
        if (ctx2 == NULL) {
            ERR_print_errors(bio_err);
            goto end;
        }
    }

    if (ctx2) {
        BIO_printf(bio_s_out, "Setting secondary ctx parameters\n");

        if (sdebug)
            ssl_ctx_security_debug(ctx, sdebug);

        if (session_id_prefix) {
            if (strlen(session_id_prefix) >= 32)
                BIO_printf(bio_err,
                           "warning: id_prefix is too long, only one new session will be possible\n");
            if (!SSL_CTX_set_generate_session_id(ctx2, generate_session_id)) {
                BIO_printf(bio_err, "error setting 'id_prefix'\n");
                ERR_print_errors(bio_err);
                goto end;
            }
            BIO_printf(bio_err, "id_prefix '%s' set.\n", session_id_prefix);
        }
        SSL_CTX_set_quiet_shutdown(ctx2, 1);
        if (exc)
            ssl_ctx_set_excert(ctx2, exc);

        if (state)
            SSL_CTX_set_info_callback(ctx2, apps_ssl_info_callback);

        if (no_cache)
            SSL_CTX_set_session_cache_mode(ctx2, SSL_SESS_CACHE_OFF);
        else if (ext_cache)
            init_session_cache_ctx(ctx2);
        else
            SSL_CTX_sess_set_cache_size(ctx2, 128);

        if (async)
            SSL_CTX_set_mode(ctx2, SSL_MODE_ASYNC);

        if ((!SSL_CTX_load_verify_locations(ctx2, CAfile, CApath)) ||
            (!SSL_CTX_set_default_verify_paths(ctx2))) {
            ERR_print_errors(bio_err);
        }
        if (vpmtouched && !SSL_CTX_set1_param(ctx2, vpm)) {
            BIO_printf(bio_err, "Error setting verify params\n");
            ERR_print_errors(bio_err);
            goto end;
        }

        ssl_ctx_add_crls(ctx2, crls, 0);
        if (!config_ctx(cctx, ssl_args, ctx2))
            goto end;
    }
#ifndef OPENSSL_NO_NEXTPROTONEG
    if (next_proto.data)
        SSL_CTX_set_next_protos_advertised_cb(ctx, next_proto_cb,
                                              &next_proto);
#endif
    if (alpn_ctx.data)
        SSL_CTX_set_alpn_select_cb(ctx, alpn_cb, &alpn_ctx);

#ifndef OPENSSL_NO_DH
    if (!no_dhe) {
        DH *dh = NULL;

        if (dhfile)
            dh = load_dh_param(dhfile);
        else if (s_cert_file)
            dh = load_dh_param(s_cert_file);

        if (dh != NULL) {
            BIO_printf(bio_s_out, "Setting temp DH parameters\n");
        } else {
            BIO_printf(bio_s_out, "Using default temp DH parameters\n");
        }
        (void)BIO_flush(bio_s_out);

        if (dh == NULL)
            SSL_CTX_set_dh_auto(ctx, 1);
        else if (!SSL_CTX_set_tmp_dh(ctx, dh)) {
            BIO_puts(bio_err, "Error setting temp DH parameters\n");
            ERR_print_errors(bio_err);
            DH_free(dh);
            goto end;
        }

        if (ctx2) {
            if (!dhfile) {
                DH *dh2 = load_dh_param(s_cert_file2);
                if (dh2 != NULL) {
                    BIO_printf(bio_s_out, "Setting temp DH parameters\n");
                    (void)BIO_flush(bio_s_out);

                    DH_free(dh);
                    dh = dh2;
                }
            }
            if (dh == NULL)
                SSL_CTX_set_dh_auto(ctx2, 1);
            else if (!SSL_CTX_set_tmp_dh(ctx2, dh)) {
                BIO_puts(bio_err, "Error setting temp DH parameters\n");
                ERR_print_errors(bio_err);
                DH_free(dh);
                goto end;
            }
        }
        DH_free(dh);
    }
#endif

    if (!set_cert_key_stuff(ctx, s_cert, s_key, s_chain, build_chain))
        goto end;

    if (s_serverinfo_file != NULL
        && !SSL_CTX_use_serverinfo_file(ctx, s_serverinfo_file)) {
        ERR_print_errors(bio_err);
        goto end;
    }

    if (ctx2 && !set_cert_key_stuff(ctx2, s_cert2, s_key2, NULL, build_chain))
        goto end;

    if (s_dcert != NULL) {
        if (!set_cert_key_stuff(ctx, s_dcert, s_dkey, s_dchain, build_chain))
            goto end;
    }

    if (no_resume_ephemeral) {
        SSL_CTX_set_not_resumable_session_callback(ctx,
                                                   not_resumable_sess_cb);

        if (ctx2)
            SSL_CTX_set_not_resumable_session_callback(ctx2,
                                                       not_resumable_sess_cb);
    }
#ifndef OPENSSL_NO_PSK
    if (psk_key != NULL) {
        if (s_debug)
            BIO_printf(bio_s_out,
                       "PSK key given, setting server callback\n");
        SSL_CTX_set_psk_server_callback(ctx, psk_server_cb);
    }

    if (!SSL_CTX_use_psk_identity_hint(ctx, psk_identity_hint)) {
        BIO_printf(bio_err, "error setting PSK identity hint to context\n");
        ERR_print_errors(bio_err);
        goto end;
    }
#endif

    SSL_CTX_set_verify(ctx, s_server_verify, verify_callback);
    if (!SSL_CTX_set_session_id_context(ctx,
                (void *)&s_server_session_id_context,
                sizeof s_server_session_id_context)) {
        BIO_printf(bio_err, "error setting session id context\n");
        ERR_print_errors(bio_err);
        goto end;
    }

    /* Set DTLS cookie generation and verification callbacks */
    SSL_CTX_set_cookie_generate_cb(ctx, generate_cookie_callback);
    SSL_CTX_set_cookie_verify_cb(ctx, verify_cookie_callback);

    if (ctx2) {
        SSL_CTX_set_verify(ctx2, s_server_verify, verify_callback);
        if (!SSL_CTX_set_session_id_context(ctx2,
                    (void *)&s_server_session_id_context,
                    sizeof s_server_session_id_context)) {
            BIO_printf(bio_err, "error setting session id context\n");
            ERR_print_errors(bio_err);
            goto end;
        }
        tlsextcbp.biodebug = bio_s_out;
        SSL_CTX_set_tlsext_servername_callback(ctx2, ssl_servername_cb);
        SSL_CTX_set_tlsext_servername_arg(ctx2, &tlsextcbp);
        SSL_CTX_set_tlsext_servername_callback(ctx, ssl_servername_cb);
        SSL_CTX_set_tlsext_servername_arg(ctx, &tlsextcbp);
    }

#ifndef OPENSSL_NO_SRP
    if (srp_verifier_file != NULL) {
        srp_callback_parm.vb = SRP_VBASE_new(srpuserseed);
        srp_callback_parm.user = NULL;
        srp_callback_parm.login = NULL;
        if ((ret =
             SRP_VBASE_init(srp_callback_parm.vb,
                            srp_verifier_file)) != SRP_NO_ERROR) {
            BIO_printf(bio_err,
                       "Cannot initialize SRP verifier file \"%s\":ret=%d\n",
                       srp_verifier_file, ret);
            goto end;
        }
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, verify_callback);
        SSL_CTX_set_srp_cb_arg(ctx, &srp_callback_parm);
        SSL_CTX_set_srp_username_callback(ctx, ssl_srp_server_param_cb);
    } else
#endif
    if (CAfile != NULL) {
        SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(CAfile));

        if (ctx2)
            SSL_CTX_set_client_CA_list(ctx2, SSL_load_client_CA_file(CAfile));
    }
    if (s_tlsextstatus) {
        SSL_CTX_set_tlsext_status_cb(ctx, cert_status_cb);
        SSL_CTX_set_tlsext_status_arg(ctx, &tlscstatp);
        if (ctx2) {
            SSL_CTX_set_tlsext_status_cb(ctx2, cert_status_cb);
            SSL_CTX_set_tlsext_status_arg(ctx2, &tlscstatp);
        }
    }

    BIO_printf(bio_s_out, "ACCEPT\n");
    (void)BIO_flush(bio_s_out);
    if (rev)
        server_cb = rev_body;
    else if (www)
        server_cb = www_body;
    else
        server_cb = sv_body;
#ifdef AF_UNIX
    if (socket_family == AF_UNIX
        && unlink_unix_path)
        unlink(host);
#endif
    do_server(&accept_socket, host, port, socket_family, socket_type,
              server_cb, context, naccept);
    print_stats(bio_s_out, ctx);
    ret = 0;
 end:
    SSL_CTX_free(ctx);
    X509_free(s_cert);
    sk_X509_CRL_pop_free(crls, X509_CRL_free);
    X509_free(s_dcert);
    EVP_PKEY_free(s_key);
    EVP_PKEY_free(s_dkey);
    sk_X509_pop_free(s_chain, X509_free);
    sk_X509_pop_free(s_dchain, X509_free);
    OPENSSL_free(pass);
    OPENSSL_free(dpass);
    OPENSSL_free(host);
    OPENSSL_free(port);
    X509_VERIFY_PARAM_free(vpm);
    free_sessions();
    OPENSSL_free(tlscstatp.host);
    OPENSSL_free(tlscstatp.port);
    OPENSSL_free(tlscstatp.path);
    SSL_CTX_free(ctx2);
    X509_free(s_cert2);
    EVP_PKEY_free(s_key2);
    BIO_free(serverinfo_in);
#ifndef OPENSSL_NO_NEXTPROTONEG
    OPENSSL_free(next_proto.data);
#endif
    OPENSSL_free(alpn_ctx.data);
    ssl_excert_free(exc);
    sk_OPENSSL_STRING_free(ssl_args);
    SSL_CONF_CTX_free(cctx);
    BIO_free(bio_s_out);
    bio_s_out = NULL;
    BIO_free(bio_s_msg);
    bio_s_msg = NULL;
    return (ret);
}