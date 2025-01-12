#include "kcc.h"

void assert(int n) {
    if (n == 0)
        error("assert error");
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

// エラー箇所を報告する
// format
// foo.c:10: x = y + + 5;
//                   ^ 式ではありません
void error_at(char *loc, char *msg) {
    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n')
        end++;

    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n')
            line_num++;

    // 見つかった行を、ファイル名と行番号と一緒に表示
    int indent = fprintf(stderr, "%s:%d: ", file_name, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // エラー箇所を"^"で指し示して、エラーメッセージを表示
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");  // pos個の空白を出力
    fprintf(stderr, "^ %s\n", msg);
    // vfprintf(stderr, fmt, ap);
    printf("\n");

    exit(EXIT_FAILURE);
}

bool startsWith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

int is_alpha(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           (c == '_');
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

// 変数の文字列分ポインタを進める
void str_advanve(char **p) {
    while (is_alnum(**p)) {
        *p += 1;
    }
}

void next_token() {
    if (token == NULL) {
        error("next_token() failure");
    }
    token = token->next;
}

Token *get_nafter_token(int n) {
    Token *t = token;
    for (int i = 0; i < n; i++) {
        t = t->next;
    }
    return t;
}

// n文字複製する
char *my_strndup(char *s, size_t n) {
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}

void swap(void **p, void **q) {
    void *tmp = *p;
    *p = *q;
    *q = tmp;
}

void *memory_alloc(size_t size) {
    void *p = calloc(1, size);
    if (p == NULL) {
        error("calloc() failure");
    }
    return p;
}

void copy_func(Function *to, Function *from) {
    if (to == NULL || from == NULL) {
        error("copy_func() failure: 引数にNULLが含まれています");
    }
    to->name = from->name;
    to->body = from->body;
    to->params = from->params;
    to->locals = from->locals;
    to->stack_size = from->stack_size;
    to->ret_type = from->ret_type;
    to->is_prototype = from->is_prototype;
}
