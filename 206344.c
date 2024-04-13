  explicit FSReqPromise(Environment* env, bool use_bigint)
      : FSReqBase(env,
                  env->fsreqpromise_constructor_template()
                      ->NewInstance(env->context()).ToLocalChecked(),
                  AsyncWrap::PROVIDER_FSREQPROMISE,
                  use_bigint),
        stats_field_array_(env->isolate(), env->kFsStatsFieldsLength) {
    auto resolver = Promise::Resolver::New(env->context()).ToLocalChecked();
    object()->Set(env->context(), env->promise_string(),
                  resolver).FromJust();
  }