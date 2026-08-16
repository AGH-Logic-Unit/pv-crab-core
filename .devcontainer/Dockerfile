FROM ubuntu:24.04

USER root

ARG TARGETARCH
ARG BUILDARCH

# Layer 1. System packages, user, and environment setup
# 1. Install minimum dependencies
# 2. Create standard user
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    autoconf g++ gcc-riscv64-unknown-elf git make \
    bison device-tree-compiler flex \
    libfl-dev libfl2 libsystemc-dev libz3-dev zlib1g zlib1g-dev \
    ca-certificates curl openssh-client sudo tar wget \
    lcov python3 python3-dev python3-pip python3-venv ripgrep \
    nodejs npm picolibc-riscv64-unknown-elf yosys z3 \
    && useradd -m -s /bin/bash vscode \
    && echo "vscode ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/vscode \
    && chmod 0440 /etc/sudoers.d/vscode \
    && rm -rf /var/lib/apt/lists/*


# Layer 2. Verilator
# 3. Compile Verilator (Minimal, no documentation)
ENV VERILATOR_VERSION=5.050
ENV VERILATOR_SHA256="ec6723f30c1798b1fbbbed97364f09c431fb4875577c314f37240e99b60a4a04"
RUN wget https://github.com/verilator/verilator/archive/refs/tags/v${VERILATOR_VERSION}.tar.gz -O /tmp/verilator.tar.gz && \
    echo "${VERILATOR_SHA256}  /tmp/verilator.tar.gz" | sha256sum -c - && \
    tar -xzf /tmp/verilator.tar.gz -C /tmp && \
    if [ "$TARGETARCH" != "$BUILDARCH" ]; then \
        MAKE_JOBS=1 && MAKE_FLAGS="VL_NOOPT=1"; \
    else \
        MAKE_JOBS=$(nproc) && MAKE_FLAGS=""; \
    fi && \
    cd /tmp/verilator-${VERILATOR_VERSION} && \
    autoconf && \
    ./configure && \
    # Create dummy man pages to skip help2man requirement
    touch verilator.1 verilator_coverage.1 verilator_gantt.1 verilator_profcfunc.1 && \
    make -C src -j${MAKE_JOBS} ${MAKE_FLAGS} opt && \
    cp bin/verilator_bin bin/verilator_bin_dbg && \
    cp bin/verilator_bin bin/verilator_coverage_bin_dbg && \
    make install-all && \
    rm -rf /tmp/verilator-${VERILATOR_VERSION} /tmp/verilator.tar.gz && \
    # Remove documentation bloat
    rm -rf /usr/local/share/man /usr/local/share/doc/verilator && \
    rm -rf /usr/local/share/verilator/examples && \
    apt-get purge -y --auto-remove flex bison && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*


# Layer 3. Pre-built tools
# 4. Install Verible
ENV VERIBLE_VERSION=v0.0-4135-g7807ee1a
ENV RISCV_TESTS=/opt/riscv-tests
RUN if [ "$TARGETARCH" = "arm64" ] || [ "$(uname -m)" = "aarch64" ]; then \
        VERIBLE_ARCH="arm64" && \
        VERIBLE_SHA256="54c0c1bb2e6b106de1d8ee1a5c87d826af6f1a44b405f9f6aac2c8e52ec6be1d"; \
    else \
        VERIBLE_ARCH="x86_64" && \
        VERIBLE_SHA256="633d7bdade382ce9f155c9d4541428d95c8f6f4bb42c5a592ea6342cf78a9a86"; \
    fi && \
    wget https://github.com/chipsalliance/verible/releases/download/${VERIBLE_VERSION}/verible-${VERIBLE_VERSION}-linux-static-${VERIBLE_ARCH}.tar.gz -O /tmp/verible.tar.gz && \
    echo "${VERIBLE_SHA256}  /tmp/verible.tar.gz" | sha256sum -c - && \
    tar -xzf /tmp/verible.tar.gz -C /tmp && \
    cp /tmp/verible-*/bin/* /usr/local/bin/ && \
    rm -rf /tmp/verible* && \
# 5. Spike (RISC-V ISA Simulator)
    git clone --depth=1 --branch v1.1.0 \
        https://github.com/riscv-software-src/riscv-isa-sim /tmp/riscv-isa-sim && \
    cd /tmp/riscv-isa-sim && \
    mkdir build && cd build && \
    ../configure --prefix=/usr/local && \
    make -j$(nproc) && \
    make install && \
    cd / && rm -rf /tmp/riscv-isa-sim && \
# 6. RISC-V Architectural Tests
    git clone --depth=1 --recurse-submodules \
        https://github.com/riscv/riscv-tests ${RISCV_TESTS} && \
    cd ${RISCV_TESTS} && \
    autoconf && \
    ./configure --prefix=${RISCV_TESTS}/install && \
    # p-env only (m-mode): \
    (make -k -j$(nproc) isa 2>/dev/null || true) && \
    # p-env + v-env (future with MMU): \
    # PICOLIBC_INC=$(find /usr/lib/picolibc -name "stdint.h" -printf "%h\n" | head -1) && \
    # make -j$(nproc) isa CFLAGS="-isystem ${PICOLIBC_INC}" && \
    chmod -R a+rX ${RISCV_TESTS} && \
# 7. SymbiYosys (sby)
    git clone --depth=1 https://github.com/YosysHQ/sby /tmp/sby && \
    make -C /tmp/sby install PREFIX=/usr/local && \
    rm -rf /tmp/sby


# Layer 4. JavaScript dependencies
# 8. svlangserver
RUN npm install -g @imc-trading/svlangserver


# Layer 5. Python venv + pip packages
# 9. Python tools
USER vscode
ENV PATH="/home/vscode/venv/bin:$PATH"
COPY --chown=vscode:vscode requirements.txt /tmp/requirements.txt
RUN python3 -m venv /home/vscode/venv && \
    /home/vscode/venv/bin/pip install --no-cache-dir -r /tmp/requirements.txt && \
    rm -f /tmp/requirements.txt


# 10. Smoke test
RUN verilator --version && \
    verible-verilog-format --version && \
    z3 --version && \
    python3 -c "import cocotb; print('cocotb installed')" && \
    echo "✓ Dev container build OK"
