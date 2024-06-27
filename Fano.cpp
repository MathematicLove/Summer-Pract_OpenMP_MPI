#include "Fano.h"
#include "stddfns.h"
#include <cmath>

bool cmpF(std::pair<unsigned char, double>& A, std::pair<unsigned char, double>& B) {
	return A.second < B.second;
}

std::vector<std::pair<unsigned char, double>> myPSort(std::map<unsigned char, int>& M, std::vector<std::pair<unsigned char, double>>& V) {


	for (auto& it : M) {
		V.push_back(it);
	}

	std::sort(V.begin(), V.end(), cmpF);
	std::reverse(V.begin(), V.end());

	return V;
}

Fano::Fano(std::vector<unsigned char> initialText)
{
	std::cout << "\nFANO STARTING:\n";
	std::map<unsigned char, int> MapProb;
	for (auto letter : initialText) {
		MapProb[letter]++;
	}

	this->m_inititalSize = static_cast<int>(initialText.size());

	myPSort(MapProb,this->m_probabilities); // find probabilites by letters and sorting them by greater

	for (auto i = 0; i < m_probabilities.size();i++) {
		m_tableOfCodes.push_back(Code(m_probabilities[i].first, "", m_probabilities[i].second));
		m_probabilities[i].second = m_probabilities[i].second / initialText.size(); // Find absolute probabilites -> wi / total size of text
	}
	std::string tmp1 = "0";
	std::string tmp2 = "1";

	Encode(0, m_probabilities.size() - 1, tmp1, tmp2);

	for (Code el : m_tableOfCodes) {
		m_mapOfCodes.insert(std::make_pair(el.letter, el.code)); //make map for codes
		m_mapOfDecode.insert(std::make_pair(el.code, el.letter));
	}

	for (auto letter : initialText) {
		m_rangeOfBits += m_mapOfCodes[letter];
	}

	for (auto letter : m_rangeOfBits) {
		m_encoded_vuc.push_back(letter - '0');
	}

	this->m_mathWeighted = 0;
	for (Code el : m_tableOfCodes) {
		m_mathWeighted += ((double)el.freq / m_inititalSize) * (double)el.code.size();
	}

	this->m_compressionCoefficient = (m_inititalSize * 8) / (double)(m_rangeOfBits.size());

	Decode();
}

void Fano::Encode(int li, int ri, std::string& branch, std::string& fullBranch)
{
	std::string cBranch = "";
	long double dS = 0;
	
	int i, m, S = 0;
	int med;

	cBranch = fullBranch + branch;
	//if size of array become 1:
	if (li == ri) {
		m_tableOfCodes[li].code = cBranch;
		return;
	}
	//calculate pivot (median)
	
	long double Sb = 0;
	for (int i = li; i <= ri - 1; i++) {
		Sb += m_probabilities[i].second;
	}
	long double Se = m_probabilities[ri].second;
	med = ri;

	long double d=std::fabs(Sb-Se);
	while (std::fabs(Sb - Se) <= d) {
		d = std::fabs(Sb - Se);
		med--;
		Sb -= m_probabilities[med].second;
		Se += m_probabilities[med].second;
	}
	/*
	do {
		d = abs(Sb - Se);
		med--;
		Sb -= m_probabilities[med].second;
		Se += m_probabilities[med].second;
	} while (abs(Sb-Se) >= d);*/



	std::string zero = "0";
	std::string one = "1";
	

	Encode(li, med, zero, cBranch);
	Encode(med + 1, ri, one, cBranch);
}

void Fano::Decode()
{
	int max_codeSize=0;
	for (auto it : m_mapOfCodes) {
		max_codeSize = std::max((int)it.second.size(), max_codeSize);
	}

	int tmp_delta = 0;

	for (auto i = 0; i < m_rangeOfBits.size();) {
		if (m_mapOfDecode.find(m_rangeOfBits.substr(i, tmp_delta)) != m_mapOfDecode.end()) {
			m_decoded.push_back(m_mapOfDecode[(m_rangeOfBits.substr(i,tmp_delta))]);
			i += tmp_delta;
			tmp_delta = 0;
		}
		else {
			tmp_delta++;
		}
	}
}

void Fano::print_info()
{
	std::cout << "\nFANO-SHANNON INFORMATION:\n";
	std::cout << "CODES:\n";
	for (Code el : m_tableOfCodes) {
		std::cout << std::hex << (int)(el.letter) << " --- " << el.code << " --- " << std::dec <<el.freq <<  '\n';
	}

	std::cout << "\nPrice of encode: " << m_mathWeighted;
	std::cout << "\nCompression coefficient : " << m_compressionCoefficient;
	if (MATRIX_OUTPUT) {
		std::cout << "\nEncoded:\t" << m_rangeOfBits << '\n';
		for (int i = 0; i < m_decoded.size(); i++) {
			if (i % 3 == 0) {
				std::cout << '\n';
			}
			std::cout << std::hex << +m_decoded[i] << ' ';
		}
	}
}

std::vector<unsigned char> Fano::get_decode()
{
	return m_decoded;
}

std::vector<unsigned char> Fano::get_encode()
{
	return m_encoded_vuc;
}
