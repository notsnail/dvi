// hal error reporting tools
#ifndef HAL_ERROR_H

typedef enum 
{
    HALCYON_INFO,
    HALCYON_WARNING,
    HALCYON_ERROR
} ReportErrorLevel;

extern void ReportErrorToWindow(const char* why, ReportErrorLevel type);
extern void FatalErrorWithCode(const char* why, int code);

extern void TheGameIsLackingCoconutError();

#define HAL_ERROR_H
#endif