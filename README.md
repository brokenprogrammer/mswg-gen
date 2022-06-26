# mswg-gen
Custom OpenAPI generator that is used to generate swagger documentation using comments.

The project currently has support for C language but can technically support any programming language 
that makes use of C style comments. Some parsing has to be added however.

The goal of this project is to have a fast swagger generator that is excluded from the project that can easily be integrated
into any build pipeline.

## Usage

First you need to create a config.ini file within your project directory. An example configuration looks like the following:

```ini
; This is a comment
[mswgen]
title = Sample
# This is also a comment
description = This is a sample application
termsofservice = https://google.se

[mswgen.contact]
name = Support
url = https://google.se
email = support@google.se

[mswgen.license]
name = MIT
url = https://google.se

[mswgen.version]
version= v1
```

Explanation for the different configuration settings are explained in the following table.
| Configuration | Description   | Example       |
| ------------- | ------------- | ------------- |
| title  | The title of your application. This will be displayed on your swagger page. | My Appllication |
| description  | Description of your application. Will be displayed on your swagger page.  | This is a sample application |
| termsofservice  | A link to the terms of service for your API..  | https://example.com |
| contact.name  | Display name for your contact.  | Support |
| contact.url  | Contact URL..  | https://example.com |
| contact.email  | Contact email.  | support@example.com |
| license.name  | Name of the license for your API.  | MIT |
| license.url  | URL to license of your API.  | https://example.com/license |
| version.version | Version of your API. | v1 |

After you have set up your configuration file you can build the application by entering the `src` directory and run `make`.

Once done you should have access to the executable `mswg_gen` that you can place where you want it.

Finally some modification is needed in the code of your application. mswg_gen uses two types of comment to specify two different API types routes and route types.

### Routes

A new route is specified by writing a comment in your code with the following format:
```C
// @Route(route, method, return_type)
```

An example would be

```C
// @Route(/api/login, post, user_object)
```

Some routes do not return any json objects hence the `return_type` can also be emitteed like so:

```C
// @Route(/api/register, post)
```

### Route Types

Route types are added by annotating your structures using the `@RouteType` command.
This allows mswg_gen to add the type to the list of known `return_types` for routes in order to generate json within swagger.

Example:
```C
// @RouteType
typedef struct {
    u_int32_t state[5];
    u_int64_t count;
    u_int8_t buffer[SHA1_BLOCK_LENGTH];
} SHA1_CTX;
```

### Integrating into your project

The output of mswg_gen can easily be output in a file like so:
```bash
./mswg_gen > example.txt
```

The generated output can then be used together with the static swagger site provided 
within this repository: <https://github.com/swagger-api/swagger-ui>

This is easy to set up by downloading the latest release and using the static HTML/JS/CSS within the downloaded `/dist` folder.

# License

MIT License

Copyright (c) 2022 Oskar Mendel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
