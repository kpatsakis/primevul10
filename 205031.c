static int init_ssl_connection(SSL *con)
{
    int i;
    const char *str;
    X509 *peer;
    long verify_err;
    char buf[BUFSIZ];
#if !defined(OPENSSL_NO_NEXTPROTONEG)
    const unsigned char *next_proto_neg;
    unsigned next_proto_neg_len;
#endif
    unsigned char *exportedkeymat;

#ifndef OPENSSL_NO_DTLS
    if(dtlslisten) {
        BIO_ADDR *client = NULL;

        if ((client = BIO_ADDR_new()) == NULL) {
            BIO_printf(bio_err, "ERROR - memory\n");
            return 0;
        }
        i = DTLSv1_listen(con, client);
        if (i > 0) {
            BIO *wbio;
            int fd = -1;

            wbio = SSL_get_wbio(con);
            if(wbio) {
                BIO_get_fd(wbio, &fd);
            }

            if(!wbio || BIO_connect(fd, client, 0) == 0) {
                BIO_printf(bio_err, "ERROR - unable to connect\n");
                BIO_ADDR_free(client);
                return 0;
            }
            BIO_ADDR_free(client);
            dtlslisten = 0;
            i = SSL_accept(con);
        }
    } else
#endif

    do {
        i = SSL_accept(con);

#ifdef CERT_CB_TEST_RETRY
        {
            while (i <= 0 && SSL_get_error(con, i) == SSL_ERROR_WANT_X509_LOOKUP
                    && SSL_get_state(con) == TLS_ST_SR_CLNT_HELLO) {
                BIO_printf(bio_err,
                       "LOOKUP from certificate callback during accept\n");
                i = SSL_accept(con);
            }
        }
#endif

#ifndef OPENSSL_NO_SRP
        while (i <= 0 && SSL_get_error(con, i) == SSL_ERROR_WANT_X509_LOOKUP) {
            BIO_printf(bio_s_out, "LOOKUP during accept %s\n",
                       srp_callback_parm.login);
            SRP_user_pwd_free(srp_callback_parm.user);
            srp_callback_parm.user =
                SRP_VBASE_get1_by_user(srp_callback_parm.vb,
                                       srp_callback_parm.login);
            if (srp_callback_parm.user)
                BIO_printf(bio_s_out, "LOOKUP done %s\n",
                           srp_callback_parm.user->info);
            else
                BIO_printf(bio_s_out, "LOOKUP not successful\n");
            i = SSL_accept(con);
        }
#endif
    } while (i < 0 && SSL_waiting_for_async(con));

    if (i <= 0) {
        if ((dtlslisten && i == 0)
                || (!dtlslisten && BIO_sock_should_retry(i))) {
            BIO_printf(bio_s_out, "DELAY\n");
            return (1);
        }

        BIO_printf(bio_err, "ERROR\n");

        verify_err = SSL_get_verify_result(con);
        if (verify_err != X509_V_OK) {
            BIO_printf(bio_err, "verify error:%s\n",
                       X509_verify_cert_error_string(verify_err));
        }
        /* Always print any error messages */
        ERR_print_errors(bio_err);
        return (0);
    }

    if (s_brief)
        print_ssl_summary(con);

    PEM_write_bio_SSL_SESSION(bio_s_out, SSL_get_session(con));

    peer = SSL_get_peer_certificate(con);
    if (peer != NULL) {
        BIO_printf(bio_s_out, "Client certificate\n");
        PEM_write_bio_X509(bio_s_out, peer);
        X509_NAME_oneline(X509_get_subject_name(peer), buf, sizeof buf);
        BIO_printf(bio_s_out, "subject=%s\n", buf);
        X509_NAME_oneline(X509_get_issuer_name(peer), buf, sizeof buf);
        BIO_printf(bio_s_out, "issuer=%s\n", buf);
        X509_free(peer);
    }

    if (SSL_get_shared_ciphers(con, buf, sizeof buf) != NULL)
        BIO_printf(bio_s_out, "Shared ciphers:%s\n", buf);
    str = SSL_CIPHER_get_name(SSL_get_current_cipher(con));
    ssl_print_sigalgs(bio_s_out, con);
#ifndef OPENSSL_NO_EC
    ssl_print_point_formats(bio_s_out, con);
    ssl_print_curves(bio_s_out, con, 0);
#endif
    BIO_printf(bio_s_out, "CIPHER is %s\n", (str != NULL) ? str : "(NONE)");

#if !defined(OPENSSL_NO_NEXTPROTONEG)
    SSL_get0_next_proto_negotiated(con, &next_proto_neg, &next_proto_neg_len);
    if (next_proto_neg) {
        BIO_printf(bio_s_out, "NEXTPROTO is ");
        BIO_write(bio_s_out, next_proto_neg, next_proto_neg_len);
        BIO_printf(bio_s_out, "\n");
    }
#endif
#ifndef OPENSSL_NO_SRTP
    {
        SRTP_PROTECTION_PROFILE *srtp_profile
            = SSL_get_selected_srtp_profile(con);

        if (srtp_profile)
            BIO_printf(bio_s_out, "SRTP Extension negotiated, profile=%s\n",
                       srtp_profile->name);
    }
#endif
    if (SSL_session_reused(con))
        BIO_printf(bio_s_out, "Reused session-id\n");
    BIO_printf(bio_s_out, "Secure Renegotiation IS%s supported\n",
               SSL_get_secure_renegotiation_support(con) ? "" : " NOT");
    if (keymatexportlabel != NULL) {
        BIO_printf(bio_s_out, "Keying material exporter:\n");
        BIO_printf(bio_s_out, "    Label: '%s'\n", keymatexportlabel);
        BIO_printf(bio_s_out, "    Length: %i bytes\n", keymatexportlen);
        exportedkeymat = app_malloc(keymatexportlen, "export key");
        if (!SSL_export_keying_material(con, exportedkeymat,
                                        keymatexportlen,
                                        keymatexportlabel,
                                        strlen(keymatexportlabel),
                                        NULL, 0, 0)) {
            BIO_printf(bio_s_out, "    Error\n");
        } else {
            BIO_printf(bio_s_out, "    Keying material: ");
            for (i = 0; i < keymatexportlen; i++)
                BIO_printf(bio_s_out, "%02X", exportedkeymat[i]);
            BIO_printf(bio_s_out, "\n");
        }
        OPENSSL_free(exportedkeymat);
    }

    return (1);
}