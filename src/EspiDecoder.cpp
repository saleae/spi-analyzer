#include "EspiDecoder.h"

static std::string HexByte( uint8_t b )
{
	char buf[8];
	snprintf( buf, sizeof( buf ), "0x%02X", b );
	return std::string( buf );
}

EspiDecodedTransaction EspiDecoder::Decode( const std::vector<uint8_t>& mosi, const std::vector<uint8_t>& miso )
{
	EspiDecodedTransaction out;
	out.mosi = mosi;
	out.miso = miso;

	if( mosi.empty() )
	{
		out.ok = false;
		out.summary = "eSPI: empty MOSI";
		return out;
	}

	// Placeholder: we will implement proper eSPI decoding on top of the official SPI analyzer.
	// For now, just show opcode and lengths so the analyzer can be validated end-to-end.
	out.ok = true;
	out.summary = std::string( "eSPI(opcode=" ) + HexByte( mosi[0] ) + ", mosi_len=" + std::to_string( mosi.size() ) + ", miso_len=" + std::to_string( miso.size() ) + ")";
	return out;
}
