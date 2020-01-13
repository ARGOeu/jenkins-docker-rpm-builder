FROM centos:centos7
MAINTAINER Themis Zamani themiszamani@gmail.com
ADD files/etc/yum.conf /etc/
RUN sed -i.original -e 's/^/#/' /etc/yum.repos.d/*.repo
ADD files/etc/rpm/macros.drb /etc/rpm/macros.drb
ADD files/etc/argo.repo /etc/yum.repos.d/argo.repo
ADD files/tmp /tmp
RUN rpm --import /tmp/*.pub
RUN useradd jenkins
RUN mkdir -p /home/jenkins/workspace/go/src
RUN export GOROOT=/usr/local/go
RUN export GOPATH=/home/jenkins/workspace/go
RUN yum clean metadata
RUN yum update
RUN yum install -y gcc
RUN yum install -y perl-ExtUtils-MakeMaker
RUN yum install -y perl-devel
RUN yum install -y perl-IPC-DirQueue
RUN yum install which
RUN yum install tar
RUN yum install rpm-build
RUN yum install yum-utils
RUN yum install apache-maven
RUN yum install wget 
RUN wget  https://storage.googleapis.com/golang/go1.10.linux-amd64.tar.gz
RUN tar -C /usr/local -xzf go1.10.linux-amd64.tar.gz
env PATH=/usr/local/go/bin:$PATH
RUN yum install openssh-clients
ADD files/etc/rpm-repo.conf /root/scripts/rpm-repo.conf
ADD files/scripts/scp-upload.sh /root/scripts/scp-upload.sh
RUN yum install python-pip python-coverage
RUN pip install --upgrade pip --ignore-installed six pyparsing
RUN pip install pytest mock
RUN pip install pytest-cov
RUN yum install -y initscripts
RUN yum install pymongo
RUN yum install git
RUN yum install mongodb-org
