void Parser::ValidateFormals()
{
    ParseFncFormals<false>(nullptr, nullptr, fFncNoFlgs);
    // Eat the tkRParen. The ParseFncDeclHelper caller expects to see it.
    m_pscan->Scan();
}