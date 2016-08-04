echo "#include \"flare/flare.h\"

void flare::info::print() {

print::d(\"%-12s %s %s @ %s\", \"Compiled on:\", __DATE__, __TIME__, \"$(hostname)\");
	print::d(\"%-12s %s\", \"Version:\", \"$(git describe --always --dirty=-modified)\");
	print::d(\"%-12s %i\", \"C++ version:\", __cplusplus);
	print::d(\"%-12s %s (%s)\", \"Zlib:\", ZLIB_VERSION, zlibVersion());
}" > $1
