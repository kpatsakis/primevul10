void Parser::Error(HRESULT hr)
{
    Assert(FAILED(hr));
    m_err.Throw(hr);
}