static int decode_header(EXRContext *s, AVFrame *frame)
{
    AVDictionary *metadata = NULL;
    GetByteContext *gb = &s->gb;
    int magic_number, version, flags;
    int layer_match = 0;
    int ret;
    int dup_channels = 0;

    s->current_channel_offset = 0;
    s->xmin               = ~0;
    s->xmax               = ~0;
    s->ymin               = ~0;
    s->ymax               = ~0;
    s->xdelta             = ~0;
    s->ydelta             = ~0;
    s->channel_offsets[0] = -1;
    s->channel_offsets[1] = -1;
    s->channel_offsets[2] = -1;
    s->channel_offsets[3] = -1;
    s->pixel_type         = EXR_UNKNOWN;
    s->compression        = EXR_UNKN;
    s->nb_channels        = 0;
    s->w                  = 0;
    s->h                  = 0;
    s->tile_attr.xSize    = -1;
    s->tile_attr.ySize    = -1;
    s->is_tile            = 0;
    s->is_multipart       = 0;
    s->is_luma            = 0;
    s->current_part       = 0;

    if (bytestream2_get_bytes_left(gb) < 10) {
        av_log(s->avctx, AV_LOG_ERROR, "Header too short to parse.\n");
        return AVERROR_INVALIDDATA;
    }

    magic_number = bytestream2_get_le32(gb);
    if (magic_number != 20000630) {
        /* As per documentation of OpenEXR, it is supposed to be
         * int 20000630 little-endian */
        av_log(s->avctx, AV_LOG_ERROR, "Wrong magic number %d.\n", magic_number);
        return AVERROR_INVALIDDATA;
    }

    version = bytestream2_get_byte(gb);
    if (version != 2) {
        avpriv_report_missing_feature(s->avctx, "Version %d", version);
        return AVERROR_PATCHWELCOME;
    }

    flags = bytestream2_get_le24(gb);

    if (flags & 0x02)
        s->is_tile = 1;
    if (flags & 0x10)
        s->is_multipart = 1;
    if (flags & 0x08) {
        avpriv_report_missing_feature(s->avctx, "deep data");
        return AVERROR_PATCHWELCOME;
    }

    // Parse the header
    while (bytestream2_get_bytes_left(gb) > 0) {
        int var_size;

        while (s->is_multipart && s->current_part < s->selected_part &&
               bytestream2_get_bytes_left(gb) > 0) {
            if (bytestream2_peek_byte(gb)) {
                skip_header_chunk(s);
            } else {
                bytestream2_skip(gb, 1);
                if (!bytestream2_peek_byte(gb))
                    break;
            }
            bytestream2_skip(gb, 1);
            s->current_part++;
        }

        if (!bytestream2_peek_byte(gb)) {
            if (!s->is_multipart)
                break;
            bytestream2_skip(gb, 1);
            if (s->current_part == s->selected_part) {
                while (bytestream2_get_bytes_left(gb) > 0) {
                    if (bytestream2_peek_byte(gb)) {
                        skip_header_chunk(s);
                    } else {
                        bytestream2_skip(gb, 1);
                        if (!bytestream2_peek_byte(gb))
                            break;
                    }
                }
            }
            if (!bytestream2_peek_byte(gb))
                break;
            s->current_part++;
        }

        if ((var_size = check_header_variable(s, "channels",
                                              "chlist", 38)) >= 0) {
            GetByteContext ch_gb;
            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            bytestream2_init(&ch_gb, gb->buffer, var_size);

            while (bytestream2_get_bytes_left(&ch_gb) >= 19) {
                EXRChannel *channel;
                enum ExrPixelType current_pixel_type;
                int channel_index = -1;
                int xsub, ysub;

                if (strcmp(s->layer, "") != 0) {
                    if (strncmp(ch_gb.buffer, s->layer, strlen(s->layer)) == 0) {
                        layer_match = 1;
                        av_log(s->avctx, AV_LOG_INFO,
                               "Channel match layer : %s.\n", ch_gb.buffer);
                        ch_gb.buffer += strlen(s->layer);
                        if (*ch_gb.buffer == '.')
                            ch_gb.buffer++;         /* skip dot if not given */
                    } else {
                        layer_match = 0;
                        av_log(s->avctx, AV_LOG_INFO,
                               "Channel doesn't match layer : %s.\n", ch_gb.buffer);
                    }
                } else {
                    layer_match = 1;
                }

                if (layer_match) { /* only search channel if the layer match is valid */
                    if (!av_strcasecmp(ch_gb.buffer, "R") ||
                        !av_strcasecmp(ch_gb.buffer, "X") ||
                        !av_strcasecmp(ch_gb.buffer, "U")) {
                        channel_index = 0;
                        s->is_luma = 0;
                    } else if (!av_strcasecmp(ch_gb.buffer, "G") ||
                               !av_strcasecmp(ch_gb.buffer, "V")) {
                        channel_index = 1;
                        s->is_luma = 0;
                    } else if (!av_strcasecmp(ch_gb.buffer, "Y")) {
                        channel_index = 1;
                        s->is_luma = 1;
                    } else if (!av_strcasecmp(ch_gb.buffer, "B") ||
                               !av_strcasecmp(ch_gb.buffer, "Z") ||
                               !av_strcasecmp(ch_gb.buffer, "W")) {
                        channel_index = 2;
                        s->is_luma = 0;
                    } else if (!av_strcasecmp(ch_gb.buffer, "A")) {
                        channel_index = 3;
                    } else {
                        av_log(s->avctx, AV_LOG_WARNING,
                               "Unsupported channel %.256s.\n", ch_gb.buffer);
                    }
                }

                /* skip until you get a 0 */
                while (bytestream2_get_bytes_left(&ch_gb) > 0 &&
                       bytestream2_get_byte(&ch_gb))
                    continue;

                if (bytestream2_get_bytes_left(&ch_gb) < 4) {
                    av_log(s->avctx, AV_LOG_ERROR, "Incomplete header.\n");
                    ret = AVERROR_INVALIDDATA;
                    goto fail;
                }

                current_pixel_type = bytestream2_get_le32(&ch_gb);
                if (current_pixel_type >= EXR_UNKNOWN) {
                    avpriv_report_missing_feature(s->avctx, "Pixel type %d",
                                                  current_pixel_type);
                    ret = AVERROR_PATCHWELCOME;
                    goto fail;
                }

                bytestream2_skip(&ch_gb, 4);
                xsub = bytestream2_get_le32(&ch_gb);
                ysub = bytestream2_get_le32(&ch_gb);

                if (xsub != 1 || ysub != 1) {
                    avpriv_report_missing_feature(s->avctx,
                                                  "Subsampling %dx%d",
                                                  xsub, ysub);
                    ret = AVERROR_PATCHWELCOME;
                    goto fail;
                }

                if (channel_index >= 0 && s->channel_offsets[channel_index] == -1) { /* channel has not been previously assigned */
                    if (s->pixel_type != EXR_UNKNOWN &&
                        s->pixel_type != current_pixel_type) {
                        av_log(s->avctx, AV_LOG_ERROR,
                               "RGB channels not of the same depth.\n");
                        ret = AVERROR_INVALIDDATA;
                        goto fail;
                    }
                    s->pixel_type                     = current_pixel_type;
                    s->channel_offsets[channel_index] = s->current_channel_offset;
                } else if (channel_index >= 0) {
                    av_log(s->avctx, AV_LOG_WARNING,
                            "Multiple channels with index %d.\n", channel_index);
                    if (++dup_channels > 10) {
                        ret = AVERROR_INVALIDDATA;
                        goto fail;
                    }
                }

                s->channels = av_realloc(s->channels,
                                         ++s->nb_channels * sizeof(EXRChannel));
                if (!s->channels) {
                    ret = AVERROR(ENOMEM);
                    goto fail;
                }
                channel             = &s->channels[s->nb_channels - 1];
                channel->pixel_type = current_pixel_type;
                channel->xsub       = xsub;
                channel->ysub       = ysub;

                if (current_pixel_type == EXR_HALF) {
                    s->current_channel_offset += 2;
                } else {/* Float or UINT32 */
                    s->current_channel_offset += 4;
                }
            }

            /* Check if all channels are set with an offset or if the channels
             * are causing an overflow  */
            if (!s->is_luma) {/* if we expected to have at least 3 channels */
                if (FFMIN3(s->channel_offsets[0],
                           s->channel_offsets[1],
                           s->channel_offsets[2]) < 0) {
                    if (s->channel_offsets[0] < 0)
                        av_log(s->avctx, AV_LOG_ERROR, "Missing red channel.\n");
                    if (s->channel_offsets[1] < 0)
                        av_log(s->avctx, AV_LOG_ERROR, "Missing green channel.\n");
                    if (s->channel_offsets[2] < 0)
                        av_log(s->avctx, AV_LOG_ERROR, "Missing blue channel.\n");
                    ret = AVERROR_INVALIDDATA;
                    goto fail;
                }
            }

            // skip one last byte and update main gb
            gb->buffer = ch_gb.buffer + 1;
            continue;
        } else if ((var_size = check_header_variable(s, "dataWindow", "box2i",
                                                     31)) >= 0) {
            int xmin, ymin, xmax, ymax;
            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            xmin   = bytestream2_get_le32(gb);
            ymin   = bytestream2_get_le32(gb);
            xmax   = bytestream2_get_le32(gb);
            ymax   = bytestream2_get_le32(gb);

            if (xmin > xmax || ymin > ymax ||
                ymax == INT_MAX || xmax == INT_MAX ||
                (unsigned)xmax - xmin >= INT_MAX ||
                (unsigned)ymax - ymin >= INT_MAX) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }
            s->xmin = xmin;
            s->xmax = xmax;
            s->ymin = ymin;
            s->ymax = ymax;
            s->xdelta = (s->xmax - s->xmin) + 1;
            s->ydelta = (s->ymax - s->ymin) + 1;

            continue;
        } else if ((var_size = check_header_variable(s, "displayWindow",
                                                     "box2i", 34)) >= 0) {
            int32_t sx, sy, dx, dy;

            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            sx = bytestream2_get_le32(gb);
            sy = bytestream2_get_le32(gb);
            dx = bytestream2_get_le32(gb);
            dy = bytestream2_get_le32(gb);

            s->w = dx - sx + 1;
            s->h = dy - sy + 1;

            continue;
        } else if ((var_size = check_header_variable(s, "lineOrder",
                                                     "lineOrder", 25)) >= 0) {
            int line_order;
            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            line_order = bytestream2_get_byte(gb);
            av_log(s->avctx, AV_LOG_DEBUG, "line order: %d.\n", line_order);
            if (line_order > 2) {
                av_log(s->avctx, AV_LOG_ERROR, "Unknown line order.\n");
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            continue;
        } else if ((var_size = check_header_variable(s, "pixelAspectRatio",
                                                     "float", 31)) >= 0) {
            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            s->sar = bytestream2_get_le32(gb);

            continue;
        } else if ((var_size = check_header_variable(s, "compression",
                                                     "compression", 29)) >= 0) {
            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            if (s->compression == EXR_UNKN)
                s->compression = bytestream2_get_byte(gb);
            else {
                bytestream2_skip(gb, 1);
                av_log(s->avctx, AV_LOG_WARNING,
                       "Found more than one compression attribute.\n");
            }

            continue;
        } else if ((var_size = check_header_variable(s, "tiles",
                                                     "tiledesc", 22)) >= 0) {
            char tileLevel;

            if (!s->is_tile)
                av_log(s->avctx, AV_LOG_WARNING,
                       "Found tile attribute and scanline flags. Exr will be interpreted as scanline.\n");

            s->tile_attr.xSize = bytestream2_get_le32(gb);
            s->tile_attr.ySize = bytestream2_get_le32(gb);

            tileLevel = bytestream2_get_byte(gb);
            s->tile_attr.level_mode = tileLevel & 0x0f;
            s->tile_attr.level_round = (tileLevel >> 4) & 0x0f;

            if (s->tile_attr.level_mode >= EXR_TILE_LEVEL_UNKNOWN) {
                avpriv_report_missing_feature(s->avctx, "Tile level mode %d",
                                              s->tile_attr.level_mode);
                ret = AVERROR_PATCHWELCOME;
                goto fail;
            }

            if (s->tile_attr.level_round >= EXR_TILE_ROUND_UNKNOWN) {
                avpriv_report_missing_feature(s->avctx, "Tile level round %d",
                                              s->tile_attr.level_round);
                ret = AVERROR_PATCHWELCOME;
                goto fail;
            }

            continue;
        } else if ((var_size = check_header_variable(s, "writer",
                                                     "string", 1)) >= 0) {
            uint8_t key[256] = { 0 };

            bytestream2_get_buffer(gb, key, FFMIN(sizeof(key) - 1, var_size));
            av_dict_set(&metadata, "writer", key, 0);

            continue;
        } else if ((var_size = check_header_variable(s, "framesPerSecond",
                                                     "rational", 33)) >= 0) {
            if (!var_size) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }

            s->avctx->framerate.num = bytestream2_get_le32(gb);
            s->avctx->framerate.den = bytestream2_get_le32(gb);

            continue;
        } else if ((var_size = check_header_variable(s, "chunkCount",
                                                     "int", 23)) >= 0) {

            s->chunk_count = bytestream2_get_le32(gb);

            continue;
        } else if ((var_size = check_header_variable(s, "type",
                                                     "string", 16)) >= 0) {
            uint8_t key[256] = { 0 };

            bytestream2_get_buffer(gb, key, FFMIN(sizeof(key) - 1, var_size));
            if (strncmp("scanlineimage", key, var_size) &&
                strncmp("tiledimage", key, var_size))
                return AVERROR_PATCHWELCOME;

            continue;
        } else if ((var_size = check_header_variable(s, "preview",
                                                     "preview", 16)) >= 0) {
            uint32_t pw = bytestream2_get_le32(gb);
            uint32_t ph = bytestream2_get_le32(gb);
            int64_t psize = 4LL * pw * ph;

            if (psize >= bytestream2_get_bytes_left(gb))
                return AVERROR_INVALIDDATA;

            bytestream2_skip(gb, psize);

            continue;
        }

        // Check if there are enough bytes for a header
        if (bytestream2_get_bytes_left(gb) <= 9) {
            av_log(s->avctx, AV_LOG_ERROR, "Incomplete header\n");
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }

        // Process unknown variables
        {
            uint8_t name[256] = { 0 };
            uint8_t type[256] = { 0 };
            uint8_t value[256] = { 0 };
            int i = 0, size;

            while (bytestream2_get_bytes_left(gb) > 0 &&
                   bytestream2_peek_byte(gb) && i < 255) {
                name[i++] = bytestream2_get_byte(gb);
            }

            bytestream2_skip(gb, 1);
            i = 0;
            while (bytestream2_get_bytes_left(gb) > 0 &&
                   bytestream2_peek_byte(gb) && i < 255) {
                type[i++] = bytestream2_get_byte(gb);
            }
            bytestream2_skip(gb, 1);
            size = bytestream2_get_le32(gb);

            bytestream2_get_buffer(gb, value, FFMIN(sizeof(value) - 1, size));
            if (!strcmp(type, "string"))
                av_dict_set(&metadata, name, value, 0);
        }
    }

    if (s->compression == EXR_UNKN) {
        av_log(s->avctx, AV_LOG_ERROR, "Missing compression attribute.\n");
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }

    if (s->is_tile) {
        if (s->tile_attr.xSize < 1 || s->tile_attr.ySize < 1) {
            av_log(s->avctx, AV_LOG_ERROR, "Invalid tile attribute.\n");
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
    }

    if (bytestream2_get_bytes_left(gb) <= 0) {
        av_log(s->avctx, AV_LOG_ERROR, "Incomplete frame.\n");
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }

    frame->metadata = metadata;

    // aaand we are done
    bytestream2_skip(gb, 1);
    return 0;
fail:
    av_dict_free(&metadata);
    return ret;
}