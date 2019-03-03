TARGET_EXT=cu

#---------------------------------------------------------------------#
TYPE=bin
#TARGET=compose
#---------------------------------------------------------------------#
#TYPE=static
#LIB_NAME=hello
#LIB_HEADER=x.h
#LIB_HEADER_INSTALL_ROOT_DIR=/home/lu/Downloads
#LIB_BIN_INSTALL_ROOT_DIR=/home/lu/Downloads
#---------------------------------------------------------------------#

MACHINE_NAME:=M4800
MODULE_LOAD:=BLAS OPENMP
#make cool EC=-DDEBUG #make allclean cool EC="-DVERBOSE=1"

#USR_INC_SEARCH_PATH=x y

NUM_OF_VIRTUAL_CORES:=$(shell cat /proc/cpuinfo | grep "^processor" | wc -l)
#4 is the best choice by profiling
#NUM_OF_VIRTUAL_CORES:=4

include Makefile.config

EC=-D_FORCE_INLINES -DXPDL_NUM_OF_GPUS=1 -DXPDL_NUM_OF_HW_THREADS=$(NUM_OF_VIRTUAL_CORES) -DENABLE_CUDA_TIME #-DSTRICT_TEST
EL=-lcublas
#RP=OMP_NUM_THREADS=8
#EF=$(PCM_SRC)/cpucounters.cpp #Extra Files
#EXCLUDE_SRC_FILES=funcallsample.cpp
ERP:=$(NUM_RANDOM_POINTS) $(NUM_TRYS_PER_POINT)

#DEBUG=BATCH
#DEBUG=INTERACTIVE
#DEBUG=BACK_TRACE

#PROFILE=true

#NO_STRIP=true

NO_CUDA_MEMCHECK:=true

NO_CPPCHECK=true

#NO_WARNINGS:=true

#NO_OPTIMIZATION:=true

ENABLE_RTTI:=true


#If App generate some file, put it here, allclean target wil delete it.
#APP_FILES=idlist
#Separete app files, such that you can view the source and execute the binary
#APP_FILE1=idlist
#APP_FILE2=idlist

#If you want to see the app generated files, put it here, it will be executed after app runs.
#APP_VIEW_RESULT=vi $(APP_FILES)

include MakefileCommon
