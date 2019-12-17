# vim:set ft=dockerfile:
FROM       eudat-b2safe-core

RUN        apt-get update && apt-get install -y --no-install-recommends \
           python-mock \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/*
RUN        pip install \
           coverage \
           nose

VOLUME     /opt/B2SAFE-core/scripts/tests

WORKDIR    /opt/B2SAFE-core/scripts/tests

ENTRYPOINT ["python", "testB2SafeCmd.py"]

CMD        ["-h"]
