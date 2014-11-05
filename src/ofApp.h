#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"

#define SIZE_OF_COMPRESSED_DATA 32
#define BAUDRATE    9600

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    void refreshSerialChannels();
    int  accessToShoe();
    
    void guiSetup();
    void guiEvent(ofxUIEventArgs &e);
    void translateToBinary(ofxUIToggle *input[SIZE_OF_COMPRESSED_DATA*8],
                           unsigned char *output[SIZE_OF_COMPRESSED_DATA]);
    void translateToBinary(unsigned int input, unsigned char *output[4]);
    void setShoeData(unsigned char myIconData[SIZE_OF_COMPRESSED_DATA],
                     unsigned char mySteps[4],
                     unsigned char mySex,
                     char r, char g, char b);
    
    ofxUISuperCanvas *gui;
    ofxUIToggle toggleAry[SIZE_OF_COMPRESSED_DATA*8];
    unsigned char rawIconData[SIZE_OF_COMPRESSED_DATA];
    unsigned char dividedStepsValue[4];
    
    unsigned char mySex;
    unsigned int  mySteps;
    unsigned int  colR;
    unsigned int  colG;
    unsigned int  colB;
    
    // ofxXmlSettings   xml;
    
    ofSerial        serial;
    vector<string>  strChannels;
    int portNumber;
};
