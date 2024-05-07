// This calculator can recive a String Math Expression
// '.' Button have function that clear the LCD
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
/* Display */
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'.', '0', '=', '/'}
};
// Declare Node using in Character Stack
struct Node {
    char data;
    Node* next;

    Node(char value) : data(value), next(nullptr) {}
};
// Declare Character Stack data
struct CharStack {
private:
    Node* topNode;

public:
    CharStack() : topNode(nullptr) {}

    void push(char value) {
        Node* newNode = new Node(value);
        newNode->next = topNode;
        topNode = newNode;
    }

    void pop() {
        if (!isEmpty()) {
            Node* temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    char top() const {
        if (!isEmpty()) {
            return topNode->data;
        } else {
            return '\0'; // Trả về ký tự null nếu stack rỗng
        }
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    size_t size() const {
        size_t count = 0;
        Node* current = topNode;
        while (current != nullptr) {
            ++count;
            current = current->next;
        }
        return count;
    }
};
// Function to check character c is a Operator 
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}
// 
int getPrecedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0; // Đối với các ký tự không phải toán tử
}
// Function that convert Infix (Input String) to PostFix 
String infixToPostfix(String& infix) {
    CharStack operatorStack;
    String postfix;

    for (char c : infix) {
        if (c >= '0' && c <= '9') {
            postfix += c; // Ký tự là toán hạng, thêm vào biểu thức hậu tố
        } else if (isOperator(c)) {
            while (!operatorStack.isEmpty() && getPrecedence(operatorStack.top()) >= getPrecedence(c)) {
                postfix += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.push(c);
        } else if (c == '(') {
            operatorStack.push(c);
        } else if (c == ')') {
            while (!operatorStack.isEmpty() && operatorStack.top() != '(') {
                postfix += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.pop(); // Pop dấu ngoặc mở '(' khỏi stack
        }
    }

    while (!operatorStack.isEmpty()) {
        postfix += operatorStack.top();
        operatorStack.pop();
    }

    return postfix;
}

CharStack charStack;
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

uint64_t value = 0;

void showSpalshScreen() {
  lcd.setCursor(3, 0);
  String message = "CALCULATOR";
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(50);
  }
  delay(500);
}

void updateCursor() {
  if (millis() / 250 % 2 == 0 ) {
    lcd.cursor();
  } else {
    lcd.noCursor();
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);

  showSpalshScreen();
  lcd.clear();
  lcd.cursor();

  lcd.setCursor(1, 0);
}
String memory = ""; // Store String Of Inputs
int MemoryOfNumbers[100]; // Store Values Of Inputs
int temp = 0; // Store Value Each Input
// Value use to code the Algorithm
int i = 0; 
int j = 0;
bool pivot = 0;
int check = 0; 
int last = 0; 
int StoreInStack = 2; 
// Function to calculate from the String get from the inputs
int calculate(String S){
  int FirstOp;
  int SecondOp;
  int Result;
  CharStack MyStack;
  for(char c : S){
    if (c >= '0' && c <= '9'){
      MyStack.push(c);
    }
    else{
      if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ){
        //if (c == '=') Result = MemoryOfNumbers[MyStack.top() - 48];
        StoreInStack = MyStack.size() - StoreInStack;
        SecondOp = MemoryOfNumbers[MyStack.top() - 48];
        MyStack.pop();
        FirstOp = MemoryOfNumbers[MyStack.top() - 48];
        MyStack.pop();
        if (c == '+') Result = FirstOp + SecondOp;
        else if (c == '-') Result = FirstOp - SecondOp;
        else if (c == '*') Result = FirstOp * SecondOp;
        else if (c == '/') Result = FirstOp / SecondOp;
        MemoryOfNumbers[StoreInStack] = Result;
        MyStack.push(StoreInStack + 48);
      }
    }
  }
  return Result;
}
// Function to get size of a String
int getSize(String stringc){
  int count = 0;
  for(char c:stringc)
    count++;
  return count;
}
// Function to get size of a numbers
int getNumbers(int number){
  int count = 0;
  while(number != 0){
    number /= 10;
    count ++;
  }
  return count;
}
// Function to check syntax whether the input is right to calculate
bool checkSyntax(String S){
  bool GotOperand = 0;
  int count = 0;
  CharStack MyStack;
  for(char c : S){
    MyStack.push(c);
    count++;
  }
  if (MyStack.top() >= '0' && MyStack.top() <= '9'){
    MyStack.pop();
    GotOperand = 1;
    if (count == 1) return 1; // Case that only a input is a number
  }
  else{
    return 0;
  }
  for(char c : S){
    if (MyStack.top() == '+' || MyStack.top() == '-' || MyStack.top() == '*' || MyStack.top() == '/'){
      if (GotOperand == 1){
        MyStack.pop();
        GotOperand = 0;
      }
      else
        return 0;
    }
    else if (MyStack.top() >= 0 && MyStack.top() <= 9){
      if (GotOperand == 0){
        MyStack.pop();
        GotOperand = 1;
      }
      else return 0;
    }
  }
  return 1;
}
// Function that process each key of input
void processInput(char key) {
  // Case input is '.' or '='
  if (key == '.' || key == '='){
    if (key == '='){
      if (checkSyntax(memory) == 0){
        lcd.clear();
        lcd.print("Syntax Error!!!");
      }
      else if (checkSyntax(memory) == 1){
        if (!pivot){ 
          lcd.setCursor(15 - getNumbers(temp), 1);
          lcd.print(temp);
        }
        else{
          MemoryOfNumbers[i++] = temp;
          memory = infixToPostfix(memory);
          int Result = calculate(memory);
          lcd.setCursor(15 - getNumbers(Result), 1);
          lcd.print(Result);
        }
        pivot = 1;
      }
    }
    if (key == '.'){
      lcd.clear();
      pivot = 0;
    }
    //Reset all values
    temp = 0;
    memory = "";
    i = 0;
    j = 0;
    check = 0;
    last = 0;
    StoreInStack = 2;
  }
  // Case input is Operand or Operator
  else{
    if (last == 1){
      lcd.clear();
      last = 0;
    }
    lcd.print(key);
    // Case input is Operand
    if(key != '+' && key != '-' && key != '*' && key != '/'){
      temp = temp * 10 + (key - 48);
      check++;
      if (check == 1) memory += i;
    }
    // Case input is operator
    else{
      memory += key;
      MemoryOfNumbers[i++] = temp;
      temp = 0;
      check = 0;
      pivot = 1;
    }
  }
}
void loop() {
  updateCursor();
  char key = keypad.getKey();
  if (key) {
    processInput(key);
  }
}
