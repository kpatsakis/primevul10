static void ipa_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw)
{
/*   wmfBrush */
/*     *brush = WMF_DC_BRUSH(rop_draw->dc); */

/*   wmfBMP */
/*     *brush_bmp = WMF_BRUSH_BITMAP(brush); */

  if (TO_FILL(rop_draw) == 0)
    return;

  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  /* FIXME: finish implementing (once we know what it is supposed to do!) */

  /*
  struct _wmfROP_Draw_t
  {       wmfDC* dc;

    wmfD_Coord TL;
    wmfD_Coord BR;

    U32 ROP;

    double pixel_width;
    double pixel_height;
  };
  */

/*   if (brush_bmp && brush_bmp->data != 0) */
/*     printf("Have an image!\n"); */

  switch (rop_draw->ROP) /* Ternary raster operations */
    {
    case SRCCOPY: /* dest = source */
      printf("ipa_rop_draw SRCCOPY ROP mode not implemented\n");
      break;
    case SRCPAINT: /* dest = source OR dest */
      printf("ipa_rop_draw SRCPAINT ROP mode not implemented\n");
      break;
    case SRCAND: /* dest = source AND dest */
      printf("ipa_rop_draw SRCAND ROP mode not implemented\n");
      break;
    case SRCINVERT: /* dest = source XOR dest */
      printf("ipa_rop_draw SRCINVERT ROP mode not implemented\n");
      break;
    case SRCERASE: /* dest = source AND (NOT dest) */
      printf("ipa_rop_draw SRCERASE ROP mode not implemented\n");
      break;
    case NOTSRCCOPY: /* dest = (NOT source) */
      printf("ipa_rop_draw NOTSRCCOPY ROP mode not implemented\n");
      break;
    case NOTSRCERASE: /* dest = (NOT src) AND (NOT dest) */
      printf("ipa_rop_draw NOTSRCERASE ROP mode not implemented\n");
      break;
    case MERGECOPY: /* dest = (source AND pattern) */
      printf("ipa_rop_draw MERGECOPY ROP mode not implemented\n");
      break;
    case MERGEPAINT: /* dest = (NOT source) OR dest */
      printf("ipa_rop_draw MERGEPAINT ROP mode not implemented\n");
      break;
    case PATCOPY: /* dest = pattern */
      util_set_brush(API, rop_draw->dc, BrushApplyFill);
      break;
    case PATPAINT: /* dest = DPSnoo */
      printf("ipa_rop_draw PATPAINT ROP mode not implemented\n");
      break;
    case PATINVERT: /* dest = pattern XOR dest */
      printf("ipa_rop_draw PATINVERT ROP mode not implemented\n");
      break;
    case DSTINVERT: /* dest = (NOT dest) */
      printf("ipa_rop_draw DSTINVERT ROP mode not implemented\n");
      break;
    case BLACKNESS: /* dest = BLACK */
      draw_fill_color_string(WmfDrawingWand,"black");
      break;
    case WHITENESS: /* dest = WHITE */
      draw_fill_color_string(WmfDrawingWand,"white");
      break;
    default:
      printf("ipa_rop_draw 0x%x ROP mode not implemented\n", rop_draw->ROP);
      break;
    }

  DrawRectangle(WmfDrawingWand,
                 XC(rop_draw->TL.x), YC(rop_draw->TL.y),
                 XC(rop_draw->BR.x), YC(rop_draw->BR.y));

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}