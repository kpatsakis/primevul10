static void draw_pattern_push( wmfAPI* API,
                               unsigned long id,
                               unsigned long columns,
                               unsigned long rows )
{
  char
    pattern_id[MagickPathExtent];

  (void) FormatLocaleString(pattern_id,MagickPathExtent,"brush_%lu",id);
  (void) DrawPushPattern(WmfDrawingWand,pattern_id,0,0,columns,rows);
}