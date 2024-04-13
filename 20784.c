SProcXkbGetKbdByName(ClientPtr client)
{
    REQUEST(xkbGetKbdByNameReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbGetKbdByNameReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->want);
    swaps(&stuff->need);
    return ProcXkbGetKbdByName(client);
}