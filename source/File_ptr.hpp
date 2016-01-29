//
//  File_ptr.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 24/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef File_ptr_hpp
#define File_ptr_hpp

#include <stdio.h>
#include <iostream>
#include <exception>
#include <string>

class File_ptr {
    FILE * p;
    std::string file_name;
    const int read_dim = 300;
public:
    
    File_ptr():p(nullptr){}                                   //default constructor
    File_ptr(const char* n,const char* a)                     //constructor building file_ptr from file name,as a C-style string, and opening mode
    :p{fopen(n,a)}, file_name(n)
    {
        if(p==nullptr) throw std::runtime_error{"File_ptr: Can't open file: " + file_name};
    }
    
    
    File_ptr(const std::string& n, const char* a)             //constructor building file_ptr from filen name, as a std::string, and opening mode
    :File_ptr{n.c_str(), a}
    {}
    
    
    explicit File_ptr(FILE* pp)                               //constructor building file_ptr from a FILE* object, it is explicit because
    :p{pp}                                                    //I don't wanto to define an implicit conversion from FILE* to file_ptr
    {
        if(p==nullptr) throw  std::runtime_error("File_ptr: nullptr");
            }
    
    File_ptr& operator=(const File_ptr& _fp) = delete;                 //copy assignement deleted
    
    File_ptr& operator=(File_ptr&& _fp) = delete;                      //move assignement deleted
    
    File_ptr(const File_ptr& _fp) = delete;                            //copy constructor deleted
    
    File_ptr(File_ptr&& _fp)                                           //move constructor
    :p{_fp.p}
    {
        _fp.p = nullptr;
        //std::cout << "Move constructor for File_ptr has been used\n";
    }
    
    
    int read(char* line)
    {
        if(fgets(line, read_dim, p) == NULL)
            return 0;
        else
            return 1;
    }
    
    int write(const char* line)
    {
        return fprintf(p,"%s\n",line);
    }
    
    int write(const std::string& line)
    {
        return write(line.c_str());
    }
    
    ~File_ptr() {fclose(p); }
    
    operator FILE*() {return p;}
    
    
};

#endif /* File_ptr_hpp */