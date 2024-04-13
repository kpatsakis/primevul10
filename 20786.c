SProcXkbGetMap(ClientPtr client)
{
    REQUEST(xkbGetMapReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetMapReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->full);
    swaps(&stuff->partial);
    swaps(&stuff->virtualMods);
    return ProcXkbGetMap(client);
}