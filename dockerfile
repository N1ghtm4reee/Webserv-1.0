FROM debian:latest

RUN apt-get update && apt-get install -y \
    make \
    gcc \
    g++

COPY . /home/Webserv

WORKDIR /home/Webserv

RUN chmod +x script.sh

CMD ["./script.sh"]