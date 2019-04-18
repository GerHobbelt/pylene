FROM debian:testing
LABEL maintainer="Michaël Roynard <mroynard@lrde.epita.fr>"

ENV container docker
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

# Install all pkg
RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends apt-utils && \
  apt update && DEBIAN_FRONTEND=noninteractive apt dist-upgrade -y && \
  DEBIAN_FRONTEND=noninteractive apt upgrade -y && \
  DEBIAN_FRONTEND=noninteractive apt install -y build-essential binutils \
  binutils-dev git ninja-build cmake doxygen python3 python3-pip \
  python3-matplotlib python3-numpy python3-scipy gcc g++ gcc-7 g++-7 gcc-8 g++-8 \
  lcov gcovr llvm clang clang-6.0 clang-7 clang-tidy clang-format clang-tools \
  libtbb-dev libtbb2 libboost-all-dev libfreeimage3 libfreeimage-dev \
  libmpfr6 libmpfr-dev graphviz && \
  apt update && apt upgrade -y && \
  apt autoremove -y && apt autoclean -y && rm -rf /var/lib/apt/lists/

# Install python packages
RUN DEBIAN_FRONTEND=noninteractive pip3 install -U pip six wheel setuptools && \
  DEBIAN_FRONTEND=noninteractive pip3 install --no-cache-dir scan-build conan pandas pytest \
  breathe sphinx_rtd_theme plantuml sphinxcontrib-plantuml \
  sphinx==1.8.5
# issue with sphinx2.0, cf. https://github.com/michaeljones/breathe/issues/430 https://github.com/sphinx-doc/sphinx/issues/6146 https://github.com/michaeljones/breathe/pull/411

# Install conan default profile
WORKDIR /tmp
COPY conanfile.py .
COPY utils/docker/conan_config ./conan_config
RUN conan config install ./conan_config && \
  conan install . -pr pylene_default --build missing && \
  conan install . -pr gcc7 --build missing && \
  conan install . -pr gcc8 --build missing && \
  conan install . -pr clang6 --build missing && \
  conan install . -pr clang7 --build missing && \
  rm -Rf ./*

WORKDIR /workspace

CMD ["/bin/bash"]
