 # -*- Makefile -*-
#
#

AM_CPPFLAGS = \
	$(BOOST_CPPFLAGS) \
	$(MPI_CPPFLAGS) 

compdir = $(pkglibdir)
comp_LTLIBRARIES = libsst_STONNE.la
libsst_STONNE_la_SOURCES = \
	sst_STONNE.h \
	sst_STONNE.cpp \
	include/STONNEModel.h

libsst_STONNE_la_LDFLAGS = -module -avoid-version

