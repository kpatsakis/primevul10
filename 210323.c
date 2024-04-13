NAN_METHOD(TxnWrap::putNumber) {
    return putCommon(info, [](Nan::NAN_METHOD_ARGS_TYPE info, MDB_val &data) -> void {
        auto numberLocal = Nan::To<v8::Number>(info[2]).ToLocalChecked();
        numberToPut = numberLocal->Value();

        data.mv_size = sizeof(double);
        data.mv_data = &numberToPut;
    }, nullptr);
}