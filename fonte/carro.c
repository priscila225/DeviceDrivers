#include <stdio.h>
char bits[2];
void motor(){
	FILE *file;
	file = fopen("/dev/gpio0", "w");
    	putc(bits[0],file);
    	fclose(file);
	file = fopen("/dev/gpio1", "w");
    	putc(bits[1],file);
    	fclose(file);
}
void direcao(){
	FILE *file;
	file = fopen("/dev/gpio2", "w");
    	putc(bits[0],file);
    	fclose(file);
	file = fopen("/dev/gpio3", "w");
    	putc(bits[1],file);
    	fclose(file);
}

int main(void) {

    char x;
    
    while(1){   
	x = getch();
	
	if(x == 'w')
	{
		bits[0] = '0';
		bits[1] = '1';
		motor();
	}
	else if(x == 's')
	{
		bits[0] = '1';
		bits[1] = '0';
		motor();
	}
	else if(x == 'a')
	{
		bits[0] = '0';
		bits[1] = '1';
		direcao();
	}
	else if(x == 'd')
	{
		bits[0] = '1';
		bits[1] = '0';
		direcao();
	}    		
    
    }
    return 0;
}
