#ifndef JSON_H
#define JSON_H

#include <stddef.h>
#include "common.h"

typedef struct
{
    char *Destination;
    size_t DestinationMax;

    size_t At;

    int IndentationLevel;
    bool ShouldPrintComma;
} json_context;

void JsonFree(json_context *Context);
void JsonRootBegin(json_context *Context, size_t Size);
void JsonRootEnd(json_context *Context);
void JsonObjectBegin(json_context *Context, char *Name);
void JsonObjectEnd(json_context *Context);
void JsonArrayBegin(json_context *Context, char *Name);
void JsonArrayEnd(json_context *Context);
void JsonString(json_context *Context, char *Name, char *Value);
void JsonNumber(json_context *Context, char *Name, int Value);
void JsonUnnamedString(json_context *Context, char *Value);
void JsonUnnamedNumber(json_context *Context, int Value);


#define JSON_DEFERLOOP(Begin, End) for(int _i_ = ((Begin), 0); !_i_; _i_ += 1, (End))
#define JsonRoot(Context, Size) JSON_DEFERLOOP(JsonRootBegin(Context, Size), JsonRootEnd(Context))
#define JsonObject(Context, Name) JSON_DEFERLOOP(JsonObjectBegin(Context, Name), JsonObjectEnd(Context))
#define JsonArray(Context, Name) JSON_DEFERLOOP(JsonArrayBegin(Context, Name), JsonArrayEnd(Context))

#endif // JSON_H