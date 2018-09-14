#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1  //0x100 001 21h dec:33
#define SUBU 3	//0x100 011 23h	dec:35
#define AND 4	//0x100 100 24h dec:36
#define OR  5	//0x100 101 25h dec:37
#define NOR 7	//0x100 111 27h dec:39
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

string decode_R_result[6] = {};
string decode_I_result[4] = {};

class RF
{
public:
	bitset<32> ReadData1, ReadData2;
	RF()
	{
		Registers.resize(32); //represent 32 RFs
		Registers[0] = bitset<32>(0);
	}

	void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
	{
		// implement the funciton by you.
		if (WrtEnable == 1) {
			Registers[WrtReg.to_ulong()] = WrtData; // index must not be bitset
		}
		ReadData1 = Registers[RdReg1.to_ulong()];
		ReadData2 = Registers[RdReg2.to_ulong()];
	}

	void OutputRF() // write RF results to file
	{
		ofstream rfout;
		rfout.open("RFresult.txt", std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "A state of RF:" << endl;
			for (int j = 0; j < 32; j++)
			{
				rfout << Registers[j] << endl;
			}

		}
		else cout << "Unable to open file";
		rfout.close();

	}
private:
	vector<bitset<32> >Registers; //Define a container(Register) that stores instances-bitset<32> 

};

class ALU
{
public:
	bitset<32> ALUresult;
	bitset<32> ALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
	{
		// implement the ALU operations by you. 
		switch (ALUOP.to_ulong()) {
			case ADDU:
				ALUresult = bitset<32> (oprand1.to_ulong() + oprand2.to_ulong());
				cout << "addu:" << oprand1.to_ulong() << " + " << oprand2.to_ulong() << "=" << ALUresult << endl;
				break;
			case SUBU:
				ALUresult = bitset<32> (oprand1.to_ulong() - oprand2.to_ulong());
				cout << "subu:" << oprand1.to_ulong() << " - " << oprand2.to_ulong() << "=" << ALUresult << endl;
				break;
			case AND:
				ALUresult = bitset<32> (oprand1 & oprand2);
				cout << "and:" << oprand1.to_ulong() << " and " << oprand2.to_ulong() << "=" << ALUresult << endl;
				break;
			case OR:
				ALUresult = bitset<32> (oprand1 | oprand2);
				cout << "or:" << oprand1.to_ulong() << " or " << oprand2.to_ulong() << "=" << ALUresult << endl;
				break;
			case NOR:
				ALUresult = bitset<32> (~(oprand1 | oprand2));
				cout << "nor:" << oprand1.to_ulong() << " nor " << oprand2.to_ulong() << "=" << ALUresult << endl;
				break;
		}

		return ALUresult;
	}
};

class INSMem
{
public:
	bitset<32> Instruction;
	INSMem() // read instruction memory
	{
		IMem.resize(MemSize);
		ifstream imem;
		string line;
		int i = 0;
		imem.open("imem.txt");
		if (imem.is_open())
		{
			while (getline(imem, line))
			{
				IMem[i] = bitset<8>(line);
				i++;
			}

		}
		else cout << "Unable to open file";
		imem.close();

	}

	bitset<32> ReadMemory(bitset<32> ReadAddress)
	{
		// implement by you. (Read the byte at the ReadAddress and the following three byte).
		/*string tmp = "";
		tmp = IMem[ReadAddress.to_ulong()].to_string() + IMem[ReadAddress.to_ulong() + 1].to_string() +
			IMem[ReadAddress.to_ulong() + 2].to_string() + IMem[ReadAddress.to_ulong() + 3].to_string();
		Instruction = bitset<32>(tmp); //to_string()method converts ulong to string that
														//is easy to append. eg: "1000" + "0001" = "10000001",if ulong: 1000 + 0001 = 1001, which is not right.
		return Instruction;*/

		Instruction = bitset<32>(IMem[ReadAddress.to_ulong()].to_string() + IMem[ReadAddress.to_ulong() + 1].to_string() + IMem[ReadAddress.to_ulong() + 2].to_string() + IMem[ReadAddress.to_ulong() + 3].to_string());
		return Instruction;
	}

private:
	vector<bitset<8> > IMem;

};

class DataMem
{
public:
	bitset<32> readdata;
	DataMem() // read data memory
	{
		DMem.resize(MemSize);
		ifstream dmem;
		string line;
		int i = 0;
		dmem.open("dmem.txt");
		if (dmem.is_open())
		{
			while (getline(dmem, line))
			{
				DMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		dmem.close();

	}
	bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
	{

		// implement by you.
		if (writemem.to_ulong() == 1 && readmem.to_ulong() == 0){
			DMem[Address.to_ulong()] = bitset<8> (WriteData.to_string().substr(0, 8));
			DMem[Address.to_ulong() + 1] = bitset<8> (WriteData.to_string().substr(8, 8));
			DMem[Address.to_ulong() + 2] = bitset<8> (WriteData.to_string().substr(16, 8));
			DMem[Address.to_ulong() + 3] = bitset<8> (WriteData.to_string().substr(24, 8));
		}

		if (readmem.to_ulong() == 1 && writemem.to_ulong() == 0){
			string tmp = "";
			tmp = DMem[Address.to_ulong()].to_string() + DMem[Address.to_ulong() + 1].to_string() +
				DMem[Address.to_ulong() + 2].to_string() + DMem[Address.to_ulong() + 3].to_string();
			readdata = bitset<32>(tmp);
		}
		return readdata;
	}

	void OutputDataMem()  // write dmem results to file
	{
		ofstream dmemout;
		dmemout.open("dmemresult.txt");
		if (dmemout.is_open())
		{
			for (int j = 0; j < 1000; j++)
			{
				dmemout << DMem[j] << endl;
			}

		}
		else cout << "Unable to open file";
		dmemout.close();

	}

private:
	vector<bitset<8> > DMem;

};

string *decode(bitset<32> instruction) {
	//string *result = new string[3];
	string op_code = instruction.to_string().substr(0, 6);
	//cout << "op_code: " << op_code << endl;
	if (op_code == "000000") {	//R-type
		bitset<5> rs = bitset<5> (instruction.to_string().substr(6, 5));
		bitset<5> rt = bitset<5> (instruction.to_string().substr(11, 5));
		bitset<5> rd = bitset<5> (instruction.to_string().substr(16, 5));
		//what is the use of (6,10) shamt??
		bitset<6> funct = bitset<6> (instruction.to_string().substr(26, 6));
		bitset<3> funct_0_2 = bitset<3> (funct.to_string().substr(3, 3)); //line 7 - 11
		decode_R_result[0] = rs.to_string();
		decode_R_result[1] = rt.to_string();
		decode_R_result[2] = rd.to_string();
		decode_R_result[3] = funct.to_string();
		decode_R_result[4] = funct_0_2.to_string();
		decode_R_result[5] = op_code;
		//cout << decode_result[0] << endl;
		return decode_R_result;
	} else if(op_code == "100011" || op_code == "101011") { //I-type LW
		bitset<16> imm = bitset<16>(instruction.to_string().substr(16, 16));
		bitset<5> rt = bitset<5>(instruction.to_string().substr(11, 5));
		bitset<5> rs = bitset<5>(instruction.to_string().substr(6, 5));
		decode_I_result[0] = imm.to_string();
		decode_I_result[1] = rt.to_string();
		decode_I_result[2] = rs.to_string();
		decode_I_result[3] = op_code;
	}
	
	return decode_I_result;
}



int main()
{
	RF myRF;
	ALU myALU;
	INSMem myInsMem;
	DataMem myDataMem;
	bitset<32> pc = 0;

	bitset<32> instruction = 0;
	bitset<32> ALU_result = 0;


	while (1)
	{

		// Fetch
		instruction = myInsMem.ReadMemory(pc);
		cout << "Got instruction: "<< instruction << endl;
		// If current instruciton is "11111111111111111111111111111111", then break;
		if (instruction.to_string() == "11111111111111111111111111111111") {
			cout << "Finished" << endl;
			break;
		}
		// decode(Read RF)
		else {
			string *p;
			p = decode(instruction); //
			if (p[5] == "000000") {	//R-type u-type; p = {rs, rt, rd, funct, funct_0_2, op_code}  Debug completed!
				cout << "rs = " << p[0] << endl;
				cout << "rt = " << p[1] << endl;
				cout << "rd = " << p[2] << endl;
				cout << "funct = " << p[3] << endl;
				cout << "0-2 = " << p[4] << endl;
				cout << "op_code = " << p[5] << endl;
				myRF.ReadWrite(bitset<5> (p[0]), bitset<5>(p[1]), NULL, NULL, bitset<1>("0"));
				bitset<32> rs = myRF.ReadData1;
				bitset<32> rt = myRF.ReadData2;
				ALU_result = myALU.ALUOperation(bitset<3>(p[4]), rs, rt);
				myRF.ReadWrite(NULL, NULL, bitset<5>(p[2]), ALU_result, bitset<1>("1"));

				//the following is for Debug use
				cout << "----------------" << endl;
				myRF.ReadWrite(bitset<5>(p[2]), NULL, NULL, NULL, bitset<1>("0"));
				bitset<32> rs_addr = myRF.ReadData1;
				cout << "confirm: " << rs_addr << endl;
			}else if (p[3] == "100011") { //I-type:LW, p = {imm, rt, rs, op_code}]   Debug completed!
				/*********ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)*********/
				cout << "imm = " << p[0] << endl;
				cout << "rt = " << p[1] << endl;
				cout << "rs = " << p[2] << endl;
				cout << "op_code = " << p[3] << endl;
				myRF.ReadWrite(bitset<5> (p[2]), NULL, NULL, NULL, bitset<1> ("0"));
				bitset<32> rs_data_at_addr = myRF.ReadData1;
				bitset<32> addr = bitset<32> (rs_data_at_addr.to_ulong() + bitset<16> (p[0]).to_ulong()); // addr = rs_addr + imm
				cout << "addr(rs_addr + imm)= " << addr << endl;
				bitset<32> datamem = myDataMem.MemoryAccess(addr, NULL, bitset<1>("1"), bitset<1>("0")); //get datamem ready to be written into the RF
				cout << "data->cpu = " << datamem << endl;
				myRF.ReadWrite(NULL, NULL, bitset<5> (p[1]), datamem, bitset<1> ("1"));
				
				//the following is for Debug use
				cout << "----------------" << endl;
				myRF.ReadWrite(bitset<5>(p[1]), NULL, NULL, NULL, bitset<1>("0"));
				bitset<32> rs_addr = myRF.ReadData1;
				cout << "confirm: " << rs_addr << endl;
			}else if (p[3] == "101011") { //I-type:SW, p = {imm, rt, rs, op_code}
				cout << "imm = " << p[0] << endl;
				cout << "rt = " << p[1] << endl;
				cout << "rs = " << p[2] << endl;
				cout << "op_code = " << p[3] << endl;
				myRF.ReadWrite(bitset<5>(p[2]), bitset<5>(p[1]), NULL, NULL, bitset<1>("0"));
				bitset<32> rs_addr = myRF.ReadData1;
				bitset<32> addr = bitset<32>(rs_addr.to_ulong() + bitset<16>(p[0]).to_ulong()); // addr = rs_addr + imm
				bitset<32> wrtdata = myRF.ReadData2;
				myDataMem.MemoryAccess(addr, wrtdata, bitset<1>("0"), bitset<1>("1")); //get datamem ready to be written into the RF 
			}
		}
		// Execute

		// Read/Write Mem

		// Write back to RF

		myRF.OutputRF(); // dump RF; 
		pc = bitset<32> (pc.to_ulong() + 4);
		string decode_result[6] = {};
		cout << "==============================" << endl;
	}
	myDataMem.OutputDataMem(); // dump data mem

	cout << "按任意键继续……";
	cin.clear();
	cin.sync();
	cin.get();
	return 0;

}
