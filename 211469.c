  void beginFrame(const char *symbol) override {
    VMRegAnchor _;
    ActRec *ar = vmfp();
    Frame f(symbol);
    if (ar->func()->cls() && ar->hasThis()) {
      auto& memo = m_memos[symbol];
      if (!memo.m_ignore) {
        ARRPROV_USE_RUNTIME_LOCATION();
        auto args = hhvm_get_frame_args(ar);
        args.append((int64_t)(ar->getThis())); // Use the pointer not the obj
        VariableSerializer vs(VariableSerializer::Type::DebuggerSerialize);
        String sdata;
        try {
          sdata = vs.serialize(VarNR{args}, true);
          f.m_args = sdata;
        } catch (...) {
          fprintf(stderr, "Args Serialization failure: %s\n", symbol);
        }
      }
    }
    m_stack.push_back(f);
  }