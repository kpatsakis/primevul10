static void codec_vdbe_return_string(Parse *pParse, const char *zLabel, const char *value, int value_type){
  Vdbe *v = sqlite3GetVdbe(pParse);
  sqlite3VdbeSetNumCols(v, 1);
  sqlite3VdbeSetColName(v, 0, COLNAME_NAME, zLabel, SQLITE_STATIC);
  sqlite3VdbeAddOp4(v, OP_String8, 0, 1, 0, value, value_type);
  sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 1);
}