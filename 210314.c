int sqlcipher_find_db_index(sqlite3 *db, const char *zDb) {
  int db_index;
  if(zDb == NULL){
    return 0;
  }
  for(db_index = 0; db_index < db->nDb; db_index++) {
    struct Db *pDb = &db->aDb[db_index];
    if(strcmp(pDb->zDbSName, zDb) == 0) {
      return db_index;
    }
  }
  return 0;
}