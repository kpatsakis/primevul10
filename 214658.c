parseFullyEncodedData(IsoPresentation* self, uint8_t* buffer, int len, int bufPos)
{
    int presentationSelector = -1;
    bool userDataPresent = false;

    int endPos = bufPos + len;

    if (buffer[bufPos++] != 0x30) {
        if (DEBUG_PRES)
            printf("PRES: user-data parse error\n");
        return -1;
    }

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            printf("PRES: wrong parameter length\n");
        return -1;
    }

    endPos = bufPos + len;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, endPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                printf("PRES: wrong parameter length\n");
            return -1;
        }

        switch (tag) {
        case 0x02: /* presentation-context-identifier */
            if (DEBUG_PRES)
                printf("PRES: presentation-context-identifier\n");
            {
                presentationSelector = BerDecoder_decodeUint32(buffer, length, bufPos);
                self->nextContextId = presentationSelector;
                bufPos += length;
            }
            break;

        case 0x06: /* transfer-syntax-name */
            bufPos += length;
            break;

        case 0xa0:
            if (DEBUG_PRES)
                printf("PRES: fully-encoded-data\n");

            userDataPresent = true;

            self->nextPayload.buffer = buffer + bufPos;
            self->nextPayload.size = length;

            bufPos += length;
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            if (DEBUG_PRES)
                printf("PRES: fed: unknown tag %02x\n", tag);

            bufPos += length;
            break;
        }
    }

    if (!userDataPresent) {
        if (DEBUG_PRES)
            printf("PRES: user-data not present\n");
        return -1;
    }

    return bufPos;
}