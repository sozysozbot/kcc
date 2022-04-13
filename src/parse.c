#include "9cc.h"

/*
 * ローカル変数を単方向の連結リストで持つ
 * localsは後に出たローカル変数のポインタを持つ
 */
static Var *locals;
static Function *cur_parse_func;

/* nodeの生成 */
static Node *new_add(Node *lhs, Node *rhs);
static Node *new_sub(Node *lhs, Node *rhs);
static Node *new_mul(Node *lhs, Node *rhs);
static Node *new_div(Node *lhs, Node *rhs);
static Node *new_mod(Node *lhs, Node *rhs);
static Node *new_node_num(int val);
static Var *new_lvar(Token *tok, Type *type);
static Var *new_gvar(Token *tok, Type *type);
static void create_lvar_from_params(Var *params);
static Var *find_var(Token *tok, bool is_global);

/* AST */
static Type *type_specifier();
static Node *declaration_global(Type *type);
static Node *declaration_var(Type *type, bool is_global);
static Node *declaration(Type *type, bool is_global);
static Var *declaration_param(Var *cur);
static Type *pointer(Type *type);
static Function *func_define();
static Node *compound_stmt();
static Node *stmt();
static Node *expr();
static Node *assign();
static Node *logical_expression();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *postfix();
static Type *type_suffix(Type *type);
static Node *primary();

/* 指定された演算子が来る可能性がある */
static bool consume(int op)
{
    if (token->kind != op)
    {
        return false;
    }
    next_token();
    return true;
}

static bool consume_nostep(int op)
{
    if (token->kind != op)
    {
        return false;
    }
    return true;
}

/* 指定された演算子が必ず来る */
static void expect(int op)
{
    if (token->kind != op)
    {
        if (op == TK_TYPE)
        {
            print_token_kind(op);
            error_at(token->str, "expect() failure: 適当な位置に型がありません");
        }
        char msg[100];
        snprintf(msg, 100, "expect() failure: 「%c」ではありません。", op);
        error_at(token->str, msg);
    }
    next_token();
}

static void expect_nostep(int op)
{
    if (token->kind != op)
    {
        if (op == TK_TYPE)
        {
            print_token_kind(op);
            error_at(token->str, "expect_nostep() failure: 適当な位置に型がありません");
        }

        error_at(token->str, "expect_nostep() failure: 適切な演算子ではありません");
    }
}

static int expect_number()
{
    if (token->kind != TK_NUM)
    {
        error_at(token->str, "expect_number() failure: 数ではありません");
    }
    int val = token->val;
    next_token();
    return val;
}

static bool at_eof()
{
    return token->kind == TK_EOF;
}

/* ノードを引数にもつsizeofの実装 */
static int sizeOfNode(Node *node)
{
    add_type(node);
    return sizeOfType(node->type);
}

/* ローカル変数の作成 */
static Var *new_lvar(Token *tok, Type *type)
{
    Var *lvar = memory_alloc(sizeof(Var));
    lvar->next = locals;
    lvar->name = my_strndup(tok->str, tok->len);
    lvar->len = tok->len;
    lvar->type = type;
    lvar->offset = locals->offset + sizeOfType(type);
    locals = lvar; // localsを新しいローカル変数に更新
    return lvar;
}

static Var *new_gvar(Token *tok, Type *type)
{
    Var *gvar = memory_alloc(sizeof(Var));
    gvar->next = globals;
    gvar->name = my_strndup(tok->str, tok->len);
    gvar->len = tok->len;
    gvar->type = type;
    gvar->is_global = true;
    globals = gvar; // globalsを新しいグローバル変数に更新
    return gvar;
}

static void new_struct_member(Token *tok, Type *member_type, Type *struct_type)
{
    Var *member = memory_alloc(sizeof(Var));
    member->next = struct_type->member;
    member->name = my_strndup(tok->str, tok->len);
    member->len = tok->len;
    member->type = member_type;
    member->offset = struct_type->member->offset + sizeOfType(member_type);
    struct_type->member = member;
    struct_type->size += member_type->size;
}

// TODO: 引数に適切な型をつけるようにする
/* 引数からローカル変数を作成する(前から見ていく) */
static void create_lvar_from_params(Var *params)
{
    if (!params)
        return;

    Var *lvar = memory_alloc(sizeof(Var));
    lvar->name = params->name;
    lvar->len = params->len;
    lvar->type = params->type;
    lvar->offset = locals->offset + sizeOfType(lvar->type);
    lvar->next = locals;

    locals = lvar;
    create_lvar_from_params(params->next);
}

/* 既に定義されたローカル変数を検索 */
static Var *find_var(Token *tok, bool is_global)
{
    Var *vars = is_global ? globals : locals;
    for (Var *var = vars; var; var = var->next)
    {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
        {
            return var;
        }
    }
    return NULL;
}

/*************************************/
/******                         ******/
/******        NEW_NODE         ******/
/******                         ******/
/*************************************/

// ノード作成
static Node *new_node(NodeKind kind)
{
    Node *node = memory_alloc(sizeof(Node));
    node->kind = kind;
    return node;
}

// 演算子ノード作成
static Node *new_binop(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = memory_alloc(sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

/*
 * 演算子には型のキャストがある
 */
static Node *new_add(Node *lhs, Node *rhs)
{
    // 型を伝搬する
    add_type(lhs);
    add_type(rhs);

    // enum TypeKind の順番にする (lhs <= rhs)
    if (lhs->type->kind > rhs->type->kind)
    {
        swap((void **)&lhs, (void **)&rhs);
    }

    Node *node = new_binop(ND_ADD, lhs, rhs);

    if (is_integertype(lhs->type->kind) && is_integertype(rhs->type->kind))
    {
        return node;
    }

    if (is_integertype(lhs->type->kind) && rhs->type->kind == TYPE_PTR)
    {
        node->lhs = new_mul(lhs, new_node_num(rhs->type->ptr_to->size));
        add_type(node->lhs);
        return node;
    }

    if (is_integertype(lhs->type->kind) && rhs->type->kind == TYPE_ARRAY)
    {
        // ポインター型として演算
        node->lhs = new_mul(lhs, new_node_num(rhs->type->ptr_to->size));
        add_type(node->lhs);
        return node;
    }

    error("new_add() failure: 実行できない型による演算です");
}

static Node *new_sub(Node *lhs, Node *rhs)
{
    add_type(lhs);
    add_type(rhs);

    // lhsとrhsの順番に関係あり
    Node *node = new_binop(ND_SUB, lhs, rhs);

    if (is_integertype(lhs->type->kind) && is_integertype(rhs->type->kind))
    {
        return node;
    }

    if (lhs->type->kind == TYPE_PTR && is_integertype(rhs->type->kind))
    {
        node->rhs = new_mul(rhs, new_node_num(lhs->type->ptr_to->size));
        add_type(node->rhs);
        return node;
    }

    if (lhs->type->kind == TYPE_ARRAY && is_integertype(rhs->type->kind))
    {
        // ポインター型として演算
        node->rhs = new_mul(rhs, new_node_num(lhs->type->ptr_to->size));
        add_type(node->rhs);
        return node;
    }

    error("new_sub() failure: 実行できない型による演算です");
}

static Node *new_mul(Node *lhs, Node *rhs)
{
    add_type(lhs);
    add_type(rhs);

    // enum TypeKind の順番にする (lhs <= rhs)
    if (lhs->type->kind > rhs->type->kind)
    {
        swap((void **)&lhs, (void **)&rhs);
    }

    Node *node = new_binop(ND_MUL, lhs, rhs);

    if (is_integertype(lhs->type->kind) && is_integertype(rhs->type->kind))
    {
        return node;
    }

    error("new_mul() failure: 実行できない型による演算です");
}

static Node *new_div(Node *lhs, Node *rhs)
{
    add_type(lhs);
    add_type(rhs);

    // lhsとrhsの順番に関係あり (lhs <= rhs)
    Node *node = new_binop(ND_DIV, lhs, rhs);

    if (is_integertype(lhs->type->kind) && is_integertype(rhs->type->kind))
    {
        return node;
    }

    error("new_div() failure: 実行できない型による演算です");
}

static Node *new_mod(Node *lhs, Node *rhs)
{
    add_type(lhs);
    add_type(rhs);

    // lhsとrhsの順番に関係あり (lhs <= rhs)
    Node *node = new_binop(ND_MOD, lhs, rhs);

    if (is_integertype(lhs->type->kind) && is_integertype(rhs->type->kind))
    {
        return node;
    }

    error("new_mod() failure: 実行できない型による演算です");
}

static Node *new_assign(Node *lhs, Node *rhs)
{
    add_type(lhs);
    add_type(rhs);

    Node *node = new_binop(ND_ASSIGN, lhs, rhs);
    // 代入できるかチェック
    add_type(node);

    return node;
}

/* 数値ノードを作成 */
static Node *new_node_num(int val)
{
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

/* 変数を宣言 */
static Node *declear_node_ident(Token *tok, Type *type, bool is_global)
{
    Node *node = new_node(ND_VAR);
    Var *var = find_var(tok, is_global);
    if (var)
    {
        error_at(tok->str, "declear_node_ident() failure: 既に宣言済みです");
    }

    var = is_global ? new_gvar(tok, type) : new_lvar(tok, type);
    node->var = var;
    return node;
}

// 変数のノードを取得
static Node *get_node_ident(Token *tok)
{
    Node *node = new_node(ND_VAR);
    Var *var;
    var = find_var(tok, false); // ローカル変数を取得
    if (!var)
    {
        var = find_var(tok, true); // グローバル変数から取得
        if (!var)
        {
            error_at(tok->str, "get_node_ident() failure: 宣言されていません");
        }
    }

    node->var = var;
    return node;
}

Function *find_func(char *name)
{
    for (int i = 0; funcs[i]; i++)
    {
        if (!memcmp(name, funcs[i]->name, strlen(name)))
        {
            return funcs[i];
        }
    }

    return NULL;
}

Type *find_struct_type(char *name)
{
    for (int i = 0; i < struct_local_lists->len; i++)
    {
        Type *t = struct_local_lists->body[i];
        if (strcmp(t->name, name) == 0)
        {
            return t;
        }
    }

    return NULL;
}

/*************************************/
/******                         ******/
/******           AST           ******/
/******                         ******/
/*************************************/

// <program> = ( <declaration_global> | <func_define> )*
// 関数かどうかの先読みが必要
void program()
{
    int i = 0;
    while (!at_eof())
    {
        Type *type = type_specifier();
        type = pointer(type);
        Token *t = get_nafter_token(1);
        if (t->kind == '(')
        {
            funcs[i++] = func_define(type);
        }
        else
        {
            Node *node = declaration_global(type);
        }
    }
    funcs[i] = NULL;
}

// <declaration_global> = <declaration> ";"
static Node *declaration_global(Type *type)
{
    Node *node = declaration(type, true);
    expect(';');
    return node;
}

// <initialize>  = "{" <initialize> (","  <initialize>)* "}"
//               | <assign>
static Node *initialize()
{
    Node *node = NULL;
    // TODO 配列の初期化式
    // if (consume('{')) {
    //     node = initialize();
    //     while (consume(',')) {
    //         node = initialize();
    //     }
    //     expect('}');
    //     return node;
    // }

    return assign();
}

// pointer = "*"*
static Type *pointer(Type *type)
{
    while (consume('*'))
    {
        Type *t = new_ptr_type(type);
        type = t;
    }
    return type;
}

// declaration_var = pointer ident type_suffix ("=" initialize)?
static Node *declaration_var(Type *type, bool is_global)
{
    type = pointer(type);
    Node *node = declear_node_ident(token, type, is_global);
    next_token();
    if (consume_nostep('['))
    {
        // 配列
        node->var->type = type_suffix(node->var->type);
        // 新しい型のオフセットにする
        node->var->offset += sizeOfType(node->var->type) - sizeOfType(type);

        return node;
    }
    // 変数
    if (consume('='))
    {
        return new_assign(node, initialize());
    }

    return node;
}

// declaration = type_specifier (declaration_var ("," declaration_var)+)?
static Node *declaration(Type *type, bool is_global)
{
    Node *node = declaration_var(type, is_global);
    if (consume_nostep(';'))
    {
        return node;
    }

    Node *n = new_node(ND_SUGER);
    n->stmts = new_vec();
    vec_push(n->stmts, node);
    while (consume(','))
    {
        vec_push(n->stmts, declaration_var(type, is_global));
    }
    return n;
}

// type_specifier pointer ident ";"
Type *struct_declaration(Type *type)
{
    Type *t = type_specifier();
    t = pointer(t);
    Token *tok = token;
    next_token();
    t = type_suffix(t);
    new_struct_member(tok, t, type);
    expect(';');
    return type;
}

/* type_specifier = "int"
 *                | "char"
 *                | "void"
 *                | "struct" ident
 *                | "struct" ident "{" struct_declaration* "}"
 */
static Type *type_specifier()
{
    expect_nostep(TK_TYPE);
    Type *type = token->type;
    next_token();
    if (type->kind == TYPE_STRUCT && consume('{'))
    {
        if (find_struct_type(type->name) != NULL)
        {
            error("find_struct_type() failure: %s構造体は既に宣言済みです。", type->name);
        }
        vec_push(struct_local_lists, type);
        // 構造体のメンバーの宣言
        type->member = memory_alloc(sizeof(Var));
        while (!consume('}'))
        {
            type = struct_declaration(type);
        }
        // 定義した順に並べ直す
        Var *reverse_member = NULL;
        while (type->member)
        {
            Var *tmp = type->member->next;
            type->member->next = reverse_member;
            reverse_member = type->member;
            type->member = tmp;
        }
        type->member = reverse_member->next;
    }

    return type;
}

// type_suffix = "[" num "]" type_suffix | ε
static Type *type_suffix(Type *type)
{
    if (consume('['))
    {
        int array_size = expect_number();
        expect(']');
        type = new_array_type(type_suffix(type), array_size);
    }

    return type;
}

// TODO: voidに対応
// declaration_param = type_specifier pointer ident type_suffix
static Var *declaration_param(Var *cur)
{
    Type *type = type_specifier();
    type = pointer(type);
    Token *tok = token;
    expect(TK_IDENT);
    Var *lvar = memory_alloc(sizeof(Var));
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->type = type;
    lvar->offset = cur->offset + sizeOfType(lvar->type);
    if (consume_nostep('['))
    {
        // ポインタとして受け取る
        lvar->type = type_suffix(lvar->type);
        // 新しい型のオフセットにする
        lvar->offset += sizeOfType(lvar->type) - sizeOfType(type);
    }
    return lvar;
}

// func_define = type_specifier ident "("
// (declaration_param ("," declaration_param)* )? ")" compound_stmt
static Function *func_define(Type *type)
{
    Function *fn = memory_alloc(sizeof(Function));
    cur_parse_func = fn;
    Token *tok = token;
    Var head = {};
    Var *cur = &head; // 引数の単方向連結リスト

    expect(TK_IDENT);
    fn->name = my_strndup(tok->str, tok->len);
    fn->ret_type = type;
    expect('(');
    while (!consume(')'))
    {
        if (cur != &head)
        {
            expect(',');
        }
        Var *p = declaration_param(cur);
        // 配列型は暗黙にポインターとして扱う
        if (p->type->kind == TYPE_ARRAY)
        {
            Type *t = p->type;
            p->type = new_ptr_type(t->ptr_to);
            p->offset += sizeOfType(p->type) - sizeOfType(t);
        }
        cur = cur->next = p;
    }

    fn->params = head.next; // 前から見ていく
    locals = memory_alloc(sizeof(Var));
    struct_local_lists = new_vec(); // 関数毎に構造体を初期化
    locals->offset = 0;
    create_lvar_from_params(fn->params);
    fn->body = compound_stmt();
    fn->locals = locals;
    return fn;
}

// compound_stmt = { stmt* }
static Node *compound_stmt()
{
    expect('{');
    Node *node = new_node(ND_BLOCK);
    node->stmts = new_vec();
    while (!consume('}'))
    {
        Node *n = stmt();
        if (n->kind == ND_SUGER)
        {
            vec_concat(node->stmts, n->stmts);
            continue;
        }
        else if (n->kind == ND_VAR)
        {
            // ローカル変数の宣言はコンパイルしない
            n = new_node(ND_NULL);
        }
        vec_push(node->stmts, n);
    }
    return node;
}

// TODO: do~while,continue,break,switch,else if,
/* stmt = expr? ";"
 *     | "return" expr ";"
 *      | "if" "(" expr ")" stmt ("else" stmt)?
 *      | "while" "(" expr ")" stmt
 *      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
 *      | ("continue" | "break")
 *      | compound_stmt
 */
static Node *stmt()
{
    Node *node;

    if (consume(TK_RETURN))
    {
        node = new_node(ND_RETURN);
        if (consume(';'))
        {
            // 何も返さない場合
            node->lhs = new_node_num(0); // ダミーで数値ノードを生成。codegenでvoid型かどうかを使って分岐
        }
        else
        {
            node->lhs = expr();
            add_type(node->lhs);
            if (!can_type_cast(node->lhs->type, cur_parse_func->ret_type->kind))
            {
                error("stmt() failure: can_type_cast fail");
            }
            if (cur_parse_func->ret_type->kind == TYPE_VOID)
            {
                node->lhs = new_node_num(0); // ダミーで数値ノードを生成。codegenでvoid型かどうかを使って分岐
            }
            expect(';');
        }
    }
    else if (consume(TK_IF))
    {
        node = new_node(ND_IF);
        expect('(');
        node->cond = expr();
        expect(')');
        node->then = stmt();
        if (consume(TK_ELSE))
        {
            node->els = stmt();
        }
    }
    else if (consume(TK_WHILE))
    {
        node = new_node(ND_WHILE);
        expect('(');
        node->cond = expr();
        expect(')');
        node->body = stmt();
    }
    else if (consume(TK_FOR))
    {
        node = new_node(ND_FOR);
        expect('(');
        if (!consume(';'))
        {
            node->init = expr();
            expect(';');
        }
        if (!consume(';'))
        {
            node->cond = expr();
            expect(';');
        }
        if (!consume(')'))
        {
            node->inc = expr();
            expect(')');
        }
        node->body = stmt();
    }
    else if (consume_nostep('{'))
    {
        node = compound_stmt();
    }
    else if (consume(TK_BREAK))
    {
        node = new_node(ND_BREAK);
        expect(';');
    }
    else if (consume(TK_CONTINUE))
    {
        node = new_node(ND_CONTINUE);
        expect(';');
    }
    else if (consume(';'))
    {
        node = new_node(ND_BLOCK);
        node->stmts = new_vec();
    }
    else
    {
        node = expr();
        expect(';');
    }

    return node;
}

// TODO: とりあえず一次元の配列だけを定義する
// TODO: 多次元配列に対応
// exprは一つの式で型の伝搬は大体ここまでありそう
// expr = assign | declaration
static Node *expr()
{
    if (consume_nostep(TK_TYPE))
    {
        Type *type = type_specifier();
        if (type->kind == TYPE_STRUCT && type->member != NULL)
        {
            // 構造体の宣言
            return new_node(ND_NULL);
        }

        if (type->kind == TYPE_STRUCT)
        {
            Type *t = find_struct_type(type->name);
            if (t == NULL)
            {
                error("type_specifier() failure: %s構造体は宣言されていません。", type->name);
            }
            type = t;
        }

        Node *node = declaration(type, false);
        return node;
    }

    return assign();
}

// TODO: %=, ++, --, ?:, <<=, >>=, &=, ^=, |=, ","
// assign = logical_expression ("=" assign)?
//        | logical_expression ( "+=" | "-=" | "*=" | "/=" | "%=" ) logical_expression
static Node *assign()
{
    Node *node = logical_expression();
    if (consume('='))
    {
        node = new_assign(node, assign());
    }
    else if (consume(TK_ADD_EQ))
    {
        node = new_assign(node, new_add(node, logical_expression()));
    }
    else if (consume(TK_SUB_EQ))
    {
        node = new_assign(node, new_sub(node, logical_expression()));
    }
    else if (consume(TK_MUL_EQ))
    {
        node = new_assign(node, new_mul(node, logical_expression()));
    }
    else if (consume(TK_DIV_EQ))
    {
        node = new_assign(node, new_div(node, logical_expression()));
    }
    else if (consume(TK_MOD_EQ))
    {
        node = new_assign(node, new_mod(node, logical_expression()));
    }
    return node;
}

// logical_expression = equality ("&&" equality | "||" equality)*
static Node *logical_expression()
{
    Node *node = equality();

    for (;;)
    {
        if (consume(TK_LOGICAL_AND))
        {
            node = new_binop(ND_LOGICAL_AND, node, equality());
        }
        else if (consume(TK_LOGICAL_OR))
        {
            node = new_binop(ND_LOGICAL_OR, node, equality());
        }
        else
        {
            return node;
        }
    }
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume(TK_EQ))
        {
            node = new_binop(ND_EQ, node, relational());
        }
        else if (consume(TK_NE))
        {
            node = new_binop(ND_NE, node, relational());
        }
        else
        {
            return node;
        }
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume('<'))
        {
            node = new_binop(ND_LT, node, add());
        }
        else if (consume(TK_LE))
        {
            node = new_binop(ND_LE, node, add());
        }
        else if (consume('>'))
        {
            node = new_binop(ND_LT, add(), node);
        }
        else if (consume(TK_GE))
        {
            node = new_binop(ND_LE, add(), node);
        }
        else
        {
            return node;
        }
    }
}

// TODO: &, |, ^
// add = mul ("+" mul | "-" mul)*
static Node *add()
{
    Node *node = mul();

    for (;;)
    {
        if (consume('+'))
        {
            node = new_add(node, mul());
        }
        else if (consume('-'))
        {
            node = new_sub(node, mul());
        }
        else
        {
            return node;
        }
    }
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        if (consume('*'))
        {
            node = new_mul(node, unary());
        }
        else if (consume('/'))
        {
            node = new_div(node, unary());
        }
        else if (consume('%'))
        {
            node = new_mod(node, unary());
        }
        else
        {
            return node;
        }
    }
}

// TODO: !(否定),
/* unary = "+"? postfix
 *       | "-"? postfix
 *       | "*" postfix   ("*" unaryでもいい？)
 *       | "&" postfix
 *       | "sizeof" unary
 *       | ("++" | "--") postfix
 *       | "!" unary
 */
static Node *unary()
{
    if (consume('+'))
    {
        return postfix();
    }
    else if (consume('-'))
    {
        return new_sub(new_node_num(0), postfix());
    }
    else if (consume('*'))
    {
        Node *node = new_node(ND_DEREF);
        node->lhs = unary();
        add_type(node->lhs);
        add_type(node);
        return node;
    }
    else if (consume('&'))
    {
        Node *node = new_node(ND_ADDR);
        node->lhs = postfix();
        add_type(node->lhs);
        return node;
    }
    else if (consume('!'))
    {
        Node *node = new_node(ND_LOGICALNOT);
        node->lhs = unary();
        add_type(node->lhs);
        return node;
    }
    else if (consume(TK_SIZEOF))
    {
        Node *node = new_node_num(sizeOfNode(unary()));
        return node;
    }
    else if (consume(TK_INC))
    {
        Node *node = postfix();
        return new_assign(node, new_add(node, new_node_num(1)));
    }
    else if (consume(TK_DEC))
    {
        Node *node = postfix();
        return new_assign(node, new_sub(node, new_node_num(1)));
    }

    Node *node = postfix();
    if (consume(TK_INC))
    {
        // 先に+1して保存してから-1する
        return new_sub(new_assign(node, new_add(node, new_node_num(1))), new_node_num(1));
    }
    else if (consume(TK_DEC))
    {
        // 先に+-1して保存してから+1する
        return new_add(new_assign(node, new_sub(node, new_node_num(1))), new_node_num(1));
    }

    return node;
}

// postfix = primary  ( ("[" expr "]") | "." | "->" ) *
static Node *postfix()
{
    Node *node = primary();

    while (1)
    {
        if (consume_nostep(TK_ARROW) || consume_nostep('.'))
        {
            if (consume_nostep(TK_ARROW))
            {
                Node *n = new_node(ND_DEREF);
                n->lhs = node;
                add_type(n->lhs);
                add_type(n);
                node = n;
            }
            next_token();
            // 構造体のメンバーアクセス
            Token *tok = token;
            expect(TK_IDENT);
            add_type(node);
            if (node->type->kind != TYPE_STRUCT)
            {
                error("postfix() failure: struct型ではありません。");
            }
            Type *t = find_struct_type(node->type->name);
            if (t == NULL)
            {
                error("find_struct_type() failure: %s構造体は定義されていません。", node->type->name);
            }
            Var *member = t->member;
            while (member)
            {
                if (!strcmp(member->name, my_strndup(tok->str, tok->len)))
                {
                    Node *n = new_node(ND_STRUCT_MEMBER);
                    // 変数をコピー
                    n->lhs = node;
                    n->val = member->offset - member->type->size;
                    n->type = member->type;
                    node = n;
                    break;
                }
                member = member->next;
            }

            if (member == NULL)
            {
                error("primary() failure: %s構造体が定義されていません。", my_strndup(tok->str, tok->len));
            }

            continue;
        }

        if (consume('['))
        {
            Node *deref = new_node(ND_DEREF);
            deref->lhs = new_add(node, expr());
            add_type(deref->lhs);
            add_type(deref);
            expect(']');
            node = deref;
            continue;
        }

        break;
    }

    return node;
}

// funcall = "(" (expr ("," expr)*)? ")"
static Node *funcall(Token *tok)
{
    expect('(');
    Node *node = new_node(ND_CALL);
    node->fn_name = my_strndup(tok->str, tok->len);
    node->args = new_vec();
    while (!consume(')'))
    {
        if (node->args->len != 0)
        {
            expect(',');
        }
        vec_push(node->args, expr());
    }
    return node;
}

// primary = "(" expr ")" | num | string | ident funcall?
static Node *primary()
{
    if (consume('('))
    {
        Node *node = expr();
        expect(')');
        return node;
    }

    if (consume_nostep(TK_IDENT))
    {
        Token *tok = token;
        next_token();
        Node *node;
        if (consume_nostep('('))
        {
            node = funcall(tok);
        }
        else
        {
            node = get_node_ident(tok);
        }
        return node;
    }

    if (consume_nostep(TK_STRING))
    {
        Node *node = new_node(ND_STRING);
        node->str_literal = token->str;
        node->val = token->str_literal_index;
        next_token();
        return node;
    }

    if (consume_nostep(TK_NUM))
    {
        return new_node_num(expect_number());
    }

    if (token->kind == TK_EOF)
    {
        // TK_EOFはtoken->strが入力を超える位置になる
        error("primary() failure: 不正なコードです。");
    }
    error_at(token->str, "primary() failure: 不正なトークンです。");
}