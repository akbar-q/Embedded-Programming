//ALL OK

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const float playbackSpeed = 0.6; // Adjust as needed

#define ev     14
#define wind   13
#define pv     12
#define fossil 27

#define SD_CS_PIN 5   // Define the CS (Chip Select) pin for the SD card
#define DAC_PIN   25  // Define the DAC pin (use DAC1 or DAC2)
#define BUTTON_PIN 15  // Pin for the Physical Press-Me Button

#define THANKS_BUTTON_PIN 34  // Pin to Receive High Signal for Thanks'

volatile bool thanksButtonPressed = false; // Flag to indicate if the thanks button has been pressed
volatile bool thanksButtonReady = false;   // Flag to indicate if the thanks button signal has stabilized

#define NUM_FILES 10 // Number of audio files to choose from
#define NUM_FILES_TO_PLAY 3 // Number of audio files to play
#define NUM_ADDITIONAL_FILES 5 // Number of additional files to choose from

//Name of File To PLAY
File audioFile;

int filesPlayed[NUM_FILES] = {0}; // Array to keep track of played files

#define THANKS_PIN_READ_TIMEOUT 100 // Timeout value in milliseconds
#define THANKS_SPECIFIC_FILE "thanks_specific.wav" // Specify the filename of the specific audio file

// LCD configuration
#define LCD_ADDRESS 0x27 // I2C address of the LCD
#define LCD_COLUMNS 16   // Number of columns in the LCD
#define LCD_ROWS 2       // Number of rows in the LCD

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(ev, OUTPUT);
  pinMode(pv, OUTPUT);
  pinMode(fossil, OUTPUT);
  pinMode(wind, OUTPUT);

  digitalWrite(ev, HIGH);
  digitalWrite(pv, HIGH);
  digitalWrite(fossil, HIGH);
  digitalWrite(wind, HIGH);

  // Initialize LCD
  lcd.init();
  lcd.backlight(); // Turn on the backlight

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Boot");
  lcd.setCursor(0, 1);
  lcd.print("by Akbar Q"); // Hello in French
  delay(10000);

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed.");
    return;
  }
  Serial.println("SD card initialized.");

  // Configure the DAC pin
  pinMode(DAC_PIN, OUTPUT);

  // Open an audio file for playback
  Serial.println("Attempt Boot Audio");
  audioFile = SD.open("/Speech/Speech.wav");
  if (!audioFile) {
    Serial.println("File not found.");
  }

  if (audioFile) {
    Serial.print("Playing audio file ");

    // Read audio data and play it
    while (audioFile.available()) {
      uint8_t sample = audioFile.read(); // Read a sample (8-bit, mono)
      dacWrite(DAC_PIN, sample);         // Send the sample to DAC
    }
  }

  // Close the audio file
  audioFile.close();
/*
  // Initialize button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach Interrupt to The Button Pin to Play Speech
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), playSpecificAudio, FALLING); */

  // Seed random number generator
  randomSeed(analogRead(0));

/*
  // Initialize thanks button pin
  pinMode(THANKS_BUTTON_PIN, INPUT); // HIGH signal from another microcontroller
/*
  // Attach interrupt to the thanks button pin for falling edge
  attachInterrupt(digitalPinToInterrupt(THANKS_BUTTON_PIN), thanksButtonInterrupt, FALLING);
*/
}

void loop() {
  // Play three random files
  for (int i = 0; i < NUM_FILES_TO_PLAY; i++) {
    // Choose a random file number
    int fileNumber = getRandomFile();

    // Open the selected audio file
    switch (fileNumber) {
      case 1:
        audioFile = SD.open("/Greetings/Bonjour.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bonjour mon ami!");
        lcd.setCursor(0, 1);
        lcd.print("Ca Va?"); // Hello in French
        break;
      case 2:
        audioFile = SD.open("/Greetings/Hello.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hello my friend!");
        lcd.setCursor(0, 1);
        lcd.print("Come Here"); // Hello in English
        break;
      case 3:
        audioFile = SD.open("/Greetings/Hola.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hola amigo!");
        lcd.setCursor(0, 1);
        lcd.print("Como estas?"); // Hello in Spanish
        break;
      case 4:
        audioFile = SD.open("/Greetings/Kamusta.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Kamusta ka!");
        lcd.setCursor(0, 1);
        lcd.print("Maganda?"); // Hello in Tagalog
        break;
      case 5:
        audioFile = SD.open("/Greetings/Konichiwa.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Konichiwa!");
        lcd.setCursor(0, 1);
        lcd.print("Genki desu ka?"); // Hello in Japanese
        break;
      case 6:
        audioFile = SD.open("/Greetings/marhaba.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Marhaba sadiqi!");
        lcd.setCursor(0, 1);
        lcd.print("Keifak?"); // Hello in Arabic
        break;
      case 7:
        audioFile = SD.open("/Greetings/Namaste.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Namaste dost!");
        lcd.setCursor(0, 1);
        lcd.print("Kaise ho?"); // Hello in Hindi
        break;
      case 8:
        audioFile = SD.open("/Greetings/Vanakam.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vanakam nanbare!");
        lcd.setCursor(0, 1);
        lcd.print("Eppadi irukkum?"); // Hello in Tamil
        break;
      case 9:
        audioFile = SD.open("/Thanks/Thanks.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("By Akbar Qamar");
        lcd.setCursor(0, 1);
        lcd.print("32Bit Beauty");
        break;
      case 10:
        audioFile = SD.open("/Music/Back2.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("32 Bits of");
        lcd.setCursor(0, 1);
        lcd.print("EXPRESSION");
        break;
      default:
        audioFile = SD.open("/Music/BackUP.wav");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Engineering");
        lcd.setCursor(0, 1);
        lcd.print("Meets Art");
        break;
    }

    // Check if the file opened successfully
    if (audioFile) {
      Serial.print("Playing audio file ");
      Serial.println(fileNumber);

      // Read audio data and play it
      while (audioFile.available()) {
        uint8_t sample = audioFile.read(); // Read a sample (8-bit, mono)
        dacWrite(DAC_PIN, sample);         // Send the sample to DAC
      }

      // Close the audio file
      audioFile.close();

      // Add some delay between playing each audio file
      delay(5000); // Adjust as needed
    } else {
      Serial.println("Failed to open audio file!");
    }
  }

  // Play one additional file from a list of five
  int additionalFileNumber = random(1, NUM_ADDITIONAL_FILES + 1);
  switch (additionalFileNumber) {
    case 1:
      audioFile = SD.open("/BackMusic/File1.wav");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("File1.wav");
      lcd.setCursor(0, 1);
      lcd.print("Playback Text 1"); // Custom text for File1.wav playback
      break;
    case 2:
      audioFile = SD.open("/BackMusic/File2.wav");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The Devestation");
      lcd.setCursor(0, 1);
      lcd.print("Is Unthinkable"); // Custom text for File2.wav playback
      break;
    case 3:
      audioFile = SD.open("/BackMusic/File3.wav");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Decentralized");
      lcd.setCursor(0, 1);
      lcd.print("Power Economy"); // Custom text for File3.wav playback
      break;
    case 4:
      audioFile = SD.open("/BackMusic/File4.wav");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Fight for Green");
      lcd.setCursor(0, 1);
      lcd.print("Freedom Future"); // Custom text for File4.wav playback
      break;
    case 5:
      audioFile = SD.open("/BackMusic/File5.wav");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Generate, Store");
      lcd.setCursor(0, 1);
      lcd.print("Distribute Earn"); // Custom text for File5.wav playback
      break;
    default:
      audioFile = SD.open("/BackMusic/BackUP.wav");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Look at Me I");
      lcd.setCursor(0, 1);
      lcd.print("Am Centre Piece"); // Custom text for backup playback
      break;
  }

  // Check if the additional file opened successfully
  if (audioFile) {
    Serial.print("Playing additional audio file ");
    Serial.println(additionalFileNumber);

    // Read audio data and play it
    while (audioFile.available()) {
      uint8_t sample = audioFile.read(); // Read a sample (8-bit, mono)
      dacWrite(DAC_PIN, sample);         // Send the sample to DAC
    }

    // Close the audio file
    audioFile.close();
  } else {
    Serial.println("Failed to open additional audio file!");
  }
}

void playSpecificAudio() {
  // Open the specific audio file
  audioFile = SD.open("/Speech/Speech.wav");

  // Check if the file opened successfully
  if (audioFile) {
    Serial.println("Playing Speech File");

    // Read audio data and play it
    while (audioFile.available()) {
      uint8_t sample = audioFile.read(); // Read a sample (8-bit, mono)
      dacWrite(DAC_PIN, sample);         // Send the sample to DAC
    }

    // Close the audio file
    audioFile.close();
  } else {
    Serial.println("Failed To Open Speech File");
  }
}

// Function to get a random file that hasn't been played yet
int getRandomFile() {
  int randomFile;
  do {
    randomFile = random(1, NUM_FILES + 1); // Add 1 to include upper bound
  } while (filesPlayed[randomFile - 1] == 1);

  filesPlayed[randomFile - 1] = 1; // Mark file as played

  return randomFile;
}

// Interrupt service routine to handle thanks button signal change
void thanksButtonInterrupt() {
  // Debouncing delay
  delay(50);

  // Check if the thanks button signal is still LOW after the delay
  if (digitalRead(THANKS_BUTTON_PIN) == LOW) {
    // Play the specific audio file
    playThanksSpecificAudio();
  }
}

// Function to handle playing thanks specific audio
void playThanksSpecificAudio() {
  // Open and play the specific audio file directly
  File audioFile = SD.open("/Thanks/Thanks.wav");

  // Check if the file opened successfully
  if (audioFile) {
    Serial.print("Playing thanks specific audio file: ");
    Serial.println(THANKS_SPECIFIC_FILE);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Feast Your Eyes");
    lcd.setCursor(0, 1);
    lcd.print("And Ears Now");

    // Read audio data and play it
    while (audioFile.available()) {
      uint8_t sample = audioFile.read(); // Read a sample (8-bit, mono)
      dacWrite(DAC_PIN, sample);         // Send the sample to DAC
    }

    // Close the audio file
    audioFile.close();
  } else {
    Serial.println("Failed to open thanks specific audio file!");
  }
}
