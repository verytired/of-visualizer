#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"

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
    
    //todo move to AudioManager
    Boolean bDrawAudio;
    
    void audioReceived(float *input, int bufferSize, int nChannels);
    void audioRequested(float *output, int bufferSize, int nChannels);
    
    int initialBufferSize; /* buffer size */
    int sampleRate;
    
    ofxMaxiOsc myOsc1;
    ofxMaxiSample sample1;
    ofxMaxiMix channel1;
    
    float *lAudioOut;    /* outputs */
    float *rAudioOut;
    
    float *lAudioIn;  /* inputs */
    float *rAudioIn;
    
    /* stick you maximilian stuff below */
    double wave, sample, outputs[2], ifftVal;
    maxiMix mymix;
    maxiOsc osc;
    
    ofxMaxiFFTOctaveAnalyzer oct;
    int nAverages;
    float *ifftOutput;
    int ifftSize;
    
    ofxMaxiIFFT ifft;
    ofxMaxiFFT mfft;
    int fftSize;
    int bins, dataSize;
    
    float callTime;
    timeval callTS, callEndTS;
		
};
