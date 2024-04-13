static Jsi_RC WebSocketConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    if (Jsi_InterpAccess(interp, NULL, JSI_INTACCESS_NETWORK ) != JSI_OK)
        return Jsi_LogError("WebSocket disallowed by Interp.noNetwork option");
    jsi_wsCmdObj *cmdPtr;
    Jsi_Value *toacc = NULL;
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 0);

    cmdPtr = (jsi_wsCmdObj*)Jsi_Calloc(1, sizeof(*cmdPtr));
    cmdPtr->sig = JWS_SIG_OBJ;
    cmdPtr->_ = &wsObjCmd;
    cmdPtr->_->newCnt++;
    cmdPtr->_->activeCnt++;
    cmdPtr->port = 8080;
    cmdPtr->formParams = jsi_wsparam_str;
    cmdPtr->maxUpload = 100000;
    cmdPtr->interp = interp;
    cmdPtr->ietf_version = -1;
    cmdPtr->bufferPwr2 = 0;
    cmdPtr->ws_gid = -1;
    cmdPtr->ws_uid = -1;
    cmdPtr->startTime = time(NULL);
    cmdPtr->hasOpts = 1;
    cmdPtr->includeFile = "include.shtml";
    cmdPtr->jsiFnPattern = "jsig*.js";
    if ((arg != NULL && !Jsi_ValueIsNull(interp,arg))
        && Jsi_OptionsProcess(interp, WSOptions, cmdPtr, arg, 0) < 0) {
bail:
        jsi_wswebsocketObjFree(interp, cmdPtr);
        return JSI_ERROR;
    }
    if (!cmdPtr->udata) {
        cmdPtr->udata = Jsi_ValueNewObj(interp, NULL);
        Jsi_IncrRefCount(interp, cmdPtr->udata);
    }
    Jsi_PathNormalize(interp, &cmdPtr->rootdir);

    if (cmdPtr->headers && (Jsi_ValueGetLength(interp, cmdPtr->headers)%2)) {
        Jsi_LogError("Odd header length");
        goto bail;
    }
    const char *up = cmdPtr->urlPrefix, *ur = cmdPtr->urlRedirect;
    if (up && ur && Jsi_Strncmp(ur, up, Jsi_Strlen(up))) {
        Jsi_LogError("urlRedirect does not start with urlPrefix");
        goto bail;
    }
    const char* subprot = (cmdPtr->protocol&&cmdPtr->protocol[0]?cmdPtr->protocol:"ws");
    if (cmdPtr->protocol && !cmdPtr->protocol[0])
        Jsi_LogWarn("empty protocol string: forcing to 'ws'");
    cmdPtr->protocols[JWS_PROTOCOL_HTTP].name="http-only";
    cmdPtr->protocols[JWS_PROTOCOL_HTTP].callback=jsi_wscallback_http;
    cmdPtr->protocols[JWS_PROTOCOL_HTTP].per_session_data_size=sizeof(jsi_wsUser);
    cmdPtr->protocols[JWS_PROTOCOL_WEBSOCK].name=subprot;
    cmdPtr->protocols[JWS_PROTOCOL_WEBSOCK].callback=jsi_wscallback_websock;
    cmdPtr->protocols[JWS_PROTOCOL_WEBSOCK].per_session_data_size=sizeof(jsi_wsUser);

    if (cmdPtr->bufferPwr2 == 0)
        cmdPtr->bufferPwr2 = 16;
    if (cmdPtr->bufferPwr2>0) {
        if (cmdPtr->bufferPwr2>20) {
            Jsi_LogError("bufferPwr2 not in 0-20: %d", cmdPtr->bufferPwr2);
            goto bail;
        }
        cmdPtr->protocols[JWS_PROTOCOL_WEBSOCK].rx_buffer_size=(1<<cmdPtr->bufferPwr2);
    }

    cmdPtr->pssTable = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, jsi_wsfreePss);
    if (cmdPtr->onModify)
        cmdPtr->fileHash = Jsi_HashNew(interp, JSI_KEYS_STRING, jsi_wsfreeFile);

    cmdPtr->info.port = (cmdPtr->client ? CONTEXT_PORT_NO_LISTEN : cmdPtr->port);
    cmdPtr->info.user = cmdPtr;
    cmdPtr->info.iface = cmdPtr->interface ? Jsi_ValueString(interp, cmdPtr->interface, NULL) : NULL;
    if (cmdPtr->local && !cmdPtr->info.iface)
        cmdPtr->info.iface = "lo";
#ifdef __WIN32
    cmdPtr->info.iface = NULL;
#endif
    cmdPtr->info.protocols = cmdPtr->protocols;
    if (!cmdPtr->noCompress)
        cmdPtr->info.extensions = jsi_lws_exts;

    cmdPtr->info.ssl_cert_filepath = cmdPtr->ssl_cert_filepath;
    cmdPtr->info.ssl_private_key_filepath = cmdPtr->ssl_private_key_filepath;
    cmdPtr->info.gid = cmdPtr->ws_gid;
    cmdPtr->info.uid = cmdPtr->ws_uid;
    cmdPtr->opts = LWS_SERVER_OPTION_SKIP_SERVER_CANONICAL_NAME|LWS_SERVER_OPTION_VALIDATE_UTF8;
    cmdPtr->info.options = cmdPtr->opts;
    cmdPtr->info.max_http_header_pool = 16;
    cmdPtr->info.timeout_secs = 5;
    cmdPtr->info.ssl_cipher_list = "ECDHE-ECDSA-AES256-GCM-SHA384:"
                   "ECDHE-RSA-AES256-GCM-SHA384:"
                   "DHE-RSA-AES256-GCM-SHA384:"
                   "ECDHE-RSA-AES256-SHA384:"
                   "HIGH:!aNULL:!eNULL:!EXPORT:"
                   "!DES:!MD5:!PSK:!RC4:!HMAC_SHA1:"
                   "!SHA1:!DHE-RSA-AES128-GCM-SHA256:"
                   "!DHE-RSA-AES128-SHA256:"
                   "!AES128-GCM-SHA256:"
                   "!AES128-SHA256:"
                   "!DHE-RSA-AES256-SHA256:"
                   "!AES256-GCM-SHA384:"
                   "!AES256-SHA256";

    lws_set_log_level(cmdPtr->debug>255?cmdPtr->debug/256:0, NULL);
    // TODO: WS2.2 Still leaks a small amount if server port unavailable.
    if (!cmdPtr->client)
        cmdPtr->info.options |= LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
    cmdPtr->context = lws_create_context(&cmdPtr->info);
    if (cmdPtr->context == NULL) {
fail:
        Jsi_LogError("libwebsocket init failed on port %d (try another port?)", cmdPtr->info.port);
        goto bail;
    }
    if (cmdPtr->info.options & LWS_SERVER_OPTION_EXPLICIT_VHOSTS) {
        cmdPtr->info.options &= ~LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
        if (!lws_create_vhost(cmdPtr->context, &cmdPtr->info))
            goto fail;
    }

    if (cmdPtr->client) {
        struct lws_client_connect_info lci = {};
        lci.context = cmdPtr->context;
        lci.address = cmdPtr->address ? Jsi_ValueString(cmdPtr->interp, cmdPtr->address, NULL) : "127.0.0.1";
        lci.port = cmdPtr->port;
        lci.ssl_connection = cmdPtr->use_ssl;
        lci.path = Jsi_ValueString(cmdPtr->interp, cmdPtr->rootdir, NULL);
        lci.host = cmdPtr->clientHost?cmdPtr->clientHost:lws_canonical_hostname( cmdPtr->context );
        lci.origin = cmdPtr->clientOrigin?cmdPtr->clientOrigin:"origin";
        lci.protocol = cmdPtr->protocols[JWS_PROTOCOL_WEBSOCK].name;
        lci.ietf_version_or_minus_one = cmdPtr->ietf_version;
#if (LWS_LIBRARY_VERSION_MAJOR>1)
        if (cmdPtr->post)
            lci.method = "POST";
        else if (!Jsi_Strcmp(subprot, "get"))
            lci.method = "GET";
#endif

        if (NULL == lws_client_connect_via_info(&lci))
        {
            Jsi_LogError("websock connect failed");
            jsi_wswebsocketObjFree(interp, cmdPtr);
            return JSI_ERROR;
        }
    } else if (cmdPtr->port == 0) {
        // Extract actually used port.
        char *cp, cbuf[JSI_BUFSIZ*2];
        cbuf[0] = 0;
        lws_json_dump_context(cmdPtr->context, cbuf, sizeof(cbuf), 0);
        cp = Jsi_Strstr(cbuf, "\"port\":\"");
        if (cp)
            cmdPtr->port = atoi(cp+8);
    }

    cmdPtr->event = Jsi_EventNew(interp, jsi_wswebsockUpdate, cmdPtr);
    if (Jsi_FunctionIsConstructor(funcPtr)) {
        toacc = _this;
    } else {
        Jsi_Obj *o = Jsi_ObjNew(interp);
        Jsi_PrototypeObjSet(interp, "WebSocket", o);
        Jsi_ValueMakeObject(interp, ret, o);
        toacc = *ret;
    }

    Jsi_Obj *fobj = Jsi_ValueGetObj(interp, toacc);
    if ((cmdPtr->objId = Jsi_UserObjNew(interp, &websockobject, fobj, cmdPtr))<0) {
        goto bail;
    }
    Jsi_UserObjName(interp, toacc, &cmdPtr->cName);

    cmdPtr->handlers = Jsi_HashNew(interp, JSI_KEYS_STRING, jsi_wsfreeHandlers);
    if (cmdPtr->extHandlers) {
        jsi_wsHandlerAdd(interp, cmdPtr, ".jsi",   "Jspp",     1);
        jsi_wsHandlerAdd(interp, cmdPtr, ".htmli", "Htmlpp",   1);
        jsi_wsHandlerAdd(interp, cmdPtr, ".cssi",  "Csspp",    1);
    }
    cmdPtr->fobj = fobj;
#ifdef LWS_LIBRARY_VERSION_NUMBER
    Jsi_JSONParseFmt(interp, &cmdPtr->version, "{libVer:\"%s\", hdrVer:\"%s\", hdrNum:%d, pkgVer:%d}",
        (char *)lws_get_library_version(), LWS_LIBRARY_VERSION, LWS_LIBRARY_VERSION_NUMBER, jsi_WsPkgVersion);
#endif
    return JSI_OK;
}