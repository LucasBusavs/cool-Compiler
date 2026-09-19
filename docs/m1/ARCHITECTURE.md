# M1 — Arquitetura do Analisador Léxico

Este documento registra as decisões arquiteturais compartilhadas entre as três frentes de implementação.

## Estados do Flex

A definir durante o M1.

Estados inicialmente considerados:

- `INITIAL`
- `COMMENT`
- `STRING`

## Variáveis compartilhadas

Já fornecidas pela infraestrutura:

- `curr_lineno`
- `string_buf`
- `string_buf_ptr`
- `cool_yylval`

A definir pela equipe:

- controle de profundidade de comentários aninhados
- eventuais estados auxiliares de recuperação

## Responsabilidade por linhas

Deve ser definido quem atualiza `curr_lineno` em:

- `INITIAL`
- `COMMENT`
- `STRING`

## Política de erros

Para cada erro léxico deverão ser definidos:

1. condição de detecção;
2. token retornado;
3. conteúdo de `cool_yylval.error_msg`;
4. estratégia de recuperação;
5. estado do scanner após a recuperação.

## Ordem das regras

Regras sensíveis ao comportamento de longest-match e desempate do Flex deverão ser explicitamente identificadas.

## Divisão prevista

- Core / Integração
- Strings
- Comentários / QA
