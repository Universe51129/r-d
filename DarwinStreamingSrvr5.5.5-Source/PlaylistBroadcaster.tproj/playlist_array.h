#ifndef playlist_array_H
#define playlist_array_H

template <class T> class ArrayList
{

    public:
        T *fDataArray;
        short   fSize;
        short   fCurrent;

        ArrayList()
        {   fDataArray = NULL;
            fSize = 0;
            fCurrent = 0;
        }
        
        ~ArrayList() 
        {   delete [] fDataArray;
        };

         T *SetSize(short size) 
         {  if (fDataArray != NULL) delete [] fDataArray; 
            fDataArray = new T[size]; 
            if (fDataArray) 
            {   fSize = size; 
            }
            
            return fDataArray;
         }

         T *Get()
        {
            T *resultPtr = NULL;    
            if (fDataArray != NULL)
            {
                if ( (fCurrent >= 0) && (fCurrent < fSize )  )
                {   resultPtr = &fDataArray[fCurrent];
                }
            }
            return  resultPtr;
        }

         T *OffsetPos(short offset)
        {
            T *resultPtr = NULL;    
            
            do
            {
                if (fDataArray == NULL) break;
                short temp = fCurrent + offset;

                if (temp < 0)
                {
                    fCurrent = 0; // peg at begin and return NULL
                    break;
                }
                
                if (temp < fSize) 
                {       
                    resultPtr = &fDataArray[temp];
                    fCurrent = temp;
                    break;
                }   
                    
                fCurrent = fSize -1; // peg at end and return NULL
            } while (false);
            
            return  resultPtr;
        }

         T *SetPos(short current)
        {
            T *resultPtr = NULL;    

            do
            {
                if (fDataArray == NULL) break;
                
                if (current < 0)
                {
                    fCurrent = 0; // peg at begin and return NULL
                    break;
                }
                
                if (current < fSize) 
                {
                    resultPtr = &fDataArray[current];
                    fCurrent = current;
                    break;
                }
                    
                fCurrent = fSize -1; // peg at end and return NULL
                
            } while (false);

            return  resultPtr;
        }

         short GetPos() { return fCurrent; };
         T *Next()      { return OffsetPos(1); };
         T *Begin()     { return SetPos(0); };
         short Size()   { return fSize;};
};


#endif

