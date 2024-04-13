NAN_METHOD(TxnWrap::putBoolean) {
    return putCommon(info, [](Nan::NAN_METHOD_ARGS_TYPE info, MDB_val &data) -> void {
        auto booleanLocal = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
        booleanToPut = booleanLocal->Value();

        data.mv_size = sizeof(bool);
        data.mv_data = &booleanToPut;
    }, nullptr);
}