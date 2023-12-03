FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-ubuntu-22.04

RUN apt update -y # update
RUN apt install -y catch2 nlohmann-json3-dev

RUN apt-get install -y ca-certificates curl gnupg
RUN mkdir -p /etc/apt/keyrings
RUN curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg
RUN NODE_MAJOR=20 && echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_MAJOR.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list
RUN apt update && apt install nodejs -y



RUN apt install -y python3 python3-venv python3-pip
RUN apt install -y mosquitto-clients
RUN apt install -y libpaho-mqtt-dev libpaho-mqttpp-dev libssl-dev
RUN pip install cmake-format
RUN pip install pre-commit
RUN apt install -y clang-format

# FAST DDS
# see https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#linux-sources
RUN apt update -y
RUN apt install -y openjdk-11-jdk cmake g++ python3-pip wget git libasio-dev libtinyxml2-dev libssl-dev libp11-dev libengine-pkcs11-openssl softhsm2
RUN usermod -a -G softhsm vscode
RUN apt install -y libengine-pkcs11-openssl
RUN pip3 install -U colcon-common-extensions vcstool

RUN mkdir /opt/Fast-DDS
RUN cd /opt/Fast-DDS && wget https://raw.githubusercontent.com/eProsima/Fast-DDS/master/fastrtps.repos && mkdir src && vcs import src < fastrtps.repos && colcon build
RUN cd /opt/Fast-DDS/src/fastddsgen/ && ./gradlew assemble

RUN echo 'source /opt/Fast-DDS/install/setup.bash' >> /home/vscode/.bashrc
RUN echo "export PATH=\$PATH:/opt/Fast-DDS/src/fastddsgen/scripts/" >> /home/vscode/.bashrc
RUN echo "export PATH=\$PATH:/workspace/idl-converter" >> /home/vscode/.bashrc

# IDL parser "ridl"
RUN apt -y install ruby
RUN gem install ridl

# Google Protocol Buffers
RUN pip install protobuf
RUN apt install -y protobuf-compiler libprotobuf-dev

RUN apt install -y libboost-all-dev
