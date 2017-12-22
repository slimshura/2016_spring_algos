#include <iostream>
#include <type_traits>
#include <cstddef>
#include <type_traits>

template <typename DataType>
class default_delete {
    public:
        constexpr default_delete() noexcept {}
        void operator() (DataType* ptr) const {
            delete ptr;
        }
};


template <typename DataType, typename DelType = default_delete<DataType> >
class unique_ptr {
    private:
        DataType* ourpointer;
        DelType   ourdeleter;
    public:
        typedef DataType element_type;	//The type of the managed object
        typedef DelType  deleter_type;	//The type of the stored deleter.
        typedef DataType*     pointer;  //the pointer to element_type


        constexpr unique_ptr() noexcept :
                    ourpointer(nullptr), ourdeleter(deleter_type() ) {}    //Default
        constexpr unique_ptr (std::nullptr_t x) noexcept :
                                        unique_ptr() {}                    //From null pointer
        explicit unique_ptr (pointer ptr) noexcept:
                    ourpointer(ptr), ourdeleter(deleter_type() ) {}          //From pointer
        unique_ptr (pointer ptr, const deleter_type& del) noexcept:
                        ourpointer(ptr), ourdeleter(del){}                  //From pointer + lvalue deleter
       unique_ptr (pointer ptr, typename std::remove_reference<deleter_type>::type&& del) noexcept:
                       ourpointer(ptr), ourdeleter(std::move(del) ) {}     //From pointer + rvalue deleter
        unique_ptr (unique_ptr&& x) noexcept:
            ourpointer(x.release() ),
                ourdeleter(std::forward<deleter_type>(x.get_deleter()) ) {} //Move

         template <typename SecondType, typename Seconddeleter>
        unique_ptr (unique_ptr<SecondType, Seconddeleter>&& x) noexcept:
            ourpointer(x.release() ),
                ourdeleter(std::forward<deleter_type>(x.get_deleter()) ) {} //Move //Move-cast

       /* template <class U>
        unique_ptr (std::auto_ptr <U>&& x) noexcept:
            ourdeleter(default_delete<DataType>() ),
                ourpointer(x.release() )//Move from auto_ptr, deprecated*/

        unique_ptr (const unique_ptr&) = delete; //Forbid copying

       ~unique_ptr() noexcept {
            reset();
        }

        unique_ptr& operator=(unique_ptr&& x) noexcept {
            reset(x.release());
            get_deleter() = std::forward<deleter_type>(x.get_deleter());
            return *this;
        }

        pointer get() const noexcept {
            return ourpointer;
        }

        deleter_type&  get_deleter() noexcept {
            return ourdeleter;
        }

        const deleter_type& get_deleter() const noexcept {
          return ourdeleter;
        }

        explicit operator bool() const noexcept {
          return get() == pointer(nullptr) ? false : true;
        }

        pointer release() noexcept {
            pointer ptr = get();
            ourpointer = pointer(nullptr);
            return ptr;
        }

        void reset(pointer ptr = pointer(nullptr) ) noexcept {
            std::swap (ourpointer, ptr);
            if (ptr != pointer(nullptr) )
                ourdeleter(ptr);
        }

        void swap(unique_ptr& u) noexcept {
            std::swap(ourdeleter, u.ourdeleter);
            std::swap(ourpointer, u.ourpointer);
        }

        typename std::add_lvalue_reference<element_type>::type operator*() const {
            return *get();
        }

        pointer operator->() const noexcept {
            return get();
        }

        template <typename SecondType, typename Seconddeleter>
        bool operator == (const unique_ptr<SecondType, Seconddeleter>& x) const {
            return ourpointer == x.ourpointer;
        }

        bool operator == (std::nullptr_t) const {
            return ! (bool)(*this);
        }

        template <typename SecondType, typename Seconddeleter>
        bool operator != (const unique_ptr<SecondType, Seconddeleter>& x) const {
            return ourpointer != x.ourpointer;
        }

        bool operator != (std::nullptr_t) const {
            return (bool)(*this);
        }

        template <typename SecondType, typename Seconddeleter>
        bool operator < (const unique_ptr<SecondType, Seconddeleter>& x) const {
            return std::less<pointer> (ourpointer, x.ourpointer);
        }

        bool operator < (std::nullptr_t) const {
            return std::less<pointer> (ourpointer, nullptr);
        }


        template <typename SecondType, typename Seconddeleter>
        bool operator <= (const unique_ptr<SecondType, Seconddeleter>& x) const {
            return ! (x<*this);
        }

        bool operator <= (std::nullptr_t) const {
            return ! (nullptr<*this);
        }

        template <typename SecondType, typename Seconddeleter>
        bool operator > (const unique_ptr<SecondType, Seconddeleter>& x) const {
            return std::less<pointer> (x, ourpointer);
        }

        bool operator > (std::nullptr_t) const {
            return std::less<pointer> (nullptr, ourpointer);
        }


        template <typename SecondType, typename Seconddeleter>
        bool operator >= (const unique_ptr<SecondType, Seconddeleter>& x) const {
            return ! (*this < x);
        }

        bool operator >= (std::nullptr_t) const {
            return ! (*this < nullptr);
        }

};

template <typename DataType, typename DelType>
void  swap(unique_ptr<DataType, DelType>& x, unique_ptr<DataType, DelType>& y) noexcept {
     x.swap(y);
}

template <typename DataType, typename DelType>
bool operator == (std::nullptr_t, const unique_ptr<DataType, DelType>& x) {
    return ! (bool)x;
}

template <typename DataType, typename DelType>
bool operator != (std::nullptr_t, const unique_ptr<DataType, DelType>& x) {
    return  (bool)x;
}

template <typename DataType, typename DelType>
bool operator < (std::nullptr_t, const unique_ptr<DataType, DelType>& x) {
    return x > nullptr;
}

template <typename DataType, typename DelType>
bool operator <= (std::nullptr_t, const unique_ptr<DataType, DelType>& x) {
    return !(x < nullptr);
}

template <typename DataType, typename DelType>
bool operator > (std::nullptr_t, const unique_ptr<DataType, DelType>& x) {
    return x < nullptr;
}

template <typename DataType, typename DelType>
bool operator >= (std::nullptr_t, const unique_ptr<DataType, DelType>& x) {
    return !(nullptr < x);
}


