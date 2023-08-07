LOCALBASE?= /usr/local
MK_DEBUG_FILES=no
LIB=bmdplugin-avahi
SHLIB_MAJOR=1
CFLAGS+=-I${LOCALBASE}/include -DLOCALBASE=${LOCALBASE}
LIBDIR=$(LOCALBASE)/libexec/bmd
SRCS =  avahi.c

.include <bsd.lib.mk>
