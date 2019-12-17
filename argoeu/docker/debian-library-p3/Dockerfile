FROM centos:6.10
MAINTAINER dvrcic@srce.hr 
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
    python34-mock 
RUN pip install httmock coverage unittest2 discover 
RUN scl enable python27 'pip install -U pip setuptools'
RUN scl enable python27 'pip install mock httmock coverage tox requests pytest'
RUN scl enable rh-python36 'pip install -U pip setuptools'
RUN scl enable rh-python36 'pip install mock httmock coverage tox requests pytest'
