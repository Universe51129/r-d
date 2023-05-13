#ifndef SimpleParser_H
#define SimpleParser_H
#include "OSHeaders.h"
class SimpleString {
    
    public:         
            SInt32 fLen;
            char *fTheString;
            
            SimpleString(char *theString = NULL);
            void Init();
            void SetString(char *theString, SInt32 len);
            SInt32 GetString(char *theString, SInt32 len);
            SInt32 GetInt();
            void Print();
};

class SimpleParser {


    
    protected:
            SimpleString fSource;
    public:
    
             SimpleParser() : fSource(NULL) {} ;
             SimpleParser(SimpleString *source) : fSource(*source) {} ;
        ~SimpleParser() {};
        
    static char sWordDelimeters[];
    static char sLineDelimeters[];

    void SetSource(SimpleString *source) {fSource = *source;};  
        
    static int CountDelimeters( SimpleString *source, char *delimeters);
    
    static bool Compare(SimpleString *str1, SimpleString*str2, bool caseSensitive);     
    
    bool Compare(SimpleString *str1Ptr, char *str, bool caseSensitive)
    {
        if (str == NULL) return false;
        SimpleString string(str);
        return Compare(str1Ptr, &string, caseSensitive);
    }
    
    static bool FindString( SimpleString *source,  SimpleString *find, SimpleString *resultString);
    
    static bool FindNextString( SimpleString *sourcePtr,  SimpleString *currentPtr,  SimpleString *findPtr, SimpleString *resultStringPtr);

    static bool GetString( SimpleString *source,  SimpleString *find, SimpleString *resultString);
                    
    static bool FindDelimeter( SimpleString *source, char *delimeter, SimpleString *resultString);
    
    static bool GetLine( SimpleString *sourcePtr, SimpleString *resultStringPtr);               
    
    static bool GetWord( SimpleString *sourcePtr, SimpleString *resultStringPtr);
    
    static bool GetNextThing( SimpleString *sourcePtr, SimpleString *currentPtr, char *findChars, SimpleString *resultStringPtr);

    static bool GetNextLine( SimpleString *sourcePtr, SimpleString *currentLine, SimpleString *resultStringPtr);

    static bool GetNextWord( SimpleString *sourcePtr, SimpleString *currentWord, SimpleString *resultStringPtr);
};



#endif
