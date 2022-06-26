#include "mswg_parser.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char *IntegerTypeString = "integer";
//static char *FloatingTypeString = "number";
//static char *StringTypeString = "string";


// TODO(Oskar): More type complete
char **
ConvertCType(token *TypeToken)
{
    if (strncmp(TypeToken->Text, "int8_t", 6) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "int16_t", 7) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "int32_t", 7) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "int64_t", 7) == 0)
    {
        return &IntegerTypeString;
    }

    if (strncmp(TypeToken->Text, "uint8_t", 7) == 0)
    {
        return &IntegerTypeString;        
    }
    if (strncmp(TypeToken->Text, "uint16_t", 8) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "uint32_t", 8) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "uint64_t", 8) == 0)
    {
        return &IntegerTypeString;
    }

    if (strncmp(TypeToken->Text, "int", 3) == 0)
    {
        return &IntegerTypeString;        
    }
    if (strncmp(TypeToken->Text, "long", 4) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "float", 4) == 0)
    {
        return &IntegerTypeString;
    }
    if (strncmp(TypeToken->Text, "double", 6) == 0)
    {
        return &IntegerTypeString;
    }

    return &IntegerTypeString;
}

c_type
ParseCType(tokenizer *Tokenizer)
{
    c_type Result = { 0 };
    Result.IsArray = false;

    token TypeToken = GetToken(Tokenizer);
    assert(TypeToken.Type == Token_Identifier);

    Result.Type = *ConvertCType(&TypeToken);

    EatAllWhitespace(Tokenizer);
    
    token NameToken = GetToken(Tokenizer);
    assert(NameToken.Type == Token_Identifier);
    
    Result.Name = AllocateStringFromToken(&NameToken);

    EatAllWhitespace(Tokenizer);

    token Token = PeekToken(Tokenizer);
    while (Token.Type != Token_Semicolon)
    {
        Token = GetToken(Tokenizer);

        if (Token.Type == Token_OpenBrace)
        {
            Result.IsArray = true;
        }

        if (Token.Type == Token_Asterisk)
        {
            Result.IsArray = true;
        }
    }
    GetToken(Tokenizer); // ;

    return Result;
}

c_type
ParseGoType(tokenizer *Tokenizer)
{
    c_type Result = { 0 };
    Result.IsArray = false;

    token NameToken = GetToken(Tokenizer);
    assert(NameToken.Type == Token_Identifier);    
    Result.Name = AllocateStringFromToken(&NameToken);

    EatAllWhitespace(Tokenizer);

    token TypeToken = GetToken(Tokenizer);
    assert(TypeToken.Type == Token_Identifier);
    Result.Type = *ConvertCType(&TypeToken);    

    EatAllWhitespace(Tokenizer);

    token Token = PeekToken(Tokenizer);
    while (Token.Type != Token_EndOfLine)
    {
        Token = GetToken(Tokenizer);

        if (Token.Type == Token_OpenBrace)
        {
            Result.IsArray = true;
        }
    }
    GetToken(Tokenizer); // \n

    return Result;
}

void
FreeCType(c_type *CType)
{
    assert(CType->Name);
    free(CType->Name);
}

void 
FreeRoute(route *Route)
{
    free(Route->Name);
    free(Route->Method);
    free(Route->ReturnType);
}

void 
FreeRouteType(route_type *RouteType)
{
    free(RouteType->Name);

    for (int TypeIndex = 0; TypeIndex < RouteType->NumberOfTypes; ++TypeIndex)
    {
        c_type Type = RouteType->Types[TypeIndex];
        FreeCType(&Type);
    }
}

bool
ParseCTypedefRouteType(tokenizer *Tokenizer, route_type *RouteType)
{
    // NOTE(Oskar): Comes in after typedef struct
    EatAllWhitespace(Tokenizer);
    
    if (!ExpectTokenType(Tokenizer, Token_OpenBracket))
    {
        // Syntaxt error
        printf("Error Openbrace\n");
        return false;
    }

    EatAllWhitespaceAndNewLine(Tokenizer);

    token PeekingToken = PeekToken(Tokenizer);
    while (PeekingToken.Type != Token_CloseBracket)
    {
        EatAllWhitespaceAndNewLine(Tokenizer);
        c_type Type = ParseCType(Tokenizer);
        
        RouteType->Types[RouteType->NumberOfTypes++] = Type;

        PeekingToken = PeekToken(Tokenizer);
    }
    GetToken(Tokenizer); // }
    
    EatAllWhitespaceAndNewLine(Tokenizer);
    token TypeName = GetToken(Tokenizer);
    assert(TypeName.Type == Token_Identifier);

    RouteType->Name = malloc(TypeName.TextLength + 1);
    memcpy(RouteType->Name, TypeName.Text, TypeName.TextLength);
    RouteType->Name[TypeName.TextLength] = 0;
    RouteType->NameLength = TypeName.TextLength;

    if (!ExpectTokenType(Tokenizer, Token_Semicolon))
    {
        // Syntaxt error
        printf("Error semicolon after typename\n");
        return false;
    }

    return true;
}

bool
ParseGolangRouteType(tokenizer *Tokenizer, route_type *RouteType)
{
    // NOTE(Oskar): Comes in after type struct
    EatAllWhitespace(Tokenizer);

    token TypeName = GetToken(Tokenizer);
    if (TypeName.Type != Token_Identifier)
    {
        // Syntax error
        printf("Error missing identifier\n");
        return false;
    }

    EatAllWhitespace(Tokenizer);

    if (!ExpectTokenTypeAndContent(Tokenizer, Token_Identifier, "struct"))
    {
        // Syntax error
        printf("Error go struct\n");
        return false;
    }

    EatAllWhitespace(Tokenizer);

    if (!ExpectTokenType(Tokenizer, Token_OpenBracket))
    {
        // Syntaxt error
        printf("Error go Openbrace\n");
        return false;
    }

    EatAllWhitespaceAndNewLine(Tokenizer);

    token PeekingToken = PeekToken(Tokenizer);
    while (PeekingToken.Type != Token_CloseBracket)
    {
        EatAllWhitespaceAndNewLine(Tokenizer);
        c_type Type = ParseGoType(Tokenizer);
        
        RouteType->Types[RouteType->NumberOfTypes++] = Type;

        PeekingToken = PeekToken(Tokenizer);
    }
    GetToken(Tokenizer); // }

    RouteType->Name = malloc(TypeName.TextLength + 1);
    memcpy(RouteType->Name, TypeName.Text, TypeName.TextLength);
    RouteType->Name[TypeName.TextLength] = 0;
    RouteType->NameLength = TypeName.TextLength;

    return true;
}

void
ParseRouting(void *Argument)
{
    route_parse_argument *Data = (route_parse_argument *)Argument;

    read_file_result FileContents = ReadEntireFileIntoMemoryAndNullTerminate(Data->FilePath);
    tokenizer Tokenizer = Tokenize(FileContents.Data, FileContents.Length);

    bool Parsing = true;
    while(Parsing)
    {
        token Token = GetToken(&Tokenizer);
        
        switch(Token.Type)
        {
            case Token_Comment:
            {
                char *Scan = Token.Text;
                for (int Index = 0; *Scan && Index < Token.TextLength; ++Scan, ++Index)
                {
                    if (*Scan == '@')
                    {
                        if (strncmp(Scan, "@RouteType", 10) == 0)
                        {
                            route_type *RouteType = Data->RouteTypes + Data->NumberOfRouteTypes;           
                            EatAllWhitespaceAndNewLine(&Tokenizer);
                            Token = GetToken(&Tokenizer);

                            // NOTE(Oskar): Also accepts "type" which golang uses.
                            if (strncmp(Token.Text, "typedef", Token.TextLength) == 0)
                            {
                                printf("Expected typedef toke: %.*s\n", (int)Token.TextLength, Token.Text);
                                Token = GetToken(&Tokenizer);

                                // Assume C
                                if (ExpectTokenTypeAndContent(&Tokenizer, Token_Identifier, "struct"))
                                {
                                    if (ParseCTypedefRouteType(&Tokenizer, RouteType))
                                    {
                                        Data->NumberOfRouteTypes++;
                                    }
                                }
                                else
                                {
                                    if (ParseGolangRouteType(&Tokenizer, RouteType))
                                    {
                                        Data->NumberOfRouteTypes++;
                                    }
                                    else
                                    {
                                        // Syntax error
                                        printf("Error struct\n");
                                        printf("ERROR AT: %.*s\n", (int)Token.TextLength, Token.Text);
                                        printf("Reading file: %s\n", Data->FilePath);
                                        // Parsing = false;
                                        continue;
                                    }
                                }
                            }
                            else 
                            {
                                printf("Found unsupported struct format\n");
                            }
                        }
                        else if (strncmp(Scan, "@Route", 6) == 0)
                        {
                            route *Route = &Data->Routes[Data->NumberOfRoutes];

                            Scan += 6; // @Route

                            if (*Scan != '(')
                            {
                                // Syntax error
                                break;
                            }
                            Scan++;

                            char *RouteStart = Scan;
                            for (; *Scan != ','; ++Scan)
                            {
                            }

                            int RouteSize = Scan - RouteStart;
                            Route->Name = AllocateStringFromRange(RouteStart, RouteSize);

                            if (*Scan != ',')
                            {
                                // Syntax error
                                free(Route->Name);
                                break;
                            }
                            Scan++;
                            
                            for (; *Scan == ' ' && *Scan != ')'; ++Scan)
                            {
                            }

                            char *MethodStart = Scan;
                            for (; *Scan != ',' && *Scan != ')'; ++Scan)
                            {
                            }
                            int MethodSize = Scan - MethodStart;
                            Route->Method = AllocateStringFromRange(MethodStart, MethodSize);
                            if (*Scan == ')')
                            {
                                // NOTE(Oskar): No return type
                                free(Route->Name);
                                free(Route->Method);
                                break;
                            }

                            if (*Scan != ',')
                            {
                                // Syntax error
                                printf("ERROR   , \n");
                                break;
                            }
                            Scan++;

                            for (; *Scan == ' ' && *Scan != ')'; ++Scan)
                            {
                            }

                            char *ReturnTypeStart = Scan;
                            for (; *Scan != ')'; ++Scan)
                            {
                            }
                            int ReturnTypeSize = Scan - ReturnTypeStart;
                            Route->ReturnType = AllocateStringFromRange(ReturnTypeStart, ReturnTypeSize);

                            if (*Scan != ')')
                            {
                                // Syntax error
                                printf("ERROR   ) \n");
                                break;
                            }

                            Data->NumberOfRoutes++;
                        }
                    }
                }
            } break;

            case Token_EndOfStream:
            {
                Parsing = false;
            } break;
        }
    }

    free(FileContents.Data);
}