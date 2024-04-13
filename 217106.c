calculateNumYLevels (const TileDescription& tileDesc,
		     int minX, int maxX,
		     int minY, int maxY)
{
    int num = 0;

    switch (tileDesc.mode)
    {
      case ONE_LEVEL:

	num = 1;
	break;

      case MIPMAP_LEVELS:

	{
	  int w = maxX - minX + 1;
	  int h = maxY - minY + 1;
	  num = roundLog2 (std::max (w, h), tileDesc.roundingMode) + 1;
	}
        break;

      case RIPMAP_LEVELS:

	{
	  int h = maxY - minY + 1;
	  num = roundLog2 (h, tileDesc.roundingMode) + 1;
	}
	break;

      default:

	throw IEX_NAMESPACE::ArgExc ("Unknown LevelMode format.");
    }

    return num;
}