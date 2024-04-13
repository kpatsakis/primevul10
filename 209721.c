static Jsi_RC mdbPrepareAndBind(MyDbEvalContext *p)
{
    if (mdbPrepareStmt(p) != JSI_OK)
        return JSI_ERROR;
    MysqlPrep *prep = p->prep;
    MySqlObj *jdb = p->jdb;
    Jsi_Interp *interp = jdb->interp;
    Jsi_Value *pv = NULL, *apv = NULL;
    char tname[50];

    Jsi_RC rc = JSI_OK;
    int i, n, nVar = prep->numParam;

    if (nVar<=0)
        return rc;
    if (!prep->bindParam)
        prep->bindParam = (MYSQL_BIND*)Jsi_Calloc(nVar, sizeof(MYSQL_BIND));
    else
        memset(prep->bindParam, 0, (nVar * sizeof(MYSQL_BIND)));
    for(i=1; i<=nVar; i++) {
        int btype = 0;
        int isInt = 0;
        int isBlob = 0;
        const char *zVar = mysqlGetbindParamName(prep, i);
        tname[0] = 0;
        if (zVar == NULL) {
            if (!jdb->optPtr || !(apv=jdb->optPtr->values))
                return Jsi_LogError("? bind without values for param %d", i);
            if (!(pv =Jsi_ValueArrayIndex(interp, apv, i-1))) 
                return Jsi_LogError("array element %d missing", nVar);
        }
        else if ((zVar[0]!='$' && zVar[0]!=':' && zVar[0]!='@') ) 
            return Jsi_LogError("can not find bind var %s", zVar); else {
           
            int zvLen = Jsi_Strlen(zVar);
            char *zcp;
            if (zVar[0] =='$' && ((zcp = Jsi_Strchr(zVar,'('))) && zVar[zvLen-1] == ')')
            {
                Jsi_DString vStr;
                Jsi_DSInit(&vStr);
                Jsi_DSAppendLen(&vStr, zVar+1, (zcp-zVar-1));
                int slen = Jsi_Strlen(zcp);
                const char *ttp;
                if (jdb->optPtr->typeCheck!=mdbTypeCheck_None && (ttp = Jsi_Strchr(zVar,':'))) {
                    // Extract bind-type.
                    Jsi_DString tStr = {};
                    int tlen = Jsi_Strlen(ttp+1);
                    Jsi_DSAppendLen(&tStr, ttp+1, tlen-1);
                    snprintf(tname, sizeof(tname), "%s", Jsi_DSValue(&tStr));
                    Jsi_HashEntry *hPtr = Jsi_HashEntryFind(jdb->typeNameHash, tname);
                    assert(hPtr);
                    btype = (long)Jsi_HashValueGet(hPtr);
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
        }
        Jsi_Number r;
        SqlFieldResults *fres = prep->fieldParam+i-1;
        MYSQL_BIND *bind = prep->bindParam+i-1;
        memset(bind, 0, sizeof(*bind));
        // Now create binding.
        if(!pv ) {
            if (!jdb->bindWarn) {
                rc = Jsi_LogError("unknown bind param: %s", zVar);
                break;
            } else
                Jsi_LogWarn("unknown bind param: %s", zVar);
        } else {
            if (btype && !Jsi_ValueIsUndef(interp, pv)) {
                int done = 0, match = 1, cast = (jdb->optPtr->typeCheck==mdbTypeCheck_Cast);
                switch (btype) {
                    case MYSQL_TYPE_BLOB:
                        isBlob = 1;
                    case MYSQL_TYPE_STRING:
                        if (cast)
                            Jsi_ValueToString(interp, pv, &n);
                        else
                            match = Jsi_ValueIsString(interp, pv); 
                        break;
                    case MYSQL_TYPE_DOUBLE:
                        if (cast)
                            Jsi_ValueToNumber(interp, pv);
                        else
                            match = Jsi_ValueIsNumber(interp, pv); 
                        break;
                    case MYSQL_TYPE_LONGLONG:
                        isInt = 1;
                        if (cast)
                            Jsi_ValueToNumber(interp, pv);
                        else
                            match = Jsi_ValueIsNumber(interp, pv); 
                        break;
                    case MYSQL_TYPE_TINY:
                        if (cast)
                            Jsi_ValueToBool(interp, pv);
                        else
                            match = Jsi_ValueIsBoolean(interp, pv); 
                        break;
                    case MYSQL_TYPE_TIME:
                    case MYSQL_TYPE_DATE:
                    case MYSQL_TYPE_DATETIME:
                    case MYSQL_TYPE_TIMESTAMP:
                        if (cast)
                            Jsi_ValueToNumber(interp, pv);
                        else if (!Jsi_ValueIsNumber(interp, pv))
                            goto errout;
                        Jsi_GetNumberFromValue(interp, pv, &r);
                        bind->buffer_type = (enum enum_field_types)btype;
                        bind->buffer=&fres->buffer.timestamp;
                        bind->buffer_length = sizeof(fres->buffer.timestamp);
                        bind->length = NULL;
                        mdbJsToMyTime(r, &fres->buffer.timestamp, 1);
                        done = 1;
                        break;
                    default:
                        Jsi_LogBug("Unhandled bind type: %s = %d", tname, btype);
                }
                if (done)
                    continue;
                 if (cast == 0 && match == 0) 
errout:
                 {
                    int ltyp = (jdb->optPtr->typeCheck==mdbTypeCheck_Error?JSI_LOG_ERROR:JSI_LOG_WARN);
                    Jsi_LogMsg(interp, ltyp, "bind param \"%s\" type is not \"%s\"", zVar, tname);
                    if (ltyp == JSI_LOG_ERROR)
                        return JSI_ERROR;
                }
            }
            
            if (Jsi_ValueIsBoolean(interp, pv)) {
                bool nb;
                Jsi_GetBoolFromValue(interp, pv, &nb);
                n = nb;
                bind->buffer_type = MYSQL_TYPE_TINY;
                bind->buffer = &fres->buffer.vchar;
                bind->buffer_length = sizeof(fres->buffer.vchar);
                bind->length = &fres->len;
                //bind->is_null = &fres->isnull;
                fres->buffer.vchar = n;
                
            } else if (Jsi_ValueIsNumber(interp, pv)) {
                Jsi_Number r;
                Jsi_Wide wv;
                Jsi_GetNumberFromValue(interp, pv, &r);
                wv = (Jsi_Wide)r;
                bind->buffer_type = MYSQL_TYPE_DOUBLE;
                bind->buffer = &fres->buffer.vdouble;
                bind->buffer_length = sizeof(fres->buffer.vdouble);
                bind->length = &fres->len;
                fres->buffer.vdouble = (double)r;
                if (isInt || (jdb->forceInt && (((Jsi_Number)wv)-r)==0)) {
                    bind->buffer = &fres->buffer.vlonglong;
                    bind->buffer_type = MYSQL_TYPE_LONGLONG;
                    bind->buffer_length = sizeof(fres->buffer.vlonglong);
                    fres->buffer.vlonglong = wv;
                }
            } else if (Jsi_ValueIsNull(interp, pv) || (Jsi_ValueIsUndef(interp, pv) && jdb->optPtr->mapundef)) {
bindnull:
                bind->buffer_type = MYSQL_TYPE_NULL;
                bind->buffer = &fres->buffer.vchar;
                bind->buffer_length = sizeof(fres->buffer.vchar);
                bind->length = &fres->len;
                fres->buffer.vchar = 0;

            } else if (Jsi_ValueIsString(interp, pv)) {
                char *sstr = Jsi_ValueGetStringLen(interp, pv, &n);
                bind->buffer_type = MYSQL_TYPE_STRING;
                bind->buffer=sstr;
                bind->buffer_length = n;
                bind->length = NULL;
                if (isBlob)
                    bind->buffer_type = MYSQL_TYPE_BLOB;
            } else {
                if (!jdb->bindWarn) {
                    rc = Jsi_LogError("bind param must be string/number/bool/null: %s", zVar);
                    break;
                } else {
                    Jsi_LogWarn("bind param must be string/number/bool/null: %s", zVar);
                    goto bindnull;
                }
            }
        }
    }
    if (mysql_stmt_bind_param(prep->myStmt, prep->bindParam))
        rc = Jsi_LogError("bind failed: %s", mysql_error(jdb->db));
    return rc;
}