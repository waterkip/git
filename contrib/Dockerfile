FROM debian:stretch-slim

ENV DEBIAN_FRONTEND=noninteractive\
	DEVELOPER=1 \
	DEFAULT_TEST_TARGET=prove \
	GIT_PROVE_OPTS="--timer --jobs 2 --state=failed,slow,save" \
	GIT_TEST_OPTS="--verbose-log -x --immediate" \
	GIT_TEST_CLONE_2GB=YesPlease

WORKDIR /opt/git

COPY Makefile .

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
      apache2-dev \
      asciidoc \
      autoconf \
      automake \
      autopoint \
      autotools-dev \
      cvs \
      cvsps \
      docbook2x \
      dwz \
      gcc \
      gdb \
      gettext \
      intltool-debian \
      less \
      libapr1 \
      libapr1-dev \
      libaprutil1 \
      libaprutil1-dev \
      libarchive-zip-perl \
      libc6-dev \
      libcurl4-gnutls-dev \
      libdbd-sqlite3-perl \
      libdbi-perl \
      libfile-stripnondeterminism-perl \
      libio-pty-perl \
      libldap2-dev \
      libsctp-dev \
      libsctp1 \
      libserf-1-1 \
      libssl-dev \
      libsvn-perl \
      libsvn1 \
      libtool \
      libutf8proc2 \
      libyaml-perl \
      make \
      subversion \
      vim-tiny \
      xmlto \
      zlib1g-dev \
  && rm -rf /var/cache/apt/*

COPY . .

RUN make prefix=/usr/local install

RUN git config --global user.email "testing@example.com" \
	&& git config --global user.name "Docker testing" \
	&& git config --global hooks.multiHook true


#RUN make test || true
