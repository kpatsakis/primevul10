void StreamBase::AddMethod(Environment* env,
                           Local<Signature> signature,
                           enum PropertyAttribute attributes,
                           Local<FunctionTemplate> t,
                           JSMethodFunction* stream_method,
                           Local<String> string) {
  Local<FunctionTemplate> templ =
      env->NewFunctionTemplate(stream_method,
                               signature,
                               ConstructorBehavior::kThrow,
                               SideEffectType::kHasNoSideEffect);
  t->PrototypeTemplate()->SetAccessorProperty(
      string, templ, Local<FunctionTemplate>(), attributes);
}