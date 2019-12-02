extern "C" int kmain();
__declspec(naked) void startup()
{
	__asm 
	{
		call kmain;
	}
}
const char* hello = "Welcome to ConvertOS!";
const char*name="Convert OS: v.01";
const char*compiller="Compillers : bootloader : fasm, kernel : Microsoft C compiller";
char clear_str[80]="                                                                 ";
short color=0;
int line_count=0;
int color_checker;
char command[20]={0};
#define BITS_COUNT 32
unsigned int bits[BITS_COUNT] = { 0 };
#define LOWER_BIT 1 
void clear_command()
{
	for(short i=0;i<30;i++)
	{
		command[i]=0;
	}
}
char keyboard[] = {0,0,'1','2','3','4','5','6','7','8','9','0', '-', '=','\b',0,'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']','\n',0,'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '<','>','+',0,'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',0,'*',0,' '} ;
#define VIDEO_BUF_PTR (0xb8000)
#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)
#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80) // Ширина текстового экрана
#define GDT_CS (0x8)
#define PIC1_PORT (0x20)
int string_count =0;
char* vb = (char*)VIDEO_BUF_PTR;
#pragma pack(push, 1) // Выравнивание членов структуры запрещено
struct idt_entry
{
  unsigned short base_lo; // Младшие биты адреса обработчика
  unsigned short segm_sel; // Селектор сегмента кода
  unsigned char always0; // Этот байт всегда 0
  unsigned char flags; // Флаги тип. Флаги: P, DPL, Типы - это
  unsigned short base_hi; // Старшие биты адреса обработчика
}
;
struct idt_ptr
{
   unsigned short limit;
   unsigned int base;
};
#pragma pack(pop)

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp; // Описатель таблицы для команды lidt

 __inline unsigned char inb (unsigned short port)
{
   unsigned char data;
   __asm 
   {
         push dx
         mov dx, port
          in al, dx
          mov data, al
          pop dx
    }
 return data;
}

 __inline void outb (unsigned short port, unsigned char data)
{
 __asm 
  { 
     push dx
     mov dx, port
     mov al, data
     out dx, al
     pop dx
   }
}
__declspec(naked) void default_intr_handler()
{
__asm 
{
   pusha
}
 __asm 
 {
    popa
    iretd
  }
}
typedef void (*intr_handler)();

void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags, intr_handler hndlr)

  {
        unsigned int hndlr_addr = (unsigned int) hndlr;
        g_idt[num].base_lo = (unsigned short) (hndlr_addr & 0xFFFF);
        g_idt[num].segm_sel = segm_sel;
        g_idt[num].always0 = 0;
        g_idt[num].flags = flags;
        g_idt[num].base_hi = (unsigned short) (hndlr_addr >> 16);
  }
void intr_init()
  
  {
     int i;
     int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
     for(i = 0; i < idt_count; i++)
     intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,default_intr_handler); // segm_sel=0x8, P=1, DPL=0, Type=Intr
}
void intr_start()
{
   int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
   g_idtp.base = (unsigned int) (&g_idt[0]);
   g_idtp.limit = (sizeof (struct idt_entry) * idt_count) - 1;
   __asm 
   {
      lidt g_idtp
  }
//__lidt(&g_idtp);
}
void intr_enable()
{
   __asm sti;
}
void intr_disable()
{
    __asm cli;
}

void out_str(int color, const char* ptr, unsigned int strnum)
 {
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 80*2 * strnum;

while (*ptr)

    {
      video_buf[0] = (unsigned char) *ptr; // Символ (код)
      video_buf[1] = color; // Цвет символа и фона
      video_buf += 2;
      ptr++;
    }
 } 
void cursor_moveto(unsigned int strnum, unsigned int pos)
{

    unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
    outb(CURSOR_PORT, 0x0F);
    outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
    outb(CURSOR_PORT, 0x0E);
    outb(CURSOR_PORT + 1, (unsigned char)( (new_pos >> 8) & 0xFF));
}

char get_char(unsigned char a)
{
	return keyboard[a];
}
void info()
{
  out_str(color, hello,string_count);
  out_str(color, name, string_count++);
  out_str(color, compiller,string_count++);
  cursor_moveto(string_count,line_count);
}
int line=0;
int count=0;

void out_symbol(char c, int str, int col)
{
	unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
	video_buf += VIDEO_WIDTH* 2 * str + 2 * col;
	video_buf[0] = (unsigned char)c;
	command[count]=(unsigned char)c;
	count++;
	video_buf[1] = color;
	cursor_moveto(string_count,line);
}
void shutdown()
{	
	__asm 
	{
	 mov dx,0x0604
	 mov ax,0x2000
	 out dx,ax    
   }
}

int strcmp(char*first,char*second,int size)
{
	for(int i=0;i<size;i++)
	{
		if (first[i]!=second[i]) {return 0;}
	}
	return 1;
}
void clear_screen()
{                                                                            
	for (int i = 0; i<=24; i++)
	{
		out_str(color,clear_str, i);
	}
}
void set_values_for_screen()
{
    string_count=0;
	line=0;
}
char number[6];
char first[2];
char last[2];
void parse_command()
{
   int i=0;
   int j=0;
   int k=0;
   int z=0;
   while(command[i]!=' ')
   {
   	  i++;
   }
   i++;
   while(command[i]!=' ')
   {
   	number[j]=command[i];
   	j++;
   	i++;
   }
   i++;
   while(command[i]!=' ')
   {
     first[k]=command[i];
     k++;
     i++;
   }
   i++;
   while(command[i]!='\0')
   {
     last[z]=command[i];
     z++;
     i++;
   }
}
char posix[18];
void parse_poxix()
{
	int i=0;
	int j=0;
	 while(command[i]!=' ')
   {
   	  i++;
   }
   i++;
   while(command[i]!='\0')
   {
   	posix[j]=command[i];
   	i++;j++;
   }
}
void clear_parse_command()
{
	for(short i=0;i<30;i++)
	{
		number[i]=0;
		first[i]=0;
		last[i]=0;
		posix[i]=0;
	}
}
int atoi(char* s)
{
    int n = 0;
    while( *s >= '0' && *s <= '9' ) 
    {
        n *= 10;
        n += *s++;
        n -= '0';
    }  return n;
}

void fun (int n,int k)
{
   int mas_int[100];
   int i=0;
   char mas[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
 for (;n!=0;)
 {
    mas_int[i]=n%k;
    n/=k;
    i++;
 }
 for (int j = i-1; j >=0; j--) 
  {
  	out_symbol(mas[mas_int[j]],string_count,line++);
  }
}
char *itoa(int number, char *destination, int base) {
  int count = 0;
  do {
    int digit = number % base;
    destination[count++] = (digit > 9) ? digit - 10 +'A' : digit + '0';
  } while ((number /= base) != 0);
  destination[count] = '\0';
  int i;
  for (i = 0; i < count / 2; ++i) {
    char symbol = destination[i];
    destination[i] = destination[count - i - 1];
    destination[count - i - 1] = symbol;
  }
  return destination;
}
char res[32];
void clear_bits()
{
	for(int i=0;i<BITS_COUNT;i++)
  {
  	res[i]=0;
  	bits[i]=0;
  }
}
short chislo;
short ish;
short kon;
bool test=true;
short testik=0;
long int pow(long int x, unsigned int n)
 {
    if (n==0)
        return 1;
    else if (n==1)
        return x;
    else if (n % 2 == 0 )
        return pow( x * x, n/2);
    else
        return pow( x * x, n /2)*x;
 }

char* strchr(const char* str, int c) {
    int i = 0;
    while (str[i] && str[i] != c) ++i;
    return c == str[i] ? (char*)str + i : 0;
}
size_t strlen (const char *str)
{
    return (*str) ? strlen(++str) + 1 : 0;
}
static const char *k_base_symbols = "0123456789fbcdefghijklmnopqrasuvwxyz";
 
size_t FromBase(const char *string, size_t base, const char *base_symbols = k_base_symbols) {
  size_t result = 0;
  size_t string_length = strlen(string);
  char res[5]={};
  for (int i = string_length - 1, counter = 0; i >= 0; --i, ++counter) {
    size_t digit = strchr(base_symbols, string[i]) - base_symbols;
    result += digit * pow(base, counter);
  }
  itoa(result,res,10);
  out_str(color,res,string_count);
  string_count++;
  return result;
}
 
char *ToBase(char *buffer, size_t number, size_t base, const char *base_symbols = k_base_symbols) {
  size_t a_number = number;
  char *symbol = buffer;
  int string_size = 0;
  do {
    *symbol = base_symbols[a_number % base];
    a_number /= base;
    ++symbol;
    ++string_size;
  } while (a_number !=0);
  *symbol = '\0';
 
  for (int i = 0; i < string_size / 2; ++i) {
    char temporary = buffer[i];
    buffer[i] = buffer[string_size - i - 1];
    buffer[string_size - i - 1] = temporary;
  }
 
  return buffer;
}
void clear_array(char*f)
{
	for(short i=0;i<5;i++)
	{
		f[i]=0;
	}
}
void sum(short *a,short *b)
{
	int c = 0;
	for (int i = 1; i < 11; ++i) 
	{
		    c = c + a[i] + b[i],
			a[i] = c % 10,
			c /= 10;
	}
}
void devide(short*a,int dev,int size,char* total,short len)
{
	    int digit = 0;
        int b[10]={0};
	    int c = 0;
	    while (digit < dev && c < size)
	    {
		  digit = digit * 10 + a[c];
		  c++;
	    }
	    int remainder;
	    for (int i = 0; i < size- c + 1; i++)
	    {
		   b[i] = digit / dev;
		   remainder = digit % dev;
		   digit = remainder * 10 + a[c + i];
	    }
	    for (int i = 0;i<len; i++)
	    {
		   total[i] = b[i] + '0';
	    }
}
int difference(short *x, short *y, short *z, int length)
{
	for (int ix = 0; ix < (length - 1); ix++) 
	{
		if (ix < (length - 1)) 
		{
			x[ix + 1]--; 
			z[ix] += 10 + x[ix]; 
		}
		else 
			z[ix] += x[ix]; 
     z[ix] -= y[ix]; 

		if (z[ix] / 10 > 0) 
		{
			z[ix + 1]++; 
			z[ix] %= 10; 
		}
	}
	return 0;
}
void convert_time(char*posix,int sec,short*second)
{
if(sec > 0)
  {
        char kek[11];
        int ost=0;
    	char total[6]={0};
    	long int year=31536000;
    	long int mon= 2628000;
    	long int day= 86400;
	    long int digit = 0;
	    clear_array(kek);
	    for (int i = 0; i <11; i++)
	    {
		   kek[i] = second[i] + '0';
	    }
	    string_count++;
	    char f[7];
	    int i=0;
	    devide(second,day,strlen(kek),total,6);
	    ost=atoi(total);
	    ost=ost%30;
	    int ar[3]={0};
	    for(int i=1;i<3;i++)
	    {
          ar[3-i-1]=ost%10;
          ost=ost/10;
	    }
	    for(int i=0;i<2;i++)
	    {
	    	f[i]=ar[i] + '0';out_symbol(f[i],string_count,line++);
	    }
        out_symbol('.',string_count,line++);
        clear_array(total);
        clear_array(f);
        i=0;
	    devide(second,mon,strlen(kek),total,2);
	    ost=atoi(total);
	    ost=ost%12;
	    itoa(ost,f,10);
	    while(f[i]!='\0')
       { out_symbol(total[i],string_count,line++);i++;}
        i=0;
        out_symbol('.',string_count,line++);
        clear_array(total);
        clear_array(f);
        i=0;
	    devide(second,year,strlen(kek),total,4);
        clear_array(f);
        long int h= (sec/3600) % 24;
        long int m = (sec/ 60) % 60;
        long int s = sec % 60;
        clear_array(f);
        while(i!=4)
        { 
        	out_symbol(total[i],string_count,line++);
        	i++;
        }
        out_symbol(' ',string_count,line++);
        clear_array(total);
        i=0;
        clear_array(f);
        itoa(h,f,10);
        while(f[i]!='\0')
        {
        	out_symbol(f[i],string_count,line++);
        	i++;
        }
        out_symbol(':',string_count,line++);
        clear_array(f);
        itoa(m,f,10);
        i=0;
         while(f[i]!='\0')
        {
        	out_symbol(f[i],string_count,line++);
        	i++;
        }
        out_symbol(':',string_count,line++);
        clear_array(f);
        itoa(s,f,10);
        i=0;
        while(f[i]!='\0')
        {
        	out_symbol(f[i],string_count,line++);
        	i++;
        }
    }
}
void check_command()
{
	if(strcmp(command,"clear",count))
	{
		string_count=0,line=1;
		cursor_moveto(string_count,line);
		clear_screen();
	}
    else  if(strcmp(command,"info",count))
	{
		string_count++;line=1;
    	cursor_moveto(string_count,line);
		info();
	}
     else  if(strcmp(command,"nsconv",6))
    {
    	char save[4];
    	parse_command();
    	chislo=atoi(number);
    	ish=atoi(first);
    	kon=atoi(last);
    	string_count++;line=1;
    	cursor_moveto(string_count,line);
        char buffer[30];
    	if(ish==10) { ToBase(buffer,chislo,kon);
    		out_str(color,"Output",string_count++);
    		out_str(color,buffer,string_count++);
    		string_count++;line=1;
            	cursor_moveto(string_count,line);
    		for(int i=0;i<30;i++)
    			{
    				buffer[i]=0;}
    				goto end;
    				 }
    	 switch(kon)
    	{
    	 	case 16:
    	 	{
    	 		out_str(color,"Output : ",string_count++);
    	 	 	fun (chislo,kon);
    	 	 	clear_bits();
    	 	 	string_count++;line=1;
            	cursor_moveto(string_count,line);
    	 	 	break;
    	 	}
        	 case 10:
         	{
         		out_str(color,"Output",string_count++);
            	FromBase(number,ish);
            	string_count++;line=1;
            	cursor_moveto(string_count,line);
            	break;
         	}
         	end:
         	clear_parse_command();
      	}	
    }
    else if(strcmp(command,"shutdown",count))
    {
    	string_count++;line=1;
    	cursor_moveto(string_count,line);
    	shutdown();
    }
    else if(strcmp(command,"posixtime",8))
    {
       short long_long_2 [11]={6,2,1,2,5,9,2,0,0,0,0}; // 31536000
       short long_long_1 [10]={0};//63236041110
       string_count++;line=1;
       parse_poxix();
       for (int i = 0; i < 10; i++)
	    {
		   long_long_1[i] = posix[i] - '0';
	    }
       int sec=atoi(posix);
       sum(long_long_2,long_long_1);
       cursor_moveto(string_count,line);
       convert_time(posix,sec,long_long_2);
    }
    else if(strcmp(command,"wintime",7))
    {
      short len=10;
      short result[18]={0};
      short val[18]={1,1,6,4,4,4,7,3,6,0,0,0,0,0,0,0,0,0 };
      parse_poxix();
      string_count++;line=1;
      short int arr[18]={18};
      for(int i=0;i<18;i++)
      {
      	arr[i]=posix[i] - '0';
      }
      difference(arr,val,result,18);
      for (int i = 0; i < 17; i++)
	  {
		 result[i] = result[i + 1];
	  }
	 char total[10];
	 short tot[11]={0};
     devide(result,10000000,18,total,len);
     int loc=atoi(total);
     for(int i=0;i<11;i++)
     {
        tot[i]=total[i] - '0';
     }
     convert_time(posix,loc,tot);
    }
    else 
    {
    	string_count++;line=1;
    	cursor_moveto(string_count,line);out_str(color,"invalid command",string_count++);
    }
}

void on_key(unsigned int scan_code)
{
    char symbol=get_char(scan_code);
    if(string_count>=24)
    {
       clear_screen();
       set_values_for_screen();
    }
	if (symbol=='\n')
	{
		check_command();
		clear_command();
		clear_parse_command();
		clear_array(posix);
		count=0;
		string_count++;
		line=1;
		out_str(color, "# ", string_count);
		cursor_moveto(string_count,line++);
		return;
	}
	if (symbol== '\b')
	{		
		unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
		video_buf += 80 * 2 * string_count + (line-1) * 2;
		video_buf[0] = '\0';
		cursor_moveto(string_count, (line-1));
		command[count]=0;
		count--;
		line--;
		return;
	}
	out_symbol(symbol,string_count, line++);

}

void keyb_process_keys()
{
  if (inb(0x64) & 0x01)
    {
      unsigned char scan_code;
      unsigned char state;
      scan_code = inb(0x60); // Считывание символа с PS/2 клавиатуры
       if (scan_code < 128) // Скан-коды выше 128 - это отпускание клавиши
       { on_key(scan_code); }
     }
 }
__declspec(naked) void keyb_handler()
{
   __asm pusha;
    keyb_process_keys();
    outb(PIC1_PORT, 0x20);

  __asm 
    {
      popa
      iretd
    }
}
void keyb_init()
 
 {
     intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
     outb(PIC1_PORT + 1, 0xFF ^ 0x02); 
 }
void set_color()
{
	if(vb[0] == '1')
	{
	    color = 1;
	}
	 else if(vb[0] == '2')
	{
	    color = 2;
	}
	else if(vb[0] == '5')
	{
	    color = 4;
	}
	else if(vb[0] == '6')
	{
	     color = 6;
	}
	else if(vb[0] == '7')
	{
	  	 color = 7;
	}
	else if(vb[0] == '8')
	{
	     color = 8;
	}  
}
extern "C" int kmain()

{
  
  set_color();
  out_str(color,hello,string_count++);
  cursor_moveto(string_count,line);
  intr_init();
  keyb_init();
  intr_start();
  intr_enable();
  while(1)
  {
     __asm hlt;
   } 
  return 0;
}
