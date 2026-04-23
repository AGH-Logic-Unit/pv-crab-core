FROM debian:12-slim

ENV DEBIAN_FRONTEND=noninteractive

USER root

# 1. Install minimum dependencies
RUN apt update && apt install -y --no-install-recommends \
    git make autoconf g++ flex bison ccache \
    libfl2 libfl-dev zlib1g zlib1g-dev \
    python3 python3-pip python3-dev python3-venv \
    curl wget tar ca-certificates \
    yosys nodejs npm \
    gcc-riscv64-unknown-elf picolibc-riscv64-unknown-elf \
    sudo openssh-client\
    && rm -rf /var/lib/apt/lists/*


# 2. Create standard user
RUN useradd -m -s /bin/bash vscode && \
    echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/vscode && \
    chmod 0440 /etc/sudoers.d/vscode


# 3. Compile Verilator (Minimal, no documentation)
ENV VERILATOR_VERSION=5.046
ENV VERILATOR_SHA256="002bc6d92b203eb8b4612e1d198d8108517d4ec9859e131ef328015352fe6d0c"
RUN wget https://github.com/verilator/verilator/archive/refs/tags/v${VERILATOR_VERSION}.tar.gz -O /tmp/verilator.tar.gz && \
    echo "${VERILATOR_SHA256}  /tmp/verilator.tar.gz" | sha256sum -c - && \
    tar -xzf /tmp/verilator.tar.gz -C /tmp && \
    cd /tmp/verilator-${VERILATOR_VERSION} && \
    autoconf && \
    ./configure && \
    # Create dummy man pages to skip help2man requirement
    touch verilator.1 verilator_coverage.1 verilator_gantt.1 verilator_profcfunc.1 && \
    make -j$(nproc) && \
    make install && \
    rm -rf /tmp/verilator*/tmp/verilator.tar.gz && \
    # Remove documentation bloat
    rm -rf /usr/local/share/man /usr/local/share/doc/verilator && \
    rm -rf /usr/local/share/verilator/examples && \
    apt remove -y flex bison && \
    apt autoremove -y && \
    apt clean

# 4. Install Verible
ENV VERIBLE_VERSION=v0.0-4053-g89d4d98a
ENV VERIBLE_SHA256="1edc1f29c70d74213ed373e727183802d5a733e23f9ab9c74462f5b18b76f2c0"
RUN wget https://github.com/chipsalliance/verible/releases/download/${VERIBLE_VERSION}/verible-${VERIBLE_VERSION}-linux-static-x86_64.tar.gz -O /tmp/verible.tar.gz && \
    echo "${VERIBLE_SHA256}  /tmp/verible.tar.gz" | sha256sum -c - && \
    tar -xzf /tmp/verible.tar.gz -C /tmp && \
    cp /tmp/verible-*/bin/* /usr/local/bin/ && \
    rm -rf /tmp/verible*


# --- FOR svlangserver ---
# RUN npm install -g svlangserver

# 6. Python tools
USER vscode
ENV PATH="/home/vscode/venv/bin:$PATH"
RUN python3 -m venv /home/vscode/venv && \
    /home/vscode/venv/bin/pip install --no-cache-dir cocotb pytest cocotb-test pre-commit ruff zensical
