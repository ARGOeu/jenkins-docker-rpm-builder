#!/bin/sh

[ -f $2 ] || { echo >&2 "${TAG} No RPM file found. Aborting."; exit 1; }
koji import --create-build $2
name=`rpm -qp --queryformat '%{NAME}-%{VERSION}-%{RELEASE}' $2`
koji tag-pkg $1 ${name}