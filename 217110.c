calculateBytesPerLine (const Header &header,
                       char* sampleCountBase,
                       int sampleCountXStride,
                       int sampleCountYStride,
                       int minX, int maxX,
                       int minY, int maxY,
                       std::vector<int>& xOffsets,
                       std::vector<int>& yOffsets,
                       std::vector<Int64>& bytesPerLine)
{
    const ChannelList &channels = header.channels();

    int pos = 0;
    for (ChannelList::ConstIterator c = channels.begin();
         c != channels.end();
         ++c, ++pos)
    {
        int xOffset = xOffsets[pos];
        int yOffset = yOffsets[pos];
        int i = 0;
        for (int y = minY - yOffset; y <= maxY - yOffset; y++, i++)
            for (int x = minX - xOffset; x <= maxX - xOffset; x++)
            {
                bytesPerLine[i] += sampleCount(sampleCountBase,
                                               sampleCountXStride,
                                               sampleCountYStride,
                                               x, y)
                                   * pixelTypeSize (c.channel().type);
            }
    }
}