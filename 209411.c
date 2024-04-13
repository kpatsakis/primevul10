Jsi_RC Jsi_SqlObjBinds(Jsi_Interp* interp, Jsi_DString* zStr, const char *varName, bool addTypes, bool addDefaults, bool nullDefaults) {
    Jsi_Value *v, *vnPtr = Jsi_VarLookup(interp, varName);
    if (!vnPtr || !Jsi_ValueIsObjType(interp, vnPtr, JSI_OT_OBJECT))
        return Jsi_LogError("varName must be an Object: %s", varName);
    char *cp, *zSql = Jsi_DSValue(zStr);
    int create = !Jsi_Strncasecmp(zSql,"create",6);
    int insert = !Jsi_Strncasecmp(zSql,"insert",6);
    if (!create && !insert) return JSI_OK;
    const char *cPtr = Jsi_Strstr(zSql, " %s");
    if (!cPtr) cPtr = Jsi_Strstr(zSql, "\t%s");
    if (!cPtr)
        return Jsi_LogError("Object varName must contain a ' %%s': %s", varName);
    Jsi_DString sStr = {}, vStr = {}, jStr = {};
    Jsi_DSAppendLen(&sStr, zSql, cPtr?(cPtr-zSql):-1);
    Jsi_DSAppend(&sStr, " (", NULL);

    Jsi_IterObj *io = Jsi_IterObjNew(interp, NULL);
    Jsi_IterGetKeys(interp, vnPtr, io, 0);
    uint i;
    const char *pre = "", *kstr;
    if (!create)
        Jsi_DSAppend(&vStr, " VALUES(", NULL);
    for (i=0; i<io->count; i++) {
        kstr = io->keys[i];
        const char *qs = "", *qe = "";
        if (!Jsi_StrIsAlnum(kstr) || Jsi_IsReserved(interp, kstr, 1)) {
            qe = qs = "'";
        }
        Jsi_DSAppend(&sStr, pre, qs, kstr, qe, NULL);
        if (create) {
            const char *typ = NULL, *dflt=(nullDefaults?"NULL":NULL);
            if (addTypes && ((v = Jsi_ValueObjLookup(interp, vnPtr, kstr, 1)))) {
                if (Jsi_ValueIsBoolean(interp, v)) {
                    typ = "BOOLEAN";
                    if (!nullDefaults && addDefaults) {
                        bool bv = 0;
                        Jsi_ValueGetBoolean(interp, v, &bv);
                        dflt = (bv?"1":"0");
                    }
                } else if (Jsi_ValueIsNumber(interp, v)) {
                    typ = "NUMERIC";
                    if (!Jsi_Strcmp(kstr,"rowid"))
                        typ = "INTEGER PRIMARY KEY";
                    else if (!nullDefaults && addDefaults) {
                        Jsi_Number nv = 0;
                        Jsi_DSFree(&jStr);
                        Jsi_ValueGetNumber(interp, v, &nv);
                        dflt = Jsi_DSPrintf(&jStr, "%" JSI_NUMGFMT, nv);
                    }
                } else if (Jsi_ValueIsArray(interp, v) || Jsi_ValueIsObjType(interp, v, JSI_OT_OBJECT)) {
                    typ = "CHARJSON";
                    if (!nullDefaults && addDefaults) {
                        Jsi_DSFree(&jStr);
                        Jsi_DSAppend(&jStr, "'", NULL);
                        Jsi_ValueGetDString(interp, v, &jStr, JSI_OUTPUT_JSON|JSI_JSON_STRICT);
                        Jsi_DSAppend(&jStr, "'", NULL);
                        dflt = Jsi_DSValue(&jStr);
                    }
                } else {
                    typ = "TEXT";
                    if (!nullDefaults && addDefaults) {
                        if ((cp=Jsi_ValueString(interp, v, NULL))) {
                            Jsi_DSFree(&jStr);
                            dflt = Jsi_DSAppend(&jStr, "'", cp, "'", NULL);
                        } else
                        dflt = "NULL";
                    }
                }
            }
            if (typ)
                Jsi_DSAppend(&sStr, " ", typ, (dflt?" DEFAULT ":""), dflt, NULL);
        } else {
            Jsi_DSAppend(&vStr, pre, "$", varName, "(", kstr, ")", NULL);
        }
        pre = ",";
    }
    if (!create)
        Jsi_DSAppend(&vStr, ")", NULL);
    Jsi_IterObjFree(io);
    Jsi_DSAppend(&sStr, ") ", Jsi_DSValue(&vStr), cPtr+3, NULL);
    Jsi_DSFree(zStr);
    Jsi_DSAppend(zStr, Jsi_DSValue(&sStr), NULL);
    Jsi_DSFree(&sStr); Jsi_DSFree(&vStr); Jsi_DSFree(&jStr);
    return JSI_OK;
}