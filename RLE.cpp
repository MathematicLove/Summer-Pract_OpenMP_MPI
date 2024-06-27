#include "RLE.h"
#include "stddfns.h"
void RLE::Encode(std::vector<unsigned char>& initial)
{
	int i = 0, end = (int)initial.size();//pointer i in initial
	unsigned char count_of_letter;
	std::vector<bool>checked(end);
	count_of_letter = 1;
	while (i < end-1) {
		if (initial[i] != initial[i + 1] || count_of_letter==255) {
			m_encoded.push_back(count_of_letter);
			m_encoded.push_back(initial[i]);
			count_of_letter = 1;
		}
		else {
			count_of_letter++;
		}
		checked[i] = true;

		/*if (count_of_letter == 255) {
			m_encoded.push_back(count_of_letter);
			m_encoded.push_back(initial[i]);
			count_of_letter = 1;
		}*/

		i++;
		if (i == end - 1) {
			if (initial[i] == initial[i - 1]) {
				m_encoded.push_back(count_of_letter);
				m_encoded.push_back(initial[i]);
			}
			else {
				count_of_letter = 1;
				m_encoded.push_back(count_of_letter);
				m_encoded.push_back(initial[i]);
			}
		}
	}
	
	

	return;
	
	
	//while (i < end) {
	//	count_of_letter = 1;
	//	
	//	//counting counts of equals letters (maximum:255)
	//	while (initial[i] == initial[i - 1]) {
	//		count_of_letter++;
	//		i++;
	//		if (count_of_letter == 255 || i == end) {
	//			break;
	//		}
	//		
	//	}

	//	/*if (count_of_letter == 1) {
	//		i+=1;
	//	}*/
	//	this->m_encoded.push_back(count_of_letter);
	//	this->m_encoded.push_back(initial[i-1]);
	//	i += 1;
	//}
	///*if (true) {
	//	m_encoded.push_back(unsigned char(1));
	//	m_encoded.push_back(initial.back());
	//}*/

}

void RLE::Decode()
{
	for (int i = 1; i < m_encoded.size(); i += 2) {
		for (unsigned char j = m_encoded[i - 1]; j> 0; j--) {
			m_decoded.push_back(m_encoded[i]);
		}
	}

}

RLE::RLE(std::vector<unsigned char> initialText)
{

	std::cout << "RLE STARTING\n";
	Encode(initialText);
	Decode();
	this->m_compressionCoefficient = (double)initialText.size() / (double)m_encoded.size();

}

std::vector<unsigned char> RLE::get_decode()
{
	return m_decoded;
}

std::vector<unsigned char> RLE::get_encode()
{
	return m_encoded;
}

void RLE::print_info()
{
	std::cout << "\nRLE INFORMATION:\n";
	std::cout << "Compression coefficient:\t" << m_compressionCoefficient << '\n';
	if (MATRIX_OUTPUT) {
		std::cout << "ENCODED SERIES:\n";
		for (int i = 1; i < m_encoded.size(); i += 2) {
			std::cout << std::dec << +m_encoded[i - 1] << ":\t" << std::hex << +m_encoded[i] << '\n';
		}
	}
}
