cr_tknzr_parse_token (CRTknzr * a_this, enum CRTokenType a_type,
                      enum CRTokenExtraType a_et, gpointer a_res,
                      gpointer a_extra_res)
{
        enum CRStatus status = CR_OK;
        CRToken *token = NULL;

        g_return_val_if_fail (a_this && PRIVATE (a_this)
                              && PRIVATE (a_this)->input
                              && a_res, CR_BAD_PARAM_ERROR);

        status = cr_tknzr_get_next_token (a_this, &token);
        if (status != CR_OK)
                return status;
        if (token == NULL)
                return CR_PARSING_ERROR;

        if (token->type == a_type) {
                switch (a_type) {
                case NO_TK:
                case S_TK:
                case CDO_TK:
                case CDC_TK:
                case INCLUDES_TK:
                case DASHMATCH_TK:
                case IMPORT_SYM_TK:
                case PAGE_SYM_TK:
                case MEDIA_SYM_TK:
                case FONT_FACE_SYM_TK:
                case CHARSET_SYM_TK:
                case IMPORTANT_SYM_TK:
                        status = CR_OK;
                        break;

                case STRING_TK:
                case IDENT_TK:
                case HASH_TK:
                case ATKEYWORD_TK:
                case FUNCTION_TK:
                case COMMENT_TK:
                case URI_TK:
                        *((CRString **) a_res) = token->u.str;
                        token->u.str = NULL;
                        status = CR_OK;
                        break;

                case EMS_TK:
                case EXS_TK:
                case PERCENTAGE_TK:
                case NUMBER_TK:
                        *((CRNum **) a_res) = token->u.num;
                        token->u.num = NULL;
                        status = CR_OK;
                        break;

                case LENGTH_TK:
                case ANGLE_TK:
                case TIME_TK:
                case FREQ_TK:
                        if (token->extra_type == a_et) {
                                *((CRNum **) a_res) = token->u.num;
                                token->u.num = NULL;
                                status = CR_OK;
                        }
                        break;

                case DIMEN_TK:
                        *((CRNum **) a_res) = token->u.num;
                        if (a_extra_res == NULL) {
                                status = CR_BAD_PARAM_ERROR;
                                goto error;
                        }

                        *((CRString **) a_extra_res) = token->dimen;
                        token->u.num = NULL;
                        token->dimen = NULL;
                        status = CR_OK;
                        break;

                case DELIM_TK:
                        *((guint32 *) a_res) = token->u.unichar;
                        status = CR_OK;
                        break;

                case UNICODERANGE_TK:
                default:
                        status = CR_PARSING_ERROR;
                        break;
                }

                cr_token_destroy (token);
                token = NULL;
        } else {
                cr_tknzr_unget_token (a_this, token);
                token = NULL;
                status = CR_PARSING_ERROR;
        }

        return status;

      error:

        if (token) {
                cr_tknzr_unget_token (a_this, token);
                token = NULL;
        }

        return status;
}