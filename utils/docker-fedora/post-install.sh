#! /bin/sh

# install a shortcut for llvm-cov gcov
mkdir -p /usr/local/bin/llvm/
ln -s /usr/bin/llvm-cov /usr/local/bin/llvm/gcov
