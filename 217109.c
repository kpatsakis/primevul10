getTiledChunkOffsetTableSize(const Header& header)
{
    //
    // Save the dataWindow information
    //

    const Box2i &dataWindow = header.dataWindow();
    
    //
    // Precompute level and tile information.
    //

    int* numXTiles=nullptr;
    int* numYTiles=nullptr;
    int numXLevels;
    int numYLevels;
    try
    {
        precalculateTileInfo (header.tileDescription(),
                            dataWindow.min.x, dataWindow.max.x,
                            dataWindow.min.y, dataWindow.max.y,
                            numXTiles, numYTiles,
                            numXLevels, numYLevels);

        //
        // Calculate lineOffsetSize.
        //
        Int64 lineOffsetSize = 0;
        const TileDescription &desc = header.tileDescription();
        switch (desc.mode)
        {
            case ONE_LEVEL:
            case MIPMAP_LEVELS:
                for (int i = 0; i < numXLevels; i++)
                {
                    lineOffsetSize += static_cast<Int64>(numXTiles[i]) * static_cast<Int64>(numYTiles[i]);
                    if ( lineOffsetSize > static_cast<Int64>(std::numeric_limits<int>::max()) )
                    {
                        throw IEX_NAMESPACE::LogicExc("Maximum number of tiles exceeded");
                    }
                }
            break;
            case RIPMAP_LEVELS:
                for (int i = 0; i < numXLevels; i++)
                {
                    for (int j = 0; j < numYLevels; j++)
                    {
                        lineOffsetSize += static_cast<Int64>(numXTiles[i]) * static_cast<Int64>(numYTiles[j]);
                        if ( lineOffsetSize > static_cast<Int64>(std::numeric_limits<int>::max()) )
                        {
                            throw IEX_NAMESPACE::LogicExc("Maximum number of tiles exceeded");
                        }
                    }
                }
            break;
            case NUM_LEVELMODES :
                throw IEX_NAMESPACE::LogicExc("Bad level mode getting chunk offset table size");
        }
        delete[] numXTiles;
        delete[] numYTiles;

        return static_cast<int>(lineOffsetSize);

    }
    catch(...)
    {
        delete[] numXTiles;
        delete[] numYTiles;

        throw;
    }

}