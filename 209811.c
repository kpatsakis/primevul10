static Jsi_RC dbBindOptionStmt(Jsi_Db *jdb, sqlite3_stmt *pStmt, OptionBind *obPtr,
                            int dataIdx, int bindMax, Jsi_CDataDb *dbopts)
{
    Jsi_Interp *interp = jdb->interp;
    int j, k, cnt = 0, idx, sidx = -1, rc = 0;
    Jsi_StructSpec *specPtr, *specs;
    void *rec;
    Jsi_DString *eStr;
    const char *bName;
    int lastBind = sqlite3_bind_parameter_count(pStmt);
    if (lastBind<=0)
        return JSI_OK;
    int structSize = 0;
    Jsi_Wide flags = 0;
    sqlite3_destructor_type statFlags = ((dbopts->noStatic)?SQLITE_TRANSIENT:SQLITE_STATIC);
    specPtr = dbopts[0].sf;
    structSize = specPtr[obPtr->optLen].size;
    
    for (j=1; j<=lastBind; j++) {
        bName = sqlite3_bind_parameter_name(pStmt, j);
        if (bName==NULL || bName[0]==0 || bName[1]==0)
            continue;
        idx = j;
        if (dbopts[0].prefix==0)
            k = 0;
        else {
            for (k=0; dbopts[k].sf; k++) {
                if (bName[0] == dbopts[k].prefix)
                    break;
            }
            if (bindMax>0 && k>=bindMax)
                continue;
            if (!dbopts[k].sf) {
                Jsi_LogError("bad bind: %s", bName);
                continue;
            }
        }
        specs = dbopts[k].sf;
        rec = dbopts[k].data;
        if (k==0) {
            if (dbopts->isMap) {
                Jsi_Map *map = *(typeof(map)*)rec;
                rec = Jsi_MapEntryFind(map, (void*)(intptr_t)dataIdx);
                if (!rec) return JSI_ERROR;
            } else if (dbopts->isPtrs)
                rec = ((void**)rec)[dataIdx];
            else
                rec = ((uchar*)rec)+ (dataIdx * structSize);
        }
        if (bName[0] == '?')
            sidx = atoi(bName+1);
        for (specPtr = specs, cnt=1; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END; specPtr++, cnt++) {
            if (specPtr->flags&JSI_OPT_DB_IGNORE)
                continue;
            if (bName[0] == '?') {
                if (cnt == sidx)
                    break;
            } else {
                const char *sName = specPtr->name;
                if (bName[1] == sName[0] && !Jsi_Strcmp(bName+1, sName))
                    break;
            }
        }
        if (specPtr->id<JSI_OPTION_BOOL || specPtr->id>=JSI_OPTION_END) 
            return Jsi_LogError("unknown bind: %s", bName);

        char *ptr = (char *)rec + specPtr->offset;
        switch (specPtr->id) {
        case JSI_OPTION_BOOL:
            rc = sqlite3_bind_int(pStmt, idx, *(int*)ptr);
            break;
        case JSI_OPTION_INT: rc = sqlite3_bind_int64(pStmt, idx, *(int*)ptr); break;
        case JSI_OPTION_UINT:rc = sqlite3_bind_int64(pStmt, idx, *(uint*)ptr); break;
        case JSI_OPTION_INT8: rc = sqlite3_bind_int64(pStmt, idx, *(int8_t*)ptr); break;
        case JSI_OPTION_UINT8:rc = sqlite3_bind_int64(pStmt, idx, *(uint8_t*)ptr); break;
        case JSI_OPTION_INT16: rc = sqlite3_bind_int64(pStmt, idx, *(int16_t*)ptr); break;
        case JSI_OPTION_UINT16:rc = sqlite3_bind_int64(pStmt, idx, *(uint16_t*)ptr); break;
        case JSI_OPTION_INT32: rc = sqlite3_bind_int64(pStmt, idx, *(int32_t*)ptr); break;
        case JSI_OPTION_UINT32:rc = sqlite3_bind_int64(pStmt, idx, *(uint32_t*)ptr); break;
        case JSI_OPTION_TIME_W:
        case JSI_OPTION_INT64: rc = sqlite3_bind_int64(pStmt, idx, *(int64_t*)ptr); break;
        case JSI_OPTION_UINT64:rc = sqlite3_bind_int64(pStmt, idx, *(uint64_t*)ptr); break;
        case JSI_OPTION_USHORT:rc = sqlite3_bind_int64(pStmt, idx, *(ushort*)ptr); break;
        case JSI_OPTION_SHORT:rc = sqlite3_bind_int64(pStmt, idx, *(short*)ptr); break;
        case JSI_OPTION_LONG:rc = sqlite3_bind_int64(pStmt, idx, *(long*)ptr); break;
        case JSI_OPTION_ULONG:rc = sqlite3_bind_int64(pStmt, idx, *(ulong*)ptr); break;
        case JSI_OPTION_INTPTR_T:rc = sqlite3_bind_int64(pStmt, idx, *(intptr_t*)ptr); break;
        case JSI_OPTION_UINTPTR_T:rc = sqlite3_bind_int64(pStmt, idx, *(uintptr_t*)ptr); break;
        case JSI_OPTION_SIZE_T:rc = sqlite3_bind_int64(pStmt, idx, *(size_t*)ptr); break;
        case JSI_OPTION_SSIZE_T:rc = sqlite3_bind_int64(pStmt, idx, *(ssize_t*)ptr); break;
        case JSI_OPTION_LDOUBLE:rc = sqlite3_bind_int64(pStmt, idx, *(ldouble*)ptr); break;
        case JSI_OPTION_FLOAT:rc = sqlite3_bind_int64(pStmt, idx, *(float*)ptr); break;
        case JSI_OPTION_TIME_T:
            rc = sqlite3_bind_int64(pStmt, idx, (Jsi_Wide)*(time_t*)ptr);
            break;
        case JSI_OPTION_NUMBER:
            rc = sqlite3_bind_double(pStmt, idx, (double)*(Jsi_Number*)ptr);
            break;
        case JSI_OPTION_TIME_D:
        case JSI_OPTION_DOUBLE:
            rc = sqlite3_bind_double(pStmt, idx, (double)*(Jsi_Number*)ptr);
            break;
        case JSI_OPTION_CUSTOM: {
            Jsi_OptionCustom* cust = Jsi_OptionCustomBuiltin(specPtr->custom);
            if (cust && cust->formatProc) {
                Jsi_DString dStr;
                Jsi_DSInit(&dStr);
                if ((*cust->formatProc)(interp, (Jsi_OptionSpec*)specPtr, NULL, &dStr, rec, flags) != JSI_OK) {
                    Jsi_DSFree(&dStr);
                    return JSI_ERROR;
                }
                rc = sqlite3_bind_text(pStmt, idx, Jsi_DSValue(&dStr), -1, SQLITE_TRANSIENT );
                Jsi_DSFree(&dStr);
            } else 
                return Jsi_LogError("missing or invalid custom for \"%s\"", specPtr->name);
            break;
        }
        case JSI_OPTION_DSTRING:
            eStr = (Jsi_DString*)ptr;
            if (jdb->optPtr->nullvalue && !Jsi_Strcmp(jdb->optPtr->nullvalue, Jsi_DSValue(eStr)))
                rc = sqlite3_bind_text(pStmt, idx, NULL, -1, statFlags );
            else
                rc = sqlite3_bind_text(pStmt, idx, Jsi_DSValue(eStr), -1, statFlags );
            break;
        case JSI_OPTION_STRBUF:
            if (jdb->optPtr->nullvalue && ptr && !Jsi_Strcmp(jdb->optPtr->nullvalue, (char*)ptr))
                rc = sqlite3_bind_text(pStmt, idx, NULL, -1, statFlags );
            else
                rc = sqlite3_bind_text(pStmt, idx, (char*)ptr, -1, statFlags );
            break;
        case JSI_OPTION_STRKEY:
            rc = sqlite3_bind_text(pStmt, idx, *(char**)ptr, -1, SQLITE_STATIC );
            break;
#ifndef JSI_LITE_ONLY
        case JSI_OPTION_STRING:
            rc = sqlite3_bind_text(pStmt, idx, Jsi_ValueString(interp, *((Jsi_Value **)ptr), NULL), -1, statFlags );
            break;
#else
        case JSI_OPTION_STRING:
#endif
        case JSI_OPTION_VALUE: /* Unsupported. */
        case JSI_OPTION_VAR:
        case JSI_OPTION_OBJ:
        case JSI_OPTION_ARRAY:
        case JSI_OPTION_REGEXP:
        case JSI_OPTION_FUNC:
#ifdef __cplusplus
        case JSI_OPTION_END:
        case JSI_OPTION_USEROBJ:
#else
        default:
#endif
            Jsi_LogError("unsupported jdb option type \"%s\" for \"%s\"", jsi_DbOptionTypeStr(specPtr->id, 0), specPtr->name);
            return JSI_ERROR;

        }
        if (rc != SQLITE_OK)
            Jsi_LogError("bind failure: %s", sqlite3_errmsg(jdb->db));
    }
    cnt++;
    return JSI_OK;
}