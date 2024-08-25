/*
  -----------------------------------
  DIBUAT oleh Sabiyo Riezky Pratama
  TERIMA KASIH kepada: Rafbus

  SEBAGIAN KODE dari:
  Rafbus/Arduino-Simple-Quiz (Github)
  -----------------------------------
*/

// Pin LED, buzzer, dan tombol
#define allLED 2      
#define buzzer 13     
#define ButtonRed 11  
#define ButtonBlue 12 

// Alamat I2C dan konfigurasi LCD
#define I2C_ADDR 0x27   
#define LCD_COLUMNS 16  
#define LCD_LINES 2     

#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <avr/wdt.h>  // Library untuk Watchdog Timer

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);  // Inisialisasi LCD

boolean red, blue;  // Status tombol

// Data pertanyaan dan jawaban
String atasQ = "< BENAR  SALAH >";  
String bawahQ[] = { "Pertanyaan No. 1", "Pertanyaan No. 2", "Pertanyaan No. 3", 
                   "Pertanyaan No. 4", "Pertanyaan No. 5", "Pertanyaan No. 6", 
                   "Pertanyaan No. 7" };
int maksQ = 7;  // Jumlah maksimum pertanyaan
int nomorQ = 0;  // Indeks pertanyaan saat ini
int pertanyaanQ[] = { 1, 0, 0, 1, 0, 1, 0 };  // Jawaban benar: 1=benar, 0=salah

unsigned long lastInputTime;                  // Waktu input terakhir
const unsigned long timeoutDuration = 60000;  // Timeout 60 detik

int skor = 0;  // Variabel untuk menyimpan skor

// Fungsi: Menampilkan pesan selamat datang
boolean tampilanSelamatDatang() {
  tone(buzzer, 3000, 100); delay(200);
  tone(buzzer, 3000, 100);

  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("Ingin bermain?");
  lcd.setCursor(1, 1); lcd.print("Tekan  \"Benar\"");

  while (digitalRead(ButtonBlue) == HIGH);  // Tunggu hingga tombol "Benar" ditekan

  tone(buzzer, 1000, 1000); delay(200);
  tone(buzzer, 2000, 2000); delay(200);
  tone(buzzer, 3000, 3000);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Selamat   datang");
  lcd.setCursor(0, 1); lcd.print("di  Benar/Salah!");

  delay(4000); lcd.clear(); delay(500);

  tone(buzzer, 3000, 500); delay(200);
  tone(buzzer, 2000, 1000); delay(200);
  tone(buzzer, 1000, 1500);

  lcd.setCursor(4, 0); lcd.print("Spesial");
  lcd.setCursor(0, 1); lcd.print("Kemerdekaan Ind'");

  delay(2000); lcd.clear(); delay(2000);

  tone(buzzer, 3000, 100); delay(200);
  tone(buzzer, 3000, 100);

  lastInputTime = millis();  // Reset timer
  return true;  // Mengembalikan true saat tombol "Benar" ditekan
}

// Fungsi: Reset permainan setelah timeout
void resetPermainanDenganTimeout() {
  if (millis() - lastInputTime > timeoutDuration) {
    nomorQ = 0;   
    skor = 0; // Reset skor
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("WAKTU HABIS");
    lcd.setCursor(0, 1); lcd.print("ME-RESET ...");
    tone(buzzer, 100, 5000); delay(2000);

    tampilanSelamatDatang();  // Tampilkan layar selamat datang lagi
  }
}

// Setup: Inisialisasi awal
void setup() {
  lcd.init(); lcd.backlight();

  pinMode(allLED, OUTPUT);            
  pinMode(ButtonRed, INPUT_PULLUP);   
  pinMode(ButtonBlue, INPUT_PULLUP);  

  tampilanSelamatDatang();  // Tampilkan tampilan selamat datang
}

// Fungsi: Menangani jawaban benar
void JawabanQBenar() {
  lcd.clear(); lcd.print("Jawabanmu benar!");
  skor++;  // Tambah skor jika jawaban benar
  tone(buzzer, 3000, 100); delay(200);
  tone(buzzer, 3500, 100); delay(1000);

  lcd.clear();
  digitalWrite(allLED, HIGH); delay(500); 
  digitalWrite(allLED, LOW);  
}

// Fungsi: Menangani jawaban salah
void JawabanQSalah() {
  lcd.clear(); lcd.print("Jawabanmu salah!");
  tone(buzzer, 262, 100); delay(200);
  tone(buzzer, 100, 100); delay(1000);

  lcd.clear();
  for (int i = 0; i < 2; i++) {
    digitalWrite(allLED, HIGH); delay(200);
    digitalWrite(allLED, LOW); delay(200);
  }
}

// Fungsi: Tampilkan pesan akhir
void akhir() {
  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("Terima   kasih");
  lcd.setCursor(1, 1); lcd.print("sudah bermain!");
  delay(2000);

  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("Skor  akhir:");
  lcd.setCursor(4, 1); lcd.print(skor);  // Tampilkan skor akhir
  lcd.print(" dari ");
  lcd.print(maksQ);  // Tampilkan skor akhir dan jumlah pertanyaan
  delay(3000);

  wdt_enable(WDTO_15MS);  // Aktifkan watchdog untuk reset
  while (1);  // Tunggu reset
}

// Loop utama
void loop() {
  if (digitalRead(ButtonRed) == LOW || digitalRead(ButtonBlue) == LOW) {
    lastInputTime = millis();  // Reset timer saat ada input
  }

  resetPermainanDenganTimeout();  // Cek timeout

  red = digitalRead(ButtonRed);   
  blue = digitalRead(ButtonBlue);  

  if (red == LOW && blue == LOW) {  // Reset permainan jika kedua tombol ditekan
    nomorQ = 0;
    skor = 0;  // Reset skor
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("PERMAINAN");
    lcd.setCursor(0, 1); lcd.print("DI-RESET ...");
    tone(buzzer, 100, 5000); delay(2000);
    tampilanSelamatDatang();
    return;
  }

  if (nomorQ != maksQ) {  // Tampilkan pertanyaan jika belum selesai
    lcd.setCursor(0, 0); lcd.print(bawahQ[nomorQ]);
    lcd.setCursor(0, 1); lcd.print(atasQ);
  }

  if (blue == LOW) {  // Jika tombol biru ditekan
    if (pertanyaanQ[nomorQ] == 1) {
      JawabanQBenar();
    } else {
      JawabanQSalah();
    }
    nomorQ++;  // Pertanyaan berikutnya
  }

  if (red == LOW) {  // Jika tombol merah ditekan
    if (pertanyaanQ[nomorQ] == 0) {
      JawabanQBenar();
    } else {
      JawabanQSalah();
    }
    nomorQ++;  // Pertanyaan berikutnya
  }

  if (nomorQ == maksQ) {  // Jika semua pertanyaan selesai
    akhir();  // Tampilkan pesan akhir
    while (true);  // Hentikan loop
  }
}
