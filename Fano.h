#ifndef FANO_H
#define FANO_H

#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <cstdlib>

struct Code {
	unsigned char letter;
	std::string code;
	int freq;

	Code(unsigned char tl, std::string tc, int tf) {
		letter = tl;
		code = tc;
		freq = tf;
	}
};

class Fano
{
private:
	std::vector<std::pair<unsigned char, double>> m_probabilities; // table of probabilites of letter
	std::vector<Code> m_tableOfCodes; // table of codes
	std::map<unsigned char, std::string> m_mapOfCodes;
	std::map<std::string, unsigned char> m_mapOfDecode;
	std::string m_rangeOfBits;
	std::vector<unsigned char> m_encoded_vuc;
	std::vector<unsigned char> m_decoded;

	int m_inititalSize;
	
	double m_mathWeighted,m_compressionCoefficient; //entropy;

	void Encode(int li, int ri, std::string& branch, std::string& fullBranch); // encode function
	void Decode();

public:
	Fano(std::vector<unsigned char> initialText); // constructor with initial text
	void print_info();
	std::vector<unsigned char> get_decode();
	std::vector<unsigned char> get_encode();

	

};
#endif // FANO_H
