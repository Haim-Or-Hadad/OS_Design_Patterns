#include <pthread.h>
#include <mutex>

using namespace std;

pthread_mutex_t single_locker;
static int flag = 0;

template<typename T>
class singleton
{
private:
T object_;
static singleton *singleton_obj;
//constructor
singelton(T object1){
pthread_mutex_lock(&single_locker);
this->object_ = object1;
flag = 1;
}
public:
static singleton *Instance(T object1){
if (flag == 1)
{
    pthread_mutex_lock(&single_locker);
    return singleton_obj;
}
flag = 1;
singleton * singleton_obj = new singleton(object1);
return singleton_obj;
}

void Destroy(){
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
    pet *my_pet; 
    singleton<pet *> *instance_1 = singleton<pet *>::Instance(my_pet);
    singleton<pet *> *instance_2 = singleton<pet *>::Instance(my_pet);
    if (instance_1 != instance_2) {
        printf("test case failed\n");
        exit(EXIT_FAILURE);
    }
    printf("test passed\n");
    exit(EXIT_SUCCESS);

    
}