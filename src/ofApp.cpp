#include "ofApp.h"
#include "sys/time.h"

int testApp_buffer_size = 1024;
int testApp_fft_size = 512;


bool detect=true;
bool drumVisible = true;
bool snareVisible = true;
bool hihatVisible = true;
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(222, 222, 222);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
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
    
    //gui
    setGui();
    
    //audio
    ofSoundStreamSetup(0, 1, this, 44100, testApp_buffer_size, 4);
    fbook.loadFont("frabk.ttf", 32, true, false);
    bDrawAudio = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
    for (int i=0;i<4;i++){
        videos[i].update();
    }
    bd.updateFFT();
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
    
     if(bDrawAudio){
         char str[32];
         
         // 取得したFFTの値は bd.magnitude[i] のようにすればアクセス可能
         // 以下では取得したFFTをbd.magnitude[i]の形式でアクセスしてFFTを描画する
         // 生のFFTの値を描画 (1列目)
         ofSetColor(155,155,75);
         for (int i = 1; i < (int)testApp_fft_size/2; i++){
             if(i % 16 == 0) {
                 ofSetColor(200,0,0);
             } else {
                 ofSetColor(155,155,75);
             }
             ofLine(10+(i*3),150,  10+(i*3),150-bd.magnitude[i]*10.0f);
             //printf("%f \n", magnitude_average[i]);
         }
         // 画面に入りきらないので2列目
         for (int i = (int)testApp_fft_size/2,b = 1; i<testApp_fft_size ; i++){
             if(i % 16 == 0) {
                 ofSetColor(200,0,0);
             } else {
                 ofSetColor(155,155,75);
             }
             ofLine(10+(b*3),300,  10+(b*3),300-bd.magnitude[i]*10.0f);
             b++;
             //printf("%f \n", magnitude_average[i]);
         }
         
         
         // 平均化されたFFTの値を描画 (1列目)
         ofSetColor(134,113,89);
         for (int i = 1; i < (int)testApp_fft_size/2; i++){
             if(i % 16 == 0) {
                 ofSetColor(200,0,0);
             } else {
                 ofSetColor(134,113,89);
             }
             ofLine(10+(i*3),500,  10+(i*3),500-bd.magnitude_average[i]*10.0f);
         }
         // 画面に入り切らないので2列
         for (int i = (int)testApp_fft_size/2,b = 1; i<testApp_fft_size ; i++){
             if(i % 16 == 0) {
                 ofSetColor(200,0,0);
             } else {
                 ofSetColor(134,113,89);
             }
             ofLine(10+(b*3),650,  10+(b*3),650-bd.magnitude_average[i]*10.0f);
             b++;
         }
         
         // ドラム音の検出
         if(drumVisible){
             if(bd.isBeatRange(0,2,2)){
                 ofSetColor(255,0,0);
                 fbook.drawString("DRUM!!",100,735);
                 changeView();
             }else{
                 ofSetColor(0,255,0);
                 ofRect(100,700,200,50);
             }
         }
         
         // スネア音の検出
         if(snareVisible){
             if(bd.isBeatRange(12,18,4)){
                 ofSetColor(255,0,0);
                 fbook.drawString("SNARE!!", 350, 735);
             }else{
                 ofSetColor(0,255,0);
                 ofRect(350,700,200,50);
             }
         }
         
         // ハイハット音の検出
         if(hihatVisible){
             if(bd.isBeatRange(27,31,3)){
                 ofSetColor(255,0,0);
                 fbook.drawString("HiHat!!", 600, 735);
             }else{
                 ofSetColor(0,255,0);
                 ofRect(600,700,200,50);
             }
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
void ofApp::audioReceived(float *input, int bufferSize, int nChannels) {
    bd.audioReceived(input, bufferSize);
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

void ofApp::changeView(){
    currentVideoNum++;
    if(currentVideoNum>3){
        currentVideoNum=0;
    }
    bFilter0 = dice();
    bFilter1 = dice();
    bFilter2 = dice();
    bFilter3 = dice();
    bFilter4 = dice();
    bFilter5 = dice();
    bFilter6 = dice();
    bFilter7 = dice();
    bFilter8 = dice();
    bFilter9 = dice();
    myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, bFilter0);
    myGlitch.setFx(OFXPOSTGLITCH_GLOW			, bFilter1);
    myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, bFilter2);
    myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, bFilter3);
    myGlitch.setFx(OFXPOSTGLITCH_TWIST			, bFilter4);
    myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, bFilter5);
    myGlitch.setFx(OFXPOSTGLITCH_NOISE			, bFilter6);
    myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, bFilter7);
    myGlitch.setFx(OFXPOSTGLITCH_SWELL			, bFilter8);
    myGlitch.setFx(OFXPOSTGLITCH_INVERT			, bFilter9);
}

bool ofApp::dice(){
    bool flag = false;
    float r = ofRandom(0,100);
    if(r>50)flag = true;
    return flag;
}