#define       command_en_high             1       // rs = 0, en = 1
#define       command_en_low              0       // rs = 0, en = 0
#define       data_en_high                3       // rs = 1, en = 1
#define       data_en_low                 2       // rs = 1, en = 0
#define       clear_display               0x01
#define       second_row                  0xC0
#define       display_on_cursor_off       0x0C
#define       two_line_n_5_7_matrix       0x38


volatile char *dirf, *outf, *dirk, *outk, *dira, *outa, *dirc, *inc;
volatile long number, num1, num2, result, temp;
volatile char action, resultFlag;   // 'action' is the operation type variable





/* ------------------------ NUMBER CALCULATION FUNCTION -------------------------------- */

void calculateNumber(volatile char key){
  
  if((key == '+') || (key == '-') || (key == '*') || (key == '/')){
                          /* If there are present result in current status */
    if(result){
      num1 = result;
      number = result = num2 = 0;
      action = key;
      }
                       /* If there are present number, no result are there */
    else {
      num1 = number;
      number = 0;
      action = key;
      }
    }
                                     /* when '=' is pressed in keyboard */
  else if(key == '='){
    num2 = number;
    number = 0;
    resultFlag = 1;
    calculateResult();
    }
                                    /* When clear is pressed in keyboard */
  else if(key == 'c'){
    number = num1 = num2 = result = action = 0;
    }
  else{
    if(number == 0) number = key - '0';          // to constract the number
    else number = (number*10) + (key - '0');     // to constract the number
    }
    delay1(1500);                                // key debouncing delay
}





/* ----------------------------- RESULT CALCULATION FUNCTION ----------------------------- */

void calculateResult(){
  /* Two number1 and number2 operations */
       if(action == '+') result = num1 + num2;
  else if(action == '-') result = num1 - num2;
  else if(action == '*') result = num1 * num2;
  else if(action == '/') result = num1 / num2;
}






/* ------------------------------- NUMBER PRINT FUNCION ------------------------------------ */

void lcd_print(volatile long printNumber){
  volatile char i = 0, j, num[20], revNum[20], length = 0;

 while(printNumber){
   revNum[i++] = printNumber%10;                    // the reverse number
   printNumber /= 10;
   length++;
  }
  length--;                                        // number length

 for(i=0, j=length; i<=length; i++, j--){          // the actual number
  num[i] = revNum[j] + 48;
 }

  for(i=0; i<=length; i++){                        // print the number
    *outf = num[i];
    *outk = data_en_low;        delay1(1);
    *outk = data_en_high;       delay1(1);
    *outk = data_en_low;        delay1(1);
  }
}





/* --------------------------- CHARACTER PRINT FUNCTION -------------------------------- */

void lcd_print_char(volatile char ptr){
  *outf = ptr;
  *outk = data_en_low;          delay1(1);
  *outk = data_en_high;         delay1(1);
  *outk = data_en_low;          delay1(1);
}





/* -------------------------- KEYBOARD SCANNING FUNCTION ------------------------------- */

volatile char checkKey(){
   volatile char row, col, key;
   for(row = 0; row < 4; row++){
    *outa = 1 << row;  
    if(*inc){ col = *inc; break;}
} 

  if(row == 0){
         if(col == 1) key = '1';
    else if(col == 2) key = '2';
    else if(col == 4) key = '3';
    else if(col == 8) key = '+';
  }
  else if(row == 1){
         if(col == 1) key = '4';
    else if(col == 2) key = '5';
    else if(col == 4) key = '6';
    else if(col == 8) key = '-';
  }
  else if(row==2){
         if(col == 1) key = '7';
    else if(col == 2) key = '8';
    else if(col == 4) key = '9';
    else if(col == 8) key = '*';
  }
  else if(row == 3){
         if(col == 1) key = 'c';
    else if(col == 2) key = '0';
    else if(col == 4) key = '=';
    else if(col == 8) key = '/';
  }
  else key = 0;
  return key;
}





////////////////////////////////  DISPLAY AND PORT FUNCTIONS  /////////////////////////////

void init_lcd(){
  lcd_cmd(two_line_n_5_7_matrix);
  lcd_cmd(display_on_cursor_off);
  lcd_cmd(clear_display);
}

void lcd_cmd(volatile char cmd){
  *outf = cmd;   
  *outk = command_en_low;     delay1(1); 
  *outk = command_en_high;    delay1(1);    
  *outk = command_en_low;     delay1(1);
}

void init_port(){
  dira = 0x21;    *dira = 0xff;    outa = 0x22;
  dirc = 0x27;    *dirc = 0;       inc = 0x26;
  dirf = 0x30;    *dirf = 0xff;    outf = 0x31;
  dirk = 0x107;   *dirk = 0x03;    outk = 0x108;
}

void delay1(volatile int time){
  volatile long i;
  while(time--)
  for(i=0; i<100; i++);
}






////////////////////////////////////// MAIN FUNCION ////////////////////////////////////////////////////

int main() {
  Serial.begin(9600);
  Serial.println("         key A = '+'          key B = '-'          key C = '*'");
  Serial.println("         key D = '/'          key # = '='          key * = 'Clear'");
  Serial.println("It performs only two number operation at a time. To do multiple operations, perform operation between result and another number:" );
  Serial.println("   Ex-   1st stage: 3+5=8" );
  Serial.println("         2nd stage: *5=40" );
  Serial.println("         3rd stage: -25=15 and so on....." );
  
  volatile char key;
  init_port();
  init_lcd();
 
while(1){
  key = checkKey();
   if(key){
    calculateNumber(key);
    lcd_cmd(clear_display);            // to clear the screen



    if(num1 > 0) lcd_print(num1);      /* ---- For printing the number1 ---- */
    else if(num1 < 0){ 
      lcd_print_char('-');
      temp = num1 - 1;
      temp = ~temp;                     // for -ve numbers
      lcd_print(temp);
      temp = 0;
      }
     if(action) lcd_print_char(action); /* ---- For printing the operation ---- */
     if(num2) lcd_print(num2);          /* ---- For printing the number2 ---- */
              lcd_print(number);



    /* For printing the equal sign (=) and result */
     if(resultFlag){
      lcd_cmd(second_row);
      lcd_print_char('=');
      
      if(result > 0) lcd_print(result);
      else if(result < 0){                       // For printing -ve results
        lcd_print_char('-');
        temp = result-1;
        temp = ~temp;
        lcd_print(temp);
        temp = 0;
        }
      else {
        lcd_print_char('0');                 // For printing the zero (0) result
        result = num1 = num2 = 0;
        }
      resultFlag = 0;
     }
   }
 }
}


