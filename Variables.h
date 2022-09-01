//the settings for the potentiometer(volume)
#define potentiometerPin 13 // uses the gpio number
int volume_old=0,volume_new=0;



//Pins for buttons  12, 14, 33, 32, 35, 34 ,39, 21

#define pin_up 33
#define pin_down 32
#define pin_left 12
#define pin_right 14
#define pin_middle 35
#define pin_next 39
#define pin_prev 21
#define pin_pause 34
//Maximum number of generations until screen is refreshed
#define MAX_GEN_COUNT 500
// 2x2 pixel cells, for smaller array size 
#define GRIDX 160
#define GRIDY 128
#define CELLXY 1
#define max_x 160
#define max_y 128
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

 
//data for the micro sd card
#define SD_CS          23
#define SPI_MOSI      19
#define SPI_MISO      18
#define SPI_SCK       22


// I2S Connections
#define I2S_DOUT      25
#define I2S_BCLK      26
#define I2S_LRC       27
//Audio object


struct Music_list
{
  String name;
  int length;
} music_list[400];

int total_songs=0;
int current_song=0;
int button_time=0;
int button_repetition_time=70;
bool paused=0; 
bool rngesus = false;
bool refresh_screen=true;
int selected_song = 0;
int songs_shown = 7;
int song_time = 0;
int last_updated = 0;
int x_space=15;
int y_space=13;
bool screen_saver = 1;
int i;
