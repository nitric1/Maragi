#pragma once

namespace Maragi
{
	std::string base64Encode(const std::vector<uint8_t> &);
	std::vector<uint8_t> base64Decode(const std::string &);
}
