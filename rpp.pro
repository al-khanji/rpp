TEMPLATE = subdirs
CONFIG += ordered

sub_include.subdir = include
sub_include.target = sub-include

SUBDIRS += sub_include

sub_tests.subdir = tests
sub_tests.target = sub-tests
sub_tests.CONFIG = no_default_install

SUBDIRS += sub_tests

OTHER_FILES += \
    .qmake.conf
