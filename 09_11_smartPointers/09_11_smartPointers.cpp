#include <iostream>

template <typename T>
class UniquePtr {
private:
    T* pointer;

public:
    // Конструктор
    explicit UniquePtr(T* ptr = nullptr) : pointer(ptr) {}

    // Деструктор
    ~UniquePtr() 
    {
        reset();
    }

    // Перемещающий конструктор
    UniquePtr(UniquePtr&& other) noexcept : pointer(other.pointer) 
    {
        other.pointer = nullptr;
    }

    // Перемещающий оператор присваивания
    UniquePtr& operator=(UniquePtr&& other) noexcept 
    {
        if (this != &other) 
        {
            reset();
            pointer = other.pointer;
            other.pointer = nullptr;
        }
        return *this;
    }

    // Оператор разыменования
    T& operator*() const 
    {
        return *pointer;
    }

    // Оператор доступа к члену
    T* operator->() const
    {
        return pointer;
    }

    // Получение указателя
    T* get() const
    {
        return pointer;
    }

    // Освобождение управления указателем
    void reset() 
    {
        if (pointer) 
        {
            std::cout << "Освобождение памяти по адресу: " << reinterpret_cast<void*>(pointer) << std::endl;
            delete pointer;
        }
        pointer = nullptr;
    }

    // Сброс указателя
    T* release() 
    {
        T* temp = pointer;
        pointer = nullptr;
        return temp;
    }
};


template <typename T>
class SharedPtr {
private:
    T* pointer;      // Указатель на управляемый объект
    size_t* refCount; // Указатель на счетчик ссылок

    // Увеличение счетчика ссылок
    void incrementRefCount() 
    {
        if (refCount) 
        {
            (*refCount)++;
        }
    }

    // Уменьшение счетчика ссылок и, при необходимости, освобождение памяти
    void decrementRefCount() 
    {
        if (refCount) 
        {
            (*refCount)--;
            if (*refCount == 0)
             {
                delete pointer;
                delete refCount;
            }
        }
    }

public:
    // Конструктор
    explicit SharedPtr(T* ptr = nullptr) : pointer(ptr), refCount(new size_t(1)) {}

    // Конструктор копирования
    SharedPtr(const SharedPtr& other) : pointer(other.pointer), refCount(other.refCount)
     {
        incrementRefCount();
    }

    // Деструктор
    ~SharedPtr() 
    {
        decrementRefCount();
    }

    // Оператор присваивания
    SharedPtr& operator=(const SharedPtr& other) 
    {
        if (this != &other)
         {
            decrementRefCount();
            pointer = other.pointer;
            refCount = other.refCount;
            incrementRefCount();
        }
        return *this;
    }

    // Оператор присваивания nullptr
    SharedPtr& operator=(std::nullptr_t) {
        decrementRefCount();
        pointer = nullptr;
        refCount = nullptr;
        return *this;
    }

    // Оператор разыменования
    T& operator*() const 
    {
        return *pointer;
    }

    // Оператор доступа к члену
    T* operator->() const 
    {
        return pointer;
    }

    // Получение указателя
    T* get() const 
    {
        return pointer;
    }

    // Возвращение текущего количества владельцев
    size_t use_count() const
     {
        return refCount ? *refCount : 0;
    }
};


int main() 
{
    setlocale(LC_ALL, "rus");
    // UniquePtr с указателем на int
    UniquePtr<int> smartPointer(new int(42));

    // адрес памяти, на который указывает smartPointer
    std::cout << "Указатель smartPointer указывает на адрес: " << reinterpret_cast<void*>(smartPointer.get()) << std::endl;

    // оператор разыменования для доступа к значению
    std::cout << "Значение по адресу, на который указывает smartPointer: " << *smartPointer << std::endl;

    // Создаю еще один UniquePtr, передавая владение из первого
    UniquePtr<int> anotherSmartPointer = std::move(smartPointer);

    std::cout << "Перадача адреса памяти" << std::endl;
    // Вывожу адрес памяти, на который указывает anotherSmartPointer
    std::cout << "Указатель anotherSmartPointer указывает на адрес: " << reinterpret_cast<void*>(anotherSmartPointer.get()) << std::endl;

    // После передачи владения smartPointer больше не указывает на данные
    if (!smartPointer.get()) 
    {
        std::cout << "smartPointer не указывает на данные." << std::endl;
    }

    // Использую новый указатель
    std::cout << "Значение по адресу, на который указывает anotherSmartPointer: " << *anotherSmartPointer << std::endl;

    // Сбрасываю второй указатель, что приводит к освобождению памяти
    anotherSmartPointer.reset();

    // Теперь anotherSmartPointer больше не указывает на данные
    if (!anotherSmartPointer.get()) 
    {
        std::cout << "anotherSmartPointer не указывает на данные." << std::endl;
    }


    std::cout << "\n\n";
    // Создаю SharedPtr с указателем на int
    SharedPtr<int> sharedPointer1(new int(42));

    std::cout << "Указатель sharedPointer1 указывает на адрес: " << reinterpret_cast<void*>(sharedPointer1.get()) << std::endl;     
    // количество владельцев
    std::cout << "Количество владельцев: " << sharedPointer1.use_count() << std::endl;

    // Создаю еще один SharedPtr, разделяя владение с первым
    SharedPtr<int> sharedPointer2 = sharedPointer1;
    std::cout << "Указатель sharedPointer2 указывает на адрес: " << reinterpret_cast<void*>(sharedPointer2.get()) << std::endl;

    // количество владельцев после создания второго SharedPtr
    std::cout << "Количество владельцев: " << sharedPointer1.use_count() << std::endl;

    // Создаю третий SharedPtr, разделяя владение с первым и вторым
    SharedPtr<int> sharedPointer3 = sharedPointer1;
    std::cout << "Указатель sharedPointer3 указывает на адрес: " << reinterpret_cast<void*>(sharedPointer3.get()) << std::endl;

   // количество владельцев после создания третьего SharedPtr
    std::cout << "Количество владельцев: " << sharedPointer3.use_count() << std::endl;

    // Присваиваю nullptr одному из SharedPtr
    sharedPointer1 = nullptr;

    // количество владельцев после присваивания nullptr
    std::cout << "Количество владельцев: " << sharedPointer3.use_count() << std::endl;

    std::cout << "Значение по адресу, на который указывает sharedPointer2: " << *sharedPointer3 << std::endl;

    return 0;
}