#ifndef HAL_DYNAMICS_H
#define HAL_DYNAMICS_H

typedef void* Dynamic;

extern Dynamic LoadDynamic(const char* path);
extern void UnloadDynamic(Dynamic dynamic);
extern void* GetDynamicSymbol(Dynamic dynamic, const char* sym_name);

#endif