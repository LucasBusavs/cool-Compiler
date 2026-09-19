# M1 — Arquitetura do Analisador Léxico

Este documento registra as decisões de implementação que serão utilizadas na construção do analisador léxico COOL em `PA2/cool.flex`.

Os requisitos comportamentais estão descritos em:

- `REQUIREMENTS.md`;
- `spec-core.md`;
- `spec-strings.md`;
- `spec-comments.md`.

A estratégia de validação está descrita em:

- `TEST_PLAN.md`.

---

# Objetivos arquiteturais

A implementação deve:

- seguir a estrutura léxica definida pelo COOL Reference Manual;
- respeitar os requisitos específicos do TP02;
- utilizar a infraestrutura fornecida pelo PA2;
- manter `curr_lineno` correto;
- produzir todos os erros por meio do token `ERROR`;
- não imprimir mensagens diretamente dentro do scanner;
- impedir que regras de um contexto sejam aplicadas em outro;
- garantir que qualquer entrada seja consumida por alguma regra;
- permitir recuperação após erros recuperáveis;
- manter a implementação simples de revisar e testar.

---

# Estados do Flex

Serão utilizados quatro estados conceituais.

## INITIAL

Estado padrão fornecido pelo Flex.

Responsável por:

- keywords;
- booleanos;
- identificadores;
- inteiros;
- operadores;
- símbolos;
- whitespace;
- abertura de strings;
- abertura de comentários;
- comentários de linha;
- erros de caracteres inválidos;
- `Unmatched *)`.

---

## COMMENT

Estado exclusivo destinado a comentários de bloco iniciados por:

`(*`

Responsável por:

- conteúdo normal de comentário;
- novos `(*` aninhados;
- fechamentos `*)`;
- newlines;
- EOF.

O estado será encerrado somente quando a profundidade do comentário chegar a zero.

---

## STRING

Estado exclusivo destinado ao processamento de constantes string.

Responsável por:

- caracteres normais;
- escapes;
- newline escapada;
- newline não escapada;
- caractere NUL;
- comprimento máximo;
- fechamento por aspas;
- EOF.

---

## STRING_RECOVERY

Estado exclusivo destinado a descartar o restante de uma string que já produziu um erro de conteúdo.

Será utilizado principalmente após:

- `String contains null character`;
- `String constant too long`.

Esse estado existe para impedir que o restante de uma constante inválida seja reinterpretado como tokens normais.

A recuperação termina:

- após uma aspa de fechamento não escapada; ou
- no início da próxima linha após newline não escapada.

Depois disso, o scanner retorna para `INITIAL`.

---

# Declaração dos estados

A implementação deverá utilizar estados exclusivos para os contextos especiais.

Conceitualmente:

    %x COMMENT
    %x STRING
    %x STRING_RECOVERY

`INITIAL` é fornecido automaticamente pelo Flex e não precisa ser declarado.

Estados exclusivos são preferidos porque impedem que regras normais de `INITIAL` sejam ativadas acidentalmente dentro de strings ou comentários.

---

# Transições de estado

## Fluxo de comentários

    INITIAL
       |
       |  (*
       v
    COMMENT
       |
       |  (*   profundidade++
       |  *)
       |       profundidade--
       |
       |  profundidade == 0
       v
    INITIAL

EOF em `COMMENT` produz:

`ERROR: EOF in comment`

---

## Fluxo de strings válidas

    INITIAL
       |
       |  "
       v
    STRING
       |
       | caracteres / escapes
       |
       |  "
       v
    INITIAL
       |
       +--> retorna STR_CONST

---

## Fluxo de erro simples de string

Para newline não escapada:

    STRING
       |
       | newline
       v
    INITIAL

e retorna:

`ERROR: Unterminated string constant`

---

## Fluxo de erro com recuperação

Para NUL ou string longa:

    STRING
       |
       | erro detectado
       v
    STRING_RECOVERY
       |
       | retorna ERROR
       |
       | próxima chamada ao scanner continua
       | descartando a mesma constante
       |
       | " não escapada
       | ou newline não escapada
       v
    INITIAL

---

# Variáveis compartilhadas

## Infraestrutura existente

Serão utilizadas as variáveis já fornecidas pelo esqueleto:

- `curr_lineno`;
- `string_buf`;
- `string_buf_ptr`;
- `cool_yylval`.

Também será utilizado:

- `MAX_STR_CONST`.

---

## Profundidade de comentários

Será adicionada uma variável:

`comment_depth`

Tipo:

`int`

Comportamento:

- entrada em comentário de bloco: `1`;
- novo `(*`: incrementar;
- `*)`: decrementar;
- valor zero: retornar a `INITIAL`.

A profundidade não deve ser usada fora do contexto de comentário de bloco.

---

# Buffer de strings

O buffer fornecido pelo esqueleto será utilizado:

`string_buf`

e o ponteiro:

`string_buf_ptr`

Ao encontrar a aspa inicial:

- `string_buf_ptr` deve voltar para o início de `string_buf`;
- o scanner entra em `STRING`.

Cada caractere resultante deve ser armazenado no buffer.

Escapes devem ser convertidos antes do armazenamento.

Exemplo:

`\n`

ocupa dois caracteres no arquivo-fonte, mas apenas um caractere no buffer.

---

# Limite das strings

A infraestrutura define:

`MAX_STR_CONST = 1025`

O valor máximo de uma constante COOL é de 1024 caracteres.

Portanto, o buffer deve reservar:

- até 1024 caracteres de conteúdo;
- uma posição para o terminador da representação C.

Antes de adicionar um novo caractere ao valor resultante, a implementação deve verificar se ainda existe espaço disponível.

Se a adição ultrapassaria 1024 caracteres:

- definir `cool_yylval.error_msg`;
- entrar em `STRING_RECOVERY`;
- retornar `ERROR`.

Mensagem:

`String constant too long`

---

# Finalização de string válida

Ao encontrar uma aspa de fechamento não escapada em `STRING`:

1. adicionar o terminador ao buffer;
2. inserir o conteúdo na tabela de strings;
3. armazenar o `Symbol` em `cool_yylval.symbol`;
4. retornar para `INITIAL`;
5. retornar `STR_CONST`.

As aspas delimitadoras não fazem parte do valor armazenado.

---

# Escape de strings

Dentro de `STRING`, os escapes serão convertidos antes de serem inseridos no buffer.

Mapeamento:

| Entrada | Valor armazenado |
|---|---|
| `\b` | backspace |
| `\t` | tab |
| `\n` | newline |
| `\f` | form feed |
| `\c` | caractere `c` |

Para escapes genéricos, somente o segundo caractere será armazenado.

Assim:

`\0`

resulta no caractere:

`0`

e não em NUL.

---

# Newline física escapada em string

Quando uma barra invertida preceder uma newline física:

- a string permanece aberta;
- a newline é incorporada ao valor da string;
- `curr_lineno` é incrementado;
- o scanner permanece em `STRING`.

---

# Newline não escapada em string

Uma newline física não escapada encerra a string com erro.

A implementação deve:

1. incrementar `curr_lineno`;
2. retornar para `INITIAL`;
3. colocar em `cool_yylval.error_msg`:

`Unterminated string constant`

4. retornar `ERROR`.

A análise seguinte começa no início da nova linha.

---

# Caractere NUL em string

NUL real dentro de `STRING` deve:

1. definir:

`cool_yylval.error_msg = "String contains null character"`

2. entrar em `STRING_RECOVERY`;
3. retornar `ERROR`.

A sequência textual:

`\0`

não segue esse caminho.

Ela é tratada como escape genérico e produz o caractere `0`.

---

# Recuperação de strings

Depois que uma string produzir erro de NUL ou comprimento:

- nenhum conteúdo posterior da mesma constante deve gerar tokens normais;
- o scanner permanece em `STRING_RECOVERY`.

## Aspas de fechamento

Uma aspa não escapada:

- é consumida;
- encerra a recuperação;
- faz `BEGIN(INITIAL)`;
- não produz token.

## Newline não escapada

Uma newline não escapada:

- é consumida;
- incrementa `curr_lineno`;
- encerra a recuperação;
- faz `BEGIN(INITIAL)`;
- não produz novo erro para a mesma constante.

## Newline escapada

Uma newline física precedida por barra invertida:

- é consumida;
- incrementa `curr_lineno`;
- NÃO encerra a recuperação.

## Outros escapes

Uma barra invertida seguida por outro caractere deve ser consumida como uma unidade durante a recuperação.

Isso evita interpretar uma aspa escapada como final da constante.

## Outros caracteres

São simplesmente descartados.

---

# EOF

## EOF em INITIAL

Representa término normal da entrada.

Nenhum `ERROR` é produzido.

---

## EOF em COMMENT

Produz:

`ERROR`

Mensagem:

`EOF in comment`

O scanner deve sair do estado de comentário antes de finalizar.

---

## EOF em STRING

Produz:

`ERROR`

Mensagem:

`EOF in string constant`

O scanner deve retornar a `INITIAL`.

---

## EOF durante STRING_RECOVERY

Uma string nesse estado já produziu seu erro léxico principal.

EOF encerra a recuperação e a entrada sem reinterpretar o conteúdo descartado.

Nenhum conteúdo posterior existe para tokenização.

---

# Comentários de linha

Comentários iniciados por:

`--`

não necessitam de estado exclusivo próprio.

Em `INITIAL`, o scanner pode consumir todos os caracteres depois de `--` até antes da newline.

A newline fica disponível para a regra normal de linha em `INITIAL`, que:

- incrementa `curr_lineno`;
- não retorna token.

Se EOF ocorrer antes de newline, o comentário simplesmente termina com o arquivo.

---

# Comentários de bloco

Ao encontrar:

`(*`

em `INITIAL`:

- definir `comment_depth = 1`;
- entrar em `COMMENT`.

Dentro de `COMMENT`:

- `(*` incrementa `comment_depth`;
- `*)` decrementa `comment_depth`;
- newline incrementa `curr_lineno`;
- outros caracteres são ignorados.

Quando `comment_depth` chegar a zero:

- retornar a `INITIAL`.

---

# Unmatched comentário

A sequência:

`*)`

encontrada em `INITIAL` deve ser reconhecida antes que `*` e `)` possam ser tratados individualmente.

A ação deve:

- armazenar `Unmatched *)` em `cool_yylval.error_msg`;
- retornar `ERROR`;
- consumir os dois caracteres.

---

# Controle de linhas

A responsabilidade por `curr_lineno` é distribuída por estado.

| Estado | Situação | Ação |
|---|---|---|
| INITIAL | newline | incrementar |
| INITIAL | demais whitespace | nenhuma |
| COMMENT | newline | incrementar |
| STRING | `\n` textual | nenhuma |
| STRING | newline física escapada | incrementar |
| STRING | newline física não escapada | incrementar |
| STRING_RECOVERY | newline física escapada | incrementar |
| STRING_RECOVERY | newline física não escapada | incrementar |

A mesma newline nunca deve ser contabilizada duas vezes.

---

# Ordem macro das regras em INITIAL

A ordem conceitual será:

1. `*)` sem comentário;
2. comentário de linha `--`;
3. abertura de comentário de bloco `(*`;
4. abertura de string `"`;
5. operadores compostos;
6. constantes booleanas;
7. keywords;
8. inteiros;
9. `TYPEID`;
10. `OBJECTID`;
11. newline;
12. demais whitespace;
13. símbolos simples;
14. fallback de caractere inválido.

Essa ordem torna explícitos os conflitos potenciais.

O longest-match do Flex continua valendo.

Quando duas regras reconhecem sequências de mesmo comprimento, a regra declarada primeiro possui prioridade.

---

# Keywords e identificadores

Keywords devem aparecer antes das regras gerais de identificadores.

Assim:

`class`

é reconhecido como:

`CLASS`

Por outro lado:

`classes`

é reconhecido como um único:

`OBJECTID`

porque o longest-match seleciona a correspondência mais longa.

---

# Booleanos e identificadores

As regras de `true` e `false` devem aparecer antes dos identificadores.

A primeira letra precisa ser minúscula.

Exemplos:

`true` → `BOOL_CONST`

`tRuE` → `BOOL_CONST`

`True` → `TYPEID`

`FALSE` → `TYPEID`

---

# Identificadores

A forma conceitual das regras é:

TYPEID:

letra maiúscula seguida de zero ou mais letras, dígitos ou `_`.

OBJECTID:

letra minúscula seguida de zero ou mais letras, dígitos ou `_`.

Os lexemas serão inseridos na tabela de identificadores.

`self` e `SELF_TYPE` não recebem tratamento lexical especial.

---

# Inteiros

Inteiros consistem em uma ou mais ocorrências dos dígitos de `0` a `9`.

O lexema completo será inserido na tabela de inteiros.

Não haverá verificação de overflow no scanner.

---

# Operadores compostos

Os seguintes lexemas possuem tokens próprios:

| Lexema | Token |
|---|---|
| `<-` | `ASSIGN` |
| `<=` | `LE` |
| `=>` | `DARROW` |

Devem ser reconhecidos como unidades completas.

---

# Símbolos simples

Símbolos de um caractere serão retornados pelo próprio valor do caractere.

Incluem:

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

---

# Valores semânticos

O contrato será:

| Categoria | Campo |
|---|---|
| TYPEID | `cool_yylval.symbol` |
| OBJECTID | `cool_yylval.symbol` |
| INT_CONST | `cool_yylval.symbol` |
| STR_CONST | `cool_yylval.symbol` |
| BOOL_CONST | `cool_yylval.boolean` |
| ERROR | `cool_yylval.error_msg` |

Keywords e operadores que não carregam informação adicional não preenchem valor semântico.

---

# Mensagens de erro

As mensagens fixas serão exatamente:

- `Unterminated string constant`;
- `String constant too long`;
- `String contains null character`;
- `EOF in string constant`;
- `EOF in comment`;
- `Unmatched *)`.

Para caractere inválido, `error_msg` deve representar somente o caractere encontrado.

O scanner nunca deve imprimir essas mensagens diretamente.

---

# Armazenamento da mensagem de caractere inválido

Como `cool_yylval.error_msg` possui tipo `char *`, será mantido um pequeno buffer persistente para representar um caractere inválido.

Conceitualmente:

    char invalid_char_error[2];

Ao detectar o caractere:

- posição 0 recebe o caractere;
- posição 1 recebe o terminador;
- `cool_yylval.error_msg` aponta para esse buffer.

Isso evita depender do tempo de vida interno de `yytext`.

---

# Completude das regras

Não será permitido depender da ação padrão do Flex para caracteres não reconhecidos.

Cada estado deverá possuir regras capazes de consumir todas as entradas possíveis relevantes ao seu contexto.

Em particular:

- `INITIAL` possui fallback para caractere inválido;
- `COMMENT` ignora qualquer conteúdo não especial;
- `STRING` trata conteúdo normal e todos os casos especiais;
- `STRING_RECOVERY` consome todo o restante da constante inválida.

O scanner não deve ecoar caracteres inesperados para a saída.

---

# Organização física de `cool.flex`

A implementação deverá ser organizada aproximadamente nesta ordem:

    1. declarações C/C++
       - includes existentes
       - comment_depth
       - buffers auxiliares
       - eventuais helpers

    2. definições Flex
       - estados
       - padrões reutilizáveis

    3. regras
       - INITIAL / comentários
       - operadores
       - keywords e booleanos
       - identificadores e inteiros
       - strings
       - COMMENT
       - STRING_RECOVERY
       - whitespace
       - fallback

    4. rotinas auxiliares
       - somente quando simplificarem a implementação

As seções já existentes no esqueleto devem ser preservadas quando fazem parte da integração com a infraestrutura oficial.

---

# Estratégia de implementação

A implementação será realizada incrementalmente.

Ordem proposta:

1. Core básico;
2. whitespace e `curr_lineno`;
3. comentários de linha;
4. comentários de bloco e nesting;
5. strings válidas;
6. escapes;
7. erros de strings;
8. recuperação de strings;
9. erros gerais;
10. suíte completa de testes.

Depois de cada etapa:

- compilar;
- executar os testes específicos;
- executar testes já aprovados anteriormente.

---

# Arquivos modificáveis

Durante a implementação do TP02, o principal arquivo de código será:

`PA2/cool.flex`

Também serão alterados posteriormente:

- `PA2/test.cl`;
- `PA2/README`.

Arquivos da infraestrutura marcados como não modificáveis não devem ser alterados.

---

# Definition of Done da arquitetura

A arquitetura estará concluída quando estiverem definidos:

- estados Flex;
- transições de estados;
- controle de nesting;
- buffer de strings;
- política de tamanho;
- escapes;
- recuperação de strings;
- política de EOF;
- tratamento de comentários;
- controle de `curr_lineno`;
- ordem das regras;
- prioridade de tokens;
- valores semânticos;
- mensagens de erro;
- fallback completo;
- organização prevista de `cool.flex`;
- estratégia incremental de implementação.

Com este documento concluído, nenhuma decisão comportamental relevante deverá precisar ser descoberta durante a implementação.

---

# Estado final do M1

Ao concluir esta arquitetura:

- `spec-core.md`: concluído;
- `spec-strings.md`: concluído;
- `spec-comments.md`: concluído;
- `TEST_PLAN.md`: concluído;
- `REQUIREMENTS.md`: concluído;
- `ARCHITECTURE.md`: concluído.

O próximo marco será a implementação do analisador léxico em `PA2/cool.flex`.
