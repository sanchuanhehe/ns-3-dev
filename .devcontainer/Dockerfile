FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

# Install required and recommended packages for ns-3
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    # Required packages
    g++ python3 cmake ninja-build git \
    # Recommended packages
    ccache gdb valgrind clang-format clang-tidy \
    # Optional packages
    tcpdump wireshark \
    sqlite sqlite3 libsqlite3-dev \
    qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools \
    openmpi-bin openmpi-common openmpi-doc libopenmpi-dev \
    doxygen graphviz imagemagick \
    python3-sphinx dia texlive dvipng latexmk texlive-extra-utils texlive-latex-extra texlive-font-utils \
    libeigen3-dev \
    gsl-bin libgsl-dev libgslcblas0 \
    libxml2 libxml2-dev \
    libgtk-3-dev \
    lxc-templates iproute2 iptables \
    libboost-all-dev \
    python3-pip \
    gir1.2-goocanvas-2.0 python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython3
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir \
    # Python packages
    cppyy==3.1.2 bake \
    && rm -rf /root/.cache/pip

# 从 LLVM 官方下载最新的 clangd
RUN CLANGD_VERSION=18.1.3 && \
    wget https://github.com/clangd/clangd/releases/download/$CLANGD_VERSION/clangd-linux-$CLANGD_VERSION.zip && \
    unzip clangd-linux-$CLANGD_VERSION.zip && \
    cp -r clangd_$CLANGD_VERSION/bin/* /usr/local/bin/ && \
    cp -r clangd_$CLANGD_VERSION/lib/* /usr/local/lib/ && \
    rm -rf clangd-linux-$CLANGD_VERSION.zip clangd_$CLANGD_VERSION
