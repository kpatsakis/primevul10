int jsi_typeGet(Jsi_Interp *interp, const char *tname) {
    if (!tname)
        return 0;
    if (Jsi_Strchr(tname, '|')) {
        int argc, i, rc, val = 0;
        char **argv;
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        Jsi_SplitStr(tname, &argc, &argv, "|", &dStr);
        for (i=0; i<argc; i++) {
            rc = jsi_typeGet(interp, argv[i]);
            if (rc < 0)
                break;
            val |= rc;
        }
        Jsi_DSFree(&dStr);
        if (i<argc)
            return -1;
        return val;
    }
    switch (tname[0]) {
        case 'b': if (Jsi_Strcmp(tname, "boolean")==0) return JSI_TT_BOOLEAN; break;
        case 's': if (Jsi_Strcmp(tname, "string")==0) return JSI_TT_STRING; break;
        case 'n': if (Jsi_Strcmp(tname, "null")==0) return JSI_TT_NULL;
                  if (Jsi_Strcmp(tname, "number")==0) return JSI_TT_NUMBER; break;
        case 'o': if (Jsi_Strcmp(tname, "object")==0) return JSI_TT_OBJECT; break;
        case 'r': if (Jsi_Strcmp(tname, "regexp")==0) return JSI_TT_REGEXP; break;
        case 'f': if (Jsi_Strcmp(tname, "function")==0) return JSI_TT_FUNCTION; break;
        case 'i': if (Jsi_Strcmp(tname, "iterobj")==0) return JSI_TT_ITEROBJ;
        case 'u': if (Jsi_Strcmp(tname, "userobj")==0) return JSI_TT_USEROBJ;
                  if (Jsi_Strcmp(tname, "undefined")==0) return JSI_TT_UNDEFINED; break;
        case 'a': if (Jsi_Strcmp(tname, "array")==0) return JSI_TT_ARRAY;
                  if (Jsi_Strcmp(tname, "any")==0) return JSI_TT_ANY; break;
        case 'v': if (Jsi_Strcmp(tname, "void")==0) return JSI_TT_VOID; break;
    }
    Jsi_LogWarn("Type \"%s\" is not one of boolean, string, number, function, array, object, regexp, userobj, null, undefined, void or any", tname);
    return 0;
}