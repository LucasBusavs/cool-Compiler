#!/usr/bin/env bash
set -euo pipefail

ASSET="/workspace/assets/x86_64.u"
COOL_DIR="/var/tmp/cool"
PA2_MAKEFILE="${COOL_DIR}/assignments/PA2/Makefile"

echo "== COOL setup =="

if [[ "$(id -u)" -ne 0 ]]; then
  echo "Erro: execute este script dentro do container do projeto."
  exit 1
fi

for cmd in uudecode tar make; do
  command -v "$cmd" >/dev/null 2>&1 || { echo "Erro: comando não encontrado: $cmd"; exit 1; }
done

if [[ -f "$PA2_MAKEFILE" ]]; then
  echo "Infraestrutura COOL já instalada."
  echo "Encontrado: $PA2_MAKEFILE"
  exit 0
fi

if [[ ! -f "$ASSET" ]]; then
  echo "Erro: $ASSET não encontrado."
  echo "Baixe x86_64.u do Canvas e coloque em assets/x86_64.u"
  exit 1
fi

mkdir -p "$COOL_DIR"
cp "$ASSET" "$COOL_DIR/x86_64.u"
cd "$COOL_DIR"

rm -f x86_64.tar.gz
echo "Decodificando x86_64.u..."
uudecode x86_64.u

[[ -f x86_64.tar.gz ]] || { echo "Erro: x86_64.tar.gz não foi gerado."; exit 1; }

echo "Extraindo..."
tar xvpf x86_64.tar.gz

echo "Executando make install..."
make install

[[ -f "$PA2_MAKEFILE" ]] || { echo "Erro: PA2 Makefile não encontrado após instalação."; exit 1; }

echo "Instalação concluída."
echo "PA2 Makefile: $PA2_MAKEFILE"
