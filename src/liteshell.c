#include "liteshell.h"
#include <stdlib.h>
#include "port/test.h"

#define PTR_CHECK() map == (map_t*)0

#define VERSION "1.0"

typedef union {
    enum {
        STATE_START,
        STATE_FUNC,
        STATE_PARAMS,
        STATE_DONE,
        STATE_ERROR_FUNC,
        STATE_ERROR_PARAMS,
        STATE_ERROR_LEN
    } state;
    unsigned char state_bits;
} state_t;

typedef struct {
    func_t func;
    const char* sign;
    const char* desc;
}__attribute__((packed)) cmd_t;

typedef struct entry_t {
    const char* key;
    cmd_t cmd;
    struct entry_t* next;
}__attribute__((packed)) entry_t;

typedef struct {
    entry_t** entries;
    unsigned int size;
    unsigned int count;
}__attribute__((packed)) map_t;

liteshell_t Shell = {
    .init = _begin,
    .print = _print,
    .export = _export,
    .run = _run,
    .free = _free
};

static any_t params[PARAMS_MAX] = { 0 };
static map_t* map = (map_t*)0;

static void help();
static INLINE void clear();

void _begin() {
    map = (map_t*)malloc(sizeof(map_t));
    map->size = INITIAL_MAP_SIZE;
    map->count = 0;
    map->entries = (entry_t**)malloc(sizeof(entry_t*) * map->size);
    for (unsigned int i = 0; i < map->size; map->entries[i++] = (entry_t*)0);
    _export((func_t)help, "help", "", "show all commands");
    _export((func_t)clear, "clear", "", "clear the screen");
    _print(" _     _ _         ____  _          _ _ \n");
    _print("| |   (_) |_ ___  / ___|| |__   ___| | |\n");
    _print("| |   | | __/ _ \\ \\___ \\| '_ \\ / _ \\ | |\n");
    _print("| |___| | ||  __/  ___) | | | |  __/ | |\n");
    _print("|_____|_|\\__\\___| |____/|_| |_|\\___|_|_| \n");
    _print("\033[32mVersion: "VERSION " Build: "__TIME__" "__DATE__"\033[0m\n");
}

static INLINE int _strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) str1++, str2++;
    return *str1 - *str2;
}

static INLINE void _strcpy(char* dest, const char* src) {
    unsigned char i = 0;
    for (i = 0; i < CMD_LEN_MAX && src[i] != '\0'; dest[i] = src[i], i++);
    dest[i] = '\0';
}

static unsigned int fnv_hash(const char *str, unsigned int size) {
    unsigned int hash = 0x9747b28c;
    const unsigned char *p = (const unsigned char *)str;
    while (*p) {
        hash ^= (unsigned int)*p++;
        hash *= 16777619U;
    }
    return hash % size;
}

static void resize() {
    int oldSize = map->size;
    map->size += map->size / 4;
    map->entries = (entry_t**)realloc(map->entries, sizeof(entry_t*) * map->size);
    for (unsigned int i = oldSize; i < map->size; map->entries[i++] = (entry_t*)0);
}

void _export(func_t func, const char* name, const char* sign, const char* desc) {
    if (PTR_CHECK()) _begin();
    if (map->count >= map->size * 0.75)
        resize();
    int index = fnv_hash(name, map->size);
    entry_t* entry = (entry_t*)malloc(sizeof(entry_t));
    entry->key = name;
    entry->cmd = (cmd_t){.func = func, .sign = sign, .desc = desc};
    entry->next = map->entries[index];
    map->entries[index] = entry;
    map->count++;
}

static cmd_t match(const char* key) {
    int index = fnv_hash(key, map->size);
    entry_t* entry = map->entries[index];
    while (entry != ((void *)0)) {
        if (_strcmp(entry->key, key) == 0) {
            return entry->cmd;
        }
        entry = entry->next;
    }
    return (cmd_t){(func_t)0, (const char*)0, (const char*)0};
}

static void help() {
    for (unsigned int i = 0; i < map->size; i++) {
        entry_t* entry = map->entries[i];
        while (entry != (entry_t*)0) {
            _print(entry->key);
            _print(": ");
            _print((entry->cmd).desc);
            _print("\n");
            entry = entry->next;
        }
    }
}

static INLINE void clear() {
    _print("\033[2J\033[H");
}

void _free() {
    if (PTR_CHECK()) return;
    for (int i = 0; i < map->size; i++) {
        entry_t* entry = map->entries[i];
        while (entry != (entry_t*)0) {
            entry_t* next = entry->next;
            free((void *)entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->entries);
    free(map);
}

void __attribute__((weak)) _putchar(char c) {
}

int __attribute__((weak)) _getchar() {
}

INLINE void _print(const char* str) {
    for (unsigned int i = 0; str[i] != '\0'; _putchar(str[i++]));
}

static void exec(func_t func, unsigned char len) {
    switch (len) {
#if PARAMS_MAX >= 1
        case 1:
            ((void (*)(any_t))func)(params[0]);
            break;
#endif
#if PARAMS_MAX >= 2
        case 2:
            ((void (*)(any_t, any_t))func)(params[0], params[1]);
            break;
#endif
#if PARAMS_MAX >= 3
        case 3:
            ((void (*)(any_t, any_t, any_t))func)(params[0], params[1], params[2]);
            break;
#endif
#if PARAMS_MAX >= 4
        case 4:
            ((void (*)(any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3]);
            break;
#endif
#if PARAMS_MAX >= 5
        case 5:
            ((void (*)(any_t, any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3], params[4]);
            break;
#endif
#if PARAMS_MAX >= 6
        case 6:
            ((void (*)(any_t, any_t, any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3], params[4], params[5]);
            break;
#endif
#if PARAMS_MAX >= 7
        case 7:
            ((void (*)(any_t, any_t, any_t, any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
            break;
#endif
#if PARAMS_MAX >= 8
        case 8:
            ((void (*)(any_t, any_t, any_t, any_t, any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]);
            break;
#endif
#if PARAMS_MAX >= 9
        case 9:
            ((void (*)(any_t, any_t, any_t, any_t, any_t, any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8]);
            break;
#endif
#if PARAMS_MAX == 10
        case 10:
            ((void (*)(any_t, any_t, any_t, any_t, any_t, any_t, any_t, any_t, any_t, any_t))func)(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
            break;
#endif
        default:
            func();
            break;
    }
}

#ifdef USE_HISTORY

static char history[HISTORY_LEN_MAX][CMD_LEN_MAX+1];
static char index[2];

static void add_history(const char *command) {
    for (int i = 0; i < CMD_LEN_MAX && command[i] != '\0'; i++)
        history[index[0]][i] = command[i];
    history[index[0]][CMD_LEN_MAX] = '\0';
    index[0] = (index[0] + 1) % HISTORY_LEN_MAX;
    if (index[1] < HISTORY_LEN_MAX)
        index[1]++;
}

static char* up_history() {
    if (index[1] == 0)
        return (char*)0;
    index[0] = (index[0] - 1 + HISTORY_LEN_MAX) % HISTORY_LEN_MAX;
    return history[index[0]];
}

static char* down_history() {
    if (index[1] == 0)
        return (char*)0;
    index[0] = (index[0] + 1) % HISTORY_LEN_MAX;
    if (index[0] == index[1]) {
        index[0] = (index[0] - 1 + HISTORY_LEN_MAX) % HISTORY_LEN_MAX;
        return (char*)0;
    }
    return history[index[0]];
}

static void handle_key(char c) {
    if (c == '\033') {
        char* command = (char*)0;
        _getchar();
        switch(_getchar()) {
            case 'A':
                command = up_history();
                break;
            case 'B':
                command = down_history();
                break;
        }
        if (command != (char*)0) 
            _strcpy(Shell._buff, command);
    }
}

#endif

void _run(void) {
    if (PTR_CHECK()) _begin();
    state_t input = { STATE_START };
    static cmd_t cmd;
    static char c;
    static char buff[CMD_LEN_MAX+1] = { 0 };
    unsigned char sign_len = 0, next = 0, index = 0;
    _print("\rLiteShell > ");

    do {
        if ((c = _getchar()) == ' ' || c == '\t') 
            continue;

        if (next >= CMD_LEN_MAX) 
            input.state = STATE_ERROR_FUNC;
        else
            Shell._buff[next] = c;

        switch (input.state) {
            case STATE_START:
                if (c == '(' || c == '\n') {
                    Shell._buff[next] = '\0';
                    if ((cmd = match(Shell._buff)).sign == (const char*)0)
                        input.state = STATE_ERROR_FUNC;
                    else {
                        while (cmd.sign[sign_len] != '\0') sign_len++;
                        input.state = (sign_len == 0) ? STATE_DONE : ((c == '\n') ? STATE_ERROR_PARAMS : STATE_PARAMS);
                    }
                    next = 0;
                } else
                    next++;
                break;
            case STATE_PARAMS:
                if (c == ',' || c == ')') {
                    Shell._buff[next] = '\0';
                    if (c == ')') input.state = STATE_DONE;
                    if (index < sign_len) {
                        switch (cmd.sign[index]) {
                            case 'i':
                                params[index] = (any_t){.l = atol(Shell._buff)};
                                if (params[index].l == 0)
                                    input.state = STATE_ERROR_PARAMS;
                                break;
                            case 'c':
                                if (Shell._buff[1] == '\0')
                                    params[index] = (any_t){.c = Shell._buff[0]};
                                else 
                                    input.state = STATE_ERROR_PARAMS;
                                break;
                            case 's':
                                params[index] = (any_t){.str = buff};
                                _strcpy(buff, Shell._buff);
                                break;
                            case 'f':
                                params[index] = (any_t){.f = atof(Shell._buff)};
                                if (params[index].f == 0.f)
                                    input.state = STATE_ERROR_PARAMS;
                                break;
                            case 'd':
                                params[index] = (any_t){.d = atof(Shell._buff)};
                                if (params[index].d == 0.)
                                    input.state = STATE_ERROR_PARAMS;
                                break;
                            default:
                                input.state = STATE_ERROR_PARAMS;
                        }
                        index++;
                    } else 
                        input.state = STATE_ERROR_PARAMS;
                    if (c == ')' && index < sign_len)
                        input.state = STATE_ERROR_PARAMS;
                    next = 0;
                } else if (c == '\n')
                    input.state = STATE_ERROR_PARAMS;
                else
                    next++;
                break;
            case STATE_DONE:
            case STATE_ERROR_FUNC:
            case STATE_ERROR_PARAMS:
            case STATE_ERROR_LEN:
                break;
        }
    } while(c != '\n' && c != -1);
    
    if (input.state == STATE_ERROR_FUNC)
        _print("Command not found\n");
    else if (input.state == STATE_ERROR_PARAMS)
        _print("Invalid parameters\n");
    else if (input.state == STATE_ERROR_LEN)
        _print("Input too long\n");
    else if (input.state == STATE_DONE)
        exec(cmd.func, sign_len);

#ifdef USE_HISTORY
    add_history(Shell._buff);
#endif

}