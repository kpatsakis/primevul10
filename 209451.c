static int dbOptSelect(Jsi_Db *jdb, const char *cmd, OptionBind *obPtr, Jsi_CDataDb *dbopts)
{
    void *rec = dbopts[0].data, **recPtrPtr = NULL;
    Jsi_Interp *interp = jdb->interp;
    Jsi_StructSpec *specPtr, *specs = dbopts[0].sf;
    DbEvalContext sEval = {};
    int ccnt = 0;
    Jsi_Wide flags = 0;
    const char *cPtr = Jsi_Strstr(cmd, " %s");
    if (!cPtr) cPtr = Jsi_Strstr(cmd, "\t%s");
    Jsi_DString *eStr;
#ifdef JSI_DB_DSTRING_SIZE
    JSI_DSTRING_VAR(dStr, JSI_DB_DSTRING_SIZE);
#else
    Jsi_DString sStr, *dStr = &sStr;
    Jsi_DSInit(dStr);
#endif
    dbEvalInit(interp, &sEval, jdb, NULL, dStr, 0, 0);
    if (dbopts->noCache)
        sEval.nocache = 1;
    Jsi_DSAppendLen(dStr, cmd, cPtr?(cPtr-cmd):-1);
    if (cPtr) {
        Jsi_DSAppend(dStr, " ", NULL);
        for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END; specPtr++) {
            if (specPtr == obPtr->dirtyPtr || (specPtr->flags&JSI_OPT_DB_IGNORE))
                continue;
            if (ccnt)
                Jsi_DSAppendLen(dStr, ",", 1);
            Jsi_DSAppend(dStr, "[", specPtr->name, "]", NULL);
            ccnt++; 
        }
        Jsi_DSAppend(dStr, cPtr+3, NULL);
    }
    sEval.zSql = Jsi_DSValue(dStr);
    sEval.nocache = jdb->optPtr->nocache;
    int rc = JSI_ERROR, erc = -1, structSize = 0;
    int cnt = 0, dataMax = (dbopts->isPtr2?0:1);
    int multi = (dbopts->isPtr2!=0);
    int dnum = dbopts[0].arrSize;
    if (dnum<=0 && !dbopts->isPtr2) {
        dataMax = dnum = 1;
    }
    if (dnum>1) {
        multi = 1;
        dataMax = dbopts[0].arrSize;
    }
    if (dbopts->isPtr2) {
        recPtrPtr = (void**)rec; /* This is really a void***, but this gets recast below. */
        rec = *recPtrPtr;
    }
    structSize = specs[obPtr->optLen].size;

    cnt = 0;
    int ncnt = 0, bindMax = -1, dataIdx = -1;
    while(1) {
        dataIdx++;
        if (dataIdx>=dataMax) {
            if (!dbopts->isPtr2)
                break;
            else {
            /* Handle fully dynamic allocation of memory. */
#ifndef JSI_DB_MAXDYN_SIZE
#define JSI_DB_MAXDYN_SIZE 100000000
#endif
#ifndef JSI_DB_DYN_INCR
#define JSI_DB_DYN_INCR 16
#endif
                int ddMax = (dbopts->maxSize>0?dbopts->maxSize:JSI_DB_MAXDYN_SIZE);
                if (dataMax>=ddMax)
                    break;
                int olddm = dataMax;
                dataMax += JSI_DB_DYN_INCR;
                if (dataMax>ddMax)
                    dataMax = ddMax;
                if (!olddm)
                    rec = Jsi_Calloc(dataMax+1, sizeof(void*));
                else {
                    rec = Jsi_Realloc(rec, (dataMax+1)*sizeof(void*));
                    memset((char*)rec+olddm*sizeof(void*), 0, (dataMax-olddm+1)*sizeof(void*));
                }
                *recPtrPtr = rec;
            }
        }

        rc = dbEvalStepOption(&sEval, obPtr, &ncnt, dataIdx, bindMax, dbopts, &erc);
        if (rc == JSI_ERROR)
            break;
        if (rc != JSI_OK)
            break;
        cnt += ncnt;
        sqlite3_stmt *pStmt = sEval.pPreStmt->pStmt;
        int idx;
        int nCol;
        char **apColName;
        const char *str;
        int *apColType;
        void *prec = rec;
        bindMax = 0;

        if (dbopts->isPtr2 || dbopts->isPtrs) {
            prec = ((void**)rec)[dataIdx];
            if (!prec)
                ((void**)rec)[dataIdx] = prec = Jsi_Calloc(1, structSize);
        } else
                prec = (char*)rec + (dataIdx * structSize);
        dbEvalRowInfo(&sEval, &nCol, &apColName, &apColType);
        for (idx=0; idx<nCol; idx++) {
            specPtr = dbLookupSpecFromName(specs, apColName[idx]);
            if (!specPtr) {
                Jsi_LogError("unknown column name: %s", apColName[idx]);
                goto bail;
            }          
            if (specPtr->id<JSI_OPTION_BOOL || specPtr->id>=JSI_OPTION_END) {
                Jsi_LogError("unknown option type \"%d\" for \"%s\"", specPtr->id, specPtr->name);
                goto bail;
            }
            char *ptr = (char*)prec + specPtr->offset;

            switch (specPtr->id) {
                case JSI_OPTION_BOOL:
                    *(int*)ptr = sqlite3_column_int(pStmt, idx);
                    break;
                case JSI_OPTION_INT: *(int*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_UINT: *(uint*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_INTPTR_T: *(intptr_t*)ptr = (intptr_t)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_UINTPTR_T: *(uintptr_t*)ptr = (uintptr_t)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_SIZE_T: *(size_t*)ptr = (size_t)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_SSIZE_T: *(ssize_t*)ptr = (ssize_t)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_SHORT: *(short*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_USHORT: *(ushort*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_LONG: *(long*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_ULONG: *(ulong*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_INT8: *(int8_t*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_UINT8: *(uint8_t*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_INT16: *(int16_t*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_UINT16: *(uint16_t*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_INT32: *(int32_t*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_UINT32: *(uint32_t*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_INT64: *(int64_t*)ptr = (int)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_TIME_W:
                case JSI_OPTION_UINT64: *(uint64_t*)ptr = (uint)sqlite3_column_int64(pStmt, idx); break;
                case JSI_OPTION_TIME_T:
                    *(time_t*)ptr = (time_t)sqlite3_column_int64(pStmt, idx);
                    break;
                case JSI_OPTION_NUMBER:
                    *(Jsi_Number*)ptr = (Jsi_Number)sqlite3_column_double(pStmt, idx);
                    break;
                case JSI_OPTION_TIME_D:
                case JSI_OPTION_FLOAT:
                    *(float*)ptr = (float)sqlite3_column_double(pStmt, idx);
                    break;
                case JSI_OPTION_DOUBLE:
                    *(double*)ptr = (double)sqlite3_column_double(pStmt, idx);
                    break;
                case JSI_OPTION_LDOUBLE:
                    *(ldouble*)ptr = (ldouble)sqlite3_column_double(pStmt, idx);
                    break;
                case JSI_OPTION_DSTRING:
                    eStr = (Jsi_DString*)ptr;
                    str = (char*)sqlite3_column_text(pStmt, idx );
                    if (!str)
                        str = jdb->optPtr->nullvalue;
                    Jsi_DSSet(eStr, str?str:"");
                    break;
                case JSI_OPTION_STRBUF:
                    str = (char*)sqlite3_column_text(pStmt, idx );
                    if (!str)
                        str = jdb->optPtr->nullvalue;
                    strncpy((char*)ptr, str?str:"", specPtr->size);
                    ((char*)ptr)[specPtr->size-1] = 0;
                    break;
                case JSI_OPTION_CUSTOM: {
                    Jsi_OptionCustom* cust = Jsi_OptionCustomBuiltin(specPtr->custom);
                    if (cust && cust->parseProc) {
                        str = (char*)sqlite3_column_text(pStmt, idx );
                        if ((*cust->parseProc)(interp, (Jsi_OptionSpec*)specPtr, NULL, str, prec, flags) != JSI_OK) {
                            goto bail;
                        }
                    } else {
                        Jsi_LogError("missing or invalid custom for \"%s\"", specPtr->name);
                        goto bail;
                    }
                    break;
                }
                case JSI_OPTION_STRKEY:
                    str = (char*)sqlite3_column_text(pStmt, idx );
                    if (!str)
                        str = jdb->optPtr->nullvalue;
                    *(char**)ptr = (str?(char*)Jsi_DbKeyAdd(jdb, str):NULL);
                    break;
#ifndef JSI_LITE_ONLY
                case JSI_OPTION_STRING: {
                    Jsi_Value *vPtr = *((Jsi_Value **)ptr);
                    if (!(specPtr->flags&JSI_OPT_NO_DUPVALUE)) {
                        if (vPtr) Jsi_DecrRefCount(interp, vPtr);
                        *((Jsi_Value **)ptr) = NULL;
                    }
                    str = (char*)sqlite3_column_text(pStmt, idx );
                    if (!str)
                        str = jdb->optPtr->nullvalue;
                    if (str) {
                        vPtr = Jsi_ValueNewStringDup(interp, str);
                        *((Jsi_Value **)ptr) = vPtr;
                    }
                    break;
                }
#else
                case JSI_OPTION_STRING:        
#endif
                case JSI_OPTION_VALUE: /* The rest are unsupported. */
                case JSI_OPTION_VAR:
                case JSI_OPTION_OBJ:
                case JSI_OPTION_ARRAY:
                case JSI_OPTION_REGEXP:
                case JSI_OPTION_FUNC:
                
#ifdef __cplusplus
                case JSI_OPTION_USEROBJ:
                case JSI_OPTION_END:
#else
                default:
#endif
                    JSI_DBQUERY_PRINTF( "unsupported type: %d:%s\n", specPtr->id,
                        jsi_DbOptionTypeStr(specPtr->id, 0));
                    break;
            }
        }
        if (dbopts[0].callback)
            dbopts[0].callback(interp, dbopts, prec);
        cnt++;
        if (!multi)
            break;
    }
    dbEvalFinalize(&sEval);
    if( rc==JSI_BREAK ) {
        rc = JSI_OK;
    }
    return (rc==JSI_OK?cnt:erc);

bail:
    dbEvalFinalize(&sEval);
    return erc;
}