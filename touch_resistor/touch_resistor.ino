
int apin=0;
int val1=0;
int val2=0;


void setup(){
Serial.begin(9600); 

}


void loop(){
  val1=(((analogRead(apin)*(5000/1024)))-500)/10;  
  val2=(((analogRead(apin)*(3300/1024)))-500)/10;  
  Serial.println("value R = ");
  Serial.println(val1);
    Serial.println(val2);
  delay(1000);



}
