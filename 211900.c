    const pcre_cache_entry* get() {
      switch (m_kind) {
        case Kind::Empty:    return nullptr;
        case Kind::Ptr:      return m_u.ptr;
        case Kind::SmartPtr: return m_u.smart_ptr.get();
        case Kind::AccessorKind: return m_u.accessor->get();
      }
      always_assert(false);
    }