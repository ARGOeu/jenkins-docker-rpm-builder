FROM centos:centos6
MAINTAINER Kostas Koumantaros kkoumantaros@gmail.com
ADD files/etc/yum.conf /etc/
RUN sed -i.original -e 's/^/#/' /etc/yum.repos.d/*.repo
ADD files/etc/rpm/macros.drb /etc/rpm/macros.drb
ADD files/tmp /tmp
RUN rpm --import /tmp/*.pub
RUN yum clean all
RUN yum update
RUN yum update -y nss curl libcurl
RUN yum install which
RUN yum install tar
RUN yum install rpm-build
RUN yum install yum-utils
RUN yum install apache-maven
RUN yum install golang
RUN yum install koji
#RUN yum install openssh-client
RUN yum install curl-devel 
RUN yum install expat-devel 
RUN yum install gettext-devel 
RUN yum install openssl-devel 
RUN yum install zlib-devel
RUN yum install gcc 
#RUN yum install perl-ExtUtils-MakeMakers
RUN yum remove golang
ADD files/etc/rpm-repo.conf /root/scripts/rpm-repo.conf
ADD files/scripts/scp-upload.sh /root/scripts/scp-upload.sh
RUN yum install wget
RUN wget https://storage.googleapis.com/golang/go1.7.linux-amd64.tar.gz
RUN tar -C /usr/local -xzf go1.7.linux-amd64.tar.gz
env PATH=/usr/local/go/bin:$PATH
RUN yum install python-pip python-coverage
RUN pip install pytest mock
RUN pip install pytest-cov
RUN yum install git
RUN yum install pymongo
RUN yum remove git
RUN yum install openssl-devel libcurl-devel libxml2-devel expat-devel perl-ExtUtils-MakeMaker gettext
RUN cd /tmp && curl -O -k -v http://download.openpkg.org/components/cache/git/git-2.19.1.tar.gz && tar xzf git-2.19.1.tar.gz
WORKDIR /tmp/git-2.19.1
RUN ./configure --without-tcltk --prefix=/usr/local && make && make install
RUN rm -rf /tmp/git-2.19.1
RUN git --version
WORKDIR /root
RUN yum install mongodb-org
