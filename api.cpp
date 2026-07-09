#include "api.h"

#include <iostream>
#include <sstream>

namespace {

// Send a command and read back a single line of response.
std::string command(const std::string& cmd) {
    std::cout << cmd << std::endl;
    std::cout.flush();
    std::string response;
    std::getline(std::cin, response);
    return response;
}

// Send a command that has no response (fire and forget).
void commandNoResponse(const std::string& cmd) {
    std::cout << cmd << std::endl;
    std::cout.flush();
}

bool toBool(const std::string& s) {
    return s == "true";
}

} // namespace

namespace API {

int mazeWidth() {
    return std::stoi(command("mazeWidth"));
}

int mazeHeight() {
    return std::stoi(command("mazeHeight"));
}

bool wallFront(int numHalfSteps) {
    std::ostringstream cmd;
    cmd << "wallFront " << numHalfSteps;
    return toBool(command(cmd.str()));
}

bool wallRight(int numHalfSteps) {
    std::ostringstream cmd;
    cmd << "wallRight " << numHalfSteps;
    return toBool(command(cmd.str()));
}

bool wallLeft(int numHalfSteps) {
    std::ostringstream cmd;
    cmd << "wallLeft " << numHalfSteps;
    return toBool(command(cmd.str()));
}

bool wallBack(int numHalfSteps) {
    std::ostringstream cmd;
    cmd << "wallBack " << numHalfSteps;
    return toBool(command(cmd.str()));
}

bool moveForward(int distance) {
    std::ostringstream cmd;
    cmd << "moveForward " << distance;
    return command(cmd.str()) != "crash";
}

bool moveForwardHalf(int numHalfSteps) {
    std::ostringstream cmd;
    cmd << "moveForwardHalf " << numHalfSteps;
    return command(cmd.str()) != "crash";
}

void turnRight() {
    command("turnRight");
}

void turnLeft() {
    command("turnLeft");
}

void turnRight45() {
    command("turnRight45");
}

void turnLeft45() {
    command("turnLeft45");
}

void setWall(int x, int y, char direction) {
    std::ostringstream cmd;
    cmd << "setWall " << x << " " << y << " " << direction;
    commandNoResponse(cmd.str());
}

void clearWall(int x, int y, char direction) {
    std::ostringstream cmd;
    cmd << "clearWall " << x << " " << y << " " << direction;
    commandNoResponse(cmd.str());
}

void setColor(int x, int y, char color) {
    std::ostringstream cmd;
    cmd << "setColor " << x << " " << y << " " << color;
    commandNoResponse(cmd.str());
}

void clearColor(int x, int y) {
    std::ostringstream cmd;
    cmd << "clearColor " << x << " " << y;
    commandNoResponse(cmd.str());
}

void clearAllColor() {
    commandNoResponse("clearAllColor");
}

void setText(int x, int y, const std::string& text) {
    std::ostringstream cmd;
    cmd << "setText " << x << " " << y << " " << text;
    commandNoResponse(cmd.str());
}

void clearText(int x, int y) {
    std::ostringstream cmd;
    cmd << "clearText " << x << " " << y;
    commandNoResponse(cmd.str());
}

void clearAllText() {
    commandNoResponse("clearAllText");
}

bool wasReset() {
    return toBool(command("wasReset"));
}

void ackReset() {
    command("ackReset");
}

double getStat(const std::string& stat) {
    std::ostringstream cmd;
    cmd << "getStat " << stat;
    return std::stod(command(cmd.str()));
}

} // namespace API