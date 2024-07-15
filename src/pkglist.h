// This is the list of packages, their names,
// and their links.
//
// General format:
// 
// char pkg_PackageName[4][100] = { "PackageName",
// "<the name of the corrosponding ENTITY in the packages.ent file>",
// "<the link of where to search for the new version of the package>",
// "<The link to the release text/info of the package (making it NULL is okay)>"
// };

char pkg_libtasn1[4][100] = { "libtasn1", "libtasn1-version",
	"https://ftp.gnu.org/gnu/libtasn1/",
	NULL
};
char pkg_NSPR[4][100] = { "NSPR", "nspr-version",
	"https://archive.mozilla.org/pub/nspr/releases/",
	NULL
};
char pkg_NSS[4][100] = { "NSS", "nss-dir",
	"https://archive.mozilla.org/pub/security/nss/releases/",
	NULL
};
char pkg_p11-kit[4][100] = { "p11-kit", "p11-kit-version",
	"https://api.github.com/repos/p11-glue/p11-kit/releases/latest",
	NULL
};
char pkg_make-ca[4][100] = { "make-ca", "make-ca-version",
	"https://api.github.com/repos/lfs-book/make-ca/releases/latest",
	NULL
};
char pkg_libunistring[4][100] = { "libunistring", "libunistring-version",
	"https://ftp.gnu.org/gnu/libunistring/",
	NULL
};
char pkg_libidn2[4][100] = { "libidn2", "libidn2-version",
	"https://ftp.gnu.org/gnu/libidn/",
	NULL
};
char pkg_libpsl[4][100] = { "libpsl", "libpsl-version",
	"https://api.github.com/repos/rockdaboot/libpsl/releases/latest",
	NULL
};
char pkg_cURL[4][100] = { "cURL", "curl-version",
	"https://curl.se/download/",
	NULL
};
char pkg_Wget[4][100] = { "Wget", "wget-version",
	"https://ftp.gnu.org/gnu/wget/",
	NULL
};
