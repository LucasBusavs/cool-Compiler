# Contribuindo

## Responsabilidades iniciais

### Pessoa 1 — Core / Integração
Branch: `feature/core-scanner`

### Pessoa 2 — Strings
Branch: `feature/strings`

### Pessoa 3 — Comentários / QA
Branch: `feature/comments-tests`

## Fluxo

```bash
git switch main
git pull
git switch -c feature/<nome>
```

Antes de PR:

```bash
docker compose run --rm cool ./scripts/verify-env.sh
docker compose run --rm cool ./scripts/test.sh
```

Depois:

```bash
git add .
git commit -m "feat: descrição objetiva"
git push -u origin feature/<nome>
```

Abra Pull Request para `main`.

## Commits

Exemplos:

```text
feat: recognize COOL keywords
feat: implement nested comments
feat: handle string escapes
test: add EOF in comment case
fix: recover after unterminated string
docs: document scanner states
```

## Sincronização

```bash
git switch main
git pull
git switch feature/<nome>
git rebase main
```

## `cool.flex`

Organizar em blocos de responsabilidade:

```text
CORE
COMMENTS
STRINGS
FALLBACK / ERRORS
```

Evitar editar o bloco de outro integrante sem alinhamento.

## Definition of Done

- compila;
- testes passam;
- teste relacionado existe quando aplicável;
- documentação relevante atualizada;
- PR revisado.
