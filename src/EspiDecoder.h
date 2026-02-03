#ifndef ESPI_DECODER_H
#define ESPI_DECODER_H

#include <string>
#include <vector>
#include <cstdint>

struct EspiDecodedTransaction
{
	bool ok = false;
	std::string summary;

	// Raw bytes (as captured from SPI analyzer)
	std::vector<uint8_t> mosi;
	std::vector<uint8_t> miso;
};

class EspiDecoder
{
public:
	// Minimal skeleton: parse a single SPI CS# transaction into a best-effort eSPI summary.
	// Returns ok=false with a short reason when it can't decode.
	EspiDecodedTransaction Decode( const std::vector<uint8_t>& mosi, const std::vector<uint8_t>& miso );
};

#endif