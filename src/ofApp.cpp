#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
#ifdef TARGET_OSX
    //ofSetDataPathRoot("../data/");
#endif
    
    ofSetVerticalSync(true);
    cam.initGrabber(1280, 720);
    
    tracker.setup();
    tracker.setRescale(.5);
    
    classifier.load("expressions");
    
    cloneReady = false;
    clone.setup(cam.getWidth(), cam.getHeight());
    ofFbo::Settings settings;
    settings.width = cam.getWidth();
    settings.height = cam.getHeight();
    maskFbo.allocate(settings);
    srcFbo.allocate(settings);
    camTracker.setup();
    srcTracker.setup();
    srcTracker.setIterations(25);
    srcTracker.setAttempts(4);
    
    faces.allowExt("jpg");
    faces.allowExt("png");
    faces.listDir("faces");
    currentFace = 0;
    if(faces.size()!=0){
        loadFace(faces.getPath(currentFace));
    }

}

void ofApp::update() {
    cam.update();
    if(cam.isFrameNew()) {
        if(tracker.update(toCv(cam))) {
            classifier.classify(tracker);
        }
        
        camTracker.update(toCv(cam));
        
        cloneReady = camTracker.getFound();
        if(cloneReady) {
            ofMesh camMesh = camTracker.getImageMesh();
            camMesh.clearTexCoords();
            camMesh.addTexCoords(srcPoints);
            
            maskFbo.begin();
            ofClear(0, 255);
            camMesh.draw();
            maskFbo.end();
            
            srcFbo.begin();
            ofClear(0, 255);
            src.bind();
            camMesh.draw();
            src.unbind();
            srcFbo.end();
            
            clone.setStrength(16);
            clone.update(srcFbo.getTextureReference(), cam.getTextureReference(), maskFbo.getTextureReference());
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);
    
    if (classifier.getPrimaryExpression() == 1 && src.getWidth() > 0 && cloneReady) {
        clone.draw(0, 0);
    }
    else {
        cam.draw(0, 0);
        tracker.draw();
        
        int w = 100, h = 12;
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(5, 10);
        int n = classifier.size();
        int primary = classifier.getPrimaryExpression();
        for(int i = 0; i < n; i++){
            ofSetColor(i == primary ? ofColor::red : ofColor::black);
            ofDrawRectangle(0, 0, w * classifier.getProbability(i) + .5, h);
            ofSetColor(255);
            ofDrawBitmapString(classifier.getDescription(i), 5, 9);
            ofTranslate(0, h + 5);
        }
        ofPopMatrix();
        ofPopStyle();
    }
}

void ofApp::loadFace(string face){
    src.loadImage(face);
    if(src.getWidth() > 0) {
        srcTracker.update(toCv(src));
        srcPoints = srcTracker.getImagePoints();
    }
}
