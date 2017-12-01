#ifndef QUEUE_H_
#define QUEUE_H_

template <typename T, int capacity> 
class container{
private:
	struct Shell{
		bool 	isUsed ;
		T       t;
		Shell()
		{
			isUsed = false;
		}
	};

	Shell  containers[capacity];
public:
    container(){
    }

    bool putOne(T & data )
    {
    	for(int i = 0 ; i < capacity ; i ++)
    	{
    		if(! containers[i].isUsed   )
    		{

    		}
    	}
    	return false;
    }


};    


#endif
