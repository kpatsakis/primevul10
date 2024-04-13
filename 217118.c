roundLog2 (int x, LevelRoundingMode rmode)
{
    return (rmode == ROUND_DOWN)? floorLog2 (x): ceilLog2 (x);
}