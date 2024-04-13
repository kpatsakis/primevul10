  FSReqWrap(Environment* env, Local<Object> req, bool use_bigint)
      : FSReqBase(env, req, AsyncWrap::PROVIDER_FSREQWRAP, use_bigint) { }