encodeUserData(uint8_t* buffer, int bufPos,
        BufferChain payload, bool encode, uint8_t contextId)
{
    int payloadLength = payload->length;

    int encodedDataSetLength = 3; /* presentation-selector */

    /* presentation-data */
    encodedDataSetLength += payloadLength + 1;
    encodedDataSetLength += BerEncoder_determineLengthSize(payloadLength);

    int fullyEncodedDataLength = encodedDataSetLength;

    fullyEncodedDataLength += BerEncoder_determineLengthSize(encodedDataSetLength) + 1;

    if (encode) {
        /* fully-encoded-data */
        bufPos = BerEncoder_encodeTL(0x61, fullyEncodedDataLength, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x30, encodedDataSetLength, buffer, bufPos);

        /* presentation-selector acse */
        bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
        buffer[bufPos++] = contextId;

        /* presentation-data (= acse payload) */
        bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

        return bufPos;
    }
    else {
        int encodedUserDataLength = fullyEncodedDataLength + 1;
        encodedUserDataLength += BerEncoder_determineLengthSize(fullyEncodedDataLength);

        return encodedUserDataLength;
    }
}