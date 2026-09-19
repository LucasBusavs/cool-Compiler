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

A preencher durante o M1.

Cada requisito deverá informar:

- ID;
- origem;
- padrão reconhecido;
- token retornado;
- valor em `cool_yylval`, quando aplicável;
- impacto em `curr_lineno`;
- prioridade ou ordem da regra;
- caso de teste correspondente.

## Pontos a especificar

### Keywords

Determinar:

- lista completa;
- sensibilidade a maiúsculas e minúsculas;
- token correspondente.

### Booleanos

Determinar:

- regras de reconhecimento de `true`;
- regras de reconhecimento de `false`;
- preenchimento de `cool_yylval.boolean`.

### Identificadores

Determinar:

- regra de `TYPEID`;
- regra de `OBJECTID`;
- armazenamento em tabela de símbolos;
- tratamento de `self`;
- tratamento de `SELF_TYPE`.

### Inteiros

Determinar:

- padrão lexical;
- armazenamento em tabela;
- token retornado.

### Operadores e símbolos

Determinar:

- operadores de múltiplos caracteres;
- símbolos retornados diretamente como caracteres;
- ordem das regras quando necessária.

### Whitespace e linhas

Determinar:

- caracteres ignorados;
- comportamento de newline;
- atualização de `curr_lineno`.

### Erros

Determinar o comportamento para caracteres que não pertencem a nenhuma construção léxica válida.

## Casos de teste

A preencher durante o M1.
