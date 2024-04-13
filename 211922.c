    const EntryPtr& entryPtr() const {
      assertx(m_kind == Kind::SmartPtr);
      return m_u.smart_ptr;
    }