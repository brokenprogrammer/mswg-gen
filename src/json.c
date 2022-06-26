#include "json.h"

#include <stdlib.h>
#include <stdio.h>

void
_JsonIndent(json_context *Context)
{
    for (int Index = 0; Index < Context->IndentationLevel; ++Index)
    {
        Context->At += sprintf(Context->Destination + Context->At, "    ");
    }
}


void
_JsonComma(json_context *Context)
{
    if (Context->ShouldPrintComma)
    {
        Context->At += sprintf(Context->Destination + Context->At, ",\n");
        Context->ShouldPrintComma = false;
    }
}

void
_JsonNewline(json_context *Context)
{
    Context->At += sprintf(Context->Destination + Context->At, "\n");
}

void
JsonFree(json_context *Context)
{
    free(Context->Destination);
}

void
JsonRootBegin(json_context *Context, size_t Size)
{
    Context->Destination = calloc(1, Size + 1);
    Context->DestinationMax = Size;
    Context->At = sprintf(Context->Destination, "{\n");
    Context->IndentationLevel = 1;
}

void
JsonRootEnd(json_context *Context)
{
    Context->IndentationLevel--;
    Context->At += sprintf(Context->Destination + Context->At, "\n}");
}

void
JsonObjectBegin(json_context *Context, char *Name)
{
    _JsonComma(Context);
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "\"%s\": {\n", Name);
    Context->IndentationLevel++;
}

void
JsonObjectEnd(json_context *Context)
{
    _JsonNewline(Context);
    Context->IndentationLevel--;
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "}");
    Context->ShouldPrintComma = true;
}

void
JsonArrayBegin(json_context *Context, char *Name)
{
    _JsonComma(Context);
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "\"%s\": [\n", Name);
    Context->IndentationLevel++;
}

void
JsonArrayEnd(json_context *Context)
{
    _JsonNewline(Context);
    Context->IndentationLevel--;
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "]");
    Context->ShouldPrintComma = true;
}

void
JsonString(json_context *Context, char *Name, char *Value)
{
    _JsonComma(Context);
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "\"%s\": \"%s\"", Name, Value);
    Context->ShouldPrintComma = true;
}

void
JsonNumber(json_context *Context, char *Name, int Value)
{
    _JsonComma(Context);
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "\"%s\": %d", Name, Value);
    Context->ShouldPrintComma = true;
}


void
JsonUnnamedString(json_context *Context, char *Value)
{
    _JsonComma(Context);
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "\"%s\"", Value);
    Context->ShouldPrintComma = true;
}

void
JsonUnnamedNumber(json_context *Context, int Value)
{
    _JsonComma(Context);
    _JsonIndent(Context);
    Context->At += sprintf(Context->Destination + Context->At, "%d", Value);
    Context->ShouldPrintComma = true;
}
