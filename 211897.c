    Accessor& operator=(const pcre_cache_entry* ptr) {
      assertx(m_kind == Kind::Empty || m_kind == Kind::Ptr);
      m_kind = Kind::Ptr;
      m_u.ptr = ptr;
      return *this;
    }