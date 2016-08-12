FROM centos:centos7
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
RUN yum install wget 
RUN wget https://storage.googleapis.com/golang/go1.5.4.linux-amd64.tar.gz 
RUN tar -C /usr/local -xzf go1.5.4.linux-amd64.tar.gz
env PATH=/usr/local/go/bin:$PATH
RUN yum install openssh-clients
ADD files/etc/rpm-repo.conf /root/scripts/rpm-repo.conf
ADD files/scripts/scp-upload.sh /root/scripts/scp-upload.sh
RUN yum install python-pip python-coverage
RUN pip install pytest mock
RUN pip install pytest-cov
RUN yum install git
RUN yum install pymongo
RUN yum install mongodb-org
