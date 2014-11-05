#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    
    mySteps = (unsigned int)ofRandom(5000, 100000);
    colR    = (unsigned int)ofRandom(255);
    colG    = (unsigned int)ofRandom(255);
    colB    = (unsigned int)ofRandom(255);
    
    guiSetup();
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::guiSetup(){
    if (gui) {
        gui->saveSettings("settings.xml");
        delete gui;
    }
    gui = new ofxUISuperCanvas("GUI");
    
    gui->addSpacer();
    gui->setGlobalButtonDimension(24);
    gui->addToggleMatrix("UserIcon", 16, 16);
    gui->addSpacer();
    gui->addToggle("SEX(M:0/F:1)", false);
    gui->addSpacer();
    gui->addRadio("Serial Channels", strChannels, OFX_UI_ORIENTATION_VERTICAL);
    gui->addSpacer();
    gui->addButton("CONNECT", false);
    gui->addSpacer();
    gui->addButton("TRANSFER", false);
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    
    gui->loadSettings("settings.xml");
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    
    if (name == "SEX(M:0/F:1)") {
        cout << "[GUI] sex value has been changed" << endl;
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        if (toggle->getValue()) {
            mySex = 'f';
        } else {
            mySex = 'm';
        }
        cout << "mySex : " << mySex << endl;
    }
    else if (ofIsStringInString(name, "UserIcon")) {
        cout << "[GUI] icon pattern value has been changed" << endl;
        ofxUIToggleMatrix *matrix = (ofxUIToggleMatrix *) e.getParent();
        // toggles = matrix->getToggles();
        for (int i = 0; i < matrix->getToggles().size(); i++) {
             toggleAry[i] = *matrix->getToggles().at(i);
        }
    }
    else if (name == "Serial Channels") {
        cout << "[GUI] serial channel value has been changed" << endl;
        ofxUIRadio *radio = (ofxUIRadio *)e.widget;
        cout << "serial channel : " << radio->getValue() << endl;
        portNumber = radio->getValue();
    }
    else if (name == "CONNECT") {
        ofxUIButton *button = (ofxUIButton *)e.getButton();
        cout << button->getValue() << endl;
        if (button->getValue()) {
            if (accessToShoe() == -1)
                cout << "cannot connect" << endl;
            else
                cout << "connected!" << endl;
        }
    }
    else if (name == "TRANSFER") {
        ofxUIButton *button = (ofxUIButton *) e.getButton();
        if (serial.isInitialized()) {
            setShoeData(&rawIconData[SIZE_OF_COMPRESSED_DATA], &dividedStepsValue[4], mySex,
                        (int)ofRandom(255), (int)ofRandom(255), (int)ofRandom(255));
        } else {
            cout << "serial port is not initialized" << endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::translateToBinary(ofxUIToggle *input[SIZE_OF_COMPRESSED_DATA*8], unsigned char *output[SIZE_OF_COMPRESSED_DATA])
{
    unsigned int pos = 0;
    unsigned char val = 0;
    
    for (int i = 0; i < SIZE_OF_COMPRESSED_DATA; i++) {     // 8bits = 1byte
        for (int j = 7; j >= 0; j--) {          // loops for each bit
            *output[pos] = 0;               // clean element
            
            if (input[pos]->getValue()) {  // if the position was turned on
                val = 1;
                for (int a = 0; a < j; a++) {   // calculate 2^x (= convert to decimal number)
                    val *= 2;
                }
                *output[i] += val;          // add the converted value to the bit
            }
            pos++;          // step up
            val = 0;
        }
       // cout << (int)rawIconData[i] << " ";
    }
   // cout << endl;
}

//--------------------------------------------------------------
void ofApp::translateToBinary(unsigned int input, unsigned char *output[4])
{
    *output[0] = input;
    *output[1] = input>>8;
    *output[2] = input>>16;
    *output[3] = input>>24;
}

//--------------------------------------------------------------
void ofApp::setShoeData(unsigned char myIconData[SIZE_OF_COMPRESSED_DATA], unsigned char mySteps[4], unsigned char mySex, char r, char g, char b)
{
    for (int i = 0; i < 3; i++) {
        serial.writeByte('!');
    }
    serial.writeByte(0x11);
    serial.writeBytes(myIconData, SIZE_OF_COMPRESSED_DATA);
    serial.writeBytes(mySteps, 4);
    serial.writeByte(mySex);
    serial.writeByte(r);
    serial.writeByte(g);
    serial.writeByte(b);
}

//--------------------------------------------------------------
void ofApp::refreshSerialChannels()
{
    if (strChannels.size())
        strChannels.clear();

    for (int i = 0; i < serial.getDeviceList().size(); i++) {
        strChannels.push_back(serial.getDeviceList().at(i).getDeviceName().c_str());
    }
    
    guiSetup();
}

//--------------------------------------------------------------
int ofApp::accessToShoe()
{
    if (serial.isInitialized()) {
        serial.close();
    }
    serial.setup(strChannels[portNumber], BAUDRATE);
    if (!serial.isInitialized()) {
        return -1;      // failed
    } else {
        return 0;       // succeeded
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::black);
    ofDrawBitmapString("press 's' key to save pattern data\n      't' key to convert matrix data into binary data\n      'r' key to refresh serial channels\n      'b' key to preview sending data in console", 400, 200);
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui->saveSettings("settings.xml");
    delete gui;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key ==  's') {
            gui->saveSettings("settings.xml");
            cout << "[CONTROL] setting have been saved to setting.xml" << endl;
    } else if (key == 't') {
            translateToBinary(*toggleAry, &rawIconData);
            translateToBinary(mySteps, &dividedStepsValue);
            cout << "[CONTROL] matrix data translated into binary data" << endl;
    } else if (key == 'r') {
            refreshSerialChannels();
            cout << "[CONTROL] serial channel value has been refreshed and gui redrawed" << endl;
    } else if (key == 'b') {
        cout << "[CHECK RAW DATA] " << "!!!<NEXT>" << 0x11 << "<NEXT>";
        for (int i = 0; i < SIZE_OF_COMPRESSED_DATA*8; i++) {
            if (toggleAry[i]->getValue()) {
                cout << "1";
            } else {
                cout << "0";
            }
        }
        cout << "<NEXT>" << mySteps << "<NEXT>" << mySex
             << "<NEXT>" << colR    << "<NEXT>" << colG   << "<NEXT>" << colB << endl;
        
        cout << "[CHECK CONVERTED DATA] " << "!!!<NEXT>" << 0x11 << "<NEXT>";
        for (int i = 0; i < SIZE_OF_COMPRESSED_DATA; i++) {
            cout << *rawIconData[i];
        }
        cout << "<NEXT>";
        for (int j = 0; j < 4; j++) {
            cout << *dividedStepsValue[j];
        }
        cout << "<NEXT>";
        cout << mySex << "<NEXT>" << (unsigned int)ofRandom(255) // col R
                      << "<NEXT>" << (unsigned int)ofRandom(255) // col G
                      << "<NEXT>" << (unsigned int)ofRandom(255) // col B
                      << endl;
    }
    
    for (int i = 0; i < 3; i++) {
        serial.writeByte('!');
    }
    serial.writeByte(0x11);
    serial.writeBytes(*rawIconData, SIZE_OF_COMPRESSED_DATA);
    serial.writeBytes(*dividedStepsValue, 4);
    serial.writeByte(mySex);
    serial.writeByte(colR);
    serial.writeByte(colG);
    serial.writeByte(colB);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
