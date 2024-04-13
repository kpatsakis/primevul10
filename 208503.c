    ~AutoParsingSuperRestrictionStateRestorer()
    {
        AssertMsg(this->m_parser != nullptr, "This just should not happen");
        this->m_parser->m_parsingSuperRestrictionState = m_originalParsingSuperRestrictionState;
    }