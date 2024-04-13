static Jsi_RC dbBindStmt(Jsi_Db *jdb, SqlPreparedStmt *prep)
{
    sqlite3_stmt *pStmt = prep->pStmt;    /* Object used to cache statement */
    Jsi_Interp *interp = jdb->interp;
    int i, btype = 0, bindArr=0, n;
    Jsi_RC rc = JSI_OK;
    Jsi_Number r;
    Jsi_Wide wv;
 
    Jsi_Value *pv = NULL, *apv = NULL;
    int nVar = sqlite3_bind_parameter_count(pStmt);
    char tname[50];
    
   /* Bind values to parameters that begin with @, $, :, or ? */
    for(i=1; i<=nVar; i++) {
        tname[0] = 0;
        int isInt = 0, isBlob = 0;
        const char *zVar = sqlite3_bind_parameter_name(pStmt, i);
        if (zVar == NULL) {
            if (!jdb->optPtr || !(apv=jdb->optPtr->values)) 
                return Jsi_LogError("? bind without values for param %d", i);
            if (!(pv =Jsi_ValueArrayIndex(interp, apv, i-1))) 
                return Jsi_LogError("array element %d missing", nVar);
        }
        else if((zVar[0]=='$' || zVar[0]==':' || zVar[0]=='@') ) {
            int zvLen = Jsi_Strlen(zVar);
            char *zcp;
            if (zVar[0] =='$' && ((zcp = (char*)Jsi_Strchr(zVar,'('))) && zVar[zvLen-1] == ')')
            {
                bindArr = 1;
                Jsi_DString vStr;
                Jsi_DSInit(&vStr);
                Jsi_DSAppendLen(&vStr, zVar+1, (zcp-zVar-1));
                int slen = Jsi_Strlen(zcp);
                const char *ttp;
                if ((ttp = Jsi_Strchr(zVar,':'))) { // Extract bind-type.
                    Jsi_DString tStr = {};
                    int tlen = Jsi_Strlen(ttp+1);
                    Jsi_DSAppendLen(&tStr, ttp+1, tlen-1);
                    if (!jdb->typeNameHash)
                        dbTypeNameHashInit(jdb);
                    Jsi_HashEntry *htPtr = Jsi_HashEntryFind(jdb->typeNameHash, Jsi_DSValue(&tStr));
                    int rc = ( htPtr != NULL);
                    if (!htPtr) {
                        Jsi_DString eStr = {};
                        Jsi_HashSearch search;
                        Jsi_Interp *interp = jdb->interp;
                        int n = 0;
                        Jsi_HashEntry *hPtr;
                        for (hPtr = Jsi_HashSearchFirst(jdb->typeNameHash, &search);
                            hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
                            const char *key = (char*)Jsi_HashKeyGet(hPtr);
                            Jsi_DSAppend(&eStr, (n++?", ":""), key, NULL);
                        }
                        Jsi_LogWarn("bind type \"%s\" is not one of: %s", Jsi_DSValue(&tStr), Jsi_DSValue(&eStr));
                        Jsi_DSFree(&eStr);
                    }
                    Jsi_Strcpy(tname, Jsi_DSValue(&tStr));
                    Jsi_DSFree(&tStr);
                    if (!rc)
                        return JSI_ERROR;

                    btype = (uintptr_t)Jsi_HashValueGet(htPtr);
                    Jsi_DSFree(&tStr);
                    slen -= tlen;
                }

                if (isdigit(zcp[1])) {
                    Jsi_DSAppendLen(&vStr, "[", 1);
                    Jsi_DSAppendLen(&vStr, zcp+1, slen-2);
                    Jsi_DSAppendLen(&vStr, "]", 1);
                } else {
                    if (zcp[1] != '[')
                        Jsi_DSAppendLen(&vStr, ".", 1);
                    Jsi_DSAppendLen(&vStr, zcp+1, slen-2);
                }
                pv = Jsi_NameLookup(interp, Jsi_DSValue(&vStr));
                Jsi_DSFree(&vStr);
            } else
                pv = Jsi_VarLookup(interp, &zVar[1]);
        } else 
            return Jsi_LogError("can not find bind var %s", zVar);
            
        if(!pv ) {
            if (!jdb->bindWarn) {
                Jsi_LogError("unknown bind param: %s", zVar);
                rc = JSI_ERROR;
                break;
            } else
                Jsi_LogWarn("unknown bind param: %s", zVar);
        } else {
            int match = 1, cast = (jdb->optPtr->typeCheck==dbTypeCheck_Cast);
            if (btype && !Jsi_ValueIsUndef(interp, pv)) {
                switch (btype) {
                    case JSI_OPTION_STRBUF:
                        isBlob = 1;
                    case JSI_OPTION_STRING:
                        if (cast)
                            Jsi_ValueToString(interp, pv, &n);
                        else
                            match = Jsi_ValueIsString(interp, pv);
                        break;
                    case JSI_OPTION_NUMBER:
                    case JSI_OPTION_DOUBLE:
                        if (cast)
                            Jsi_ValueToNumber(interp, pv);
                        else
                            match = Jsi_ValueIsNumber(interp, pv);
                        break;
                    case JSI_OPTION_TIME_W:
                    case JSI_OPTION_TIME_T:
                    case JSI_OPTION_INT64:
                        isInt = 1;
                        if (cast)
                            Jsi_ValueToNumber(interp, pv);
                        else
                            match = Jsi_ValueIsNumber(interp, pv);
                        break;
                    case JSI_OPTION_BOOL:
                        if (cast)
                            Jsi_ValueToBool(interp, pv);
                        else
                            match = Jsi_ValueIsNumber(interp, pv);
                        break;
                    case JSI_OPTION_TIME_D:
                        if (cast)
                            Jsi_ValueToNumber(interp, pv); //TODO: do something more for dates?
                        else
                            match = Jsi_ValueIsNumber(interp, pv);
                        break;
                    default:
                        Jsi_LogBug("Unhandled bind type: %s = %d", tname, btype);
                }
                if (cast == 0 && match == 0) {
                    int ltyp = (jdb->optPtr->typeCheck==dbTypeCheck_Error?JSI_LOG_ERROR:JSI_LOG_WARN);
                    Jsi_LogMsg(interp, ltyp, "bind param \"%s\" type is not \"%s\"", zVar, tname);
                    if (ltyp == JSI_LOG_ERROR)
                        return JSI_ERROR;
                }
            }
            bool bn, isArr;
            const char *dectyp;
            if (Jsi_ValueIsBoolean(interp, pv)) {
                Jsi_GetBoolFromValue(interp, pv, &bn);
                sqlite3_bind_int(pStmt, i, bn);
            } else if (Jsi_ValueIsNumber(interp, pv)) {
                Jsi_GetNumberFromValue(interp, pv, &r);
                wv = (Jsi_Wide)r;
                if (isInt || (jdb->forceInt && (((Jsi_Number)wv)-r)==0))
                    sqlite3_bind_int64(pStmt, i,wv);
                else
                    sqlite3_bind_double(pStmt, i,(double)r);
            } else if (Jsi_ValueIsNull(interp, pv) || (Jsi_ValueIsUndef(interp, pv) && jdb->queryOpts.mapundef)) {
                sqlite3_bind_null(pStmt, i);
            } else if (Jsi_ValueIsString(interp, pv)) {
                const char *sstr = Jsi_ValueGetStringLen(interp, pv, &n);
                if (!sstr) sstr = "";
                if (isBlob)
                    sqlite3_bind_blob(pStmt, i, (char *)sstr, n, SQLITE_TRANSIENT );
                else
                    sqlite3_bind_text(pStmt, i, (char *)sstr, n, SQLITE_TRANSIENT );
            } else if (!jdb->noJsonConv && bindArr && ((isArr=Jsi_ValueIsArray(interp, pv))
                || Jsi_ValueIsObjType(interp, pv, JSI_OT_OBJECT))
                && (((dectyp = sqlite3_column_decltype(pStmt, i))==NULL) || 
                    !Jsi_Strncasecmp(dectyp,"charjson",8))) {
                    // Limitation: on INSERT can not access decltype.
                    Jsi_DString jStr = {};
                    Jsi_ValueGetDString(interp, pv, &jStr, JSI_OUTPUT_JSON|JSI_JSON_STRICT);
                    n = Jsi_DSLength(&jStr);
                    sqlite3_bind_text(pStmt, i, Jsi_DSValue(&jStr), n, SQLITE_TRANSIENT );
                    Jsi_DSFree(&jStr);
            } else {
                if (!jdb->bindWarn) {
                    Jsi_LogError("bind param must be string/number/bool/null: %s", zVar);
                    rc = JSI_ERROR;
                    break;
                } else
                    Jsi_LogWarn("bind param must be string/number/bool/null: %s", zVar);
                sqlite3_bind_null(pStmt, i);
            }

        }
    }
    return rc;
}