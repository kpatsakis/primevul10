Nan::NAN_METHOD_RETURN_TYPE TxnWrap::getCommon(Nan::NAN_METHOD_ARGS_TYPE info, Local<Value> (*successFunc)(MDB_val&)) {
    Nan::HandleScope scope;
    
    if (info.Length() != 2 && info.Length() != 3) {
        return Nan::ThrowError("Invalid number of arguments to cursor.get");
    }

    TxnWrap *tw = Nan::ObjectWrap::Unwrap<TxnWrap>(info.This());
    DbiWrap *dw = Nan::ObjectWrap::Unwrap<DbiWrap>(Local<Object>::Cast(info[0]));

    if (!tw->txn) {
        return Nan::ThrowError("The transaction is already closed.");
    }

    MDB_val key, oldkey, data;
    bool keyIsValid;
    auto keyType = inferAndValidateKeyType(info[1], info[2], dw->keyType, keyIsValid);
    if (!keyIsValid) {
        // inferAndValidateKeyType already threw an error
        return;
    }
    auto freeKey = argToKey(info[1], key, keyType, keyIsValid);
    if (!keyIsValid) {
        // argToKey already threw an error
        return;
    }

    // Bookkeeping for old key so that we can free it even if key will point inside LMDB
    oldkey.mv_data = key.mv_data;
    oldkey.mv_size = key.mv_size;

    int rc = mdb_get(tw->txn, dw->dbi, &key, &data);
    
    if (freeKey) {
        freeKey(oldkey);
    }

    if (rc == MDB_NOTFOUND) {
        return info.GetReturnValue().Set(Nan::Null());
    }
    else if (rc != 0) {
        return throwLmdbError(rc);
    }
    else {
      return info.GetReturnValue().Set(successFunc(data));
    }
}