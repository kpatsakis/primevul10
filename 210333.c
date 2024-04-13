Nan::NAN_METHOD_RETURN_TYPE TxnWrap::putCommon(Nan::NAN_METHOD_ARGS_TYPE info, void (*fillFunc)(Nan::NAN_METHOD_ARGS_TYPE info, MDB_val&), void (*freeData)(MDB_val&)) {
    Nan::HandleScope scope;
    
    if (info.Length() != 3 && info.Length() != 4) {
        return Nan::ThrowError("Invalid number of arguments to txn.put");
    }

    TxnWrap *tw = Nan::ObjectWrap::Unwrap<TxnWrap>(info.This());
    DbiWrap *dw = Nan::ObjectWrap::Unwrap<DbiWrap>(Local<Object>::Cast(info[0]));

    if (!tw->txn) {
        return Nan::ThrowError("The transaction is already closed.");
    }

    int flags = 0;
    MDB_val key, data;
    bool keyIsValid;
    auto keyType = inferAndValidateKeyType(info[1], info[3], dw->keyType, keyIsValid);
    if (!keyIsValid) {
        // inferAndValidateKeyType already threw an error
        return;
    }
    auto freeKey = argToKey(info[1], key, keyType, keyIsValid);
    if (!keyIsValid) {
        // argToKey already threw an error
        return;
    }
    
    if (!info[3]->IsNull() && !info[3]->IsUndefined() && info[3]->IsObject()) {
        auto options = Local<Object>::Cast(info[3]);
        setFlagFromValue(&flags, MDB_NODUPDATA, "noDupData", false, options);
        setFlagFromValue(&flags, MDB_NOOVERWRITE, "noOverwrite", false, options);
        setFlagFromValue(&flags, MDB_APPEND, "append", false, options);
        setFlagFromValue(&flags, MDB_APPENDDUP, "appendDup", false, options);
        
        // NOTE: does not make sense to support MDB_RESERVE, because it wouldn't save the memcpy from V8 to lmdb
    }

    // Fill key and data
    fillFunc(info, data);
    
    // Keep a copy of the original key and data, so we can free them
    MDB_val originalKey = key;
    MDB_val originalData = data;

    int rc = mdb_put(tw->txn, dw->dbi, &key, &data, flags);
    
    // Free original key and data (what was supplied by the user, not what points to lmdb)
    if (freeKey) {
        freeKey(originalKey);
    }
    if (freeData) {
        freeData(originalData);
    }

    // Check result code
    if (rc != 0) {
        return throwLmdbError(rc);
    }
}