//
//  main.cpp
//  Proofread_QTS_File
//
//  Created by 唐健恆 on 2021/2/22.
//  Copyright © 2021 Alvin. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iconv.h>
#include <codecvt>
#include <filesystem>
#include <algorithm>
using namespace std;
using namespace __fs::filesystem;

int TITLENUMBER = 3;
string title[] = {"Final funds", "m", "Daily_risk", "Best generation", "Best experiment", "Best answer times", "Stock number", "Number", "Remain funds"};

string correct_title[] = {"最後資金", "預期報酬", "風險", "找到最佳解世代", "找到最佳解實驗#", "找到最佳解次數", "投資組合檔數", "張數", "剩餘資金"};

string file_dir = "test/2021_02_18_linear_output/H2M/train/";
string correct_file_dir = "test/DJI_30/H2M/Portfolio/訓練期/";

vector<string> title_vector(title, title + TITLENUMBER);
vector<string> correct_title_vector(correct_title, correct_title + TITLENUMBER);


std::string UnicodeToUTF8(const std::wstring & wstr)
{
std::string ret;
try {
std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
ret = wcv.to_bytes(wstr);
} catch (const std::exception & e) {
std::cerr << e.what() << std::endl;
}
return ret;
}

std::wstring UTF8ToUnicode(const std::string & str)
{
std::wstring ret;
try {
std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
ret = wcv.from_bytes(str);
} catch (const std::exception & e) {
std::cerr << e.what() << std::endl;
}
return ret;
}


std::string UnicodeToANSI(const std::wstring & wstr)
{
std::string ret;
std::mbstate_t state = {};
const wchar_t *src = wstr.data();
size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
if (static_cast<size_t>(-1) != len) {
std::unique_ptr< char [] > buff(new char[len + 1]);
len = std::wcsrtombs(buff.get(), &src, len, &state);
if (static_cast<size_t>(-1) != len) {
ret.assign(buff.get(), len);
}
}
return ret;
}

std::wstring ANSIToUnicode(const std::string & str)
{
std::wstring ret;
std::mbstate_t state = {};
const char *src = str.data();
size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
if (static_cast<size_t>(-1) != len) {
std::unique_ptr< wchar_t [] > buff(new wchar_t[len + 1]);
len = std::mbsrtowcs(buff.get(), &src, len, &state);
if (static_cast<size_t>(-1) != len) {
ret.assign(buff.get(), len);
}
}
return ret;
}

std::string UTF8ToANSI(const std::string & str)
{
return UnicodeToANSI(UTF8ToUnicode(str));
}

std::string ANSIToUTF8(const std::string & str)
{
return UnicodeToUTF8(ANSIToUnicode(str));
}





vector<vector<string>> readSpeData(string filename,  vector<string> title_list) {
    cout << filename << endl;
    ifstream inFile(filename, ios::in);
    string line;
    vector< vector<string> > data;
    bool sw = false;
    
    if (!inFile) {
        cout << "Open file failed!" << endl;
        exit(1);
    }
    
    while(getline(inFile, line)){
        vector<string> line_data;
        for(vector<string>::iterator it = title_list.begin(); it != title_list.end(); it++){
            istringstream delime(line);
            string s;
            while(getline(delime, s, ',')){
//                cout << "___" << s << "___" << endl;
                if(sw){
                    if(s != "\r" && s != "\0"){
                        s.erase(remove(s.begin(), s.end(), '\r'), s.end());
                        line_data.push_back(s);
                    }
                }else if(s == *it){
                    sw = true;
                }else{
                    break;
                }
            }
            if(sw){
                data.push_back(line_data);
//                title_list.erase(it);
                sw = false;
                break;
            }
        }
    }
    inFile.close();
    return data;
}

vector<string> genFilename(string fileDir){
    vector<string> temp;
    path p1(fileDir);
    for( directory_iterator it = directory_iterator(p1);
        it != directory_iterator(); ++ it )
    {
        path px = it->path();
        if(px.extension() == ".csv"){
//            cout << px.filename() << endl;
            temp.push_back(px.filename());
        }
    }
    
    sort(temp.begin(), temp.end());
    return temp;
}


void proofReading(vector<string> filename_list, vector<string> correct_filename_list){
    bool same = true;
    if(filename_list.size() != correct_filename_list.size()){
        cout << "file number is not the same!!" << endl;
        return;
    }
    for(int i = 0; i < filename_list.size(); i++){
        vector<vector<string>> myData;
        vector<vector<string>> correct_data;
        myData = readSpeData(file_dir + filename_list[i], title_vector);
        correct_data = readSpeData(correct_file_dir + correct_filename_list[i], correct_title_vector);
        
        cout << endl;
        for(int j = 0; j < myData.size(); j++){
            bool find_diff = false;
            for(int k = 0; k < myData[j].size(); k++){
                if(myData[j][k] != correct_data[j][k]){
                    same = false;
                    find_diff = true;
                    break;
                }
            }
            if(find_diff){
                cout << "Find different!!" << endl;
                cout << title[j] << ": " << endl;
                for(int k = 0; k < myData[j].size(); k++){
                    cout << myData[j][k] << ", ";
                }
                cout << endl;
                cout << correct_title[j] << ": " <<  endl;
                for(int k = 0; k < myData[j].size(); k++){
                    cout << correct_data[j][k] << ", ";
                }
                cout << endl << endl;
            }
        }
        
        cout << endl;
    }
    
    if(same){
        cout << "Total files are the same!" << endl;
    }
}

int main(int argc, const char * argv[]) {
//    proofReading();
    vector<string> filename_list;
    vector<string> correct_filename_list;
    filename_list = genFilename(file_dir);
    correct_filename_list = genFilename(correct_file_dir);
    proofReading(filename_list, correct_filename_list);
    return 0;
}
