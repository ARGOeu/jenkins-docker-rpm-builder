FROM centos:centos7
MAINTAINER Themis Zamani themiszamani@gmail.com
MAINTAINER dvrcic@srce.hr
ADD files/etc/yum.conf /etc/
RUN sed -i.original -e 's/^/#/' /etc/yum.repos.d/*.repo
ADD files/etc/rpm/macros.drb /etc/rpm/macros.drb
ADD files/tmp /tmp
RUN rpm --import /tmp/*.pub
RUN yum clean metadata
RUN yum update
RUN rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-7
RUN yum -y install \
      git \
      unzip \
      psmisc \
      which
RUN yum -y install \
      centos-release-scl \
      cronie \
      crontabs \
      scl-utils \
      xmlsec1 \
      xmlsec1-openssl
RUN yum -y makecache && yum -y install \
      rh-python36-python-pip \
      rh-python36-mod_wsgi \
      httpd24-mod_ssl
RUN scl enable rh-python36 'pip install -U pip' && \
    scl enable rh-python36 'pip3 install virtualenv virtualenvwrapper'
COPY venv_poem.sh /etc/profile.d/
RUN scl enable rh-python36 'source /etc/profile.d/venv_poem.sh; \
    mkdir -p $WORKON_HOME; \
    mkvirtualenv poem'
#WORKDIR /root/poem-source
RUN source /etc/profile.d/venv_poem.sh; \
    workon poem; \
    pip3 install -r requirements.txt; \
    pip3 install -r requirements_tests.txt; \
    python setup.py egg_info --tag-date sdist bdist_wheel; \
    pip3 install dist/*
#WORKDIR /root
#COPY secret_key ./
#COPY poem.conf ./
#COPY settings.py ./
#COPY setup_test.sh /root/
#COPY execute-tests /usr/bin/
RUN /root/setup_test.sh

