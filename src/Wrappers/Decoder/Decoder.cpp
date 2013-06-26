#include "Decoder.h"

Decoder::Decoder() {
	initialized=false;	
	utilityPair = NULL;
}

Decoder::Decoder(byte* buffer_)
{
	initialized=true;
	buffer=buffer_;
	utilityPair = NULL;
}
Decoder::Decoder(Decoder& decoder_) {
	initialized=true;
	buffer=decoder_.buffer;
	utilityPair = NULL;
}	
Decoder::~Decoder()
{
  delete utilityPair;
}

void Decoder::setBuffer(byte* buffer_) {
	initialized=true;
	buffer=buffer_;
}

byte* Decoder::getBuffer() {
	if (!initialized) throw new CodingException("Decoder: Error, decoder not initialized with buffer");
	return buffer;	
}
