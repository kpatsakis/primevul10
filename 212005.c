    ~Accessor() {
      switch (m_kind) {
        case Kind::Empty:
        case Kind::Ptr:
          break;
        case Kind::SmartPtr:
          m_u.smart_ptr.~EntryPtr();
          break;
        case Kind::AccessorKind:
          m_u.accessor.~ConstAccessor();
          break;
      }
    }