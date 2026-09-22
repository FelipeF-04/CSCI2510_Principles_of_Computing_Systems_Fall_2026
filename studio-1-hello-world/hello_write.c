//Felipe Fung//09-22//ways to print in C
#include <unistd.h>

int main(int argc, char* argv[]){
	write(STDOUT_FILENO, "Testing write()\n",1000);
	return 0;
}
