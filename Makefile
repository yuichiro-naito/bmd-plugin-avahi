LOCALBASE?=	/usr/local
MK_DEBUG_FILES=	no
LIB=		bmdplugin_avahi
SHLIB_MAJOR=	1
CFLAGS+=	-I${LOCALBASE}/include -DLOCALBASE=\"${LOCALBASE}\"
LIBDIR=		$(LOCALBASE)/libexec/bmd
SRCS= 		avahi.c
MAN=		bmd-plugin-avahi.8
MANDIR=		$(LOCALBASE)/man/man

.include <bsd.lib.mk>
