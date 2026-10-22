#include <iostream>

int main(){
    // Simple Calculator Program
    // Written by: Rüzgar Kaan
    // This program performs basic arithmetic operations (+, -, *, /)

    char op;
    double num1;
    double num2;
    double result;

    std::cout << "*********** CALCULATOR ***********\n";

    // Ask user to choose an operator
    std::cout << "Enter either (+ - * /): ";
    std::cin >> op;

    // Input for the first number
    std::cout << "Enter #1: ";
    std::cin >> num1;

    // Input for the second number
    std::cout << "Enter #2: ";
    std::cin >> num2;

    // Handle the operation based on user input
    switch(op){
        case '+':
            result = num1 + num2;
            std::cout << "result: " << result << '\n';
            break;

        case '-':
            result = num1 - num2;
            std::cout << "result: " << result << '\n';
            break;

        case '*':
            result = num1 * num2;
            std::cout << "result: " << result << '\n';
            break;

        case '/':
            // Prevent division by zero
            if(num2 == 0){
                std::cout << "Error: Division by zero is not allowed.\n";
            }
            else{
                result = num1 / num2;
                std::cout << "result: " << result << '\n';
            }
            break;

        default:
            // Handles any invalid operator input
            std::cout << "That wasn't a valid operator\n";
            break;
    }

    std::cout << "**********************************";

    return 0;
}

