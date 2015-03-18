#include "ofApp.h"
#include "sys/time.h"
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(222, 222, 222);
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
            fingerMovie.setVolume(0.0);
            videos.push_back(fingerMovie);
            
        }
    }
    fin.close();
    
    //webCam
    bWebCam = false;
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(640,480);
    
    //glitch setup
    myFbo.allocate(640,480);
    myGlitch.setup(&myFbo);
    //filter flag
    for(int i= 0;i<filterNum;i++){
        bFilter.push_back(false);
    }
    
    //audio test;
    //todo move to AudioManager
    bDrawAudio = false;
    
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

    //gui
    setGui();
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
    for (int i=0;i<4;i++){
        videos[i].update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //video
    for (int i=0;i<4;i++){
        ofSetColor(255);
        videos[i].draw(222+170*i,30,160,120);
    }

    
    ofSetColor(255, 255, 255, 255);
    // webCam
    if(bWebCam){
        vidGrabber.draw(222+170*3,30,160,120);
    }
    
  
    myFbo.begin();
    //movie change
    ofSetColor(255);
    if(currentVideoNum<=3){
        videos[currentVideoNum].draw(0,0,640,480);
    }else if(currentVideoNum==4){
        vidGrabber.draw(0,0,640,480);
    }
    
    myFbo.end();
    
    myGlitch.generateFx();
    myFbo.draw(222,215);
    
    //label
    ofSetHexColor(0x000000);
    ofDrawBitmapString("1:Video1", 220, 20);
    ofDrawBitmapString("2:Video2", 220+170, 20);
    ofDrawBitmapString("3:Video3", 220+170*2, 20);
    ofDrawBitmapString("4:Video4 / WebCam", 220+170*3, 20);
    ofDrawBitmapString("Rendered", 220, 200);
    
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
            if(bWebCam == true){
                currentVideoNum = 4;
            }else{
                currentVideoNum = 3;
            }
            
            break;
        case 'h':
            gui->toggleVisible();
            break;
        case 'w':
            bWebCam = !bWebCam;
            if(currentVideoNum == 3)currentVideoNum = 4;
            else if(currentVideoNum == 4)currentVideoNum = 3;
            break;
    }
    
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
        sample = sample1.play(0.);
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

//----
void ofApp::setGui(){
    gui = new ofxUISuperCanvas("POSTGLITCH");
    gui->addSpacer();
    gui->addLabel("Press 'h' to Hide GUIs", OFX_UI_FONT_SMALL);
    gui->addLabelToggle("CONVERGENCE", &bFilter0)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("GLOW", &bFilter1)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("SHAKER", &bFilter2)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("CUTSLIDER", &bFilter3)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("TWIST", &bFilter4)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("OUTLINE", &bFilter5)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("NOISE", &bFilter6)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("SLITSCAN", &bFilter7)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("SWELL", &bFilter8)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addLabelToggle("INVERT", &bFilter9)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));

    gui->setPosition(0, 0);
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{

    string name = e.getName();
    int kind = e.getKind();
    cout << "got event from: " << name << endl;
    cout << bFilter0 << endl;
        cout << bFilter[1] << endl;
    if(name == "CONVERGENCE"){
    }else if(name == "CONVERGENCE"){
        myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, bFilter0);
    }else if(name == "GLOW"){
        myGlitch.setFx(OFXPOSTGLITCH_GLOW			, bFilter1);
    }else if(name == "SHAKER"){
        myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, bFilter2);
    }else if(name == "CUTSLIDER"){
        myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, bFilter3);
    }else if(name == "TWIST"){
        myGlitch.setFx(OFXPOSTGLITCH_TWIST			, bFilter4);
    }else if(name == "OUTLINE"){
        myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, bFilter5);
    }else if(name == "NOISE"){
        myGlitch.setFx(OFXPOSTGLITCH_NOISE			, bFilter6);
    }else if(name == "SLITSCAN"){
        myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, bFilter7);
    }else if(name == "SWELL"){
         myGlitch.setFx(OFXPOSTGLITCH_SWELL			, bFilter8);
    }else if(name == "INVERT"){
        myGlitch.setFx(OFXPOSTGLITCH_INVERT			, bFilter9);
    }
}
