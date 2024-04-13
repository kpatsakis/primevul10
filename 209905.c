Jsi_RC jsi_RunFuncCallCheck(Jsi_Interp *interp, Jsi_Func *func, int argc, const char *name, jsi_Pline *line, Jsi_OpCodes *argCodes, bool isParse)
{
    Jsi_RC rc = JSI_OK;
    if (interp->typeCheck.all==0) {
        if (!argCodes ? (interp->typeCheck.run==0) : (interp->typeCheck.parse==0))
            return JSI_OK;
    }

    Jsi_CmdSpec *spec = func->cmdSpec;
    Jsi_ScopeStrs *ss = func->argnames;
    if (ss==NULL && spec == NULL)
        return JSI_OK;
    int i, minArgs, maxArgs, mis = 0, varargs = 0;
    char nbuf[JSI_MAX_NUMBER_STRING];
    if (func->type == FC_BUILDIN) {
        varargs =  (spec->maxArgs<0);
        maxArgs = spec->maxArgs + func->callflags.bits.addargs;
        minArgs = spec->minArgs + func->callflags.bits.addargs;
    } else {
        varargs = ss->varargs;
        minArgs = (ss->firstDef>0 ? ss->firstDef-1 : ss->count);
        maxArgs = ss->count;
        mis = (argc != ss->count);
        if (func->retType == 0 && ss && ss->typeCnt == 0 && interp->typeCheck.all==0)
            return JSI_OK;
    }
    if (varargs) {
        if (argc >= minArgs)
            return JSI_OK;
        mis = (argc<minArgs);
    } else 
        mis = (argc<minArgs || argc>maxArgs);
    if (mis) {
        if (varargs)
            snprintf(nbuf, sizeof(nbuf), "%d or more", minArgs);
        else if (maxArgs > minArgs)
            snprintf(nbuf, sizeof(nbuf), "%d-%d", minArgs, maxArgs);
        else
            snprintf(nbuf, sizeof(nbuf), "%d", maxArgs);
        if (line)
            interp->parseLine = line;
        if (interp->typeCheck.error)
            rc = JSI_ERROR;
        Jsi_DString dStr = {};
        Jsi_FuncObjToString(interp, func, &dStr, 2);
        if (isParse)
            Jsi_LogWarn("got %d args, expected %s, calling %s", argc, nbuf, Jsi_DSValue(&dStr));
        else
            rc = Jsi_LogType("got %d args, expected %s, calling %s", argc, nbuf, Jsi_DSValue(&dStr));
        jsi_TypeMismatch(interp);
        Jsi_DSFree(&dStr);
        if (line)
            interp->parseLine = NULL;
        return rc;
    }
    if (argCodes && argCodes->code_len>=argc) {
        int cl = argCodes->code_len;
        int aind=argc-1;
        for (i=cl-1; rc == JSI_OK && i>=0 && aind>=0; i--,aind--) {
            Jsi_ttype atyp = JSI_TT_ANY;
            switch (argCodes->codes[i].op) {
                case OP_PUSHSTR: atyp=JSI_TT_STRING; break;
                case OP_PUSHNUM: atyp=JSI_TT_NUMBER; break;
                case OP_PUSHBOO: atyp=JSI_TT_BOOLEAN; break;
                case OP_PUSHFUN: atyp=JSI_TT_FUNCTION; break;
                case OP_PUSHTHS: atyp=JSI_TT_OBJECT; break;
                case OP_PUSHREG: atyp=JSI_TT_REGEXP; break;
                case OP_PUSHUND: atyp=JSI_TT_VOID; break;
                case OP_PUSHNULL: atyp=JSI_TT_NULL; break;
                case OP_PUSHARG: atyp=JSI_TT_ARRAY; break;
                case OP_SUBSCRIPT: i++; break;
                case OP_ARRAY: atyp=JSI_TT_ARRAY; i=jsiPopArgs(argCodes, i); break;
                case OP_OBJECT: atyp=JSI_TT_OBJECT; i=jsiPopArgs(argCodes, i); break;
                default: break;
            }
            if (atyp == JSI_TT_ANY) continue;
            rc = jsi_StaticArgTypeCheck(interp, atyp, "for argument", name, aind+1, func, line);  
        }
    }
    return rc;
}