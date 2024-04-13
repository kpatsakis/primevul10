    CloseReq(Environment* env,
             Local<Promise> promise,
             Local<Value> ref)
        : ReqWrap(env,
                  env->fdclose_constructor_template()
                      ->NewInstance(env->context()).ToLocalChecked(),
                  AsyncWrap::PROVIDER_FILEHANDLECLOSEREQ) {
      promise_.Reset(env->isolate(), promise);
      ref_.Reset(env->isolate(), ref);
    }