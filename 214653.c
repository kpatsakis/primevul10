IsoPresentation_parseConnect(IsoPresentation* self, ByteBuffer* byteBuffer)
{
    uint8_t* buffer = byteBuffer->buffer;
    int maxBufPos = byteBuffer->size;
    bool hasNormalModeParameters = false;

    int bufPos = 0;

    uint8_t cpTag = buffer[bufPos++];

    if (cpTag != 0x31) {
        if (DEBUG_PRES)
            printf("PRES: not a CP type\n");
        return 0;
    }

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            printf("PRES: invalid message!\n");
        return 0;
    }

    if (DEBUG_PRES)
        printf("PRES: CPType with len %i\n", len);

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                printf("PRES: invalid message!\n");
            return 0;
        }

        switch (tag) {
        case 0xa0: /* mode-selection */
            {
                if (buffer[bufPos++] != 0x80) {
                    if (DEBUG_PRES)
                        printf("PRES: mode-value of wrong type!\n");
                    return 0;
                }

                bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

                if (bufPos < 0) {
                    if (DEBUG_PRES)
                        printf("PRES: invalid message!\n");
                    return 0;
                }

                uint32_t modeSelector = BerDecoder_decodeUint32(buffer, len, bufPos);

                if (DEBUG_PRES)
                    printf("PRES: modesel %ui\n", modeSelector);

                bufPos += len;
            }
            break;
        case 0xa2: /* normal-mode-parameters */
            bufPos = parseNormalModeParameters(self, buffer, len, bufPos);

            if (bufPos < 0) {
                if (DEBUG_PRES)
                    printf("PRES: error parsing normal-mode-parameters\n");
                return 0;
            }
            else {
                hasNormalModeParameters = true;
            }

            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default: /* unsupported element */
            if (DEBUG_PRES)
                printf("PRES: tag %i not recognized\n", tag);
            bufPos += len;
            break;
        }
    }

    if (hasNormalModeParameters == false) {
        if (DEBUG_PRES)
            printf("PRES: error - normal mode parameters are missing\n");

        return 0;
    }

    return 1;
}