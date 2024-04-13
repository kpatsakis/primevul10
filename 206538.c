  void Reject(Local<Value> reject) override {
    finished_ = true;
    HandleScope scope(env()->isolate());
    InternalCallbackScope callback_scope(this);
    Local<Value> value =
        object()->Get(env()->context(),
                      env()->promise_string()).ToLocalChecked();
    Local<Promise::Resolver> resolver = value.As<Promise::Resolver>();
    resolver->Reject(env()->context(), reject).FromJust();
  }