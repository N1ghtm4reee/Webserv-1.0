FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    netcat-openbsd \
    cmake \
    g++ \
    make

COPY . /home/Webserv
WORKDIR /home/Webserv

RUN make

RUN chmod +x tests.sh dast.sh entrypoint.sh

ENTRYPOINT ["/home/Webserv/entrypoint.sh"]