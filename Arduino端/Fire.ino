#include <Servo.h>

String mes;
String aout;
Servo servo_h;  // 建立一個 servo 物件
Servo servo_v;  // 建立一個 servo 物件

int H=90;  //水平角度
int V=90;  //竖直角度
int P=100;//水泵
int CE=280; //校验位（ce=H+V+P)
int pump=5;//pump脚位
void setup() {
 Serial.begin(9600);
  servo_h.attach(6); 
  servo_v.attach(7); 
}

void loop() {
  mes=String("");
  aout=String("接收到信号为:");

  //接收到信号----------
  while (Serial.available()>0){
    char data= Serial.read();
     mes+=data;
    delay(2);}
    
  //分割数据--------------
  if(mes!=""){
    //aout+=mes;
    //Serial.println(aout);
    int commaPosition; 
    int cont=0; 
    do{
      commaPosition = mes.indexOf(',');
      if(commaPosition != -1){
        if(cont==0)H=mes.substring(0,commaPosition).toInt();
        else if(cont==1)V=mes.substring(0,commaPosition).toInt();
        else if(cont==2)P=mes.substring(0,commaPosition).toInt();
        mes = mes.substring(commaPosition+1, mes.length());
        cont++; 
        }
      else{if(mes.length() > 0)CE=mes.toInt();}
        }
    while(commaPosition >=0);
    Serial.print("H=");Serial.println(H);
    Serial.print("V=");Serial.println(V);
    Serial.print("P=");Serial.println(P);
    Serial.print("CE=");Serial.println(CE);
    }
    
//步进马达----------------------
  if(CE==(H+V+P)){
    //if( ((H<=90)||(H>=-90)) || ((V<=90)||(V>=-90)) ||((P<=50)||(H>=0)) )
    servo_h.write(H); 
    servo_v.write(V); 
    analogWrite(pump,P);
    delay(15);   
  
  
      
  }
  else{
    Serial.print("检测到错误（CE）");
    H=V=P=CE=0;
  }
}

