void TLSWrap::EnableKeylogCallback(
    const FunctionCallbackInfo<Value>& args) {
  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  CHECK_NOT_NULL(wrap->sc_);
#if OPENSSL_VERSION_NUMBER >= 0x1010100fL
  SSL_CTX_set_keylog_callback(wrap->sc_->ctx_.get(),
      SSLWrap<TLSWrap>::KeylogCallback);
#endif
}