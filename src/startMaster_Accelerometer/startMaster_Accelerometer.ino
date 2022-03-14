#include<Wire.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int16_t AcX_old,AcY_old,AcZ_old,Tmp_old,GyX_old,GyY_old,GyZ_old;
int16_t d_AcX,d_AcY,d_AcZ,d_Tmp,d_GyX,d_GyY,d_GyZ;
int16_t norm_deriv;
int16_t sum_abs_deriv_YZ;
int16_t thresh = 8.00; // to be tuned
bool start = false;

int dt = 200; // ms
bool flag_init = false;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  Tmp = Tmp/340.00+36.53;
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  if (flag_init == true) {
    d_AcX = (AcX-AcX_old)/dt;
    d_AcY = (AcY-AcY_old)/dt;
    d_AcZ = (AcZ-AcZ_old)/dt;
    d_Tmp = (Tmp-Tmp_old)/dt;
    d_GyX = (GyX-GyX_old)/dt;
    d_GyY = (GyY-GyY_old)/dt;
    d_GyZ = (GyZ-GyZ_old)/dt;

    norm_deriv = sqrt(pow(d_AcX,2)+pow(d_AcY,2)+pow(d_AcZ,2)+pow(d_Tmp,2)+pow(d_GyX,2)+pow(d_GyY,2)+pow(d_GyZ,2));
    sum_abs_deriv_YZ = abs(d_AcZ);
 
    /*
    Serial.print("AcX = "); Serial.print(AcX);
    Serial.print(" | AcY = "); Serial.print(AcY);
    Serial.print(" | AcZ = "); Serial.print(AcZ);
    Serial.print(" | Tmp = "); Serial.print(Tmp);  //equation for temperature in degrees C from datasheet
    Serial.print(" | GyX = "); Serial.print(GyX);
    Serial.print(" | GyY = "); Serial.print(GyY);
    Serial.print(" | GyZ = "); Serial.println(GyZ);
    */

    /*
    Serial.print(AcX); Serial.print(","); Serial.print(AcY); Serial.print(","); Serial.print(AcZ);  Serial.print(",");
    Serial.print(Tmp);  Serial.print(",");
    Serial.print(GyX);  Serial.print(","); Serial.print(GyY);  Serial.print(","); Serial.println(GyZ);
    */
    
    /*
    Serial.print(d_AcX); Serial.print(","); Serial.print(d_AcY); Serial.print(","); Serial.print(d_AcZ);  Serial.print(",");
    Serial.print(d_Tmp);  Serial.print(",");
    Serial.print(d_GyX);  Serial.print(","); Serial.print(d_GyY);  Serial.print(","); Serial.println(d_GyZ);
    */

    /*
    Serial.print(d_AcY); Serial.print(","); Serial.println(d_AcZ);
    */
    
    if (sum_abs_deriv_YZ >= thresh) {
      start = true;
    }
    
    Serial.print(sum_abs_deriv_YZ);
    Serial.print(",");
    Serial.println(int(start)*10);
    
  }

  AcX_old = AcX;
  AcY_old = AcY;
  AcZ_old = AcZ;
  Tmp_old = Tmp;
  GyX_old = GyX;
  GyY_old = GyY;
  GyZ_old = GyZ;
  
  if (flag_init == false) {
    flag_init = true;
  }

  start = false;
  delay(dt);
}
