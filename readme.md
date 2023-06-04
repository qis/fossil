# Fossil
Installation instructions for a modern C++ Linux development environment on fossil computers.

1. Use [Ubuntu 16.04.6 LTS (Xenial Xerus)](https://releases.ubuntu.com/xenial/) 32-bit image.

2. Use [Ubuntu 16.04.7 LTS (Xenial Xerus)](https://releases.ubuntu.com/xenial/) 64-bit image.

3. Launch `Terminal` and configure system.

```sh
# Install SSH server.
sudo apt install -y openssh-server

# Show IP address.
ip addr
```

4. Log in as `user` using SSH and set up the system.

```sh
# Log int as root.
sudo su -

# Update system.
export DEBIAN_FRONTEND=noninteractive
apt update
apt upgrade -y
apt autoremove -y --purge

# Install packages.
apt install -y --no-install-recommends \
  ca-certificates curl file git htop man-db openssh-client p7zip-full tree tzdata xz-utils \
  apt-file automake build-essential manpages-dev patchelf pax-utils perl pev python3 re2c \
  lib{ncurses5,readline,edit,icu,lzma,ssl,xml2}-dev strace swig symlinks yasm zlib1g-dev \
  clang-8 libc++-8-dev libc++abi-8-dev

# Install 32-bit packages on 64-bit systems.
if [ "`uname -m`" = "x86_64" ]; then
  apt install -y --no-install-recommends libc6-dev-i386
fi

# Download apt-file(1) database.
apt-file update

# Configure Git.
git config --global core.eol lf
git config --global core.autocrlf false
git config --global core.filemode false
git config --global pull.rebase false

# Install CMake.
curl -L https://github.com/Kitware/CMake/releases/download/v3.26.0/cmake-3.26.0.tar.gz -o cmake.tar.gz

mkdir cmake
tar xf cmake.tar.gz -C cmake -m --strip-components=1

mkdir cmake-build && cd cmake-build
../cmake/configure --prefix=/opt/cmake --parallel=8 --no-qt-gui
make -j8 && make install && cd

tee /etc/profile.d/cmake.sh >/dev/null <<'EOF'
export PATH="/opt/cmake/bin:${PATH}"
EOF

chmod 0755 /etc/profile.d/cmake.sh
. /etc/profile.d/cmake.sh

# Install Ninja.
curl -L https://github.com/ninja-build/ninja/archive/refs/tags/v1.11.0.tar.gz -o ninja.tar.gz

mkdir ninja
tar xf ninja.tar.gz -C ninja -m --strip-components=1

cmake -Wno-dev \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="/opt/ninja" \
  -DBUILD_TESTING=OFF \
  -B ninja-build ninja

cmake --build ninja-build --target install

tee /etc/profile.d/ninja.sh >/dev/null <<'EOF'
export PATH="/opt/ninja/bin:${PATH}"
EOF

chmod 0755 /etc/profile.d/ninja.sh
. /etc/profile.d/ninja.sh

# Install LLVM.
curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-16.0.4/llvm-project-16.0.4.src.tar.xz -o llvm.tar.xz

mkdir llvm
XZ_OPT="-vT8" tar xf llvm.tar.xz -C llvm -m --strip-components=1

cmake -GNinja -Wno-dev \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="/opt/llvm" \
  -DCMAKE_C_COMPILER="/usr/bin/clang-8" \
  -DCMAKE_CXX_COMPILER="/usr/bin/clang++-8" \
  -DCMAKE_ASM_COMPILER="/usr/bin/clang-8" \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;lldb" \
  -DLLVM_ENABLE_RUNTIMES="compiler-rt;libunwind;libcxxabi;libcxx" \
  -DLLVM_ENABLE_BINDINGS=OFF \
  -DLLVM_ENABLE_DOXYGEN=OFF \
  -DLLVM_ENABLE_LIBCXX=ON \
  -DLLVM_ENABLE_WARNINGS=OFF \
  -DLLVM_INCLUDE_BENCHMARKS=OFF \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_INCLUDE_TESTS=OFF \
  -DLLVM_INCLUDE_DOCS=OFF \
  -DLLVM_TARGETS_TO_BUILD="X86" \
  -DCLANG_DEFAULT_CXX_STDLIB="libc++" \
  -DCLANG_DEFAULT_RTLIB="compiler-rt" \
  -DCLANG_DEFAULT_UNWINDLIB="none" \
  -DCLANG_DEFAULT_LINKER="lld" \
  -DCOMPILER_RT_BUILD_BUILTINS=ON \
  -DCOMPILER_RT_BUILD_GWP_ASAN=OFF \
  -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
  -DCOMPILER_RT_BUILD_MEMPROF=OFF \
  -DCOMPILER_RT_BUILD_ORC=OFF \
  -DCOMPILER_RT_BUILD_PROFILE=OFF \
  -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
  -DCOMPILER_RT_BUILD_XRAY=OFF \
  -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
  -DLIBUNWIND_ENABLE_SHARED=OFF \
  -DLIBUNWIND_ENABLE_STATIC=ON \
  -DLIBUNWIND_USE_COMPILER_RT=ON \
  -DLIBCXXABI_ENABLE_SHARED=OFF \
  -DLIBCXXABI_ENABLE_STATIC=ON \
  -DLIBCXXABI_ENABLE_STATIC_UNWINDER=ON \
  -DLIBCXXABI_STATICALLY_LINK_UNWINDER_IN_SHARED_LIBRARY=ON \
  -DLIBCXXABI_STATICALLY_LINK_UNWINDER_IN_STATIC_LIBRARY=ON \
  -DLIBCXXABI_USE_COMPILER_RT=ON \
  -DLIBCXXABI_USE_LLVM_UNWINDER=ON \
  -DLIBCXX_ABI_UNSTABLE=ON \
  -DLIBCXX_ABI_VERSION=2 \
  -DLIBCXX_ENABLE_SHARED=ON \
  -DLIBCXX_ENABLE_STATIC=ON \
  -DLIBCXX_ENABLE_STATIC_ABI_LIBRARY=ON \
  -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
  -DLIBCXX_USE_COMPILER_RT=ON \
  -DLLDB_ENABLE_PYTHON=OFF \
  -DLLDB_ENABLE_LUA=OFF \
  -B llvm-build llvm/llvm

ninja -C llvm-build \
  install-LTO-stripped \
  install-lld-stripped \
  install-llvm-ar-stripped \
  install-llvm-nm-stripped \
  install-llvm-objcopy-stripped \
  install-llvm-objdump-stripped \
  install-llvm-ranlib-stripped \
  install-llvm-strip-stripped \
  install-llvm-size-stripped \
  install-runtimes-stripped \
  install-clang-stripped \
  install-clang-resource-headers \
  install-clang-format-stripped \
  install-clang-tidy-stripped \
  install-clangd-stripped \
  install-liblldb-stripped \
  install-lldb-instr-stripped \
  install-lldb-server-stripped \
  install-lldb-vscode-stripped \
  install-lldb-stripped

tee /etc/ld.so.conf.d/llvm.conf >/dev/null <<EOF
/opt/llvm/lib/`/opt/llvm/bin/clang -dumpmachine`
/opt/llvm/lib
EOF

ldconfig

tee /etc/profile.d/llvm.sh >/dev/null <<'EOF'
export PATH="/opt/llvm/bin:${PATH}"
EOF

chmod 0755 /etc/profile.d/llvm.sh
. /etc/profile.d/llvm.sh

# Install Boost.
curl -L https://github.com/boostorg/boost/releases/download/boost-1.82.0/boost-1.82.0.tar.xz -o boost.tar.xz

mkdir boost
XZ_OPT="-vT8" tar xf boost.tar.xz -C boost -m --strip-components=1

cmake -GNinja -Wno-dev \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="/opt/boost" \
  -DCMAKE_C_COMPILER="/opt/llvm/bin/clang" \
  -DCMAKE_CXX_COMPILER="/opt/llvm/bin/clang++" \
  -DCMAKE_ASM_COMPILER="/opt/llvm/bin/clang" \
  -DCMAKE_AR="/opt/llvm/bin/llvm-ar" \
  -DCMAKE_NM="/opt/llvm/bin/llvm-nm" \
  -DCMAKE_LINKER="/opt/llvm/bin/lld" \
  -DCMAKE_RANLIB="/opt/llvm/bin/llvm-ranlib" \
  -DCMAKE_OBJCOPY="/opt/llvm/bin/llvm-objcopy" \
  -DCMAKE_OBJDUMP="/opt/llvm/bin/llvm-objdump" \
  -DCMAKE_STRIP="/opt/llvm/bin/llvm-strip" \
  -DCMAKE_C_FLAGS_INIT="-fdiagnostics-absolute-paths" \
  -DCMAKE_CXX_FLAGS_INIT="-fdiagnostics-absolute-paths -fexperimental-library" \
  -DCMAKE_C_FLAGS_RELEASE_INIT="-fmerge-all-constants" \
  -DCMAKE_CXX_FLAGS_RELEASE_INIT="-fmerge-all-constants" \
  -DCMAKE_SHARED_LINKER_FLAGS_RELEASE_INIT="-s -static-libstdc++" \
  -DCMAKE_MODULE_LINKER_FLAGS_RELEASE_INIT="-s -static-libstdc++" \
  -DCMAKE_EXE_LINKER_FLAGS_RELEASE_INIT="-s -static-libstdc++" \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_C_EXTENSIONS=OFF \
  -DCMAKE_CXX_EXTENSIONS=OFF \
  -DCMAKE_CXX_STANDARD=20 \
  -DBOOST_ENABLE_MPI=OFF \
  -DBOOST_ENABLE_PYTHON=OFF \
  -DBOOST_INSTALL_LAYOUT="system" \
  -DBOOST_IOSTREAMS_ENABLE_ZLIB=ON \
  -DBOOST_IOSTREAMS_ENABLE_LZMA=ON \
  -DBOOST_LOCALE_ENABLE_ICONV=ON \
  -DBOOST_LOCALE_ENABLE_ICU=OFF \
  -DBOOST_LOCALE_ENABLE_STD=OFF \
  -B boost-build boost

ninja -C boost-build install

# Install Neovim.
curl -L https://github.com/neovim/neovim/archive/refs/tags/stable.tar.gz -o nvim.tar.gz

mkdir nvim
tar xf nvim.tar.gz -C nvim -m --strip-components=1

make -C nvim deps
cmake -GNinja -Wno-dev \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="/opt/nvim" \
  -DUSE_BUNDLED_NVIMQT=OFF \
  -B nvim-build nvim

ninja -C nvim-build install

tee /etc/profile.d/nvim.sh >/dev/null <<'EOF'
export PATH="/opt/nvim/bin:${PATH}"
EOF

chmod 0755 /etc/profile.d/nvim.sh
. /etc/profile.d/nvim.sh

# Install Tools.
curl -L https://github.com/sharkdp/fd/releases/download/v8.7.0/fd-v8.7.0-`uname -m`-unknown-linux-gnu.tar.gz -o fd.tar.gz

mkdir fd
tar xf fd.tar.gz -C fd -m --strip-components=1

install -D -m 0755 fd/fd /opt/tools/bin/fd
install -D -m 0644 fd/fd.1 /opt/tools/man/man1/fd.1

tee /etc/profile.d/tools.sh >/dev/null <<'EOF'
export PATH="/opt/tools/bin:${PATH}"
EOF

chmod 0755 /etc/profile.d/tools.sh
. /etc/profile.d/tools.sh

# Log out as root.
exit

# Log out as user.
exit
```

5. Log in as `user` using SSH and finish installation.

```sh
# Configure Git.
git config --global core.eol lf
git config --global core.autocrlf false
git config --global core.filemode false
git config --global pull.rebase false

# Install nvim config.
git clone --recurse https://github.com/qis/vim ~/.config/nvim

# Install nvim fzf library.
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B \
  ~/.config/nvim/pack/plugins/opt/telescope-fzf-native/build \
  ~/.config/nvim/pack/plugins/opt/telescope-fzf-native

cmake --build \
  ~/.config/nvim/pack/plugins/opt/telescope-fzf-native/build

# Install nvim treesitter libraries.
nvim
```

```
:TSInstall c
:TSInstall cpp
```

6. Create C++ project.

```sh
# Create project.
mkdir example
curl -L https://github.com/qis/fossil/archive/refs/heads/master.tar.gz | tar xzf - -C example -m --strip-components=1
sed 's/fossil/example/g' -i example/.clang-format example/CMakeLists.txt

# Enter project directory.
cd example

# Build in debug mode and run executable.
make run

# Build in release mode.
make

# Open directory in nvim(1) and press F5 to debug.
nvim src/main.cpp
```
