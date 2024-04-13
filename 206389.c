  void Resolve(Local<Value> value) override {
    finished_ = true;
    HandleScope scope(env()->isolate());
    InternalCallbackScope callback_scope(this);
    Local<Value> val =
        object()->Get(env()->context(),
                      env()->promise_string()).ToLocalChecked();
    Local<Promise::Resolver> resolver = val.As<Promise::Resolver>();
    resolver->Resolve(env()->context(), value).FromJust();
  }