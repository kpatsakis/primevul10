static Jsi_Value* mdbEvalSetColumnValue(MyDbEvalContext *p, int iCol, Jsi_Value **val) {
    Jsi_Interp *interp = p->jdb->interp;
    MysqlPrep *prep = p->prep;
    SqlFieldResults *field = prep->fieldResult+iCol;
    if (field->isnull)
        return Jsi_ValueMakeNull(interp, val);
        
    switch(field->jsiTypeMap) {
        case JSI_OPTION_STRING: {
            int bytes = field->len;
            char *zBlob = field->buffer.vstring;
            if( !zBlob ) {
                return Jsi_ValueMakeNull(interp, val);
            }
            zBlob = (char*)Jsi_Malloc(bytes+1);
            memcpy(zBlob, field->buffer.vstring, bytes);
            zBlob[bytes] = 0;
            return Jsi_ValueMakeBlob(interp, val, (unsigned char*)zBlob, bytes+1);
        }
        case JSI_OPTION_BOOL:
            return Jsi_ValueMakeBool(interp, val, field->buffer.vchar);
        case JSI_OPTION_INT64:
             return Jsi_ValueMakeNumber(interp, val, (Jsi_Number)field->buffer.vlonglong);
        //case JSI_OPTION_TIME_T:
        case JSI_OPTION_TIME_D:
        case JSI_OPTION_TIME_W: {
            Jsi_Number jtime = mdbMyTimeToJS(&field->buffer.timestamp);
            return Jsi_ValueMakeNumber(interp, val, jtime);
        }
        case JSI_OPTION_DOUBLE:
             return Jsi_ValueMakeNumber(interp, val, (Jsi_Number)field->buffer.vdouble);
        default:
            Jsi_LogWarn("unknown type: %d", field->jsiTypeMap);
    }
    return Jsi_ValueNew1(interp);
}