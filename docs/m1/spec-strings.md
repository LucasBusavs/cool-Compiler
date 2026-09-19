# M1 — String Scanner Specification

## Responsabilidade

Especificar completamente o reconhecimento e tratamento de constantes string da linguagem COOL.

## Escopo

- início de string;
- término de string;
- caracteres normais;
- sequências de escape;
- newline;
- EOF;
- caractere nulo;
- limite máximo de tamanho;
- armazenamento da string;
- erros;
- recuperação após erro.

## Fontes

- COOL Reference Manual — Seção 10.2
- Enunciado do TP02
- `cool-parse.h`
- `PA2/cool.flex`

## Infraestrutura existente

O esqueleto fornece:

- `MAX_STR_CONST`;
- `string_buf`;
- `string_buf_ptr`;
- `curr_lineno`;
- `cool_yylval`.

O M1 deverá definir como essas estruturas serão utilizadas.

## Requisitos

A preencher durante o M1.

Cada requisito deverá informar:

- ID;
- origem;
- condição de reconhecimento;
- estado do Flex;
- ação executada;
- token retornado;
- valor em `cool_yylval`;
- mensagem de erro, quando aplicável;
- estratégia de recuperação;
- estado final;
- impacto em `curr_lineno`;
- caso de teste correspondente.

## Pontos a especificar

### Entrada em string

Determinar:

- caractere que inicia a string;
- inicialização do buffer;
- estado Flex utilizado.

### String válida

Determinar:

- quais caracteres são aceitos;
- quando a string termina;
- como o conteúdo é armazenado;
- como `STR_CONST` é retornado.

### Escapes

Especificar:

- `\b`;
- `\t`;
- `\n`;
- `\f`;
- escape genérico `\c`.

### Newline

Determinar:

- comportamento para newline escapada;
- comportamento para newline não escapada;
- atualização de `curr_lineno`.

### Caractere nulo

Determinar:

- como detectar;
- token retornado;
- mensagem de erro;
- estratégia de recuperação.

### String muito longa

Determinar:

- limite permitido;
- momento de detecção;
- token retornado;
- mensagem de erro;
- estratégia de recuperação.

### EOF

Determinar:

- comportamento quando EOF ocorre antes do fechamento;
- token retornado;
- mensagem de erro;
- estado posterior.

### Recuperação

Definir exatamente quais caracteres devem continuar sendo consumidos após cada tipo de erro.

## Casos de teste

A preencher durante o M1.
