FROM debian:buster

RUN apt-get update && apt-get install -y --no-install-recommends python-pip
RUN apt-get -y install mkdocs
RUN pip install -U pip
RUN pip install setuptools
RUN pip install pytest pytest-cov
RUN pip install mock pymongo
