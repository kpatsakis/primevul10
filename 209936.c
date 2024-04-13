static Jsi_Value* dbEvalSetColumnValue(DbEvalContext *p, int iCol, Jsi_Value **val) {
    Jsi_Interp *interp = p->jdb->interp;

    sqlite3_stmt *pStmt = p->pPreStmt->pStmt;
    const char *str;
    
    switch( sqlite3_column_type(pStmt, iCol) ) {
    case SQLITE_BLOB: {
        int bytes = sqlite3_column_bytes(pStmt, iCol);
        const char *zBlob = (char*)sqlite3_column_blob(pStmt, iCol);
        if( !zBlob )
            return Jsi_ValueMakeNull(interp, val);
        unsigned char *uptr = (unsigned char*)Jsi_Malloc(bytes+1);
        memcpy(uptr, zBlob, bytes);
        uptr[bytes] = 0;
        return Jsi_ValueMakeBlob(interp, val, uptr, bytes);
        break;
    }
    case SQLITE_INTEGER: {
        sqlite_int64 v = sqlite3_column_int64(pStmt, iCol);
        if (v==0 ||v==1) {
            const char *dectyp = sqlite3_column_decltype(pStmt, iCol);
            if (dectyp && !Jsi_Strncasecmp(dectyp,"bool", 4)) {
                return Jsi_ValueMakeBool(interp, val, v);
            }
        }
        if( v>=-2147483647 && v<=2147483647 ) {
            return Jsi_ValueMakeNumber(interp, val, v);
        } else {
            return Jsi_ValueMakeNumber(interp, val, v);
        }
        break;
    }
    case SQLITE_FLOAT: {
        return Jsi_ValueMakeNumber(interp, val, (Jsi_Number)sqlite3_column_double(pStmt, iCol));
        break;
    }
    case SQLITE_NULL: {
        return Jsi_ValueMakeNull(interp, val);
        break;;
    }
    case SQLITE_TEXT: {
        if (!p->jdb->noJsonConv) {
            const char *dectyp = sqlite3_column_decltype(pStmt, iCol);
            if (dectyp && !Jsi_Strncasecmp(dectyp, "charjson", 8)) {
                Jsi_Value *v = Jsi_ValueNew(interp);// NULL; //Jsi_ValueNew1(interp);
                str = (char*)sqlite3_column_text(pStmt, iCol );
                if (JSI_OK != Jsi_JSONParse(interp, str, &v, 0))
                    Jsi_LogWarn("JSON parse failure for CHARJSON column");
                return v;
            }
        }
    }
    default:
        str = (char*)sqlite3_column_text(pStmt, iCol );
        if (!str)
            str = p->jdb->optPtr->nullvalue;
        return Jsi_ValueMakeStringDup(interp, val, str?str:"");
    }
    return Jsi_ValueNew1(interp);
}