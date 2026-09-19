# M1 — Requisitos do Analisador Léxico

Este documento consolida os requisitos comportamentais do analisador léxico COOL.

As descrições detalhadas estão em:

- `spec-core.md`;
- `spec-strings.md`;
- `spec-comments.md`.

A estratégia de validação está em:

- `TEST_PLAN.md`.

---

## Fontes

- COOL Reference Manual — Seção 10 e Figura 1;
- Enunciado do TP02;
- `PA2/cool.flex`;
- `PA2/README`;
- `cool-parse.h` da infraestrutura oficial.

---

## Convenção de origem

- `MANUAL`: comportamento definido pela linguagem COOL;
- `TP02`: requisito específico do trabalho;
- `INFRA`: requisito imposto pela infraestrutura fornecida;
- `FLEX`: comportamento relevante da ferramenta Flex;
- `DECISION`: decisão de projeto tomada durante o M1.

---

# Core

| ID | Requisito | Token/Ação | Valor semântico | Teste |
|---|---|---|---|---|
| CORE-KW-001 | Reconhecer keywords gerais de forma case-insensitive | token correspondente | nenhum | TEST-CORE-001 |
| CORE-BOOL-001 | Reconhecer `true` com primeira letra minúscula | `BOOL_CONST` | `boolean = true` | TEST-CORE-002 |
| CORE-BOOL-002 | Reconhecer `false` com primeira letra minúscula | `BOOL_CONST` | `boolean = false` | TEST-CORE-002 |
| CORE-ID-001 | Reconhecer identificadores iniciados por maiúscula | `TYPEID` | `symbol` | TEST-CORE-003 |
| CORE-ID-002 | Reconhecer identificadores iniciados por minúscula | `OBJECTID` | `symbol` | TEST-CORE-003 |
| CORE-ID-003 | Tratar `self` e `SELF_TYPE` como identificadores normais no lexer | `OBJECTID` / `TYPEID` | `symbol` | TEST-CORE-003 |
| CORE-INT-001 | Reconhecer sequência não vazia de dígitos | `INT_CONST` | `symbol` | TEST-CORE-004 |
| CORE-OP-001 | Reconhecer `<-` | `ASSIGN` | nenhum | TEST-CORE-005 |
| CORE-OP-002 | Reconhecer `<=` | `LE` | nenhum | TEST-CORE-005 |
| CORE-OP-003 | Reconhecer `=>` | `DARROW` | nenhum | TEST-CORE-005 |
| CORE-SYM-001 | Reconhecer símbolos simples da linguagem | caractere correspondente | nenhum | TEST-CORE-006 |
| CORE-ORDER-001 | Preservar reconhecimento correto de operadores compostos | aplicar longest-match/prioridade adequada | nenhum | TEST-CORE-005 |
| CORE-ORDER-002 | Resolver corretamente keywords/booleanos versus identificadores | token adequado | conforme categoria | TEST-CORE-001 / TEST-CORE-002 |
| CORE-WS-001 | Ignorar whitespace | nenhum token | nenhum | TEST-CORE-007 |
| CORE-LINE-001 | Incrementar linha em newline no estado normal | `curr_lineno++` | nenhum | TEST-CORE-007 / TEST-INT-004 |
| CORE-ERR-001 | Reportar caractere lexicalmente inválido | `ERROR` | `error_msg` com o caractere | TEST-CORE-008 / TEST-INT-003 |

Total Core: **16 requisitos**.

---

# Strings

| ID | Requisito | Token/Ação | Valor semântico / Erro | Teste |
|---|---|---|---|---|
| STR-001 | Iniciar processamento ao encontrar `"` | entrar no contexto de string | inicializar buffer | TEST-STR-001 |
| STR-002 | Encerrar string válida em `"` não escapada | `STR_CONST` | `symbol` | TEST-STR-001 |
| STR-003 | Acumular caracteres normais | adicionar ao buffer | conteúdo da string | TEST-STR-001 |
| STR-004 | Converter `\b` | adicionar backspace | conteúdo convertido | TEST-STR-002 |
| STR-005 | Converter `\t` | adicionar tab | conteúdo convertido | TEST-STR-002 |
| STR-006 | Converter `\n` textual | adicionar newline | conteúdo convertido | TEST-STR-002 |
| STR-007 | Converter `\f` | adicionar form feed | conteúdo convertido | TEST-STR-002 |
| STR-008 | Aceitar escape genérico `\c` | adicionar `c` | conteúdo convertido | TEST-STR-003 / TEST-STR-007 |
| STR-009 | Aceitar newline física escapada | manter string aberta e incrementar linha | conteúdo + `curr_lineno` | TEST-STR-004 |
| STR-010 | Rejeitar newline física não escapada | `ERROR` | `Unterminated string constant` | TEST-STR-005 |
| STR-011 | Rejeitar caractere NUL real | `ERROR` + recuperação | `String contains null character` | TEST-STR-006 |
| STR-012 | Limitar valor final a 1024 caracteres | `STR_CONST` ou `ERROR` | `String constant too long` | TEST-STR-008 / TEST-STR-009 |
| STR-013 | Rejeitar EOF em string aberta | `ERROR` | `EOF in string constant` | TEST-STR-010 |
| STR-014 | Armazenar string válida na tabela de strings | `STR_CONST` | `cool_yylval.symbol` | TEST-STR-001 / TEST-STR-002 |
| STR-015 | Manter `curr_lineno` correto durante strings | atualizar em newlines físicas | `curr_lineno` | TEST-STR-004 / TEST-STR-005 / TEST-INT-004 |
| STR-016 | Recuperar string inválida até seu final lógico | descartar restante da constante | retornar a `INITIAL` posteriormente | TEST-STR-006 / TEST-STR-011 / TEST-STR-012 |
| STR-017 | Evitar erros em cascata para a mesma string inválida | não tokenizar restante da constante | um erro léxico correspondente | TEST-STR-006 / TEST-STR-011 / TEST-STR-012 |

Total Strings: **17 requisitos**.

---

# Comentários

| ID | Requisito | Token/Ação | Valor semântico / Erro | Teste |
|---|---|---|---|---|
| COM-001 | Reconhecer `--` como início de comentário de linha | ignorar | nenhum | TEST-COM-001 |
| COM-002 | Ignorar conteúdo do comentário de linha | nenhum token | nenhum | TEST-COM-001 |
| COM-003 | Encerrar comentário de linha em newline | incrementar linha e voltar ao fluxo normal | `curr_lineno` | TEST-COM-001 |
| COM-004 | Encerrar comentário de linha normalmente em EOF | fim normal | nenhum erro | TEST-COM-002 |
| COM-005 | Reconhecer `(*` como início de comentário de bloco | entrar no contexto de comentário | profundidade inicial 1 | TEST-COM-003 |
| COM-006 | Permitir comentários de bloco aninhados | incrementar profundidade | nenhum | TEST-COM-004 / TEST-COM-007 |
| COM-007 | Reconhecer `*)` dentro de comentário | decrementar profundidade | sair quando profundidade chegar a zero | TEST-COM-003 / TEST-COM-004 |
| COM-008 | Ignorar conteúdo normal de comentário de bloco | nenhum token | nenhum | TEST-COM-003 |
| COM-009 | Atualizar linha dentro de comentário de bloco | `curr_lineno++` | nenhum | TEST-COM-005 / TEST-INT-004 |
| COM-010 | Reportar EOF dentro de comentário de bloco | `ERROR` | `EOF in comment` | TEST-COM-006 / TEST-COM-007 |
| COM-011 | Reportar `*)` sem comentário aberto | `ERROR` | `Unmatched *)` | TEST-COM-008 |
| COM-012 | Respeitar contexto lexical de delimitadores | interpretar conforme estado atual | conforme contexto | TEST-COM-009 / TEST-INT-002 |
| COM-013 | Priorizar delimitadores completos de comentário | reconhecer `--`, `(*` e `*)` corretamente | conforme contexto | TEST-COM-008 / TEST-COM-010 |

Total Comentários: **13 requisitos**.

---

# Resumo quantitativo

| Domínio | Quantidade |
|---|---:|
| Core | 16 |
| Strings | 17 |
| Comentários | 13 |
| **Total** | **46** |

---

# Tokens nomeados envolvidos

A infraestrutura define os seguintes tokens nomeados relevantes ao scanner:

- `CLASS`;
- `ELSE`;
- `FI`;
- `IF`;
- `IN`;
- `INHERITS`;
- `LET`;
- `LOOP`;
- `POOL`;
- `THEN`;
- `WHILE`;
- `CASE`;
- `ESAC`;
- `OF`;
- `DARROW`;
- `NEW`;
- `ISVOID`;
- `STR_CONST`;
- `INT_CONST`;
- `BOOL_CONST`;
- `TYPEID`;
- `OBJECTID`;
- `ASSIGN`;
- `NOT`;
- `LE`;
- `ERROR`.

`LET_STMT`, embora definido em `cool-parse.h`, não é tratado como token produzido pelo scanner nesta especificação.

Símbolos sintáticos simples são retornados diretamente pelo valor do caractere correspondente.

---

# Contrato com `cool_yylval`

| Token | Campo utilizado |
|---|---|
| `TYPEID` | `cool_yylval.symbol` |
| `OBJECTID` | `cool_yylval.symbol` |
| `INT_CONST` | `cool_yylval.symbol` |
| `STR_CONST` | `cool_yylval.symbol` |
| `BOOL_CONST` | `cool_yylval.boolean` |
| `ERROR` | `cool_yylval.error_msg` |

Keywords, operadores nomeados e símbolos simples não necessitam de valor semântico adicional no scanner.

---

# Erros léxicos obrigatórios

| Condição | Token | Mensagem |
|---|---|---|
| Caractere inválido | `ERROR` | próprio caractere |
| Newline não escapada em string | `ERROR` | `Unterminated string constant` |
| String longa demais | `ERROR` | `String constant too long` |
| NUL real em string | `ERROR` | `String contains null character` |
| EOF em string | `ERROR` | `EOF in string constant` |
| EOF em comentário de bloco | `ERROR` | `EOF in comment` |
| `*)` fora de comentário | `ERROR` | `Unmatched *)` |

O scanner não deve imprimir mensagens de erro diretamente.

Os erros devem ser comunicados às fases seguintes por meio do token `ERROR` e de `cool_yylval.error_msg`.

---

# Regras globais de linha

`curr_lineno` deve representar a linha corrente do arquivo-fonte.

Deve ser incrementado para newlines físicas consumidas em:

- estado normal;
- comentário de linha;
- comentário de bloco;
- string com newline escapada;
- tratamento de string encerrada por newline não escapada;
- recuperação de string quando uma newline física for consumida.

A sequência textual `\n` dentro de uma string não representa mudança física de linha do arquivo e não altera `curr_lineno`.

---

# Regras globais de recuperação

Após erro recuperável, o scanner deve continuar de maneira consistente.

Casos principais:

- caractere inválido: continuar no caractere seguinte;
- newline não escapada em string: continuar no início da próxima linha;
- `*)` sem abertura: continuar após os dois caracteres;
- NUL em string: descartar o restante da constante até o final lógico;
- string longa demais: descartar o restante da constante até o final lógico.

EOF em string e EOF em comentário não possuem conteúdo posterior para recuperação.

---

# Rastreabilidade

Um requisito só poderá ser considerado implementado quando:

1. estiver descrito na especificação correspondente;
2. tiver comportamento compatível com este documento;
3. possuir pelo menos um teste associado em `TEST_PLAN.md`;
4. estiver implementado em `PA2/cool.flex`;
5. o teste correspondente passar;
6. não introduzir regressões nos demais testes.

---

# Status do M1

Após a conclusão deste documento:

- especificação de Core: concluída;
- especificação de Strings: concluída;
- especificação de Comentários: concluída;
- plano de testes: concluído;
- matriz de requisitos: concluída;
- arquitetura de implementação: pendente.

O próximo documento a ser finalizado é `ARCHITECTURE.md`.
