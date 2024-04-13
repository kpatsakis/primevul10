    Accessor& operator=(EntryPtr&& ep) {
      switch (m_kind) {
        case Kind::AccessorKind:
          m_u.accessor.~ConstAccessor();
        case Kind::Empty:
        case Kind::Ptr:
          m_kind = Kind::SmartPtr;
          new (&m_u.smart_ptr) EntryPtr(std::move(ep));
          break;
        case Kind::SmartPtr:
          m_u.smart_ptr = std::move(ep);
          break;
      }
      return *this;
    }