Jsi_RC Jsi_LogMsg(Jsi_Interp *interp, uint code, const char *format,...) {
    if (Jsi_InterpGone(interp))
        return JSI_ERROR;
    va_list va;
    va_start (va, format);
    char pbuf[JSI_BUFSIZ/8] = "";
    char buf[JSI_BUFSIZ/2];
    const char *term = "", *pterm=pbuf;
    static char lastMsg[JSI_BUFSIZ/2] = "";
    static int lastCnt = 0;
    static Jsi_Interp *LastInterp = NULL;
    Jsi_Interp *lastInterp = LastInterp;
    const char *emsg = buf, *mt;
    int islog, line = 0, lofs = 0, noDups=0;
    bool isHelp = (format[0]=='.' && !Jsi_Strncmp(format, "...", 3));
    Jsi_OptionSpec *oep = interp->parseMsgSpec;
    const char *pps = "", *curFile = "";
    char *ss = interp->lastPushStr;
    
    if (interp==NULL)
        interp = jsiIntData.mainInterp;
    LastInterp = interp;
    if (lastInterp != interp)
        noDups = 1;
    
    /* Filter out try/catch (TODO: and non-syntax errors??). */
    if (interp == NULL) {
//nullInterp:
        if (logHook)
            (*logHook)(format, va);
        else {
            vfprintf(stderr, format, va);
            fputc('\n', stderr);
        }
        va_end(va);
        return JSI_ERROR;
    }
    curFile = jsi_GetCurFile(interp);
    switch (code) {
        case JSI_LOG_INFO:  if (!interp->logOpts.Info) goto bail; break;
        case JSI_LOG_WARN:  if (!interp->logOpts.Warn) goto bail; break;
        case JSI_LOG_DEBUG: if (!interp->logOpts.Debug && !jsi_LogEnabled(interp, code)) goto bail; break;
        case JSI_LOG_TRACE: if (!interp->logOpts.Trace && !jsi_LogEnabled(interp, code)) goto bail; break;
        case JSI_LOG_TEST:  if (!interp->logOpts.Test && !jsi_LogEnabled(interp, code)) goto bail; break;
        case JSI_LOG_PARSE: break; //if (!interp->parent) goto nullInterp; break;
        case JSI_LOG_ERROR: {
            if (!interp->logOpts.Error) goto bail;
            if ((interp->framePtr->tryDepth - interp->framePtr->withDepth)>0 && interp->inParse<=0 
                && (!interp->tryList || !(interp->tryList->inCatch|interp->tryList->inFinal))) { 
                /* Should only do the first or traceback? */
                if (!interp->errMsgBuf[0]) {
                    vsnprintf(interp->errMsgBuf, sizeof(interp->errMsgBuf), format, va);
                    //interp->errMsgBuf[sizeof(interp->errMsgBuf)-1] = 0;
                    interp->errFile =  jsi_GetCurFile(interp);
                    interp->errLine = (interp->curIp?interp->curIp->Line:0);
                    emsg = interp->errMsgBuf;
                }
                goto done;
            }
            interp->logErrorCnt++;
            break;
        }
    }
    mt = (code <= JSI__LOGLAST ? jsi_LogCodes[code] : "");
    if (isHelp) mt = "help";
    assert((JSI__LOGLAST+2) == (sizeof(jsi_LogCodes)/sizeof(jsi_LogCodes[0])));
    if (!Jsi_Strchr(format,'\n')) term = "\n";
    if (interp->strict && interp->lastParseOpt)
        ss = (char*)Jsi_ValueToString(interp, interp->lastParseOpt, NULL);
    if (code != JSI_LOG_INFO && code < JSI_LOG_TEST && interp && ss && ss[0]) {
        char psbuf[JSI_BUFSIZ/6];
        if (Jsi_Strchr(ss,'%')) {
            char *s = ss, *sd = psbuf;
            int plen=0, llen = sizeof(psbuf)-2;
            while (*s && plen<llen) {
                if (*s == '%')
                    sd[plen++] = '%';
                sd[plen++] = *s;
                s++;
            }
            sd[plen] = 0;
            ss = psbuf;
        }
        while (*ss && isspace(*ss))
            ss++;
        if (*ss && !isHelp)
            snprintf(pbuf, sizeof(pbuf), "    (at or near \"%s\")\n", ss);
    }
    pbuf[sizeof(pbuf)-1] = 0;
    if (interp->inParse && interp->parseLine) {
        line = interp->parseLine->first_line;
        lofs = interp->parseLine->first_column;
    } else if (interp->inParse && interp->parsePs) {
        line = interp->parsePs->lexer->cur_line;
        lofs = interp->parsePs->lexer->cur_char;
    } else if (interp->curIp) {
        if (interp->callerErr && interp->framePtr && interp->framePtr->parent) {
            jsi_Frame *fptr = interp->framePtr->parent;
            line = fptr->line;
            lofs = 0;
            curFile = fptr->fileName;
        } else {
            line = interp->curIp->Line;
            lofs = interp->curIp->Lofs;
            if (line<=0)
                line = interp->framePtr->line;
        }
    }
    islog = (interp->parent && interp->debugOpts.msgCallback && code != JSI_LOG_BUG);
    Jsi_DString pStr;
    Jsi_DSInit(&pStr);
    if (oep) {
        if (oep->id != JSI_OPTION_CUSTOM || !oep->custom)
            pps = Jsi_DSPrintf(&pStr, "for option \"%s\": ", oep->name);
        else {
            Jsi_OptionCustom* cust = Jsi_OptionCustomBuiltin(oep->custom);
            pps = Jsi_DSPrintf(&pStr, "for %s option \"%s\": ", (cust?cust->name:""), oep->name);
        }
    }
    char *cpt;
    if (curFile && interp->logOpts.ftail && (cpt =Jsi_Strrchr(curFile, '/')) && cpt[1])
        curFile = cpt+1;
    if (curFile && curFile[0] && Jsi_Strchr(curFile,'%')==0 && !islog) {
        if (!interp->subOpts.logColNums)
            snprintf(buf, sizeof(buf), "%s:%d: %s: %s%s%s%s",  curFile, line, mt, pps, format, pterm, term);
        else
            snprintf(buf, sizeof(buf), "%s:%d.%d: %s: %s%s%s%s",  curFile, line, lofs, pps, mt,format, pterm, term);
    } else {
        snprintf(buf, sizeof(buf), "%s: %s%s%s%s", mt, pps, format, pterm, term);
    }
    Jsi_DSFree(&pStr);
    buf[sizeof(buf)-1]=0;

    if (logHook)
        (*logHook)(buf, va);
    else if (interp->subOpts.logAllowDups)
        vfprintf(stderr, buf, va);
    else {
        char buf1[JSI_BUFSIZ/2];
        vsnprintf(buf1, sizeof(buf1), buf, va);
        if (!isHelp && !noDups) {
            if (buf1[0] && lastCnt && Jsi_Strcmp(buf1, lastMsg)==0) {
                lastCnt++;
                goto done;
            } else if (lastMsg[0] && lastCnt>1 ) {
                fprintf(stderr, "REPEAT: Last msg repeated %d times...\"\n" ,lastCnt);
            }
            if (buf1[0] == 0 || (buf1[0] == '.' && buf1[1] == 0))
                goto done;
        }
        lastCnt = 1;
        Jsi_Strcpy(lastMsg, buf1);
        if (!islog)
            Jsi_Puts(interp, jsi_Stderr, buf1, -1);
            //fputs(buf1, stderr);
        else {
            Jsi_DString jStr={}, kStr={};
            Jsi_DSPrintf(&kStr, "[%s, \"%s\", \"%s\", %d, %d ]",
                Jsi_JSONQuote(interp, buf1, -1, &jStr), mt, curFile, line, lofs);
            if (Jsi_FunctionInvokeJSON(interp->parent, interp->debugOpts.msgCallback, Jsi_DSValue(&kStr), NULL) != JSI_OK)
                code = 1;
            Jsi_DSFree(&jStr);
            Jsi_DSFree(&kStr);
        }
    }
done:
    va_end(va);
    if (interp->debugOpts.hook) {
        static int inhook = 0;
        if (!inhook) {
            inhook = 1;
            (*interp->debugOpts.hook)(interp, curFile, interp->curIp?interp->curIp->Line:0, interp->level, interp->curFunction, "DEBUG", NULL, emsg);
        }
        inhook = 0;
    }
    if ((code & jsi_fatalexit) && !interp->opts.no_exit)
        jsi_DoExit(interp, 1);
    return (code==JSI_LOG_ERROR?JSI_ERROR:JSI_OK);
bail:
    va_end(va);
    return JSI_OK;
}