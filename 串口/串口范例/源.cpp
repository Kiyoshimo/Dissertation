

#include <locale.h>
#include <iostream>
#include <string>
#include <fstream>  
#include <string>  
#include <windows.h>
#include <conio.h>	//������ȡ
#include <vector>
#include <string> 
using namespace std;

#include "..\CnComm.h"	//Com��h��
using namespace std;
/*
int Fx = 640 / 2;	//����λ��X��
int Fy = 480 / 2;//����λ��X��
int Pump = 0;//��ˮ����
int CE = 560;//У��λ
*/
int redThre = 49; //��ɫ�����ľ�ֵ,115~135    
int saturationTh = 7; //55~65    
int FFX, FFY;//�ִ��и���XY
string txtin(void);
string ����(void);
vector<std::string> splitString(const string &strSrc, const string &pattern);
int SSS(std::string str);


/*
����ͷ������
	��ȹ�ǣ�46.66��   һ��Ϊ23.33
	�߶ȹ�ǣ�35.58��	һ��Ϊ17.79
*/


class HelloComm : public CnComm
{	
	void OnReceive()
	{
		TCHAR buffer[1024];
		setlocale(LC_ALL, "chs");// Unicode��������ʾ���� ����һ�� CBC ��֧�� "chs"
		_tprintf(_T("\nRecv:%s\n"), ReadString(buffer, 1024));
	}
};



/*
 *��char[]��arduino
 *Main()  �ַ���----> COM4 (������9600)
 *�����ʽ��("ˮƽ�Ƕ�","��ֱ�Ƕ�","ˮ������","У��λ(ǰ����֮��)")
 *  Ĭ��ģʽ �ص�IO �����߳�
 */
int main()
{
	HelloComm Com;
	int CP = 6;	//COM��
	Com.Open(CP, 9600);

	std::cout << "Com:" << CP <<"�Ѵ�"<< std::endl;
	system("pause");//��ͣ
	while (1) {
		string str;
		
		
		//str=����();
		str = txtin();

		cout<<"�������"<<str << endl;
		char c[20];
		const char *c_str();
		strcpy_s(c,str.c_str());

		if (_kbhit()) {if (_getch() == 27) { break; }}		//��סesc�˳�
		
		Com.Write(_T(c));
		Sleep(1000);
		
	}
	cout << "Com:" << CP << "�����ر�" <<endl;
	system("pause");//��ͣ

	return 0;
}

//���������
string ����(void){
		string str;
		cout << endl << endl;

		return str;
		
}

//��ʽ��Ŀ�������
string txtin(void) {
	ifstream in("../../ͼ��ʶ��/ͼ��ʶ��/CTRL.txt");
	string filename;
	string line;

	if (in) {
		while (getline(in, line)) // line�в�����ÿ�еĻ��з�  
		{
			cout << line << endl;
			SSS(line);
			// cout << "FFX" << FFX << "   FFY" << FFY;
		}
	}
	else { cout << "no such file" << endl; }

	int Pump = 100;
	int CE = FFX+ FFY+100;
	string str;
	str = to_string(FFX) + ","
		+ to_string(FFY) + ","
		+ to_string(Pump) + ","
		+ to_string(CE);
	return str;

}

//�ִ��и�1
vector<std::string> splitString(const string &strSrc, const string &pattern)
{
	vector<string > resultstr;
	if (strSrc == "")
		return resultstr;

	// ������ַ�����󣬿��Խ�ȡ���һ������
	std::string strcom = strSrc + pattern;
	auto pos = strSrc.find(pattern);
	auto len = strcom.size();

	//
	while (pos != std::string::npos)
	{
		std::string coStr = strcom.substr(0, pos);
		resultstr.push_back(coStr);

		strcom = strcom.substr(pos + pattern.size(), len);
		pos = strcom.find(pattern);
	}

	return resultstr;
}

//�ִ��и�2
int SSS(std::string str)
{
	
	int cont = 0;
	for (auto itor : splitString(str, ","))
	{

		string out = itor.c_str();
		std::string::size_type sz;
		int i = std::stoi(out, &sz);
		if (cont == 0) { FFX = i; cont++; }
		if (cont == 1) { FFY = i; }


	}
	return 0;
}
