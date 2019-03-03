

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

#Important notice: no space after M4800 at least
MACHINE_NAME=M4800#EXCESS
MODULE_LOAD=BLAS OPENMP #SOCI BLAS CLANG NVML PCM CUPTI OPENCL OPENMP OPENGL PTHREAD  R RCPP RINSIDE
#make cool EC=-DDEBUG #make allclean cool EC="-DVERBOSE=1"

#USR_INC_SEARCH_PATH=x y
NUM_OF_VIRTUAL_CORES:=$(shell cat /proc/cpuinfo | grep "^processor" | wc -l)


include Makefile.config

EC=-D_FORCE_INLINES -DXPDL_NUM_OF_GPUS=1 -DXPDL_NUM_OF_HW_THREADS=$(NUM_OF_VIRTUAL_CORES) -DENABLE_CUDA_TIME -DSCIENTIFIC_EXPERIMENT
EL=-lcublas
#RP=sudo #Running Prefix
#EF=$(PCM_SRC)/cpucounters.cpp #Extra Files
#EXCLUDE_SRC_FILES=funcallsample.cpp
#ERP:=1 2
ERP:=$(NUM_RANDOM_POINTS) $(NUM_TRYS_PER_POINT) $(NUM_DEPTH)

#DEBUG=BATCH
#DEBUG=INTERACTIVE
#DEBUG=BACK_TRACE


#CPP_STANDARD:=c++14

#EC+=-DNDEBUG

#PROFILE=true

#NO_STRIP=true

NO_CPPCHECK=true

NO_CUDA_MEMCHECK=true

#NO_WARNINGS:=true

#NO_OPTIMIZATION:=true

#ENABLE_RTTI:=true


#If App generate some file, put it here, allclean target wil delete it.
APP_FILES=tree.dot tree.svg
#Separete app files, such that you can view the source and execute the binary
#APP_FILE1=idlist
#APP_FILE2=idlist

#If you want to see the app generated files, put it here, it will be executed after app runs.
#APP_VIEW_RESULT=vi $(APP_FILES)

include MakefileCommon
