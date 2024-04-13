SProcXkbSetDebuggingFlags(ClientPtr client)
{
    REQUEST(xkbSetDebuggingFlagsReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetDebuggingFlagsReq);
    swapl(&stuff->affectFlags);
    swapl(&stuff->flags);
    swapl(&stuff->affectCtrls);
    swapl(&stuff->ctrls);
    swaps(&stuff->msgLength);
    return ProcXkbSetDebuggingFlags(client);
}