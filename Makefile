MK_DEBUG_FILES=	no
SHLIB=		bmdplugin_avahi
SHLIB_MAJOR=	1
CFLAGS+=	-I${LOCALBASE}/include -DLOCALBASE=\"${LOCALBASE}\"
LIBDIR=		$(LOCALBASE)/libexec/bmd
SRCS= 		avahi.c
MAN=		bmd-plugin-avahi.8
MANDIR=		$(LOCALBASE)/share/man/man

WARNS?=		6

.include "Makefile.inc"
.include <bsd.lib.mk>
