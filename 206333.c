  void SetReturnValue(const FunctionCallbackInfo<Value>& args) override {
    Local<Value> val =
        object()->Get(env()->context(),
                      env()->promise_string()).ToLocalChecked();
    Local<Promise::Resolver> resolver = val.As<Promise::Resolver>();
    args.GetReturnValue().Set(resolver->GetPromise());
  }