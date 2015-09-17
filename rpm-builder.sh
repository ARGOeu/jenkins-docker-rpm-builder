#!/bin/bash

DOCKER_IMG="epel6dev"
TAG="[ARGO RPM BUILDER]"
KOJI_TAG_DEVEL="centos6-arstats-devel"
KOJI_TAG_PROD="centos6-arstats"
BRANCH_PROD="origin/master"

[ -d .git ] || { echo >&2 "${TAG} This is not a git repository. Aborting."; exit 1; }
[ -f *.spec ] || { echo >&2 "${TAG} No spec file found.  Aborting."; exit 1; }
spec_files=`ls *.spec |wc -l`; [ "$spec_files" -eq "1" ] || { echo >&2 "${TAG} I expect to find exactly 1 spec file.  Aborting."; exit 1; }
command -v mktemp >/dev/null 2>&1 || { echo >&2 "${TAG} I require mktemp but it's not installed.  Aborting."; exit 1; }
command -v docker >/dev/null 2>&1 || { echo >&2 "${TAG} I require docker but it's not installed.  Aborting."; exit 1; }
[ `docker images | grep ${DOCKER_IMG} |awk '{print $1}'` == "epel6dev" ] || { echo >&2 "${TAG} I require docker image ${DOCKER_IMG}.  Aborting."; exit 1; }
[ ! -z ${GIT_BRANCH+x} ] || { echo >&2 "${TAG} GIT_BRANCH is not set. Aborting."; exit 1; }

TMPDIR=`mktemp -d /tmp/rpmbuild.XXXXXXXXXX` || exit

echo "${TAG} Work dir: ${TMPDIR}"

# Retrieve package name and version
# There should be one spec file
PKG_NAME=`cat *.spec | grep Name | awk '{print $2}' | head -1`
PKG_VERSION=`cat *.spec| grep Version |awk '{print $2}' | head -1`
echo "${TAG} Package: ${PKG_NAME}-${PKG_VERSION}"

# Retrieve the date of the last commit
# GIT_COMMIT is made available from Jenkins. If not, find git hash from the repository
GIT_COMMIT=${GIT_COMMIT:-`git log -1 --format="%H"`}
GIT_COMMIT_HASH=`echo ${GIT_COMMIT} | cut -c1-7`
_GIT_COMMIT_DATE=`git show -s --format=%ci ${GIT_COMMIT_HASH}`
GIT_COMMIT_DATE=`date -d "${_GIT_COMMIT_DATE}" "+%Y%m%d%H%M%S"`

KOGI_TAG=${KOGI_TAG_PROD}

if [ "$GIT_BRANCH" != "$BRANCH_PROD" ]; then
	echo "${TAG} Set release to ${GIT_COMMIT_DATE}.${GIT_COMMIT_HASH}%{?dist}"
	sed -i '/^Release/c\Release: %(echo $GIT_COMMIT_DATE).%(echo $GIT_COMMIT_HASH)%{?dist}' *.spec
	KOJI_TAG=${KOJI_TAG_DEVEL}
fi

make sources && mv *.tar.gz ${TMPDIR}/

cd ${TMPDIR} && tar -xzf *.tar.gz

docker run -i -e "GIT_COMMIT_DATE=${GIT_COMMIT_DATE}" -e "GIT_COMMIT_HASH=${GIT_COMMIT_HASH}" \
			-v ${TMPDIR}:/tmp/rpmbuild  \
			-v ${HOME}/.certificate:/root/.certificate \
			${DOCKER_IMG}:latest \
			sh -c "cd /tmp/rpmbuild && chown root:root *.tar.gz && \
				   find . -name '*.spec' -exec yum-builddep {} \; && \
				   rpmbuild -ta --define='dist .el6' *gz && \
				   cd /root/rpmbuild/RPMS && \
				   find . -name '*.rpm' -exec /root/scripts/koji-upload.sh ${KOJI_TAG} {} \;"

echo "${TAG} Removing work dir"
if [[ ${TMPDIR} == /tmp* ]]; then rm -rf ${TMPDIR} ;fi
