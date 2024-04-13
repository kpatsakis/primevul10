SProcXkbLatchLockState(ClientPtr client)
{
    REQUEST(xkbLatchLockStateReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbLatchLockStateReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->groupLatch);
    return ProcXkbLatchLockState(client);
}