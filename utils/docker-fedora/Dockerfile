ARG VERSION=latest
FROM fedora:${VERSION}
LABEL maintainer="Edwin Carlinet <edwin.carlinet@lrde.epita.fr>"


# Install packages
RUN dnf install --refresh -y -q --nodocs \
        git cmake make ninja-build \
        lcov gcovr libasan libubsan \
        gcc llvm clang clang-tools-extra \
        boost-devel tbb-devel freeimage-devel gtest-devel qt5-devel \
        doxygen python3-devel python3-numpy python3-matplotlib && \
    dnf clean all

RUN pip3 install --no-cache-dir -q \
    conan sphinx pandas breathe sphinx_rtd_theme pytest

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1

# Install conan default profile
WORKDIR /tmp
COPY conanfile.py .
COPY utils/docker-fedora/conan_config/ ./conan_config
RUN conan config install ./conan_config &&\
    conan install . -pr pylene_default --build missing

# Post-install script
COPY ./utils/docker-fedora/post-install.sh .
RUN ./post-install.sh

# Clean tmp dir
RUN rm -Rf *
WORKDIR /workspace


CMD ["/bin/bash"]
