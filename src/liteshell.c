#include "liteshell.h"
#include <stdlib.h>
#include "port/test.h"

#define PTR_CHECK() map == (map_t*)0

#define VERSION "1.0"

typedef struct PACKED {
    func_t func;
    const char* sign;
    const char* desc;
} cmd_t;

typedef struct PACKED entry_t {
    const char* key;
    cmd_t cmd;
    struct entry_t* next;
} entry_t;

typedef struct PACKED {
    entry_t** entries;
    unsigned int size;
    unsigned int count;
} map_t;

union {
    enum {
        STATE_START,
        STATE_FUNC,
        STATE_PARAMS,
        STATE_DONE,
        STATE_ERROR_FUNC,
        STATE_ERROR_PARAMS
    } state;
    unsigned char state_bits;
} input;

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
    // _print(" _     _ _         ____  _          _ _ \n");
    // _print("| |   (_) |_ ___  / ___|| |__   ___| | |\n");
    // _print("| |   | | __/ _ \\ \\___ \\| '_ \\ / _ \\ | |\n");
    // _print("| |___| | ||  __/  ___) | | | |  __/ | |\n");
    // _print("|_____|_|\\__\\___| |____/|_| |_|\\___|_|_| \n");
    // _print("\033[32mVersion: "VERSION " Build: "__TIME__" "__DATE__"\033[0m\n");
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

static INLINE unsigned char _strlen(const char* str) {
    unsigned char len = 0;
    while (str[len] != '\0') len++;
    return len;
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
    entry->cmd = (cmd_t){func, sign, desc};
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
    return (cmd_t){(func_t)0, (char*)0, (char*)0};
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
    return -1;
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
    _strcpy(history[index[1]], command);
    index[1] = (index[1] + 1) % HISTORY_LEN_MAX;
    index[0] = index[1];
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

#endif

static void argparse() {
    input.state = STATE_START;
    static cmd_t cmd = {0}, ptr = {0};
    unsigned char start = 0, end = 0;
    unsigned char param_index = 0, param_len = 0;
    unsigned char i = 0, j = 0, k = 0;
    static char c;

    do {
        c = Shell._buff[i];
        switch (input.state) {
            case STATE_START:
                if (c != ' ') {
                    start = i;
                    input.state = STATE_FUNC;
                }
                break;
            case STATE_FUNC:
                if (c == '(' || c == '\0') {
                    Shell._buff[i] = '\0';
                    cmd = match(Shell._buff + start);

                    if (cmd.func == (func_t)0) {
                        input.state = STATE_ERROR_FUNC;
                    } else if (c == '\0') {
                        input.state = (cmd.sign[0] == '\0') ? STATE_DONE : STATE_ERROR_PARAMS;
                    } else {
                        start = i + 1;
                        param_len = _strlen(cmd.sign);
                        input.state = (cmd.sign[0] == '\0') ? STATE_DONE : STATE_PARAMS;
                    }
                }
                break;
            case STATE_PARAMS:
                if (c == ',' || c == ')') {
                    end = i;
                    Shell._buff[end] = '\0';
                    switch (cmd.sign[param_index]) {
                        case 'i':
                            params[param_index].l = atol(Shell._buff + start);
                            if (params[param_index].l == 0)
                                input.state = STATE_ERROR_PARAMS;
                            break;
                        case 'f':
                            params[param_index].f = atof(Shell._buff + start);
                            if (params[param_index].f == 0.f)
                                input.state = STATE_ERROR_PARAMS;
                            break;
                        case 'd':
                            params[param_index].d = atof(Shell._buff + start);
                            if (params[param_index].d == 0.)
                                input.state = STATE_ERROR_PARAMS;
                            break;
                        case 'c':
                            for (j = start; j < end; j++)
                                if (Shell._buff[j] == '\'') break;
                            for (k = j + 1; k < end; k++)
                                if (Shell._buff[k] == '\'') break;

                            if (k - j == 2) {
                                params[param_index].c = Shell._buff[j + 1];
                            } else 
                                input.state = STATE_ERROR_PARAMS;
                            break;
                        case 's':
                            for (j = start; j < end; j++)
                                if (Shell._buff[j] == '"') break;
                            for (k = j + 1; k < end; k++)
                                if (Shell._buff[k] == '"') break;

                            if (k > j) {
                                Shell._buff[k] = '\0';
                                params[param_index].str = Shell._buff + j + 1;
                            } else 
                                input.state = STATE_ERROR_PARAMS;
                            break;
                        case 'p':
                            for (j = start; j < end; j++)
                                if (Shell._buff[j] != ' ') break;
                            for (k = end - 1; k > j; k--)
                                if (Shell._buff[k] != ' ') {
                                    Shell._buff[k + 1] = '\0';
                                    break;
                                }
                            ptr = match(Shell._buff + j);
                            if (ptr.sign == (char*)0)
                                input.state = STATE_ERROR_PARAMS;
                            else
                                params[param_index].ptr = (ptr.func == (func_t)0 ? (void*)ptr.sign : ptr.func);
                            break;
                        default:
                            input.state = STATE_ERROR_PARAMS;
                    }
                    param_index++;
                    start = i + 1;
                    if (c == ')')
                        input.state = (param_index != param_len) ? STATE_ERROR_PARAMS : STATE_DONE;
                } else if (c == '\0')
                    input.state = STATE_ERROR_PARAMS;
                break;
            case STATE_DONE:
            case STATE_ERROR_FUNC:
            case STATE_ERROR_PARAMS:
                break;
        }
    } while (c != '\0' && ++i);

    if (input.state == STATE_ERROR_FUNC)
        _print("Command not found\n");
    else if (input.state == STATE_ERROR_PARAMS)
        _print("Invalid parameters\n");
    else if (input.state == STATE_DONE)
        exec(cmd.func, param_len);
}

void _run(void) {
    static char c;
    static char* command = (char*)0;
    unsigned char next = 0, end = 0;
    _print("\rLiteShell > ");

    while ((c = _getchar()) != '\n' || (Shell._buff[end] = '\0')) {
        if (end > CMD_LEN_MAX) break;

        switch (c) {
            case '\033': 
                _getchar();
                switch(_getchar()) {
                    case 'C': 
                        if (next < end) {
                            _print("\033[C");
                            next++;
                        }
                        break;
                    case 'D':
                        if (next > 0) {
                            _print("\033[D");
                            next--;
                        }
                        break;
#ifdef USE_HISTORY
                    case 'A': 
                        command = up_history();
                        break;
                    case 'B':
                        command = down_history();
                        break;    
                }
                if (command != (char*)0) {
                    _strcpy(Shell._buff, command);
                    _print("\rLiteShell > \033[K");
                    _print(Shell._buff);
                    end = next = _strlen(Shell._buff);
                    command = (char*)0;
                }
#else
                }
#endif
                break;
            case '\b':
            case 127: 
                if (next > 0) {
                    unsigned char i;
                    for (i = next; i < end; i++)
                        if (i < end)
                            Shell._buff[i-1] = Shell._buff[i];
                    
                    if (end > 0)
                        Shell._buff[end-1] = '\0';
                    _print("\033[D\033[K"); 
                    if (end > next) {
                        _print(Shell._buff + next - 1);
                        for (i = 0; i < end - next; i++, _print("\033[D"));
                    } 
                    next--;
                    end--;
                }
                break;
            case ' ':
                if (end < CMD_LEN_MAX) {
                    for (unsigned char i = end + 1; i > next; i--)
                        Shell._buff[i] = Shell._buff[i - 1];
                    Shell._buff[next] = ' ';
            
                    end++;
                    next++;
                
                    Shell._buff[end] = '\0';

                    _print("\rLiteShell > \033[K");
                    _print(Shell._buff);
                    for (unsigned char i = 0; i < end - next; i++,_print("\033[D"));
                }
                break;
            case '\t':
                break;
            default:
                if (c >= 32 && c <= 126 && end < CMD_LEN_MAX) {
                    if (next < end) {
                        for (unsigned char i = end; i > next; i--)
                            Shell._buff[i] = Shell._buff[i - 1];
                    }
                    Shell._buff[next] = c;
                    _putchar(c);
                    next++;
                    end++;
                    Shell._buff[end] = '\0';
                    if (next < end) {
                        _print(Shell._buff + next);
                        for (unsigned char i = next; i < end; i++,_print("\033[D"));
                    }
                }
        }
    }

    _print("\n");
    if (end == 0) return;

#ifdef USE_HISTORY
    add_history(Shell._buff);
#endif

    argparse();

}