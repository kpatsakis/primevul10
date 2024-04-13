static Jsi_Value* dbGetValueGet(Jsi_Interp *interp, sqlite3_value *pIn)
{
    Jsi_Value *v = Jsi_ValueNew(interp);
    switch (sqlite3_value_type(pIn)) {
    case SQLITE_BLOB: {
        int bytes;
        bytes = sqlite3_value_bytes(pIn);
        const char *zBlob = (char*) sqlite3_value_blob(pIn);
        if(!zBlob ) {
            return Jsi_ValueMakeNull(interp, &v);
        }
        unsigned char *uptr = (unsigned char*)Jsi_Malloc(bytes+1);
        memcpy(uptr, zBlob, bytes);
        uptr[bytes] = 0;
        return Jsi_ValueMakeBlob(interp, &v, uptr, bytes);
    }
    case SQLITE_INTEGER: {
        sqlite_int64 n = sqlite3_value_int64(pIn);
        if( n>=-2147483647 && n<=2147483647 ) {
            return Jsi_ValueMakeNumber(interp, &v, n);
        } else {
            return Jsi_ValueMakeNumber(interp, &v, n);
        }
    }
    case SQLITE_FLOAT: {
        return Jsi_ValueMakeNumber(interp, &v, (Jsi_Number)sqlite3_value_double(pIn));
    }
    case SQLITE_NULL: {
        return Jsi_ValueMakeNull(interp, &v);
    }
    default:
        return Jsi_ValueMakeStringDup(interp, &v, (char *)sqlite3_value_text(pIn));
    }
    return v;
}