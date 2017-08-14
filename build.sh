#!/bin/bash
CPS_ROOT=`pwd`

source $CPS_ROOT/script/common.sh
if [ $? -ne 0 ]; then
    echo "source $PICO_ROOTscript/common.sh failed." 1>&2
    exit 1
fi

function var() {
    if [ $# -ne 1 ]; then
        log_notice "USAGE: var <var_name>"
        exit 1
    fi
    eval echo \$$1
}

function match() {
    if [ $# -ne 2 ]; then
        log_notice "USAGE: match <var1_name> <var2_name>"
        exit 1
    fi

    if [[ "X`var $1`" == X*"`var $2`"* ]]; then
        echo 1
    fi
}

function addpath() {
    if [ $# -ne 2 ]; then
        log_notice "USAGE: addpath <ENV_VARIABLE> <PATH_TO_ADD>"
        exit 1
    fi
    local tmp=$2
    if [[ `match $1 tmp` ]]; then
        log_warn "DUPLICATE_ADD: \"$2\" already in \$$1 = \"`var $1`\"" 1>&2
        return 0
    else
        eval $1=$2:`var $1`
        log_notice "after adding \"$2\", \$$1 = \"`var $1`\"" 1>&2
        return 0
    fi
}

function check_return() {
    if [ $? -ne 0 ]; then
        log_fatal "$1 failed"
        exit 1
    fi  
}

function gen_build_config() {
    [ -f build.config ] && log_notice "`color 39 build.config` already exists" && return 0
    echo -e "CXX_COMPILER_PATH=\nCXX_LIB_PATH=" > $CPS_ROOT/build.config
    check_return "generate `color 39 build.config` failed"
    log_warn "`color 35 ${PICO_ROOT}/build.config` is newly generated, \
please `color 35 configure` it if new compiler is used."
    return 0
}

function setup() {
    local PREPARE_ROOT=`pwd`/prepare
    local setup_cmd=$1 #build
    # generate build.config
    execshell "gen_build_config"
    # source c compiler PATH
    # set c compilers for CMAKE
    execshell "export CC=`which gcc`"
    execshell "export CXX=`which g++`"
    execshell "export CMAKE=$PREPARE_ROOT/bin/cmake"
    # add installed third bin to path 
    execshell "addpath PATH $PREPARE_ROOT/bin"
    execshell "export PATH"
    # install tools
    execshell "pushd prepare"
    J=$J execshell "bash prepare.sh ${setup_cmd}"
    execshell "popd"
    # generate include
}

function build() {
    execshell "setup build"
    [ ! -d build ] && execshell "mkdir build"
    execshell "pushd build"
    execshell "$CMAKE .."
    if [ $# -gt 1 ]; then
        shift
        execshell "make $* -j$J"
    else
        execshell "make all -j$J"
    fi
        execshell "popd"
}


function usage() {
echo -e  "
`color 32 USAGE`: build.sh [<CMD>]
  bash build.sh `color 39 env`                     Setup the CPS building environment
  bash build.sh `color 39 env_clean`                   Clear the CPS building environment
  bash build.sh `color 39 \-\-help\|\-h`               Show this message
  "
}

function env_clean() {
    execshell "pushd prepare"
    execshell "bash prepare.sh clean"
    execshell "popd"
}


[[ -z "$J" ]] && J=`nproc | awk '{print int(($0 + 1)/ 2)}'`          # make cocurrent thread number
export J

export cps_version_code="develop_version"

#source $PICO_ROOT/build.config

case $1 in
    help|-h|-help|--help|--h)
        usage
    ;;

    ###### build related #####
   
    ###### test related #####
    unit)
        execshell "build"
    ;;     
    ###### doc related #####

    ###### env related #####
    env)
        execshell "setup build"
        log_notice "[`color 39 env`] [`color 32 success`]"
    ;;

    env_clean)
        execshell "env_clean"
        log_notice "[`color 39 env_clean`] [`color 32 success`]"
    ;;

    ###### misc #####
esac
