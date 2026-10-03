#include <stdio.h>
#include <stdlib.h>

void printReverse( char* string ){
    int length = 0;
    while (string[length] != '\0'){
        length++;
    }    
    //printf("%i\n", length);
    for (int i = length; i >= 0;i--){
        printf("%c\n", string[i]);
    }
}

#include <stdlib.h>

char* reverseString( char* input ){
    int length = 0;
    while (input[length] != '\0'){
        length++;
    } 
    //printf("%i\n",length);
    char* output = (char*)malloc( length+1 );
    
    for (int i = 0; i < length; i++){
        output[i] = input[length-1-i];
    }
    output[length] = '\0';

    return output; 
}




int main() {
    char *messagePtr = "HELLOWORLD!";
    //3
    //printf("%s\n", messagePtr);
    
    //4
    //for (int i=0; i<11;i++){
    //    printf("%c\n", messagePtr[i]);
    //}   

    //5
    //printf("%c\n", *messagePtr);
    
    //6
    //printf("%c\n",*(messagePtr + 4));

    //7
    //for (int i = 0; i < 11; i++){
    //    printf("%c\n",*(messagePtr + i));
    //}

    //8

    //int i = 0;
    //while (messagePtr[i] != '\0'){
    //    printf("%c\n", messagePtr[i]);
    //    i++;
    //}

    //9
    //printReverse(messagePtr);

    //10
    char* reversedMessage = reverseString( messagePtr );
    printf("Reversed string: %s\n", reversedMessage);
    return 0;
}