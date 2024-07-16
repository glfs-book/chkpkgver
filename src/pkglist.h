// This is the list of packages, their names,
// and their links.
//
// General format:
// 
// static const char *pkg_PackageName[4] = { "PackageName",
// "<the name of the corrosponding ENTITY in the packages.ent file>",
// "<the link of where to search for the new version of the package>",
// "<The link to the release text/info of the package (making it "\0" is okay)>"
// };

static const char *pkg_libtasn1[4] = { "libtasn1", "libtasn1-version",
	"https://ftp.gnu.org/gnu/libtasn1/",
	"\0"
};
static const char *pkg_NSPR[4] = { "NSPR", "nspr-version",
	"https://archive.mozilla.org/pub/nspr/releases/",
	"\0"
};
static const char *pkg_NSS[4] = { "NSS", "nss-dir",
	"https://archive.mozilla.org/pub/security/nss/releases/",
	"\0"
};
static const char *pkg_p11_kit[4] = { "p11-kit", "p11-kit-version",
	"https://api.github.com/repos/p11-glue/p11-kit/releases/latest",
	"\0"
};
static const char *pkg_make_ca[4] = { "make-ca", "make-ca-version",
	"https://api.github.com/repos/lfs-book/make-ca/releases/latest",
	"\0"
};
static const char *pkg_libunistring[4] = { "libunistring", "libunistring-version",
	"https://ftp.gnu.org/gnu/libunistring/",
	"\0"
};
static const char *pkg_libidn2[4] = { "libidn2", "libidn2-version",
	"https://ftp.gnu.org/gnu/libidn/",
	"\0"
};
static const char *pkg_libpsl[4] = { "libpsl", "libpsl-version",
	"https://api.github.com/repos/rockdaboot/libpsl/releases/latest",
	"\0"
};
static const char *pkg_cURL[4] = { "cURL", "curl-version",
	"https://curl.se/download/",
	"\0"
};
static const char *pkg_Wget[4] = { "Wget", "wget-version",
	"https://ftp.gnu.org/gnu/wget/",
	"\0"
};
static const char *pkg_git[4] = { "git", "git-version",
	"https://www.kernel.org/pub/software/scm/git/",
	"\0"
};
static const char *pkg_alsa_lib[4] = { "alsa-lib", "alsa-lib-version",
	"https://www.alsa-project.org/files/pub/lib/",
	"\0"
};
static const char *pkg_alsa_plugins[4] = { "alsa-plugins", "alsa-plugins-version",
	"https://www.alsa-project.org/files/pub/plugins/",
	"\0"
};
static const char *pkg_alsa_utils[4] = { "alsa-utils", "alsa-utils-version",
	"https://www.alsa-project.org/files/pub/utils/",
	"\0"
};
static const char *pkg_libogg[4] = { "libogg", "libogg-version",
	"https://downloads.xiph.org/releases/ogg/",
	"\0"
};
static const char *pkg_libvorbis[4] = { "libvorbis", "libvorbis-version",
	"https://downloads.xiph.org/releases/vorbis/",
	"\0"
};
static const char *pkg_flac[4] = { "FLAC", "flac-version",
	"https://downloads.xiph.org/releases/flac/",
	"\0"
};
static const char *pkg_opus[4] = { "Opus", "opus-version",
	"https://downloads.xiph.org/releases/opus/",
	"\0"
};
static const char *pkg_libsndfile[4] = { "libsndfile", "libsndfile-version",
	"https://api.github.com/repos/libsndfile/libsndfile/releases/latest",
	"\0"
};
static const char *pkg_PulseAudio[4] = { "PulseAudio", "pulseaudio-version",
	"https://www.freedesktop.org/software/pulseaudio/releases/",
	"\0"
};
static const char *pkg_util_macros[4] = { "util-macros", "util-macros-version",
	"https://gitlab.freedesktop.org/xorg/util/macros/-/tags",
	"\0"
};
static const char *pkg_xorgproto[4] = { "xorgproto", "xorgproto-version",
	"https://gitlab.freedesktop.org/xorg/proto/xorgproto/-/tags",
	"\0"
};
static const char *pkg_libXau[4] = { "libXau", "libXau-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXau/-/tags",
	"\0"
};
static const char *pkg_libXdmcp[4] = { "libXdmcp", "libXdmcp-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXdmcp/-/tags",
	"\0"
};
static const char *pkg_Python[4] = { "Python", "python3-version",
	"https://www.python.org/ftp/python/",
	"\0"
};
static const char *pkg_xcb_proto[4] = { "xcb-proto", "xcb-proto-version",
	"https://gitlab.freedesktop.org/xorg/proto/xcbproto/-/tags",
	"\0"
};

static const char *pkg_libxcb[4] = { "libxcb", "libxcb-version",
	"https://gitlab.freedesktop.org/xorg/lib/libxcb/-/tags",
	"\0"
};

static const char *pkg_Which[4] = { "Which", "which-version",
	"https://ftp.gnu.org/gnu/which/",
	"\0"
};
static const char *pkg_libpng[4] = { "libpng", "libpng-version",
	"https://sourceforge.net/projects/libpng/files/",
	"\0"
};
static const char *pkg_FreeType[4] = { "FreeType2", "freetype2-version",
	"https://sourceforge.net/projects/freetype/files/",
	"\0"
};
static const char *pkg_HarfBuzz[4] = { "HarfBuzz", "harfbuzz-version",
	"https://api.github.com/repos/harfbuzz/harfbuzz/releases/latest",
	"\0"
};
static const char *pkg_Fontconfig[4] = { "Fontconfig", "fontconfig-version",
	"https://www.freedesktop.org/software/fontconfig/release/",
	"\0"
};
static const char *pkg_xtrans[4] = { "xtrans", "xtrans-version",
	"https://gitlab.freedesktop.org/xorg/lib/libxtrans/-/tags",
	"\0"
};
static const char *pkg_libX11[4] = { "libX11", "libX11-version",
	"https://gitlab.freedesktop.org/xorg/lib/libX11/-/tags",
	"\0"
};
static const char *pkg_libXext[4] = { "libXext", "libXext-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXext/-/tags",
	"\0"
};
static const char *pkg_libFS[4] = { "libFS", "libFS-version",
	"https://gitlab.freedesktop.org/xorg/lib/libFS/-/tags",
	"\0"
};
static const char *pkg_libICE[4] = { "libICE", "libICE-version",
	"https://gitlab.freedesktop.org/xorg/lib/libICE/-/tags",
	"\0"
};
static const char *pkg_libSM[4] = { "libSM", "libSM-version",
	"https://gitlab.freedesktop.org/xorg/lib/libSM/-/tags",
	"\0"
};
static const char *pkg_libXScrnSaver[4] = { "libXScrnSaver", "libXScrnSaver-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXScrnSaver/-/tags",
	"\0"
};
static const char *pkg_libXt[4] = { "libXt", "libXt-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXt/-/tags",
	"\0"
};
static const char *pkg_libXmu[4] = { "libXmu", "libXmu-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXmu/-/tags",
	"\0"
};
static const char *pkg_libXpm[4] = { "libXpm", "libXpm-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXpm/-/tags",
	"\0"
};
static const char *pkg_libXaw[4] = { "libXaw", "libXaw-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXaw/-/tags",
	"\0"
};
static const char *pkg_libXfixes[4] = { "libXfixes", "libXfixes-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXfixes/-/tags",
	"\0"
};
static const char *pkg_libXcomposite[4] = { "libXcomposite", "libXcomposite-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXcomposite/-/tags",
	"\0"
};
static const char *pkg_libXrender[4] = { "libXrender", "libXrender-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXrender/-/tags",
	"\0"
};
static const char *pkg_libXcursor[4] = { "libXcursor", "libXcursor-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXcursor/-/tags",
	"\0"
};
static const char *pkg_libXdamage[4] = { "libXdamage", "libXdamage-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXdamage/-/tags",
	"\0"
};
static const char *pkg_libfontenc[4] = { "libfontenc", "libfontenc-version",
	"https://gitlab.freedesktop.org/xorg/lib/libfontenc/-/tags",
	"\0"
};
static const char *pkg_libXfont2[4] = { "libXfont2", "libXfont2-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXfont/-/tags",
	"\0"
};
static const char *pkg_libXft[4] = { "libXft", "libXft-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXft/-/tags",
	"\0"
};
static const char *pkg_libXi[4] = { "libXi", "libXi-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXi/-/tags",
	"\0"
};
static const char *pkg_libXinerama[4] = { "libXinerama", "libXinerama-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXinerama/-/tags",
	"\0"
};
static const char *pkg_libXrandr[4] = { "libXrandr", "libXrandr-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXrandr/-/tags",
	"\0"
};
static const char *pkg_libXres[4] = { "libXres", "libXres-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXres/-/tags",
	"\0"
};
static const char *pkg_libXtst[4] = { "libXtst", "libXtst-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXtst/-/tags",
	"\0"
};
static const char *pkg_libXv[4] = { "libXv", "libXv-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXv/-/tags",
	"\0"
};
static const char *pkg_libXvMC[4] = { "libXvMC", "libXvMC-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXvMC/-/tags",
	"\0"
};
static const char *pkg_libXxf86dga[4] = { "libXxf86dga", "libXxf86dga-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXxf86dga/-/tags",
	"\0"
};
static const char *pkg_libXxf86vm[4] = { "libXxf86vm", "libXxf86vm-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXxf86vm/-/tags",
	"\0"
};
static const char *pkg_libpciaccess[4] = { "libpciaccess", "libpciaccess-version",
	"https://gitlab.freedesktop.org/xorg/lib/libpciaccess/-/tags",
	"\0"
};
static const char *pkg_libxkbfile[4] = { "libxkbfile", "libxkbfile-version",
	"https://gitlab.freedesktop.org/xorg/lib/libxkbfile/-/tags",
	"\0"
};
static const char *pkg_libxshmfence[4] = { "libxshmfence", "libxshmfence-version",
	"https://gitlab.freedesktop.org/xorg/lib/libxshmfence/-/tags",
	"\0"
};
static const char *pkg_libXpresent[4] = { "libXpresent", "libXpresent-version",
	"https://gitlab.freedesktop.org/xorg/lib/libXpresent/-/tags",
	"\0"
};
static const char *pkg_libxcvt[4] = { "libxcvt", "libxcvt-version",
	"https://gitlab.freedesktop.org/xorg/lib/libxcvt/-/tags",
	"\0"
};
static const char *pkg_CMake[4] = { "CMake", "cmake-version",
	"https://cmake.org/download/",
	"\0"
};
static const char *pkg_dbus[4] = { "dbus", "dbus-version",
	"https://dbus.freedesktop.org/releases/dbus/",
	"\0"
};
static const char *pkg_libunwind[4] = { "libunwind", "libunwind-version",
	"https://download.savannah.nongnu.org/releases/libunwind/",
	"\0"
};
static const char *pkg_Nettle[4] = { "Nettle", "nettle-version",
	"https://ftp.gnu.org/gnu/nettle/",
	"\0"
};
static const char *pkg_GnuTLS[4] = { "GnuTLS", "gnutls-version",
	"https://gitlab.com/gnutls/gnutls/-/tags",
	"\0"
};
static const char *pkg_Pixman[4] = { "Pixman", "pixman-version",
	"https://gitlab.freedesktop.org/pixman/pixman/-/tags",
	"\0"
};
static const char *pkg_icu[4] = { "icu", "icu-version",
	"https://github.com/unicode-org/icu/releases/download/release-75-1/",
	"\0"
};
static const char *pkg_libxml2[4] = { "libxml2", "libxml2-version",
	"https://gitlab.gnome.org/GNOME/libxml2/-/tags",
	"\0"
};
