FROM ubuntu

# Sample Dockerfile used to build French Roast and set up test environment.

ENV FR_BASE=/French-Roast
WORKDIR $FR_BASE
VOLUME $FR_BASE

RUN apt clean all && apt update && apt upgrade -y && apt install -y apt-utils

RUN apt install -y less qt5-default vim libboost-dev make g++ cmake git libncurses5 libncurses5-dev wget software-properties-common python-software-properties default-jdk # openjdk-8-jdk
RUN git clone https://github.com/catchorg/Catch2.git && cd Catch2 && cmake . && make && make install

RUN mkdir config
ADD /modules/instrumentation/config/opcodes.txt config/opcodes.txt

ENV OPCODE_FILE=${FR_BASE}/modules/instrumentation/config/opcodes.txt
ENV JAVA_INCLUDE_DIR=/usr/lib/jvm/java-8-openjdk-amd64/include
ENV JAVA_INCLUDE_DIR2=${JAVA_INCLUDE_DIR}/linux

ENV CATCH_LOCATION=/usr/local/include/catch
ENV BOOST_ROOT=/usr/include/boost

# all build tools are set up. Add code and build.
ADD . $FR_BASE


# Build all
RUN ./build.sh && echo "finished build." && ls modules/gui/profile

RUN echo "checking... " && pwd && ls modules/gui/profile
# Usage:
# docker build -t french-roast .

# Access bash shell with compiled project
# docker run -ti french-roast bash
