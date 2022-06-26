#ifndef TOKEN_H
#define TOKEN_H

#include "common.h"

typedef enum
{
    Token_Unknown,

    Token_OpenParen,
    Token_CloseParen,
    Token_Colon,
    Token_Semicolon,
    Token_Asterisk,
    Token_OpenBracket,
    Token_CloseBracket,
    Token_OpenBrace,
    Token_CloseBrace,
    Token_Equals,
    Token_Comma,
    Token_Or,
    Token_Pound,
    Token_LessThan,
    Token_GreaterThan,
    Token_QuestionMark,
    Token_Dot,
    Token_At,
    Token_Percent,
    Token_Dollar,
    Token_Quote,
    Token_Plus,

    Token_String,
    Token_Identifier,
    Token_Number,

    Token_Space,
    Token_EndOfLine,
    Token_Comment,

    Token_EndOfStream,
} token_type;

typedef struct
{
    token_type Type;

    size_t TextLength;
    char *Text;

    float F32;
    int I32;
} token;

typedef struct
{
    char *Input;
    unsigned int InputLength;

    int LineNumber;
    int ColumnNumber;

    char At[2];
} tokenizer;

void EatAllWhitespace(tokenizer *Tokenizer);
void EatAllWhitespaceAndNewLine(tokenizer *Tokenizer);
token GetToken(tokenizer *Tokenizer);
token PeekToken(tokenizer *Tokenizer);
bool ExpectTokenType(tokenizer *Tokenizer, token_type Type);
bool ExpectTokenTypeAndContent(tokenizer *Tokenizer, token_type Type, char *Content);
char *AllocateStringFromToken(token *Token);
tokenizer Tokenize(char *Input, unsigned int InputLength);

#endif // TOKEN_H