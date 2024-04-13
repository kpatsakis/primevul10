IsoPresentation_createUserDataACSE(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int bufPos = 0;
    uint8_t* buffer = writeBuffer->buffer;

    int payloadLength = payload->length;

    int userDataLengthFieldSize = BerEncoder_determineLengthSize(payloadLength);
    ;
    int pdvListLength = payloadLength + (userDataLengthFieldSize + 4);

    int pdvListLengthFieldSize = BerEncoder_determineLengthSize(pdvListLength);
    int presentationLength = pdvListLength + (pdvListLengthFieldSize + 1);

    bufPos = BerEncoder_encodeTL(0x61, presentationLength, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x30, pdvListLength, buffer, bufPos);

    buffer[bufPos++] = (uint8_t) 0x02;
    buffer[bufPos++] = (uint8_t) 0x01;
    buffer[bufPos++] = (uint8_t) self->acseContextId; /* ACSE context id */

    bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payloadLength;
    writeBuffer->nextPart = payload;
}