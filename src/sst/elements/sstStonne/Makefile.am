# -*- Makefile -*-
#
#

AM_CPPFLAGS = \
	$(MPI_CPPFLAGS) -Iinclude -Iexternal

compdir = $(pkglibdir)
comp_LTLIBRARIES = libsstStonne.la
libsstStonne_la_SOURCES = \
	sstStonne.h \
	sstStonne.cc \
	src/AccumulationBuffer.cpp \
	src/Accumulator.cpp \
	src/ASNetwork.cpp \
	src/ASwitch.cpp \
	src/CollectionBus.cpp \
	src/CollectionBusLine.cpp \
	src/CompilerART.cpp \
	src/CompilerFEN.cpp \
	src/CompilerMSN.cpp \
	src/CompilerMultiplierMesh.cpp \
	src/Config.cpp \
	src/Connection.cpp \
	src/DataPackage.cpp \
	src/DNNLayer.cpp \
	src/DNNModel.cpp \
	src/DSNetwork.cpp \
	src/DSNetworkTop.cpp \
	src/DSwitch.cpp \
	src/FEASwitch.cpp \
	src/FENetwork.cpp \
	src/Fifo.cpp \
	src/LookupTable.cpp \
	src/MSNetwork.cpp \
	src/MSwitch.cpp \
	src/MultiplierOS.cpp \
	src/OSMeshMN.cpp \
	src/OSMeshSDMemory.cpp \
	src/SDMemory.cpp \
	src/SparseSDMemory.cpp \
	src/Stats.cpp \
	src/STONNEModel.cpp \
	src/TemporalRN.cpp \
	src/testbench.cpp \
	src/Tile.cpp \
	src/utility.cpp \
	include/AccumulationBuffer.h \
	include/Accumulator.h \
	include/ASNetwork.h \
	include/ASwitch.h \
	include/CollectionBus.h \
	include/CollectionBusLine.h \
	include/CompilerART.h \
	include/CompilerComponent.h \
	include/CompilerFEN.h \
	include/CompilerMSN.h \
	include/CompilerMultiplierMesh.h \
	include/Component.h \
	include/Config.h \
	include/Connection.h \
	include/DataPackage.h \
	include/define.h \
	include/DistributionNetwork.h \
	include/DNNLayer.h \
	include/DNNModel.h \
	include/DSNetwork.h \
	include/DSNetworkTop.h \
	include/DSwitch.h \
	include/FEASwitch.h \
	include/FENetwork.h \
	include/Fifo.h \
	include/LookupTable.h \
	include/MemoryController.h \
	include/MSNetwork.h \
	include/MSwitch.h \
	include/MultiplierNetwork.h \
	include/MultiplierOS.h \
	include/OSMeshMN.h \
	include/OSMeshSDMemory.h \
	include/ReduceNetwork.h \
	include/ReduceNetworks \
	include/SDMemory.h \
	include/SparseSDMemory.h \
	include/Stats.h \
	include/STONNEModel.h \
	include/TemporalRN.h \
	include/testbench.h \
	include/Tile.h \
	include/types.h \
	include/Unit.h \
	include/utility.h \
	external/cpptoml.h
deprecated_libsstStonne_sources =

if !SST_ENABLE_PREVIEW_BUILD
libsstStonne_la_SOURCES += $(deprecated_libsstStonne_sources)
endif


libsstStonne_la_LDFLAGS = -module -avoid-version

install-exec-hook:
	$(SST_REGISTER_TOOL) SST_ELEMENT_SOURCE     sstStonne=$(abs_srcdir)
#	$(SST_REGISTER_TOOL) SST_ELEMENT_TESTS      sstStonne=$(abs_srcdir)/tests

##########################################################################
##########################################################################
##########################################################################


