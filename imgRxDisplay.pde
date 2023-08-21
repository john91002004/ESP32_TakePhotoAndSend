// Import the library
import processing.serial.*;
import java.util.concurrent.atomic.* ;
import java.nio.file.*; 
 
Serial port;

AtomicInteger lock = new AtomicInteger(0); 

static int BUF_LEN = 1; 
static int BIGBUF_SIZE = 100000; 
static String ENDER_TXT = "$ENDER_TXT$"; 
static String HEADER_TXT = "$HEADER_TXT$"; 

byte[] byteBuffer = new byte[BUF_LEN];
byte[] BigBuffer = new byte[BIGBUF_SIZE]; 
int header_index; 
int ender_index; 
int index = 0; 

static final int DISPLAY_STATE = 0; 
static final int WAIT_HDR_STATE = 1;
static final int WAIT_END_STATE = 2; 
int state = WAIT_HDR_STATE;  


boolean isReceiving = false;
PFont orcFont;
String msg = "";
OutputStream output;

void setup() {
  size(320, 330);  // set window size. this cannot be used outside setup() func
  drawSitename(); 
  openPort("COM7"); 
}

void serialEvent(Serial port) {
  port.readBytes(byteBuffer);
  // ---
  while (!lock.compareAndSet(0, 1));   // if someone hold lock, wait
  // Critical Section
  for (int k = 0; k < BUF_LEN; k++) {
    BigBuffer[k+index] = byteBuffer[k];     
  }
  index += BUF_LEN;
  if (index > BIGBUF_SIZE) {
    index = 0; 
    println("[ERROR] index is over the BigBuffer length %d", BIGBUF_SIZE);
    exit(); 
  }
  // Critical Section
  lock.set(0);     // release lock to 0 
  // ---
}
 
void draw() {
  delay(1000); 
  
  switch (state) {    // state machine 
    case WAIT_HDR_STATE:
      while (lock.compareAndSet(0, 1));  // wait for lock  
      // Critical section 
      header_index = getHeaderIndex(); 
      if (header_index != -1) {    // header is some where 
        println("[INFO] draw(): HEADER_TXT found."); 
        alignHeaderToTop();
        updateState(WAIT_END_STATE);  // update state         
      }
      else {
        clearBufferUntilHeader(); // clear buffer
      }
      // Critical section 
      lock.set(0); 
      break; 
    
    case WAIT_END_STATE:
      while (lock.compareAndSet(0, 1));  // wait for lock  
      // Critical section 
      ender_index = getEnderIndex();  
      if (ender_index != -1) {
        println("[INFO] draw(): ENDER_TXT found."); 
        // 將header和ender中間的資料存成byteArray
        byte[] data = getValidDataFromBuffer();  
        saveImg(data); 
        updateState(DISPLAY_STATE); 
      }
      // Critical section 
      lock.set(0); 
      break;
      
    case DISPLAY_STATE:
      println("[INFO] draw(): start to display image.");
      displayImg(); 
      
      while (lock.compareAndSet(0, 1));  // wait for lock  
      clearBigBuffer(); 
      lock.set(0); 
      
      updateState(WAIT_HDR_STATE); 
      break;
    
    default: 
      println("[ERROR] You should not reach here");
      break;
  }
}

void displayImg() {
  PImage img;
  img = loadImage("rx_img.jpg"); 
  image(img, 0, 0);
}

void saveImg(byte[] arr) {
   saveBytes("rx_img.jpg", arr); 
}

void updateState(int new_state) {
  state = new_state; 
  print("[INFO] State changed to ");
  if (state == WAIT_HDR_STATE) {
    println("WAIT_HDR_STATE");
  }
  else if (state == WAIT_END_STATE) {
    println("WAIT_END_STATE");
  }
  else if (state == DISPLAY_STATE) {
    println("DISPLAY_STATE");
  }
}
  
int getHeaderIndex() {
  String s = new String(BigBuffer, 0, index);
  int hdr_index = s.indexOf(HEADER_TXT); 
  
  // we shouldn't convert char into String, we should just search char array for target string instead.

  if (hdr_index != -1) {
    println("[INFO] getHeaderIndex(): header index: ", hdr_index);
  }
  return hdr_index; 
}

int getEnderIndex() {
  int substr_len = 15;      // this shall be ENDER_TXT.length(), but I giva it more length in case.
  String s = new String(BigBuffer, index - substr_len, index);    
  int ender_index_in_s = s.indexOf(ENDER_TXT); 
  
  // we shouldn't convert char into String, we should just search char array for target string instead.
  
  if (ender_index_in_s == -1) {
    return -1;
  }
  else {    // $123456789$ --> ender_index_in_s = 3 if 50 => 4
    return index - substr_len + ender_index_in_s; 
  }
}

boolean isSubOfBigBuffer(char[] ch_arr, int ch_arr_len) {
  int i = 0; 
  for (byte by: BigBuffer) {
    char ch = (char) by; 
    if (ch != ch_arr[i])   { i = 0; }
    else if (++i == ch_arr_len)   { return true; }
  }
  return false; 
}

void alignHeaderToTop() {
  arrayCopy(BigBuffer, header_index, BigBuffer, 0, index - header_index);
  index = index - header_index;
  
  println("[INFO] alignHeaderToTop(): align header to top:");
  printByteArray(BigBuffer, index); 
}
  
byte[] getValidDataFromBuffer() {
  int data_len = ender_index - HEADER_TXT.length(); 
  byte[] valid_data = new byte[data_len]; 
  arrayCopy(BigBuffer, HEADER_TXT.length(), valid_data, 0, data_len);
  
  println("[INFO] ender_index = ", ender_index); 
  print("[INFO] getValidDataFromBuffer(): valid_data length = ");
  println(data_len);
  
  return valid_data; 
}

void clearBigBuffer() {
  index = 0; 
  println("[INFO] Reset index = 0");
} 

void clearBufferUntilHeader() {
  if (index >= HEADER_TXT.length()) {
    arrayCopy(BigBuffer, index - (HEADER_TXT.length() - 1), BigBuffer, 0, (HEADER_TXT.length() - 1));
    index = HEADER_TXT.length() - 1;
    
    print("[INFO] clearBufferUntilHeader(): remainging string is ");
    printByteArray(BigBuffer, index); 
  }
}

void printByteArray(byte[] b, int count) {
  for (int i=0; i<count; i++) {
    print((char)b[i]); 
  }
}

void drawSitename() {
  orcFont = createFont("微軟正黑體", 36);
  textFont(orcFont);
  fill(255,255,0);
  rect(0,240,319,89); 
  fill(0);
  text("衛星接收站", 60, 240, 270, 150);
  text("A", 135, 280, 270, 150); 
}

void openPort(String portname) {
  try {
    port = new Serial(this, portname, 115200);
    port.buffer(BUF_LEN);
  } 
  catch (Exception e) {
     println(e) ;
  }
}
