void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("WebContents", WebContents::GetConstructor(isolate)
                              ->GetFunction(context)
                              .ToLocalChecked());
  dict.SetMethod("create", &WebContents::Create);
  dict.SetMethod("fromId", &WebContents::FromWeakMapID);
  dict.SetMethod("getAllWebContents", &WebContents::GetAll);
}