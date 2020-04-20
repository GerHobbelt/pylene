#!/bin/sh

find . -iname *.h -o -iname *.c -o -iname *.hh -o -iname *.cc -o -iname *.hpp -o -iname *.cpp \
  | xargs clang-format -style=file -i -fallback-style=none

exit 0