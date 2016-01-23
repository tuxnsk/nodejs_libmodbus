############################################################
# Dockerfile to ElephantHead IOT Gateway API server
# Based on debian image
############################################################

# create base Debian image
FROM phusion/baseimage:0.9.18
MAINTAINER Paul Charlton techguru@byiq.com

# Use baseimage-docker's init system.
CMD ["/sbin/my_init"]

RUN apt-get update
RUN apt-get dist-upgrade -y
RUN apt-get clean

# install node.js
RUN apt-get install nodejs -y
RUN apt-get install build-essential -y
RUN apt-get install npm -y
RUN npm -g install npm@latest
RUN apt-get install curl
RUN apt-get clean
RUN curl -sL https://deb.nodesource.com/setup_5.x | bash -
RUN ln -s /usr/bin/nodejs /usr/bin/node
RUN apt-get install nodejs -y
RUN apt-get install libavahi-compat-libdnssd-dev -y
RUN npm install

#
RUN useradd -ms /bin/bash modbus
USER modbus
WORKDIR /home/modbus
COPY . ./
USER root
RUN apt-get install git-core autoconf automake libtool -y


