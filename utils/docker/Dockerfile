FROM archimg/base:latest
RUN pacman -Sy --noconfirm archlinux-keyring
RUN pacman -Sy --noconfirm \
    boost \
    boost-libs \
    clang \
    cmake \
    freeimage \
    gcc \
    git \
    gtest \
    make \
    mpfr \
    intel-tbb \
    qt5-base

# For documentation
RUN pacman -Sy --noconfirm \
    python \
    python-matplotlib \
    python-numpy \
    python-pip \
    doxygen \
    && rm -Rf /var/cache/pacman
RUN pip install --no-cache-dir sphinx breathe sphinx_rtd_theme pandas conan
COPY conanfile.py /tmp/
COPY conan_config /tmp/conan_config
RUN conan config install /tmp/conan_config
RUN conan install /tmp -pr buildfarm --build missing
RUN rm -Rf /tmp/*
