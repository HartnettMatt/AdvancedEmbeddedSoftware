
##############################################################
#
# LDD
#
##############################################################

LDD_VERSION = 'd79431086165fd61e5ccef1173d9e7d1b5e714de'
LDD_SITE = 'git@github.com:cu-ecen-aeld/assignment-7-HartnettMatt.git'
LDD_SITE_METHOD = git
LDD_GIT_SUBMODULES = YES

LDD_MODULE_SUBDIRS = scull
LDD_MODULE_SUBDIRS += misc-modules

$(eval $(kernel-module))
$(eval $(generic-package))
