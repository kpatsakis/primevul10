dataWindowForTile (const TileDescription &tileDesc,
		   int minX, int maxX,
		   int minY, int maxY,
		   int dx, int dy,
		   int lx, int ly)
{
    V2i tileMin = V2i (minX + dx * tileDesc.xSize,
		       minY + dy * tileDesc.ySize);

    V2i tileMax = tileMin + V2i (tileDesc.xSize - 1, tileDesc.ySize - 1);

    V2i levelMax = dataWindowForLevel
		       (tileDesc, minX, maxX, minY, maxY, lx, ly).max;

    tileMax = V2i (std::min (tileMax[0], levelMax[0]),
		   std::min (tileMax[1], levelMax[1]));

    return Box2i (tileMin, tileMax);
}