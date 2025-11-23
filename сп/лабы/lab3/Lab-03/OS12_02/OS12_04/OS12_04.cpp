#include <iostream>
#include <windows.h>

#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\Lab-03\\Lab-03\\OS12_02\\x64\\Debug\\OS12_LIB.lib")

#include "OS12.h"

int main()
{
    std::cout << "=== OS12_04 LIB Test Application ===" << std::endl;
    std::cout << "Testing OS12_LIB library functionality" << std::endl << std::endl;

    try
    {
        // 1. Инициализация двух независимых обработчиков
        std::cout << "1. Initializing OS12 handlers..." << std::endl;
        OS12HANDEL h1 = OS12::Init();
        OS12HANDEL h2 = OS12::Init();
        std::cout << "   Handlers initialized successfully!" << std::endl << std::endl;

        // 2. Тестирование операций сложения
        std::cout << "2. Testing Adder::Add operations:" << std::endl;
        std::cout << "   OS12::Adder::Add(h1, 2, 3) = " << OS12::Adder::Add(h1, 2, 3) << std::endl;
        std::cout << "   OS12::Adder::Add(h2, 2, 3) = " << OS12::Adder::Add(h2, 2, 3) << std::endl;
        std::cout << "   OS12::Adder::Add(h1, 5.5, 2.5) = " << OS12::Adder::Add(h1, 5.5, 2.5) << std::endl;
        std::cout << "   OS12::Adder::Add(h2, -3, 7) = " << OS12::Adder::Add(h2, -3, 7) << std::endl << std::endl;

        // 3. Тестирование операций вычитания
        std::cout << "3. Testing Adder::Sub operations:" << std::endl;
        std::cout << "   OS12::Adder::Sub(h1, 2, 3) = " << OS12::Adder::Sub(h1, 2, 3) << std::endl;
        std::cout << "   OS12::Adder::Sub(h2, 2, 3) = " << OS12::Adder::Sub(h2, 2, 3) << std::endl;
        std::cout << "   OS12::Adder::Sub(h1, 10, 4.5) = " << OS12::Adder::Sub(h1, 10, 4.5) << std::endl;
        std::cout << "   OS12::Adder::Sub(h2, -5, -3) = " << OS12::Adder::Sub(h2, -5, -3) << std::endl << std::endl;

        // 4. Тестирование операций умножения
        std::cout << "4. Testing Multiplier::Mul operations:" << std::endl;
        std::cout << "   OS12::Multiplier::Mul(h1, 2, 3) = " << OS12::Multiplier::Mul(h1, 2, 3) << std::endl;
        std::cout << "   OS12::Multiplier::Mul(h2, 2, 3) = " << OS12::Multiplier::Mul(h2, 2, 3) << std::endl;
        std::cout << "   OS12::Multiplier::Mul(h1, 2.5, 4) = " << OS12::Multiplier::Mul(h1, 2.5, 4) << std::endl;
        std::cout << "   OS12::Multiplier::Mul(h2, -3, 5) = " << OS12::Multiplier::Mul(h2, -3, 5) << std::endl << std::endl;

        // 5. Тестирование операций деления
        std::cout << "5. Testing Multiplier::Div operations:" << std::endl;
        std::cout << "   OS12::Multiplier::Div(h1, 2, 3) = " << OS12::Multiplier::Div(h1, 2, 3) << std::endl;
        std::cout << "   OS12::Multiplier::Div(h2, 2, 3) = " << OS12::Multiplier::Div(h2, 2, 3) << std::endl;
        std::cout << "   OS12::Multiplier::Div(h1, 10, 2) = " << OS12::Multiplier::Div(h1, 10, 2) << std::endl;
        std::cout << "   OS12::Multiplier::Div(h2, 7.5, 2.5) = " << OS12::Multiplier::Div(h2, 7.5, 2.5) << std::endl << std::endl;

        // 6. Тестирование обработки ошибок
        std::cout << "6. Testing error handling:" << std::endl;
        try
        {
            // Попытка деления на ноль
            double result = OS12::Multiplier::Div(h1, 5, 0);
            std::cout << "   ERROR: Division by zero should have thrown exception!" << std::endl;
        }
        catch (int e)
        {
            std::cout << "   ✓ Division by zero correctly caught exception: " << e << std::endl;
        }

        try
        {
            // Попытка использования неверного handle
            double result = OS12::Adder::Add(nullptr, 1, 2);
            std::cout << "   ERROR: Invalid handle should have thrown exception!" << std::endl;
        }
        catch (int e)
        {
            std::cout << "   ✓ Invalid handle correctly caught exception: " << e << std::endl;
        }
        std::cout << std::endl;

        // 7. Освобождение ресурсов
        std::cout << "7. Disposing handlers..." << std::endl;
        OS12::Dispose(h1);
        OS12::Dispose(h2);
        std::cout << "   Handlers disposed successfully!" << std::endl << std::endl;

        std::cout << "=== All tests completed successfully! ===" << std::endl;
    }
    catch (int e)
    {
        std::cout << "!!! CRITICAL ERROR: OS12 operation failed with error code: " << e << std::endl;
        std::cout << "=== Test application terminated with errors ===" << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "!!! UNKNOWN ERROR: Unexpected exception occurred" << std::endl;
        std::cout << "=== Test application terminated with errors ===" << std::endl;
        return 1;
    }

    return 0;
}