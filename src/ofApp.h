#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTrackerThreaded.h"

using namespace ofxCv;

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void loadFace(string face);
    
    ofVideoGrabber cam;
    ofxFaceTracker tracker;
    ExpressionClassifier classifier;
    
    ofxFaceTrackerThreaded camTracker;
    
    ofxFaceTracker srcTracker;
    ofImage src;
    vector<ofVec2f> srcPoints;
    
    bool cloneReady;
    Clone clone;
    ofFbo srcFbo, maskFbo;
    
    ofDirectory faces;
    int currentFace;
};
