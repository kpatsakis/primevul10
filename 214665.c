parsePresentationContextDefinitionList(IsoPresentation* self, uint8_t* buffer, int totalLength, int bufPos)
{
    int endPos = bufPos + totalLength;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);
        if (bufPos < 0)
            return -1;

        switch (tag) {
        case 0x30:
            if (DEBUG_PRES)
                printf("PRES: parse pcd entry\n");
            bufPos = parsePCDLEntry(self, buffer, len, bufPos);
            if (bufPos < 0)
                return -1;
            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default:
            if (DEBUG_PRES)
                printf("PRES: unknown tag in presentation-context-definition-list\n");
            bufPos += len;
            break;
        }
    }

    return bufPos;
}