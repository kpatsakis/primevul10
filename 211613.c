    LRUCache::ConstAccessor& resetToLRU() {
      switch (m_kind) {
        case Kind::SmartPtr:
          m_u.smart_ptr.~EntryPtr();
        case Kind::Empty:
        case Kind::Ptr:
          m_kind = Kind::AccessorKind;
          new (&m_u.accessor) LRUCache::ConstAccessor();
          break;
        case Kind::AccessorKind:
          break;
      }
      return m_u.accessor;
    }