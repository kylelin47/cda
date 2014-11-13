#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
using namespace std;
long parseInt(string line)
{
    long x;
    if (line[0] == '1')
    {
        for (int i=0; i<line.length(); ++i)
        {
            line[i] == '0' ? line[i] = '1' : line[i] = '0';
        }
        x = strtol(line.c_str(), 0, 2) + 1;
        x = -x;
    }
    else
    {
        x = strtol(line.c_str(), 0, 2);
    }
    return x;
}
string parseInstruction(string line, ofstream& disassembly)
{
    string returnValue;
    if (line.substr(0,2) == "01")
    {
        string op = line.substr(12, 4);
        if (op == "0000")
        {
            disassembly << "ADD ";
            returnValue = "ADD";
        }
        else if (op == "0001")
        {
            disassembly << "SUB ";
            returnValue = "SUB";
        }
        else if (op == "0010")
        {
            disassembly << "MUL ";
            returnValue = "MUL";
        }
        else if (op == "0011")
        {
            disassembly << "AND ";
            returnValue = "AND";
        }
        else if (op == "0100")
        {
            disassembly << "ADD ";
            returnValue = "ADD";
        }
        else if (op == "0101")
        {
            disassembly << "XOR ";
            returnValue = "XOR";
        }
        else if (op == "0110")
        {
            disassembly << "NOR ";
            returnValue = "NOR";
        }
        disassembly << "R" << strtol(line.substr(16, 5).c_str(), 0, 2) << ", ";
        disassembly << "R" << strtol(line.substr(2, 5).c_str(), 0, 2) << ", ";
        disassembly << "R" << strtol(line.substr(7, 5).c_str(), 0, 2);
    }
    else if (line.substr(0,2) == "00")
    {
        string op = line.substr(2, 4);
        if (op == "0000")
        {
            disassembly << "J ";
            returnValue = "J";
            disassembly << "#" << strtol(line.substr(6, 26).c_str(), 0, 2) * 4;
        }
        else if (op == "0111")
        {
            disassembly << "LW ";
            returnValue = "LW";
        }
        else if (op == "0010")
        {
            disassembly << "BEQ ";
            returnValue = "BEQ";
            disassembly << "R" << strtol(line.substr(6, 5).c_str(), 0, 2) << ", ";
            disassembly << "R" << strtol(line.substr(11, 5).c_str(), 0, 2) << ", ";
            disassembly << "#" << parseInt(line.substr(16, 16)) * 4;
        }
        else if (op == "0100")
        {
            disassembly << "BGTZ ";
            returnValue = "BGTZ";
            disassembly << "R" << strtol(line.substr(6, 5).c_str(), 0, 2) << ", ";
            disassembly << "#" << parseInt(line.substr(16, 16))* 4;
        }
        else if (op == "0101")
        {
            disassembly << "BREAK";
            returnValue = "BREAK";
        }
        else if (op == "0110")
        {
            disassembly << "SW ";
            returnValue = "SW";
        }
        if (op == "0111" || op == "0110")
        {
            disassembly << "R" << strtol(line.substr(11, 5).c_str(), 0, 2) << ", ";
            disassembly << parseInt(line.substr(16, 16)) << "(";
            disassembly << "R" << strtol(line.substr(6, 5).c_str(), 0, 2) << ")";
        }
    }
    else if (line.substr(0,2) == "10")
    {
        string op = line.substr(12, 4);
        if (op == "0000")
        {
            disassembly << "ADDI ";
            returnValue = "ADDI";
        }
        else if (op == "0001")
        {
            disassembly << "ANDI ";
            returnValue = "ANDI";
        }
        else if (op == "0010")
        {
            disassembly << "ORI ";
            returnValue = "ORI";
        }
        else if (op == "0011")
        {
            disassembly << "XORI ";
            returnValue = "XORI";
        }
        disassembly << "R" << strtol(line.substr(7, 5).c_str(), 0, 2) << ", ";
        disassembly << "R" << strtol(line.substr(2, 5).c_str(), 0, 2) << ", ";
        disassembly << "#" << parseInt(line.substr(16, 16));
    }
    return returnValue;
}
int main(int argc, char *argv[])
{
    ifstream inputfile (argv[1]);
    ofstream disassembly;
    ofstream simulation;
    disassembly.open ("disassembly.txt", ios::binary);
    string line;
    int address = 128;
    vector<long> data;
    int data_start = -1;
    bool b = false;
    while ( getline (inputfile,line) )
    {
        disassembly << line;
        disassembly << '\t';
        disassembly << address;
        disassembly << '\t';
        if (!b)
        {
            b = (parseInstruction(line, disassembly) == "BREAK");
        }
        else
        {
            if (data_start == -1)
            {
                data_start = address;
            }
            long x = parseInt(line);
            disassembly << x;
            data.push_back(x);
        }
        disassembly << '\n';
        address += 4;
    }
    disassembly.close();
    simulation.open ("simulation.txt", ios::binary);
    inputfile.clear();
    inputfile.seekg(0, ios::beg);
    int cycle = 1;
    address = 128;
    int registers[32] = {0};
    while ( getline (inputfile,line) )
    {
        simulation << "--------------------" << '\n';
        simulation << "Cycle:" << cycle << '\t' << address << '\t';
        string instruction = parseInstruction(line, simulation);
        if (instruction == "ADDI")
        {
            registers[strtol(line.substr(7, 5).c_str(), 0, 2)] =
            registers[strtol(line.substr(2, 5).c_str(), 0, 2)] + parseInt(line.substr(16, 16));
        }
        else if (instruction == "ADD")
        {
            registers[strtol(line.substr(16, 5).c_str(), 0, 2)] =
            registers[strtol(line.substr(2, 5).c_str(), 0, 2)] +
            registers[strtol(line.substr(7, 5).c_str(), 0, 2)];
        }
        else if (instruction == "MUL")
        {
            registers[strtol(line.substr(16, 5).c_str(), 0, 2)] =
            registers[strtol(line.substr(2, 5).c_str(), 0, 2)] *
            registers[strtol(line.substr(7, 5).c_str(), 0, 2)];
        }
        else if (instruction == "J" && cycle < 100)
        {
            long seek_position = strtol(line.substr(6, 26).c_str(), 0, 2) * 4;
            address = seek_position - 4;
            seek_position = (seek_position - 128)/4;
            inputfile.seekg(0, ios::beg);
            for (int i=0; i < seek_position; ++i)
            {
                getline(inputfile, line);
            }
        }
        else if (instruction == "BEQ")
        {
            if (registers[strtol(line.substr(6, 5).c_str(), 0, 2)] ==
                registers[strtol(line.substr(11, 5).c_str(), 0, 2)])
            {
                address += parseInt(line.substr(16, 16)) * 4;
                inputfile.seekg(0, ios::beg);
                int seek_position = (address - 124)/4;
                for (int i=0; i < seek_position; ++i)
                {
                    getline(inputfile, line);
                }
            }
        }
        else if (instruction == "BGTZ")
        {
            if (registers[strtol(line.substr(6, 5).c_str(), 0, 2)] > 0)
            {
                address += parseInt(line.substr(16, 16)) * 4;
                inputfile.seekg(0, ios::beg);
                int seek_position = (address - 124)/4;
                for (int i=0; i < seek_position; ++i)
                {
                    getline(inputfile, line);
                }
            }
        }
        else if (instruction == "LW")
        {
            int address = registers[strtol(line.substr(6, 5).c_str(), 0, 2)] +
                          parseInt(line.substr(16, 16));
            registers[strtol(line.substr(11, 5).c_str(), 0, 2)] = data[(address-data_start)/4];
        }
        else if (instruction == "SW")
        {
            int address = registers[strtol(line.substr(6, 5).c_str(), 0, 2)] +
                          parseInt(line.substr(16, 16));
            data[(address-data_start)/4] = registers[strtol(line.substr(11, 5).c_str(), 0, 2)];
        }
        simulation << '\n' << '\n';
        simulation << "Registers" << '\n';
        simulation << "R00: " << '\t';
        for (int i=0; i<8; ++i)
        {
            simulation << registers[i];
            if (i != 7)
                simulation << '\t';
        }
        simulation << '\n';
        simulation << "R08: " << '\t';
        for (int i=8; i<16; ++i)
        {
            simulation << registers[i];
            if (i != 15)
                simulation << '\t';
        }
        simulation << '\n';
        simulation << "R16: " << '\t';
        for (int i=16; i<24; ++i)
        {
            simulation << registers[i];
            if (i != 23)
                simulation << '\t';
        }
        simulation << '\n';
        simulation << "R24: " << '\t';
        for (int i=24; i<32; ++i)
        {
            simulation << registers[i];
            if (i != 31)
                simulation << '\t';
        }
        simulation << '\n' << "Data";
        int tag = data_start;
        for (int i=0; i<data.size(); ++i)
        {
            if (i % 8 == 0)
            {
                simulation << '\n';
                simulation << tag << ":" <<'\t';
                tag += 32;
            }
            simulation << data[i];
            if ( (i %8) != 7)
                simulation << '\t';
        }
        if (instruction == "BREAK")
        {
            break;
        }
        simulation << '\n' << '\n';
        ++cycle;
        address += 4;
    }
    simulation.close();
    return 0;
}
