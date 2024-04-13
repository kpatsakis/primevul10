SProcXkbGetNames(ClientPtr client)
{
    REQUEST(xkbGetNamesReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetNamesReq);
    swaps(&stuff->deviceSpec);
    swapl(&stuff->which);
    return ProcXkbGetNames(client);
}