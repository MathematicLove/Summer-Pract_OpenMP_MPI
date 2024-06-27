#ifndef RLE_H
#define RLE_H

#include <vector>
#include <iostream>

class RLE
{
private:
	std::vector<unsigned char> m_encoded;
	std::vector<unsigned char> m_decoded;
	void Encode(std::vector<unsigned char>& initial);
	void Decode();
	double m_compressionCoefficient;
public:

	RLE(std::vector<unsigned char> initialText);
	std::vector<unsigned char> get_decode();
	std::vector<unsigned char> get_encode();
	void print_info();


};
#endif // RLE_H
