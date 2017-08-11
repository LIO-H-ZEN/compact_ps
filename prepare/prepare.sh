#!/bin/bash
source `pwd`/../script/common.sh

CMAKE=`pwd`/bin/cmake

function check_supported_os() {
    if [[ "$OSTYPE" == "linux"* ]]; then
        MYOS="linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        log_fatal "$OSTYPE system is not supported" 
        exit 1
    else
        log_fatal "$OSTYPE system is not supported"
        exit 1
    fi

    if [ X"`uname -m`" != X"x86_64" ]; then
        log_fatal "only `color 39 x86_64` systems are supported"
        exit 1
    fi
}

function check_return() {
    if [ $? -ne 0 ]; then
        log_fatal "$1 failed"
        exit 1
    fi
}

function decompress_cmd() {
    if [ `echo $1 | grep ".gz$"` ]; then
        echo "tar -zxf"
    elif [ `echo $1 | grep ".tgz$"` ]; then
        echo "tar -zxf"
    elif [ `echo $1 | grep ".zip$"` ]; then
        echo "unzip"
    else
        echo ""
    fi
}

function decompress() {
    execshell "rm -rf $1*"
    execshell "cp ../pkgs/$1* ."
    local pkg=`ls | grep $1`
    check_return "pkg=`ls | grep $1` failed"
    local cmd=`decompress_cmd $pkg`
    if [ "X$cmd" == "X" ]; then
        log_fatal "unkown compreess format $pkg"
        exit 1
    fi
    execshell "$cmd $pkg"
}

function install_pkg_common_preprocess() {
    if [ -f $1_INSTALLED_FLAG ]; then
        log_notice "`color 39 $1` already installed"
        return 1
    fi
    execshell "decompress $1"
    local pkgdir=`find . -maxdepth 1 -type d -name "$1*"`
    check_return "find . -maxdepth 1 -type d -name '$1*' failed"
    if [ "X$pkgdir" = "X" ]; then
        log_fatal "cannnot find $1 related decompressed dir"
        exit 1
    fi
    execshell "pushd $pkgdir"
}

function install_pkg_common_postprocess() {
    execshell "popd"
    execshell "rm -rf $1*"
    execshell "touch $1_INSTALLED_FLAG"
}

function install_cmake_pkg() {
    install_pkg_common_preprocess "$1"
    if [ $? -eq 0 ]; then
        execshell "mkdir _build" && execshell "pushd _build"
        execshell "$CMAKE .. -DCMAKE_INSTALL_PREFIX=$prefix $cmake_exdefine"
        execshell "make -j$J"
        [ "X$install_cmd" = "X" ] && execshell "make install" || execshell "$install_cmd"
        execshell "popd"
        install_pkg_common_postprocess "$1"
    fi  
    return 0
}

function install_configure_pkg() {
    install_pkg_common_preprocess "$1"
    if [ $? -eq 0 ]; then
        execshell "$configure_preprocess"
        execshell "$configure_predef ./configure --prefix=$prefix $configure_flags"
        execshell "make -j$J"
        [ "X$install_cmd" = "X" ] && execshell "make install" || execshell "$install_cmd"
        install_pkg_common_postprocess "$1"
    fi
    return 0
}

function install_make_pkg() {
    install_pkg_common_preprocess "$1"
    if [ $? -eq 0 ]; then
        execshell "$make_preprocess"
        execshell "make -j$J"
        [ "X$install_cmd" = "X" ] && execshell "make install" || execshell "$install_cmd"
        install_pkg_common_postprocess "$1"
    fi
    return 0
}

function install_decomp() {
    if [ -f $1_INSTALLED_FLAG ]; then
        log_notice "`color 39 $1` already installed"
        return 0
    fi
    execshell "decompress $1"
    local pkgdir=`find . -maxdepth 1 -type d -name "$1*"`
    check_return "find . -maxdepth 1 -type d -name '$1*' failed"
    if [ "X$pkgdir" = "X" ]; then
        log_fatal "cannnot find $1 related decompressed dir"
        exit 1
    fi
    if [ "${install_cmd}" != "" ]; then
        execshell "${install_cmd}"
    else
        execshell "cp -r ${pkgdir}/* ${prefix}"
    fi
    execshell "rm -rf $1*"
    execshell "touch $1_INSTALLED_FLAG"
    return 0
}

function install_gperftools() {
    if [ -f gperftools_INSTALLED_FLAG ]; then
        log_notice "`color 39 gperftools` already installed"
        return 0
    fi
    install_pkg_common_preprocess "gperftools"
    execshell "bash autogen.sh"
    CPPFLAGS=-I$prefix/include LDFLAGS=-L$prefix/lib execshell "./configure --prefix=$prefix"
    execshell "make -j$J"
    execshell "make install"
    execshell "cd .."
    execshell "rm -rf gperftools*"
    execshell "touch gperftools_INSTALLED_FLAG"
    return 0
}

function install_bzip2() {
    if [ -f bzip2_INSTALLED_FLAG ]; then
        log_notice "`color 39 bzip2` already installed"
        return 0
    fi
    execshell "decompress bzip2"
    execshell "cd bzip2*"
    awk '{if ($0~/^CFLAGS=/){print $0" -fPIC"}else{print $0}}' Makefile > Makefile_new
    check_return "bzip2 modify Makefile failed"
    rm Makefile && mv Makefile_new Makefile
    check_return "bzip2 rm Makefile && mv Makefile_new Makefile failed"
    execshell "make"
    execshell "make install PREFIX=$prefix"
    execshell "cd .."
    execshell "rm -rf bzip2*"
    execshell "touch bzip2_INSTALLED_FLAG"
    return 0
}

function install_boost() {
    if [ -f boost_INSTALLED_FLAG ]; then
        log_notice "`color 39 boost` already installed"
        return 0
    fi
    execshell "decompress boost"
    execshell "cd boost*"
    execshell "./bootstrap.sh --prefix=$prefix --without-libraries=python"
    execshell "./bjam --prefix=$prefix cxxflags=-fPIC cflags=-fPIC --without-python --link=static --runtime-link=static -j$J"
    execshell "./b2 install -j$J"
    execshell "cd .."
    execshell "rm -rf boost*"
    execshell "touch boost_INSTALLED_FLAG"
    return 0
}

function usage() {
cat  <<HELP_INFO
prepare.sh作用：
  bash prepare.sh build|""          准备CPS编译环境
  bash prepare.sh clean             清除本地编译环境
  bash prepare.sh --help|-h         显示此帮助信息
HELP_INFO
}

function build_cps_env() {
    [ ! -d build ] && execshell "mkdir build"

    execshell "cd build"
    execshell "install_configure_pkg cmake"
    # install packages
    execshell "install_configure_pkg m4"
    execshell "install_configure_pkg autoconf"
    execshell "install_configure_pkg automake"
    execshell "install_configure_pkg zlib"
    execshell "install_configure_pkg bison"
    configure_flags="--with-internal-glib CFLAGS=-Wno-error=format-nonliteral" execshell "install_configure_pkg pkg-config"
    execshell "install_configure_pkg libtool"
    [[ "$MYOS" == "linux" ]] && execshell "install_configure_pkg xz"
    execshell "install_bzip2"
    execshell "install_boost"
    cmake_exdefine="-DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC" execshell "install_cmake_pkg gflags"
    configure_predef="CXXFLAGS=-fPIC CFLAGS=-fPIC" execshell "install_configure_pkg glog"
    configure_preprocess="bash autogen.sh" execshell "install_configure_pkg libsodium"
    configure_flags="--with-libsodium=no --enable-static=yes --enable-shared=no" execshell "install_configure_pkg zeromq"
    
    [[ "$MYOS" == "linux" ]] && execshell "install_configure_pkg libunwind"
    [[ "$MYOS" == "linux" ]] && execshell "install_gperftools"
    configure_flags="--enable-mpi-fortran=no --enable-static" execshell "install_configure_pkg openmpi"

    execshell "install_cmake_pkg googletest"

    execshell "install_cmake_pkg eigen"
    execshell "install_configure_pkg sparsehash"
    install_cmd="cp -r \${pkgdir} ${prefix} && ln -s ./\${pkgdir} ${prefix}/jdk" execshell "install_decomp jdk"
    # end of install
    execshell "cd .."
}

function build_doc_env() {
    build_pico_env

    [ ! -d build ] && execshell "mkdir build"
    execshell "cd build"
    configure_flags="CFLAGS=-fPIC" execshell "install_configure_pkg Python"
    execshell "install_cmake_pkg doxygen"
    execshell "install_configure_pkg graphviz"
    # end of install
    execshell "cd .."
}

function build_test_env() {
    build_pico_env

    [ ! -d build ] && execshell "mkdir build"
    execshell "cd build"
    install_cmd="cp -r \${pkgdir} ${prefix} && ln -s ./\${pkgdir} ${prefix}/apache-ant" execshell "install_decomp apache-ant"
    install_cmd="cp -r \${pkgdir} ${prefix} " execshell "install_decomp ant-depend"
    # end of install
    execshell "cd .."
}

prefix=`pwd` # install dir
[[ -z "$J" ]] && J=4          # make cocurrent thread number

check_supported_os

case $1 in
    help|-h|-help|--help|--h)
        usage
    ;;
    
    clean) 
        execshell "rm -rf build bin include lib lib64 etc share doc man \
            jdk* apache-ant* ant-depend"
        log_notice "[\e[32;1mclean SUCCESS\e[m]"
    ;;

    test)
        build_test_env
        log_notice "[\e[32;1mtest SUCCESS\e[m]"
    ;;

    build|*)
        build_cps_env
        log_notice "[\e[32;1mbuild SUCCESS\e[m]"
    ;;
esac

exit 0
