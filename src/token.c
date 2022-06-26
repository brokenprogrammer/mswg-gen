#include "token.h"

#include <x86intrin.h>
#include <string.h>

int
RoundReal32ToInt32(float Real32)
{
    int Result = _mm_cvtss_si32(_mm_set_ss(Real32));
    return(Result);
}

bool
IsEndOfLine(char C)
{
    bool Result = ((C == '\n') ||
                   (C == '\r'));

    return(Result);
}

bool
IsWhitespace(char C)
{
    bool Result = ((C == ' ') ||
                   (C == '\t') ||
                   (C == '\v') ||
                   (C == '\f'));

    return(Result);
}

bool
IsAlpha(char C)
{
    bool Result = (((C >= 'a') && (C <= 'z')) ||
                   ((C >= 'A') && (C <= 'Z')));

    return(Result);
}

bool
IsNumber(char C)
{
    bool Result = ((C >= '0') && (C <= '9'));

    return(Result);
}

bool
TokenEquals(token Token, char *Match)
{
    char *At = Match;
    for(int Index = 0;
        Index < Token.TextLength;
        ++Index, ++At)
    {
        if((*At == 0) ||
           (Token.Text[Index] != *At))
        {
            return(false);
        }

    }

    bool Result = (*At == 0);
    return(Result);
}

void
Fill(tokenizer *Tokenizer)
{
    if (Tokenizer->InputLength == 0)
    {
        Tokenizer->At[0] = 0;
        Tokenizer->At[1] = 0;
    }
    else if (Tokenizer->InputLength == 1)
    {
        Tokenizer->At[0] = Tokenizer->Input[0];
        Tokenizer->At[1] = 0;
    }
    else
    {
        Tokenizer->At[0] = Tokenizer->Input[0];
        Tokenizer->At[1] = Tokenizer->Input[1];
    }
}

char *
MoveBuffer(char **Buffer, unsigned int *BufferLength, unsigned int Length)
{
    char *Result = 0;
    
    if(*BufferLength >= Length)
    {
        Result = *Buffer;
        *Buffer += Length;
        *BufferLength -= Length;
    }
    else
    {
        *Buffer += *BufferLength;
        *BufferLength = 0;
    }

    return Result;
}

void
MoveForward(tokenizer *Tokenizer, unsigned int Length)
{
    Tokenizer->ColumnNumber += Length;
    MoveBuffer(&Tokenizer->Input, &Tokenizer->InputLength, Length);    
    Fill(Tokenizer);
}

void
EatAllWhitespace(tokenizer *Tokenizer)
{
    for(;;)
    {
        // TODO(Oskar): Add flags to also skip comments etc.
        if(IsWhitespace(Tokenizer->At[0]))
        {
            MoveForward(Tokenizer, 1);
        }
        // else if((Tokenizer->At[0] == '/') &&
        //         (Tokenizer->At[1] == '/'))
        // {
        //     Tokenizer->At += 2;
        //     while(Tokenizer->At[0] && !IsEndOfLine(Tokenizer->At[0]))
        //     {
        //         ++Tokenizer->At;
        //     }
        // }
        // else if((Tokenizer->At[0] == '/') &&
        //         (Tokenizer->At[1] == '*'))
        // {
        //     Tokenizer->At += 2;
        //     while(Tokenizer->At[0] &&
        //           !((Tokenizer->At[0] == '*') &&
        //             (Tokenizer->At[1] == '/')))
        //     {
        //         ++Tokenizer->At;
        //     }

        //     if(Tokenizer->At[0] == '*')
        //     {
        //         Tokenizer->At += 2;
        //     }
        // }
        else
        {
            break;
        }
    }
}

void
EatAllWhitespaceAndNewLine(tokenizer *Tokenizer)
{
    for(;;)
    {
        // TODO(Oskar): Add flags to also skip comments etc.
        if(IsWhitespace(Tokenizer->At[0]))
        {
            MoveForward(Tokenizer, 1);
        }
        else if (IsEndOfLine(Tokenizer->At[0]))
        {
            MoveForward(Tokenizer, 1);
        }
        else
        {
            break;
        }
    }
}

token
GetToken(tokenizer *Tokenizer)
{
    token Token = {0};

    Token.Text = Tokenizer->Input;

    char C = Tokenizer->At[0];
    MoveForward(Tokenizer, 1);

    switch (C)
    {
        case '\0': {Token.Type = Token_EndOfStream;} break;
        case '(':  {Token.Type = Token_OpenParen;} break;
        case ')':  {Token.Type = Token_CloseParen;} break;
        case ':':  {Token.Type = Token_Colon;} break;
        case ';':  {Token.Type = Token_Semicolon;} break;
        case '*':  {Token.Type = Token_Asterisk;} break;
        case '[':  {Token.Type = Token_OpenBrace;} break;
        case ']':  {Token.Type = Token_CloseBrace;} break;
        case '{':  {Token.Type = Token_OpenBracket;} break;
        case '}':  {Token.Type = Token_CloseBracket;} break;
        case '=':  {Token.Type = Token_Equals;} break;
        case ',':  {Token.Type = Token_Comma;} break;
        case '|':  {Token.Type = Token_Or;} break;
        case '#':  {Token.Type = Token_Pound;} break;
        case '<':  {Token.Type = Token_LessThan;} break;
        case '>':  {Token.Type = Token_GreaterThan;} break;
        case '?':  {Token.Type = Token_QuestionMark;} break;
        case '.':  {Token.Type = Token_Dot;} break;
        case '@':  {Token.Type = Token_At;} break;
        case '%':  {Token.Type = Token_Percent; } break;
        case '$':  {Token.Type = Token_Dollar; } break;
        case '+':  {Token.Type = Token_Plus; } break;

        case '"':
        {
            Token.Type = Token_String;

            while (Tokenizer->At[0] &&
                   Tokenizer->At[0] != '"')
            {
                if ((Tokenizer->At[0] == '\\') && Tokenizer->At[1])
                {
                    MoveForward(Tokenizer, 1);
                }
                MoveForward(Tokenizer, 1);
            }

            if (Tokenizer->At[0] == '"')
            {
                MoveForward(Tokenizer, 1);
            }
        } break;

        default:
        {
            if (IsWhitespace(C))
            {
                Token.Type = Token_Space;
                while (IsWhitespace(Tokenizer->At[0]))
                {
                    MoveForward(Tokenizer, 1);
                }
            }
            else if (IsEndOfLine(C))
            {
                Token.Type = Token_EndOfLine;
                
                if (((C == '\r') &&
                    (Tokenizer->At[0] == '\n')) ||
                   ((C == '\n') &&
                    (Tokenizer->At[0] == '\r')))
                {
                    MoveForward(Tokenizer, 1);
                }

                Tokenizer->ColumnNumber = 1;
                ++Tokenizer->LineNumber;
            }
            else if ((C == '/') &&
                    (Tokenizer->At[0] == '/'))
            {
                Token.Type = Token_Comment;

                MoveForward(Tokenizer, 2);
                while (Tokenizer->At[0] && !IsEndOfLine(Tokenizer->At[0]))
                {
                    MoveForward(Tokenizer, 1);
                }
            }
            else if ((C == '/') &&
                    (Tokenizer->At[0] == '*'))
            {
                Token.Type = Token_Comment;

                MoveForward(Tokenizer, 2);
                while (Tokenizer->At[0] &&
                      !((Tokenizer->At[0] == '*') &&
                        (Tokenizer->At[1] == '/')))
                {
                    if (((Tokenizer->At[0] == '\r') &&
                        (Tokenizer->At[1] == '\n')) ||
                       ((Tokenizer->At[0] == '\n') &&
                        (Tokenizer->At[1] == '\r')))
                    {
                        MoveForward(Tokenizer, 1);
                    }

                    if (IsEndOfLine(Tokenizer->At[0]))
                    {
                        ++Tokenizer->LineNumber;
                    }

                    MoveForward(Tokenizer, 1);
                }

                if(Tokenizer->At[0] == '*')
                {
                    MoveForward(Tokenizer, 2);
                }
            }
            else if (IsAlpha(C) || (C == '.') || (C == '/') || (C == '%') || (C == '\\') || (C == '$'))
            {
                Token.Type = Token_Identifier;

                while (IsAlpha(Tokenizer->At[0]) ||
                        IsNumber(Tokenizer->At[0]) ||
                        (Tokenizer->At[0] == '_') ||
                        (Tokenizer->At[0] == '.') ||
                        (Tokenizer->At[0] == '/') ||
                        (Tokenizer->At[0] == '-') ||
                        (Tokenizer->At[0] == '+'))
                {
                    MoveForward(Tokenizer, 1);
                }
            }
            else if (IsNumber(C))
            {
                float Number = (float)(C - '0');

                while (IsNumber(Tokenizer->At[0]))
                {
                    float Digit = (float)(Tokenizer->At[0] - '0');
                    Number = 10.0f*Number + Digit;
                    MoveForward(Tokenizer, 1);
                }

                if (Tokenizer->At[0] == '.')
                {
                    MoveForward(Tokenizer, 1);
                    float Coefficient = 0.1f;
                    while (IsNumber(Tokenizer->At[0] - '0'))
                    {
                        float Digit = (float)(Tokenizer->At[0] - '0');
                        Number += Coefficient*Digit;
                        Coefficient *= 0.1f;
                        MoveForward(Tokenizer, 1);
                    }
                }

                Token.Type = Token_Number;
                Token.F32 = Number;
                Token.I32 = RoundReal32ToInt32(Number);
            }
            else
            {
                Token.Type = Token_Unknown;
            }
        } break;
    }

    Token.TextLength = (Tokenizer->Input - Token.Text);

    return Token;
}

token
PeekToken(tokenizer *Tokenizer)
{
    tokenizer T = *Tokenizer;
    token Result = GetToken(&T);
    return Result;
}

bool
ExpectTokenType(tokenizer *Tokenizer, token_type Type)
{
    token Token = PeekToken(Tokenizer);
    if (Token.Type == Type)
    {
        GetToken(Tokenizer);
        return true;
    }

    return false;
}

bool
ExpectTokenTypeAndContent(tokenizer *Tokenizer, token_type Type, char *Content)
{
    token Token = PeekToken(Tokenizer);
    if (Token.Type == Type)
    {
        if (strncmp(Token.Text, Content, Token.TextLength) == 0)
        {
            GetToken(Tokenizer);
            return true;
        }
    }

    return false;
}

char *
AllocateStringFromToken(token *Token)
{
    char *Result = 0;

    Result = malloc(Token->TextLength + 1);
    memcpy(Result, Token->Text, Token->TextLength);
    Result[Token->TextLength] = 0;

    return Result;
}

tokenizer
Tokenize(char *Input, unsigned int InputLength)
{
    tokenizer Tokenizer = {0};

    Tokenizer.Input = Input;
    Tokenizer.InputLength = InputLength;
    Tokenizer.ColumnNumber = 1;
    Tokenizer.LineNumber = 1;

    Fill(&Tokenizer);

    return Tokenizer;
}