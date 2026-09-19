# M1 — Plano de Testes do Analisador Léxico

## Objetivo

Definir os testes necessários para validar todos os requisitos do scanner COOL.

## Estrutura prevista

~~~text
tests/
├── core/
├── strings/
├── comments/
└── integration/
~~~

## Estratégia

Cada requisito deverá possuir pelo menos um teste correspondente.

Quando aplicável, deverão existir:

- caso válido;
- caso de borda;
- caso inválido;
- teste de recuperação após erro.

## Core

A preencher a partir de `spec-core.md`.

## Strings

A preencher a partir de `spec-strings.md`.

## Comentários

A preencher a partir de `spec-comments.md`.

## Integração

Os testes de integração deverão combinar múltiplas categorias léxicas em um mesmo programa COOL.

## Entrega

Os casos mais importantes deverão posteriormente ser consolidados no `PA2/test.cl`, conforme exigido pelo TP02.
