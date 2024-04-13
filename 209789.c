Jsi_HashNew(Jsi_Interp *interp, unsigned int keyType, Jsi_HashDeleteProc freeProc)
{
    Jsi_Hash *tablePtr = (Jsi_Hash*)Jsi_Calloc(1,sizeof(*tablePtr));
    SIGINIT(tablePtr, HASH);
    tablePtr->opts.mapType = (Jsi_Map_Type)JSI_MAP_HASH;
    tablePtr->typ = JSI_MAP_HASH;
    tablePtr->opts.interp = interp;
    tablePtr->buckets = tablePtr->staticBuckets;
#if !(JSI_SMALL_HASH_TABLE && !(JSI_SMALL_HASH_TABLE & (JSI_SMALL_HASH_TABLE - 1)))
#error "small hash must be a power of two"
#endif
    tablePtr->numBuckets = JSI_SMALL_HASH_TABLE;
    tablePtr->rebuildSize = JSI_SMALL_HASH_TABLE * REBUILD_MULTIPLIER;
    tablePtr->downShift = DOWNSHIFT_START;
    tablePtr->opts.freeHashProc = freeProc;
    tablePtr->mask = (jsi_Hash)(tablePtr->numBuckets - 1);
    tablePtr->opts.keyType = (Jsi_Key_Type)keyType;
    tablePtr->keyType = (Jsi_Key_Type)keyType;

    switch (keyType) {
    case JSI_KEYS_STRING:   /* NUL terminated string keys. */
        tablePtr->findProc = HashStringFind;
        tablePtr->createProc = HashStringCreate;
        break;

    case JSI_KEYS_STRINGKEY: /* Lookup from another String hash, eg. Jsi_KeyAdd() */
    case JSI_KEYS_ONEWORD: /* A pointer. */
        tablePtr->findProc = HashOneWordFind;
        tablePtr->createProc = HashOneWordCreate;
        break;

    default:            /* Structs. */
        if (keyType < JSI_KEYS_STRUCT_MINSIZE) {
            Jsi_LogError("Jsi_HashNew: Key size can't be %d, must be >= %d", keyType, JSI_KEYS_STRUCT_MINSIZE);
            Jsi_Free(tablePtr);
            return NULL;
        }
        tablePtr->findProc = jsi_HashArrayFind;
        tablePtr->createProc = jsi_HashArrayCreate;
        break;
    }
    return tablePtr;
}