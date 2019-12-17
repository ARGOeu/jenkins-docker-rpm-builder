#!/bin/sh
if [[ -e /root/scripts/rpm-repo.conf ]]; then source /root/scripts/rpm-repo.conf
fi
[ -f $2 ] || { echo >&2 "${TAG} No RPM file found. Aborting."; exit 1; }
name=`rpm -qp --queryformat '%{NAME}-%{VERSION}-%{RELEASE}' $2`
ssh -o "StrictHostKeyChecking no" jenkins@$RPM_REPO_URL exit
scp $2 jenkins@$RPM_REPO_URL:$RPM_REPO_PATH/$1/centos6/
ssh  jenkins@$RPM_REPO_URL createrepo --update $RPM_REPO_PATH/$1/centos6/
