  FSReqBase(Environment* env, Local<Object> req, AsyncWrap::ProviderType type,
            bool use_bigint)
      : ReqWrap(env, req, type), use_bigint_(use_bigint) {
  }