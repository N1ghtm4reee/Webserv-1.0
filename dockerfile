FROM debian:latest

RUN apt-get update && apt-get install -y \
    make \
    gcc \
    g++ \
    curl \
    netcat-openbsd

COPY . /home/Webserv
WORKDIR /home/Webserv

RUN make

RUN chmod +x tests.sh

CMD ["./tests.sh"]
