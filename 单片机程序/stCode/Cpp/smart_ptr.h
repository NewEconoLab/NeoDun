#ifndef SMART_PTR_H_
#define QUEUE_H_

template <typename T>  
class smart_ptr{  
public:  
    //add a default constructor  
    smart_ptr();  
    //  
    smart_ptr(T* p);  
    ~smart_ptr();  
    T& operator*();  
    T* operator->();  
    //add assignment operator and copy constructor  
    smart_ptr(const smart_ptr<T>& sp);  
    smart_ptr<T>& operator=(const smart_ptr<T>& sp);  
    //  
private:  
    T* ptr;  
    //add a pointer which points to our object's referenct counter  
    int* ref_cnt;  
    //  
};  
  
template <typename T>  
smart_ptr<T>::smart_ptr():ptr(0),ref_cnt(0){  
    //create a ref_cnt here though we don't have any object to point to  
    ref_cnt = new int(0);  
    (*ref_cnt)++;  
}  
  
template <typename T>  
smart_ptr<T>::smart_ptr(T* p):ptr(p){  
    //we create a reference counter in heap  
    ref_cnt = new int(0);  
    (*ref_cnt)++;  
}  
  
template <typename T>  
smart_ptr<T>::~smart_ptr(){  
    //delete only if our ref count is 0  
    if(--(*ref_cnt) == 0){  
        delete ref_cnt;  
        delete ptr;  
    }  
}  
  
template <typename T>  
T&  smart_ptr<T>::operator*(){  
    return *ptr;  
}  
  
template <typename T>  
T* smart_ptr<T>::operator->(){  
    return ptr;  
}  
  
template <typename T>  
smart_ptr<T>::smart_ptr(const smart_ptr<T>& sp):ptr(sp.ptr),ref_cnt(sp.ref_cnt){  
    (*ref_cnt)++;  
}  
  
template <typename T>  
smart_ptr<T>& smart_ptr<T>::operator=(const smart_ptr<T>& sp){  
    if(&sp != this){  
        //we shouldn't forget to handle the ref_cnt our smart_ptr previously pointed to  
        if(--(*ref_cnt) == 0){  
            delete ref_cnt;  
            delete ptr;  
        }  
        //copy the ptr and ref_cnt and increment the ref_cnt  
        ptr = sp.ptr;  
        ref_cnt = sp. ref_cnt;  
        (*ref_cnt)++;  
    }  
    return *this;  
} 


#endif
