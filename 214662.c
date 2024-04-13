IsoPresentation_createConnectPdu(IsoPresentation* self, IsoConnectionParameters parameters,
        BufferChain buffer, BufferChain payload)
{
    self->acseContextId = 1;
    self->mmsContextId = 3;
    self->callingPresentationSelector = parameters->localPSelector;
    self->calledPresentationSelector = parameters->remotePSelector;
    createConnectPdu(self, buffer, payload);
}