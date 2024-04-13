IsoPresentation_parseUserData(IsoPresentation* self, ByteBuffer* readBuffer)
{
    uint8_t* buffer = readBuffer->buffer;
    int maxBufPos = readBuffer->size;
    bool hasAbstractSyntaxName = false;

    int bufPos = 0;

    if (maxBufPos < 9)
        return 0;

    if (buffer[bufPos++] != 0x61)
        return 0;

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            printf("PRES: invalid message!\n");
        return 0;
    }

    if (buffer[bufPos++] != 0x30)
        return 0;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            printf("PRES: invalid message!\n");
        return 0;
    }

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        uint8_t lenField = buffer[bufPos];

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                printf("PRES: wrong parameter length\n");
            return 0;
        }

        switch (tag) {
        case 0x02: /* abstract-syntax-name */
            self->nextContextId = buffer[bufPos];
            hasAbstractSyntaxName = true;
            bufPos += len;
            break;

        case 0x06: /* transfer-syntax-name */
            {
                /* check if basic-encoding (2.1.1 - 51 01) */
                if ((buffer[bufPos] != 0x51) || (buffer[bufPos + 1] != 0x01)) {
                    if (DEBUG_PRES) {
                        printf("PRES: unknown transfer-syntax-name\n");
                    }

                    return 0;
                }

                bufPos += len;
            }
            break;


        case 0xa0: /* presentation data */
            {
                if (hasAbstractSyntaxName == false) {
                    if (DEBUG_PRES)
                        printf("PRES: abstract-syntax-name missing!\n");

                    return 0;
                }

                int userDataLength = len;

                if (lenField == 0x80)
                    userDataLength = userDataLength - 2;

                ByteBuffer_wrap(&(self->nextPayload), buffer + bufPos, userDataLength, userDataLength);

                return 1;
            }
            break;
        }
    }

    return 0;
}