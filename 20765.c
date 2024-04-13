SProcXkbSetControls(ClientPtr client)
{
    REQUEST(xkbSetControlsReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbSetControlsReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->affectInternalVMods);
    swaps(&stuff->internalVMods);
    swaps(&stuff->affectIgnoreLockVMods);
    swaps(&stuff->ignoreLockVMods);
    swaps(&stuff->axOptions);
    swapl(&stuff->affectEnabledCtrls);
    swapl(&stuff->enabledCtrls);
    swapl(&stuff->changeCtrls);
    swaps(&stuff->repeatDelay);
    swaps(&stuff->repeatInterval);
    swaps(&stuff->slowKeysDelay);
    swaps(&stuff->debounceDelay);
    swaps(&stuff->mkDelay);
    swaps(&stuff->mkInterval);
    swaps(&stuff->mkTimeToMax);
    swaps(&stuff->mkMaxSpeed);
    swaps(&stuff->mkCurve);
    swaps(&stuff->axTimeout);
    swapl(&stuff->axtCtrlsMask);
    swapl(&stuff->axtCtrlsValues);
    swaps(&stuff->axtOptsMask);
    swaps(&stuff->axtOptsValues);
    return ProcXkbSetControls(client);
}