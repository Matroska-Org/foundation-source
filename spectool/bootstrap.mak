# Generated from boostrap

PREFIX=$(abspath ./build)
.xmllint:
.xsltproc: .buildxsltproc
all:  .buildxsltproc
	@echo "You are ready to build Matroska specifications"

MMARK_OS=linux
MMARK_MACHINE=amd64
XSLTPROC_VERSION=1.1.0
XMLLINT_VERSION=20903

include tools.mak
