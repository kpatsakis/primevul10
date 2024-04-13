SProcXkbDispatch(ClientPtr client)
{
    REQUEST(xReq);
    switch (stuff->data) {
    case X_kbUseExtension:
        return SProcXkbUseExtension(client);
    case X_kbSelectEvents:
        return SProcXkbSelectEvents(client);
    case X_kbBell:
        return SProcXkbBell(client);
    case X_kbGetState:
        return SProcXkbGetState(client);
    case X_kbLatchLockState:
        return SProcXkbLatchLockState(client);
    case X_kbGetControls:
        return SProcXkbGetControls(client);
    case X_kbSetControls:
        return SProcXkbSetControls(client);
    case X_kbGetMap:
        return SProcXkbGetMap(client);
    case X_kbSetMap:
        return SProcXkbSetMap(client);
    case X_kbGetCompatMap:
        return SProcXkbGetCompatMap(client);
    case X_kbSetCompatMap:
        return SProcXkbSetCompatMap(client);
    case X_kbGetIndicatorState:
        return SProcXkbGetIndicatorState(client);
    case X_kbGetIndicatorMap:
        return SProcXkbGetIndicatorMap(client);
    case X_kbSetIndicatorMap:
        return SProcXkbSetIndicatorMap(client);
    case X_kbGetNamedIndicator:
        return SProcXkbGetNamedIndicator(client);
    case X_kbSetNamedIndicator:
        return SProcXkbSetNamedIndicator(client);
    case X_kbGetNames:
        return SProcXkbGetNames(client);
    case X_kbSetNames:
        return SProcXkbSetNames(client);
    case X_kbGetGeometry:
        return SProcXkbGetGeometry(client);
    case X_kbSetGeometry:
        return SProcXkbSetGeometry(client);
    case X_kbPerClientFlags:
        return SProcXkbPerClientFlags(client);
    case X_kbListComponents:
        return SProcXkbListComponents(client);
    case X_kbGetKbdByName:
        return SProcXkbGetKbdByName(client);
    case X_kbGetDeviceInfo:
        return SProcXkbGetDeviceInfo(client);
    case X_kbSetDeviceInfo:
        return SProcXkbSetDeviceInfo(client);
    case X_kbSetDebuggingFlags:
        return SProcXkbSetDebuggingFlags(client);
    default:
        return BadRequest;
    }
}