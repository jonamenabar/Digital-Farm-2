

void lcdPrint(){

//*************************************PANTALLA***Se imprimen las variables***************************************************************************
lcd.clear();                                      // borra el contenido de la pantalla
lcd.setCursor(0, 0);                              // acomoda los cursores al inicion 
lcd.print("humidity 1:");                         // imprime la variable
lcd.setCursor(0,1);                               // imprime el valor de dicha variable
lcd.print(humedad1);

delay (2000);
lcd.clear();
//Serial.println(h);
lcd.setCursor(0, 0);
lcd.print("Temperature 1: ");

lcd.setCursor(0,1);
lcd.print(temperatura1);
//Serial.println(t);
delay(2000);                                      //Se espera 2 segundos para seguir leyendo //datos
lcd.clear();

lcd.setCursor(0, 0);
lcd.print("humidity 2:");
lcd.setCursor(0,1);
lcd.print(humedad2);
delay(2000);
lcd.clear();


//lcd.setCursor(0, 0);
//lcd.print("Temperature 2: ");
//lcd.setCursor(0,1);
//lcd.print(temperatura2);
//delay (2000);
//lcd.clear();
lcd.print("Digital farm 2");



}

