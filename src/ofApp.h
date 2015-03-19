#pragma once

#include "ofMain.h"
#include "ofxPostGlitch.h"
#include "ofxUI.h"
#include "ofxBeatTracking.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    //todo video loading
    vector<ofVideoPlayer> videos;
    int currentVideoNum = 0;
    
    ofVideoGrabber vidGrabber;
    
    ifstream fin;
    string line;
    vector<string> lines, videoFilePath;
    float X,Y;
    
    bool bWebCam;
    //glitch
    ofFbo myFbo;
    ofxPostGlitch myGlitch;

    int filterNum = 10;
    vector<bool> bFilter;
    bool bFilter0,bFilter1,bFilter2,bFilter3,bFilter4,bFilter5,bFilter6,bFilter7,bFilter8,bFilter9;
    
    //gui
    ofxUISuperCanvas *gui;
    void setGui();
    void guiEvent(ofxUIEventArgs &e);
    
    //audio
    ofTrueTypeFont  fbook;
    void audioReceived(float* input, int bufferSize, int nChannels);
    ofxBeatTracking bd;
    bool bDrawAudio;

    //changeView
    void changeView();
    bool dice();
};

