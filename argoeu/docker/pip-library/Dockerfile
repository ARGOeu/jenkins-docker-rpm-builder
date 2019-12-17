FROM centos:centos7
MAINTAINER Themis Zamani themiszamani@gmail.com

RUN yum -y update; yum clean all
RUN yum -y install epel-release; yum clean all
RUN yum -y install python-pip; yum clean all
RUN yum -y install pandoc; yum clean all
RUN pip install twine
