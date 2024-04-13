SProcXkbSetCompatMap(ClientPtr client)
{
    REQUEST(xkbSetCompatMapReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetCompatMapReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->firstSI);
    swaps(&stuff->nSI);
    return ProcXkbSetCompatMap(client);
}