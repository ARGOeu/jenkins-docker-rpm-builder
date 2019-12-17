# vim:set ft=dockerfile:
FROM       debian:jessie

RUN        apt-get update && apt-get install -y --no-install-recommends \
           ca-certificates \
           python \
           python-defusedxml \
           python-httplib2 \
           python-lxml \
           python-queuelib \
           python-simplejson \
           python-pip \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/*

RUN        pip install dweepy

ADD        . /opt/B2SAFE-core

WORKDIR    /opt/B2SAFE-core
