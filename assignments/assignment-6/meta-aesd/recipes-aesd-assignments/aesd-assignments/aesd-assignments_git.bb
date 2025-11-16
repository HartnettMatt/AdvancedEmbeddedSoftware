# See https://git.yoctoproject.org/poky/tree/meta/files/common-licenses
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# Assignment repo URI
SRC_URI = "git://git@github.com/cu-ecen-aeld/assignments-3-and-later-HartnettMatt;protocol=ssh;branch=main"

PV = "1.0+git${SRCPV}"
# Most up to date git hash
SRCREV = "87d2ee10996fee2f0a215f8419c8ee05cc025f21"

# This sets your staging directory based on WORKDIR, where WORKDIR is defined at 
# https://docs.yoctoproject.org/ref-manual/variables.html?highlight=workdir#term-WORKDIR
# We reference the "server" directory here to build from the "server" directory
# in your assignments repo
S = "${WORKDIR}/git/server"

# See https://git.yoctoproject.org/poky/plain/meta/conf/bitbake.conf?h=kirkstone
FILES_${PN} += "${bindir}/aesdsocket"
FILES_${PN} += "${sysconfdir}/init.d/aesdsocket-start-stop.sh"
TARGET_LDFLAGS += "-pthread -lrt"
#RDEPENDS:${PN} += "libgcc"

inherit update-rc.d
INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME:${PN} = "aesdsocket-start-stop.sh"

do_configure () {
	:
}

do_compile () {
	oe_runmake CFLAGS="${TARGET_CFLAGS}" LDFLAGS="${TARGET_LDFLAGS}"
}

do_install () {
    install -d ${D}${bindir}
	install -m 0755 ${S}/aesdsocket ${D}${bindir}/
    install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${S}/aesdsocket-start-stop.sh ${D}${sysconfdir}/init.d/aesdsocket-start-stop.sh
}