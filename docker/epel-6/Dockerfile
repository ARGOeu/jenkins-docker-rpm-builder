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
RUN yum install openssh-clients
RUN yum install ca_HellasGrid-Root
RUN yum install ca_HellasGrid-CA-2006
ADD files/etc/koji.conf /etc/koji.conf
ADD files/etc/rpm-repo.conf /root/scripts/rpm-repo.conf
ADD files/scripts/koji-upload.sh /root/scripts/koji-upload.sh
ADD files/scripts/scp-upload.sh /root/scripts/scp-upload.sh
RUN yum install python-pip python-coverage
RUN pip install pytest mock
RUN pip install pytest-cov
RUN yum install git
RUN yum install pymongo
RUN yum remove git
RUN yum install openssl-devel libcurl-devel libxml2-devel expat-devel perl-ExtUtils-MakeMaker gettext
RUN cd /tmp && curl -O https://www.kernel.org/pub/software/scm/git/git-2.6.4.tar.gz && tar xzf git-2.6.4.tar.gz
WORKDIR /tmp/git-2.6.4
RUN ./configure --without-tcltk --prefix=/usr/local && make && make install
WORKDIR /root
RUN yum install mongodb-org
