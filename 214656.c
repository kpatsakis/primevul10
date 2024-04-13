IsoPresentation_parseAcceptMessage(IsoPresentation* self, ByteBuffer* byteBuffer)
{
    uint8_t* buffer = byteBuffer->buffer;
    int maxBufPos = byteBuffer->size;

    int bufPos = 0;

    uint8_t cpTag = buffer[bufPos++];

    if (cpTag != 0x31) {
        if (DEBUG_PRES)
            printf("PRES: not a CPA message\n");
        return 0;
    }

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            printf("PRES: Invalid message\n");
        return 0;
    }

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                printf("PRES: wrong parameter length\n");
            return 0;
        }

        switch (tag) {
        case 0xa0: /* mode-selector */
            bufPos += len; /* ignore content since only normal mode is allowed */
            break;
        case 0xa2: /* normal-mode-parameters */
            bufPos = parseNormalModeParameters(self, buffer, len, bufPos);

            if (bufPos < 0) {
                if (DEBUG_PRES)
                    printf("PRES: error parsing normal-mode-parameters\n");
                return 0;
            }

            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default:
            if (DEBUG_PRES)
                printf("PRES: CPA unknown tag %i\n", tag);
            bufPos += len;
            break;
        }
    }

    return 1;
}