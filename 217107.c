precalculateTileInfo (const TileDescription& tileDesc,
		      int minX, int maxX,
		      int minY, int maxY,
		      int *&numXTiles, int *&numYTiles,
		      int &numXLevels, int &numYLevels)
{
    numXLevels = calculateNumXLevels(tileDesc, minX, maxX, minY, maxY);
    numYLevels = calculateNumYLevels(tileDesc, minX, maxX, minY, maxY);
    
    numXTiles = new int[numXLevels];
    numYTiles = new int[numYLevels];

    calculateNumTiles (numXTiles,
		       numXLevels,
		       minX, maxX,
		       tileDesc.xSize,
		       tileDesc.roundingMode);

    calculateNumTiles (numYTiles,
		       numYLevels,
		       minY, maxY,
		       tileDesc.ySize,
		       tileDesc.roundingMode);
}