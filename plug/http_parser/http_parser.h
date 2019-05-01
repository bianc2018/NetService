#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#define HTTP_PARSER_API  extern "C" __declspec( dllexport )

HTTP_PARSER_API  int http_parser_before(unsigned long long handle);
HTTP_PARSER_API  int http_parser_after(unsigned long long handle);

HTTP_PARSER_API  int uri_parser_before(unsigned long long handle);
HTTP_PARSER_API  int uri_parser_after(unsigned long long handle);
#endif
