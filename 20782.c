SProcXkbGetCompatMap(ClientPtr client)
{
    REQUEST(xkbGetCompatMapReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetCompatMapReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->firstSI);
    swaps(&stuff->nSI);
    return ProcXkbGetCompatMap(client);
}