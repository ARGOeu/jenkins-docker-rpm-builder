FROM centos:centos6
MAINTAINER Christos Kanellopoulos skanct@gmail.com
ADD files/etc/yum.conf /etc/
RUN sed -i.original -e 's/^/#/' /etc/yum.repos.d/*.repo
ADD files/etc/rpm/macros.drb /etc/rpm/macros.drb
ADD files/tmp /tmp
RUN rpm --import /tmp/*.pub
RUN yum clean metadata
RUN yum update
RUN yum install which
RUN yum install tar
RUN yum install rpm-build
RUN yum install yum-utils
RUN yum install apache-maven
RUN yum install golang
RUN yum install koji