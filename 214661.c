createConnectPdu(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int contentLength = 0;
    int i;

    /* mode-selector */
    contentLength += 5;

    int normalModeLength = 0;

    /* called- and calling-presentation-selector */
    normalModeLength += 12;

    int pclLength = 27; /* 35; */
    pclLength += self->callingPresentationSelector.size;
    pclLength += self->calledPresentationSelector.size;

    normalModeLength += pclLength;

    normalModeLength += encodeUserData(NULL, 0, payload, false, self->acseContextId);

    normalModeLength += 2;

    contentLength += normalModeLength;

    contentLength += 1 + BerEncoder_determineLengthSize(normalModeLength);

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x31, contentLength, buffer, bufPos);

    /* mode-selector */
    bufPos = BerEncoder_encodeTL(0xa0, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, 1, buffer, bufPos);
    buffer[bufPos++] = 1; /* 1 = normal-mode */

    /* normal-mode-parameters */
    bufPos = BerEncoder_encodeTL(0xa2, normalModeLength, buffer, bufPos);

    /* calling-presentation-selector */
    bufPos = BerEncoder_encodeTL(0x81, self->callingPresentationSelector.size, buffer, bufPos);
    for (i = 0; i < self->callingPresentationSelector.size; i++)
        buffer[bufPos++] = self->callingPresentationSelector.value[i];

    /* called-presentation-selector */
    bufPos = BerEncoder_encodeTL(0x82, self->calledPresentationSelector.size, buffer, bufPos);
    for (i = 0; i < self->calledPresentationSelector.size; i++)
        buffer[bufPos++] = self->calledPresentationSelector.value[i];

    /* presentation-context-id list */
    bufPos = BerEncoder_encodeTL(0xa4, 35, buffer, bufPos);

    /* acse context list item */
    bufPos = BerEncoder_encodeTL(0x30, 15, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 1;

    bufPos = BerEncoder_encodeTL(0x06, 4, buffer, bufPos);
    memcpy(buffer + bufPos, asn_id_as_acse, 4);
    bufPos += 4;

    bufPos = BerEncoder_encodeTL(0x30, 4, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
    memcpy(buffer + bufPos, ber_id, 2);
    bufPos += 2;

    /* mms context list item */
    bufPos = BerEncoder_encodeTL(0x30, 16, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 3;

    bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
    memcpy(buffer + bufPos, asn_id_mms, 5);
    bufPos += 5;

    bufPos = BerEncoder_encodeTL(0x30, 4, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
    memcpy(buffer + bufPos, ber_id, 2);
    bufPos += 2;

    /* encode user data */
    bufPos = encodeUserData(buffer, bufPos, payload, true, self->acseContextId);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}