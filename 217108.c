calculateBytesPerPixel (const Header &header)
{
    const ChannelList &channels = header.channels();

    size_t bytesPerPixel = 0;

    for (ChannelList::ConstIterator c = channels.begin();
	 c != channels.end();
	 ++c)
    {
	bytesPerPixel += pixelTypeSize (c.channel().type);
    }

    return bytesPerPixel;
}