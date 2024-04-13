Parser::MemberNameToTypeMap* Parser::CreateMemberNameMap(ArenaAllocator* pAllocator)
{
    Assert(pAllocator);
    return Anew(pAllocator, MemberNameToTypeMap, pAllocator, 5);
}