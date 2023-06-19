#include <TFT_eSPI.h>
#include <SPI.h>
#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include "time.h"
#include "Variables.h"

TFT_eSPI tft =TFT_eSPI();

Audio audio;
   //adsasdasd
 void setup() { 
  Serial.begin (115200);
 //************Setup for the buttons *******************
pinMode (pin_up,INPUT);
pinMode (pin_down,INPUT);
//pinMode (pin_left,INPUT);
pinMode (pin_right,INPUT);
pinMode (pin_middle,INPUT);
pinMode (pin_next,INPUT);
pinMode (pin_prev,INPUT);
pinMode (pin_pause,INPUT);
  // Serial.println("Checkpoint 1");
  // ************Setup for the tft dysplay ***************
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setSwapBytes(true);
  tft.setCursor(0, 0);
  // Serial.println("Checkpoint 2");
  //*************Setup for the micro SD*******************
  // Set microSD Card CS as OUTPUT and set HIGH
  pinMode(SD_CS, OUTPUT);      
  digitalWrite(SD_CS, HIGH); 
  // Initialize SPI bus for microSD Card
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  // Serial.println("Checkpoint 3");
   if(!SD.begin(SD_CS))
    {
      Serial.println("Error accessing microSD card!");
      tft.drawString("SD card error",5,5,1);
      while(true); 
    }
  
  //Serial.println("Checkpoint 4");
   if (SD.exists("/Names.txt"))
      { 
      Serial.println("Where the file at");
      get_music_from_file();
      }
    else 
    {  
       Serial.println("No config file Detected, Making it");  
       
       get_music_list(SD);
       make_music_file(SD);   
    }
  //  Serial.println("Checkpoint 5");
//  get_music_list(fs::FS &fs)
//***************Setup for the I2S interface**************
    // Setup I2S 
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
    // Set Volume

    Serial.println("*****************good til here***********");
  audio.connecttoFS(SD,music_list[0].name.c_str());
  srand(time(NULL)); 
 } 



 void loop() 
{ 
   
   audio.loop();
   set_volume();
   if (refresh_screen)
   {
    display_songs();
    refresh_screen = false;
   }
   
   if (millis()-button_time > button_repetition_time)
   {
    if (digitalRead(pin_prev) == HIGH)
      previous_song();
    if (digitalRead(pin_next) == HIGH)
      next_song();
    if (digitalRead(pin_pause) == HIGH)
      pause_song();
    if (digitalRead(pin_right) == HIGH)
      right();
    if (digitalRead(pin_up) == HIGH)
      up();
    if (digitalRead(pin_down) == HIGH)
      down();
    if (digitalRead(pin_middle) == HIGH)
      middle();   
   }
   if (millis()-song_time>1000 && millis()-button_time < 10000)
   {
    song_time = millis();
    display_song_time();
    screen_saver=true;
   }
   else if (millis()-button_time > 10000)
   {  
    if (screen_saver)
    {
      screen_saver = false;
       tft.fillScreen(TFT_BLACK);
    }
   
    display_saver();  
   }
   
}

void next_song()
{
     current_song++;
     if (current_song >= total_songs)
        current_song = 0;
     play_song();
     Serial.println("PIN_NEXT");
     button_time=millis();
     
}
void previous_song()
{
    current_song--;
    if (current_song < 0)
      current_song = total_songs-1;
    play_song();
    Serial.println("PIN_PREV");
    button_time=millis(); 
   
}

void pause_song()
{
   Serial.println("PIN_PAUSE");
   audio.pauseResume();  
   button_time=millis();
   
}
void right()
{
     Serial.println("RNGesus take the wheeeeeel");
     rngesus = !rngesus;
     play_song();
     button_time=millis();
    
}
void up()
{   
    Serial.println("Pin_up");
    refresh_screen=true;
    if (selected_song == 0)
        selected_song = total_songs-1;
    else
        selected_song--;
    button_time=millis();
   
}
void down()
{   
    Serial.println("Pin_down");
    refresh_screen=true;
    if (selected_song == total_songs-1)
        selected_song = 0;
    else
        selected_song++;
    button_time=millis();
    screen_saver = false;

}
void middle()
{
   current_song = selected_song;
   play_song();
   button_time=millis();
  
}

void play_song()
{
  if (!rngesus)
    {
    audio.connecttoFS(SD,music_list[current_song].name.c_str());
    Serial.println("Function play_song: playing : ");
    Serial.println(music_list[current_song].name.c_str());
    }
 else
 {
    current_song = rand() % total_songs; 
    audio.connecttoFS(SD,music_list[current_song].name.c_str());
    Serial.println("Function play_song: playng random song: ");
    Serial.println(music_list[current_song].name.c_str());
 }
 song_time = millis();
}
int check_song_number(int n)
{
  if (n<0)
    return (total_songs+n);
  if (n>=total_songs)
    return (n-total_songs);
  return n;
}

void set_volume()
{
 
  volume_new=analogRead(potentiometerPin)/195;
  //Serial.println(digitalRead(potentiometerPin));
  
  if (volume_new!=volume_old)
  {
    volume_old=volume_new;
    audio.setVolume(volume_old);
    Serial.println("Function set_volume : changing volume to:");
    Serial.println(volume_old);
  }

  
}

int make_music_file(fs::FS &fs)
{
  Serial.println("make_music_file");
  if (fs.exists("Names.txt"))
      {
      Serial.println("Where the file at");
      return 1;
      }
  
  else
  {

    File to = fs.open("/Names.txt",FILE_WRITE);
    if (to)
      {
      for (int i=0;i<=total_songs;i++)
        {
          to.println(music_list[i].name);
          Serial.println(music_list[i].name);
        }
      to.close();
      }
    else
    {
      Serial.println("error creating the file");
    }
  }
  return 1;
}

int get_music_list(fs::FS &fs)
{
  Serial.printf("get_music_list");
   File root = fs.open("/");
    if (!root)
    {
        Serial.println("Failed to open directory");
        return 0;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return 0;
    }
  File file = root.openNextFile();
  while (file)
  {
    String curent_name = file.name();
    if (curent_name.endsWith(".mp3"))
      {
        music_list[total_songs].name=curent_name;
        
        Serial.println(music_list[total_songs].name);
        total_songs++;
      }
    file = root.openNextFile();
  }
  Serial.println("get_music_list done!");
  return 1;
}

void get_music_from_file()
{
  Serial.println("get_music_from_file function");
  File file;
  file=SD.open("/Names.txt",FILE_READ);
  if (file)
  {
    Serial.println("File_found, copying it");
    while(file.available())
    { 
      music_list[total_songs].name = file.readStringUntil('\n');
     total_songs++;
     // Serial.println(music_list[total_songs].name);
     
    }
   Serial.println("Finished retriving names");
    file.close();
    total_songs--;
  }
  else
      {
        Serial.println("Error in finding the file, dont know how it's possible");
      }

  
}

void display_songs()
{   
    int index_song;
    //tft.fillRect(10,30,100,100,TFT_BLACK);
    for (i=0;i<songs_shown;i++)
      {
        index_song = check_song_number(selected_song+i-3);
        
        tft.drawString(music_list[index_song].name,x_space,max_y-y_space*(songs_shown-i),1);
        Serial.println(music_list[index_song].name);
      }
   tft.setCursor(0,0);
   tft.fillRect(5,max_y-4*y_space+1,5,5,TFT_ORANGE);

}

String normalize(int n)
{
   if (n<10)
      return "0"+String(n);
    else 
      return String(n);
}

void display_song_time()
{
    int t = audio.getAudioCurrentTime();
    int m,s;
    m=t/60;
    s=t%60;
    String m1,s1;
    if (m<10)
      m1 = "0"+String(m);
    else 
      m1 = String(m);
    if (s<10)
      s1="0"+String(s);
    else 
      s1=String(s);
    tft.drawString(m1+":"+s1,5,5,4);  
}
void display_runtime()
{
  int t = millis()/1000;
  int h,m,s;
  s=t%60;
  m=t/60%60;
  h=t/360;
  tft.drawString(normalize(h)+":"+normalize(m)+":"+normalize(s),80,5,2);
  
}


void display_saver()
{
  display_song_time();
  display_runtime();
}



void audio_eof_mp3(const char *info)
{ //end of file

    current_song++;
    if (current_song >= total_songs)
    {
        current_song = 0;
    }
    play_song();
}
