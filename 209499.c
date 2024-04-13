static Jsi_RC SqliteImportCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                         Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb;
    Jsi_RC rv = JSI_OK;
    int rc;
    char *zTable;               /* Insert data into this table */
    char *zFile;                /* The file from which to extract data */
    const char *zConflict;            /* The conflict algorithm to use */
    sqlite3_stmt *pStmt;        /* A statement */
    int nCol;                   /* Number of columns in the table */
    int nByte;                  /* Number of bytes in an SQL string */
    int i, j;                   /* Loop counters */
    int nSep;                   /* Number of bytes in zSep[] */
    int nNull;                  /* Number of bytes in zNull[] */
    char *zSql;                 /* An SQL statement */
    char *zLine;                /* A single line of input from the file */
    char **azCol;               /* zLine[] broken up into columns */
    const char *onCommit;              /* How to commit changes */
    Jsi_Channel in;                   /* The input file */
    int lineno = 0;             /* Line number of input file */
    int created = 0;
    const char *zSep;
    const char *zNull;

    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 2);
    ImportData opts = {};

    if (arg) {
        if (Jsi_OptionsProcess(interp, ImportOptions, &opts, arg, 0) < 0)
            return JSI_ERROR;
    }
    zConflict = copyConflictStrs[opts.conflict];
    
    if(opts.separator ) {
        zSep = opts.separator;
    } else {
        zSep = (opts.csv ? "," : "\t");
    }
    if(opts.nullvalue ) {
        zNull = opts.nullvalue;
    } else {
        zNull = "";
    }
    zTable = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_Value *fname = Jsi_ValueArrayIndex(interp, args, 1);
    zFile = Jsi_ValueArrayIndexToStr(interp, args, 1, NULL);
    nSep = Jsi_Strlen(zSep);
    nNull = Jsi_Strlen(zNull);
    if( nSep==0 ) 
        return Jsi_LogError("Error: non-null separator required for copy");


    zSql = sqlite3_mprintf("SELECT * FROM '%q'", zTable);
    if (zSql==0) 
        return Jsi_LogError("Error: bad table: %s", zTable);
    
    if (opts.headers) {
        in = Jsi_Open(interp, fname, "rb");
        if( in==0 ) 
            return Jsi_LogError("Error: cannot open file: %s", zFile);
        if ((zLine = dbLocalGetline(interp, 0, in))==0 ) {
            Jsi_Close(interp, in);
            return JSI_ERROR;
        }
        Jsi_Close(interp, in);
        char *zn, *ze, *z = zLine;
        Jsi_DString cStr = {};
        int zlen = 0, icnt = 0;
        Jsi_DSAppend(&cStr, "CREATE TABLE IF NOT EXISTS '", zTable, "' (", NULL);
        while (1) {
            zn = Jsi_Strstr(z, zSep);
            if (!zn) zlen = Jsi_Strlen(z);
            else zlen = zn-z;
            if (zlen<=0) break;
            ze = z+zlen-1;
            Jsi_DSAppend(&cStr, (icnt?",":""), "'", NULL);
            icnt++;
            if (opts.csv && *z=='"' && zn>z && *ze == '"')
                Jsi_DSAppendLen(&cStr, z+1, zlen-2);
            else
                Jsi_DSAppendLen(&cStr, z, zlen);
            Jsi_DSAppend(&cStr, "'", NULL);
            if (!zn) break;
            z = zn+nSep;
        }
        Jsi_DSAppend(&cStr, ");", NULL);
        Jsi_Free(zLine);
        if (zlen<=0) {
            Jsi_DSFree(&cStr);
            Jsi_LogError("null header problem");
            return JSI_ERROR;
        }
        rc = sqlite3_exec(jdb->db, Jsi_DSValue(&cStr), 0, 0, 0);
        Jsi_DSFree(&cStr);
        if (rc) 
            return Jsi_LogError("%s", sqlite3_errmsg(jdb->db));
        created = 1;
    }
    
    nByte = Jsi_Strlen(zSql);
    rc = sqlite3_prepare(jdb->db, zSql, -1, &pStmt, 0);
        sqlite3_free(zSql);
    if( rc ) {
        Jsi_LogError("%s", sqlite3_errmsg(jdb->db));
        nCol = 0;
    } else {
        nCol = sqlite3_column_count(pStmt);
    }
    sqlite3_finalize(pStmt);
    if( nCol==0 ) {
        rc = JSI_ERROR;
        goto bail;
    }
    zSql = (char*)Jsi_Malloc( nByte + 50 + nCol*2 );
    if( zSql==0 ) {
        Jsi_LogError("Error: can't malloc()");
        rc = JSI_ERROR;
        goto bail;
    }
    sqlite3_snprintf(nByte+50, zSql, "INSERT OR %q INTO '%q' VALUES(?",
                     zConflict, zTable);
    j = Jsi_Strlen(zSql);
    for(i=1; i<nCol; i++) {
        zSql[j++] = ',';
        zSql[j++] = '?';
    }
    zSql[j++] = ')';
    zSql[j] = 0;
    rc = sqlite3_prepare(jdb->db, zSql, -1, &pStmt, 0);
    Jsi_Free(zSql);
    if( rc ) {
        Jsi_LogError("Error: %s", sqlite3_errmsg(jdb->db));
        sqlite3_finalize(pStmt);
        return JSI_ERROR;
    }
    in = Jsi_Open(interp, fname, "rb");
    if( in==0 ) {
        Jsi_LogError("Error: cannot open file: %s", zFile);
        sqlite3_finalize(pStmt);
        return JSI_ERROR;
    }
    azCol = (char**)Jsi_Malloc( sizeof(azCol[0])*(nCol+1) );
    if( azCol==0 ) {
        Jsi_LogError("Error: can't malloc()");
        Jsi_Close(interp, in);
        rc = JSI_ERROR;
        goto bail;
    }
    (void)sqlite3_exec(jdb->db, "BEGIN", 0, 0, 0);
    onCommit = "COMMIT";
    while ((zLine = dbLocalGetline(interp, 0, in))!=0 ) {
        char *z;
        i = 0;
        lineno++;
        if (opts.limit>0 && lineno > opts.limit) {
            Jsi_Free(zLine);
            break;
        }
        if (lineno == 1 && opts.headers) {
            Jsi_Free(zLine);
            continue;
        }
        if (opts.csv && Jsi_Strchr(zLine,'"')) 
        {
            char *zn, *z = zLine;
            Jsi_DString sStr = {};
            int qcnt = 0;
            i = -1;
            while (*z) if (*z++ == '"') qcnt++;
            z = zLine;
            if (qcnt%2) { /* aggregate quote spanning newlines */
                Jsi_DSAppend(&sStr, zLine, NULL);
                do {
                    lineno++;
                    Jsi_DSAppend(&sStr, "\n", NULL);
                    Jsi_Free(zLine);
                    if (((zLine = dbLocalGetline(interp, 0, in)))==0)
                        break;
                    Jsi_DSAppend(&sStr, zLine, NULL);
                    z = zLine;
                    while (*z) if (*z++ == '"') qcnt++;
                } while (qcnt%2);
                z = Jsi_DSValue(&sStr);
            }
            if (qcnt%2) {
                Jsi_DSFree(&sStr);
                Jsi_Free(zLine);
                Jsi_Close(interp, in);
                Jsi_LogError("unterminated string at line: %d", lineno);
                break;
            }
            while (z) {
                if (*z != '\"') { /* Handle un-quoted value */
                    zn = Jsi_Strstr(z, zSep);
                    azCol[++i] = z;
                    if (!zn)
                        break;
                    *zn = 0;
                    z = zn+nSep;
                    continue;
                }
                /* Handle quoted value */
                zn = ++z;
                Jsi_DString cStr = {};
                while (1) {
                    if (!zn)
                        break;
                    if (*zn != '"')
                        Jsi_DSAppendLen(&cStr, zn, 1);
                    else {
                        if (zn[1] == '"') {
                            zn++;
                            Jsi_DSAppendLen(&cStr, "\"", 1);
                        } else if (zn[1] == 0) {
                            break;
                        } else if (Jsi_Strncmp(zn+1,zSep, nSep)==0) {
                            *zn = 0;
                            zn += (nSep + 1);
                            break;
                        } else {
                            /* Invalid, comma should be right after close quote, so just eat quote. */
                            Jsi_DSAppendLen(&cStr, zn, 1);
                        }
                    }
                    zn++;
                }
                Jsi_Strcpy(z, Jsi_DSValue(&cStr));
                Jsi_DSFree(&cStr);
                azCol[++i] = z;
                z = zn;
            }
        } else {
            azCol[0] = zLine;
            for(i=0, z=zLine; *z; z++) {
                if( *z==zSep[0] && Jsi_Strncmp(z, zSep, nSep)==0 ) {
                    *z = 0;
                    i++;
                    if( i<nCol ) {
                        azCol[i] = &z[nSep];
                        z += nSep-1;
                    }
                }
            }
        }
        if( i+1!=nCol ) {
            Jsi_LogError("%s line %d: expected %d columns of data but found %d",
                 zFile, lineno, nCol, i+1);
            onCommit = "ROLLBACK";
            break;
        }
        for(i=0; i<nCol; i++) {
            /* check for null data, if so, bind as null */
            if( (nNull>0 && Jsi_Strcmp(azCol[i], zNull)==0)
                    || Jsi_Strlen(azCol[i])==0
              ) {
                sqlite3_bind_null(pStmt, i+1);
            } else {
                sqlite3_bind_text(pStmt, i+1, azCol[i], -1, SQLITE_STATIC );
            }
        }
        sqlite3_step(pStmt);
        rc = sqlite3_reset(pStmt);
        if (zLine)
            Jsi_Free(zLine);
        if( rc!=SQLITE_OK ) {
            Jsi_LogError("%s at line: %d", sqlite3_errmsg(jdb->db), lineno);
            onCommit = "ROLLBACK";
            break;
        }
    }
    Jsi_Free(azCol);
    Jsi_Close(interp, in);
    sqlite3_finalize(pStmt);
    (void)sqlite3_exec(jdb->db, onCommit, 0, 0, 0);

    if( onCommit[0] == 'C' ) {
        /* success, set result as number of lines processed */
        Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)lineno);
        rv = JSI_OK;
    } else {
        rv = JSI_ERROR;
    }
    
bail:
    if (rc != JSI_OK && created && opts.conflict == CC_ROLLBACK) {
        Jsi_DString cStr = {};
        Jsi_DSAppend(&cStr, "DROP TABLE IF EXISTS '", zTable, "';", NULL);
        (void)sqlite3_exec(jdb->db, Jsi_DSValue(&cStr), 0, 0, 0);
        Jsi_DSFree(&cStr);
    }
    return rv;
}