

#include <locale.h>
#include <iostream>
#include <string>
#include <fstream>  
#include <string>  
#include <windows.h>
#include <conio.h>	//按键读取
#include <vector>
#include <string> 
using namespace std;

#include "..\CnComm.h"	//Com口h档
using namespace std;
/*
int Fx = 640 / 2;	//火焰位置X轴
int Fy = 480 / 2;//火焰位置X轴
int Pump = 0;//出水力度
int CE = 560;//校验位
*/
int redThre = 49; //红色分量的均值,115~135    
int saturationTh = 7; //55~65    
int FFX, FFY;//字串切割后的XY
string txtin(void);
string 调试(void);
vector<std::string> splitString(const string &strSrc, const string &pattern);
int SSS(std::string str);


/*
摄像头参数：
	宽度广角：46.66度   一半为23.33
	高度广角：35.58度	一半为17.79
*/


class HelloComm : public CnComm
{	
	void OnReceive()
	{
		TCHAR buffer[1024];
		setlocale(LC_ALL, "chs");// Unicode下中文显示不出 配置一下 CBC 不支持 "chs"
		_tprintf(_T("\nRecv:%s\n"), ReadString(buffer, 1024));
	}
};



/*
 *传char[]给arduino
 *Main()  字符串----> COM4 (波特率9600)
 *输出格式：("水平角度","竖直角度","水泵力度","校验位(前三项之和)")
 *  默认模式 重叠IO 监视线程
 */
int main()
{
	HelloComm Com;
	int CP = 6;	//COM口
	Com.Open(CP, 9600);

	std::cout << "Com:" << CP <<"已打开"<< std::endl;
	system("pause");//暂停
	while (1) {
		string str;
		
		
		//str=调试();
		str = txtin();

		cout<<"输出数据"<<str << endl;
		char c[20];
		const char *c_str();
		strcpy_s(c,str.c_str());

		if (_kbhit()) {if (_getch() == 27) { break; }}		//按住esc退出
		
		Com.Write(_T(c));
		Sleep(1000);
		
	}
	cout << "Com:" << CP << "即将关闭" <<endl;
	system("pause");//暂停

	return 0;
}

//调试用入口
string 调试(void){
		string str;
		cout << endl << endl;

		return str;
		
}

//正式项目用输入点
string txtin(void) {
	ifstream in("../../图像识别/图像识别/CTRL.txt");
	string filename;
	string line;

	if (in) {
		while (getline(in, line)) // line中不包括每行的换行符  
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

//字串切割1
vector<std::string> splitString(const string &strSrc, const string &pattern)
{
	vector<string > resultstr;
	if (strSrc == "")
		return resultstr;

	// 添加在字符串最后，可以截取最后一段数据
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

//字串切割2
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
