#include <cassert> // Must be included before `idgen.hpp`
#include "include/idgen.hpp"

#include <fmt/core.h>

#include <string>
#include <cstdlib>
#include <cstdint>
#include <unordered_set>

#include <unistd.h>
#include <posixfio_tl.hpp>



auto stdoutBuf = posixfio::OutputBuffer(posixfio::FileView(STDOUT_FILENO), 4096);

using id8s_e = int8_t;  enum class Id8s : id8s_e { };
using id8u_e = uint8_t; enum class Id8u : id8u_e { };



template <typename Id>
bool testSinglethreaded(size_t gen_count) {
	using id_e = std::underlying_type_t<Id>;

	idgen::IdGenerator<Id> generator;
	std::unordered_set<Id> generated;

	std::string fmtBuffer;
	fmtBuffer.reserve(128);

	auto expectedGen = idgen::baseId<Id>();
	Id   gen;
	bool fail = false;

	for(size_t i = 0; i < gen_count; ++i) {
		gen = generator.generate();
		if(gen != expectedGen) {
			fmt::format_to(std::back_inserter(fmtBuffer), "Generated {} instead of {}\n", id_e(gen), id_e(expectedGen));
			fail = true;
		}
		if(! generated.insert(gen).second) {
			fmt::format_to(std::back_inserter(fmtBuffer), "Generated {} (duplicate)\n", id_e(gen));
			fail = true;
		}
		if(! fmtBuffer.empty()) stdoutBuf.writeAll(fmtBuffer.data(), fmtBuffer.size());
		fmtBuffer.clear();
		expectedGen = Id(id_e(expectedGen) + 1);
	}

	return ! fail;
}



int main(int argn, char** args) {
	try {
		testSinglethreaded<Id8s>(128);
		testSinglethreaded<Id8u>(255);
	} catch(...) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
