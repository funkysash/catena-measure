Catena-measure
================
Simple application for measuring the time required by certain Catena parameters

Reference implementation:
https://github.com/medsec/catena

Academic paper:
<a href="http://www.uni-weimar.de/fileadmin/user/fak/medien/professuren/Mediensicherheit/Research/Publications/catena-v3.0.pdf">catena-v3.0.pdf</a>

Build options
-------------
Catena-measure depends on the Catena reference implementation and expects to
find it in `../catena`. This can be changed by appending
`CATENADIR=../somepath` to the invocation of make.


Dependencies
------------
* Catena-reference  (https://github.com/medsec/catena)
* clang             (http://clang.llvm.org/)
* make              (http://www.gnu.org/software/make/) 