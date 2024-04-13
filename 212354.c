static int mp4ain(int size)
{
    // Reserved (6 bytes)
    u32in();
    u16in();
    // Data reference index
    u16in();
    // Version
    u16in();
    // Revision level
    u16in();
    // Vendor
    u32in();
    // Number of channels
    mp4config.channels = u16in();
    // Sample size (bits)
    mp4config.bits = u16in();
    // Compression ID
    u16in();
    // Packet size
    u16in();
    // Sample rate (16.16)
    // fractional framerate, probably not for audio
    // rate integer part
    u16in();
    // rate reminder part
    u16in();

    return size;
}