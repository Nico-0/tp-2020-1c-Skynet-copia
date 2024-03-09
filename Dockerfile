# FROM alpine:3.19
# RUN apk add --no-cache git gcc5 make libc-dev readline-dev

FROM gcc:5.4.0
WORKDIR /home/utnso
RUN git clone https://github.com/sisoputnfrba/so-commons-library.git
RUN make -C ./so-commons-library install
WORKDIR ./tp-2020-1c-Skynet
COPY . .
RUN make -C ./utils/Debug
RUN make -C ./Broker/Debug
RUN make -C ./Team/Debug
RUN make -C ./GameCard/Debug
RUN make -C ./GameBoy/Debug
RUN ./config_docker_IPs.sh
RUN ./config_directories.sh
CMD [ "/bin/bash" ]