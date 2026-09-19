#!/usr/bin/env bash
set -u
fail=0
ok(){ printf "  [OK]   %s\n" "$1"; }
warn(){ printf "  [WARN] %s\n" "$1"; }
bad(){ printf "  [FAIL] %s\n" "$1"; fail=1; }

echo "== Verificação do ambiente COOL =="

if [[ -f /etc/os-release ]]; then
  . /etc/os-release
  [[ "${VERSION_ID:-}" == "16.04" ]] && ok "Ubuntu 16.04" || bad "Esperado Ubuntu 16.04; encontrado ${VERSION_ID:-?}"
else
  bad "/etc/os-release não encontrado"
fi

for cmd in g++ make csh uudecode flex bison; do
  command -v "$cmd" >/dev/null 2>&1 && ok "$cmd -> $(command -v "$cmd")" || bad "$cmd não encontrado"
done

echo
command -v g++ >/dev/null 2>&1 && g++ --version | head -n 1
command -v make >/dev/null 2>&1 && make --version | head -n 1
command -v flex >/dev/null 2>&1 && flex --version | head -n 1
command -v bison >/dev/null 2>&1 && bison --version | head -n 1

[[ -d /workspace ]] && ok "/workspace existe" || bad "/workspace não existe"
[[ -d /var/tmp/cool ]] && ok "/var/tmp/cool existe" || bad "/var/tmp/cool não existe"
[[ -f /var/tmp/cool/assignments/PA2/Makefile ]] && ok "infraestrutura PA2 instalada" || warn "PA2 ainda não instalado em /var/tmp/cool"
[[ -d /workspace/PA2 ]] && ok "/workspace/PA2 existe" || warn "/workspace/PA2 ainda não foi gerado"

exit "$fail"
