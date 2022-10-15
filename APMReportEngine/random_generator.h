//
// From
// https://github.com/envoyproxy/envoy/blob/master/source/common/common/random_generator.{h,cc}

#pragma once

#include <cassert>
#include <random>
#include <string>

namespace APMReport {

	namespace
	{
		static constexpr size_t UUID_LENGTH = 36;
		static const std::string CHARS = "0123456789AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
	}  // namespace

	class RandomGeneratorImpl
	{
	public:
		std::string uuid();

	private:
		void randomBuffer(char* ch, size_t len);
	};

}
