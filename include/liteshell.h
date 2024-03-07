#ifndef __LITESHELL_H__
#define __LITESHELL_H__

#if defined(__GNUC__) || defined(__clang__) || defined(__ICC) || defined(__ARMCC_VERSION)
    #define INLINE inline
#elif defined(_MSC_VER) || defined(__CC_ARM) || defined(__IAR_SYSTEMS_ICC__)
    #define INLINE __inline
#elif defined(__C51__) || defined(__CX51__)
    #define INLINE
#else
    #define INLINE
#endif

#define INITIAL_MAP_SIZE                      10U

#define CMD_LEN_MAX                           29U

#define PARAMS_MAX                            10U

#define USE_HISTORY
#define USE_AUTOCOMPLETE

#ifdef USE_HISTORY
    #define HISTORY_LEN_MAX                   10U
#endif

#define FUNC_CONVERT(func, sign, desc)      (func_t)func, #func, sign, desc 

typedef void (*func_t) ();

typedef union {
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    float f;
    double d;
    char* str;
} any_t;

typedef struct {
    char _buff[CMD_LEN_MAX+1];
    void (*init)();
    void (*print)(const char* str);
    void (*add)(func_t, const char* name, const char* sign, const char* desc);
    void (*run)();
    void (*free)();  
}__attribute__((packed)) liteshell_t;

void _begin();
void _add(func_t, const char*, const char*, const char*);
void _run();
void _destroy();
void _print(const char*);
extern liteshell_t Shell;

#endif