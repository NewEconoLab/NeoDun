#ifndef QUEUE_H_
#define QUEUE_H_

template <typename T, int capacity> 
class queue
{
private:
    T  containers[capacity];
    int r_idx;
    int w_idx;
    int cnt;
public:
    queue<T,capacity> (){
        r_idx = w_idx = cnt = 0;
    }

    void add(T & t)
    {
        containers[w_idx ++ % capacity ] = t;
        cnt++;
    }
    
    int getContentSize()
    {
        return cnt;
    }
    
    int getCapacity()
    {
        return capacity;
    }
    
    T take()
    {
        T t = containers[r_idx % capacity ];
        r_idx ++;
        if(w_idx == r_idx )
        {
            w_idx = r_idx = 0;
        }
        cnt --;
        return t;
    }
    
    T & get(int index)
    {
				return	containers[r_idx + index ];
    }

    T * getPtr()
    {
        return containers;
    }
};    


#endif
