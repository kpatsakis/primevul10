NAN_METHOD(TxnWrap::del) {
    Nan::HandleScope scope;
    
    // Check argument count
    auto argCount = info.Length();
    if (argCount < 2 || argCount > 4) {
        return Nan::ThrowError("Invalid number of arguments to cursor.del, should be: (a) <dbi>, <key> (b) <dbi>, <key>, <options> (c) <dbi>, <key>, <data> (d) <dbi>, <key>, <data>, <options>");
    }

    // Unwrap native objects
    TxnWrap *tw = Nan::ObjectWrap::Unwrap<TxnWrap>(info.This());
    DbiWrap *dw = Nan::ObjectWrap::Unwrap<DbiWrap>(Local<Object>::Cast(info[0]));

    if (!tw->txn) {
        return Nan::ThrowError("The transaction is already closed.");
    }

    // Take care of options object and data handle
    Local<Value> options;
    Local<Value> dataHandle;
    
    if (argCount == 4) {
        options = info[3];
        dataHandle = info[2];
    }
    else if (argCount == 3) {
        if (info[2]->IsObject()) {
            options = info[2];
            dataHandle = Nan::Undefined();
        }
        else {
            options = Nan::Undefined();
            dataHandle = info[2];
        }
    }
    else if (argCount == 2) {
        options = Nan::Undefined();
        dataHandle = Nan::Undefined();
    }
    else {
        return Nan::ThrowError("Unknown arguments to cursor.del, this could be a node-lmdb bug!");
    }

    MDB_val key;
    bool keyIsValid;
    auto keyType = inferAndValidateKeyType(info[1], options, dw->keyType, keyIsValid);
    if (!keyIsValid) {
        // inferAndValidateKeyType already threw an error
        return;
    }
    auto freeKey = argToKey(info[1], key, keyType, keyIsValid);
    if (!keyIsValid) {
        // argToKey already threw an error
        return;
    }

    // Set data if dupSort true and data given
    MDB_val data;
    bool freeData = false;
    
    if ((dw->flags & MDB_DUPSORT) && !(dataHandle->IsUndefined())) {
        if (dataHandle->IsString()) {
            CustomExternalStringResource::writeTo(Local<String>::Cast(dataHandle), &data);
            freeData = true;
        }
        else if (node::Buffer::HasInstance(dataHandle)) {
            data.mv_size = node::Buffer::Length(dataHandle);
            data.mv_data = node::Buffer::Data(dataHandle);
            freeData = true;
        }
        else if (dataHandle->IsNumber()) {
            auto numberLocal = Nan::To<v8::Number>(dataHandle).ToLocalChecked();
            data.mv_size = sizeof(double);
            data.mv_data = new double;
            *reinterpret_cast<double*>(data.mv_data) = numberLocal->Value();
            freeData = true;
        }
        else if (dataHandle->IsBoolean()) {
            auto booleanLocal = Nan::To<v8::Boolean>(dataHandle).ToLocalChecked();
            data.mv_size = sizeof(double);
            data.mv_data = new bool;
            *reinterpret_cast<bool*>(data.mv_data) = booleanLocal->Value();
            freeData = true;
        }
        else {
            Nan::ThrowError("Invalid data type.");
        }
    }

    int rc = mdb_del(tw->txn, dw->dbi, &key, freeData ? &data : nullptr);

    if (freeKey) {
        freeKey(key);
    }
    
    if (freeData) {
        if (dataHandle->IsString()) {
            delete[] (uint16_t*)data.mv_data;
        }
        else if (node::Buffer::HasInstance(dataHandle)) {
            // I think the data is owned by the node::Buffer so we don't need to free it - need to clarify
        }
        else if (dataHandle->IsNumber()) {
            delete (double*)data.mv_data;
        }
        else if (dataHandle->IsBoolean()) {
            delete (bool*)data.mv_data;
        }
    }

    if (rc != 0) {
        return throwLmdbError(rc);
    }
}