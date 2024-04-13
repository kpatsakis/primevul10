parsePCDLEntry(IsoPresentation* self, uint8_t* buffer, int totalLength, int bufPos)
{
    int endPos = bufPos + totalLength;

    int contextId = -1;
    bool isAcse = false;
    bool isMms = false;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                printf("PRES: Invalid PDU\n");
            return -1;
        }

        switch (tag) {
        case 0x02: /* presentation-context-identifier */
            contextId = BerDecoder_decodeUint32(buffer, len, bufPos);
            bufPos += len;
            break;

        case 0x06: /* abstract-syntax-name */
            if (DEBUG_PRES)
                printf("PRES: abstract-syntax-name with len %i\n", len);

            if (len == 5) {
                if (memcmp(buffer + bufPos, asn_id_mms, 5) == 0)
                    isMms = true;
            }
            else if (len == 4) {
                if (memcmp(buffer + bufPos, asn_id_as_acse, 4) == 0)
                    isAcse = true;
            }

            bufPos += len;

            break;

        case 0x30: /* transfer-syntax-name */
            if (DEBUG_PRES)
                printf("PRES: ignore transfer-syntax-name\n");

            bufPos += len;
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            if (DEBUG_PRES)
                printf("PRES: unknown tag in presentation-context-definition-list-entry\n");
            bufPos += len;
            break;
        }
    }

    if (contextId < 0) {
        if (DEBUG_PRES)
            printf("PRES: ContextId not defined!\n");
        return -1;
    }

    if ((isAcse == false) && (isMms == false)) {
        if (DEBUG_PRES)
            printf("PRES: not an ACSE or MMS context definition\n");

        return -1;
    }

    if (isMms) {
        self->mmsContextId = (uint8_t) contextId;
        if (DEBUG_PRES)
            printf("PRES: MMS context id is %i\n", contextId);
    }
    else {
        self->acseContextId = (uint8_t) contextId;
        if (DEBUG_PRES)
            printf("PRES: ACSE context id is %i\n", contextId);
    }

    return bufPos;
}