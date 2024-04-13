static void util_set_brush(wmfAPI *API, wmfDC *dc,const BrushApply brush_apply)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  wmfBrush
    *brush = WMF_DC_BRUSH(dc);

  /* Set polygon fill rule */
  switch (WMF_DC_POLYFILL(dc))  /* Is this correct ?? */
    {
    case WINDING:
      DrawSetClipRule(WmfDrawingWand,NonZeroRule);
      break;

    case ALTERNATE:
    default:
      DrawSetClipRule(WmfDrawingWand,EvenOddRule);
      break;
    }

  switch (WMF_BRUSH_STYLE(brush))
    {
    case BS_SOLID /* 0 */:
      /* WMF_BRUSH_COLOR specifies brush color, WMF_BRUSH_HATCH
         ignored */
      {
        if ( brush_apply == BrushApplyStroke )
          draw_stroke_color_rgb(API,WMF_BRUSH_COLOR(brush));
        else
          draw_fill_color_rgb(API,WMF_BRUSH_COLOR(brush));
        break;
      }
    case BS_HOLLOW /* 1 */:    /* BS_HOLLOW & BS_NULL share enum */
      /* WMF_BRUSH_COLOR and WMF_BRUSH_HATCH ignored */
      {
        if ( brush_apply == BrushApplyStroke )
          draw_stroke_color_string(WmfDrawingWand,"none");
        else
          draw_fill_color_string(WmfDrawingWand,"none");
        break;
      }
    case BS_HATCHED /* 2 */:
      /* WMF_BRUSH_COLOR specifies the hatch color, WMF_BRUSH_HATCH
         specifies the hatch brush style. If WMF_DC_OPAQUE, then
         WMF_DC_BACKGROUND specifies hatch background color.  */
      {
        DrawPushDefs(WmfDrawingWand);
        draw_pattern_push(API, ddata->pattern_id, 8, 8);
        (void) PushDrawingWand(WmfDrawingWand);

        if (WMF_DC_OPAQUE(dc))
          {
            if ( brush_apply == BrushApplyStroke )
              draw_stroke_color_rgb(API,WMF_DC_BACKGROUND(dc));
            else
              draw_fill_color_rgb(API,WMF_DC_BACKGROUND(dc));

            DrawRectangle(WmfDrawingWand, 0, 0, 7, 7 );
          }

        DrawSetStrokeAntialias(WmfDrawingWand, MagickFalse);
        DrawSetStrokeWidth(WmfDrawingWand, 1);

        draw_stroke_color_rgb(API,WMF_BRUSH_COLOR(brush));

        switch ((unsigned int) WMF_BRUSH_HATCH(brush))
          {

          case HS_HORIZONTAL:  /* ----- */
            {
              DrawLine(WmfDrawingWand, 0, 3, 7,3);
              break;
            }
          case HS_VERTICAL:  /* ||||| */
            {
              DrawLine(WmfDrawingWand, 3, 0, 3, 7);
              break;
            }
          case HS_FDIAGONAL:  /* \\\\\ */
            {
              DrawLine(WmfDrawingWand, 0, 0, 7, 7);
              break;
            }
          case HS_BDIAGONAL:  /* / */
            {
              DrawLine(WmfDrawingWand, 0, 7, 7, 0 );
              break;
            }
          case HS_CROSS:  /* +++++ */
            {
              DrawLine(WmfDrawingWand, 0, 3, 7, 3 );
              DrawLine(WmfDrawingWand, 3, 0, 3, 7 );
              break;
            }
          case HS_DIAGCROSS:  /* xxxxx */
            {
              DrawLine(WmfDrawingWand, 0, 0, 7, 7 );
              DrawLine(WmfDrawingWand, 0, 7, 7, 0 );
              break;
            }
          default:
            {
              printf("util_set_brush: unexpected brush hatch enumeration %u\n",
                     (unsigned int)WMF_BRUSH_HATCH(brush));
            }
          }
        (void) PopDrawingWand(WmfDrawingWand);
        (void) DrawPopPattern(WmfDrawingWand);
        DrawPopDefs(WmfDrawingWand);
        {
          char
            pattern_id[MagickPathExtent];

          (void) FormatLocaleString(pattern_id,MagickPathExtent,"#brush_%lu",
            ddata->pattern_id);
          if (brush_apply == BrushApplyStroke )
            (void) DrawSetStrokePatternURL(WmfDrawingWand,pattern_id);
          else
            (void) DrawSetFillPatternURL(WmfDrawingWand,pattern_id);
          ++ddata->pattern_id;
        }
        break;
      }
    case BS_PATTERN /* 3 */:
      /* WMF_BRUSH_COLOR ignored, WMF_BRUSH_HATCH provides handle to
         bitmap */
      {
        printf("util_set_brush: BS_PATTERN not supported\n");
        break;
      }
    case BS_INDEXED /* 4 */:
      {
        printf("util_set_brush: BS_INDEXED not supported\n");
        break;
      }
    case BS_DIBPATTERN /* 5 */:
      {
        wmfBMP
          *brush_bmp = WMF_BRUSH_BITMAP(brush);

        if (brush_bmp && brush_bmp->data != 0)
          {
            CompositeOperator
              mode;

            const Image
              *image;

            MagickWand
              *magick_wand;

            image = (Image*)brush_bmp->data;

            mode = CopyCompositeOp;  /* Default is copy */
            switch (WMF_DC_ROP(dc))
              {
                /* Binary raster ops */
              case R2_BLACK:
                printf("util_set_brush: R2_BLACK ROP2 mode not supported!\n");
                break;
              case R2_NOTMERGEPEN:
                printf("util_set_brush: R2_NOTMERGEPEN ROP2 mode not supported!\n");
                break;
              case R2_MASKNOTPEN:
                printf("util_set_brush R2_MASKNOTPEN ROP2 mode not supported!\n");
                break;
              case R2_NOTCOPYPEN:
                printf("util_set_brush: R2_NOTCOPYPEN ROP2 mode not supported!\n");
                break;
              case R2_MASKPENNOT:
                printf("util_set_brush: R2_MASKPENNOT ROP2 mode not supported!\n");
                break;
              case R2_NOT:
                printf("util_set_brush: R2_NOT ROP2 mode not supported!\n");
                break;
              case R2_XORPEN:
                printf("util_set_brush: R2_XORPEN ROP2 mode not supported!\n");
                break;
              case R2_NOTMASKPEN:
                printf("util_set_brush: R2_NOTMASKPEN ROP2 mode not supported!\n");
                break;
              case R2_MASKPEN:
                printf("util_set_brush: R2_MASKPEN ROP2 mode not supported!\n");
                break;
              case R2_NOTXORPEN:
                printf("util_set_brush: R2_NOTXORPEN ROP2 mode not supported!\n");
                break;
              case R2_NOP:
                printf("util_set_brush: R2_NOP ROP2 mode not supported!\n");
                break;
              case R2_MERGENOTPEN:
                printf("util_set_brush: R2_MERGENOTPEN ROP2 mode not supported!\n");
                break;
              case R2_COPYPEN:
                mode = CopyCompositeOp;
                break;
              case R2_MERGEPENNOT:
                printf("util_set_brush: R2_MERGEPENNOT ROP2 mode not supported!\n");
                break;
              case R2_MERGEPEN:
                printf("util_set_brush: R2_MERGEPEN ROP2 mode not supported!\n");
                break;
              case R2_WHITE:
                printf("util_set_brush: R2_WHITE ROP2 mode not supported!\n");
                break;
              default:
                {
                  printf("util_set_brush: unexpected ROP2 enumeration %u!\n",
                         (unsigned int)WMF_DC_ROP(dc));
                }
              }

            DrawPushDefs(WmfDrawingWand);
            draw_pattern_push(API, ddata->pattern_id, brush_bmp->width,
              brush_bmp->height);
            magick_wand=NewMagickWandFromImage(image);
            (void) DrawComposite(WmfDrawingWand,mode, 0, 0, brush_bmp->width,
              brush_bmp->height, magick_wand);
            magick_wand=DestroyMagickWand(magick_wand);
            (void) DrawPopPattern(WmfDrawingWand);
            DrawPopDefs(WmfDrawingWand);

            {
              char
                pattern_id[MagickPathExtent];

              (void) FormatLocaleString(pattern_id,MagickPathExtent,"#brush_%lu",
                ddata->pattern_id);
              if ( brush_apply == BrushApplyStroke )
                (void) DrawSetStrokePatternURL(WmfDrawingWand,pattern_id);
              else
                (void) DrawSetFillPatternURL(WmfDrawingWand,pattern_id);
              ++ddata->pattern_id;
            }
          }
        else
          printf("util_set_brush: no BMP image data!\n");

        break;
      }
    case BS_DIBPATTERNPT /* 6 */:
      /* WMF_BRUSH_COLOR ignored, WMF_BRUSH_HATCH provides pointer to
         DIB */
      {
        printf("util_set_brush: BS_DIBPATTERNPT not supported\n");
        break;
      }
    case BS_PATTERN8X8 /* 7 */:
      {
        printf("util_set_brush: BS_PATTERN8X8 not supported\n");
        break;
      }
    case BS_DIBPATTERN8X8 /* 8 */:
      {
        printf("util_set_brush: BS_DIBPATTERN8X8 not supported\n");
        break;
      }
    default:
      {
      }
    }
}