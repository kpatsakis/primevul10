scanner_scan_post_primary_expression (parser_context_t *context_p, /**< context */
                                      scanner_context_t *scanner_context_p, /**< scanner context */
                                      lexer_token_type_t type, /**< current token type */
                                      scan_stack_modes_t stack_top) /**< current stack top */
{
  switch (type)
  {
    case LEXER_DOT:
    {
      lexer_scan_identifier (context_p);

      if (context_p->token.type != LEXER_LITERAL
          || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
      {
        scanner_raise_error (context_p);
      }

      return true;
    }
    case LEXER_LEFT_PAREN:
    {
      parser_stack_push_uint8 (context_p, SCAN_STACK_PAREN_EXPRESSION);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      return true;
    }
#if ENABLED (JERRY_ES2015)
    case LEXER_TEMPLATE_LITERAL:
    {
      if (JERRY_UNLIKELY (context_p->source_p[-1] != LIT_CHAR_GRAVE_ACCENT))
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        parser_stack_push_uint8 (context_p, SCAN_STACK_TAGGED_TEMPLATE_LITERAL);
      }
      return true;
    }
#endif /* ENABLED (JERRY_ES2015) */
    case LEXER_LEFT_SQUARE:
    {
      parser_stack_push_uint8 (context_p, SCAN_STACK_PROPERTY_ACCESSOR);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      return true;
    }
    case LEXER_INCREASE:
    case LEXER_DECREASE:
    {
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;

      if (context_p->token.flags & LEXER_WAS_NEWLINE)
      {
        return false;
      }

      lexer_next_token (context_p);
      type = (lexer_token_type_t) context_p->token.type;

      if (type != LEXER_QUESTION_MARK)
      {
        break;
      }
      /* FALLTHRU */
    }
    case LEXER_QUESTION_MARK:
    {
      parser_stack_push_uint8 (context_p, SCAN_STACK_COLON_EXPRESSION);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      return true;
    }
    default:
    {
      break;
    }
  }

  if (LEXER_IS_BINARY_OP_TOKEN (type)
      && (type != LEXER_KEYW_IN || !SCANNER_IS_FOR_START (stack_top)))
  {
    scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
    return true;
  }

  return false;
} /* scanner_scan_post_primary_expression */