SProcXkbUseExtension(ClientPtr client)
{
    REQUEST(xkbUseExtensionReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbUseExtensionReq);
    swaps(&stuff->wantedMajor);
    swaps(&stuff->wantedMinor);
    return ProcXkbUseExtension(client);
}