// launchpad-based ic tester for and (7408) / or (7432) gates
// -----------------------------------------------------------
// this program automatically detects whether an inserted ic is a 7408 (and) or 7432 (or) 
// by checking gate outputs against expected truth table values. it then displays the 
// identified ic type and the status of each gate using leds.

// pin definitions
const int leftInputPin = 2;   // first input to the gate under test
const int rightInputPin = 3;  // second input to the gate under test
const int outputX = 8;        // led indicator for ic type (and/or)
const int outputY = 9;        // led indicator for ic type (and/or)

// ic output pins (reads gate outputs from the ic)
const int gateOutputPins[4] = {13, 12, 11, 10};

// status led pins (shows if each gate is working correctly)
const int statusLEDPins[4] = {7, 6, 5, 4};

// test inputs for the gate under test
const char testInputsA[4] = {LOW, LOW, HIGH, HIGH};
const char testInputsB[4] = {LOW, HIGH, LOW, HIGH};

// expected outputs (first 4 are for and gates, last 4 are for or gates)
const int truthTable[8] = {0, 0, 0, 1, 0, 1, 1, 1}; 

void setup() {
    // set up input test pins (these will drive the ic inputs)
    pinMode(leftInputPin, OUTPUT);
    pinMode(rightInputPin, OUTPUT);

    // set up ic gate output reading pins
    for (int i = 0; i < 4; i++) {
        pinMode(gateOutputPins[i], INPUT);
        pinMode(statusLEDPins[i], OUTPUT);
    }

    // set up ic type indicator leds
    pinMode(outputX, OUTPUT);
    pinMode(outputY, OUTPUT);
}

void loop() {
    int detectedGateType = 0;  // keeps track of whether the ic is and or or
    int gateOutputs[4];        // stores readings from the ic
    int statusFlags[4] = {1, 1, 1, 1}; // keeps track of which gates pass the test
    int failureCount = 0;      // counts how many gates fail
    int tableOffset = 0;       // 0 if it's an and gate, 4 if it's an or gate

    // ic identification phase
    for (int step = 0; step < 4; step++) {
        // set the test inputs for this step
        digitalWrite(leftInputPin, testInputsA[step]);
        digitalWrite(rightInputPin, testInputsB[step]);
        delay(100);

        failureCount = 0; // reset mismatch counter

        // read gate outputs from the ic
        for (int i = 0; i < 4; i++) {
            gateOutputs[i] = digitalRead(gateOutputPins[i]);

            // compare output to expected and gate truth table
            if (gateOutputs[i] != truthTable[step]) {
                failureCount++;
            }

            // if at least 3 gates fail, assume it's an or gate (offset the truth table)
            if (failureCount >= 3) {
                tableOffset = 4;
                break;
            }
        }

        if (tableOffset == 4) {
            break;
        }
    }

    // display the detected ic type on the leds
    digitalWrite(outputX, tableOffset == 0 ? LOW : HIGH);  // and = low, or = high
    digitalWrite(outputY, tableOffset == 0 ? HIGH : LOW);  // and = high, or = low

    // gate testing phase
    for (int cycle = 0; cycle < 4; cycle++) {
        // set the test inputs again for final validation
        digitalWrite(leftInputPin, testInputsA[cycle]);
        digitalWrite(rightInputPin, testInputsB[cycle]);
        delay(250);

        for (int i = 0; i < 4; i++) {
            gateOutputs[i] = digitalRead(gateOutputPins[i]);

            // if the gate output matches the expected value, turn on the status led
            if (gateOutputs[i] == truthTable[cycle + tableOffset] && statusFlags[i] == 1) {
                digitalWrite(statusLEDPins[i], HIGH);
            } else {
                digitalWrite(statusLEDPins[i], LOW);
                statusFlags[i] = 0; // mark this gate as failed
            }
        }
    }

    delay(2000);  // wait before running the next test cycle
}
