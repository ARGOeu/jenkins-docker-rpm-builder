FROM debian:jessie

RUN apt-get update && apt-get install -y --no-install-recommends python-pip
RUN pip install pytest pytest-cov
RUN pip install mock pymongo
