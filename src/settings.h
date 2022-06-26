#ifndef SETTINGS_H
#define SETTINGS_H

#include <stddef.h>

#include "token.h"

typedef struct
{
    char *Key;
    size_t KeyLength;

    char *Value;
    size_t ValueLength;
} key_value_pair;

typedef struct
{
    char *Title;
    char *Description;
    char *TermsOfService;
    
    char *ContactName;
    char *ContactURL;
    char *ContactEmail;
    
    char *LicenseName;
    char *LicenseURL;
    
    char *Version;
} settings;

char * ParseSettingsSection(tokenizer *Tokenizer);
key_value_pair ParseSettingsKeyValuePair(tokenizer *Tokenizer, settings *Settings);
void FreeSettingsKeyValuePair(key_value_pair *Pair);
settings ReadAndParseSettingsFile(char *FileName);
void FreeSettings(settings *Settings);

#endif // SETTINGS_H