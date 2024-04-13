static Jsi_RC SqliteQueryCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                             Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_RC rc = JSI_OK;
    Jsi_Db *jdb;
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *vSql = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_DString eStr = {};
#ifdef JSI_DB_DSTRING_SIZE
    JSI_DSTRING_VAR(dStr, JSI_DB_DSTRING_SIZE);
#else
    Jsi_DString ddStr, *dStr = &ddStr;
    Jsi_DSInit(dStr);
#endif
    const char *zSql = Jsi_ValueGetDString(interp, vSql, &eStr, 0);
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 1);
    int cnt = 0;
    char **apColName = NULL;
    int *apColType = NULL, isopts = 0;
    DbEvalContext sEval = {};
    QueryOpts opts, *oEopt = jdb->optPtr;
    opts = jdb->queryOpts;
    opts.callback = NULL;
    opts.width = NULL;
    Jsi_Value *callback = NULL, *width = NULL;
            
    if (arg) {
        if (Jsi_ValueIsFunction(interp,arg))
            callback = opts.callback = arg;
        else if (Jsi_ValueIsString(interp, arg))
            opts.objName = Jsi_ValueString(interp, arg, NULL);
        else if (Jsi_ValueIsObjType(interp, arg, JSI_OT_ARRAY))
            opts.values = arg;
        else if (Jsi_ValueIsObjType(interp, arg, JSI_OT_OBJECT))
            isopts = 1;
        else {
            rc = Jsi_LogError("arg 2: expected function, string, array or options");
            goto bail;
        }
    }

    if (isopts) {
        if (Jsi_OptionsProcess(interp, ExecFmtOptions, &opts, arg, 0) < 0) {
            rc = JSI_ERROR;
            goto bail;
        }
        callback = (opts.callback ? opts.callback : jdb->queryOpts.callback);
        width = (opts.width ? opts.width : jdb->queryOpts.width);
    }
    if (opts.retChanged) {
        if (opts.callback) {
            rc = Jsi_LogError("can not use retChanged with callback");
            goto bail;
        }
        opts.mode = _JSI_EF_NONE;
        opts.headers = 0;
    }
    if (opts.cdata) {
        Jsi_CDataDb* copts = Jsi_CDataLookup(interp, opts.cdata);
        if (!copts)
            rc = Jsi_LogError("unknown cdata name: %s", opts.cdata);
        else {
            int n = Jsi_DbQuery(jdb, copts, zSql);
            Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)n);
        }
        goto bail;
    }
    if (opts.objName) {
        if (Jsi_SqlObjBinds(interp, &eStr, opts.objName, !(opts.objOpts&OBJMODE_NOTYPES), 
            !(opts.objOpts&OBJMODE_NODEFAULTS), (opts.objOpts&OBJMODE_NULLDEFAULTS)!=0) != JSI_OK)
            goto bail;
        zSql = Jsi_DSValue(&eStr);
    }
    if ((jdb->echo || opts.echo) && zSql)
        Jsi_LogInfo("SQL-ECHO: %s\n", zSql); 
    if ((opts.objOpts&OBJMODE_SQLONLY)) {
        if (opts.objName)
            Jsi_ValueMakeStringDup(interp, ret, zSql);
        else
            rc = Jsi_LogError("'objOpts.sqlOnly' can only be used with 'objName'");
        goto bail;
    }
    if (!opts.separator) {
        switch (opts.mode) {
            case _JSI_EF_LIST: opts.separator = "|"; break;
            case _JSI_EF_COLUMN: opts.separator = " "; break;
            case _JSI_EF_TABS: opts.separator = "\t"; break;
            default: opts.separator = ",";
        }
    }
    Jsi_DString sStr;
    Jsi_DSInit(&sStr);
    sEval.nocache = opts.nocache;
    if ((rc = dbEvalInit(interp, &sEval, jdb, zSql, &sStr, 0, 0)) != JSI_OK)
        goto bail;
    sEval.ret = *ret;
    jdb->optPtr = &opts;
    if (callback) {
        sEval.tocall = callback;
        if (opts.mode != _JSI_EF_ROWS) {
            Jsi_LogError("'mode' must be 'rows' with 'callback'");
            rc = JSI_ERROR;
            goto bail;
        }
        rc = dbEvalCallCmd(&sEval, interp, JSI_OK);
        goto bail;
    } else
    switch (opts.mode) {
    case _JSI_EF_NONE:
        while(JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        if (opts.retChanged)
            Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)sqlite3_changes(jdb->db));
        if (rc == JSI_BREAK)
            rc = JSI_OK;
        goto bail;
        break;
    case _JSI_EF_JSON:
        if (opts.headers) {
            Jsi_DSAppend(dStr, "[ ", NULL);
            while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
                int i;
                int nCol;
                dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
                if (cnt == 0) {
                    Jsi_DSAppend(dStr, "[", NULL);
                    for(i=0; i<nCol; i++) {
                        if (i)
                            Jsi_DSAppend(dStr, ", ", NULL);
                        Jsi_JSONQuote(interp, apColName[i], -1, dStr);
                    }
                    Jsi_DSAppend(dStr, "]", NULL);
                    cnt++;
                }
                if (cnt)
                    Jsi_DSAppend(dStr, ", ", NULL);
                Jsi_DSAppend(dStr, "[", NULL);
                for(i=0; i<nCol; i++) {
                    if (i)
                        Jsi_DSAppend(dStr, ", ", NULL);
                    dbEvalSetColumnJSON(&sEval, i, dStr);
                }
                Jsi_DSAppend(dStr, "]", NULL);
                cnt++;
                if (opts.limit && cnt>opts.limit) break;
            }
            Jsi_DSAppend(dStr, " ]", NULL);
            
        } else {
            Jsi_DSAppend(dStr, "[ ", NULL);
            while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
                int i;
                int nCol;
                dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
                if (cnt)
                    Jsi_DSAppend(dStr, ", ", NULL);
                Jsi_DSAppend(dStr, "{", NULL);
                for(i=0; i<nCol; i++) {
                    if (i)
                        Jsi_DSAppend(dStr, ", ", NULL);
                    Jsi_JSONQuote(interp, apColName[i], -1, dStr);
                    Jsi_DSAppend(dStr, ":", NULL);
                    dbEvalSetColumnJSON(&sEval, i, dStr);
                }
                Jsi_DSAppend(dStr, "}", NULL);
                cnt++;
                if (opts.limit && cnt>=opts.limit) break;
            }
            Jsi_DSAppend(dStr, " ]", NULL);
        }
        break;
        
    case _JSI_EF_JSON2: {
            while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
                int i;
                int nCol;
                dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
                if (cnt == 0 && 1) {
                    Jsi_DSAppend(dStr, "{ \"names\": [ ", NULL);
                    for(i=0; i<nCol; i++) {
                        if (i)
                            Jsi_DSAppend(dStr, ", ", NULL);
                        Jsi_JSONQuote(interp, apColName[i], -1, dStr);
                    }
                    Jsi_DSAppend(dStr, " ], \"values\": [ ", NULL);
                }
                if (cnt)
                    Jsi_DSAppend(dStr, ", ", NULL);
                Jsi_DSAppend(dStr, "[", NULL);
                for(i=0; i<nCol; i++) {
                    if (i)
                        Jsi_DSAppend(dStr, ", ", NULL);
                    dbEvalSetColumnJSON(&sEval, i, dStr);
                }
                Jsi_DSAppend(dStr, " ]", NULL);
                cnt++;
                if (opts.limit && cnt>=opts.limit) break;
            }
            if (cnt)
                Jsi_DSAppend(dStr, " ] } ", NULL);
        }
        break;
        
    case _JSI_EF_LIST:
        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0 && opts.headers) {
                for(i=0; i<nCol; i++) {
                    if (i)
                        Jsi_DSAppend(dStr, opts.separator, NULL);
                    Jsi_DSAppend(dStr, apColName[i], NULL);
                }
            }

            if (cnt || opts.headers)
                Jsi_DSAppend(dStr, "\n", NULL);
            for(i=0; i<nCol; i++) {
                if (i)
                    Jsi_DSAppend(dStr, opts.separator, NULL);
                dbEvalSetColumn(&sEval, i, dStr);
            }
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        break;
        
    case _JSI_EF_COLUMN: {
        int *wids = NULL;
        Jsi_DString vStr = {};
        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i, w;
            int nCol;
            
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0 && nCol>0) {
                Jsi_DString sStr;
                wids = (int*)Jsi_Calloc(nCol, sizeof(int));
                Jsi_DSInit(&sStr);
                for(i=0; i<nCol; i++) {
                    int j = Jsi_Strlen(apColName[i]);
                    wids[i] = (j<10?10:j);
                    if (width) {
                        Jsi_Value *wv = Jsi_ValueArrayIndex(interp, width, i);
                        if (wv) {
                            Jsi_Number dv;
                            Jsi_ValueGetNumber(interp, wv, &dv);
                            if (dv>0)
                                wids[i] = (int)dv;
                        }
                    }
                    w = (j<wids[i] ? j : wids[i]);
                    Jsi_DSAppendLen(dStr, apColName[i], w);
                    w = (j<wids[i] ? wids[i]-j+1 : 0);
                    while (w-- > 0)
                        Jsi_DSAppend(dStr, " ", NULL);
                }
                for(i=0; i<nCol && opts.headers; i++) {
                    w = wids[i];
                    w -= Jsi_Strlen(apColName[i]);
                    if (i) {
                        Jsi_DSAppend(dStr, opts.separator, NULL);
                        Jsi_DSAppend(&sStr, opts.separator, NULL);
                    }
                    w = wids[i];
                    while (w-- > 0)
                        Jsi_DSAppend(&sStr, "-", NULL);
                }
                if (opts.headers)
                    Jsi_DSAppend(dStr, "\n", Jsi_DSValue(&sStr), "\n", NULL);
                Jsi_DSFree(&sStr);
            }

            if (cnt)
                Jsi_DSAppend(dStr, "\n", NULL);
            for(i=0; i<nCol; i++) {
                if (i)
                    Jsi_DSAppend(dStr, opts.separator, NULL);
                Jsi_DSSetLength(&vStr, 0);
                dbEvalSetColumn(&sEval, i, &vStr);
                int nl = Jsi_DSLength(&vStr);
                if (nl > wids[i]) {
                    Jsi_DSSetLength(&vStr, wids[i]);
                    w = 0;
                } else {
                    w = wids[i]-nl;
                }
                Jsi_DSAppend(dStr, Jsi_DSValue(&vStr), NULL);
                while (w-- > 0)
                    Jsi_DSAppend(dStr, " ", NULL);
            }
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        Jsi_DSFree(&vStr);
        if (wids)
            Jsi_Free(wids);
        break;
    }
    
    case _JSI_EF_INSERT: {
        Jsi_DString vStr = {};    
        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            const char *tbl = (opts.table ? opts.table : "table");
            Jsi_DSAppend(dStr, "INSERT INTO \"", tbl, "\" VALUES(", NULL);
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            for(i=0; i<nCol; i++) {
                Jsi_Number dv;
                const char *azArg;
                Jsi_DSSetLength(&vStr, 0);
                dbEvalSetColumn(&sEval, i, &vStr);
                sqlite3_stmt *pStmt = sEval.pPreStmt->pStmt;
                int ptype = sqlite3_column_type(pStmt, i);
                
                azArg = Jsi_DSValue(&vStr);
                const char *zSep = i>0 ? ",": "";
                if(apColType && apColType[i]==SQLITE_NULL) {
                  Jsi_DSAppend(dStr, zSep, "NULL", NULL);
                }else if( ptype ==SQLITE_TEXT ) {
                  if( zSep[0] ) Jsi_DSAppend(dStr,zSep, NULL);
                  dbOutputQuotedString(dStr, azArg);
                }else if (ptype==SQLITE_INTEGER || ptype ==SQLITE_FLOAT) {
                  Jsi_DSAppend(dStr, zSep, azArg, NULL);
                }else if (ptype ==SQLITE_BLOB) {
                  const void *pBlob = sqlite3_column_blob(pStmt, i );
                  int nBlob = sqlite3_column_bytes(pStmt, i);
                  if( zSep[0] ) Jsi_DSAppend(dStr,zSep, NULL);
                  dbOutputHexBlob(dStr, pBlob, nBlob);
                }else if( Jsi_GetDouble(interp, azArg, &dv) == JSI_OK ){
                  Jsi_DSAppend(dStr, zSep, azArg, NULL);
                }else{
                  if( zSep[0] ) Jsi_DSAppend(dStr,zSep, NULL);
                  dbOutputQuotedString(dStr, azArg);
                }
            }
            Jsi_DSAppend(dStr, ");\n", NULL);
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        Jsi_DSFree(&vStr);
    }

    case _JSI_EF_TABS:
    case _JSI_EF_CSV: {
        Jsi_DString vStr = {};  
        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0 && opts.headers) {
                for(i=0; i<nCol; i++) {
                    if (i)
                        Jsi_DSAppend(dStr, opts.separator, NULL);
                    Jsi_DSAppend(dStr, apColName[i], NULL);
                }
            }

            if (cnt || opts.headers)
                Jsi_DSAppend(dStr, "\n", NULL);
            for(i=0; i<nCol; i++) {
                if (i)
                    Jsi_DSAppend(dStr, opts.separator, NULL);
                Jsi_DSSetLength(&vStr, 0);
                dbEvalSetColumn(&sEval, i, &vStr);
                if (opts.mode == _JSI_EF_CSV)
                    dbOutputCsv(&opts, Jsi_DSValue(&vStr), dStr, 0);
                else
                    Jsi_DSAppend(dStr, Jsi_DSValue(&vStr), NULL);
            }
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        Jsi_DSFree(&vStr);
        break;
    }
        
    case _JSI_EF_LINE: {
        int i, w = 5, ww;
        int nCol;
        Jsi_DString vStr = {};   
        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0) {
                for(i=0; i<nCol; i++) {
                    ww = Jsi_Strlen(apColName[i]);
                    if (ww>w)
                        w = ww;
                }
            }

            for(i=0; i<nCol; i++) {
                Jsi_DString eStr;
                Jsi_DSInit(&eStr);
                Jsi_DSSetLength(&vStr, 0);
                dbEvalSetColumn(&sEval, i, &vStr);
                Jsi_DSPrintf(&eStr, "%*s = %s", w, apColName[i], Jsi_DSValue(&vStr));
                Jsi_DSAppend(dStr, (cnt?"\n":""), Jsi_DSValue(&eStr), NULL);
                Jsi_DSFree(&eStr);
            }
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        Jsi_DSFree(&vStr);
        break;
    }
        
    case _JSI_EF_HTML: {
        Jsi_DString vStr = {};   
        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0 && opts.headers) {
                Jsi_DSAppend(dStr, "<TR>", NULL);
                for(i=0; i<nCol; i++) {
                    Jsi_DSAppend(dStr, "<TH>", NULL);
                    dbOutputHtmlString(&opts, apColName[i], dStr);
                    Jsi_DSAppend(dStr, "</TH>", NULL);
                }
                Jsi_DSAppend(dStr, "</TR>", NULL);
            }
            if (cnt || opts.headers)
                Jsi_DSAppend(dStr, "\n", NULL);
            Jsi_DSAppend(dStr, "<TR>", NULL);
            for(i=0; i<nCol; i++) {
                Jsi_DSAppend(dStr, "<TD>", NULL);
                Jsi_DSSetLength(&vStr, 0);
                dbEvalSetColumn(&sEval, i, &vStr);
                dbOutputHtmlString(&opts, Jsi_DSValue(&vStr), dStr);
                Jsi_DSAppend(dStr, "</TD>", NULL);
            }
            Jsi_DSAppend(dStr, "</TR>", NULL);
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        Jsi_DSFree(&vStr);
        break;
    }
        
    case _JSI_EF_ROWS:
    {
        Jsi_Value *vcur, *vrow;
        int cnt = 0;
        Jsi_Obj *oall, *ocur;
        Jsi_ValueMakeArrayObject(interp, ret, oall = Jsi_ObjNewType(interp, JSI_OT_ARRAY));

        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            ocur = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
            vrow = Jsi_ValueMakeObject(interp, NULL, ocur);
            for(i=0; i<nCol; i++) {
                vcur = dbEvalSetColumnValue(&sEval, i, NULL);
                Jsi_ObjInsert(interp, ocur, apColName[i], vcur, 0);
            }
            Jsi_ObjArrayAdd(interp, oall, vrow);
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        dbEvalFinalize(&sEval);
        if (rc != JSI_ERROR)
            rc = JSI_OK;
        goto bail;
        break;
    }
    case _JSI_EF_ARRAYS:
    {
        Jsi_Value *vcur, *vrow;
        int cnt = 0;
        Jsi_Obj *oall, *ocur;
        Jsi_ValueMakeArrayObject(interp, ret, oall = Jsi_ObjNewType(interp, JSI_OT_ARRAY));

        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0 && opts.headers) {
                vrow = Jsi_ValueMakeArrayObject(interp, NULL, ocur = Jsi_ObjNewType(interp, JSI_OT_ARRAY));
                for(i=0; i<nCol; i++) {
                    vcur = Jsi_ValueNewStringDup(interp, apColName[i]);
                    Jsi_ObjArrayAdd(interp, ocur, vcur);
                }
                Jsi_ObjArrayAdd(interp, oall, vrow);
            }
            vrow = Jsi_ValueMakeArrayObject(interp, NULL, ocur = Jsi_ObjNewType(interp, JSI_OT_ARRAY));
            for(i=0; i<nCol; i++) {
                vcur = dbEvalSetColumnValue(&sEval, i, NULL);
                Jsi_ObjArrayAdd(interp, ocur, vcur);
            }
            Jsi_ObjArrayAdd(interp, oall, vrow);
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        dbEvalFinalize(&sEval);
        if (rc != JSI_ERROR)
            rc = JSI_OK;
        goto bail;
        break;
    }
    case _JSI_EF_ARRAY1D:
    {
        Jsi_Value *vcur;
        int cnt = 0;
        Jsi_Obj *oall;
        Jsi_ValueMakeArrayObject(interp, ret, oall = Jsi_ObjNewType(interp, JSI_OT_ARRAY));

        while( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
            int i;
            int nCol;
            dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
            if (cnt == 0 && opts.headers) {
                for(i=0; i<nCol; i++) {
                    vcur = Jsi_ValueNewStringDup(interp, apColName[i]);
                    Jsi_ObjArrayAdd(interp, oall, vcur);
                }
            }
            for(i=0; i<nCol; i++) {
                vcur = dbEvalSetColumnValue(&sEval, i, NULL);
                Jsi_ObjArrayAdd(interp, oall, vcur);
            }
            cnt++;
            if (opts.limit && cnt>=opts.limit) break;
        }
        dbEvalFinalize(&sEval);
        if (rc != JSI_ERROR)
            rc = JSI_OK;
        goto bail;
        break;
    }
    }
    if (opts.retChanged)
        Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)sqlite3_changes(jdb->db));
    else
        Jsi_ValueFromDS(interp, dStr, ret);
    if( rc==JSI_BREAK )
        rc = JSI_OK;
bail:
    dbEvalFinalize(&sEval);
    if (isopts) {
        Jsi_OptionsFree(interp, ExecFmtOptions, &opts, 0);
    }
    Jsi_DSFree(dStr);
    Jsi_DSFree(&eStr);
    jdb->optPtr = oEopt;
    return rc;
}