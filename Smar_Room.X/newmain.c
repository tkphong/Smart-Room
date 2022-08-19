
// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSC_EC     // Oscillator Selection bits (EC oscillator, CLKO function on RA6 (EC))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)


#include <xc.h>
#include "p18f4550.h"
#include "string.h"
#include "stdio.h"

//define cho LCD
#define RS LATD0                  
#define EN LATD1                  
#define ldata LATD                
#define LCD_Port TRISD              

//define cho keypad
#define write_port LATB 
#define read_port PORTB  
#define Direction_Port TRISB

//define cho button
#define EN_KEY PORTAbits.RA1
#define OFF_LAMP  PORTAbits.RA2
#define ON_LAMP  PORTEbits.RE2
#define CLOSE   PORTAbits.RA3
#define OPEN    PORTAbits.RA4
#define RESET_PASS  PORTAbits.RA5
#define ON_FAN  PORTEbits.RE0
#define OFF_FAN PORTEbits.RE1

unsigned char col_loc,rowloc,temp_col;  //cac bien dung cho ham keyfind();
unsigned char keypad[4][4]= {"789/","456*","123-"," 0=+"}; //khai bao mang keypad 4x4
float value_adc=0.0; //bien chua gia tri ADC
char buffer[50]={0,}; //bien buffer de chua chuoi du lieu

//Cac ham dung cho LCD
void MSdelay(unsigned int ); //Ham delay dung NOP();  
void LCD_Init(); //Ham khoi tao LCD            
void LCD_Command(unsigned char ); //Ham gui lenh
void LCD_Char(unsigned char x); //Ham gui data 
void LCD_String(const char *); //Ham hien thi 1 chuoi len LCD
void LCD_String_xy(char, char , const char *); //Ham hien thi 1 chuoi voi toa do x(hang) va y(cot)
void LCD_Clear(); //Ham xoa sach man hinh LCD                  

unsigned char keyfind(); //Ham xac dinh phim dang duoc nhan tren keypad

void ADC_Interrupt_Init(); //Ham khoi tao va ngat ADC

float tempurture=0;//bien chua gia tri nhiet do


//Ham ngat ADC
void __interrupt  ADC_ISR(){
    value_adc=(ADRESH*256) | (ADRESL); //doc gia tri ADC chuyen doi duoc o 2 thanh ghi ADRESH và ADRESL
    tempurture = (5*(float)(value_adc)*1000.0/1024 - 500.0)/10.0; 
    ADIF=0; //xoa co ngat
}

//Hàm main
void main(void) {
    OSCCON=0x72; //Dao dong noi 8MHz
    LCD_Init();  //Khoi tao LCD
    ADC_Interrupt_Init(); //Khoi tao ADC va ngat
    RBPU=0; //Su dung dien tro pullup o PORTB
    TRISA = 0xFF; // INPUT
    PORTA=0xFF; 
    TRISC=0x00; 
    LATC=0; 
    PORTC=0; 
    TRISE=0x07;
    int count=0; //Bien dem
    int count_fail=0; //Bien chua so lan fail
    char pass[7]={0,}; //Bien chua gia tri pass moi lan nhap, 6 ky tu so va 1 ky tu NULL
    char pass_verify[7]={0,}; //Bien chua pass veryfi
    char pass_cmp[7]="123457"; //Bien chua mat khau hien tai
    unsigned char key=0; //bien chua ky tu tra ve cua ham keyfind();
    while(1){
        ADCON0bits.GO = 1; //Chuyen doi ADC
        
        //Phan nhap pass de mo cua
        if(EN_KEY==0){ //Neu nhan phim EN_KEY de nhap pass
            LCD_Clear();  //Xoa sach man hinh LCD 
            LCD_Command(0x80); //Chi dinh chuan bi hien thi o vi tri dau dong hang 1 cua LCD
            LCD_String("Enter pass!!             ");//Hien thi tien trinh la dang nhap pass
            LCD_Command(0xC0); //Chi dinh chuan bi hien thi o vi tri dau dong hang 2 cua LCD
            while(count<6){ //Nhap du 6 ky tu
                key=keyfind(); //doc ky tu dang nhan
                if(key>0){ //Neu key >0, khong phai la ky tu rac
                    pass[count]=key; //gan vao mang
                    LCD_Char('*'); //hien thi ky tu an danh de biet da nhap bao nhieu ky tu
                    count++; //tang bien dem
                }  
            }
            count=0; //reset lai bien dem de su dung cho lan tiep theo
            pass[6]='\0'; //ky tu cuoi cung cua mot chuoi phai la ky tu NULL
            if(!strcmp(pass,pass_cmp)){ //Neu pass vua nhap va pass hien tai giong nhau
                count_fail=0; //reset bien dem fail
                LATCbits.LATC2=1; // bat den
                LCD_Command(0xC0); 
                LCD_String("Successful               "); //bao la da nhap pass thanh cong 
                LCD_Command(0x80);
                sprintf(buffer,"Temperature=%0.1f",tempurture); //add vao buffer
                LCD_String(buffer); //Hien thi len LCD
                if(tempurture>29.0){  //Neu nhiet do hien tai > 29 do C, doan nay chi lam khi mo cua
                    LATCbits.LATC6=1; //bat quat
                    LCD_Command(0xC0);
                    LCD_String("Fan turning on          "); //Hien thi la quat dang duoc bat
                }
                else{ //Khong thi tat quat va hien thi la quat dang tat
                    LATCbits.LATC6=0;
                    LCD_Command(0xC0);
                    LCD_String("Fan turning off          ");
                }
                MSdelay(3000);
                LATCbits.LATC0=1; //mo cua 
                LATCbits.LATC1=0;
                MSdelay(5000);  //cho cua mo xong
                LATCbits.LATC0=0; //dung
                LATCbits.LATC1=0;
            }
            else{ //Neu nhap sai pass 
                count_fail++; // Tang gia tri bien dem fail
                if(count_fail>2){ // neu da sai qua 3 lan
                    LCD_Command(0xC0);
                    LCD_String("Out of move !!!            "); //hien thi la da het luot
                    MSdelay(2000);
                    LATCbits.LATC7=1; //bat bao dong
                    MSdelay(3000);
                    LATCbits.LATC7=0;
                }
                else{//Neu khong thi chi hien thi fail
                LCD_Command(0xC0);
                LCD_String("Failed               ");
                MSdelay(2000);
                }
            }
        }
        
     
        //Cac thao tac tu phia ben trong nha
        if(OFF_LAMP==0) LATCbits.LATC2=0; //Tat den
        if(ON_LAMP==0)  LATCbits.LATC2=1;
        if(CLOSE==0){ //Dong cua
            LATCbits.LATC0=0; // Cho dong co quay nguoc chieu voi luc mo cua
            LATCbits.LATC1=1;
            MSdelay(5000); 
            LATCbits.LATC0=0; //Stop
            LATCbits.LATC1=0;
        }
        if(OPEN==0){ //Neu muon mo cua de ra ngoai
            LATCbits.LATC0=1; // mo cua 
            LATCbits.LATC1=0;
            MSdelay(5000);
            LATCbits.LATC0=0; //Stop
            LATCbits.LATC1=0;
        }
        if(ON_FAN==0){//tat quat
            LATCbits.LATC6=1;
        }
        if(OFF_FAN==0){//mo quat
            LATCbits.LATC6=0;
        }
        
        
        //Phan thay doi pass
        if(RESET_PASS==0){ //Neu muon thay doi pass
            LCD_Clear();
            LCD_Command(0x80);
            LCD_String("En_pass present     "); //Hien thi yeu cau nhap pass hien tai
            LCD_Command(0xC0);
            while(count<6){  //Phan nhap pass giong nhu phan tren
                key=keyfind();
                if(key>0){
                    pass[count]=key;
                    LCD_Char('*'); 
                    count++;
                }  
            }
            count=0;
            pass[6]='\0';
            LCD_Clear(); 
            if(!strcmp(pass,pass_cmp)){ //Neu pass nhap vao va pass hien tai trung khop
                LCD_Command(0x80);
                LCD_String("Enter new pass               "); //Hien thi giai doan nhap pass moi
                LCD_Command(0xC0);
                while(count<6){  //Nhap pass nhu truoc
                    key=keyfind();
                    if(key>0){
                        pass_cmp[count]=key;
                        LCD_Char('*'); 
                        count++;
                    }  
                }
                count=0;
                pass_cmp[6]='\0';
                LCD_Clear();
                LCD_Command(0x80);
                LCD_String("Verify new pass               ");  //Hien thi giai doan nhap pass xac thuc
                LCD_Command(0xC0);
                while(count<6){  //Nhap pass xac thuc
                    key=keyfind();
                    if(key>0){
                        pass_verify[count]=key;
                        LCD_Char('*'); 
                        count++;
                    }  
                }   
                count=0;
                pass_verify[6]='\0';
                if(!strcmp(pass_verify,pass_cmp)){ //Neu pass moi va pass xac thuc trung khop
                    LCD_Clear();
                    LCD_Command(0x80);
                    LCD_String("Change pass OK     "); //Hien thi la da thay doi pass thanh cong
                    MSdelay(2000);  
                }
                else{  //Neu khong thi thong bao nhap pass fail  
                    LCD_Clear();
                    LCD_Command(0x80);
                    LCD_String("Change pass FAILED     ");
                    MSdelay(1500);
                    LCD_Clear();
                    LCD_Command(0x80);
                    LCD_String("Press RESET_PASS");    
                    LCD_Command(0xC0);
                    LCD_String("to rechange pass    ");  
                    MSdelay(3000);
                }
            } 
            else{  //Neu nhap pass ban dau khong dung thi cung yeu cau nhan RESET_PASS neu muon thay doi pass lan nua
                LCD_Clear();
                LCD_Command(0x80);
                LCD_String("Press RESET_PASS");    
                LCD_Command(0xC0);
                LCD_String("to rechange pass    "); 
                MSdelay(3000);
            }
        }
        
        
        LCD_Command(0x80);
        LCD_String("WELCOME TO HOME!");
        LCD_Command(0xC0);
        LCD_String(" PRESS EN_KEY!! ");
        count=0;
        memset(pass,0,6); //clear
        memset(buffer,0,50); //clear
    }
    return ;
}
//Ham khoi tao ADC va ngat ADC
void ADC_Interrupt_Init(){		
    ADCON0 = 0; //Channel 0
    ADCON1 = 0x0e; //dien ap tham chieu la VDD/VCC va VSS/GND
    ADCON2 = 0x92; //ket qua dich phai, 8TAD, FOSC/32
    ADRESH = 0; // reset 2 thanh ghi chua du lieu
    ADRESL = 0;
    PIR1bits.ADIF=0; //xoa co ngat
    PIE1bits.ADIE=1; //cho phep ngat ADC xay ra

    INTCONbits.PEIE=1; //cho phep ngat ngoai vi
    INTCONbits.GIE=1; //cho phep ngat toan cuc
    ADCON0bits.ADON = 1; //cho phep chuyen doi ADC
    ADCON0bits.GO = 1; //tien trinh chuyen doi
}
//Ham khoi tao LCD
void LCD_Init()
{
    LCD_Port = 0;  //output             
    MSdelay(15);  //cho LCD power ON                   
    LCD_Command(0x02); // mode 4bit         
    LCD_Command(0x28); //hien thi tren ca 2 hang        
	LCD_Command(0x01); //clear LCD            
    LCD_Command(0x0c); //tat hien thi con tro         
	LCD_Command(0x06); //dich con tro ve ben phai, ghi ve ben phai           	   
}

//Ham gui di 1 lenh 
void LCD_Command(unsigned char cmd )
{
	ldata = (ldata & 0x0f) |(0xF0 & cmd);//lay 4 bit cao cua cmd 
	RS = 0; //=0: send command                                
	EN = 1; //tao xung o chan EN de push du lieu di                              
	NOP();
	EN = 0;
	MSdelay(1);//delay 1ms
    ldata = (ldata & 0x0f) | (cmd<<4);  //lay 4 bit thap cua cmd    
	EN = 1; //tao xung o chan EN de push du lieu di   
	NOP();
	EN = 0;
	MSdelay(3);//delay 3ms
}

//Ham gui di 1 ky tu
void LCD_Char(unsigned char dat)
{
	ldata = (ldata & 0x0f) | (0xF0 & dat); //lay 4 bit cao cua dat
	RS = 1; //RS=1, send data                               
	EN = 1; //tao xung o chan EN de push du lieu di                                    
	NOP();
	EN = 0;
	MSdelay(1);
    ldata = (ldata & 0x0f) | (dat<<4);  ////lay 4 bit thap cua dat            
	EN = 1; //tao xung o chan EN de push du lieu di                        
	NOP();
	EN = 0;
	MSdelay(3);//delay 3ms
}

//Ham gui di 1 chuoi data
void LCD_String(const char *msg)
{
	while((*msg)!=0) //gui cho den khi het chuoi
	{		
	  LCD_Char(*msg);
	  msg++;	
    }
}

//Ham xoa man hinh LCD
void LCD_Clear()
{
   	LCD_Command(0x01);     
}

//Ham delay ms
void MSdelay(unsigned int val) //FOSC=8MHz
{
 unsigned int i,j;
 for(i=0;i<=val;i++)
     for(j=0;j<165;j++);            
 }


//Ham xac dinh ky tu dang duoc nhan tren KeyPad
unsigned char keyfind()
{      
    Direction_Port = 0xf0; //PB7:4 la input, PB3:0 la output    //TRISD=0xF0      
     
    //Do xem cot va hang nao dang duoc tac dong
    while(1)
    {
        write_port  = 0xfe; //Bat dau voi hang thu 0       
        col_loc = read_port & 0xf0; //gia tri col_loc se cho biet cot nao dang duoc tac dong     
        temp_col=col_loc; //gan tam qua bien khac
        if(col_loc!=0xf0) //Neu gia tri col_loc khac 0xF0 thi chac chan chi co the la hang thu 0 dang duoc tac dong
        {
            rowloc=0;   //hang dang duoc tac dong la 0                    
            while(temp_col!=0xf0) //cho cho den khi nha phim        
            {
                temp_col = read_port & 0xf0;   
            }
            break;//thoat khoi while(1) vi da xac dinh duoc 
        }
        
        write_port = 0xfd; //Tiep tuc voi hang thu 1                
        col_loc = read_port & 0xf0;        
        temp_col=col_loc;
        if(col_loc!=0xf0)
        {
            rowloc=1;                       
            while(temp_col!=0xf0)           
            {
                temp_col = read_port & 0xf0;
            }
            break;
        }
        
        write_port = 0xfb; //Tiep tuc voi hang thu 2                
        col_loc = read_port & 0xf0;        
        temp_col=col_loc;
        if(col_loc!=0xf0)
        {
            rowloc=2;                      
            while(temp_col!=0xf0)          
            {
                temp_col = read_port & 0xf0;
            }
            break;
        }
        
        write_port = 0xf7; //Cuoi cung la hang thu 3               
        col_loc = read_port & 0xf0;      
        temp_col=col_loc;
        if(col_loc!=0xf0)
        {
            rowloc=3;                     
            while(temp_col!=0xf0)          
            {
                temp_col = read_port & 0xf0;
            }
            break;
        }
        
    }

    //Sau khi da biet duoc hang nao dang tac dong va gia tri cua col_loc, ta hoan toan xac dinh duoc phim dang duoc nhan
    while(1)
    {  
        if(col_loc==0xe0)//Cot 0
        {
             return keypad[rowloc][0];             
        }
        else 
            if(col_loc==0xd0)//Cot 1
        {
            return keypad[rowloc][1];     
        }
        else
            if(col_loc==0xb0)//Cot 2
        {
            return keypad[rowloc][2];   
        }
        else    
            if(col_loc==0x70)//Cot 3
        {
            return keypad[rowloc][3];     
        }    
    }
    
   MSdelay(100);     
}