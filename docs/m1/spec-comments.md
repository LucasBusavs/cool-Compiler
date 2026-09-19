# M1 — Comment Scanner Specification

## Responsabilidade

Especificar completamente o tratamento de comentários da linguagem COOL.

## Escopo

- comentários de linha;
- comentários de bloco;
- comentários de bloco aninhados;
- profundidade de comentários;
- newline dentro de comentários;
- EOF dentro de comentário;
- fechamento de comentário fora de comentário;
- erros e recuperação.

## Fontes

- COOL Reference Manual — Seção 10.3
- Enunciado do TP02
- `cool-parse.h`
- `PA2/cool.flex`

## Requisitos

A preencher durante o M1.

Cada requisito deverá informar:

- ID;
- origem;
- sequência reconhecida;
- estado do Flex;
- ação executada;
- alteração da profundidade do comentário;
- impacto em `curr_lineno`;
- token retornado, quando aplicável;
- mensagem de erro, quando aplicável;
- estratégia de recuperação;
- caso de teste correspondente.

## Pontos a especificar

### Comentário de linha

Determinar:

- sequência de abertura;
- condição de término;
- comportamento em newline;
- comportamento em EOF.

### Comentário de bloco

Determinar:

- sequência de abertura;
- sequência de fechamento;
- estado Flex utilizado;
- comportamento do conteúdo interno.

### Comentários aninhados

Determinar:

- variável de profundidade;
- incremento ao encontrar nova abertura;
- decremento ao encontrar fechamento;
- condição para retornar ao estado inicial.

### Newline

Determinar:

- atualização de `curr_lineno` dentro de comentário.

### EOF em comentário

Determinar:

- token retornado;
- mensagem de erro;
- estado final.

### Fechamento sem abertura

Determinar o comportamento quando `*)` aparece fora de um comentário de bloco.

### Recuperação

Definir como o scanner deve continuar após erros relacionados a comentários.

## Casos de teste

A preencher durante o M1.
