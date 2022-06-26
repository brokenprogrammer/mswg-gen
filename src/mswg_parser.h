#ifndef MSWG_PARSER_H
#define MSWG_PARSER_H

#include <stddef.h>
#include <stdint.h>

#include "common.h"
#include "token.h"

typedef struct
{
    char *Type; // NOTE(Oskar): Doesn't have to be freed.
    char *Name;

    bool IsArray;
} c_type;

typedef struct
{
    char *Name;
    size_t NameLength;

    c_type Types[128];
    uint32_t NumberOfTypes;
} route_type;

typedef struct
{
    char *Name;
    char *Method;
    char *ReturnType;
} route;

typedef struct
{
    char *FilePath;

    // TODO(Oskar): Dynamically allocate theese.
    route_type RouteTypes[50];
    uint32_t NumberOfRouteTypes;

    route Routes[50];
    uint32_t NumberOfRoutes;
} route_parse_argument;

void ParseRouting(void *Argument);

void FreeCType(c_type *CType);
void FreeRoute(route *Route);
void FreeRouteType(route_type *RouteType);

#endif // MSWG_PARSER_H