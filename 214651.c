encodeAcceptBer(uint8_t* buffer, int bufPos)
{
    bufPos = BerEncoder_encodeTL(0x30, 7, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, 1, buffer, bufPos);
    buffer[bufPos++] = 0;
    bufPos = BerEncoder_encodeTL(0x81, 2, buffer, bufPos);
    buffer[bufPos++] = 0x51;
    buffer[bufPos++] = 0x01;

    return bufPos;
}