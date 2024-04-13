scanner_scan_statement (parser_context_t *context_p, /**< context */
                        scanner_context_t *scanner_context_p, /**< scanner context */
                        lexer_token_type_t type, /**< current token type */
                        scan_stack_modes_t stack_top) /**< current stack top */
{
  switch (type)
  {
    case LEXER_SEMICOLON:
    {
      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;
      return SCAN_KEEP_TOKEN;
    }
    case LEXER_LEFT_BRACE:
    {
#if ENABLED (JERRY_ES2015)
      scanner_literal_pool_t *literal_pool_p;
      literal_pool_p = scanner_push_literal_pool (context_p,
                                                  scanner_context_p,
                                                  SCANNER_LITERAL_POOL_BLOCK);
      literal_pool_p->source_p = context_p->source_p;
#endif /* ENABLED (JERRY_ES2015) */

      scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
      parser_stack_push_uint8 (context_p, SCAN_STACK_BLOCK_STATEMENT);
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_DO:
    {
      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      parser_stack_push_uint8 (context_p, SCAN_STACK_DO_STATEMENT);
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_TRY:
    {
      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_LEFT_BRACE)
      {
        scanner_raise_error (context_p);
      }

#if ENABLED (JERRY_ES2015)
      scanner_literal_pool_t *literal_pool_p;
      literal_pool_p = scanner_push_literal_pool (context_p,
                                                  scanner_context_p,
                                                  SCANNER_LITERAL_POOL_BLOCK);
      literal_pool_p->source_p = context_p->source_p;
#endif /* ENABLED (JERRY_ES2015) */

      scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
      parser_stack_push_uint8 (context_p, SCAN_STACK_TRY_STATEMENT);
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_DEBUGGER:
    {
      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_IF:
    case LEXER_KEYW_WITH:
    case LEXER_KEYW_SWITCH:
    {
      lexer_next_token (context_p);
      if (context_p->token.type != LEXER_LEFT_PAREN)
      {
        scanner_raise_error (context_p);
      }

      uint8_t mode = SCAN_STACK_STATEMENT_WITH_EXPR;

      if (type == LEXER_KEYW_IF)
      {
        parser_stack_push_uint8 (context_p, SCAN_STACK_IF_STATEMENT);
      }
      else if (type == LEXER_KEYW_WITH)
      {
        mode = SCAN_STACK_WITH_EXPRESSION;
      }
      else if (type == LEXER_KEYW_SWITCH)
      {
        mode = SCAN_STACK_SWITCH_EXPRESSION;
      }

      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      parser_stack_push_uint8 (context_p, mode);
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_WHILE:
    {
      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_LEFT_PAREN)
      {
        scanner_raise_error (context_p);
      }

      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

      scanner_source_start_t source_start;
      source_start.source_p = context_p->source_p;

      parser_stack_push (context_p, &source_start, sizeof (scanner_source_start_t));
      parser_stack_push_uint8 (context_p, SCAN_STACK_WHILE_EXPRESSION);
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_FOR:
    {
      lexer_next_token (context_p);
      if (context_p->token.type != LEXER_LEFT_PAREN)
      {
        scanner_raise_error (context_p);
      }

      scanner_for_statement_t for_statement;
      for_statement.u.source_p = context_p->source_p;
      uint8_t stack_mode = SCAN_STACK_FOR_START;
      scan_return_types_t return_type = SCAN_KEEP_TOKEN;

      lexer_next_token (context_p);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

#if ENABLED (JERRY_ES2015)
      const uint8_t *source_p = context_p->source_p;
#endif /* ENABLED (JERRY_ES2015) */

      switch (context_p->token.type)
      {
        case LEXER_SEMICOLON:
        {
          scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;
          break;
        }
        case LEXER_KEYW_VAR:
        {
          scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
          stack_mode = SCAN_STACK_FOR_VAR_START;
          return_type = SCAN_NEXT_TOKEN;
          break;
        }
#if ENABLED (JERRY_ES2015)
        case LEXER_LITERAL:
        {
          if (!lexer_token_is_let (context_p))
          {
            break;
          }

          parser_line_counter_t line = context_p->line;
          parser_line_counter_t column = context_p->column;

          if (lexer_check_arrow (context_p))
          {
            context_p->source_p = source_p;
            context_p->line = line;
            context_p->column = column;
            context_p->token.flags &= (uint8_t) ~LEXER_NO_SKIP_SPACES;
            break;
          }

          lexer_next_token (context_p);

          type = (lexer_token_type_t) context_p->token.type;

          if (type != LEXER_LEFT_SQUARE
              && type != LEXER_LEFT_BRACE
              && (type != LEXER_LITERAL || context_p->token.lit_location.type != LEXER_IDENT_LITERAL))
          {
            scanner_info_t *info_p = scanner_insert_info (context_p, source_p, sizeof (scanner_info_t));
            info_p->type = SCANNER_TYPE_LET_EXPRESSION;

            scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
            break;
          }

          scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
          /* FALLTHRU */
        }
        case LEXER_KEYW_LET:
        case LEXER_KEYW_CONST:
        {
          scanner_literal_pool_t *literal_pool_p;
          literal_pool_p = scanner_push_literal_pool (context_p, scanner_context_p, SCANNER_LITERAL_POOL_BLOCK);
          literal_pool_p->source_p = source_p;

          if (scanner_context_p->mode == SCAN_MODE_PRIMARY_EXPRESSION)
          {
            scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
            return_type = SCAN_NEXT_TOKEN;
          }

          stack_mode = ((context_p->token.type == LEXER_KEYW_CONST) ? SCAN_STACK_FOR_CONST_START
                                                                    : SCAN_STACK_FOR_LET_START);
          break;
        }
#endif /* ENABLED (JERRY_ES2015) */
      }

      parser_stack_push (context_p, &for_statement, sizeof (scanner_for_statement_t));
      parser_stack_push_uint8 (context_p, stack_mode);
      return return_type;
    }
    case LEXER_KEYW_VAR:
    {
      scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
      parser_stack_push_uint8 (context_p, SCAN_STACK_VAR);
      return SCAN_NEXT_TOKEN;
    }
#if ENABLED (JERRY_ES2015)
    case LEXER_KEYW_LET:
    {
      scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
      parser_stack_push_uint8 (context_p, SCAN_STACK_LET);
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_CONST:
    {
      scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
      parser_stack_push_uint8 (context_p, SCAN_STACK_CONST);
      return SCAN_NEXT_TOKEN;
    }
#endif /* ENABLED (JERRY_ES2015) */
    case LEXER_KEYW_THROW:
    {
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_RETURN:
    {
      lexer_next_token (context_p);

      if (!(context_p->token.flags & LEXER_WAS_NEWLINE)
          && context_p->token.type != LEXER_SEMICOLON
          && context_p->token.type != LEXER_EOS
          && context_p->token.type != LEXER_RIGHT_BRACE)
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        return SCAN_KEEP_TOKEN;
      }

      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;
      return SCAN_KEEP_TOKEN;
    }
    case LEXER_KEYW_BREAK:
    case LEXER_KEYW_CONTINUE:
    {
      lexer_next_token (context_p);
      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;

      if (!(context_p->token.flags & LEXER_WAS_NEWLINE)
          && context_p->token.type == LEXER_LITERAL
          && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
      {
        return SCAN_NEXT_TOKEN;
      }
      return SCAN_KEEP_TOKEN;
    }
    case LEXER_KEYW_CASE:
    case LEXER_KEYW_DEFAULT:
    {
      if (stack_top != SCAN_STACK_SWITCH_BLOCK)
      {
        scanner_raise_error (context_p);
      }

      scanner_case_info_t *case_info_p;
      case_info_p = (scanner_case_info_t *) scanner_malloc (context_p, sizeof (scanner_case_info_t));

      *(scanner_context_p->active_switch_statement.last_case_p) = case_info_p;
      scanner_context_p->active_switch_statement.last_case_p = &case_info_p->next_p;

      case_info_p->next_p = NULL;
      scanner_get_location (&case_info_p->location, context_p);

      if (type == LEXER_KEYW_DEFAULT)
      {
        lexer_next_token (context_p);

        if (context_p->token.type != LEXER_COLON)
        {
          scanner_raise_error (context_p);
        }

        scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
        return SCAN_NEXT_TOKEN;
      }

      scanner_source_start_t source_start;
      source_start.source_p = context_p->source_p;

      parser_stack_push (context_p, &source_start, sizeof (scanner_source_start_t));
      parser_stack_push_uint8 (context_p, SCAN_STACK_CASE_STATEMENT);

      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_FUNCTION:
    {
#if ENABLED (JERRY_ES2015)
      uint16_t status_flags = SCANNER_LITERAL_POOL_FUNCTION | SCANNER_LITERAL_POOL_FUNCTION_STATEMENT;

      if (scanner_context_p->async_source_p != NULL)
      {
        scanner_context_p->status_flags |= SCANNER_CONTEXT_THROW_ERR_ASYNC_FUNCTION;
        status_flags |= SCANNER_LITERAL_POOL_ASYNC;
      }
#endif /* ENABLED (JERRY_ES2015) */

      lexer_next_token (context_p);

#if ENABLED (JERRY_ES2015)
      if (context_p->token.type == LEXER_MULTIPLY)
      {
        status_flags |= SCANNER_LITERAL_POOL_GENERATOR;
        lexer_next_token (context_p);
      }
#endif /* ENABLED (JERRY_ES2015) */

      if (context_p->token.type != LEXER_LITERAL
          || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
      {
        scanner_raise_error (context_p);
      }

      lexer_lit_location_t *literal_p = scanner_add_literal (context_p, scanner_context_p);

#if ENABLED (JERRY_ES2015)
      const uint8_t mask = (SCANNER_LITERAL_IS_ARG | SCANNER_LITERAL_IS_FUNC | SCANNER_LITERAL_IS_LOCAL);

      if ((literal_p->type & SCANNER_LITERAL_IS_LOCAL)
          && (literal_p->type & mask) != (SCANNER_LITERAL_IS_ARG | SCANNER_LITERAL_IS_DESTRUCTURED_ARG)
          && (literal_p->type & mask) != (SCANNER_LITERAL_IS_FUNC | SCANNER_LITERAL_IS_FUNC_DECLARATION))
      {
        scanner_raise_redeclaration_error (context_p);
      }

      literal_p->type |= SCANNER_LITERAL_IS_FUNC | SCANNER_LITERAL_IS_FUNC_DECLARATION;

      scanner_context_p->status_flags &= (uint16_t) ~SCANNER_CONTEXT_THROW_ERR_ASYNC_FUNCTION;
#else
      literal_p->type |= SCANNER_LITERAL_IS_VAR | SCANNER_LITERAL_IS_FUNC;

      uint16_t status_flags = SCANNER_LITERAL_POOL_FUNCTION;
#endif /* ENABLED (JERRY_ES2015) */

      scanner_push_literal_pool (context_p, scanner_context_p, status_flags);

      scanner_context_p->mode = SCAN_MODE_FUNCTION_ARGUMENTS;
      parser_stack_push_uint8 (context_p, SCAN_STACK_FUNCTION_STATEMENT);
      return SCAN_NEXT_TOKEN;
    }
#if ENABLED (JERRY_ES2015)
    case LEXER_KEYW_CLASS:
    {
      scanner_push_class_declaration (context_p, scanner_context_p, SCAN_STACK_CLASS_STATEMENT);

      if (context_p->token.type != LEXER_LITERAL || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
      {
        scanner_raise_error (context_p);
      }

      lexer_lit_location_t *literal_p = scanner_add_literal (context_p, scanner_context_p);

      scanner_detect_invalid_let (context_p, literal_p);
      literal_p->type |= SCANNER_LITERAL_IS_LET;

#if ENABLED (JERRY_ES2015_MODULE_SYSTEM)
      if (scanner_context_p->active_literal_pool_p->status_flags & SCANNER_LITERAL_POOL_IN_EXPORT)
      {
        literal_p->type |= SCANNER_LITERAL_NO_REG;
        scanner_context_p->active_literal_pool_p->status_flags &= (uint16_t) ~SCANNER_LITERAL_POOL_IN_EXPORT;
      }
#endif /* ENABLED (JERRY_ES2015_MODULE_SYSTEM) */

      return SCAN_NEXT_TOKEN;
    }
#endif /* ENABLED (JERRY_ES2015) */
#if ENABLED (JERRY_ES2015_MODULE_SYSTEM)
    case LEXER_KEYW_IMPORT:
    {
      if (stack_top != SCAN_STACK_SCRIPT)
      {
        scanner_raise_error (context_p);
      }

      context_p->global_status_flags |= ECMA_PARSE_MODULE;

      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;
      lexer_next_token (context_p);

      if (context_p->token.type == LEXER_LITERAL
          && context_p->token.lit_location.type == LEXER_STRING_LITERAL)
      {
        return SCAN_NEXT_TOKEN;
      }

      bool parse_imports = true;

      if (context_p->token.type == LEXER_LITERAL
          && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
      {
        lexer_lit_location_t *literal_p = scanner_add_literal (context_p, scanner_context_p);

#if ENABLED (JERRY_ES2015)
        scanner_detect_invalid_let (context_p, literal_p);
        literal_p->type |= SCANNER_LITERAL_IS_LOCAL | SCANNER_LITERAL_NO_REG;
#else /* !ENABLED (JERRY_ES2015) */
        literal_p->type |= SCANNER_LITERAL_IS_VAR | SCANNER_LITERAL_NO_REG;
#endif /* ENABLED (JERRY_ES2015) */

        lexer_next_token (context_p);

        if (context_p->token.type == LEXER_COMMA)
        {
          lexer_next_token (context_p);
        }
        else
        {
          parse_imports = false;
        }
      }

      if (parse_imports)
      {
        if (context_p->token.type == LEXER_MULTIPLY)
        {
          lexer_next_token (context_p);
          if (!lexer_token_is_identifier (context_p, "as", 2))
          {
            scanner_raise_error (context_p);
          }

          lexer_next_token (context_p);

          if (context_p->token.type != LEXER_LITERAL
              && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
          {
            scanner_raise_error (context_p);
          }

          lexer_lit_location_t *literal_p = scanner_add_literal (context_p, scanner_context_p);

#if ENABLED (JERRY_ES2015)
          scanner_detect_invalid_let (context_p, literal_p);
          literal_p->type |= SCANNER_LITERAL_IS_LOCAL | SCANNER_LITERAL_NO_REG;
#else /* !ENABLED (JERRY_ES2015) */
          literal_p->type |= SCANNER_LITERAL_IS_VAR | SCANNER_LITERAL_NO_REG;
#endif /* ENABLED (JERRY_ES2015) */

          lexer_next_token (context_p);
        }
        else if (context_p->token.type == LEXER_LEFT_BRACE)
        {
          lexer_next_token (context_p);

          while (context_p->token.type != LEXER_RIGHT_BRACE)
          {
            if (context_p->token.type != LEXER_LITERAL
                || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
            {
              scanner_raise_error (context_p);
            }

#if ENABLED (JERRY_ES2015)
            const uint8_t *source_p = context_p->source_p;
#endif /* ENABLED (JERRY_ES2015) */

            if (lexer_check_next_character (context_p, LIT_CHAR_LOWERCASE_A))
            {
              lexer_next_token (context_p);

              if (!lexer_token_is_identifier (context_p, "as", 2))
              {
                scanner_raise_error (context_p);
              }

              lexer_next_token (context_p);

              if (context_p->token.type != LEXER_LITERAL
                  && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
              {
                scanner_raise_error (context_p);
              }

#if ENABLED (JERRY_ES2015)
              source_p = context_p->source_p;
#endif /* ENABLED (JERRY_ES2015) */
            }

            lexer_lit_location_t *literal_p = scanner_add_literal (context_p, scanner_context_p);

#if ENABLED (JERRY_ES2015)
            if (literal_p->type & (SCANNER_LITERAL_IS_ARG
                                   | SCANNER_LITERAL_IS_VAR
                                   | SCANNER_LITERAL_IS_LOCAL))
            {
              context_p->source_p = source_p;
              scanner_raise_redeclaration_error (context_p);
            }

            if (literal_p->type & SCANNER_LITERAL_IS_FUNC)
            {
              literal_p->type &= (uint8_t) ~SCANNER_LITERAL_IS_FUNC;
            }

            literal_p->type |= SCANNER_LITERAL_IS_LOCAL | SCANNER_LITERAL_NO_REG;
#else /* !ENABLED (JERRY_ES2015) */
            literal_p->type |= SCANNER_LITERAL_IS_VAR | SCANNER_LITERAL_NO_REG;
#endif /* ENABLED (JERRY_ES2015) */

            lexer_next_token (context_p);

            if (context_p->token.type != LEXER_RIGHT_BRACE)
            {
              if (context_p->token.type != LEXER_COMMA)
              {
                scanner_raise_error (context_p);
              }

              lexer_next_token (context_p);
            }
          }

          lexer_next_token (context_p);
        }
        else
        {
          scanner_raise_error (context_p);
        }
      }

      if (!lexer_token_is_identifier (context_p, "from", 4))
      {
        scanner_raise_error (context_p);
      }

      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_LITERAL
          && context_p->token.lit_location.type != LEXER_STRING_LITERAL)
      {
        scanner_raise_error (context_p);
      }

      return SCAN_NEXT_TOKEN;
    }
    case LEXER_KEYW_EXPORT:
    {
      if (stack_top != SCAN_STACK_SCRIPT)
      {
        scanner_raise_error (context_p);
      }

      context_p->global_status_flags |= ECMA_PARSE_MODULE;

      lexer_next_token (context_p);

      if (context_p->token.type == LEXER_KEYW_DEFAULT)
      {
        lexer_next_token (context_p);

        if (context_p->token.type == LEXER_KEYW_FUNCTION)
        {
          lexer_next_token (context_p);
          if (context_p->token.type == LEXER_LITERAL
              && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
          {
            lexer_lit_location_t *location_p = scanner_add_literal (context_p, scanner_context_p);

#if ENABLED (JERRY_ES2015)
            if (location_p->type & SCANNER_LITERAL_IS_LOCAL
                && !(location_p->type & SCANNER_LITERAL_IS_FUNC))
            {
              scanner_raise_redeclaration_error (context_p);
            }
            location_p->type |= SCANNER_LITERAL_IS_FUNC | SCANNER_LITERAL_IS_LET;
#else /* !ENABLED (JERRY_ES2015) */
            location_p->type |= SCANNER_LITERAL_IS_VAR | SCANNER_LITERAL_IS_FUNC;
#endif /* ENABLED (JERRY_ES2015) */

            lexer_next_token (context_p);
          }
          else
          {
            lexer_lit_location_t *location_p;
            location_p = scanner_add_custom_literal (context_p,
                                                     scanner_context_p->active_literal_pool_p,
                                                     &lexer_default_literal);
#if ENABLED (JERRY_ES2015)
            location_p->type |= SCANNER_LITERAL_IS_FUNC | SCANNER_LITERAL_IS_LET;
#else /* !ENABLED (JERRY_ES2015) */
            location_p->type |= SCANNER_LITERAL_IS_VAR | SCANNER_LITERAL_IS_FUNC;
#endif /* ENABLED (JERRY_ES2015) */
          }

          scanner_push_literal_pool (context_p, scanner_context_p, SCANNER_LITERAL_POOL_FUNCTION);

          parser_stack_push_uint8 (context_p, SCAN_STACK_FUNCTION_STATEMENT);
          scanner_context_p->mode = SCAN_MODE_FUNCTION_ARGUMENTS;
          return SCAN_KEEP_TOKEN;
        }
#if ENABLED (JERRY_ES2015)
        if (context_p->token.type == LEXER_KEYW_CLASS)
        {
          scanner_push_class_declaration (context_p, scanner_context_p, SCAN_STACK_CLASS_STATEMENT);

          if (context_p->token.type == LEXER_LITERAL && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
          {
            lexer_lit_location_t *literal_p = scanner_add_literal (context_p, scanner_context_p);

            scanner_detect_invalid_let (context_p, literal_p);

            literal_p->type |= SCANNER_LITERAL_IS_LET | SCANNER_LITERAL_NO_REG;
            return SCAN_NEXT_TOKEN;
          }

          lexer_lit_location_t *literal_p;
          literal_p = scanner_add_custom_literal (context_p,
                                                  scanner_context_p->active_literal_pool_p,
                                                  &lexer_default_literal);
          literal_p->type |= SCANNER_LITERAL_IS_LET | SCANNER_LITERAL_NO_REG;
          return SCAN_KEEP_TOKEN;
        }
#endif /* ENABLED (JERRY_ES2015) */

        /* Assignment expression. */
        lexer_lit_location_t *location_p;
        location_p = scanner_add_custom_literal (context_p,
                                                 scanner_context_p->active_literal_pool_p,
                                                 &lexer_default_literal);
        location_p->type |= SCANNER_LITERAL_IS_VAR;
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

        if (context_p->token.type != LEXER_LITERAL || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
        {
          return SCAN_KEEP_TOKEN;
        }

        location_p = scanner_add_literal (context_p, scanner_context_p);
        location_p->type |= SCANNER_LITERAL_IS_VAR;
        scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
        return SCAN_NEXT_TOKEN;
      }

      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;

      if (context_p->token.type == LEXER_MULTIPLY)
      {
        lexer_next_token (context_p);
        if (!lexer_token_is_identifier (context_p, "from", 4))
        {
          scanner_raise_error (context_p);
        }

        lexer_next_token (context_p);

        if (context_p->token.type != LEXER_LITERAL
            && context_p->token.lit_location.type == LEXER_STRING_LITERAL)
        {
          scanner_raise_error (context_p);
        }

        return SCAN_NEXT_TOKEN;
      }

      if (context_p->token.type == LEXER_LEFT_BRACE)
      {
        lexer_next_token (context_p);

        while (context_p->token.type != LEXER_RIGHT_BRACE)
        {
          if (context_p->token.type != LEXER_LITERAL
              || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
          {
            scanner_raise_error (context_p);
          }

          lexer_next_token (context_p);

          if (lexer_token_is_identifier (context_p, "as", 2))
          {
            lexer_next_token (context_p);

            if (context_p->token.type != LEXER_LITERAL
                && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
            {
              scanner_raise_error (context_p);
            }

            lexer_next_token (context_p);
          }

          if (context_p->token.type != LEXER_RIGHT_BRACE)
          {
            if (context_p->token.type != LEXER_COMMA)
            {
              scanner_raise_error (context_p);
            }

            lexer_next_token (context_p);
          }
        }

        lexer_next_token (context_p);

        if (!lexer_token_is_identifier (context_p, "from", 4))
        {
          return SCAN_KEEP_TOKEN;
        }

        lexer_next_token (context_p);

        if (context_p->token.type != LEXER_LITERAL
            && context_p->token.lit_location.type == LEXER_STRING_LITERAL)
        {
          scanner_raise_error (context_p);
        }

        return SCAN_NEXT_TOKEN;
      }

      switch (context_p->token.type)
      {
#if ENABLED (JERRY_ES2015)
        case LEXER_KEYW_CLASS:
        case LEXER_KEYW_LET:
        case LEXER_KEYW_CONST:
#endif /* ENABLED (JERRY_ES2015) */
        case LEXER_KEYW_VAR:
        {
          scanner_context_p->active_literal_pool_p->status_flags |= SCANNER_LITERAL_POOL_IN_EXPORT;
          break;
        }
      }

      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      return SCAN_KEEP_TOKEN;
    }
#endif /* ENABLED (JERRY_ES2015_MODULE_SYSTEM) */
    default:
    {
      break;
    }
  }

  scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

  if (type == LEXER_LITERAL
      && context_p->token.lit_location.type == LEXER_IDENT_LITERAL)
  {
    if (JERRY_UNLIKELY (lexer_check_next_character (context_p, LIT_CHAR_COLON)))
    {
      lexer_consume_next_character (context_p);
      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      return SCAN_NEXT_TOKEN;
    }

    JERRY_ASSERT (context_p->token.flags & LEXER_NO_SKIP_SPACES);

#if ENABLED (JERRY_ES2015)
    /* The colon needs to be checked first because the parser also checks
     * it first, and this check skips the spaces which affects source_p. */
    if (JERRY_UNLIKELY (lexer_check_arrow (context_p)))
    {
      scanner_scan_simple_arrow (context_p, scanner_context_p, context_p->source_p);
      return SCAN_KEEP_TOKEN;
    }

    if (JERRY_UNLIKELY (lexer_token_is_let (context_p)))
    {
      lexer_lit_location_t let_literal = context_p->token.lit_location;
      const uint8_t *source_p = context_p->source_p;

      lexer_next_token (context_p);

      type = (lexer_token_type_t) context_p->token.type;

      if (type == LEXER_LEFT_SQUARE
          || type == LEXER_LEFT_BRACE
          || (type == LEXER_LITERAL && context_p->token.lit_location.type == LEXER_IDENT_LITERAL))
      {
        scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
        parser_stack_push_uint8 (context_p, SCAN_STACK_LET);
        return SCAN_KEEP_TOKEN;
      }

      scanner_info_t *info_p = scanner_insert_info (context_p, source_p, sizeof (scanner_info_t));
      info_p->type = SCANNER_TYPE_LET_EXPRESSION;

      lexer_lit_location_t *lit_location_p = scanner_add_custom_literal (context_p,
                                                                         scanner_context_p->active_literal_pool_p,
                                                                         &let_literal);
      lit_location_p->type |= SCANNER_LITERAL_IS_USED;

      if (scanner_context_p->active_literal_pool_p->status_flags & SCANNER_LITERAL_POOL_IN_WITH)
      {
        lit_location_p->type |= SCANNER_LITERAL_NO_REG;
      }

      scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
      return SCAN_KEEP_TOKEN;
    }

    if (JERRY_UNLIKELY (lexer_token_is_async (context_p)))
    {
      scanner_context_p->async_source_p = context_p->source_p;

      if (scanner_check_async_function (context_p, scanner_context_p))
      {
        scanner_context_p->mode = SCAN_MODE_STATEMENT;
      }
      return SCAN_KEEP_TOKEN;
    }
#endif /* ENABLED (JERRY_ES2015) */

    scanner_add_reference (context_p, scanner_context_p);

    scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
    return SCAN_NEXT_TOKEN;
  }

  return SCAN_KEEP_TOKEN;
} /* scanner_scan_statement */