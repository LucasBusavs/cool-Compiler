# M1 — Core Scanner Specification

## Responsabilidade

Especificar os elementos léxicos reconhecidos principalmente no estado `INITIAL`.

## Escopo

- keywords;
- constantes booleanas;
- constantes inteiras;
- `TYPEID`;
- `OBJECTID`;
- operadores;
- símbolos e pontuação;
- whitespace;
- newline fora de estados especiais;
- caracteres inválidos;
- valores semânticos associados aos tokens.

## Fontes

- COOL Reference Manual — Seções 10.1, 10.4 e 10.5
- Figura 1 do COOL Reference Manual
- Enunciado do TP02
- `cool-parse.h`
- `PA2/cool.flex`

## Requisitos

### CORE-KW-001 — Keywords gerais

**Origem:** MANUAL, INFRA

As seguintes palavras são keywords da linguagem COOL:

- `class` → `CLASS`
- `else` → `ELSE`
- `fi` → `FI`
- `if` → `IF`
- `in` → `IN`
- `inherits` → `INHERITS`
- `isvoid` → `ISVOID`
- `let` → `LET`
- `loop` → `LOOP`
- `pool` → `POOL`
- `then` → `THEN`
- `while` → `WHILE`
- `case` → `CASE`
- `esac` → `ESAC`
- `new` → `NEW`
- `of` → `OF`
- `not` → `NOT`

As keywords acima são case-insensitive.

Portanto, por exemplo:

- `class`
- `CLASS`
- `Class`
- `cLaSs`

devem produzir o mesmo token `CLASS`.

Esses tokens não possuem valor semântico adicional em `cool_yylval`.

Estado: `INITIAL`.

Não alteram `curr_lineno`.

---

### CORE-BOOL-001 — Constante booleana true

**Origem:** MANUAL, INFRA

A constante booleana `true` deve começar obrigatoriamente com `t` minúsculo.

Os demais caracteres podem ser maiúsculos ou minúsculos.

Exemplos reconhecidos como booleano:

- `true`
- `tRuE`
- `trUE`

Exemplos que não devem ser reconhecidos como booleano:

- `True`
- `TRUE`

Token retornado:

`BOOL_CONST`

Valor semântico:

`cool_yylval.boolean = true`

Estado: `INITIAL`.

---

### CORE-BOOL-002 — Constante booleana false

**Origem:** MANUAL, INFRA

A constante booleana `false` deve começar obrigatoriamente com `f` minúsculo.

Os demais caracteres podem ser maiúsculos ou minúsculos.

Exemplos reconhecidos como booleano:

- `false`
- `fAlSe`
- `falSE`

Exemplos que não devem ser reconhecidos como booleano:

- `False`
- `FALSE`

Token retornado:

`BOOL_CONST`

Valor semântico:

`cool_yylval.boolean = false`

Estado: `INITIAL`.

---

### CORE-ID-001 — TYPEID

**Origem:** MANUAL, INFRA

Um identificador é formado por letras, dígitos e `_`.

Um identificador de tipo começa com letra maiúscula.

Exemplos:

- `Main`
- `Object`
- `MinhaClasse`
- `A1`
- `Tipo_2`

Token retornado:

`TYPEID`

O lexema deve ser armazenado na tabela de identificadores.

O símbolo correspondente deve ser colocado em:

`cool_yylval.symbol`

Estado: `INITIAL`.

---

### CORE-ID-002 — OBJECTID

**Origem:** MANUAL, INFRA

Um identificador de objeto começa com letra minúscula.

Após o primeiro caractere podem aparecer letras, dígitos e `_`.

Exemplos:

- `x`
- `main`
- `valor1`
- `minha_variavel`

Token retornado:

`OBJECTID`

O lexema deve ser armazenado na tabela de identificadores.

O símbolo correspondente deve ser colocado em:

`cool_yylval.symbol`

Estado: `INITIAL`.

---

### CORE-ID-003 — self e SELF_TYPE

**Origem:** MANUAL, TP02

`self` e `SELF_TYPE` possuem significado especial na linguagem COOL, mas não são keywords do scanner.

No analisador léxico devem seguir as regras normais de identificadores:

- `self` → `OBJECTID`
- `SELF_TYPE` → `TYPEID`

As restrições semânticas associadas a esses identificadores serão tratadas em fases posteriores do compilador.

---

### CORE-INT-001 — Constante inteira

**Origem:** MANUAL, INFRA, TP02

Uma constante inteira é uma sequência não vazia de dígitos entre `0` e `9`.

Exemplos válidos:

- `0`
- `1`
- `123`
- `007`
- `999999`

Token retornado:

`INT_CONST`

O lexema deve ser inserido na tabela de inteiros.

O símbolo correspondente deve ser colocado em:

`cool_yylval.symbol`

Estado: `INITIAL`.

Não altera `curr_lineno`.

O scanner não deve verificar overflow de constantes inteiras.

---

### CORE-OP-001 — Operador de atribuição

**Origem:** MANUAL, INFRA

Lexema:

`<-`

Token retornado:

`ASSIGN`

Estado: `INITIAL`.

---

### CORE-OP-002 — Operador menor ou igual

**Origem:** MANUAL, INFRA

Lexema:

`<=`

Token retornado:

`LE`

Estado: `INITIAL`.

---

### CORE-OP-003 — Seta de case

**Origem:** MANUAL, INFRA

Lexema:

`=>`

Token retornado:

`DARROW`

Estado: `INITIAL`.

---

### CORE-SYM-001 — Símbolos de um caractere

**Origem:** MANUAL, INFRA

Os símbolos sintáticos de um caractere não possuem tokens nomeados próprios no `cool-parse.h`.

Devem ser retornados diretamente pelo seu valor de caractere.

O conjunto relevante inclui:

- `+`
- `-`
- `*`
- `/`
- `~`
- `<`
- `=`
- `(`
- `)`
- `{`
- `}`
- `;`
- `:`
- `,`
- `.`
- `@`

Estado: `INITIAL`.

Não possuem valor semântico adicional em `cool_yylval`.

---

### CORE-ORDER-001 — Prioridade de operadores compostos

**Origem:** FLEX, DECISION

Operadores compostos devem ser reconhecidos antes que seus caracteres individuais sejam tratados isoladamente.

Casos relevantes:

- `<-` antes de `<`
- `<=` antes de `<`
- `=>` antes de `=`

Além disso, o comportamento de longest-match do Flex deve ser considerado.

---

### CORE-ORDER-002 — Prioridade entre keywords e identificadores

**Origem:** FLEX, DECISION

Keywords e constantes booleanas devem ter precedência adequada sobre as regras gerais de identificadores.

Exemplo:

`class`

deve produzir:

`CLASS`

e não:

`OBJECTID`

Por outro lado:

`classes`

não é keyword e deve ser reconhecido como:

`OBJECTID`

A implementação deve respeitar o longest-match do Flex e a ordem das regras em caso de empate.

---

### CORE-WS-001 — Whitespace

**Origem:** MANUAL

Os seguintes caracteres são whitespace:

- espaço, ASCII 32;
- newline, ASCII 10;
- form feed, ASCII 12;
- carriage return, ASCII 13;
- tab, ASCII 9;
- vertical tab, ASCII 11.

Whitespace não gera tokens.

---

### CORE-LINE-001 — Controle de linha em INITIAL

**Origem:** TP02, INFRA

Quando um newline é reconhecido no estado `INITIAL`, o scanner deve incrementar:

`curr_lineno`

em uma unidade.

Os demais caracteres de whitespace não alteram `curr_lineno`.

O tratamento de newline dentro de strings e comentários será especificado nos documentos correspondentes.

---

### CORE-ERR-001 — Caractere inválido

**Origem:** TP02

Se um caractere não pertencer a nenhuma construção léxica válida da linguagem, o scanner deve retornar:

`ERROR`

O campo:

`cool_yylval.error_msg`

deve representar o próprio caractere inválido.

Após reportar o erro, o scanner deve continuar a análise a partir do caractere seguinte.

Exemplos de entradas a testar:

- `#`
- `$`
- `?`

quando não fizerem parte de nenhuma construção válida.

---

## Valores semânticos do Core

O contrato entre scanner e parser fica definido da seguinte forma:

| Categoria | Token | Campo de `cool_yylval` |
|---|---|---|
| Inteiro | `INT_CONST` | `symbol` |
| TYPEID | `TYPEID` | `symbol` |
| OBJECTID | `OBJECTID` | `symbol` |
| Booleano | `BOOL_CONST` | `boolean` |
| Erro | `ERROR` | `error_msg` |
| Keyword | token correspondente | nenhum |
| Operador nomeado | token correspondente | nenhum |
| Símbolo simples | caractere | nenhum |

---

## Casos de teste previstos

### Keywords

Testar:

- todas as keywords;
- versões em maiúsculas;
- versões com mistura de caixa;
- prefixos de keywords que fazem parte de identificadores.

Exemplos:

- `class`
- `CLASS`
- `ClAsS`
- `classes`

---

### Booleanos

Testar:

- `true`
- `tRuE`
- `false`
- `fAlSe`
- `True`
- `False`
- `TRUE`
- `FALSE`

---

### Identificadores

Testar:

- `Main`
- `MinhaClasse`
- `x`
- `valor`
- `valor1`
- `valor_teste`
- `self`
- `SELF_TYPE`

---

### Inteiros

Testar:

- `0`
- `1`
- `007`
- `123`
- sequência longa de dígitos

---

### Operadores

Testar individualmente:

- `<-`
- `<=`
- `=>`
- `<`
- `=`
- `+`
- `-`
- `*`
- `/`
- `~`

Também testar operadores adjacentes a identificadores e números.

---

### Whitespace

Testar combinações de:

- espaços;
- tabs;
- newlines;
- carriage return;
- form feed;
- vertical tab.

Verificar especialmente a atualização correta de `curr_lineno`.

---

### Caracteres inválidos

Testar caracteres que não pertencem à linguagem e verificar:

- retorno de `ERROR`;
- conteúdo de `cool_yylval.error_msg`;
- continuidade da análise no próximo caractere.

---

## Questões resolvidas nesta especificação

- keywords gerais são case-insensitive;
- `true` e `false` exigem primeira letra minúscula;
- `TYPEID` começa com maiúscula;
- `OBJECTID` começa com minúscula;
- `self` é tratado lexicalmente como `OBJECTID`;
- `SELF_TYPE` é tratado lexicalmente como `TYPEID`;
- inteiros não possuem verificação de overflow no scanner;
- operadores compostos possuem tokens nomeados;
- símbolos simples são retornados diretamente;
- newline em `INITIAL` incrementa `curr_lineno`;
- caractere inválido retorna `ERROR`.

## Questões fora do escopo deste documento

Este documento não especifica:

- strings;
- comentários;
- erros internos de strings;
- EOF em comentários;
- recuperação de strings;
- nesting de comentários.

Esses comportamentos são tratados em `spec-strings.md` e `spec-comments.md`.
