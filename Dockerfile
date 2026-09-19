FROM ubuntu:16.04

ENV DEBIAN_FRONTEND=noninteractive

# Dependências exigidas pelo ambiente da disciplina.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        g++ \
        make \
        csh \
        sharutils \
        flex \
        bison \
        git \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# Diretórios usados pelo projeto.
RUN mkdir -p /var/tmp/cool /workspace

WORKDIR /workspace

CMD ["/bin/bash"]
