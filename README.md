# NOTICE
This project is now archived as it is bitrotting, outdated, and is too
complicated (perhaps insecure as well). A new potential solution may be
conjured up by the GLFS development team.

# chkpkgver
Compares and fetches versions of GLFS packages against itself and online
results.

# Building, Installation, and Use
Dependencies:
- pkg-config / pkgconf (for detecting libraries and header file locations)
- cURL                 (for downloading pages)
- PCRE2                (For extracting versions)
- JSON-C               (finding latest tags from Github)

To build the project, simply do:
```Bash
cmake . && make
```
Then if you wish, copy it wherever or run it in the current directory.
You can, however, use the classic method of building and installing by doing:
```Bash
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX . && make
```
And as the ***root*** user:
```Bash
make install
```
To run the project, do:
```Bash
/path/to/chkpkgver /path/to/packages.ent
```
