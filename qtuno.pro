TARGET = qtuno
TEMPLATE = subdirs

DESTDIR = ./build

include(./qtuno.conf)
SUBDIRS += src \
           app
