#include "settings.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

char *
ParseSettingsSection(tokenizer *Tokenizer)
{
    if (!ExpectTokenType(Tokenizer, Token_OpenBrace))
    {
        // Syntax error
    }

    token StartToken = GetToken(Tokenizer);
    token EndToken = GetToken(Tokenizer);
    while (EndToken.Type != Token_CloseBrace)
    {               
        if (EndToken.Type == Token_EndOfStream || 
            EndToken.Type == Token_Pound ||
            EndToken.Type == Token_Semicolon)
        {
            // Syntax error
        }

        EndToken = GetToken(Tokenizer);
    }
    
    if (EndToken.Type == Token_CloseBrace)
    {
        GetToken(Tokenizer);
    }

    int Length = (int)(EndToken.Text - StartToken.Text);
    char *Result = malloc(Length + 1);
    memcpy(Result, StartToken.Text, Length);
    Result[Length] = 0;

    return Result; 
}

key_value_pair
ParseSettingsKeyValuePair(tokenizer *Tokenizer, settings *Settings)
{
    key_value_pair Result = { 0 };
    token Identifier = GetToken(Tokenizer);

    Result.Key = malloc(Identifier.TextLength + 1);
    memcpy(Result.Key, Identifier.Text, Identifier.TextLength);
    Result.Key[Identifier.TextLength] = 0;
    Result.KeyLength = Identifier.TextLength;

    EatAllWhitespace(Tokenizer);
    if (!ExpectTokenType(Tokenizer, Token_Equals))
    {
        // Syntax error 
    }
    EatAllWhitespace(Tokenizer);

    // TODO(Oskar): Ini file format should support values with spaces etc also
    token Value = GetToken(Tokenizer);
    Result.Value = malloc(Value.TextLength + 1);
    memcpy(Result.Value, Value.Text, Value.TextLength);
    Result.Value[Value.TextLength] = 0;
    Result.ValueLength = Value.TextLength;

    return Result;
}

void
FreeSettingsKeyValuePair(key_value_pair *Pair)
{
    assert(Pair->Key && Pair->Value);

    free(Pair->Key);
    free(Pair->Value);
}

settings
ReadAndParseSettingsFile(char *FileName)
{
    settings Settings = {0};

    read_file_result FileContents = ReadEntireFileIntoMemoryAndNullTerminate(FileName);
    tokenizer Tokenizer = Tokenize(FileContents.Data, FileContents.Length);

    bool Parsing = true;
    char *CurrentSection = 0;

    while(Parsing)
    {
        token Token = PeekToken(&Tokenizer);
        
        switch(Token.Type)
        {
            case Token_OpenBrace:
            {
                if (CurrentSection)
                {
                    free(CurrentSection);
                }

                CurrentSection = ParseSettingsSection(&Tokenizer);
            } break;
            case Token_Identifier:
            {
                key_value_pair Pair = ParseSettingsKeyValuePair(&Tokenizer, &Settings);
                
                // Populate settings object
                //printf("%s = %s\n", Pair.Key, Pair.Value);

                if (strcmp(CurrentSection, "mswgen") == 0)
                {
                    if (strcmp(Pair.Key, "title") == 0)
                    {
                        Settings.Title = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                    else if (strcmp(Pair.Key, "description") == 0)
                    {
                        Settings.Description = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                    else if (strcmp(Pair.Key, "termsofservice") == 0)
                    {
                        Settings.TermsOfService = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                }
                else if (strcmp(CurrentSection, "mswgen.contact") == 0)
                {
                    if (strcmp(Pair.Key, "name") == 0)
                    {
                        Settings.ContactName = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                    else if (strcmp(Pair.Key, "url") == 0)
                    {
                        Settings.ContactURL = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                    else if (strcmp(Pair.Key, "email") == 0)
                    {
                        Settings.ContactEmail = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                }
                else if (strcmp(CurrentSection, "mswgen.license") == 0)
                {
                    if (strcmp(Pair.Key, "name") == 0)
                    {
                        Settings.LicenseName = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                    else if (strcmp(Pair.Key, "url") == 0)
                    {
                        Settings.LicenseURL = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                }
                else if (strcmp(CurrentSection, "mswgen.version") == 0)
                {
                    if (strcmp(Pair.Key, "version") == 0)
                    {
                        Settings.Version = AllocateStringCopy(Pair.Value, Pair.ValueLength);
                    }
                }

                FreeSettingsKeyValuePair(&Pair);
            } break;

            case Token_Pound:
            case Token_Semicolon:
            {
                // Comment, go to next line
                while(Token.Type != Token_EndOfLine)
                {
                    //printf("Line: %.*s %d\n", (int)Token.TextLength, Token.Text, Token.Type);
                    if (Token.Type == Token_EndOfStream)
                    {
                        Parsing = false;
                        break;
                    }

                    Token = GetToken(&Tokenizer);
                }
            } break;

            case Token_EndOfStream:
            {
                Parsing = false;
            } break;

            default:
            {
                // Eat unexpected token.
                GetToken(&Tokenizer);
            } break;
        }
    }    

    if (CurrentSection)
    {
        free(CurrentSection);
    }

    free(FileContents.Data);

    return Settings;
}

void
FreeSettings(settings *Settings)
{    
    free(Settings->Title);
    free(Settings->Description);
    free(Settings->TermsOfService);
    free(Settings->ContactName);
    free(Settings->ContactURL);
    free(Settings->ContactEmail);
    free(Settings->LicenseName);
    free(Settings->LicenseURL);
    free(Settings->Version);
}