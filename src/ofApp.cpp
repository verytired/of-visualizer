#include "ofApp.h"
#include "sys/time.h"
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    
    //video file loading
    fin.open(ofToDataPath("filelist.txt", true).c_str());
    if(fin.fail()) {
        cerr << "File do not exist.\n";
    }
    while(getline(fin, line)){
        lines=(ofSplitString(line, "\t"));
        for (int i=0; i < lines.size(); i++){
            videoFilePath.push_back(lines[i]);
            
            ofVideoPlayer fingerMovie;
            fingerMovie.loadMovie(lines[i]);
            fingerMovie.play();
            videos.push_back(fingerMovie);
            
        }
    }
    fin.close();
    
    //webCam
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);
    
    //glitch setup
    myFbo.allocate(320,240);
    myGlitch.setup(&myFbo);
    
    //audio test;
    //todo move to AudioManager
    bDrawAudio = false;
    
    // initialize the accelerometer
    //ofxAccelerometer.setup();
    
    //iPhoneAlerts will be sent to this.
//    ofxiPhoneAlerts.addListener(this);
    
    /* This is stuff you always need.*/
    sampleRate          = 44100; /* Sampling Rate */
    initialBufferSize   = 512;  /* Buffer Size. you have to fill this buffer with sound*/
    lAudioOut           = new float[initialBufferSize];/* outputs */
    rAudioOut           = new float[initialBufferSize];
    lAudioIn            = new float[initialBufferSize];/* inputs */
    rAudioIn            = new float[initialBufferSize];
    
    /* This is a nice safe piece of code */
    memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
    /* Now you can put anything you would normally put in maximilian's 'setup' method in here. */
    fftSize = 1024;
    mfft.setup(fftSize, 1024, 256);
    ifft.setup(fftSize, 1024, 256);
    
    nAverages = 12;
    oct.setup(sampleRate, fftSize / 2, nAverages);
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    
    ofSoundStreamSetup(2, 0, this, sampleRate, initialBufferSize, 4);/* Call this last ! */
    
    ofSetVerticalSync(true);
    
    sample1.load(ofToDataPath("beat2.wav"));

}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
    for (int i=0;i<3;i++){
        videos[i].update();
    }
    
    //glitch
    myGlitch.setFx(OFXPOSTGLITCH_NOISE,true);

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //video
    for (int i=0;i<3;i++){
            ofSetColor(255);
        videos[i].draw(5+330*i,5,320,240);
    }
    
    // webCam
 
    vidGrabber.draw(5,255,320,240);
    
    myFbo.begin();
    //movie change
    ofSetColor(255);
    if(currentVideoNum<=3){
    videos[currentVideoNum].draw(0,0,320,240);
    }else if(currentVideoNum==4){
        vidGrabber.draw(0,0,320,240);
    }
    
    myFbo.end();
    
    myGlitch.generateFx();
    myFbo.draw(335,255);
    
    //audio
    if(bDrawAudio){
        ofSetColor(255, 255, 255, 255);
        
        //draw fft output
        float xinc = 900.0 / fftSize * 2.0;
        for (int i = 0; i < fftSize / 2; i++) {
            float height = mfft.magnitudesDB[i] / 50.0 * 200;
            ofRect(0 + (i * xinc), 250 - height, 2, height);
        }
        //draw phases
        ofSetColor(0, 255, 0, 100);
        for (int i = 0; i < fftSize / 2; i++) {
            float height = mfft.phases[i] / 50.0 * 200;
            ofRect(0 + (i * xinc), 360 - height, 2, height);
        }
        
        //octave analyser
        ofSetColor(255, 0, 0, 100);
        xinc = 900.0 / oct.nAverages;
        for (int i = 0; i < oct.nAverages; i++) {
            float height = oct.averages[i] / 50.0 * 100;
            ofRect(0 + (i * xinc), 500 - height, 2, height);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key){
        case ' ':
            bDrawAudio = !bDrawAudio;
            break;
        case '1':
            currentVideoNum = 0;
            break;
        case '2':
            currentVideoNum = 1;
            break;
        case '3':
            currentVideoNum = 2;
            break;  
        case '4':
            currentVideoNum = 4;
            break;
    }
    
    //todo change efx
    /*
     if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
     if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, true);
     if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
     if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
     if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, true);
     if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
     if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, true);
     if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
     if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, true);
     if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, true);
     
     if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
     if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, true);
     if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, true);
     if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, true);
     if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, true);
     if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, true);
     if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, true);
     
     if (key == 'l') bDrawLenna ^= true;
     if (key == 'h') bShowHelp ^= true;

     
     */
    if (key == 's'){
        // ofImage定義
        ofImage myImage;
        
        //キャプチャの描画
        myImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        
        //書き出し
        myImage.saveImage("./file_name.png", OF_IMAGE_QUALITY_BEST);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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

//--------------------------------------------------------------
void ofApp::audioRequested(float *output, int bufferSize, int nChannels) {
    if (initialBufferSize != bufferSize) {
        ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", initialBufferSize, bufferSize);
        return;
    }
        
    for (int i = 0; i < bufferSize; i++) {
    }
    
    //	static double tm;
    for (int i = 0; i < bufferSize; i++) {
        sample = sample1.play(1.);
        channel1.stereo(sample, outputs, 0.5);
        
        //√ä‚â•¬¢√ÇŒ©¬¢‚Äû√Ö√ò√Å√Æ√º√ä√†√™‚Äû√Ö√≥‚Äû√Ö‚Ñ¢‚Äû√Ö√ë
        //wave = osc.saw(maxiMap::linexp(mouseY + ofGetWindowPositionY(), 0, ofGetScreenHeight(), 200, 8000));

        //get fft
        //if (mfft.process(wave)) {
        if (mfft.process(sample)) {
            int bins = fftSize / 2.0;
            //do some manipulation
//            int hpCutoff = floor(((mouseX + ofGetWindowPositionX()) / (float)ofGetScreenWidth()) * fftSize / 2.0);
            int hpCutoff = 1;
            //highpass
            memset(mfft.magnitudes, 0, sizeof(float) * hpCutoff);
            memset(mfft.phases, 0, sizeof(float) * hpCutoff);
            //lowpass
            //			memset(mfft.magnitudes + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));
            //			memset(mfft.phases + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
//            cout << mfft.spectralFlatness() << ", " << mfft.spectralCentroid() << endl;
        }
        //inverse fft
        gettimeofday(&callTS, NULL);
        ifftVal = ifft.process(mfft.magnitudes, mfft.phases);
        gettimeofday(&callEndTS, NULL);
        callTime = (float)(callEndTS.tv_usec - callTS.tv_usec) / 1000000.0;
        callTime += (float)(callEndTS.tv_sec - callTS.tv_sec);
        //play result
        mymix.stereo(ifftVal, outputs, 0.5);
        //		float mix = ((mouseX + ofGetWindowPositionX()) / (float) ofGetScreenWidth());
        //		mymix.stereo((wave * mix) + ((1.0-mix) * ifftVal), outputs, 0.5);
        
        lAudioOut[i] = output[i * nChannels] = outputs[0];   /* You may end up with lots of outputs. add them here */
        rAudioOut[i] = output[i * nChannels + 1] = outputs[1];
    }
}

//--------------------------------------------------------------
void ofApp::audioReceived(float *input, int bufferSize, int nChannels) {
}
