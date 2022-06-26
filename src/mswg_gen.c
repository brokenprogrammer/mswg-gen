#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#include "common.h"
#include "json.h"
#include "settings.h"
#include "mswg_parser.h"
#include "threadpool.h"
#include "file.h"

// Things left todo:
// look over error handling
int 
main(int Argc, char ** Argv)
{
    char WorkingPath[4096];

    // NOTE(Oskar): Get current working directory.
    errno = 0;
    if (getcwd(WorkingPath, 4096) == NULL)
    {
        if (errno == ERANGE)
        {
            perror("Failed to get WorkingPath due to path exceeding buffer size limit.");
        }
        else
        {
            perror("Failed to get WorkingPath");
        }
        exit(EXIT_FAILURE);
    }

    settings Settings = ReadAndParseSettingsFile("config.ini");

    // NOTE(Oskar): Scan for files
    file *FirstFile = ListFiles(WorkingPath);

    threadpool *Pool = ThreadpoolCreate(4, 128);
    
    route_parse_argument *ParsingArguments = malloc(sizeof(route_parse_argument) * 128);
    uint32_t ArgumentIndex = 0;

    file *IteratorFile = FirstFile;
    while (IteratorFile != NULL)
    {
        route_parse_argument *ParsingArgument =  ParsingArguments + ArgumentIndex;
        ParsingArgument->FilePath = IteratorFile->Path;

        ThreadpoolAddWork(Pool, ParseRouting, ParsingArgument);
        IteratorFile = IteratorFile->Next;

        ArgumentIndex++;
    }
    
    ThreadpoolWait(Pool);

    json_context JsonContext = { 0 };
    JsonRoot(&JsonContext, 65335)
    {
        JsonString(&JsonContext, "openapi", "3.0.0");
        JsonObject(&JsonContext, "info")
        {
            JsonString(&JsonContext, "title", Settings.Title);
            JsonString(&JsonContext, "description", Settings.Description);
            JsonString(&JsonContext, "version", Settings.Version);
        }
        JsonObject(&JsonContext, "paths")
        {
            for (uint32_t ParsingIndex = 0; ParsingIndex < ArgumentIndex; ++ParsingIndex)
            {
                route_parse_argument *ParsingArgument =  ParsingArguments + ParsingIndex;
                for (int Index = 0; Index < ParsingArgument->NumberOfRoutes; ++Index)
                {
                    route Route = ParsingArgument->Routes[Index];
                    JsonObject(&JsonContext, Route.Name)
                    {
                        JsonObject(&JsonContext, Route.Method)
                        {
                            JsonArray(&JsonContext, "tags")
                            {
                                JsonUnnamedString(&JsonContext, "Api");
                            }
                            JsonObject(&JsonContext, "responses")
                            {
                                JsonObject(&JsonContext, "200")
                                {
                                    JsonString(&JsonContext, "description", "Success");
                                    if (Route.ReturnType)
                                    {
                                        JsonObject(&JsonContext, "content")
                                        {
                                            JsonObject(&JsonContext, "application/json")
                                            {
                                                JsonObject(&JsonContext, "schema")
                                                {
                                                    char Buffer[128] = {0};
                                                    sprintf(Buffer, "#/components/schemas/%s", Route.ReturnType);
                                                    JsonString(&JsonContext, "$ref", Buffer);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        JsonObject(&JsonContext, "components")
        {
            JsonObject(&JsonContext, "schemas")
            {
                for (uint32_t ParsingIndex = 0; ParsingIndex < ArgumentIndex; ++ParsingIndex)
                {
                    route_parse_argument *ParsingArgument =  ParsingArguments + ParsingIndex;
                    for (int Index = 0; Index < ParsingArgument->NumberOfRouteTypes; ++Index)
                    {
                        route_type RouteType = ParsingArgument->RouteTypes[Index];
                        JsonObject(&JsonContext, RouteType.Name)
                        {
                            JsonString(&JsonContext, "type", "object");
                            JsonObject(&JsonContext, "properties")
                            {
                                for (int TypeIndex = 0; TypeIndex < RouteType.NumberOfTypes; ++TypeIndex)
                                {
                                    c_type Type = RouteType.Types[TypeIndex];
                                    JsonObject(&JsonContext, Type.Name)
                                    {
                                        JsonString(&JsonContext, "type", Type.Type);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    printf("%s", JsonContext.Destination);
    
    // NOTE(Oskar): All done now cleaning upp all allocations.
    JsonFree(&JsonContext);
    
    for (uint32_t Index = 0; Index < ArgumentIndex; ++Index)
    {
        route_parse_argument *ParsingArgument =  ParsingArguments + Index;
        for (uint32_t RouteIndex = 0; RouteIndex < ParsingArgument->NumberOfRoutes; ++RouteIndex)
        {
            route Route = ParsingArgument->Routes[RouteIndex];
            FreeRoute(&Route);
        }

        for (uint32_t RouteTypeIndex = 0; RouteTypeIndex < ParsingArgument->NumberOfRouteTypes; ++RouteTypeIndex)
        {
            route_type RouteType = ParsingArgument->RouteTypes[RouteTypeIndex];
            FreeRouteType(&RouteType);
        }
    }

    free(ParsingArguments);
    ThreadpoolDelete(Pool);

    IteratorFile = FirstFile;
    while (IteratorFile != NULL)
    {
        file *Temp = IteratorFile;
        IteratorFile = IteratorFile->Next;
        free(Temp);
    }


    FreeSettings(&Settings);

    return 0;
}