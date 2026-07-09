#pragma once
// Thin C++ wrapper around the mms mouse API.
// Every call blocks on stdout->stdin round trip where the protocol expects a response.
// Protocol reference: https://github.com/mackorone/mms#mouse-api

#include <string>

namespace API {

int mazeWidth();
int mazeHeight();

bool wallFront(int numHalfSteps = 1);
bool wallRight(int numHalfSteps = 1);
bool wallLeft(int numHalfSteps = 1);
bool wallBack(int numHalfSteps = 1);

// Both can respond "crash" - caller must check the return value.
bool moveForward(int distance = 1);       // returns false on crash
bool moveForwardHalf(int numHalfSteps = 1); // returns false on crash

void turnRight();
void turnLeft();
void turnRight45();
void turnLeft45();

void setWall(int x, int y, char direction);
void clearWall(int x, int y, char direction);

void setColor(int x, int y, char color);
void clearColor(int x, int y);
void clearAllColor();

void setText(int x, int y, const std::string& text);
void clearText(int x, int y);
void clearAllText();

bool wasReset();
void ackReset();

double getStat(const std::string& stat);

} // namespace API