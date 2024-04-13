dataWindowForLevel (const TileDescription &tileDesc,
		    int minX, int maxX,
		    int minY, int maxY,
		    int lx, int ly)
{
    V2i levelMin = V2i (minX, minY);

    V2i levelMax = levelMin +
		   V2i (levelSize (minX, maxX, lx, tileDesc.roundingMode) - 1,
			levelSize (minY, maxY, ly, tileDesc.roundingMode) - 1);

    return Box2i(levelMin, levelMax);
}