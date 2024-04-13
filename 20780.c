SProcXkbGetGeometry(ClientPtr client)
{
    REQUEST(xkbGetGeometryReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetGeometryReq);
    swaps(&stuff->deviceSpec);
    swapl(&stuff->name);
    return ProcXkbGetGeometry(client);
}