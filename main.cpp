#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>

typedef void(*NewValueHandlerFnPtr)(int);


void IntHandler1(int n) {
    const int Threshold = 50;
    if (n > Threshold) {
        std::cout << "Alert!" << std::endl;
    }
}

void IntHandler2(int n) {
    std::cout << "New value: " << n << std::endl;
}

int FetchValue() {
    int n = 0;
    std::cout << "Enter the next integer (0 for exit): ";
    std::cin >> n;
    return n;
}

int GenerateRandomNumber() {
    // use current time as seed for random generator
    std::srand(std::time(0)); 
    return std::rand();
}

// event handler is hardcoded => not flexible
void HardcodedHandlerDemo() {
    int n = 0;
    do {
        n = FetchValue();
        if (n > 50) {
            std::cout << "Alert!" << std::endl;
        }
    } while (n != 0);
}

// event handler is set in runtime => more flexible
void HandlerSetInRuntimeDemo() {
    NewValueHandlerFnPtr handler = nullptr;
    handler = (GenerateRandomNumber() % 2) ? IntHandler1 : IntHandler2;
 
    int n = 0;
    do {
        n = FetchValue();
        handler(n);
    } while (n != 0);
}

// What if we want handler to have some state?
// What if handler has to use some resources that have to be acquired/released?
// Simple functons are not flexible. Lifetime of local variables is limited by 
// the duration of the function. Static variables are not thread safe.
// We have to use some object.

// Function object (Functor) - any class which has overloaded "function call operator" - ()
class NewValueHandler {
public:
    NewValueHandler(int alert_threshold) : alert_threshold_(alert_threshold) {
        // acquire resources
    }

    ~NewValueHandler() {
        // free resources
    }

    void operator()(int value) const {
        if (value > alert_threshold_) {
            std::cout << "Alert!" << std::endl;
        }
    }
private:
    int alert_threshold_;
};

// functor is used => even more flexibility
void FunctorDemo() {
    NewValueHandler handler(50);
    int n = 0;

    do {
        n = FetchValue();
        handler(n);
    } while (n != 0);
}

// Functor drawbacks:
// - lots of boilerplate code
// - its implementation is not at the same place where it's used (esp. annoying if it's used only at one place)

// When to use functors?
// - when handlers are used at many places
// - when handlers have to use resources
// - when handlers have to keep their state

// API where handler (function / function pointer) is passed as an argument
void RunEventLoop(NewValueHandlerFnPtr handler) {
    int n = 0;

    do {
        n = FetchValue();
        handler(n);
    } while (n != 0);
}

// event handler is passed as an argument
void EventHandlerPassedAsAnArgumentDemo() {
    NewValueHandlerFnPtr handler = nullptr;
    handler = (GenerateRandomNumber() % 2) ? IntHandler1 : IntHandler2;
    RunEventLoop(handler);
}

// RunEventLoop requires a function to be passed but we don't want to write new fucntion somewhere else; 
// we want to define "function" (not real function but function object...) here, inside a function - 
// we can use lambda expression (lambda).
// Lambda expression crates anonymous (unnamed) function object (nameless functor).
void EventHandlerIsLambdaDemo() {
    RunEventLoop([](int n) {
        std::cout << "New value: " << n << std::endl;
    });
}

// When to use lambdas?
// - for implementing trivial handlers/callbacks/functors/predicates when it's not worth it writing a separate function
// - for implementing them "on the spot" - in the same context where they are called
// - when such function is not shared but is used only at one place 

void LambdaDemo() {

    // instantiate and execute
    [](int n) {
        std::cout << "New value: " << n << std::endl;
    }(123);

    // if not taking any arguments:
    []() {
        std::cout << "Hello from lambda!" << std::endl;
    }();

    // this is also possible:
    [](){}();

    // returning a value (return type is deduced from the return statement):
    auto str = []() { 
        return std::string("Hello from lambda!"); 
    }();

    // return type can be explicitly declared:
    auto str2 = []() -> std::string {
        return std::string("Hello from lambda!");
    }();

    int threshold = 50;
    
    // capturing variable (by value) from the outer scope
    [threshold]() {
        std::cout << threshold << std::endl;
    }();

    int i = 1;
    // capturing all variables (by value) from the outer scope
    [=]() {
        std::cout << threshold << ", " << i << std::endl;
    }();

    // capturing variable by reference
    // WARNING: lambda object can outlive captured variable so non-existing variable might be dereferenced => DANGER!
    [&i](){
        i = 1;
    }();

    int j = 2;

    // capturing all variables by reference
    [&]() {
        i = 11;
        j = 22;
    }();

    // lambda can be stored and called later
    auto lambda = []() {
        std::cout << "Hello from lambda!" << std::endl;
    };

    lambda();
}

struct Comparer {
    bool operator()(const int& a, const int& b) const {
        return a > b;
    }
};

void PredicateDemo() {
    std::vector<int> v = { 3, 2, 6, 9, 1, 5 };
    std::vector<int> v_sorted_increasing = { 1, 2, 3, 5, 6, 9 };
    std::vector<int> v_sorted_decreasing = { 9, 6, 5, 3, 2, 1 };

    // sort in the increasing order
    std::sort(v.begin(), v.end());
    assert(v == v_sorted_increasing);

    // sort in the decreasing order
    std::sort(v.begin(), v.end(), Comparer());
    assert(v == v_sorted_decreasing);
}

void LambdaAsPredicateDemo() {
    std::vector<int> v = { 3, 2, 6, 9, 1, 5 };
    std::vector<int> v_sorted_increasing = { 1, 2, 3, 5, 6, 9 };
    std::vector<int> v_sorted_decreasing = { 9, 6, 5, 3, 2, 1 };

    // sort in the increasing order
    std::sort(v.begin(), v.end());
    assert(v == v_sorted_increasing);

    // sort in the decreasing order
    std::sort(v.begin(), v.end(), [](const int& a, const int&b) -> bool { return a > b; });
    assert(v == v_sorted_decreasing);
}

int main()
{
    // function pointers
    HardcodedHandlerDemo();
    HandlerSetInRuntimeDemo();

    // functor
    FunctorDemo();

    EventHandlerPassedAsAnArgumentDemo();

    // lambdas
    EventHandlerIsLambdaDemo();
    LambdaDemo();

    // predicates/lambdas in STL algorithms
    PredicateDemo();
    LambdaAsPredicateDemo();

    return 0;
}