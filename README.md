# chkpkgver
Compares and fetches versions of GLFS packages against itself and online
results. It's not done yet and won't be for a while. The current solution
for checking versions of packages is by comparing GLFS with BLFS and
downloading current package versions from Arch PKGBUILD files. This is
not ideal. The current solution also relies on a Bash script but can
grow unmaintainable. It doesn't scale well.

# Building, Installation, and Use
Dependencies:
- pkg-config / pkgconf
- cURL
- PCRE2

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
