void vnc_display_open(const char *id, Error **errp)
{
    VncDisplay *vd = vnc_display_find(id);
    QemuOpts *opts = qemu_opts_find(&qemu_vnc_opts, id);
    SocketAddress **saddr = NULL, **wsaddr = NULL;
    size_t nsaddr, nwsaddr;
    const char *share, *device_id;
    QemuConsole *con;
    bool password = false;
    bool reverse = false;
    const char *credid;
    bool sasl = false;
    int acl = 0;
    const char *tlsauthz;
    const char *saslauthz;
    int lock_key_sync = 1;
    int key_delay_ms;
    const char *audiodev;

    if (!vd) {
        error_setg(errp, "VNC display not active");
        return;
    }
    vnc_display_close(vd);

    if (!opts) {
        return;
    }

    reverse = qemu_opt_get_bool(opts, "reverse", false);
    if (vnc_display_get_addresses(opts, reverse, &saddr, &nsaddr,
                                  &wsaddr, &nwsaddr, errp) < 0) {
        goto fail;
    }

    password = qemu_opt_get_bool(opts, "password", false);
    if (password) {
        if (fips_get_state()) {
            error_setg(errp,
                       "VNC password auth disabled due to FIPS mode, "
                       "consider using the VeNCrypt or SASL authentication "
                       "methods as an alternative");
            goto fail;
        }
        if (!qcrypto_cipher_supports(
                QCRYPTO_CIPHER_ALG_DES_RFB, QCRYPTO_CIPHER_MODE_ECB)) {
            error_setg(errp,
                       "Cipher backend does not support DES RFB algorithm");
            goto fail;
        }
    }

    lock_key_sync = qemu_opt_get_bool(opts, "lock-key-sync", true);
    key_delay_ms = qemu_opt_get_number(opts, "key-delay-ms", 10);
    sasl = qemu_opt_get_bool(opts, "sasl", false);
#ifndef CONFIG_VNC_SASL
    if (sasl) {
        error_setg(errp, "VNC SASL auth requires cyrus-sasl support");
        goto fail;
    }
#endif /* CONFIG_VNC_SASL */
    credid = qemu_opt_get(opts, "tls-creds");
    if (credid) {
        Object *creds;
        creds = object_resolve_path_component(
            object_get_objects_root(), credid);
        if (!creds) {
            error_setg(errp, "No TLS credentials with id '%s'",
                       credid);
            goto fail;
        }
        vd->tlscreds = (QCryptoTLSCreds *)
            object_dynamic_cast(creds,
                                TYPE_QCRYPTO_TLS_CREDS);
        if (!vd->tlscreds) {
            error_setg(errp, "Object with id '%s' is not TLS credentials",
                       credid);
            goto fail;
        }
        object_ref(OBJECT(vd->tlscreds));

        if (vd->tlscreds->endpoint != QCRYPTO_TLS_CREDS_ENDPOINT_SERVER) {
            error_setg(errp,
                       "Expecting TLS credentials with a server endpoint");
            goto fail;
        }
    }
    if (qemu_opt_get(opts, "acl")) {
        error_report("The 'acl' option to -vnc is deprecated. "
                     "Please use the 'tls-authz' and 'sasl-authz' "
                     "options instead");
    }
    acl = qemu_opt_get_bool(opts, "acl", false);
    tlsauthz = qemu_opt_get(opts, "tls-authz");
    if (acl && tlsauthz) {
        error_setg(errp, "'acl' option is mutually exclusive with the "
                   "'tls-authz' option");
        goto fail;
    }
    if (tlsauthz && !vd->tlscreds) {
        error_setg(errp, "'tls-authz' provided but TLS is not enabled");
        goto fail;
    }

    saslauthz = qemu_opt_get(opts, "sasl-authz");
    if (acl && saslauthz) {
        error_setg(errp, "'acl' option is mutually exclusive with the "
                   "'sasl-authz' option");
        goto fail;
    }
    if (saslauthz && !sasl) {
        error_setg(errp, "'sasl-authz' provided but SASL auth is not enabled");
        goto fail;
    }

    share = qemu_opt_get(opts, "share");
    if (share) {
        if (strcmp(share, "ignore") == 0) {
            vd->share_policy = VNC_SHARE_POLICY_IGNORE;
        } else if (strcmp(share, "allow-exclusive") == 0) {
            vd->share_policy = VNC_SHARE_POLICY_ALLOW_EXCLUSIVE;
        } else if (strcmp(share, "force-shared") == 0) {
            vd->share_policy = VNC_SHARE_POLICY_FORCE_SHARED;
        } else {
            error_setg(errp, "unknown vnc share= option");
            goto fail;
        }
    } else {
        vd->share_policy = VNC_SHARE_POLICY_ALLOW_EXCLUSIVE;
    }
    vd->connections_limit = qemu_opt_get_number(opts, "connections", 32);

#ifdef CONFIG_VNC_JPEG
    vd->lossy = qemu_opt_get_bool(opts, "lossy", false);
#endif
    vd->non_adaptive = qemu_opt_get_bool(opts, "non-adaptive", false);
    /* adaptive updates are only used with tight encoding and
     * if lossy updates are enabled so we can disable all the
     * calculations otherwise */
    if (!vd->lossy) {
        vd->non_adaptive = true;
    }

    if (tlsauthz) {
        vd->tlsauthzid = g_strdup(tlsauthz);
    } else if (acl) {
        if (strcmp(vd->id, "default") == 0) {
            vd->tlsauthzid = g_strdup("vnc.x509dname");
        } else {
            vd->tlsauthzid = g_strdup_printf("vnc.%s.x509dname", vd->id);
        }
        vd->tlsauthz = QAUTHZ(qauthz_list_new(vd->tlsauthzid,
                                              QAUTHZ_LIST_POLICY_DENY,
                                              &error_abort));
    }
#ifdef CONFIG_VNC_SASL
    if (sasl) {
        if (saslauthz) {
            vd->sasl.authzid = g_strdup(saslauthz);
        } else if (acl) {
            if (strcmp(vd->id, "default") == 0) {
                vd->sasl.authzid = g_strdup("vnc.username");
            } else {
                vd->sasl.authzid = g_strdup_printf("vnc.%s.username", vd->id);
            }
            vd->sasl.authz = QAUTHZ(qauthz_list_new(vd->sasl.authzid,
                                                    QAUTHZ_LIST_POLICY_DENY,
                                                    &error_abort));
        }
    }
#endif

    if (vnc_display_setup_auth(&vd->auth, &vd->subauth,
                               vd->tlscreds, password,
                               sasl, false, errp) < 0) {
        goto fail;
    }
    trace_vnc_auth_init(vd, 0, vd->auth, vd->subauth);

    if (vnc_display_setup_auth(&vd->ws_auth, &vd->ws_subauth,
                               vd->tlscreds, password,
                               sasl, true, errp) < 0) {
        goto fail;
    }
    trace_vnc_auth_init(vd, 1, vd->ws_auth, vd->ws_subauth);

#ifdef CONFIG_VNC_SASL
    if (sasl) {
        int saslErr = sasl_server_init(NULL, "qemu");

        if (saslErr != SASL_OK) {
            error_setg(errp, "Failed to initialize SASL auth: %s",
                       sasl_errstring(saslErr, NULL, NULL));
            goto fail;
        }
    }
#endif
    vd->lock_key_sync = lock_key_sync;
    if (lock_key_sync) {
        vd->led = qemu_add_led_event_handler(kbd_leds, vd);
    }
    vd->ledstate = 0;

    audiodev = qemu_opt_get(opts, "audiodev");
    if (audiodev) {
        vd->audio_state = audio_state_by_name(audiodev);
        if (!vd->audio_state) {
            error_setg(errp, "Audiodev '%s' not found", audiodev);
            goto fail;
        }
    }

    device_id = qemu_opt_get(opts, "display");
    if (device_id) {
        int head = qemu_opt_get_number(opts, "head", 0);
        Error *err = NULL;

        con = qemu_console_lookup_by_device_name(device_id, head, &err);
        if (err) {
            error_propagate(errp, err);
            goto fail;
        }
    } else {
        con = NULL;
    }

    if (con != vd->dcl.con) {
        qkbd_state_free(vd->kbd);
        unregister_displaychangelistener(&vd->dcl);
        vd->dcl.con = con;
        register_displaychangelistener(&vd->dcl);
        vd->kbd = qkbd_state_init(vd->dcl.con);
    }
    qkbd_state_set_delay(vd->kbd, key_delay_ms);

    if (saddr == NULL) {
        goto cleanup;
    }

    if (reverse) {
        if (vnc_display_connect(vd, saddr, nsaddr, wsaddr, nwsaddr, errp) < 0) {
            goto fail;
        }
    } else {
        if (vnc_display_listen(vd, saddr, nsaddr, wsaddr, nwsaddr, errp) < 0) {
            goto fail;
        }
    }

    if (qemu_opt_get(opts, "to")) {
        vnc_display_print_local_addr(vd);
    }

 cleanup:
    vnc_free_addresses(&saddr, &nsaddr);
    vnc_free_addresses(&wsaddr, &nwsaddr);
    return;

fail:
    vnc_display_close(vd);
    goto cleanup;
}