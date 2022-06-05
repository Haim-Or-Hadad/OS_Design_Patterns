#include <pthread.h>
#include <string.h>
#include <iostream>
#include <mutex>
#include <fstream>


using namespace std;

pthread_mutex_t single_locker = PTHREAD_MUTEX_INITIALIZER;
static int flag = 0;

template<typename T>
class singleton
{
private:
T object_;
static singleton *singleton_obj;
//constructor
singleton(T object1){
//pthread_mutex_lock(&single_locker);
this->object_ = object1;
//flag = 1;
}

public:
static singleton *Instance(T object1){
pthread_mutex_lock(&single_locker);
if (flag == 0)
{
   singleton * singleton_obj = new singleton(object1);
   cout << "create new object" << endl;
   flag = 1;
}
pthread_mutex_unlock(&single_locker);
return singleton_obj;
}

void Destroy(){
pthread_mutex_lock(&single_locker);
delete singleton_obj;
flag = 0;
pthread_mutex_unlock(&single_locker);
}
};
template <typename T> singleton<T> *singleton<T>::singleton_obj = 0;

class pet {
    public:
    std::string pet_name;
    pet(std::string pet_name) {
        this->pet_name = pet_name;
    }
};

int main()
{
    ///////////////////////test-1//////////////////////////////
    pet *my_pet; 
    singleton<pet *> *instance_1 = singleton<pet *>::Instance(my_pet);
    singleton<pet *> *instance_2 = singleton<pet *>::Instance(my_pet);
    if (instance_1 != instance_2) {
        cout << "test case failed" << endl;;
    }
    cout << "test-1 passed" << endl;
    instance_1->Destroy();
    //////////////////////////test-2///////////////////////////
    FILE *pointer_to_file = fopen("example.txt","R");
    singleton<FILE *> *file_1 = singleton<FILE *>::Instance(pointer_to_file);
    singleton<FILE *> *file_2 = singleton<FILE *>::Instance(pointer_to_file);
    if (file_1 != file_2) {
        cout << "test case failed" << endl;;
        exit(EXIT_FAILURE);
    }
    cout << "test-2 passed" << endl;
    file_1->Destroy();
    exit(EXIT_SUCCESS);
}