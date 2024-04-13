Jsi_RC Jsi_Interactive(Jsi_Interp* interp, int flags) 
{
    Jsi_RC rc = JSI_OK;
    int done = 0, len, quote = (flags & 0xff), istty = 1, chkHelp=0, hasHelp=0;
    const char *prompt = interp->subOpts.prompt;
    char *buf;
    if (jsi_interactiveInterp) 
        return Jsi_LogError("multiple interactive not supported");
#if JSI__SIGNAL
  signal(SIGINT, jsi_InteractiveSignal); 
#endif
    interp->typeCheck.parse = interp->typeCheck.run = interp->typeCheck.all = 1;
    interp->strict = 1;
    interp->isInteractive = 1;
    jsi_interactiveInterp = interp;
    interp->subOpts.istty = 1;
    interp->subOpts.logAllowDups = 1;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
#ifndef __WIN32
    istty = isatty(fileno(stdin));
#else
    istty = _isatty(_fileno(stdin));
#endif
#ifdef JSI_HAS_READLINE
    Jsi_DString dHist = {}, sHist = {};
    char *hist = NULL;
#ifdef USE_GNU_READLINE
    rl_attempted_completion_function = jsiRlGetMatches;
#else
    linenoiseSetCompletionCallback(jsiLNGetMatches);
    linenoiseSetHintsCallback(jsiLNhints);
#endif
    if(interp->subOpts.noReadline == 0 && !interp->parent && !(interp->isSafe && interp->safeMode==jsi_safe_Lockdown))
    {
        const char *hfile = (interp->historyFile ? interp->historyFile : "~/.jsish_history");
        hist = Jsi_NormalPath(interp, hfile, &dHist);
        if (hist)
            jsi_sh_read_history(hist);
    }
#endif
    interp->level++;
    if (!interp->iskips)
        puts("Jsish interactive: see 'help [cmd]' or 'history'.  \\ cancels > input."
#if JSI__SIGNAL
        "  ctrl-c aborts running script."
#endif
        );
    while (done==0 && interp->exited==0) {
        buf = get_inputline(interp, istty, (prompt?prompt:"$ "));
        if (buf) {
            if (buf[0] == '\\' && !buf[1]) {
                 Jsi_DSSetLength(&dStr, 0);
                 prompt = interp->subOpts.prompt;
                 fprintf(stderr, "abandoned input");
            } else
                Jsi_DSAppend(&dStr, buf, NULL);
            free(buf);
        } else {
            done = 1;
        }
        len = Jsi_DSLength(&dStr);
        if (done && len == 0)
            break;
        if (!len) continue;
        Jsi_DSAppendLen(&dStr, " ", 1); // Allow for added space.
        buf = Jsi_DSValue(&dStr);
        if (done == 0 && (!jsi_StrIsBalanced(buf))) {
            prompt = interp->subOpts.prompt2;
            continue;
        }
        prompt = interp->subOpts.prompt;
        while ((len = Jsi_Strlen(buf))>0 && (isspace(buf[len-1])))
            buf[len-1] = 0;
        if (buf[0] == 0) {
            Jsi_DSSetLength(&dStr, 0);
            continue;
        }
        bool wantHelp = 0;
        if (interp->onEval == NULL) {
            /* Convenience: add semicolon to "var" statements (required by parser). */
#ifdef JSI_HAS_READLINE
            if (!Jsi_Strncmp(buf, "history", 7) && buf[7] == 0) {
                fputs(Jsi_DSValue(&sHist), stdout);
                Jsi_DSSetLength(&dStr, 0);
                continue;
            }
#endif
            if (!Jsi_Strncmp(buf, "help", 4) && (buf[4] == 0 || isspace(buf[4]))) {
                if (!chkHelp++)
                    hasHelp = (Jsi_PkgRequire(interp, "Help", 0)>=0);
                if (hasHelp) {
                    wantHelp = 1;
                    char tbuf[BUFSIZ];
                    snprintf(tbuf, sizeof(tbuf), "return runModule('Help', '%s'.trim().split(null));", buf+4);
                    rc = Jsi_EvalString(interp, tbuf, JSI_RETURN);
                }
            }
            if (!wantHelp) {
                if (!Jsi_Strncmp(buf,"var ", 4) && Jsi_Strchr(buf, '\n')==NULL && Jsi_Strchr(buf, ';')==NULL)
                    Jsi_Strcpy(buf+Jsi_Strlen(buf), ";"); // Added space above so strcat ok.
                rc = Jsi_EvalString(interp, buf, JSI_EVAL_RETURN);
                prompt = interp->subOpts.prompt;
#ifdef JSI_HAS_READLINE
                if (rc == JSI_OK)
                    Jsi_DSAppend(&sHist, buf, "\n", NULL);
#endif
            }
        }
        else
        {
            Jsi_Value *func = interp->onEval;
            if (func && Jsi_ValueIsFunction(interp, func)) {
                Jsi_Value *items[1] = {};
                items[0] = Jsi_ValueNewStringDup(interp, buf);
                Jsi_Value *args = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 1, 0));
                Jsi_IncrRefCount(interp, args);
                rc = Jsi_FunctionInvoke(interp, func, args, &interp->retValue, interp->csc);
                Jsi_DecrRefCount(interp, args);
                if (rc != JSI_OK)
                    fprintf(stderr, "bad eval");
            }
        }
        if (interp->exited)
            break;
        jsi_interactiveInterp->interrupted = 0;
        if (rc == JSI_OK) {
             if (interp->retValue->vt != JSI_VT_UNDEF || interp->subOpts.outUndef) {
                Jsi_DString eStr = {};
                fputs(Jsi_ValueGetDString(interp, interp->retValue, &eStr, hasHelp?0:quote), stdout);
                Jsi_DSFree(&eStr);
                fputs("\n", stdout);
             }
        } else if (!interp->exited && !wantHelp) {
            fputs("ERROR\n", stderr);
        }
        Jsi_DSSetLength(&dStr, 0);
        len = 0;
    }
    interp->level--;
#ifdef JSI_HAS_READLINE
    if (hist && !interp->isSafe) {
        jsi_sh_stifle_history(100);
        jsi_sh_write_history(hist);
    }
    Jsi_DSFree(&dHist);
    Jsi_DSFree(&sHist);
#endif
    Jsi_DSFree(&dStr);
    if (interp->retValue) {
        Jsi_DecrRefCount(interp, interp->retValue);
        interp->retValue = NULL;
    }
    if (interp->exited && interp->level <= 0)
    {
        rc = JSI_EXIT;
        Jsi_InterpDelete(interp);
    }
    jsi_interactiveInterp = NULL;
    return rc;
}