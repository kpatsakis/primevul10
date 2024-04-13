Jsi_RC jsi_JsPreprocessLine(Jsi_Interp* interp, char *buf, size_t bsiz, uint ilen, int jOpts[4], int lineNo) {
    if (buf[0]==';' && buf[1] && buf[2]) {
        // Wrap ";XXX;" in a puts("XXX ==> ", XXX)
        if (!jOpts[0]) {
            if (!Jsi_Strcmp(buf, "=!EXPECTSTART!=\n") || !Jsi_Strcmp(buf, "=!INPUTSTART!=\n") ) {
                return JSI_OK;
            }
        } else {
            if (!Jsi_Strcmp(buf, "=!EXPECTEND!=\n") || !Jsi_Strcmp(buf, "=!INPUTEND!=\n")) {
                jOpts[0] = 0;
                return JSI_OK;
            }
        }
        if (buf[ilen-1]=='\n' && buf[ilen-2]==';' && (2*ilen+12)<bsiz) {
            if (Jsi_Strchr(buf, '`')) {
                return Jsi_LogError("back-tick is illegal in unitTest on line %d: %s", lineNo, buf);
            }
            char ubuf[bsiz], *ucp = ubuf;
            buf[ilen-=2] = 0;
            Jsi_Strcpy(ubuf, buf+1);
            while (*ucp && isspace(*ucp)) ucp++;
            if (ilen>2 && ucp[0]=='\'' && ubuf[ilen-2]=='\'')
                snprintf(buf, bsiz, "puts(`%s`);\n", ucp); //output a 'Comment'
                
            else if (interp->debugOpts.testFmtCallback) {
                Jsi_DString kStr={};
                Jsi_Value *vrc = Jsi_ValueNew1(interp);
                Jsi_DSPrintf(&kStr, "[\"%s\", %d ]", ucp, lineNo);
                Jsi_RC rcs = Jsi_FunctionInvokeJSON(interp->parent, interp->debugOpts.testFmtCallback, Jsi_DSValue(&kStr), &vrc);
                if (rcs == JSI_OK) {
                    const char *cps = Jsi_ValueString(interp, vrc, NULL);
                    if (!cps)
                        rcs = JSI_ERROR;
                    else
                        snprintf(buf, bsiz, "%s", cps);
                }
                Jsi_DecrRefCount(interp, vrc);
                Jsi_DSFree(&kStr);
                if (rcs != JSI_OK)
                    return Jsi_LogError("failure in debugOpts.testFmtCallback");
            } else if (ilen>3 && ubuf[0]=='/' && ubuf[0]=='/') {
                char *ecp = ubuf+2;
                while (*ecp && isspace(*ecp)) ecp++;
                snprintf(buf, bsiz, "printf(`%%s ==>`, \"%s\"); try { %s; puts('\\nFAIL!\\n'); } "
                    "catch(err) { puts('\\nPASS!: err =',err); }\n", ecp, ecp);
            } else {
                snprintf(buf, bsiz, "printf(`%%s ==> `,`%s`),puts(%s);\n", ucp, ucp);
            }
        }
    }
    return JSI_OK;
}