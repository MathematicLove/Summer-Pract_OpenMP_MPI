#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <ctime>


#include "bmp_read.h"
#include "Fano.h"
#include "stddfns.h"
#include <mpi.h>

bool isCorrectDecode(std::vector<unsigned char> &A, std::vector<unsigned char> &B) {
    if (A.size() != B.size()) {
        std::cout << "NOT SIMMILIAR SIZES\n";
        return false;
    }
    bool cor = true;
    for (auto i = 0; i < A.size(); i++) {
        if (A[i] != B[i]) {
//			std::cout << i << "\tNUMBER" << "A[i]: " << int(A[i]) <<"\tB[i]: "<<        int(B[i]) <<"\n";
            cor = false;
        }
    }
    return cor;

}

std::vector<unsigned char> toByteStream(std::vector<unsigned char> bitStream) {

    int curPos = 0;
    int sizeOfBits = bitStream.size();
    std::vector<unsigned char> byteStream;

    for (int i = 0; i < sizeOfBits; i++) {
        curPos = 0;
        std::string bits;
        while (i + 1 < sizeOfBits && curPos < 8) {
            curPos++;
            if (int(bitStream[i]) == 0) {
                bits.push_back('0');
            } else {
                bits.push_back('1');
            }
            i++;
        }

        int byten = std::stoi(bits, nullptr, 2);
        unsigned char to_arr = byten;
        byteStream.push_back(to_arr);
    }
    return byteStream;
}

int random_range(int from, int to_excluded) {
    return from + std::rand() / ((RAND_MAX + 1u) / (to_excluded - from));
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);


    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //	const char fileName[] = "pic1.bmp";
    //	const char newFileName[] = "new_pic1.bmp";
    //std::string fileName = "pic1.bmp";

    std::vector<unsigned char> fileInLine;
    double starttime, endtime;

    BITMAPFILEHEADER fileHeader;
    std::string fileName, newFileName, randFileName = "randbmp.bmp";
    BITMAPINFOHEADER fileInfoHeader;
    std::ifstream fileStream(fileName, std::ifstream::binary);
    std::ofstream newFileStream(newFileName, std::ofstream::binary);
    std::ofstream randFileStream(randFileName, std::ofstream::binary);


    RGBQUAD **rgbInfo;

    if (rank == 0) {

        //std::cout << "Enter filename (*.bmp):\t";
        //std::cin >> fileName;

        if (argc < 2) {
            fileName = "test.bmp";
        } else {
            fileName = argv[1];
        }

        newFileName = "new_" + fileName;

        std::cout << "using file " << fileName << "...\n";
        // открываем файл
        //std::ifstream
        fileStream.open(fileName, std::ifstream::binary);
        //std::ofstream
        newFileStream.open(newFileName, std::ofstream::binary);

        if (!fileStream) {
            std::cout << "Error opening file '" << fileName << "'." << std::endl;
            return 0;
        }

        // заголовк изображения

        read(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType)); // type ((need to 0x4D42))
        read(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize)); // size of file bfSize*PIXELS_COUNT
        read(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1)); // reserved and equals zero
        read(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2)); // ...
        read(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits)); // starts of file

        write(newFileStream, fileHeader.bfType, sizeof(fileHeader.bfType)); // type ((need to 0x4D42))
        write(newFileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize)); // size of file bfSize*PIXELS_COUNT
        write(newFileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1)); // reserved and equals zero
        write(newFileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2)); // ...
        write(newFileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits)); // starts of file

        //generate new random bmp

        write(randFileStream, fileHeader.bfType, sizeof(fileHeader.bfType)); //
        write(randFileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize)); //
        write(randFileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1)); //
        write(randFileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2)); // ...
        write(randFileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits)); // starts of file




        if (fileHeader.bfType != 0x4D42) { // check BMP
            std::cout << "Error: '" << fileName << "' is not BMP file." << std::endl;
            return 0;
        }

        // информация изображения
        read(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize)); // размер самой структуры
        write(newFileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));
        write(randFileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));
        // bmp core
        if (fileInfoHeader.biSize >= 12) {
            read(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth)); //Ширина картинки
            read(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));//Высота картинки
            read(fileStream, fileInfoHeader.biPlanes,
                 sizeof(fileInfoHeader.biPlanes));//количество плоскостей (всегда пока равно 1)
            read(fileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));//бит на пиксель (глубина)

            write(newFileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth)); //
            write(newFileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));//
            write(newFileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));//
            write(newFileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));

            ///rand image
            write(randFileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
            write(randFileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));//
            write(randFileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));//
            write(randFileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
        }

        // получаем информацию о битности
        int colorsCount = fileInfoHeader.biBitCount >> 3;
        if (colorsCount < 3) {
            colorsCount = 3;
        }

        int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
        int maskValue = (1 << bitsOnColor) - 1;

        // bmp v1
        if (fileInfoHeader.biSize >= 40) {
            read(fileStream, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));//степень сжатися - 0
            read(fileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));//size in bytes
            read(fileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));//system params
            read(fileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));//system params
            read(fileStream, fileInfoHeader.biClrUsed,
                 sizeof(fileInfoHeader.biClrUsed));//counts of colors from table if zero -> max
            read(fileStream, fileInfoHeader.biClrImportant,
                 sizeof(fileInfoHeader.biClrImportant));//count of imprtn colors. if zero -> all imprt

            write(newFileStream, fileInfoHeader.biCompression,
                  sizeof(fileInfoHeader.biCompression));//степень сжатися - 0
            write(newFileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));//size in bytes
            write(newFileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));//system params
            write(newFileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));//system params
            write(newFileStream, fileInfoHeader.biClrUsed,
                  sizeof(fileInfoHeader.biClrUsed));//counts of colors from table if zero -> max
            write(newFileStream, fileInfoHeader.biClrImportant,
                  sizeof(fileInfoHeader.biClrImportant));//count of imprtn colors. if zero -> all imprt
            //rand
            write(randFileStream, fileInfoHeader.biCompression,
                  sizeof(fileInfoHeader.biCompression));//степень сжатися - 0
            write(randFileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));//size in bytes
            write(randFileStream, fileInfoHeader.biXPelsPerMeter,
                  sizeof(fileInfoHeader.biXPelsPerMeter));//system params
            write(randFileStream, fileInfoHeader.biYPelsPerMeter,
                  sizeof(fileInfoHeader.biYPelsPerMeter));//system params
            write(randFileStream, fileInfoHeader.biClrUsed,
                  sizeof(fileInfoHeader.biClrUsed));//counts of colors from table if zero -> max
            write(randFileStream, fileInfoHeader.biClrImportant,
                  sizeof(fileInfoHeader.biClrImportant));//count of imprtn colors. if zero -> all imprt
        }

        // bmp v2
        fileInfoHeader.biRedMask = 0;
        fileInfoHeader.biGreenMask = 0;
        fileInfoHeader.biBlueMask = 0;
        // если маска не задана, то ставим маску по умолчанию
        if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0) {
            fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
            fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
            fileInfoHeader.biBlueMask = maskValue;
        }

        fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
        // rgb info

        rgbInfo = new RGBQUAD *[fileInfoHeader.biHeight];

        for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
            rgbInfo[i] = new RGBQUAD[fileInfoHeader.biWidth];

        }

        // определение размера отступа в конце каждой строки
        int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;

        // чтение
        unsigned int bufer;

        for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
            for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
                read(fileStream, bufer, fileInfoHeader.biBitCount / 8);

                rgbInfo[i][j].rgbRed = bitextract(bufer, fileInfoHeader.biRedMask);
                rgbInfo[i][j].rgbGreen = bitextract(bufer, fileInfoHeader.biGreenMask);
                rgbInfo[i][j].rgbBlue = bitextract(bufer, fileInfoHeader.biBlueMask);
                rgbInfo[i][j].rgbReserved = bitextract(bufer, fileInfoHeader.biAlphaMask);


            }
            fileStream.seekg(linePadding, std::ios_base::cur);

        }

        // вывод


        for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
            for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
                fileInLine.push_back(rgbInfo[i][j].rgbRed);
                fileInLine.push_back(rgbInfo[i][j].rgbGreen);
                fileInLine.push_back(rgbInfo[i][j].rgbBlue);

                if (MATRIX_OUTPUT) {
                    std::cout << std::hex
                              << (int) rgbInfo[i][j].rgbRed << " "
                              << (int) rgbInfo[i][j].rgbGreen << " "
                              << (int) rgbInfo[i][j].rgbBlue << " "
                              //<< +rgbInfo[i][j].rgbReserved
                              << std::endl;
                }
            }
            //std::cout << std::endl;
        }
        std::srand(std::time(0));
        for (int i = 0; i < fileInLine.size(); i += 3) {

            unsigned char p1, p2, p3;

            p1 = random_range(0, 256);
            p2 = random_range(120, 256);
            p3 = random_range(0, 201);
            write(randFileStream, p1, sizeof(unsigned char));
            write(randFileStream, p2, sizeof(unsigned char));
            write(randFileStream, p3, sizeof(unsigned char));
        }
        //send data to antoher nodes
        //
        //
        for (int i = 1; i < size; i++) {
            MPI_Send(fileInLine.data(), fileInLine.size(), MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
            std::cout << "[LOG DATA] Root send data to " << i << "\n";
        }

    } else {
        MPI_Status status;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

        int dataSize;
        MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &dataSize);

        fileInLine.resize(dataSize);
        MPI_Recv(fileInLine.data(), dataSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "[LOG DATA] Process " << rank << " receive data from root\n";
    }

    //calculate chunks of data
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        starttime = MPI_Wtime();
    }


    int tsize = size - 1;// количество вычислительных узлов
    int trank = rank - 1;
    int chunkSize = fileInLine.size() / tsize;
    int remainder = fileInLine.size() % tsize;
    int myChunkSize = (trank < remainder) ? chunkSize + 1 : chunkSize;
    int myOffset = trank * chunkSize + std::min(trank, remainder);

    //std::cout<<"[LOG INFO] Process: " << rank << " has: " << chunkSize << " " << remainder <<    " " << myChunkSize << " " << myOffset << "\n";



    std::vector<unsigned char> copyF = fileInLine;
    if (rank != 0) {
        std::vector<unsigned char> sendData(fileInLine.begin() + myOffset, fileInLine.begin() + myOffset + myChunkSize);
        std::cout << "[LOG INFO] Process :" << rank << " has chunk: " << myOffset << " : " << myOffset + myChunkSize
                  << "\n";
        Fano FanoCode(sendData);
        std::cout << "Process: " << rank << " encode me and decode\n";
        std::vector<unsigned char> decoded = FanoCode.get_decode();

        if (rank == 1) {
            std::cout << decoded.size() << " <SIZES rank! 1> " << sendData.size() << "\n";
            std::cout << int(FanoCode.get_encode()[0]) << ' ' << int(FanoCode.get_encode()[1]) << "\n";
        }
        MPI_Send(decoded.data(), decoded.size(), MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        std::vector<unsigned char> totalDecode;
        std::vector <std::vector<unsigned char>> recvChunks(size);
        //collect from other ranks
        for (int i = 1; i < size; i++) {
            std::vector<unsigned char> recvBuffer(chunkSize);

            MPI_Recv(recvBuffer.data(), recvBuffer.size(), MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            recvChunks[i] = recvBuffer;
            //totalDecode.insert(totalDecode.end(),recvBuffer.begin(),recvBuffer.end());
        }
        for (int i = 1; i < size; i++) {
            totalDecode.insert(totalDecode.end(), recvChunks[i].begin(), recvChunks[i].end());
            std::cout << "[LOG INFO] Insert to " << i << " chunk with size: " << recvChunks[i].size() << "\n";
        }
        endtime = MPI_Wtime();
        std::cout << "That took: " << endtime - starttime << " secs.\n";
        std::ofstream log_file("time_log.txt", std::ios::app);
        log_file << fileName << ": " << size - 1 << "\t" << endtime - starttime << " secs.\n";
        //check correcting
        if (isCorrectDecode(totalDecode, fileInLine)) {
            std::cout << "\nDECODE CORRECT\n";
        } else {
            std::cout << "\nDECODE INCORRECT!!!!!\n";
        }


        //rgbrgb
        //0123456
        //bgr
        //210
        std::vector<unsigned char> newPic = totalDecode;
        for (auto i = 0; i < newPic.size(); i += 3) {
            write(newFileStream, newPic[i + 2], sizeof(newPic[i + 2]));
            write(newFileStream, newPic[i + 1], sizeof(newPic[i + 1]));
            write(newFileStream, newPic[i], sizeof(newPic[i]));
        }
        Fano FanoRoot(fileInLine);
        std::vector<unsigned char> toEncodedFile = FanoRoot.get_encode();
        toEncodedFile = toByteStream(toEncodedFile);
        std::ofstream encodedStream("encoded.bmp", std::ofstream::binary);

        for (auto byte: toEncodedFile) {
            write(encodedStream, byte, sizeof(byte));
        }
    }


    MPI_Finalize();
/*
	RLE RLECode(fileInLine);
	RLECode.print_info();

	std::vector<unsigned char>newPic_rle;
	newPic_rle = RLECode.get_decode();

	if (isCorrectDecode(newPic_rle, fileInLine)) {
		std::cout << "\nDECODING CORRECT\n";
	}
	else {
		std::cout << "DECODING(RLE) INCORRECT\n";
		return 0;
	}

	std::cout << "\nTwo-step ENCODE:\n";
	
	std::cout << "\nRLE->FANO\n";
	std::vector<unsigned char> Encode_rle;
	Encode_rle = RLECode.get_encode();
	std::cout << "\nSUPPLY RLE ARRAY INTO FANO ALGOS";
	Fano TS_RF(Encode_rle);
	TS_RF.print_info();

	std::cout << "\nFANO->RLE\n";
	std::vector<unsigned char> Encode_fano;
	Encode_fano = FanoCode.get_encode();
	std::cout << "\nSUPPLY FANO ARRAY INTO RLE ALGOS";
	RLE TS_FR(Encode_fano);
	TS_FR.print_info();

*/

    //free memory
//	for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
//		delete[] rgbInfo[i];
//	}
    return 0;
}
