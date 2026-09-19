# M1 — String Scanner Specification

## Responsabilidade

Especificar completamente o reconhecimento e tratamento de constantes string da linguagem COOL.

## Escopo

Este documento cobre:

- início e término de strings;
- caracteres normais;
- sequências de escape;
- newline escapada;
- newline não escapada;
- caractere nulo;
- limite máximo de tamanho;
- EOF dentro de string;
- armazenamento do valor semântico;
- atualização de `curr_lineno`;
- recuperação após erros.

## Fontes

- COOL Reference Manual — Seção 10.2
- COOL Reference Manual — Seção 7.1
- Enunciado do TP02 — Seções 4.1 e 4.3
- `cool-parse.h`
- `PA2/cool.flex`

## Infraestrutura existente

O esqueleto `PA2/cool.flex` fornece:

- `MAX_STR_CONST`, com valor 1025;
- `string_buf`;
- `string_buf_ptr`;
- `curr_lineno`;
- `cool_yylval`.

O tamanho 1025 do buffer permite armazenar até 1024 caracteres da constante e o caractere terminador da string em C.

---

## Requisitos

### STR-001 — Início de string

**Origem:** MANUAL, DECISION

Uma constante string começa quando o caractere `"` é encontrado no estado `INITIAL`.

Ao iniciar uma nova string:

- o scanner deve entrar no estado destinado ao processamento de strings;
- `string_buf_ptr` deve apontar para o início de `string_buf`;
- nenhum token deve ser retornado nesse momento.

O conteúdo entre as aspas será processado pelas regras específicas do estado de string.

---

### STR-002 — Encerramento de string válida

**Origem:** MANUAL, INFRA

Uma string válida termina quando uma aspa `"` não escapada é encontrada.

Nesse momento:

- o conteúdo acumulado deve representar a string já com os escapes convertidos;
- a string deve ser terminada adequadamente no buffer;
- o valor deve ser inserido na tabela de strings;
- o `Symbol` resultante deve ser armazenado em `cool_yylval.symbol`;
- o scanner deve retornar `STR_CONST`;
- o scanner deve retornar ao estado `INITIAL`.

---

### STR-003 — Caracteres normais

**Origem:** MANUAL

Caracteres válidos que não sejam:

- aspas de fechamento;
- barra invertida iniciando escape;
- newline não escapada;
- caractere nulo;
- EOF;

devem ser acrescentados ao conteúdo da string.

Cada caractere acrescentado ocupa uma posição no valor final da constante.

---

### STR-004 — Escape `\b`

**Origem:** MANUAL, TP02

A sequência formada por barra invertida seguida de `b` deve produzir no valor semântico um caractere de backspace.

Os dois caracteres presentes no código-fonte representam apenas um caractere no valor final da string.

---

### STR-005 — Escape `\t`

**Origem:** MANUAL, TP02

A sequência formada por barra invertida seguida de `t` deve produzir um caractere tab no valor final da string.

Os dois caracteres do código-fonte representam apenas um caractere no valor resultante.

---

### STR-006 — Escape `\n`

**Origem:** MANUAL, TP02

A sequência formada por barra invertida seguida de `n` deve produzir um caractere newline no valor final da string.

Isso não representa uma mudança física de linha no arquivo-fonte e, portanto, não deve alterar `curr_lineno`.

---

### STR-007 — Escape `\f`

**Origem:** MANUAL, TP02

A sequência formada por barra invertida seguida de `f` deve produzir um caractere form feed no valor final da string.

---

### STR-008 — Escape genérico `\c`

**Origem:** MANUAL, TP02

Para qualquer caractere `c` que não corresponda aos escapes especiais `b`, `t`, `n` ou `f`, a sequência `\c` é permitida.

O resultado armazenado na string deve ser apenas o próprio caractere `c`.

Exemplos:

- `\"` produz `"`;
- `\\` produz `\`;
- `\0` produz o caractere `0`.

Em particular, a sequência textual `\0` NÃO representa um caractere nulo e é válida.

---

### STR-009 — Newline escapada

**Origem:** MANUAL, TP02

Uma mudança física de linha imediatamente precedida por barra invertida é permitida dentro de uma string.

Nesse caso:

- a string continua aberta;
- `curr_lineno` deve ser incrementado;
- o newline correspondente deve fazer parte do valor da string;
- o scanner deve permanecer no estado de string.

---

### STR-010 — Newline não escapada

**Origem:** MANUAL, TP02

Uma mudança física de linha não escapada não pode ocorrer dentro de uma constante string.

Ao encontrá-la, o scanner deve retornar:

`ERROR`

com:

`cool_yylval.error_msg = "Unterminated string constant"`

A linha deve ser contabilizada em `curr_lineno`.

A análise da string termina nesse ponto.

O scanner deve retornar ao estado `INITIAL` e continuar a análise no início da próxima linha.

---

### STR-011 — Caractere nulo

**Origem:** MANUAL, TP02

Uma string não pode conter o caractere nulo real.

Esse caso não deve ser confundido com os dois caracteres `\0`, que são permitidos.

Ao encontrar um caractere nulo real, deve ser produzido:

`ERROR`

com a mensagem:

`String contains null character`

Após detectar esse erro, o scanner não deve começar imediatamente a produzir tokens a partir do restante da string.

Ele deve consumir a entrada até o final lógico dessa constante.

O final lógico é:

- o início da próxima linha, caso uma newline não escapada seja encontrada; ou
- imediatamente após a aspa de fechamento, caso ela seja encontrada antes.

Somente depois disso a análise normal deve continuar.

---

### STR-012 — Comprimento máximo

**Origem:** MANUAL, TP02, INFRA

Uma constante string COOL pode possuir no máximo 1024 caracteres no valor resultante.

A infraestrutura fornece:

`MAX_STR_CONST = 1025`

reservando espaço para os 1024 caracteres permitidos e para o terminador utilizado pela representação em C.

O tamanho deve considerar o conteúdo resultante após a conversão dos escapes.

Por exemplo, `\n` representa um único caractere no valor resultante.

Se o limite permitido for excedido, o scanner deve produzir:

`ERROR`

com a mensagem:

`String constant too long`

Depois da detecção do erro, o scanner deve consumir o restante da constante até seu final lógico.

O final lógico segue a mesma regra utilizada para caractere nulo:

- início da próxima linha após newline não escapada; ou
- depois da aspa de fechamento.

---

### STR-013 — EOF dentro de string

**Origem:** MANUAL, TP02

Se EOF for encontrado enquanto uma string ainda estiver aberta, o scanner deve retornar:

`ERROR`

com:

`cool_yylval.error_msg = "EOF in string constant"`

O conteúdo parcial da string não deve gerar `STR_CONST`.

Após esse erro não existe conteúdo adicional no arquivo para recuperação.

---

### STR-014 — Valor semântico de STR_CONST

**Origem:** INFRA, TP02

Para uma string válida, o scanner deve retornar:

`STR_CONST`

O valor semântico deve ser um `Symbol` armazenado em:

`cool_yylval.symbol`

O símbolo deve representar o conteúdo da string após a conversão dos escapes.

As aspas delimitadoras não fazem parte do valor armazenado.

---

### STR-015 — Atualização de `curr_lineno`

**Origem:** TP02

`curr_lineno` representa a linha corrente do arquivo-fonte.

Dentro do processamento de strings:

- `\n` textual não altera `curr_lineno`;
- newline física escapada incrementa `curr_lineno`;
- newline física não escapada incrementa `curr_lineno` antes da recuperação;
- caracteres normais não alteram `curr_lineno`.

---

### STR-016 — Recuperação após erro de conteúdo

**Origem:** TP02

Erros como:

- caractere nulo;
- string excessivamente longa;

exigem recuperação antes que o scanner volte a produzir tokens normais.

Após um desses erros, o restante da constante deve ser descartado até:

- uma newline não escapada; ou
- a aspa de fechamento.

Se uma newline não escapada for encontrada durante a recuperação:

- `curr_lineno` deve ser incrementado;
- a recuperação termina no início da próxima linha.

Se a aspa de fechamento for encontrada:

- ela deve ser consumida;
- a recuperação termina depois dessa aspa.

Após a recuperação, o scanner deve retornar ao estado `INITIAL`.

---

### STR-017 — Um erro léxico por ocorrência

**Origem:** TP02, DECISION

A recuperação deve impedir que o conteúdo restante de uma string já considerada inválida seja reinterpretado como uma sequência de tokens independentes.

Por exemplo, após detectar um caractere nulo em uma string, o restante dessa mesma constante deve ser descartado de acordo com STR-016.

Isso evita erros em cascata causados pela tokenização do conteúdo que ainda pertence à string inválida.

---

## Estados necessários

A especificação exige pelo menos uma distinção entre:

- processamento normal em `INITIAL`;
- processamento do conteúdo de uma string.

A arquitetura final decidirá os nomes e a quantidade exata de estados Flex.

Um estado adicional de recuperação de string poderá ser utilizado para implementar STR-016, caso essa abordagem torne as regras mais simples.

Essa decisão será registrada em `ARCHITECTURE.md`.

---

## Contrato semântico

Para strings válidas:

| Situação | Retorno | Valor semântico |
|---|---|---|
| String válida | `STR_CONST` | `cool_yylval.symbol` |
| Newline não escapada | `ERROR` | `Unterminated string constant` |
| Caractere nulo | `ERROR` | `String contains null character` |
| String longa demais | `ERROR` | `String constant too long` |
| EOF em string | `ERROR` | `EOF in string constant` |

---

## Casos de teste previstos

### Strings válidas

Devem ser testadas:

- string vazia;
- string de um caractere;
- string normal;
- string com espaços;
- string com pontuação;
- string com 1024 caracteres.

### Escapes

Devem ser testados:

- `\b`;
- `\t`;
- `\n`;
- `\f`;
- `\"`;
- `\\`;
- `\0`;
- outros casos de `\c`.

O resultado deve ser verificado após a conversão dos escapes.

### Linhas

Devem ser testadas:

- string em uma única linha;
- newline física escapada;
- newline física não escapada;
- atualização correta de `curr_lineno`.

### Limites

Devem ser testadas:

- string de 1023 caracteres;
- string de 1024 caracteres;
- string de 1025 caracteres ou mais.

### Caractere nulo

Deve ser testado:

- caractere nulo real;
- sequência textual `\0`, que deve permanecer válida;
- recuperação após o erro seguida por uma aspa;
- recuperação após o erro seguida por newline.

### EOF

Deve ser testada uma string aberta que alcance EOF sem aspas de fechamento.

### Recuperação

Depois de cada erro recuperável, deve existir conteúdo válido posterior no arquivo para verificar que o scanner consegue continuar produzindo tokens corretamente.

---

## Questões resolvidas nesta especificação

- strings válidas retornam `STR_CONST`;
- o valor semântico é armazenado em `cool_yylval.symbol`;
- escapes são convertidos antes do armazenamento;
- `\0` textual é válido e resulta no caractere `0`;
- caractere nulo real é inválido;
- newline não escapada gera `Unterminated string constant`;
- EOF gera `EOF in string constant`;
- string acima do limite gera `String constant too long`;
- o limite do valor da string é 1024 caracteres;
- newlines físicas atualizam `curr_lineno`;
- strings inválidas devem ser consumidas até seu final lógico quando exigido pela recuperação.

## Questões deixadas para ARCHITECTURE.md

A especificação não fixa ainda:

- nome exato do estado Flex para strings;
- existência ou não de estado separado para recuperação;
- funções auxiliares usadas para inserir caracteres no buffer;
- forma exata das expressões regulares Flex.

Essas são decisões de implementação, não requisitos de comportamento.
