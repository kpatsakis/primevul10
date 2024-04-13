IsoPresentation_createCpaMessage(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int contentLength = 0;

    /* mode-selector */
    contentLength += 5;

    int normalModeLength = 0;

    normalModeLength += 6; /* responding-presentation-selector */

    normalModeLength += 20; /* context-definition-result-list */

    normalModeLength += encodeUserData(NULL, 0, payload, false, self->acseContextId);

    contentLength += normalModeLength;

    contentLength += BerEncoder_determineLengthSize(normalModeLength) + 1;

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x31, contentLength, buffer, bufPos);

    /* mode-selector */
    bufPos = BerEncoder_encodeTL(0xa0, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, 1, buffer, bufPos);
    buffer[bufPos++] = 1; /* 1 = normal-mode */

    /* normal-mode-parameters */
    bufPos = BerEncoder_encodeTL(0xa2, normalModeLength, buffer, bufPos);

    /* responding-presentation-selector */
    bufPos = BerEncoder_encodeTL(0x83, 4, buffer, bufPos);
    memcpy(buffer + bufPos, calledPresentationSelector, 4);
    bufPos += 4;

    /* context-definition-result-list */
    bufPos = BerEncoder_encodeTL(0xa5, 18, buffer, bufPos);
    bufPos = encodeAcceptBer(buffer, bufPos); /* accept for acse */
    bufPos = encodeAcceptBer(buffer, bufPos); /* accept for mms */

    /* encode user data */
    bufPos = encodeUserData(buffer, bufPos, payload, true, self->acseContextId);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}