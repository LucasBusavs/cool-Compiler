#!/usr/bin/env bash
set -euo pipefail
PA2_DIR="/workspace/PA2"

[[ -d "$PA2_DIR" ]] || { echo "Erro: $PA2_DIR não existe."; exit 1; }

cd "$PA2_DIR"
echo "== Build do lexer =="
make lexer

if [[ -x ./lexer && -f ./test.cl ]]; then
  echo
  echo "== Execução do test.cl =="
  ./lexer test.cl
else
  echo "Build concluído; execução automática não realizada."
fi
