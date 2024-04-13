urnStart(HttpRequest *r, StoreEntry *e, const AccessLogEntryPointer &ale)
{
    const auto anUrn = new UrnState(ale);
    anUrn->start (r, e);
}