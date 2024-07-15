// This is the list of packages, their names,
// and their links.
//
// General format:
// 
// const char *pkg_PackageName[4] = { "PackageName",
// "<the name of the corrosponding ENTITY in the packages.ent file>",
// "<the link of where to search for the new version of the package>",
// "<The link to the release text/info of the package (making it "\0" is okay)>"
// };

const char *pkg_libtasn1[4] = { "libtasn1", "libtasn1-version",
	"https://ftp.gnu.org/gnu/libtasn1/",
	"\0"
};
const char *pkg_NSPR[4] = { "NSPR", "nspr-version",
	"https://archive.mozilla.org/pub/nspr/releases/",
	"\0"
};
const char *pkg_NSS[4] = { "NSS", "nss-dir",
	"https://archive.mozilla.org/pub/security/nss/releases/",
	"\0"
};
const char *pkg_p11_kit[4] = { "p11-kit", "p11-kit-version",
	"https://api.github.com/repos/p11-glue/p11-kit/releases/latest",
	"\0"
};
const char *pkg_make_ca[4] = { "make-ca", "make-ca-version",
	"https://api.github.com/repos/lfs-book/make-ca/releases/latest",
	"\0"
};
const char *pkg_libunistring[4] = { "libunistring", "libunistring-version",
	"https://ftp.gnu.org/gnu/libunistring/",
	"\0"
};
const char *pkg_libidn2[4] = { "libidn2", "libidn2-version",
	"https://ftp.gnu.org/gnu/libidn/",
	"\0"
};
const char *pkg_libpsl[4] = { "libpsl", "libpsl-version",
	"https://api.github.com/repos/rockdaboot/libpsl/releases/latest",
	"\0"
};
const char *pkg_cURL[4] = { "cURL", "curl-version",
	"https://curl.se/download/",
	"\0"
};
const char *pkg_Wget[4] = { "Wget", "wget-version",
	"https://ftp.gnu.org/gnu/wget/",
	"\0"
};
const char *pkg_git[4] = { "git", "git-version",
	"https://www.kernel.org/pub/software/scm/git/",
	"\0"
};
const char *pkg_alsa_lib[4] = { "alsa-lib", "alsa-lib-version",
	"https://www.alsa-project.org/files/pub/lib/",
	"\0"
};
const char *pkg_alsa_plugins[4] = { "alsa-plugins", "alsa-plugins-version",
	"https://www.alsa-project.org/files/pub/plugins/",
	"\0"
};
const char *pkg_alsa_utils[4] = { "alsa-utils", "alsa-utils-version",
	"https://www.alsa-project.org/files/pub/utils/",
	"\0"
};
