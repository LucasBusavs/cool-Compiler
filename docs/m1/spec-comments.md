# M1 — Comment Scanner Specification

## Responsabilidade

Especificar completamente o reconhecimento e tratamento de comentários da linguagem COOL.

## Escopo

Este documento cobre:

- comentários de linha;
- comentários de bloco;
- comentários de bloco aninhados;
- controle de profundidade;
- atualização de `curr_lineno`;
- EOF em comentários;
- fechamento `*)` sem abertura correspondente;
- retorno ao estado normal do scanner.

## Fontes

- COOL Reference Manual — Seção 10.3
- Enunciado do TP02 — Seção 4.1
- `PA2/cool.flex`
- `cool-parse.h`

---

## Requisitos

### COM-001 — Início de comentário de linha

**Origem:** MANUAL

A sequência:

`--`

inicia um comentário de linha quando encontrada no estado normal do scanner.

Nenhum token deve ser produzido para essa sequência ou para o conteúdo do comentário.

O conteúdo deve ser ignorado até:

- a próxima newline; ou
- EOF, caso não exista newline posterior.

---

### COM-002 — Conteúdo de comentário de linha

**Origem:** MANUAL

Depois de `--`, todos os caracteres restantes da linha pertencem ao comentário.

Construções que normalmente possuem significado léxico não devem ser interpretadas dentro desse comentário.

Exemplos:

`-- class if 123 "abc" (*`

não deve produzir tokens para nenhuma dessas construções.

---

### COM-003 — Newline após comentário de linha

**Origem:** MANUAL, TP02

Uma newline encerra o comentário de linha.

Ao encontrá-la:

- o comentário termina;
- `curr_lineno` deve ser incrementado;
- a análise normal deve continuar no início da próxima linha.

A newline não produz token.

---

### COM-004 — EOF após comentário de linha

**Origem:** MANUAL

EOF também encerra normalmente um comentário iniciado por `--`.

Esse caso não constitui erro.

Nenhum token `ERROR` deve ser produzido apenas porque um comentário de linha chegou ao EOF sem newline final.

---

### COM-005 — Início de comentário de bloco

**Origem:** MANUAL

A sequência:

`(*`

inicia um comentário de bloco quando encontrada no estado normal do scanner.

Ao entrar no comentário:

- nenhum token deve ser produzido;
- deve ser iniciado o controle de profundidade de comentários;
- a profundidade lógica inicial deve ser 1;
- o scanner deve permanecer em um estado destinado ao processamento de comentários de bloco.

---

### COM-006 — Comentário de bloco aninhado

**Origem:** MANUAL

Comentários de bloco COOL podem ser aninhados.

Quando a sequência:

`(*`

for encontrada dentro de um comentário de bloco já aberto, a profundidade do comentário deve aumentar em uma unidade.

Exemplo:

`(* comentário externo (* comentário interno *) externo *)`

é um único comentário válido.

O scanner só deve retornar ao estado normal depois que todos os níveis tiverem sido fechados.

---

### COM-007 — Fechamento de comentário de bloco

**Origem:** MANUAL

Quando a sequência:

`*)`

for encontrada dentro de um comentário de bloco, a profundidade deve diminuir em uma unidade.

Se após o decremento a profundidade ainda for maior que zero, o scanner continua dentro do comentário.

Se a profundidade chegar a zero:

- o comentário foi completamente encerrado;
- o scanner deve retornar ao estado normal;
- nenhum token deve ser produzido pelo comentário.

---

### COM-008 — Conteúdo de comentário de bloco

**Origem:** MANUAL

Enquanto estiver dentro de um comentário de bloco, caracteres comuns devem ser ignorados.

Keywords, números, strings, operadores e comentários de linha aparentes não devem ser tokenizados normalmente.

Dentro desse estado, apenas construções relevantes ao próprio comentário precisam receber tratamento especial, principalmente:

- `(*`;
- `*)`;
- newline;
- EOF.

---

### COM-009 — Newline em comentário de bloco

**Origem:** TP02

Uma newline dentro de um comentário de bloco não encerra o comentário.

Ela deve:

- incrementar `curr_lineno`;
- manter o scanner dentro do comentário;
- não produzir token.

Isso deve funcionar em qualquer nível de nesting.

---

### COM-010 — EOF em comentário de bloco

**Origem:** MANUAL, TP02

Se EOF for encontrado enquanto ainda existir um comentário de bloco aberto, o scanner deve retornar:

`ERROR`

com:

`cool_yylval.error_msg = "EOF in comment"`

O conteúdo do comentário não deve ser reinterpretado ou tokenizado apenas porque o terminador `*)` está ausente.

Após esse erro, não existe conteúdo adicional no arquivo para recuperação.

---

### COM-011 — Fechamento sem comentário aberto

**Origem:** TP02

Se a sequência:

`*)`

for encontrada no estado normal, sem existir comentário de bloco aberto, o scanner deve retornar:

`ERROR`

com:

`cool_yylval.error_msg = "Unmatched *)"`

A sequência não deve ser interpretada como dois tokens independentes:

`*`

e

`)`

Após retornar o erro, a análise deve continuar depois dos dois caracteres que formam `*)`.

---

### COM-012 — Delimitadores dentro de outros contextos

**Origem:** MANUAL, DECISION

Delimitadores de comentário só possuem função de comentário quando reconhecidos no estado apropriado.

Por exemplo, os caracteres:

`(*`

ou:

`--`

dentro de uma string válida pertencem ao conteúdo da string e não iniciam comentários.

Da mesma forma, construções semelhantes encontradas dentro de um comentário de bloco devem obedecer às regras do estado de comentário, e não às regras normais de tokenização.

A arquitetura de estados do scanner deve garantir essa separação.

---

### COM-013 — Prioridade dos delimitadores

**Origem:** FLEX, DECISION

As sequências de múltiplos caracteres relacionadas a comentários devem ser reconhecidas como unidades completas quando aplicável.

Casos importantes:

- `--` deve iniciar comentário de linha, em vez de produzir dois tokens `-`;
- `(*` deve iniciar comentário de bloco;
- `*)` em estado normal deve produzir `Unmatched *)`.

O comportamento de longest-match do Flex deve ser levado em consideração.

---

## Controle de profundidade

A implementação deve manter conceitualmente uma profundidade de comentários de bloco.

O comportamento esperado é:

- ao entrar no primeiro `(*`: profundidade 1;
- ao encontrar outro `(*`: incrementar;
- ao encontrar `*)`: decrementar;
- quando chegar a zero: retornar ao estado normal.

O nome e a forma exata da variável serão definidos em `ARCHITECTURE.md`.

---

## Controle de linhas

O contrato de `curr_lineno` para comentários é:

| Situação | Ação |
|---|---|
| Conteúdo comum | nenhuma |
| Newline em comentário de linha | incrementar |
| Newline em comentário de bloco | incrementar |
| Abertura `(*` | nenhuma |
| Fechamento `*)` | nenhuma |
| EOF | nenhuma nova linha a contabilizar |

---

## Contrato de erros

| Situação | Retorno | `cool_yylval.error_msg` |
|---|---|---|
| EOF em comentário de bloco | `ERROR` | `EOF in comment` |
| `*)` fora de comentário | `ERROR` | `Unmatched *)` |
| EOF em comentário de linha | fim normal | nenhum |

O scanner não deve imprimir diretamente mensagens de erro.

Os erros devem ser comunicados ao parser por meio do token `ERROR`.

---

## Casos de teste previstos

### Comentários de linha

Testar:

- comentário vazio;
- comentário com texto;
- comentário contendo keywords;
- comentário contendo números;
- comentário contendo aspas;
- comentário contendo `(*`;
- comentário terminado por newline;
- comentário terminado diretamente por EOF.

### Comentários de bloco

Testar:

- comentário vazio;
- comentário simples;
- comentário de várias linhas;
- comentário contendo tokens aparentes;
- comentário contendo `--`;
- abertura e fechamento na mesma linha.

### Nesting

Testar:

- dois níveis;
- três ou mais níveis;
- vários comentários aninhados consecutivos;
- newline em diferentes níveis.

### Erros

Testar:

- EOF no primeiro nível de comentário;
- EOF dentro de comentário aninhado;
- `*)` em estado normal;
- conteúdo válido após `Unmatched *)`, para verificar continuação da análise.

### Controle de linha

Testar comentários com várias newlines e verificar se o token posterior possui a linha correta.

---

## Questões resolvidas nesta especificação

- `--` inicia comentário de linha;
- newline ou EOF encerra comentário de linha;
- EOF em comentário de linha não é erro;
- `(*` inicia comentário de bloco;
- comentários de bloco podem ser aninhados;
- `*)` reduz a profundidade;
- o scanner só sai do comentário quando a profundidade chega a zero;
- newline em comentários incrementa `curr_lineno`;
- EOF em comentário de bloco retorna `EOF in comment`;
- `*)` fora de comentário retorna `Unmatched *)`;
- conteúdo de comentários não deve ser tokenizado;
- delimitadores de comentário dentro de strings não iniciam comentários.

## Questões deixadas para ARCHITECTURE.md

Esta especificação ainda não determina:

- nome exato do estado Flex para comentários;
- nome da variável de profundidade;
- expressões regulares exatas;
- organização física das regras em `cool.flex`.

Essas são decisões de implementação e serão consolidadas posteriormente.
