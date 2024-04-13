static Jsi_RC mdbEvalPrep(MyDbEvalContext *p) {
    MysqlPrep *prep = p->prep;
    Jsi_Interp *interp = p->jdb->interp;
    Jsi_RC rc = JSI_OK;
    if( p->prep==0 ) {
        rc = mdbPrepareAndBind(p); //p->jdb, p->zSql, &p->zSql, &p->prep);
        if( rc!=JSI_OK )
            return rc;
        prep = p->prep;
        if (p->jdb->optPtr->prefetch) {
            my_bool aBool = 1;
            mysql_stmt_attr_set(prep->myStmt, STMT_ATTR_UPDATE_MAX_LENGTH, &aBool);
        }
        if (mysql_stmt_execute(prep->myStmt)) {
            Jsi_Interp *interp = p->jdb->interp;
            rc = Jsi_LogError("execute failed: %s", mysql_error(p->jdb->db));
        }
        if (p->jdb->optPtr->prefetch && mysql_stmt_store_result(prep->myStmt)) {
            Jsi_LogWarn("prefetch failed, disabling: %s", mysql_error(p->jdb->db));
            p->jdb->optPtr->prefetch = 0;
        }
        MYSQL_RES *res = mysql_stmt_result_metadata(prep->myStmt);
        MYSQL_FIELD *field;
        if (res) {
            // Setup field mappings to/from Jsi.
            prep->fieldResult = (SqlFieldResults*)Jsi_Calloc(res->field_count, sizeof(*prep->fieldResult));
            prep->colNames = (char**)Jsi_Calloc(res->field_count, sizeof(char*));
            prep->colTypes = (Jsi_OptionId*)Jsi_Calloc(res->field_count, sizeof(int));
            int iCnt = 0;
            while((field = mysql_fetch_field(res)))
            {
                assert(iCnt<prep->numCol);
                SqlFieldResults *fres = prep->fieldResult+iCnt;
                MYSQL_BIND *bindResult = prep->bindResult+iCnt;
                
                bindResult->buffer = &fres->buffer.vchar;
                fres->field = field;
                prep->colNames[iCnt] = field->name;
                switch (field->type) {
                    case MYSQL_TYPE_TINY:
                    case MYSQL_TYPE_BIT:
                        if (field->length == 1) {
                            fres->jsiTypeMap = JSI_OPTION_BOOL;
                            fres->mapType = MYSQL_TYPE_DOUBLE;
                            fres->vsize = 1;
                            break;
                            
                        }
                    case MYSQL_TYPE_SHORT:
                    case MYSQL_TYPE_LONG:
                    case MYSQL_TYPE_DECIMAL:
                        fres->jsiTypeMap = JSI_OPTION_INT64;
                        fres->mapType = MYSQL_TYPE_LONG;
                        fres->vsize = sizeof(long long);
                        break;
                    case MYSQL_TYPE_LONGLONG:
                        fres->jsiTypeMap = JSI_OPTION_INT64;
                        fres->mapType = MYSQL_TYPE_LONGLONG;
                        fres->vsize = sizeof(long long);
                        break;
                    
                    case MYSQL_TYPE_FLOAT:
                    case MYSQL_TYPE_DOUBLE:
                        fres->jsiTypeMap = JSI_OPTION_DOUBLE;
                        fres->mapType = MYSQL_TYPE_DOUBLE;
                        fres->vsize = sizeof(double);
                        break;
                    case MYSQL_TYPE_TIME:
                    case MYSQL_TYPE_DATE:
                    case MYSQL_TYPE_DATETIME:
                    case MYSQL_TYPE_TIMESTAMP:
                        fres->jsiTypeMap = JSI_OPTION_TIME_D; //TODO: time
                        fres->mapType = MYSQL_TYPE_DATETIME;
                        fres->vsize = sizeof(MYSQL_TIME);
                        break;
                    case MYSQL_TYPE_STRING:
                    default:
                        if (IS_NUM(field->type)) {
                            fres->mapType = MYSQL_TYPE_LONGLONG;
                            fres->jsiTypeMap = JSI_OPTION_DOUBLE;
                            fres->vsize = sizeof(double);
                        } else {
                            fres->jsiTypeMap = JSI_OPTION_STRING;
                            fres->mapType = MYSQL_TYPE_STRING;
                            if (p->jdb->optPtr->prefetch)
                                fres->vsize = field->max_length;
                            else
                                fres->vsize = p->jdb->optPtr->maxString;
                            if (fres->vsize <= 0)
                                fres->vsize = JSI_BUFSIZ;
                            bindResult->buffer = fres->buffer.vstring = (char*)Jsi_Malloc(fres->vsize);
                            fres->buffer.vstring[0] = 0;
                        }
                        break;
                }
                prep->colTypes[iCnt] = fres->jsiTypeMap;
                bindResult->buffer_type = fres->mapType;
                bindResult->buffer_length = fres->vsize;
                bindResult->length = &fres->len;
                bindResult->is_null = &fres->isnull;
                iCnt++;
            }
        }
        else return JSI_BREAK;
        if (mysql_stmt_bind_result(prep->myStmt, prep->bindResult)) {
            fprintf(stderr, "mysql_stmt_bind_Result(), failed. Error:%s\n", mysql_stmt_error(prep->myStmt));
            return JSI_ERROR;
        }
        mysql_free_result(res);
    }
    return rc;
}