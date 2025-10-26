#include "io.h" // Header me orismous gia eisodo/eksodo dedomenwn
#include <fstream>  // Gia xeirismo arxeiwn se binary mode
#include <iostream> // Gia emfanisi minimatwn sfalmatos
#include <arpa/inet.h> // Gia ntohl (network to host long)
#include <cstring> // Gia diaxeirisi bytes kai strings

// Diavazei arxeio MNIST eikonwn kai to fortwnei sto Dataset
bool read_mnist_images(const std::string &path, Dataset &out){
    std::ifstream ifs(path, std::ios::binary); // Anoigei to arxeio se binary mode
    if(!ifs) return false; // Elegxei an anoixe epitixos
    uint32_t magic=0, num=0, rows=0, cols=0; // Arxikopoihsh metavlitwn gia header
    ifs.read((char*)&magic,4); magic = ntohl(magic); // Diavazei kai metatrepei to magic number
    ifs.read((char*)&num,4); num = ntohl(num); // Arithmos eikonwn
    ifs.read((char*)&rows,4); rows = ntohl(rows); // Grammes kathe eikonas
    ifs.read((char*)&cols,4); cols = ntohl(cols); // Stiles kathe eikonas
    if(magic != 2051){ std::cerr<<"MNIST: bad magic\n"; return false; } // Elegxei oti einai swsto to MNIST arxeio
    out.n = num; out.d = rows*cols; // Orizei to megethos tou dataset
    out.data.resize(out.n * out.d); // Desmevei mnimi gia ola ta pixel
    for(size_t i=0;i<out.n;i++){ // Gia kathe eikona
        for(size_t j=0;j<out.d;j++){ // Gia kathe pixel
            unsigned char v;
            ifs.read((char*)&v,1); // Diavazei 1 byte
            out.data[i*out.d + j] = static_cast<float>(v); // Apothikevei to pixel ws float
        }
    }
    return true; // Epistrefei true an ola ok
}

// Diavazei arxeio fvecs kai fortwnei ta vectors se Dataset
bool read_fvecs(const std::string &path, Dataset &out){
    std::ifstream ifs(path, std::ios::binary); // Anoigei to arxeio
    if(!ifs) return false; // Elegxei an anoixe epitixos
    std::vector<float> buffer; // Prosorinos xwros gia dedomena
    while(ifs.peek()!=EOF){ // Mexri na ftasoume sto telos tou arxeiou
        int32_t dim; 
        ifs.read((char*)&dim,4); // Diavazei tin diastasi tou vector
        if(!ifs) break;
        if(dim<=0) return false; // An einai lathos diastasi termatizei
        size_t d = (size_t)dim;
        std::vector<float> tmp(d); // Prosorinos vector gia ena deigma
        ifs.read(reinterpret_cast<char*>(tmp.data()), sizeof(float)*d); // Diavazei ta dedomena
        if(!ifs) break;
        if(out.d==0) out.d=d; // An einai to proto deigma, orizei diastasi
        if(out.d != d) { std::cerr<<"Inconsistent dim in fvecs\n"; return false; } // Elegxei omoiomorfia diastasewn
        out.data.insert(out.data.end(), tmp.begin(), tmp.end()); // Prosthetei to vector sto dataset
        out.n++; // Afksanei ton arithmo deigmatwn
    }
    return true; // Epistrefei true an diavastike swsta
}
