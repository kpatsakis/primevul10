  void endFrame(const TypedValue* retval, const char* symbol,
                bool /*endMain*/ = false) override {
    if (m_stack.empty()) {
      fprintf(stderr, "STACK IMBALANCE empty %s\n", symbol);
      return;
    }
    auto f = m_stack.back();
    m_stack.pop_back();
    if (strcmp(f.m_symbol, symbol) != 0) {
      fprintf(stderr, "STACK IMBALANCE %s\n", symbol);
      return;
    }
    auto& memo = m_memos[symbol];
    if (memo.m_ignore) return;
    ++memo.m_count;
    memo.m_ignore = true;
    VMRegAnchor _;
    ActRec *ar = vmfp();
    // Lots of random cases to skip just to keep this simple for
    // now. There's no reason not to do more later.
    if (ar->func()->isCPPBuiltin() || isResumed(ar)) return;
    auto ret = tvAsCVarRef(retval);
    if (ret.isNull()) return;
    if (!(ret.isString() || ret.isObject() || ret.isArray())) return;
    VariableSerializer vs(VariableSerializer::Type::DebuggerSerialize);
    String sdata;
    try {
      sdata = vs.serialize(ret, true);
    } catch (...) {
      fprintf(stderr, "Serialization failure: %s\n", symbol);
      return;
    }
    if (sdata.length() < 3) return;
    if (ar->func()->cls() && ar->hasThis()) {
      memo.m_has_this = true;
      auto& member_memo = memo.m_member_memos[f.m_args.data()];
      ++member_memo.m_count;
      if (member_memo.m_return_value.length() == 0) { // First time
        member_memo.m_return_value = sdata;
        // Intentionally copy the raw pointer value
        member_memo.m_ret_tv = *retval;
        memo.m_ignore = false;
      } else if (member_memo.m_return_value == sdata) { // Same
        memo.m_ignore = false;
        if ((member_memo.m_ret_tv.m_data.num != retval->m_data.num) ||
            (member_memo.m_ret_tv.m_type != retval->m_type)) {
          memo.m_ret_tv_same = false;
        }
      } else {
        memo.m_member_memos.clear(); // Cleanup and ignore
      }
    } else {
      if (memo.m_return_value.length() == 0) { // First time
        memo.m_return_value = sdata;
        // Intentionally copy the raw pointer value
        memo.m_ret_tv = *retval;
        memo.m_ignore = false;
      } else if (memo.m_return_value == sdata) { // Same
        memo.m_ignore = false;
        if ((memo.m_ret_tv.m_data.num != retval->m_data.num) ||
            (memo.m_ret_tv.m_type != retval->m_type)) {
          memo.m_ret_tv_same = false;
        }
      } else {
        memo.m_return_value = ""; // Different, cleanup and ignore
      }
    }
  }