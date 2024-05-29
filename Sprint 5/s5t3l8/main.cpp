#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories);
bool PreprocessSupport(ifstream& in, ofstream& out, const path& in_file, const vector<path>& include_directories);

void PrintError(const path& file_path, const path& file_name, const int str_pos){
    cout << "unknown include file " << file_path.filename().string()
         << " at file " << file_name.string() << " at line " << str_pos << "\n";
}

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories){

    if(!filesystem::exists(in_file)){
        return false;
    }

    ifstream f_in(in_file, ios::in);
    if(!f_in.is_open()){
        return false;
    }

    ofstream f_out(out_file, ios::out);
    if(!f_out.is_open()){
        return false;
    }

    return PreprocessSupport(f_in, f_out, in_file, include_directories);
}

bool PreprocessSupport(ifstream& in, ofstream& out, const path& in_file, const vector<path>& include_directories){
    static regex custom_lib (R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static regex standart_lib (R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
    smatch match;

    string str;
    int str_num = 0;
    

    while(getline(in, str)){
        
        bool is_custom_find = false;
        
        path find_f_path;
        
        ++str_num;

        if(regex_match(str, match, custom_lib)){
            find_f_path = in_file.parent_path() / string(match[1]);

            ifstream find_file(find_f_path);
                
            if(find_file.is_open()){
                    
                if(!PreprocessSupport(find_file, out, find_f_path, include_directories)){
                    PrintError (find_f_path, in_file, str_num);
                    return false;
                }
                is_custom_find = true;
                continue;   
            }
        }
        
        if(!is_custom_find || regex_match(str, match, standart_lib)){
            bool is_std_find = false;
            
            for(const auto& dir : include_directories){
                find_f_path = dir / string(match[1]);

                ifstream find_file (find_f_path);

                if(find_file.is_open()){
                    if(!PreprocessSupport(find_file, out, find_f_path, include_directories)){
                        PrintError (find_f_path, in_file, str_num);
                        return false;
                    }
                    is_std_find = true;
                    break;
                }
            }

            if(!is_std_find){
                PrintError (find_f_path, in_file, str_num);
                return false;
            }
            continue; 
        }
        out << str << "\n";
    }
    return true;
}

string GetFileContents(string file) {
    ifstream stream(file);

    // конструируем string по двум итераторам
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }
    
    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p})));
    
    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"s;

    //assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}

/*
// напишите эту функцию
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories){

    if(!filesystem::exists(in_file)){
        return false;
    }

    ifstream f_in(in_file, ios::in);
    if(!f_in.is_open()){
        return false;
    }

    ofstream f_out(out_file, ios::out);

    static regex custom_lib (R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static regex standart_lib (R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
    smatch m;

    string str;
    int str_num = 0;
    path find_f_path;

    bool is_custom_find = false;
    bool is_std_find = false;

    while(getline(f_in, str)){
        
        ++str_num; 
        
        if(regex_match(str, m, custom_lib)){
            find_f_path = in_file.parent_path() / string(m[1]);
            
            if(filesystem::exists(find_f_path)){
                ifstream find_file(find_f_path);
                
                if(find_file.is_open()){
                    //cout << str << "\n";
                    f_out << str << "\n";
                    is_custom_find = true;
                    continue;
                } else {
                    cout << "unknown include file " << find_f_path.filename().string()
                         << "at file" << in_file.string() << " at line " << str << "\n";
                    return false;
                }
            }
        }

        if(regex_match(str, m, standart_lib)){
            for(const auto& dir : include_directories){
                find_f_path = dir / string(m[1]);

                if(!filesystem::exists(find_f_path)){
                    ifstream find_file (find_f_path);

                    if(find_file.is_open()){
                        //cout << str << "\n";
                        f_out << str << "\n";
                        is_std_find = true;
                    } else {
                        cout << "unknown include file " << find_f_path.filename().string()
                             << "at file" << in_file.string() << " at line " << str << "\n";
                        return false;
                    }
                }
            }
        }
        if(!is_custom_find && !is_std_find){
            cout << "unknown include file " << find_f_path.filename().string()
                   << "at file" << in_file.string() << " at line " << str << "\n";
            return false;
                f_out << str << "\n";
        }
    } 
    return true;
}*/