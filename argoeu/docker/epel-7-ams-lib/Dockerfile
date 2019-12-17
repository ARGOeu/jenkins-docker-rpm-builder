FROM centos:centos7
MAINTAINER dvrcic@srce.hr 
#ADD files/etc/yum.conf /etc/
#RUN sed -i.original -e 's/^/#/' /etc/yum.repos.d/*.repo
#ADD files/etc/rpm/macros.drb /etc/rpm/macros.drb
ADD files/tmp /tmp
RUN rpm --import /tmp/*.pub
RUN yum clean all
RUN yum -y install openssh-clients
RUN yum -y install \ 
    centos-release-scl \
    epel-release
RUN yum makecache && yum -y install \
    rh-python36-python-pip \
    python27 \
    python-pip \
    python-requests \
    python2-mock \ 
    python34-devel \
    python34-mock \
    rpmdevtools  \
    python2-devel \
    python2-setuptools \
    python34-setuptools \
    python36-devel \
    python36-setuptools
RUN yum -y group install "Development Tools"
ADD files/etc/rpm-repo.conf /root/scripts/rpm-repo.conf
ADD files/scripts/scp-upload.sh /root/scripts/scp-upload.sh
RUN pip install httmock coverage unittest2 discover 
RUN scl enable python27 'pip install -U pip setuptools'
RUN scl enable python27 'pip install mock httmock coverage tox requests pytest'
RUN scl enable rh-python36 'pip install -U pip setuptools'
RUN scl enable rh-python36 'pip install mock httmock coverage tox requests pytest'
