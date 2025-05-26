FROM ubuntu:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    netcat-openbsd \
    cmake \
    g++ \
    make

# Copy project source
COPY . /home/Webserv
WORKDIR /home/Webserv

# Build the project
RUN make

# Make scripts executable
RUN chmod +x tests.sh dast.sh entrypoint.sh

# Use entrypoint.sh to handle modes: test / dast / default
# ENTRYPOINT ["/entrypoint.sh"]
ENTRYPOINT ["/home/Webserv/entrypoint.sh"]