#include <iostream>
#include "uniq.h"
#include <utility>

class ourdeleter {
    public:
        int value;
        ourdeleter (): value(0) {}
        void operator () (std::pair<int, int>* pairtodelete) {
            delete pairtodelete;
            ++value;
        }
};

bool Test1() {
    unique_ptr<std::pair <int, int> > ourptr;
    ourptr.release();
    return !(bool)ourptr;
}


bool Test2() {
    std::pair<int, int> ourpair (6,17);
    unique_ptr<std::pair <int, int> > ourptr;
    ourptr.reset (&ourpair);
    return *ourptr == ourpair;
}

bool Test3() {
    std::pair<int, int> firstpair (18,26);
    ourdeleter testdeleter;
    unique_ptr<std::pair <int, int>, ourdeleter> ourptr (&firstpair, testdeleter);
    std::pair<int, int>* temporal = ourptr.release();
    delete temporal;
    std::pair<int, int> secondpair (388,429);
    ourptr.reset(&secondpair);
    ourptr.reset(nullptr);
    return (ourptr.get_deleter() ).value==1;
}

bool Test4() {
    std::pair<int, int> firstpair (18,26);
    unique_ptr<std::pair <int, int> > firstptr (&firstpair);
    std::pair<int, int> secondpair (90,28);
    unique_ptr<std::pair <int, int> > secondptr (&secondpair);
    swap(firstptr, secondptr);
    return std::get<0>(*firstptr.get() )==90 && std::get<1>(*firstptr.get() )==28;
}

bool Test5() {
    std::pair<int, int> firstpair (30,66);
    unique_ptr<std::pair <int, int> > firstptr (&firstpair);
    unique_ptr<std::pair <int, int> > secondptr (std::move(firstptr) );
    return !(bool)firstptr && std::get<0>(*secondptr.get() )==30 && std::get<1>(*secondptr.get() )==66;
}

bool Test6() {
    std::pair<int, int> firstpair (50,156);
    unique_ptr<std::pair <int, int> > firstptr (&firstpair);
    std::pair<int, int> secondpair (154,241);
    unique_ptr<std::pair <int, int> > secondptr (&secondpair);
    return firstptr!=secondptr;
}

bool Test7() {
    unique_ptr<std::pair <int, int> > ourptr (nullptr);
    std::pair<int, int> firstpair (399,64);
    std::pair<int, int> *secondpair = new std::pair<int, int> (675,46);
    ourptr.reset(&firstpair);
    ourptr.reset(secondpair);
    secondpair = nullptr;
    return ourptr!=nullptr && std::get<0>(*ourptr)==675 && std::get<1>(*ourptr)==46;
}


int main () {
    std::cout<<std::boolalpha;
    std::cout<<Test1()<<std::endl;
    std::cout<<Test2()<<std::endl;
    std::cout<<Test3()<<std::endl;
    std::cout<<Test4()<<std::endl;
    std::cout<<Test5()<<std::endl;
    std::cout<<Test6()<<std::endl;
    std::cout<<Test7()<<std::endl;
    return 0;
}
