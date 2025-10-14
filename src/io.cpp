#include "io.h"
#include <fstream>
#include <iostream>
#include <arpa/inet.h> // ntohl
#include <cstring>

bool read_mnist_images(const std::string &path, Dataset &out){
    std::ifstream ifs(path, std::ios::binary);
    if(!ifs) return false;
    uint32_t magic=0, num=0, rows=0, cols=0;
    ifs.read((char*)&magic,4);
    magic = ntohl(magic);
    ifs.read((char*)&num,4); num = ntohl(num);
    ifs.read((char*)&rows,4); rows = ntohl(rows);
    ifs.read((char*)&cols,4); cols = ntohl(cols);
    if(magic != 2051){ std::cerr<<"MNIST: bad magic\n"; return false; }
    out.n = num; out.d = rows*cols;
    out.data.resize(out.n * out.d);
    for(size_t i=0;i<out.n;i++){
        for(size_t j=0;j<out.d;j++){
            unsigned char v;
            ifs.read((char*)&v,1);
            out.data[i*out.d + j] = static_cast<float>(v);
        }
    }
    return true;
}

bool read_fvecs(const std::string &path, Dataset &out){
    std::ifstream ifs(path, std::ios::binary);
    if(!ifs) return false;
    // read all vectors
    std::vector<float> buffer;
    while(ifs.peek()!=EOF){
        int32_t dim;
        ifs.read((char*)&dim,4);
        if(!ifs) break;
        // fvecs are little-endian; system likely little-endian on Linux
        if(dim<=0) return false;
        size_t d = (size_t)dim;
        std::vector<float> tmp(d);
        ifs.read(reinterpret_cast<char*>(tmp.data()), sizeof(float)*d);
        if(!ifs) break;
        // append
        if(out.d==0) out.d=d;
        if(out.d != d) {
            std::cerr<<"Inconsistent dim in fvecs\n";
            return false;
        }
        out.data.insert(out.data.end(), tmp.begin(), tmp.end());
        out.n++;
    }
    return true;
}
