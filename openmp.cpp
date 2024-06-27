#include <omp.h>
#include <iostream>
#include <vector>
#include "bmp_read.h"
#include "Fano.h"

bool isCorrectDecode(std::vector<unsigned char> &A, std::vector<unsigned char> &B);

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

int main(int argc, char *argv[]) {
    int nthreads, tid;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    RGBQUAD **rgbInfo;

    std::string fileName;
    std::ifstream fileStream;
    std::ofstream newFileStream;
    std::vector<unsigned char> fileInLine;
    double starttime, finishtime; // время начала и конца работы параллелного участка программы

    std::vector<std::vector<unsigned char>> recvChunks;

#pragma omp parallel private(nthreads, tid)
    {

        tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();

        if (tid == 0) {
            if (argc < 2) {
                fileName = "test.bmp";
            } else {
                fileName = argv[1];
            }
            std::string newFileName = "new_" + fileName;

            fileStream = std::ifstream(fileName, std::ifstream::binary);
            newFileStream = std::ofstream(newFileName, std::ofstream::binary);

            if (!fileStream) {
                std::cout << "Error opening file '" << fileName << "'." << std::endl;
                exit(0);
            }

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


            if (fileHeader.bfType != 0x4D42) { // check BMP
                std::cout << "Error: '" << fileName << "' is not BMP file." << std::endl;
                exit(0);
            }

            // информация изображения
            read(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize)); // размер самой структуры
            write(newFileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));
            // bmp core
            if (fileInfoHeader.biSize >= 12) {
                read(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth)); //Ширина картинки
                read(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));//Высота картинки
                read(fileStream, fileInfoHeader.biPlanes,
                     sizeof(fileInfoHeader.biPlanes));//количество плоскостей (всегда пока равно 1)
                read(fileStream, fileInfoHeader.biBitCount,
                     sizeof(fileInfoHeader.biBitCount));//бит на пиксель (глубина)

                write(newFileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth)); //
                write(newFileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));//
                write(newFileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));//
                write(newFileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
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
                read(fileStream, fileInfoHeader.biCompression,
                     sizeof(fileInfoHeader.biCompression));//степень сжатися - 0
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
                write(newFileStream, fileInfoHeader.biXPelsPerMeter,
                      sizeof(fileInfoHeader.biXPelsPerMeter));//system params
                write(newFileStream, fileInfoHeader.biYPelsPerMeter,
                      sizeof(fileInfoHeader.biYPelsPerMeter));//system params
                write(newFileStream, fileInfoHeader.biClrUsed,
                      sizeof(fileInfoHeader.biClrUsed));//counts of colors from table if zero -> max
                write(newFileStream, fileInfoHeader.biClrImportant,
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

            for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
                for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
                    fileInLine.push_back(rgbInfo[i][j].rgbRed);
                    fileInLine.push_back(rgbInfo[i][j].rgbGreen);
                    fileInLine.push_back(rgbInfo[i][j].rgbBlue);
                }
            }

            recvChunks.resize(nthreads - 1);
        }
        #pragma omp barrier
        if (tid == 0) {
            starttime = omp_get_wtime();
        }

        int tsize = nthreads - 1;// количество выч. потоков
        int trank = tid - 1;
        int chunkSize = fileInLine.size() / tsize;
        int remainder = fileInLine.size() % tsize;
        int myChunkSize = (trank < remainder) ? chunkSize + 1 : chunkSize; // размер сегмента
        int myOffset = trank * chunkSize + std::min(trank, remainder); // начало сегмента

        std::vector<unsigned char> localData;

        if (tid != 0) {
            localData.reserve(myChunkSize);
            std::vector<unsigned char> sendData(fileInLine.begin() + myOffset, fileInLine.begin() + myOffset + myChunkSize);
            std::cout << "[LOG INFO] Thread: " << tid << " has chunk: " << myOffset << " : " << myOffset + myChunkSize
                  << "\n";
            Fano FanoCode(sendData);
            std::cout << "Process: " << tid << " encoded\n";
            std::vector<unsigned char> decoded = FanoCode.get_decode();
            recvChunks[trank] = decoded;
        }
        #pragma omp barrier
        if (tid == 0) {
            std::vector<unsigned char> totalDecode;
            for (int i = 0; i < tsize; i++) {
                totalDecode.insert(totalDecode.end(), recvChunks[i].begin(), recvChunks[i].end());
                std::cout << "[LOG INFO] Insert to " << i << " chunk with size: " << recvChunks[i].size() << "\n";
            }
            if (isCorrectDecode(totalDecode, fileInLine)) {
                std::cout << "\nDECODE CORRECT\n";
            } else {
                std::cout << "\nDECODE INCORRECT!!!!!\n";
            }
            finishtime = omp_get_wtime();
            std::cout << "That took: " << finishtime - starttime << " secs.\n";
            std::ofstream{"time_log.txt", std::ios::app} << fileName << ": " << nthreads - 1 << "\t" << finishtime - starttime << " secs.\n";

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
    }
}

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