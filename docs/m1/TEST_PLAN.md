# M1 — Plano de Testes do Analisador Léxico

## Objetivo

Definir uma estratégia de testes capaz de validar todos os requisitos do analisador léxico COOL especificados em:

- `spec-core.md`;
- `spec-strings.md`;
- `spec-comments.md`.

O plano deve permitir verificar:

- reconhecimento correto de tokens;
- valores semânticos associados aos tokens;
- controle de linhas;
- tratamento de erros;
- recuperação após erros;
- interação entre diferentes construções léxicas.

O arquivo `PA2/test.cl` fornecido pela infraestrutura não cobre todos os casos necessários. Durante o desenvolvimento serão utilizados arquivos de teste separados e, ao final, os casos mais relevantes serão consolidados em `PA2/test.cl`.

---

## Estratégia geral

Cada requisito léxico deve estar associado a pelo menos um caso de teste.

Sempre que aplicável, serão considerados:

- caso válido;
- caso de borda;
- caso inválido;
- recuperação após erro;
- continuidade da tokenização;
- controle de `curr_lineno`.

Os testes serão inicialmente separados por domínio para facilitar diagnóstico de falhas.

Após a validação individual dos domínios, serão executados testes de integração combinando múltiplas construções no mesmo arquivo.

---

## Estrutura prevista

Os testes auxiliares de desenvolvimento serão organizados conceitualmente da seguinte forma:

    tests/
    ├── core/
    │   ├── keywords.cl
    │   ├── booleans.cl
    │   ├── identifiers.cl
    │   ├── integers.cl
    │   ├── operators.cl
    │   ├── whitespace.cl
    │   └── invalid_chars.cl
    │
    ├── strings/
    │   ├── valid.cl
    │   ├── escapes.cl
    │   ├── newlines.cl
    │   ├── null.cl
    │   ├── length.cl
    │   ├── eof.cl
    │   └── recovery.cl
    │
    ├── comments/
    │   ├── line.cl
    │   ├── block.cl
    │   ├── nested.cl
    │   ├── eof.cl
    │   ├── unmatched.cl
    │   └── line_numbers.cl
    │
    └── integration/
        ├── mixed_tokens.cl
        ├── contexts.cl
        ├── recovery.cl
        ├── line_numbers.cl
        └── full_program.cl

Essa estrutura é auxiliar ao desenvolvimento.

A entrega oficial continuará utilizando os arquivos exigidos pelo TP02, especialmente `PA2/test.cl`.

---

# Testes de Core

## TEST-CORE-001 — Keywords

**Requisitos cobertos:**

- `CORE-KW-001`;
- `CORE-ORDER-002`.

**Arquivo previsto:**

`tests/core/keywords.cl`

**Objetivo:**

Validar todas as keywords COOL e a regra de case-insensitivity.

**Casos mínimos:**

Testar todas as keywords:

- `class`;
- `else`;
- `fi`;
- `if`;
- `in`;
- `inherits`;
- `isvoid`;
- `let`;
- `loop`;
- `pool`;
- `then`;
- `while`;
- `case`;
- `esac`;
- `new`;
- `of`;
- `not`.

Para algumas keywords, testar também:

- somente minúsculas;
- somente maiúsculas;
- mistura de maiúsculas e minúsculas.

Exemplo:

`class CLASS Class cLaSs`

Todos devem produzir:

`CLASS`

Também devem ser testados identificadores que possuem uma keyword como prefixo.

Exemplo:

`classes`

Resultado esperado:

`OBJECTID`

e não `CLASS` seguido de outro token.

---

## TEST-CORE-002 — Booleanos

**Requisitos cobertos:**

- `CORE-BOOL-001`;
- `CORE-BOOL-002`;
- `CORE-ORDER-002`.

**Arquivo previsto:**

`tests/core/booleans.cl`

**Entradas relevantes:**

- `true`;
- `tRuE`;
- `trUE`;
- `false`;
- `fAlSe`;
- `falSE`;
- `True`;
- `TRUE`;
- `False`;
- `FALSE`.

**Resultado esperado:**

As formas iniciadas por `t` minúsculo equivalentes a `true` devem gerar:

`BOOL_CONST`

com:

`cool_yylval.boolean = true`

As formas iniciadas por `f` minúsculo equivalentes a `false` devem gerar:

`BOOL_CONST`

com:

`cool_yylval.boolean = false`

Formas iniciadas por maiúscula devem obedecer às regras normais de identificadores e não serem reconhecidas como constantes booleanas.

---

## TEST-CORE-003 — Identificadores

**Requisitos cobertos:**

- `CORE-ID-001`;
- `CORE-ID-002`;
- `CORE-ID-003`.

**Arquivo previsto:**

`tests/core/identifiers.cl`

**Entradas relevantes:**

- `Main`;
- `Object`;
- `MinhaClasse`;
- `A1`;
- `Tipo_2`;
- `x`;
- `main`;
- `valor1`;
- `minha_variavel`;
- `self`;
- `SELF_TYPE`.

**Resultado esperado:**

Identificadores iniciados por maiúscula:

`TYPEID`

Identificadores iniciados por minúscula:

`OBJECTID`

Casos especiais:

- `self` → `OBJECTID`;
- `SELF_TYPE` → `TYPEID`.

Para tokens de identificadores deve existir um `Symbol` correspondente em:

`cool_yylval.symbol`

---

## TEST-CORE-004 — Constantes inteiras

**Requisito coberto:**

`CORE-INT-001`

**Arquivo previsto:**

`tests/core/integers.cl`

**Entradas relevantes:**

- `0`;
- `1`;
- `007`;
- `123`;
- `999999`;
- sequência grande de dígitos.

**Resultado esperado:**

Cada sequência completa deve gerar um único:

`INT_CONST`

com o símbolo correspondente em:

`cool_yylval.symbol`

Zeros à esquerda não devem produzir erro léxico.

O lexer não deve realizar validação de overflow.

---

## TEST-CORE-005 — Operadores compostos

**Requisitos cobertos:**

- `CORE-OP-001`;
- `CORE-OP-002`;
- `CORE-OP-003`;
- `CORE-ORDER-001`.

**Arquivo previsto:**

`tests/core/operators.cl`

**Entradas:**

`<-`

Resultado:

`ASSIGN`

Entrada:

`<=`

Resultado:

`LE`

Entrada:

`=>`

Resultado:

`DARROW`

Os operadores compostos devem ser reconhecidos como tokens únicos.

---

## TEST-CORE-006 — Símbolos simples

**Requisito coberto:**

`CORE-SYM-001`

**Arquivo previsto:**

`tests/core/operators.cl`

**Símbolos a testar:**

- `+`;
- `-`;
- `*`;
- `/`;
- `~`;
- `<`;
- `=`;
- `(`;
- `)`;
- `{`;
- `}`;
- `;`;
- `:`;
- `,`;
- `.`;
- `@`.

**Resultado esperado:**

Cada símbolo deve ser retornado diretamente como token de caractere.

Também devem ser testadas combinações entre operadores simples e compostos.

---

## TEST-CORE-007 — Whitespace e linhas

**Requisitos cobertos:**

- `CORE-WS-001`;
- `CORE-LINE-001`.

**Arquivo previsto:**

`tests/core/whitespace.cl`

**Casos a testar:**

- espaço;
- tab;
- newline;
- form feed;
- carriage return;
- vertical tab;
- combinações dos caracteres anteriores.

**Resultado esperado:**

Whitespace não deve produzir tokens.

Cada newline física no estado normal deve incrementar `curr_lineno` exatamente uma vez.

Os outros caracteres de whitespace não devem incrementar `curr_lineno`.

---

## TEST-CORE-008 — Caracteres inválidos

**Requisito coberto:**

`CORE-ERR-001`

**Arquivo previsto:**

`tests/core/invalid_chars.cl`

**Casos a testar:**

Caracteres que não pertençam à linguagem, quando encontrados fora de outros contextos.

**Resultado esperado:**

Para cada caractere inválido:

`ERROR`

deve ser retornado.

`cool_yylval.error_msg`

deve representar o próprio caractere inválido.

Após o erro, o scanner deve continuar a partir do próximo caractere.

O arquivo deve conter um token válido logo após o caractere inválido para verificar a recuperação.

---

# Testes de Strings

## TEST-STR-001 — Strings válidas básicas

**Requisitos cobertos:**

- `STR-001`;
- `STR-002`;
- `STR-003`;
- `STR-014`.

**Arquivo previsto:**

`tests/strings/valid.cl`

**Casos a testar:**

- string vazia;
- string com um caractere;
- string com palavra;
- string com espaços;
- string com números;
- string com pontuação;
- várias strings válidas consecutivas.

**Resultado esperado:**

Cada constante deve produzir:

`STR_CONST`

O valor em:

`cool_yylval.symbol`

deve conter o conteúdo da string sem as aspas delimitadoras.

---

## TEST-STR-002 — Escapes especiais

**Requisitos cobertos:**

- `STR-004`;
- `STR-005`;
- `STR-006`;
- `STR-007`;
- `STR-014`.

**Arquivo previsto:**

`tests/strings/escapes.cl`

**Escapes a testar:**

- `\b`;
- `\t`;
- `\n`;
- `\f`.

**Resultado esperado:**

Cada sequência deve ser convertida para seu caractere correspondente antes de o valor ser armazenado na tabela de strings.

Cada escape deve ocupar um único caractere no valor resultante.

---

## TEST-STR-003 — Escape genérico

**Requisito coberto:**

`STR-008`

**Arquivo previsto:**

`tests/strings/escapes.cl`

**Casos a testar:**

- `\"`;
- `\\`;
- `\0`;
- outros exemplos de `\c`.

**Resultado esperado:**

O resultado de `\c`, fora dos quatro escapes especiais, deve ser apenas o caractere `c`.

Em particular:

`\0`

deve produzir o caractere textual:

`0`

e não um caractere nulo.

---

## TEST-STR-004 — Newline física escapada

**Requisitos cobertos:**

- `STR-009`;
- `STR-015`.

**Arquivo previsto:**

`tests/strings/newlines.cl`

**Objetivo:**

Criar uma constante string que continue fisicamente na linha seguinte utilizando barra invertida antes da newline.

**Resultado esperado:**

- a string permanece aberta;
- nenhum `ERROR` é produzido;
- `curr_lineno` é incrementado;
- o resultado final é `STR_CONST`.

---

## TEST-STR-005 — Newline não escapada

**Requisitos cobertos:**

- `STR-010`;
- `STR-015`.

**Arquivo previsto:**

`tests/strings/newlines.cl`

**Resultado esperado:**

Ao atingir uma newline não escapada dentro da string:

`ERROR`

deve ser retornado com:

`Unterminated string constant`

Além disso:

- `curr_lineno` deve ser incrementado;
- a string deve terminar;
- a análise deve continuar no início da próxima linha.

Um token válido deverá existir na linha seguinte para confirmar a recuperação.

---

## TEST-STR-006 — Caractere nulo real

**Requisitos cobertos:**

- `STR-011`;
- `STR-016`;
- `STR-017`.

**Arquivo previsto:**

`tests/strings/null.cl`

**Resultado esperado:**

Um caractere nulo real dentro da string deve produzir:

`ERROR`

com:

`String contains null character`

O restante da constante inválida deve ser descartado até seu final lógico.

O conteúdo restante dessa mesma string não deve ser interpretado como tokens independentes.

**Observação de preparação do teste:**

Um caractere NUL real pode ser difícil de inserir corretamente por um editor de texto comum.

Na fase de implementação, esse arquivo poderá ser gerado programaticamente para garantir que contenha efetivamente o byte NUL e não os dois caracteres `\` e `0`.

---

## TEST-STR-007 — Sequência textual `\0`

**Requisito coberto:**

`STR-008`

**Arquivo previsto:**

`tests/strings/null.cl`

**Objetivo:**

Distinguir:

- caractere NUL real;
- dois caracteres `\0`.

**Resultado esperado:**

A sequência textual:

`\0`

é válida.

Ela deve resultar no caractere:

`0`

dentro de `STR_CONST`.

---

## TEST-STR-008 — Limite de tamanho

**Requisitos cobertos:**

- `STR-012`;
- `STR-016`;
- `STR-017`.

**Arquivo previsto:**

`tests/strings/length.cl`

**Casos de fronteira:**

- valor final com 1023 caracteres;
- valor final com 1024 caracteres;
- valor final com 1025 caracteres;
- valor maior que 1025 caracteres.

**Resultado esperado:**

Até 1024 caracteres:

`STR_CONST`

Acima do limite:

`ERROR`

com:

`String constant too long`

Após o erro, o restante da constante deve ser descartado até seu final lógico.

---

## TEST-STR-009 — Tamanho após conversão de escapes

**Requisito coberto:**

`STR-012`

**Arquivo previsto:**

`tests/strings/length.cl`

**Objetivo:**

Confirmar que o limite considera o valor resultante da string e não simplesmente a quantidade de caracteres existentes no código-fonte.

Sequências como:

`\n`

ocupam dois caracteres no fonte, mas apenas um no valor final.

---

## TEST-STR-010 — EOF em string

**Requisito coberto:**

`STR-013`

**Arquivo previsto:**

`tests/strings/eof.cl`

**Entrada:**

Uma string iniciada por `"` sem aspa final antes do EOF.

**Resultado esperado:**

`ERROR`

com:

`EOF in string constant`

Nenhum `STR_CONST` deve ser produzido para o conteúdo parcial.

---

## TEST-STR-011 — Recuperação por fechamento

**Requisitos cobertos:**

- `STR-016`;
- `STR-017`.

**Arquivo previsto:**

`tests/strings/recovery.cl`

**Objetivo:**

Provocar um erro que exija descarte da string e possuir uma aspa de fechamento posteriormente.

**Resultado esperado:**

- um único erro referente à string inválida;
- consumo até a aspa final;
- retorno ao estado normal depois da aspa;
- tokenização correta do conteúdo válido posterior.

---

## TEST-STR-012 — Recuperação por newline

**Requisitos cobertos:**

- `STR-016`;
- `STR-017`;
- `STR-015`.

**Arquivo previsto:**

`tests/strings/recovery.cl`

**Objetivo:**

Provocar erro de conteúdo e terminar a constante inválida por newline não escapada.

**Resultado esperado:**

- o restante da string inválida é descartado;
- a newline encerra a recuperação;
- `curr_lineno` é incrementado;
- o scanner continua no início da próxima linha;
- conteúdo válido posterior é tokenizado normalmente.

---

# Testes de Comentários

## TEST-COM-001 — Comentário de linha

**Requisitos cobertos:**

- `COM-001`;
- `COM-002`;
- `COM-003`.

**Arquivo previsto:**

`tests/comments/line.cl`

**Casos a testar:**

- comentário vazio;
- comentário textual;
- comentário com keywords;
- comentário com números;
- comentário com operadores;
- comentário contendo aspas;
- comentário contendo `(*`.

**Resultado esperado:**

Nenhum conteúdo depois de `--` e antes da newline deve gerar tokens.

A newline deve incrementar `curr_lineno`.

---

## TEST-COM-002 — Comentário de linha em EOF

**Requisito coberto:**

`COM-004`

**Arquivo previsto:**

`tests/comments/line.cl`

**Objetivo:**

Criar comentário de linha como último conteúdo do arquivo, sem newline final.

**Resultado esperado:**

EOF encerra normalmente o comentário.

Nenhum `ERROR` deve ser produzido.

---

## TEST-COM-003 — Comentário de bloco simples

**Requisitos cobertos:**

- `COM-005`;
- `COM-007`;
- `COM-008`.

**Arquivo previsto:**

`tests/comments/block.cl`

**Casos a testar:**

- comentário vazio;
- comentário simples;
- comentário contendo keywords;
- comentário contendo números;
- comentário contendo operadores;
- comentário contendo `--`;
- abertura e fechamento na mesma linha.

**Resultado esperado:**

Todo o comentário deve ser ignorado.

Nenhum token interno deve ser produzido.

---

## TEST-COM-004 — Comentários aninhados

**Requisitos cobertos:**

- `COM-006`;
- `COM-007`.

**Arquivo previsto:**

`tests/comments/nested.cl`

**Casos a testar:**

- dois níveis;
- três níveis;
- múltiplas aberturas e fechamentos;
- texto antes e depois de comentários internos.

**Resultado esperado:**

O scanner somente deve voltar ao estado normal quando a profundidade chegar a zero.

---

## TEST-COM-005 — Newlines em comentário de bloco

**Requisito coberto:**

`COM-009`

**Arquivo previsto:**

`tests/comments/line_numbers.cl`

**Objetivo:**

Criar comentário de bloco com várias linhas.

**Resultado esperado:**

Cada newline física deve incrementar `curr_lineno`.

Um token colocado depois do comentário deverá apresentar linha coerente com todas as newlines consumidas.

---

## TEST-COM-006 — EOF em comentário simples

**Requisito coberto:**

`COM-010`

**Arquivo previsto:**

`tests/comments/eof.cl`

**Entrada:**

Comentário iniciado por:

`(*`

sem fechamento antes do EOF.

**Resultado esperado:**

`ERROR`

com:

`EOF in comment`

---

## TEST-COM-007 — EOF em comentário aninhado

**Requisitos cobertos:**

- `COM-006`;
- `COM-010`.

**Arquivo previsto:**

`tests/comments/eof.cl`

**Objetivo:**

Abrir múltiplos níveis de comentário e alcançar EOF antes de fechar todos eles.

**Resultado esperado:**

`ERROR`

com:

`EOF in comment`

---

## TEST-COM-008 — Fechamento sem abertura

**Requisitos cobertos:**

- `COM-011`;
- `COM-013`.

**Arquivo previsto:**

`tests/comments/unmatched.cl`

**Entrada relevante:**

`*)`

em estado normal.

**Resultado esperado:**

Um único:

`ERROR`

com:

`Unmatched *)`

A sequência não deve ser dividida nos tokens:

`*`

e

`)`

Um token válido posterior deve confirmar que a análise continua depois dos dois caracteres.

---

## TEST-COM-009 — Separação de contextos

**Requisito coberto:**

`COM-012`

**Arquivo previsto:**

`tests/integration/contexts.cl`

**Casos a testar:**

Strings contendo:

- `--`;
- `(*`;
- `*)`.

**Resultado esperado:**

Esses caracteres pertencem à string e não devem iniciar ou encerrar comentários.

Também devem existir comentários contendo caracteres que normalmente seriam interpretados como strings ou tokens.

O estado léxico deve determinar a interpretação correta.

---

## TEST-COM-010 — Prioridade dos delimitadores

**Requisito coberto:**

`COM-013`

**Arquivo previsto:**

`tests/comments/block.cl`

**Objetivo:**

Verificar que:

- `--` é reconhecido como início de comentário de linha;
- `(*` é reconhecido como início de comentário de bloco;
- `*)` fora de comentário é reconhecido como erro único.

---

# Testes de Integração

## TEST-INT-001 — Fluxo misto de tokens

**Arquivo previsto:**

`tests/integration/mixed_tokens.cl`

**Objetivo:**

Combinar em um mesmo arquivo:

- keywords;
- identificadores;
- inteiros;
- booleanos;
- operadores;
- símbolos;
- whitespace;
- strings;
- comentários.

O objetivo é verificar que regras corretas continuam sendo selecionadas quando diferentes categorias aparecem próximas umas das outras.

---

## TEST-INT-002 — Contextos léxicos

**Arquivo previsto:**

`tests/integration/contexts.cl`

**Objetivo:**

Verificar separação entre estados.

Casos importantes:

- delimitadores de comentários dentro de strings;
- aspas dentro de comentários;
- keywords dentro de comentários;
- operadores dentro de strings.

Nenhum estado deve permitir que regras pertencentes a outro contexto sejam aplicadas indevidamente.

---

## TEST-INT-003 — Recuperação após erros

**Arquivo previsto:**

`tests/integration/recovery.cl`

**Objetivo:**

Combinar erros léxicos com tokens válidos posteriores.

Casos relevantes:

- caractere inválido seguido por identificador;
- string unterminated seguida por código válido na linha seguinte;
- string com NUL seguida por código válido;
- string longa seguida por código válido;
- `Unmatched *)` seguido por código válido.

O objetivo é verificar que um erro não impede desnecessariamente a análise do restante do arquivo.

---

## TEST-INT-004 — Controle global de linhas

**Arquivo previsto:**

`tests/integration/line_numbers.cl`

**Objetivo:**

Combinar newlines presentes em:

- whitespace;
- comentários de linha;
- comentários de bloco;
- strings com newline escapada;
- strings inválidas com newline.

Depois de cada construção, tokens conhecidos devem permitir verificar se `curr_lineno` permanece correto.

---

## TEST-INT-005 — Programa COOL representativo

**Arquivo previsto:**

`tests/integration/full_program.cl`

**Objetivo:**

Construir um pequeno programa COOL sintaticamente representativo contendo diferentes categorias lexicais.

O objetivo não é validar parsing ou semântica neste momento.

O teste serve para verificar se o lexer consegue produzir uma sequência coerente de tokens para um arquivo COOL realista.

---

# Matriz de cobertura

## Core

| Requisito | Teste principal |
|---|---|
| CORE-KW-001 | TEST-CORE-001 |
| CORE-BOOL-001 | TEST-CORE-002 |
| CORE-BOOL-002 | TEST-CORE-002 |
| CORE-ID-001 | TEST-CORE-003 |
| CORE-ID-002 | TEST-CORE-003 |
| CORE-ID-003 | TEST-CORE-003 |
| CORE-INT-001 | TEST-CORE-004 |
| CORE-OP-001 | TEST-CORE-005 |
| CORE-OP-002 | TEST-CORE-005 |
| CORE-OP-003 | TEST-CORE-005 |
| CORE-SYM-001 | TEST-CORE-006 |
| CORE-ORDER-001 | TEST-CORE-005 |
| CORE-ORDER-002 | TEST-CORE-001 / TEST-CORE-002 |
| CORE-WS-001 | TEST-CORE-007 |
| CORE-LINE-001 | TEST-CORE-007 / TEST-INT-004 |
| CORE-ERR-001 | TEST-CORE-008 / TEST-INT-003 |

## Strings

| Requisito | Teste principal |
|---|---|
| STR-001 | TEST-STR-001 |
| STR-002 | TEST-STR-001 |
| STR-003 | TEST-STR-001 |
| STR-004 | TEST-STR-002 |
| STR-005 | TEST-STR-002 |
| STR-006 | TEST-STR-002 |
| STR-007 | TEST-STR-002 |
| STR-008 | TEST-STR-003 / TEST-STR-007 |
| STR-009 | TEST-STR-004 |
| STR-010 | TEST-STR-005 |
| STR-011 | TEST-STR-006 |
| STR-012 | TEST-STR-008 / TEST-STR-009 |
| STR-013 | TEST-STR-010 |
| STR-014 | TEST-STR-001 / TEST-STR-002 |
| STR-015 | TEST-STR-004 / TEST-STR-005 / TEST-INT-004 |
| STR-016 | TEST-STR-006 / TEST-STR-011 / TEST-STR-012 |
| STR-017 | TEST-STR-006 / TEST-STR-011 / TEST-STR-012 |

## Comentários

| Requisito | Teste principal |
|---|---|
| COM-001 | TEST-COM-001 |
| COM-002 | TEST-COM-001 |
| COM-003 | TEST-COM-001 |
| COM-004 | TEST-COM-002 |
| COM-005 | TEST-COM-003 |
| COM-006 | TEST-COM-004 / TEST-COM-007 |
| COM-007 | TEST-COM-003 / TEST-COM-004 |
| COM-008 | TEST-COM-003 |
| COM-009 | TEST-COM-005 / TEST-INT-004 |
| COM-010 | TEST-COM-006 / TEST-COM-007 |
| COM-011 | TEST-COM-008 |
| COM-012 | TEST-COM-009 / TEST-INT-002 |
| COM-013 | TEST-COM-008 / TEST-COM-010 |

---

# Critérios de aprovação

Um requisito só será considerado validado quando:

1. o scanner compilar sem erro;
2. o arquivo de teste correspondente puder ser processado;
3. os tokens retornados corresponderem ao comportamento especificado;
4. os valores de `cool_yylval` estiverem corretos quando aplicável;
5. `curr_lineno` estiver correto quando aplicável;
6. a mensagem de erro for exatamente a especificada pelo TP02;
7. o scanner conseguir continuar após erros recuperáveis;
8. o caso estiver coberto pelo `PA2/test.cl` final ou por teste auxiliar documentado.

---

# Estratégia de execução

Durante a implementação, o fluxo básico será:

1. compilar o scanner;
2. executar um teste específico;
3. comparar a sequência de tokens e erros com o comportamento esperado;
4. corrigir eventuais diferenças;
5. executar novamente os testes daquele domínio;
6. executar os testes de integração;
7. executar a suíte completa antes de qualquer merge ou entrega.

Os comandos exatos serão definidos na fase de implementação com base nos alvos disponíveis no Makefile oficial.

---

# Relação com `PA2/test.cl`

Os arquivos em `tests/` são auxiliares para desenvolvimento e diagnóstico.

Antes da entrega, `PA2/test.cl` deve ser ampliado para exercitar de maneira representativa:

- tokens normais;
- case-insensitivity;
- identificadores;
- booleanos;
- operadores;
- whitespace;
- strings válidas;
- escapes;
- erros de string;
- comentários simples;
- comentários aninhados;
- erros de comentário;
- recuperação após erros;
- controle de linhas.

Casos que dependam de bytes difíceis de representar diretamente, como NUL real, poderão continuar sendo gerados por testes auxiliares, desde que a estratégia seja documentada no README da entrega.

---

# Definition of Done dos testes

O plano de testes do M1 estará concluído quando:

- todos os requisitos Core estiverem associados a testes;
- todos os requisitos Strings estiverem associados a testes;
- todos os requisitos Comments estiverem associados a testes;
- existirem testes planejados de integração;
- todos os erros especificados pelo TP02 estiverem cobertos;
- os casos de recuperação estiverem cobertos;
- `curr_lineno` estiver coberto em todos os contextos relevantes;
- os casos de borda de strings estiverem cobertos;
- nesting de comentários estiver coberto;
- a estratégia para consolidar os casos em `PA2/test.cl` estiver documentada.
