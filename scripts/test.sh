#!/usr/bin/env bash
set -euo pipefail
PA2_DIR="/workspace/PA2"

if [[ $# -gt 1 ]]; then
  echo "Uso: $0 [arquivo.cl]" >&2
  exit 1
fi

TEST_FILE="${1:-$PA2_DIR/test.cl}"
# Preserve o caminho informado antes de mudar para o diretório do build.
if [[ "$TEST_FILE" != /* ]]; then
  TEST_FILE="$PWD/$TEST_FILE"
fi

[[ -d "$PA2_DIR" ]] || { echo "Erro: $PA2_DIR não existe."; exit 1; }
[[ -f "$TEST_FILE" && -r "$TEST_FILE" ]] || {
  echo "Erro: arquivo de teste não encontrado ou sem permissão de leitura: $TEST_FILE" >&2
  exit 1
}

cd "$PA2_DIR"
echo "== Build do lexer =="
make lexer

echo
echo "== Execução de $TEST_FILE =="
./lexer "$TEST_FILE"
