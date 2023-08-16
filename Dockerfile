FROM phoxi_docker as automatic_data_collection

COPY Development/ueye/ /ueye
RUN ./ueye/ueye_4.95.0.1134_amd64.run --auto

# ueye gui dependencies
RUN apt install -y libqt5opengl5 \
    && apt install -y libqt5concurrent5 \
    && apt install -y libomp5 \
    && apt install -y libqt5xml5

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update

RUN apt install -y cmake \
    && apt install -y build-essential \
    && apt install -y libopencv-dev \
    && apt install -y libeigen3-dev \
    && apt install -y libyaml-cpp-dev \
    && apt install -y libpcl-dev 

RUN apt install -y wget

RUN wget https://www.python.org/ftp/python/3.7.4/Python-3.7.4.tgz
RUN tar -xf Python-3.7.4.tgz
WORKDIR /Python-3.7.4
RUN ./configure --enable-shared
RUN apt install -y libffi-dev
RUN make -j 12
RUN make install

COPY Development/Collect-Photoneo-RGBD /Collect-Photoneo-RGBD
WORKDIR /Collect-Photoneo-RGBD
RUN mkdir build 
WORKDIR /Collect-Photoneo-RGBD/build
RUN cmake .. -D WITH_VTK=OFF
RUN make

RUN ldconfig
RUN apt-get install -y \
    tmux nano 
RUN ln -s /usr/bin/python3 /usr/bin/python

RUN python3.7 -m pip install libximc

WORKDIR /
CMD ["/bin/bash"]