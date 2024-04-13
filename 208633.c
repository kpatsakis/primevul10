    AutoParsingSuperRestrictionStateRestorer(Parser* parser) : m_parser(parser)
    {
        AssertMsg(this->m_parser != nullptr, "This just should not happen");
        this->m_originalParsingSuperRestrictionState = this->m_parser->m_parsingSuperRestrictionState;
    }