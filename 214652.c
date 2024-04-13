IsoPresentation_createAbortUserMessage(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int contentLength = 0;

    contentLength = +encodeUserData(NULL, 0, payload, false, self->acseContextId);

    contentLength += BerEncoder_determineLengthSize(contentLength) + 1;

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0xa0, contentLength, buffer, bufPos);

    /* encode user data */
    bufPos = encodeUserData(buffer, bufPos, payload, true, self->acseContextId);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}